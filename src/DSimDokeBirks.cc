#include "G4ParticleTypes.hh" //lets you refer to G4OpticalPhoton, etc.
#include "G4EmProcessSubType.hh" //lets you call this process Scintillation
#include "G4Version.hh" //tells you what Geant4 version you are running

#include "DSimDokeBirks.hh"

// The physics in DSimDokeBirks is shamelessly stolen from NEST, so cite the NEST
// paper to give credit where credit is due.  However, all of the bugs are
// mine, so there should be a note like "Simplified implementation of the
// physics described in NEST paper".

DSimDokeBirks::DSimDokeBirks(const G4String& processName,
                       G4ProcessType type)
    : G4VRestDiscreteProcess(processName, type)
{
    SetProcessSubType(fScintillation);
    
    if (verboseLevel>0) {
        G4cout << GetProcessName() << " is created " << G4endl;
    }

    fElectricField = 500*volt/cm;
    
}

DSimDokeBirks::~DSimDokeBirks () {} //destructor needed to avoid linker error

    
// This routine simply calls the equivalent PostStepDoIt since all the
// necessary information resides in aStep.GetTotalEnergyDeposit()
G4VParticleChange*
DSimDokeBirks::AtRestDoIt(const G4Track& aTrack, const G4Step& aStep)
{
    return DSimDokeBirks::PostStepDoIt(aTrack, aStep);
}

G4VParticleChange*
DSimDokeBirks::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
// this is the most important function, where all light & charge yields happen!
{
    aParticleChange.Initialize(aTrack);
        
    const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
    const G4Material* aMaterial = aTrack.GetMaterial();

    // Check that we are in liquid argon.
    if (aMaterial->GetName() != "Argon_Liquid") {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);        
    }
    
    const G4ParticleDefinition *pDef = aParticle->GetDefinition();
    G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
    G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
    G4double totalEnergyDeposit = aStep.GetTotalEnergyDeposit();

    if (totalEnergyDeposit <= 0.0) {
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);        
    }
    
    // The code below is pulled from G4S1Light and is simplified to be
    // Doke-Birks only, in LAr only, and for an electric field only.  This is
    // for ARGON only.  The Doke-Birks constants are in volt/cm
    G4double dokeBirks[3];
    G4double electricField = std::abs(fElectricField/(volt/cm));
    dokeBirks[0] = 0.07*pow((electricField),-0.85);
    dokeBirks[2] = 0.00;
    dokeBirks[1] = dokeBirks[0]/(1-dokeBirks[2]); //B=A/(1-C) (see paper)

    G4double dE = totalEnergyDeposit/MeV;
    G4double dx = aStep.GetStepLength()/cm;
    G4double density = aMaterial->GetDensity()/(g/cm3);
    G4double LET = 0.0; //lin. energy transfer (prop. to dE/dx)
    if (dx != 0.0) LET = (dE/dx)*(1/density);
    G4double recombProb = (dokeBirks[0]*LET)/(1+dokeBirks[1]*LET)+dokeBirks[2];
    recombProb *= density/1.393;

    G4double ScintillationYield = 1 / (19.5*eV);
    G4double NumQuanta = ScintillationYield*totalEnergyDeposit;
    G4double ExcitationRatio = 0.21; //Aprile et. al book
    G4double NumExcitons = NumQuanta*ExcitationRatio/(1+ExcitationRatio);
    G4double NumIons = NumQuanta - NumExcitons;
    G4double NumPhotons = NumExcitons + NumIons*recombProb;

    // The number of photons and electrons for this step has been calculated
    // using the Doke/Birks law version of recombination.  For DETSIM we are
    // only looking at argon, and according the Matt both Doke-Birks and
    // Thomas-Imel fit the data.  It's also because we are not interested in
    // extremely low energies, so Thomas-Imel makes a negligible correction.
    G4double nonIonizingEnergy = totalEnergyDeposit;
    nonIonizingEnergy *= NumPhotons/NumQuanta; 
    aParticleChange.ProposeNonIonizingEnergyDeposit(nonIonizingEnergy);
    return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

// GetMeanFreePath
// ---------------
G4double DSimDokeBirks::GetMeanFreePath(const G4Track&,
                                     G4double ,
                                     G4ForceCondition* condition)
{
    *condition = StronglyForced;
    // Force the DSimDokeBirks physics process to always happen, even if it's
    // not the dominant process.  In effect it is a meta-process on top of any
    // and all other energy depositions which may occur, just like the
    // original G4Scintillation (disregard DBL_MAX, this function makes the
    // mean free path zero really, not infinite)

    return DBL_MAX;
}

// GetMeanLifeTime
// ---------------
G4double DSimDokeBirks::GetMeanLifeTime(const G4Track&,
                                     G4ForceCondition* condition)
{
    *condition = Forced;
    // This function and this condition has the same effect as the above
    return DBL_MAX;
}
