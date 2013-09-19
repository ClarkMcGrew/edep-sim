////////////////////////////////////////////////////////////
// $Id: DSimRooTrackerKinematicsGenerator.cc,v 1.3 2013/01/17 14:12:03 mcgrew Exp $
//


#include "DSimVertexInfo.hh"
#include "DSimKinemPassThrough.hh"
#include "DSimException.hh"
#include "kinem/DSimRooTrackerKinematicsGenerator.hh"

#include "DSimLog.hh"

#include <globals.hh>
#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4Tokenizer.hh>
#include <G4UnitsTable.hh>

#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

DSimRooTrackerKinematicsGenerator::DSimRooTrackerKinematicsGenerator(
    const G4String& name, const G4String& filename, 
    const G4String& treeName, const G4String& order,
    int firstEvent) 
    : DSimVKinematicsGenerator(name), fInput(NULL), fTree(NULL), 
      fNextEntry(0) {

    fInput = TFile::Open(filename,"OLD");
    if (!fInput->IsOpen()) {
        DSimError("DSimRooTrackerKinematicsGenerator:: File Not Open");
    }

    DSimLog("Open a RooTracker tree from " << filename);

    fTree = dynamic_cast<TTree*>(fInput->Get(treeName));
    if (!fTree) {
        DSimError("DSimRooTrackerKinematicsGenerator:: "
                    "Tree not found by constructor");
    }
    DSimInfo("   File has  " << fTree->GetEntries() << " entries");

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

    // Set the input tree to the current rootracker tree that this class is
    // using.
    DSimKinemPassThrough::GetInstance()->AddInputTree(fTree,
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
        DSimError("DSimRooTrackerKinematicsGenerator:: "
                    "File size cannot be divisible by 215441");
        break; // DSimException doesn't actually return
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
        DSimLog("   FIRST EVENT WILL BE " << firstEvent);
        if (firstEvent >= entries) {
            DSimError("DSimRooTrackerKinematicsGenerator::"
                        "  First event after last event");
        }
        fEntryVector.erase(std::copy(fEntryVector.begin()+firstEvent,
                                     fEntryVector.end(),fEntryVector.begin()),
                           fEntryVector.end());
    }
}

DSimRooTrackerKinematicsGenerator::~DSimRooTrackerKinematicsGenerator() { }

G4String DSimRooTrackerKinematicsGenerator::GetInputName() {
    if (!fInput) return G4String("not-open");
    return G4String(fInput->GetName());
}

bool DSimRooTrackerKinematicsGenerator::GeneratePrimaryVertex(
    G4Event* anEvent,
    G4LorentzVector&) {
    if (!fInput) {
        DSimError("DSimRooTrackerKinematicsGenerator:: File Not Open");
    }
    
    /// Check to see if the next event is there.
    if (fNextEntry >= fEntryVector.size()) {
        DSimLog("DSimRooTrackerKinematicsGenerator: end of input file.");
        throw DSimNoMoreEvents();
    }

    fInput->cd();

    int entry = fEntryVector.at(fNextEntry);
    // Get current entry to be used as new vertex - see comment below.
    fTree->GetEntry(entry);
    // Store current entry in the pass-through obj. N.B. To avoid mismatch 
    // and false results call DSimKinemPassThrough::AddEntry(fTreePtr, X)
    // where X is same as X in most recent call to fTreePtr->GetEntry(X).
    DSimKinemPassThrough::GetInstance()->AddEntry(fTree, entry);
    DSimVerbose("Use rooTracker event number " << fEvtNum 
                 << " (entry #" << entry << " in tree)");

    // Increment the next entry counter.
    ++fNextEntry;

    // Create a new vertex to add the new particles, and add the vertex to the
    // event.
    G4PrimaryVertex* theVertex 
        = new G4PrimaryVertex(G4ThreeVector(fEvtVtx[0]*m,
                                            fEvtVtx[1]*m,
                                            fEvtVtx[2]*m),
                              fEvtVtx[3]*second);
    anEvent->AddPrimaryVertex(theVertex);

    // Add an information field to the vertex.
    DSimVertexInfo *vertexInfo = new DSimVertexInfo;
    theVertex->SetUserInformation(vertexInfo);

    // Fill the information fields for this vertex.
    vertexInfo->SetReaction(std::string(fEvtCode->String().Data()));
    // Set the file name for this event.
    std::ostringstream fs;
    fs << fFilename << ":" << entry;
    vertexInfo->SetFilename(fs.str());
    // Set the interaction number to that of the RooTracker pass-through tree.
    vertexInfo->SetInteractionNumber(
        DSimKinemPassThrough::GetInstance()->LastEntryNumber());
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
    DSimVertexInfo *incomingVertexInfo = new DSimVertexInfo;
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
                particleDef = particleTable->GetIon(ionZ, ionA, 0.*MeV);
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
            DSimVerbose("Untracked particle: " << cnt 
                         << " " << particleName
                         << " with " << momentum.e()/MeV 
                         << " MeV " 
                         << " w/ mothers " << fStdHepFm[cnt]
                         << " to " << fStdHepLm[cnt]);        
        }
        
        // We are only interested in particles to be tracked (status==1)
        // or incident neutrino/target nucleus (status==0).
        if( !(fStdHepStatus[cnt] == 0 || fStdHepStatus[cnt] == 1))
            continue;


        if (!particleDef) {
            DSimSevere(" Particle code " << fStdHepPdg[cnt]
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
            theIncomingVertex->SetPrimary(theParticle);
        }else if (fStdHepStatus[cnt] == 1){
            theVertex->SetPrimary(theParticle);
        }
    }

    return true;
}

