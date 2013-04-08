/// Calculate the mass in a volume.

#include <memory>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <map>

#include <TFile.h>
#include <TGeoManager.h>
#include <TRandom.h>
#include <TRandom3.h>

#include <TND280Log.hxx>
#include <TManager.hxx>
#include <TGeomIdManager.hxx>
#include <TSHAHashValue.hxx>
#include <HEPUnits.hxx>

void usage(int argc, char **argv) {
    std::cerr << std::endl
              << argv[0] << " [options] <intput-file-name>" 
              << std::endl
              << "   -n <points>    The total number of test points"
              << std::endl
              << "   -r             Set the seed. (def: use the time)"
              << std::endl
              << "   -c             Print the component mixtures by isotope"
              << std::endl
              << "   Validation of the mass"
              << std::endl
              << "     -m <mass>    The ideal mass for the volume"
              << std::endl
              << "     -s <unc>     Allowed absolute diff. from ideal (def: 0)"
              << std::endl
              << "     -S <sigma>   The allowed statistical difference (def: 3)"
              << std::endl
              << "     -C <c>       Correction for known mass differences."
              << std::endl
              << "   Center of the volume"
              << std::endl
              << "     -x <coord>   X position in mm"
              << std::endl
              << "     -y <coord>   X position in mm"
              << std::endl
              << "     -z <coord>   X position in mm"
              << std::endl
              << "   Size of the volume"
              << std::endl
              << "     -L <dim>     Length in mm"
              << std::endl
              << "     -W <dim>     Length in mm"
              << std::endl
              << "     -H <dim>     Length in mm"
              << std::endl
              << "   Special Volumes:"
              << std::endl
              << "     -p           P0D Fiducial Mass"
              << std::endl
              << std::endl
              << "   Calculate the mass in a volume for the geometry in the"
              << std::endl
              << "   input file."
              << std::endl;
}

int main(int argc, char** argv) {
    std::string outputName;
    
    int trials = 1000000;
    // The random seed.  If this is left at zero, the time is used.
    int seed = 0;
    // If this is true, then print each of the materials.
    bool printMaterials = false;
    // The ideal mass for the volume.  If this is positive, then the
    // calculated mass will be compared to the ideal mass and an error will be
    // generated if there is a mismatch.
    double idealMass = -1.0;
    // The absolute uncertainty on the ideal mass.
    double massUncertainty = 0.0;
    // The allowable difference between the mass and the ideal mass in
    // standard deviations.
    double massSigma = 3.0;
    // A correction to be applied to the mass before comparing to the ideal
    // mass.  This accounts for known differences in the MC model.
    double massCorrection = 0.0;
    // The center of the mass volume to check.
    double xCenter = 0.0;
    double yCenter = 0.0;
    double zCenter = 0.0;
    // the size of the mass volume to check.
    double length = 0.0;
    double width = 0.0;
    double height = 0.0;

    for (;;) {
        int c = getopt(argc, argv, "cn:m:s:S:C:x:y:z:L:W:H:pr:h");
        switch (c) {
        case 'c':
            printMaterials = true;
            break;
        case 'n':
            std::istringstream(optarg) >> trials;
            break;
        case 'm':
            std::istringstream(optarg) >> idealMass;
            idealMass *= unit::kg;
            break;
        case 's':
            std::istringstream(optarg) >> massUncertainty;
            massUncertainty *= unit::kg;
            break;
        case 'S':
            std::istringstream(optarg) >> massSigma;
            break;
        case 'C':
            std::istringstream(optarg) >> massCorrection;
            massCorrection *= unit::kg;
            break;
        case 'y':
            std::istringstream(optarg) >> yCenter;
            break;
        case 'z':
            std::istringstream(optarg) >> zCenter;
            break;
        case 'L':
            std::istringstream(optarg) >> length;
            break;
        case 'W':
            std::istringstream(optarg) >> width;
            break;
        case 'H':
            std::istringstream(optarg) >> height;
            break;
        case 'p':
            xCenter = -36*unit::mm;
            yCenter = -1*unit::mm;
            zCenter = -2116*unit::mm;
            length = 1705*unit::mm;
            width = 1600*unit::mm;
            height = 1740*unit::mm;
            break;
        case 'r':
            std::istringstream(optarg) >> seed;
            break;
        case 'h':
            usage(argc,argv);
            return 0;
        }
        if (c<0) break;
    }

    if (length < 1*unit::mm 
        || width < 1*unit::mm 
        || height < 1*unit::mm) {
        usage(argc,argv);
        std::cerr << "ERROR: Invalid Volume ("
                  << length << " mm " 
                  << " x " << width << " mm "
                  << " x " << height << " mm)"
                  << std::endl;
        return 1;
    }
        
    if (trials<1) {
        usage(argc,argv);
        std::cerr << "ERROR: Insufficient trials (" << trials 
                  << std::endl;
        return 1;
    }

    std::cout << "Calculate mass of volume:"
              << std::endl;
    std::cout << "   Center: (" << xCenter << " mm" 
              << ", " << yCenter << " mm"
              << ", " << zCenter << " mm)"
              << std::endl;
    std::cout << "   Size:   ("
              << length << " mm" 
              << " x " << width << " mm"
              << " x " << height << " mm)"
              << " --> ( +-" << length/2 << " mm" 
              << ", +-" << width/2 << " mm"
              << ", +-" << height/2 << " mm)"
              << std::endl;
    
    std::string inputName;
    if (optind<argc) {
        inputName = argv[optind++];
    }
    else {
        usage(argc,argv);
        std::cerr << "ERROR: Missing input file" << std::endl;
        return 1;
    }

    TFile* inputPtr(TFile::Open(inputName.c_str(),"OLD"));
    if (!inputPtr) {
        usage(argc,argv);
        std::cerr << "ERROR: file not opened"
                  << std::endl;
        return 1;
    }

    std::auto_ptr<TFile> inputFile(inputPtr);
    if (!CP::TManager::Get().GeomId().LoadGeometry(*inputFile,
                                                      CP::TSHAHashValue())) {
        usage(argc,argv);
        std::cerr << "ERROR: Geometry not found in input file." << std::endl;
        return 1;
    }
    inputFile->Close();

    // Set up the random number generator.
    gRandom = new TRandom3(seed);

    double density = 0;
    double density2 = 0;
    
    // Find the average density
    int trialStep = trials/50;
    std::map<std::string, double> massMap;
    std::map<std::string, int> massCount;
    std::map<std::string, const TGeoMaterial*> materials;
    for (int trial = 0; trial<trials; ++trial) {
        if ((trial%trialStep) == 0) std::cerr << ".";
        double x = gRandom->Uniform(xCenter-width/2,xCenter+width/2);
        double y = gRandom->Uniform(yCenter-height/2,yCenter+height/2);
        double z = gRandom->Uniform(zCenter-length/2,zCenter+length/2);
        TGeoNode *node = gGeoManager->FindNode(x,y,z);
        if (!node) {
            std::cerr << std::endl
                      << "ERROR: Invalid point "
                      << " (" << x << " mm" 
                      << ", " << y << " mm"
                      << ", " << z << " mm)"
                      << std::endl;
            return 1;
        }
        double m = node->GetMedium()->GetMaterial()->GetDensity();
        density += m;
        density2 += m*m;
        massMap[node->GetMedium()->GetMaterial()->GetName()] += m;
        massCount[node->GetMedium()->GetMaterial()->GetName()] += 1;
        materials[node->GetMedium()->GetMaterial()->GetName()] 
            = node->GetMedium()->GetMaterial();
    }
    std::cerr << std::endl;

    density = density/trials;
    density2 = density2/trials;
    density2 = std::sqrt(density2 - density*density);

    double volume = length*width*height;

    if (printMaterials) {
        for (std::map<std::string,const TGeoMaterial*>::iterator m
                 = materials.begin();
             m != materials.end(); ++m) {
            m->second->Print();
        }
    }

    double massVar = 0.0;
    std::cout << "Mass of each material:" << std::endl;
    for (std::map<std::string,double>::iterator e = massMap.begin();
         e != massMap.end(); ++e) {
        double componentMass = volume*e->second/trials;
        double componentSigma = componentMass/std::sqrt(massCount[e->first]);
        std::cout << std::setw(25) << e->first 
                  << "   " << componentMass/unit::kg
                  << " kg"
                  << " +- " << componentSigma/unit::kg
                  << "     (" << massCount[e->first] << " entries)"
                  << std::endl;
        massVar += componentSigma*componentSigma;
    }
    massVar = std::sqrt(massVar);

    double mass = length*width*height*density;
    std::cout << "Total Mass:     " << mass/unit::kg << " kg"
              << " +- " << massVar/unit::kg
              << " kg"
              << std::endl;

    if (idealMass>0) {
        // correct the calculated mass.
        mass = mass + massCorrection;

        double range = massVar*massSigma + massUncertainty;
        double delta = std::abs(mass-idealMass);

        if (std::abs(massCorrection) > 0.01) {
            std::cout << "Corrected Mass: "  
                      << mass/unit::kg << " kg"
                      << " +- " << massVar/unit::kg << " kg"
                      << std::endl;
        }

        std::cout << "Ideal Mass:     "  
                  << idealMass/unit::kg << " kg"
                  << " +- " << massUncertainty/unit::kg << " kg"
                  << "       (Difference: " << (mass-idealMass)/unit::kg
                  << " kg)"
                  << std::endl;
        std::cout << "     Allowed range is +- " << range/unit::kg
                  << "    (from " << (idealMass-range)/unit::kg 
                  << " to " <<  (idealMass+range)/unit::kg << " kg)"
                  << std::endl;
        if (delta > range) {
            std::cerr << "ERROR:  "
                      << "Expected " << idealMass/unit::kg << " kg"
                      << " with "
                      << (idealMass - range)/unit::kg
                      << " kg < " << mass/unit::kg
                      << " kg < " << (idealMass + range)/unit::kg
                      << " kg"
                      << std::endl;
            std::cout << "FAIL" << std::endl;
            return 1;
        }
    }
    
    return 0;
}
