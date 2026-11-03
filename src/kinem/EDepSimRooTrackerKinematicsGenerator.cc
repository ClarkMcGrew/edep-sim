////////////////////////////////////////////////////////////
//

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>

#include <globals.hh>
#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4ParticleTable.hh>
#include <G4IonTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4Tokenizer.hh>
#include <G4UnitsTable.hh>
#include <Randomize.hh>

#include <TFile.h>
#include <TBits.h>
#include <TObjString.h>
#include <TTree.h>

#include "EDepSimVertexInfo.hh"
#include "EDepSimKinemPassThrough.hh"

#include "kinem/EDepSimRooTrackerKinematicsGenerator.hh"

#include "EDepSimLog.hh"


EDepSim::RooTrackerKinematicsGenerator::RooTrackerKinematicsGenerator(
    const G4String& name, const G4String& filename,
    const G4String& treeName, const G4String& order,
    int firstEvent)
    : EDepSim::VKinematicsGenerator(name), fInput(NULL), fTree(NULL),
      fNextEntry(0) {

    fInput = TFile::Open(filename,"OLD");
    if (!fInput->IsOpen()) {
        throw std::runtime_error("EDepSim::RooTrackerKinematicsGenerator::"
                                 " File Not Open");
    }

    EDepSimLog("Open a RooTracker tree from " << filename);

    fTree = dynamic_cast<TTree*>(fInput->Get(treeName));
    if (!fTree) {
        throw std::runtime_error("EDepSim::RooTrackerKinematicsGenerator::"
                                 " Tree not found by constructor");
    }
    EDepSimNamedInfo("rooTracker",
                     "   File has  " << fTree->GetEntries() << " entries");

    // Find the basename of the input filename.
    std::string::size_type start_pos = filename.rfind("/");
    if (start_pos == std::string::npos) start_pos = 0; else ++start_pos;
    std::string basename(filename,start_pos);
    fFilename = basename + ":" + treeName;

    // Count fatal errors while connecting to the rooTracker tree.  Some of
    // the branches are not used, or can be ignored, but *some* absolutely
    // must be correct or the output is wrong.
    int fatalErrors = 0;
    int branchResult; // Should be zero.

    fEvtFlags = NULL;
    branchResult = fTree->SetBranchAddress("EvtFlags",       &fEvtFlags);
    fEvtCode = NULL;
    branchResult = fTree->SetBranchAddress("EvtCode",        &fEvtCode);
    branchResult = fTree->SetBranchAddress("EvtNum",         &fEvtNum);
    if (branchResult) {
        EDepSimError("Previous error causes a fatally malformed file");
        ++fatalErrors;
    }
    branchResult = fTree->SetBranchAddress("EvtXSec",        &fEvtXSec);
    branchResult = fTree->SetBranchAddress("EvtDXSec",       &fEvtDXSec);
    branchResult = fTree->SetBranchAddress("EvtWght",        &fEvtWght);
    branchResult = fTree->SetBranchAddress("EvtProb",        &fEvtProb);
    branchResult = fTree->SetBranchAddress("EvtVtx",          fEvtVtx);
    if (branchResult) {
        EDepSimError("Previous error causes a fatally malformed file");
        ++fatalErrors;
    }
    branchResult = fTree->SetBranchAddress("StdHepN",        &fStdHepN);
    if (branchResult) {
        EDepSimError("Previous error causes a fatally malformed file");
        ++fatalErrors;
    }
    branchResult = fTree->SetBranchAddress("StdHepPdg",       fStdHepPdg);
    if (branchResult) {
        EDepSimError("Previous error causes a fatally malformed file");
        ++fatalErrors;
    }
    branchResult = fTree->SetBranchAddress("StdHepStatus",    fStdHepStatus);
    if (branchResult) {
        EDepSimError("Previous error causes a fatally malformed file");
        ++fatalErrors;
    }
    branchResult = fTree->SetBranchAddress("StdHepX4",        fStdHepX4);
    branchResult = fTree->SetBranchAddress("StdHepP4",        fStdHepP4);
    if (branchResult) {
        EDepSimError("Previous error causes a fatally malformed file");
        ++fatalErrors;
    }
    branchResult = fTree->SetBranchAddress("StdHepPolz",      fStdHepPolz);
    branchResult = fTree->SetBranchAddress("StdHepFd",        fStdHepFd);
    branchResult = fTree->SetBranchAddress("StdHepLd",        fStdHepLd);
    branchResult = fTree->SetBranchAddress("StdHepFm",        fStdHepFm);
    branchResult = fTree->SetBranchAddress("StdHepLm",        fStdHepLm);
#define PARENT_PARTICLE_PASS_THROUGH
#ifdef PARENT_PARTICLE_PASS_THROUGH
    branchResult = fTree->SetBranchAddress("NuParentPdg",    &fNuParentPdg);
    branchResult = fTree->SetBranchAddress("NuParentDecMode",&fNuParentDecMode);
    branchResult = fTree->SetBranchAddress("NuParentDecP4",   fNuParentDecP4);
    branchResult = fTree->SetBranchAddress("NuParentDecX4",   fNuParentDecX4);
    branchResult = fTree->SetBranchAddress("NuParentProP4",   fNuParentProP4);
    branchResult = fTree->SetBranchAddress("NuParentProX4",   fNuParentProX4);
    branchResult = fTree->SetBranchAddress("NuParentProNVtx",&fNuParentProNVtx);
#endif

    if (fatalErrors) {
        EDepSimError("Fatally malformed rooTracker file");
        throw std::runtime_error("Malformed rooTracker file");
    }

    // Set the input tree to the current rootracker tree that this class is
    // using.
    EDepSim::KinemPassThrough::GetInstance()->AddInputTree(fTree,
                                                           filename,
                                                           GetName());

    // Generate a vector of entries to be used by this generator.

    // Calculate the stride through the file.  This could be cached, but
    // recalculate each time for simplicity.  Note that the stride should be a
    // prime number, and not a divisor of the number of entries in the tree.
    int entries = fTree->GetEntries();
    fEntryVector.resize(entries);
    int stride = 1;
    while (order == "stride") {
        stride = 17;
        if (0 != entries%stride) break;
        stride = 19;
        if (0 != entries%stride) break;
        stride = 23;
        if (0 != entries%stride) break;
        stride = 29;
        if (0 != entries%stride) break;
        throw std::runtime_error("EDepSim::RooTrackerKinematicsGenerator:: "
                    "File size cannot be divisible by 215441");
        break; // throw std::runtime_error doesn't actually return
    }
    int entry = 0;
    for (int i=0; i<entries; ++i) {
        fEntryVector[i] = entry;
        entry = (entry + stride)%entries;
    }
    if (order == "random") {
        for (std::size_t i = fEntryVector.size()-1; i > 0; --i) {
            int j = (i+1)*G4UniformRand();
            std::swap(fEntryVector[i],fEntryVector[j]);
        }
    }

    if (firstEvent > 0) {
        EDepSimLog("   FIRST EVENT WILL BE " << firstEvent);
        if (firstEvent >= entries) {
            throw std::runtime_error("EDepSim::RooTrackerKinematicsGenerator::"
                        "  First event after last event");
        }
        fEntryVector.erase(std::copy(fEntryVector.begin()+firstEvent,
                                     fEntryVector.end(),fEntryVector.begin()),
                           fEntryVector.end());
    }
}

EDepSim::RooTrackerKinematicsGenerator::~RooTrackerKinematicsGenerator() { }

G4String EDepSim::RooTrackerKinematicsGenerator::GetInputName() {
    if (!fInput) return G4String("not-open");
    return G4String(fInput->GetName());
}

std::map<EDepSim::RooTrackerKinematicsGenerator::VertexKey, std::vector<int>> EDepSim::RooTrackerKinematicsGenerator::GroupParticlesByPosition() {
    
    constexpr double kPrecision = 1e-6;
    // round the coordinates to a certain precision to avoid floating point issues
    auto roundTo = [](double value){
        return std::round(value / kPrecision) * kPrecision;
    };

    // create a container to group the particles in
    std::map<VertexKey, std::vector<int>> positionToParticles;

    // group the particles by their (t,x,y,z) position
    for (int cnt = 0; cnt < fStdHepN; ++cnt)
    {
        // keep only the final state particles
        if(fStdHepStatus[cnt] != 1)
            continue;
        // extract the coordinate array
        const double* x4 = fStdHepX4[cnt];
        // build the array as {t,x,y,z} so that the vertices are time-ordered
        positionToParticles[{roundTo(x4[3]),
                             roundTo(x4[0]),
                             roundTo(x4[1]),
                             roundTo(x4[2])}].push_back(cnt);
    }
    return positionToParticles;
}

EDepSim::VKinematicsGenerator::GeneratorStatus
EDepSim::RooTrackerKinematicsGenerator::GeneratePrimaryVertex(
    G4Event* anEvent,
    const G4LorentzVector&) {
    if (!fInput) {
        throw std::runtime_error("EDepSim::RooTrackerKinematicsGenerator::"
                                 " File Not Open");
    }

    /// Check to see if the next event is there.
    if (fNextEntry >= fEntryVector.size()) {
        EDepSimLog("EDepSim::RooTrackerKinematicsGenerator: input file end.");
        throw EDepSim::NoMoreEvents();
    }

    fInput->cd();

    int entry = fEntryVector.at(fNextEntry);
    // Get current entry to be used as new vertex - see comment below.
    fTree->GetEntry(entry);
    // Store current entry in the pass-through obj. To avoid mismatch
    // and false results call EDepSim::KinemPassThrough::AddEntry(fTreePtr, X)
    // where X is same as X in most recent call to fTreePtr->GetEntry(X).
    EDepSim::KinemPassThrough::GetInstance()->AddEntry(fTree, entry);
    EDepSimVerbose("Use rooTracker event number " << fEvtNum
                 << " (entry #" << entry << " in tree)");

    std::string eventCode("not-specified");
    if (fEvtCode) {
        eventCode = fEvtCode->String();
    }
    else {
        EDepSimError("Malformed input file -- missing EvtCode");
    }

    // Increment the next entry counter.
    ++fNextEntry;

    // Set the default generator status.  This should be overridden if the
    // state changes.
    GeneratorStatus generatorStatus = kSuccess;

    // Check if this is an end-of-sequence marker.
    if (fStdHepN == 1 && fStdHepStatus[0]<0) {
        EDepSimLog("End of event");
        return kEndEvent;
    }

    // VertexInfo holding the global interaction metadata and the informational
    // vertices. It is attached to the first primary vertex; the other primary
    // vertices get their own VertexInfo with a copy of the scalar fields only.
    EDepSim::VertexInfo *vertexInfo = new EDepSim::VertexInfo;
    // Fill the information fields for this vertex.
    vertexInfo->SetReaction(eventCode);

    // Set the file name for this event.
    std::ostringstream fs;
    fs << fFilename << ":" << entry;
    vertexInfo->SetFilename(fs.str());
    // Set the interaction number to that of the RooTracker pass-through tree.
    vertexInfo->SetInteractionNumber(
        EDepSim::KinemPassThrough::GetInstance()->LastEntryNumber());
    vertexInfo->SetCrossSection(fEvtXSec*1E-38*cm2);
    vertexInfo->SetDiffCrossSection(fEvtDXSec*1E-38*cm2);
    vertexInfo->SetWeight(fEvtWght);
    vertexInfo->SetProbability(fEvtProb);

    // Add an informational vertex for storing the incoming
    // neutrino particle and target nucleus.
    // Fill the informational vertex outside the loop
    G4PrimaryVertex* theIncomingVertex
        = new G4PrimaryVertex(G4ThreeVector(fEvtVtx[0]*m,
                                            fEvtVtx[1]*m,
                                            fEvtVtx[2]*m),
                              fEvtVtx[3]*second);
    vertexInfo->AddInformationalVertex(theIncomingVertex);

    EDepSim::VertexInfo *incomingVertexInfo = new EDepSim::VertexInfo;
    incomingVertexInfo->SetName("initial-state");
    incomingVertexInfo->SetReaction(eventCode);
    theIncomingVertex->SetUserInformation(incomingVertexInfo);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // save the incident neutrino and target (status==0 particles)
    for (int cnt = 0; cnt < fStdHepN; ++cnt) {
        if(fStdHepStatus[cnt] != 0)
            continue;
        G4ParticleDefinition* particleDef
            = particleTable->FindParticle(fStdHepPdg[cnt]);
        if (!particleDef) {
            //maybe we have an ion; figure out if it makes any sense
            int ionA = (fStdHepPdg[cnt]/10) % 1000;
            int ionZ = (fStdHepPdg[cnt]/10000) % 1000;
            int type = (fStdHepPdg[cnt]/100000000);
            if (type == 10 && ionZ > 0 && ionA > ionZ) {
                G4IonTable* ionTable = particleTable->GetIonTable();
                particleDef = ionTable->GetIon(ionZ, ionA);
            }
            else if (type == 20) {
                // This is a pseudo-particle so skip it.
                continue;
            }
        }

        if (!particleDef) {
            EDepSimSevere(" Particle code " << fStdHepPdg[cnt]
                    << " not recognized (not tracking)");
            continue;
        }

        // Determine a name for the particle.
        std::string particleName =
            particleDef ? particleDef->GetParticleName(): "unknown";

        // Get the momentum.
        G4LorentzVector momentum(fStdHepP4[cnt][0]*GeV,
                                 fStdHepP4[cnt][1]*GeV,
                                 fStdHepP4[cnt][2]*GeV,
                                 fStdHepP4[cnt][3]*GeV);
        // create the particle.
        G4PrimaryParticle* theParticle
            = new G4PrimaryParticle(particleDef,
                                    momentum.px(),
                                    momentum.py(),
                                    momentum.pz());
        theParticle->SetPolarization(fStdHepPolz[cnt][0],
                                    fStdHepPolz[cnt][1],
                                    fStdHepPolz[cnt][2]);
        EDepSimNamedInfo(
            "rooTracker",
            "Incoming "
            << particleDef->GetParticleName()
            << " " << theParticle->GetPDGcode()
            << " " << momentum.e()/MeV << " MeV"
            << " " << momentum.m()/MeV << " MeV/c^2");
        // add the initial state particle to the informational vertex
        theIncomingVertex->SetPrimary(theParticle);
    }

    // Add multiple vertices by looping over the map
    auto groupedPositions = EDepSim::RooTrackerKinematicsGenerator::GroupParticlesByPosition();
    // flag for managing the assignment of vertexInfo
    bool firstVertex = true;

    // if no final state particles (fStdHepStatus==1) are found
    // create an empty primary vertex at EvtVtx carrying the info
    if (groupedPositions.empty()) {
        G4PrimaryVertex* theVertex = new G4PrimaryVertex(
            G4ThreeVector(fEvtVtx[0]*m, fEvtVtx[1]*m, fEvtVtx[2]*m),
            fEvtVtx[3]*second);
        anEvent->AddPrimaryVertex(theVertex);
        theVertex->SetUserInformation(vertexInfo);
        firstVertex = false;
    }   

    // loop over the final state particles in each vertex group
    for (const auto& particleGroup : groupedPositions){
        // get the vertex group indices
        const auto &vtxIndices = particleGroup.second;
        const int firstPartIdx = vtxIndices.front();

        // first vertex adopts vertexInfo; the others get a copy of the scalar fields only
        EDepSim::VertexInfo* thisVertexInfo = nullptr;
        if (firstVertex) {
            // adopt the original vertexInfo in the first G4PrimaryVertex
            thisVertexInfo = vertexInfo;
            firstVertex = false;
        } else {
            thisVertexInfo = new EDepSim::VertexInfo;
            thisVertexInfo->SetReaction(vertexInfo->GetReaction());
            thisVertexInfo->SetFilename(vertexInfo->GetFilename());
            thisVertexInfo->SetInteractionNumber(vertexInfo->GetInteractionNumber());
            thisVertexInfo->SetCrossSection(vertexInfo->GetCrossSection());
            thisVertexInfo->SetDiffCrossSection(vertexInfo->GetDiffCrossSection());
            thisVertexInfo->SetWeight(vertexInfo->GetWeight());
            thisVertexInfo->SetProbability(vertexInfo->GetProbability());
        }

        // fill in the vtx. position from the first element in the group
        // fStdHepX4 is assumed to be in [fermi, fermi, fermi, ys], so we convert
        G4PrimaryVertex *theVertex = new G4PrimaryVertex(G4ThreeVector(fEvtVtx[0] * m + fStdHepX4[firstPartIdx][0] * fermi,
                                                                       fEvtVtx[1] * m + fStdHepX4[firstPartIdx][1] * fermi,
                                                                       fEvtVtx[2] * m + fStdHepX4[firstPartIdx][2] * fermi),
                                                         fEvtVtx[3] * second + fStdHepX4[firstPartIdx][3] * (1e-24) * second);
        // add the vertex to the event
        anEvent->AddPrimaryVertex(theVertex);
        EDepSimNamedInfo("rooTracker","Vertex @ "
                        << G4BestUnit(theVertex->GetPosition(), "Length")
                        << " Time: " << G4BestUnit(theVertex->GetT0(), "Time"));

        // Add the information field to the vertex.
        theVertex->SetUserInformation(thisVertexInfo);

        // Fill the particles to be tracked (status ==1) in the vertex group.  
        for (const auto& cnt : vtxIndices) {
            G4ParticleDefinition* particleDef
                = particleTable->FindParticle(fStdHepPdg[cnt]);
            if (!particleDef) {
                //maybe we have an ion; figure out if it makes any sense
                int ionA = (fStdHepPdg[cnt]/10) % 1000;
                int ionZ = (fStdHepPdg[cnt]/10000) % 1000;
                int type = (fStdHepPdg[cnt]/100000000);
                if (type == 10 && ionZ > 0 && ionA > ionZ) {
                    G4IonTable* ionTable = particleTable->GetIonTable();
                    particleDef = ionTable->GetIon(ionZ, ionA);
                }
                else if (type == 20) {
                    // This is a pseudo-particle so skip it.
                    continue;
                }
            }

            // Determine a name for the particle.
            std::string particleName =
                particleDef ? particleDef->GetParticleName(): "unknown";

            // Get the momentum.
            G4LorentzVector momentum(fStdHepP4[cnt][0]*GeV,
                                    fStdHepP4[cnt][1]*GeV,
                                    fStdHepP4[cnt][2]*GeV,
                                    fStdHepP4[cnt][3]*GeV);

            if (fStdHepStatus[cnt] != 1) {
                EDepSimVerbose("Untracked particle: " << cnt
                            << " " << particleName
                            << " with " << momentum.e()/MeV
                            << " MeV "
                            << " w/ mothers " << fStdHepFm[cnt]
                            << " to " << fStdHepLm[cnt]);
            }

            if (!particleDef) {
                EDepSimSevere(" Particle code " << fStdHepPdg[cnt]
                        << " not recognized (not tracking)");
                continue;
            }

            // create the particle.
            G4PrimaryParticle* theParticle
                = new G4PrimaryParticle(particleDef,
                                        momentum.px(),
                                        momentum.py(),
                                        momentum.pz());
            theParticle->SetPolarization(fStdHepPolz[cnt][0],
                                        fStdHepPolz[cnt][1],
                                        fStdHepPolz[cnt][2]);
            EDepSimNamedInfo(
                "rooTracker",
                "Tracking "
                << particleDef->GetParticleName()
                << " " << theParticle->GetPDGcode()
                << " " << momentum.e()/MeV << " MeV"
                << " " << momentum.m()/MeV << " MeV/c^2");
            theVertex->SetPrimary(theParticle);
        }
    }

#ifdef PARENT_PARTICLE_PASS_THROUGH
    // Fill the particles at the decay vertex.
    G4PrimaryVertex* theDecayVertex
        = new G4PrimaryVertex(G4ThreeVector(fNuParentDecX4[0]*m,
                                            fNuParentDecX4[1]*m,
                                            fNuParentDecX4[2]*m),
                              fNuParentDecX4[3]*second);
    vertexInfo->AddInformationalVertex(theDecayVertex);

    // Add an information field to the vertex.
    EDepSim::VertexInfo *decayVertexInfo = new EDepSim::VertexInfo;
    decayVertexInfo->SetName("beam-particle:Decay");
    {
        std::ostringstream tmp;
        tmp << fNuParentDecMode;
        decayVertexInfo->SetReaction(tmp.str());
    }
    theDecayVertex->SetUserInformation(decayVertexInfo);

    G4PrimaryParticle* theDecayParticle
        = new G4PrimaryParticle(fNuParentPdg,
                                fNuParentDecP4[0]*GeV,
                                fNuParentDecP4[1]*GeV,
                                fNuParentDecP4[2]*GeV);
    theDecayVertex->SetPrimary(theDecayParticle);

    // Fill the particles at the production vertex.
    G4PrimaryVertex* theProductionVertex
        = new G4PrimaryVertex(G4ThreeVector(fNuParentProX4[0]*m,
                                            fNuParentProX4[1]*m,
                                            fNuParentProX4[2]*m),
                              fNuParentProX4[3]*second);
    decayVertexInfo->AddInformationalVertex(theProductionVertex);

    // Add information about the production vertex.
    EDepSim::VertexInfo *productionVertexInfo = new EDepSim::VertexInfo;
    productionVertexInfo->SetName("beam-particle:Production");
    productionVertexInfo->SetInteractionNumber(fNuParentProNVtx);
    theProductionVertex->SetUserInformation(productionVertexInfo);

    G4PrimaryParticle* theProductionParticle
        = new G4PrimaryParticle(fNuParentPdg,
                                fNuParentProP4[0]*GeV,
                                fNuParentProP4[1]*GeV,
                                fNuParentProP4[2]*GeV);
    theProductionVertex->SetPrimary(theProductionParticle);
#endif

    return generatorStatus;
}
