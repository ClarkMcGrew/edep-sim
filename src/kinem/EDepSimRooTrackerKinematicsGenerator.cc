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

    fEvtFlags = NULL;
    fTree->SetBranchAddress("EvtFlags",       &fEvtFlags);
    fEvtCode = NULL;
    fTree->SetBranchAddress("EvtCode",        &fEvtCode);
    fTree->SetBranchAddress("EvtNum",         &fEvtNum);
    fTree->SetBranchAddress("EvtXSec",        &fEvtXSec);
    fTree->SetBranchAddress("EvtDXSec",       &fEvtDXSec);
    fTree->SetBranchAddress("EvtWght",        &fEvtWght);
    fTree->SetBranchAddress("EvtProb",        &fEvtProb);
    fTree->SetBranchAddress("EvtVtx",          fEvtVtx);
    fTree->SetBranchAddress("StdHepN",        &fStdHepN);
    fTree->SetBranchAddress("StdHepPdg",       fStdHepPdg);
    fTree->SetBranchAddress("StdHepStatus",    fStdHepStatus);
    fTree->SetBranchAddress("StdHepX4",        fStdHepX4);
    fTree->SetBranchAddress("StdHepP4",        fStdHepP4);
    fTree->SetBranchAddress("StdHepPolz",      fStdHepPolz);
    fTree->SetBranchAddress("StdHepFd",        fStdHepFd);
    fTree->SetBranchAddress("StdHepLd",        fStdHepLd);
    fTree->SetBranchAddress("StdHepFm",        fStdHepFm);
    fTree->SetBranchAddress("StdHepLm",        fStdHepLm);
#define PARENT_PARTICLE_PASS_THROUGH
#ifdef PARENT_PARTICLE_PASS_THROUGH
    fTree->SetBranchAddress("NuParentPdg",    &fNuParentPdg);
    fTree->SetBranchAddress("NuParentDecMode",&fNuParentDecMode);
    fTree->SetBranchAddress("NuParentDecP4",   fNuParentDecP4);
    fTree->SetBranchAddress("NuParentDecX4",   fNuParentDecX4);
    fTree->SetBranchAddress("NuParentProP4",   fNuParentProP4);
    fTree->SetBranchAddress("NuParentProX4",   fNuParentProX4);
    fTree->SetBranchAddress("NuParentProNVtx",&fNuParentProNVtx);
#endif

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
        std::random_shuffle(fEntryVector.begin(), fEntryVector.end());
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
    // Store current entry in the pass-through obj. N.B. To avoid mismatch
    // and false results call EDepSim::KinemPassThrough::AddEntry(fTreePtr, X)
    // where X is same as X in most recent call to fTreePtr->GetEntry(X).
    EDepSim::KinemPassThrough::GetInstance()->AddEntry(fTree, entry);
    EDepSimVerbose("Use rooTracker event number " << fEvtNum
                 << " (entry #" << entry << " in tree)");

    // Increment the next entry counter.
    ++fNextEntry;

    // Set the default generator status.  This should be overridden if the
    // state changes.
    GeneratorStatus generatorStatus = kSuccess;

    // Check if this is an end-of-sequence marker.
    if (fStdHepN == 1 && fStdHepStatus[0]<0) {
        return kEndEvent;
    }

    // Create a new vertex to add the new particles, and add the vertex to the
    // event.
    G4PrimaryVertex* theVertex
        = new G4PrimaryVertex(G4ThreeVector(fEvtVtx[0]*m,
                                            fEvtVtx[1]*m,
                                            fEvtVtx[2]*m),
                              fEvtVtx[3]*second);
    anEvent->AddPrimaryVertex(theVertex);
    EDepSimNamedInfo("rooTracker","Vertex @ "
                     << G4BestUnit(theVertex->GetPosition(), "Length")
                     << " Time: " << G4BestUnit(theVertex->GetT0(), "Time"));

    // Add an information field to the vertex.
    EDepSim::VertexInfo *vertexInfo = new EDepSim::VertexInfo;
    theVertex->SetUserInformation(vertexInfo);

    // Fill the information fields for this vertex.
    vertexInfo->SetReaction(std::string(fEvtCode->String().Data()));
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
    G4PrimaryVertex* theIncomingVertex
        = new G4PrimaryVertex(G4ThreeVector(fEvtVtx[0]*m,
                                            fEvtVtx[1]*m,
                                            fEvtVtx[2]*m),
                              fEvtVtx[3]*second);
    vertexInfo->AddInformationalVertex(theIncomingVertex);

    // Add an information field to the vertex.
    EDepSim::VertexInfo *incomingVertexInfo = new EDepSim::VertexInfo;
    incomingVertexInfo->SetName("initial-state");
    incomingVertexInfo->SetReaction(std::string(fEvtCode->String().Data()));
    theIncomingVertex->SetUserInformation(incomingVertexInfo);

    // Fill the particles to be tracked (status ==1).  These particles are
    // attached to the primary vertex.  Also save the incident neutrino
    // particle and the incident target nucleus; these particles are attached
    // to informational vertex.
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    for (int cnt = 0; cnt < fStdHepN; ++cnt) {
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

        // We are only interested in particles to be tracked (status==1)
        // or incident neutrino/target nucleus (status==0).
        if( !(fStdHepStatus[cnt] == 0 || fStdHepStatus[cnt] == 1)) {
            continue;
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

        if (fStdHepStatus[cnt] == 0) {
            EDepSimNamedInfo(
                "rooTracker",
                "Incoming "
                << particleDef->GetParticleName()
                << " " << theParticle->GetPDGcode()
                << " " << momentum.e()/MeV << " MeV"
                << " " << momentum.m()/MeV << " MeV/c^2");
            theIncomingVertex->SetPrimary(theParticle);
        }
        else if (fStdHepStatus[cnt] == 1){
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
    // Fill the particles at the decay vertex.  These are the first info
    // vertex.
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
