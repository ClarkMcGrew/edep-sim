#include "kinem/EDepSimHEPEVTKinematicsGenerator.hh"
#include "EDepSimException.hh"
#include "EDepSimVertexInfo.hh"

#include <G4Event.hh>
#include <G4SystemOfUnits.hh>
#include <G4LorentzVector.hh>
#include <G4PrimaryVertex.hh>
#include <G4HEPEvtParticle.hh>

#ifdef USE_G4HEPEvtInterface
#include <G4HEPEvtInterface.hh>
#endif

#include <memory>

EDepSim::HEPEVTKinematicsGenerator::HEPEVTKinematicsGenerator(
    const G4String& name, const G4String& fileName,
    const G4String& flavor, int verbosity)
    : EDepSim::VKinematicsGenerator(name), fGenerator(NULL),
      fFileName(fileName), fFlavor(flavor),
      fVerbosity(verbosity) {}

EDepSim::HEPEVTKinematicsGenerator::~HEPEVTKinematicsGenerator() {
#ifdef USE_G4HEPEvtInterface
    if (fGenerator) delete fGenerator;
#endif
}

int EDepSim::HEPEVTKinematicsGenerator::GetTokens(
    std::vector<std::string>& tokens) {

    tokens.clear();
    if (!fInput.is_open() || fInput.eof()) {
        EDepSimError("No more events: " << fFileName);
        return 0;
    }

    std::string line;
    while (std::getline(fInput,line)) {
        ++fCurrentLine;
        // Strip out comments.
        line = line.substr(0,line.find_first_of("#"));
        // Break into tokens.
        std::istringstream input(line);
        std::string token;
        tokens.clear();
        while (input >> token) tokens.push_back(token);
        if (!tokens.empty()) return tokens.size();
    }

    EDepSimLog("No more data in " << fFileName);
    return 0;
}

int EDepSim::HEPEVTKinematicsGenerator::AsInteger(const std::string& token) {
    std::istringstream input(token);
    int value;
    input >> value;
    std::string trailer;
    if (input >> trailer) {
        EDepSimThrow(fFileName << ":" << fCurrentLine
                     << " -- Invalid integer: |" << token<< "|");
    }
    return value;
}

double EDepSim::HEPEVTKinematicsGenerator::AsReal(const std::string& token) {
    std::istringstream input(token);
    double value;
    input >> value;
    std::string trailer;
    if (input >> trailer) {
        EDepSimThrow(fFileName << ":" << fCurrentLine
                     << " -- Invalid real: |" << token << "|");
    }
    return value;
}

EDepSim::VKinematicsGenerator::GeneratorStatus
EDepSim::HEPEVTKinematicsGenerator::GeneratePrimaryVertex(
    G4Event* evt, const G4LorentzVector& /* position */) {
#ifdef USE_G4HEPEvtInterface
    if (!fGenerator) {
        fGenerator = new G4HEPEvtInterface(fFileName,fVerbosity);
    }
    try {
        fGenerator->GeneratePrimaryVertex(evt);
    }
    catch (...) {
        EDepSimError("No more events.");
        throw EDepSim::NoMoreEvents();
    }
#else
    if (!fInput.is_open()) {
        fInput.open(fFileName,std::ifstream::in);
        fCurrentLine = 0;
        if (!fInput.is_open()) {
            EDepSimThrow("File not open: " << fFileName);
        }
        EDepSimNamedLog("HEPEVT", "Open HEPEVT file: " << fFileName);
    }


    if (fVerbosity>0) {
        EDepSimNamedLog("HEPEVT", "Using HEPEVT input flavor: " << fFlavor);
    }

    std::vector<std::string> tokens;
    int event_id  = 0;
    int vertex_id = 0;
    int lines     = 0;
    double vtxX = -999;
    double vtxY = -999;
    double vtxZ = -999;
    double vtxT = -999;

    // Store particles in a list before adding to vertex. This makes
    // storing mother/daughter information possible.
    std::vector<G4HEPEvtParticle*> HPlist;

    // Parse the header line for an interaction. Depending on the input
    // flavor, this line may contain any number of the following: event ID,
    // vertex ID, number of particles, and vertex (x,y,z,t). The number
    // of particles is the only required field. Vertex positions should
    // be given in cm, and the time should be in ns.
    if (!GetTokens(tokens)) {
        EDepSimLog("No more events in " << fFileName
                   << " after " << fCurrentLine << " lines");
        throw EDepSim::NoMoreEvents();
    }
    switch (tokens.size()) {
    // For pythia flavor input, the number of particle in the event
    // is the only token.
    case 1:
        lines = AsInteger(tokens[0]);
        break;
    // The next two cases use the marley (15-number) input flavor
    // For events with only a single vertex, separate each event by
    // a line with two numbers: the eventID and number of particles
    // in that event
    case 2:
        event_id = AsInteger(tokens[0]);
        lines    = AsInteger(tokens[1]);
        break;
    // For multi-vertex events, separate each vertex with a line
    // containing three numbers: eventID, vertexID, and the number
    // of particles in that vertex
    case 3:
        event_id  = AsInteger(tokens[0]);
        vertex_id = AsInteger(tokens[1]);
        lines     = AsInteger(tokens[2]);
        evt->SetEventID(event_id);
        break;
    // Set vertex for pythia flavor
    case 5:
        vtxX = AsReal(tokens[1])*cm;
        vtxY = AsReal(tokens[2])*cm;
        vtxZ = AsReal(tokens[3])*cm;
        vtxT = AsReal(tokens[4])*ns;
        lines = AsInteger(tokens[0]);
        break;
    // Alternative header format for multi-vertex events:
    // event ID, vertex ID, number of particles in that vertex,
    // and vertex (x,y,z,t)
    case 7:
        event_id  = AsInteger(tokens[0]);
        vertex_id = AsInteger(tokens[1]);
        lines     = AsInteger(tokens[2]);
        evt->SetEventID(event_id);
        vtxX   = AsReal(tokens[3])*cm;
        vtxY   = AsReal(tokens[4])*cm;
        vtxZ   = AsReal(tokens[5])*cm;
        vtxT   = AsReal(tokens[6])*ns;
        break;
    default:
        EDepSimError("Syntax error at " << fFileName << ":" << fCurrentLine);
        throw EDepSim::NoMoreEvents();
    }

    if (fVerbosity>0) {
        EDepSimNamedLog("HEPEVT", "Reading " << lines << " particles");
    }
    if (fVerbosity>1) {
        EDepSim::LogManager::IncreaseIndentation();
        EDepSimNamedLog("HEPEVT",
                        "Vertex "
                        << vtxX << " mm, "
                        << vtxY << " mm, "
                        << vtxZ << " mm, "
                        << vtxT << " ns" );
        EDepSim::LogManager::DecreaseIndentation();
    }

    std::unique_ptr<G4PrimaryVertex> vertex(
        new G4PrimaryVertex(vtxX,vtxY,vtxZ,vtxT));

    // Add vertex ID
    EDepSim::VertexInfo *vertexInfo = new EDepSim::VertexInfo;
    vertex->SetUserInformation(vertexInfo);
    vertexInfo->SetInteractionNumber(vertex_id);

    // Parse the particle lines for the vertex and check input flavor
    // compatibility. This loses some of the context information for
    // now and only adds the particles that should be tracked.
    for (int line = 0; line < lines; ++line) {
        if (!GetTokens(tokens)) {
            EDepSimError("Input truncated in " << fFileName
                       << " after " << fCurrentLine << " lines");
            throw EDepSim::NoMoreEvents();
        }
        if (tokens.size() != 8 && fFlavor == "pythia") {
            EDepSimError("Invalid input at " << fFileName
                         << ":" << fCurrentLine
                         << " -- Wrong number of entries on line");
            throw EDepSim::NoMoreEvents();
        }
        if (tokens.size() != 11 && fFlavor == "pbomb") {
            EDepSimError("Invalid input at " << fFileName
                         << ":" << fCurrentLine
                         << " -- Wrong number of entries on line");
            throw EDepSim::NoMoreEvents();
        }
        if (tokens.size() != 15 && fFlavor == "marley") {
            EDepSimError("Invalid input at " << fFileName
                         << ":" << fCurrentLine
                         << " -- Wrong number of entries on line");
            throw EDepSim::NoMoreEvents();
        }
        int status = AsInteger(tokens[0]);
        if (status != 1) continue;
        int pid       = AsInteger(tokens[1]);
        //int mother1, mother2   // ignored, here for documentation
        int daughter1, daughter2;
        double mass;
        double momX, momY, momZ;
        //double energy;

        // Read in pythia (8-number) input flavor
        // This is the default used in Geant4; see
        // https://apc.u-paris.fr/~franco/g4doxy4.10/html/class_g4_h_e_p_evt_interface.html
        if (fFlavor == "pythia") {
            //daughter1 = AsInteger(tokens[2]);  // ignored, here for docu.
            //daughter2 = AsInteger(tokens[3]); // ignored, here for docu.
            momX   = AsReal(tokens[4])*GeV;
            momY   = AsReal(tokens[5])*GeV;
            momZ   = AsReal(tokens[6])*GeV;
            mass = AsReal(tokens[7])*GeV;
        }
        // Read in ParticleBomb flavor. Useful for multi-vertex events
        // See http://deeplearnphysics.org/DLPGenerator/index.html
        else if (fFlavor == "pbomb") {
            //mother1   = AsInteger(tokens[2]); // ignored, here for docu.
            //mother2   = AsInteger(tokens[3]); // ignored, here for docu.
            daughter1 = AsInteger(tokens[4]);
            daughter2 = AsInteger(tokens[5]);
            momX   = AsReal(tokens[6])*GeV;
            momY   = AsReal(tokens[7])*GeV;
            momZ   = AsReal(tokens[8])*GeV;
            //energy = AsReal(tokens[9])*GeV;
            mass   = AsReal(tokens[10])*GeV;
        }
        // Read Marley (15-number) input flavor, also used by LArSoft's
        // TextFileGen_module.cc; see
        // https://internal.dunescience.org/doxygen/TextFileGen__module_8cc_source.html
        else if (fFlavor == "marley") {
            //mother1   = AsInteger(tokens[2]); // ignored, here for docu.
            //mother2   = AsInteger(tokens[3]); // ignored, here for docu.
            daughter1 = AsInteger(tokens[4]);
            daughter2 = AsInteger(tokens[5]);
            momX   = AsReal(tokens[6])*GeV;
            momY   = AsReal(tokens[7])*GeV;
            momZ   = AsReal(tokens[8])*GeV;
            //energy = AsReal(tokens[9])*GeV;   // ignored, here for docu.
            mass   = AsReal(tokens[10])*GeV;
            vtxX   = AsReal(tokens[11])*cm;
            vtxY   = AsReal(tokens[12])*cm;
            vtxZ   = AsReal(tokens[13])*cm;
            vtxT   = AsReal(tokens[14])*ns;
            vertex->SetPosition(vtxX, vtxY, vtxZ);
            vertex->SetT0(vtxT);
            if (fVerbosity>1) {
                EDepSim::LogManager::IncreaseIndentation();
                EDepSimNamedLog("HEPEVT",
                                "Vertex "
                                << vtxX << " mm, "
                                << vtxY << " mm, "
                                << vtxZ << " mm, "
                                << vtxT << " ns" );
                EDepSim::LogManager::DecreaseIndentation();
            }
        }
        else {
            EDepSimError("Unrecognized HEPEVT input flavor " << fFlavor);
            throw EDepSim::NoMoreEvents();
        }

        std::unique_ptr<G4PrimaryParticle> part(
            new G4PrimaryParticle(pid, momX, momY, momZ));
        part->SetMass(mass);

        // Use Geant's G4HEPEvtParticle class to store daughter information,
        // similar to L120 here:
        // https://apc.u-paris.fr/~franco/g4doxy/html/G4HEPEvtInterface_8cc-source.html#l00078
        std::unique_ptr<G4HEPEvtParticle> hep_particle(
            new G4HEPEvtParticle(part.release(), status, daughter1, daughter2));
        HPlist.push_back(hep_particle.release());
        //vertex->SetPrimary(hep_particle.release()->GetTheParticle());
        if (fVerbosity>1) {
            EDepSim::LogManager::IncreaseIndentation();
            EDepSimNamedLog("HEPEVT",
                            "Primary "
                            << pid << " " << momX << " MeV/c, "
                            << momY << " MeV/c, "
                            << momZ << " MeV/c" );
            EDepSim::LogManager::DecreaseIndentation();
        }
    }

    // Loop stored HEPParticles and set daughter information if applicable.
    // Note that in Geant's notation, JDAHEP1 and JDAHEP2 correspond to
    // daughter1 and daughter2, respectively
    for(size_t i = 0; i < HPlist.size(); ++i) {
        if (HPlist[i]->GetJDAHEP1() > 0) {
            // Shift index to start from 0
            G4int jda1 = HPlist[i]->GetJDAHEP1()-1;
            G4int jda2 = HPlist[i]->GetJDAHEP2()-1;
            G4PrimaryParticle* mother = HPlist[i]->GetTheParticle();
            for( G4int j=jda1; j<=jda2; j++ ) {
                G4PrimaryParticle* daughter = HPlist[j]->GetTheParticle();
                if(HPlist[j]->GetISTHEP()>0) {
                    mother->SetDaughter(daughter);
                    // Set daughter status to -1 for tracking
                    HPlist[j]->Done();
                }
            }
        }
    }

    // Add initial particles to the vertex
    for(size_t ii=0; ii<HPlist.size(); ii++ ) {
        // The Done() method above sets the status of all
        // daughters to be negative
        if( HPlist[ii]->GetISTHEP() > 0 ) {
            G4PrimaryParticle* initialParticle = HPlist[ii]->GetTheParticle();
            vertex->SetPrimary(initialParticle);
        }
    }

    // Clear G4HEPEvtParticles
    for(size_t iii=0;iii<HPlist.size();iii++) {
        delete HPlist[iii];
    }
    HPlist.clear();

    evt->AddPrimaryVertex(vertex.release());
#endif

    return kSuccess;
}
