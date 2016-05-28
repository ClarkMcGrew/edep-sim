////////////////////////////////////////////////////////////
// $Id: DSimNuMIRockKinematicsGenerator.cc,v 1.3 2013/01/17 14:12:03 mcgrew Exp $
//


#include "DSimVertexInfo.hh"
#include "DSimKinemPassThrough.hh"
#include "DSimException.hh"
#include "kinem/DSimNuMIRockKinematicsGenerator.hh"

#include "DSimLog.hh"

#include <globals.hh>
#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4Tokenizer.hh>
#include <G4UnitsTable.hh>
#include <Randomize.hh>

DSimNuMIRockKinematicsGenerator::DSimNuMIRockKinematicsGenerator(
    const G4String& name, const G4String& fluxName)
    : DSimVKinematicsGenerator(name), fFluxName(fluxName) { }

DSimNuMIRockKinematicsGenerator::~DSimNuMIRockKinematicsGenerator() { }

bool DSimNuMIRockKinematicsGenerator::GeneratePrimaryVertex(
    G4Event* anEvent,
    G4LorentzVector&) {

    // Make a position vertex...
    double boxSize = 300*cm;
    G4LorentzVector eventVertex;
    for (int i=0; i<3; ++i) {
        eventVertex[i] = 2*boxSize*G4UniformRand() - boxSize;
    }
    eventVertex[0] = -3.0*meter;
    eventVertex[2] -= 0.5*meter;
    eventVertex[3] = 10.0*microsecond*G4UniformRand();
    
    // Create a new vertex to add the new particles, and add the vertex to the
    // event.
    G4PrimaryVertex* theVertex 
        = new G4PrimaryVertex(G4ThreeVector(eventVertex.x(),
                                            eventVertex.y(),
                                            eventVertex.z()),
                              eventVertex.t());
    anEvent->AddPrimaryVertex(theVertex);

    // Fill the particles to be tracked (status ==1).  These particles are
    // attached to the primary vertex.  Also save the incident neutrino
    // particle and the incident target nucleus; these particles are attached
    // to informational vertex.
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particleDef = particleTable->FindParticle("mu-");

    // Get the momentum.
    double momentum;;
    do {
        momentum = G4RandGauss::shoot(4.0*GeV,2*GeV);
    } while (momentum<2*GeV);
    momentum *= G4UniformRand();

    G4ThreeVector dir(1.0,
                      G4RandGauss::shoot(0.0,0.15),
                      G4RandGauss::shoot(0.0,0.15));
    dir = dir.unit();

    std::cout << "Add rock muon at "
              << " " << eventVertex.x()
              << " " << eventVertex.y()
              << " " << eventVertex.z()
              << " " << eventVertex.t()
              << " going " << dir.x()
              << " " << dir.y()
              << " " << dir.z()
              << std::endl;
        
    // Create the particle.
    G4PrimaryParticle* theParticle
        = new G4PrimaryParticle(particleDef,
                                momentum*dir.x(),
                                momentum*dir.y(),
                                momentum*dir.z());
    theVertex->SetPrimary(theParticle);

    return true;
}

