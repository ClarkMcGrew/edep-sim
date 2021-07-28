#include "Py8CharmDecayerPhysics.hh"
#include "Py8Decayer.hh"

#include <G4ParticleDefinition.hh>
#include <G4ProcessManager.hh>
#include <G4Decay.hh>
#include <G4DecayTable.hh>

#include <set>

// factory
#include "G4PhysicsConstructorFactory.hh"
// register it with contructor factory
G4_DECLARE_PHYSCONSTR_FACTORY(Py8CharmDecayerPhysics);


Py8CharmDecayerPhysics::Py8CharmDecayerPhysics(G4int)
  : G4VPhysicsConstructor("Py8CharmDecayerPhysics")
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Py8CharmDecayerPhysics::~Py8CharmDecayerPhysics()
{
}

void Py8CharmDecayerPhysics::ConstructParticle()
{
   // Nothing needs to be done here
}

void Py8CharmDecayerPhysics::ConstructProcess()
{

   // Loop over all particles instantiated and add external decayer
   // to all decay processes where there's no decay table, plus tau's

   // Create external decayer for G4Decay process
   // NOTE: The extDecayer will be deleted in G4Decay destructor
   Py8Decayer* extDecayer = new Py8Decayer();

   auto particleIterator=GetParticleIterator();
   particleIterator->reset();
   while ((*particleIterator)())
   {
      G4ParticleDefinition* particle = particleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
/*
      if ( verboseLevel > 1 ) {
         G4cout << "Setting ext decayer for: "
                <<  particleIterator->value()->GetParticleName()
                << G4endl;
      }
*/
      G4ProcessVector* processVector = pmanager->GetProcessList();
      for ( size_t i=0; i < (size_t)processVector->length(); ++i )
      {
         G4Decay* decay = dynamic_cast<G4Decay*>((*processVector)[i]);
         if ( decay )
         {
            // set ext decayer to all particles that don't have a decay table
            if ( !particle->GetDecayTable() )
            {
               decay->SetExtDecayer(extDecayer);
            }
            // now for charm hadrons remove the existing decay table and set ext decayer
            std::set<int> abspdglist = { 411, 421, 10411, 10421, 413, 423, 10413, 10423,
                                         20413, 20423, 415, 425, 431, 10431, 433, 10433,
                                         20433, 435, 441, 10441, 100441, 443, 10443, 20443,
                                         100443, 30443, 445 };
            // if ( abs(particle->GetPDGEncoding()) == 15 )
            int abspdg = abs(particle->GetPDGEncoding());
            if ( abspdglist.find(abspdg) != abspdglist.end() )
            {
               if ( particle->GetDecayTable() )
               {
                  delete particle->GetDecayTable();
                  particle->SetDecayTable(nullptr);
               }
               decay->SetExtDecayer(extDecayer);
            }
         }
      }
   }

   return;

}
