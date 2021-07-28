#include "EDepSimCreatePhysicsList.hh"

// The default physics list.
//#include "EDepSimPhysicsList.hh"

#include "G4PhysListRegistry.hh"
#include "G4PhysListFactoryAlt.hh"
// pull in a user defined physics list definition into the main program
// and register it with the factory (doesn't have to be the main program
// but the .o containing the declaration _must_ get linked/loaded)
#include "G4PhysListStamper.hh"  // defines macro for factory registration
// allow ourselves to give the user extra info about available physics ctors
#include "G4PhysicsConstructorFactory.hh"


#include "EDepSimException.hh"
#include "EDepSimExtraPhysics.hh"
#include "G4OpticalPhysics.hh"

#include "EDepSimCreatePhysicsListMessenger.hh"
// initialize static/global instance
EDepSim::CreatePhysicsListMessenger* gCreatePhysicsListMessenger = 0;

// anonymous namespace for use internally here only
namespace {

  void PrintAvailable(G4int verbosity) {
    G4cout << G4endl;
    G4cout << "extensibleFactory: here are the available physics lists:"
           << G4endl;
    g4alt::G4PhysListFactory factory;
    factory.PrintAvailablePhysLists();

    // if user asked for extra verbosity then print physics ctors as well
    if ( verbosity > 1 ) {
      G4cout << G4endl;
      G4cout << "extensibleFactory: "
             << "here are the available physics ctors that can be added:"
             << G4endl;
      G4PhysicsConstructorRegistry* g4pctorFactory =
        G4PhysicsConstructorRegistry::Instance();
      g4pctorFactory->PrintAvailablePhysicsConstructors();
    }
}

  void FailedToCreatePhysicsList(G4String physicsListName) {
   G4cerr << "extensibleFactory: PhysicsList '"
               << physicsListName
               << "' was not available in g4alt::PhysListFactory." << G4endl;
        PrintAvailable(2);

        // if we can't get what the user asked for...
        //    don't go on to use something else, that's confusing
        G4ExceptionDescription ED;
        ED << "The factory for the physicslist ["
           << physicsListName
           << "] does not exist!"
           << G4endl;
        G4Exception("EDepSimCreatePhysicsList",
                    "EDepSimCreatePhysicsList001", FatalException, ED);
        exit(42);
  }
} // anonymous namespace


G4VModularPhysicsList* EDepSim::CreatePhysicsList(G4String physicsListName) {

    G4VModularPhysicsList* physList = nullptr;

#ifdef EDEPSIM_USE_PYTHIA8
    // this isn't ideal, but for now ...
    G4PhysListRegistry* plreg = G4PhysListRegistry::Instance();
    plreg->AddPhysicsExtension("PY8TAUDK","Py8TauDecayerPhysics");
    plreg->AddPhysicsExtension("PY8CHARMDK","Py8CharmDecayerPhysics");
#endif

/*
    G4LossTableManager::Instance();
    defaultCutValue  = 1.*mm;
    fCutForGamma     = defaultCutValue;
    fCutForElectron  = defaultCutValue;
    fCutForPositron  = defaultCutValue;
*/

    g4alt::G4PhysListFactory plfactory;

    G4String defaultPhysListName = "QGSP_BERT";
    plfactory.SetDefaultReferencePhysList(defaultPhysListName);

    // Check to see if the physics list has been over ridden from the
    // environment variable PHYSLIST
    char* envlist = std::getenv("PHYSLIST");
    if (envlist) {
        physList = plfactory.ReferencePhysList();
        G4String plname = G4String("$PHYSLIST=")+G4String(envlist);
        if ( ! physList ) FailedToCreatePhysicsList(plname);
    }

    // Check if a list name was provided on the command line.
    // It usually is not provided.
    if ( ! physList && physicsListName.size() > 1 ) {
      // use the factory to parse the string
      physList = plfactory.GetReferencePhysList(physicsListName);
      if ( ! physList ) FailedToCreatePhysicsList(physicsListName);
    }

    // So, neither $PHYSLIST nor command line were given
    if ( ! physList ) {
      physList = plfactory.GetReferencePhysList(defaultPhysListName);
      if ( ! physList ) FailedToCreatePhysicsList(defaultPhysListName);
    }

    if ( ! physList ) {
      EDepSimThrow("No physics list was created.");
    }

    // special stuff
    EDepSim::ExtraPhysics* fExtra = new EDepSim::ExtraPhysics();
    physList->RegisterPhysics(fExtra);
    physList->RegisterPhysics(new G4OpticalPhysics());

    //handle UI messenges
    gCreatePhysicsListMessenger =
              new EDepSim::CreatePhysicsListMessenger(physList,fExtra);

    return physList;
}
