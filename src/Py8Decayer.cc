
#include "Py8Decayer.hh"

#include "G4DynamicParticle.hh"
#include "G4ParticleTable.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

using namespace Pythia8;

Py8Decayer::Py8Decayer()
   : G4VExtDecayer("Py8Decayer"),
   fDecayer(nullptr)
{

   // use default path to the xml/configs but do NOT print banner
   //
   fDecayer = new Pythia( "../share/Pythia8/xmldoc", false );

   fDecayer->readString("ProcessLevel:all = off"); // the trick!
   fDecayer->readString("ProcessLevel::resonanceDecays=on");
    
   // shut off Pythia8 (default) verbosty
   //
   fDecayer->readString("Init:showAllSettings=false");
   fDecayer->readString("Init:showChangedSettings=false");
   fDecayer->readString("Init:showAllParticleData=false");
   fDecayer->readString("Init::showChangedParticleData=false");
   //
   // specify how many Py8 events to print out, at either level
   //
   fDecayer->readString("Next::numberShowProcess = 0" );
   fDecayer->readString("Next:numberShowEvent = 10");
           
   fDecayer->init();
   
   // shut off decays of pi0's as we want Geant4 to handle them
   //
   fDecayer->readString("111:onMode = off");
}

Py8Decayer::~Py8Decayer()
{

   if ( fDecayer )
   {
      delete fDecayer;
      fDecayer = nullptr;
   }

}

G4DecayProducts* Py8Decayer::ImportDecayProducts(const G4Track& track)
{

   fDecayer->event.reset();
   
   G4DecayProducts* dproducts = nullptr;   
   
   G4ParticleDefinition* pd = track.GetDefinition();
   int    pdgid   = pd->GetPDGEncoding();
   
   // check if pdgid is consistent with Pythia8 particle table
   //   
   if ( !fDecayer->particleData.findParticle( pdgid ) )
   {
      G4cout << " can NOT find pdgid = " << pdgid << " in Pythia8::ParticleData" << G4endl;
      return dproducts;
   }
   
   if ( !fDecayer->particleData.canDecay(pdgid) ) // or mayDecay() ???
   {
      G4cout << " Particle of pdgid = " << pdgid << " can NOT be decayed by Pythia8" << G4endl;
      return dproducts;
   }
   
   // NOTE: Energy in GeV 

   // G4ThreeVector& polarz = track.GetPolarization();
   
   fDecayer->event.append( pdgid, 1, 0, 0, 
                           track.GetMomentum().x() / CLHEP::GeV, 
			   track.GetMomentum().y() / CLHEP::GeV,  
			   track.GetMomentum().z() / CLHEP::GeV,
			   track.GetDynamicParticle()->GetTotalEnergy() / CLHEP::GeV,
			   pd->GetPDGMass() / CLHEP::GeV );

//
//   double delta_phi = track.GetPolarization().angle( track.GetMomentumDirection() );
//   double cos_delta_phi = cos( delta_phi );
//   fDecayer->event.back().pol( cos_delta_phi );
   fDecayer->event.back().pol( cos( track.GetPolarization().angle( track.GetMomentumDirection() ) ) );

/* NOT sure if this is really needed...
   // NOTE: space-time coordinates all in mm, i.e. units are chosen such that 
   //       the speed of light c is unity. 
   //       In particular, times are also in mm, not in seconds
   
   // HepMC::GenEvent::vertex_const_iterator vitr.....
   // double g4t1 = (*vitr)->position().t() * mm / c_light;
   double g4time = track.GetGlobalTime() / CLHEP::second;
   double py8time = ( g4time * CLHEP::c_light ) / CLHEP::mm;
   
   fDecayer->event.back().vProd( track.GetPosition().x() / CLHEP::mm,
                                 track.GetPosition().y() / CLHEP::mm,
		                 track.GetPosition().z() / CLHEP::mm,
		                 py8time );     
*/
/* alternative (and maybe more refined ?) way to form Py8 event...
   Particle py8part( pdgid, 1,
                     0, 0, 0, 0, 0, 0,
                     track.GetMomentum().x() / CLHEP::GeV, 
	             track.GetMomentum().y() / CLHEP::GeV,  
	             track.GetMomentum().z() / CLHEP::GeV,
		     track.GetDynamicParticle()->GetTotalEnergy() / CLHEP::GeV,
		     pd->GetPDGMass() / CLHEP::GeV  );

   // NOTE: space-time coordinates all in mm, i.e. units are chosen such that 
   //       the speed of light c is unity. 
   //       In particular, times are also in mm, not in seconds
   
   py8part.vProd( track.GetPosition().x() / CLHEP::mm,
                  track.GetPosition().y() / CLHEP::mm,
		  track.GetPosition().z() / CLHEP::mm,
		  py8time ); 
		  		      
   py8part.tau( fDecayer->ParticleData.tau0(pdgid) );
   
   fDecayer->append( py8part );
*/   
   int npart_before_decay = fDecayer->event.size();
   
   fDecayer->next();
   
   int npart_after_decay = fDecayer->event.size();
   
   // create & fill up decay products
   //
   dproducts = new G4DecayProducts(*(track.GetDynamicParticle()));
   
   // create G4DynamicParticle out of each fDecayer->event entry (except the 1st one)
   // and push into dproducts
   
   for ( int ip=npart_before_decay; ip<npart_after_decay; ++ip )
   {
      
      // do we have to skip neutrinos ???
            
      //
      G4ParticleDefinition* pddec = 
         G4ParticleTable::GetParticleTable()->FindParticle( fDecayer->event[ip].id() );
      if ( !pddec ) continue; // maybe we should print out a warning !
      G4ThreeVector momentum = G4ThreeVector( fDecayer->event[ip].px() * CLHEP::GeV,
                                              fDecayer->event[ip].py() * CLHEP::GeV,
					      fDecayer->event[ip].pz() * CLHEP::GeV ); 
      //
      dproducts->PushProducts( new G4DynamicParticle( pddec, momentum) ); 
   }
   
// --->   G4cout << " Number of decay products = " << dproducts->entries() << G4endl;
   
   return dproducts;

}
