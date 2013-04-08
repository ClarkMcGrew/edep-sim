#include <sstream>

#include <globals.hh>

#include <TCaptLog.hxx>

#include "kinem/DSimVTimeFactory.hh"
#include "kinem/DSimSpillTimeFactory.hh"
#include "kinem/DSimSpillTimeGenerator.hh"

DSimSpillTimeFactory::DSimSpillTimeFactory(
    DSimUserPrimaryGeneratorMessenger* parent) 
    : DSimVTimeFactory("spill",parent),
      fSpillTime(100*ns), 
      fBunchSeparation(582*ns),
      fBunchLength(12.4*ns) {
    
    fSpillTimeCMD = new G4UIcmdWithADoubleAndUnit(CommandName("start"),this);
    fSpillTimeCMD->SetGuidance("Set the start time of the spill.");
    fSpillTimeCMD->SetParameterName("time",false);
    fSpillTimeCMD->SetUnitCategory("Time");

    fBunchSepCMD = new G4UIcmdWithADoubleAndUnit(CommandName("bunchSep"),this);
    fBunchSepCMD->SetGuidance("Set the time between bunch starts.");
    fBunchSepCMD->SetParameterName("time",false);
    fBunchSepCMD->SetUnitCategory("Time");

    fBunchLengthCMD = new G4UIcmdWithADoubleAndUnit(CommandName("bunchLength"),
                                                    this);
    fBunchLengthCMD->SetGuidance("Set the length of a bunch.");
    fBunchLengthCMD->SetParameterName("time",false);
    fBunchLengthCMD->SetUnitCategory("Time");

    fBunchCountCMD = new G4UIcmdWithAnInteger(CommandName("bunchCount"),
                                              this);
    fBunchCountCMD->SetGuidance("Set the number of bunchs in a spill.  "
                                "Reset bunch power to default.");
    fBunchCountCMD->SetParameterName("count",false);

    fBunchPowerCMD = new G4UIcommand(CommandName("bunchPower"),this);
    fBunchPowerCMD->SetGuidance("Set the power in a bunch "
                                "(bunchs are numbered from ONE)");
    G4UIparameter* bunchPar = new G4UIparameter('i');
    bunchPar->SetParameterName("bunch");
    fBunchPowerCMD->SetParameter(bunchPar);
    G4UIparameter* powerPar = new G4UIparameter('d');
    powerPar->SetParameterName("power");
    fBunchPowerCMD->SetParameter(powerPar);
    
    //Initialize the bunchs
    SetBunchCount(6);

}

DSimSpillTimeFactory::~DSimSpillTimeFactory() {
    delete fSpillTimeCMD;
    delete fBunchSepCMD;
    delete fBunchLengthCMD;
    delete fBunchCountCMD;
    delete fBunchPowerCMD;
}

void DSimSpillTimeFactory::SetNewValue(G4UIcommand* command,
                                           G4String newValue) {
    if (command == fSpillTimeCMD) {
        SetSpillTime(fSpillTimeCMD->GetNewDoubleValue(newValue));
    }
    else if (command == fBunchSepCMD) {
        SetBunchSeparation(fBunchSepCMD->GetNewDoubleValue(newValue));
    }
    else if (command == fBunchLengthCMD) {
        SetBunchLength(fBunchLengthCMD->GetNewDoubleValue(newValue));
    }
    else if (command == fBunchCountCMD) {
        SetBunchCount(fBunchCountCMD->GetNewIntValue(newValue));
    }
    else if (command == fBunchPowerCMD) {
        std::istringstream input((char*)newValue.c_str());
        int bunch;
        double power;
        input >> bunch >> power;
        SetBunchPower(bunch,power);
    }
}

DSimVTimeGenerator* DSimSpillTimeFactory::GetGenerator() {
    DSimVTimeGenerator* time = new DSimSpillTimeGenerator(
        GetName(),fSpillTime,fBunchSeparation,fBunchLength,fBunchPower);
    return time;
}

void DSimSpillTimeFactory::SetBunchCount(int bunchs) {
    fBunchPower.resize(bunchs);
    for (std::vector<double>::iterator b = fBunchPower.begin();
         b != fBunchPower.end();
         ++b) {
        *b = 1.0;
    }
}

void DSimSpillTimeFactory::SetBunchPower(int bunch, double power) {
    if (bunch<1 || (unsigned) bunch > fBunchPower.size()) {
        CaptError("Illegal bunch number: " << bunch);
        CaptError("Invalid bunch structure");
    }
    fBunchPower[bunch-1] = power;
}
