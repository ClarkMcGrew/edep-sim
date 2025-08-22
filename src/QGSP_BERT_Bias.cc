#include <iomanip>   
#include <CLHEP/Units/SystemOfUnits.h>

#include <globals.hh>
#include <G4ios.hh>
#include <G4ProcessManager.hh>
#include <G4ProcessVector.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleTable.hh>

#include <G4Material.hh>
#include <G4MaterialTable.hh>

#include <G4DecayPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4EmExtraPhysics.hh>
#include <G4IonPhysics.hh>
#include <G4StoppingPhysics.hh>
#include <G4HadronElasticPhysics.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4StepLimiterPhysics.hh>

#include "QGSP_BERT_Bias.hh"
#include "G4HadronPhysicsQGSP_BERT_Bias.hh"

#include <G4PhysListStamper.hh>
G4_DECLARE_PHYSLIST_FACTORY(QGSP_BERT_Bias);

QGSP_BERT_Bias::QGSP_BERT_Bias(G4int ver)
{

  G4cout << "<<< Geant4 Physics List simulation engine: QGSP_BERT_Bias"<<G4endl;
  G4cout <<G4endl;

  defaultCutValue = 0.7*CLHEP::mm;  
  SetVerboseLevel(ver);

  // EM Physics
  RegisterPhysics( new G4EmStandardPhysics(ver) );

  // Synchroton Radiation & GN Physics
  RegisterPhysics( new G4EmExtraPhysics(ver) );

  // Decays
  RegisterPhysics( new G4DecayPhysics(ver) );

   // Hadron Elastic scattering
  RegisterPhysics( new G4HadronElasticPhysics(ver) );

  // Hadron Physics
  RegisterPhysics( new G4HadronPhysicsQGSP_BERT_Bias(ver));

  // Stopping Physics
  RegisterPhysics( new G4StoppingPhysics(ver) );

  // Ion Physics
  RegisterPhysics( new G4IonPhysics(ver));
  
  // Neutron tracking cut
  RegisterPhysics( new G4NeutronTrackingCut(ver));

  // Step Limiting
//  RegisterPhysics(new G4StepLimiterPhysics("STEPLIMIT"));

}
