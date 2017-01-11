#ifndef DSimBuilder_hh_Seen
#define DSimBuilder_hh_Seen

#include "DSimUserDetectorConstruction.hh"
#include "DSimException.hh"

#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4RotationMatrix.hh>
#include <G4VisAttributes.hh>

#include <iostream>
#include <map>

/// A Base for class to build elements of the detector.  The method get
/// GetPiece() should construct a new unplaced element that is facing up along
/// the Z axis.  The caller will then then rotate the object into position and
/// and place the object into an existing mother volume.  These are designed
/// to be called "recursively" with one builder calling another builder.  A
/// builder will usually use one or more sub-builders to make any new piece.
///
/// The builders should follow a single naming convention.  All builder
/// classes should be named "DSim<thename>Builder".  When builders are used in
/// code, they are declared using the AddBuilder() method which takes a name
/// (string) which is used to reference the builder, and to make macro
/// commands to control the object.  The builder name should start with an
/// upper case letter.
///
/// Most builders (all should) will define a messenger.  The command names are
/// build with the CommandName() method.  All command names should begin with
/// a lower case letter.
class DSimBuilder {
public:
    DSimBuilder(G4String n, DSimUserDetectorConstruction* c);
    DSimBuilder(G4String n, DSimBuilder* parent);
    virtual ~DSimBuilder();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void) = 0;

    /// Return the base name of the object that this builds.
    G4String GetName(void);

    /// Return the base name of the object that this builds.
    G4String GetLocalName(void) {return fLocalName;};

    /// Set the base name of the logical volume that this builds.
    void SetLocalName(const G4String& name);

    /// Set the relative opacity of the constructed object.
    void SetOpacity(double v);
    
    /// Get the relative opacity of the constructed object.
    double GetOpacity(void) const {return fOpacity;}

    /// Set the relative opacity of the object daughters
    void SetDaughterOpacity(double v);
    
    /// Return the detector construction that is building this piece.
    DSimUserDetectorConstruction* GetConstruction(void) {
        return fConstruction;
    };
    
    /// Set the sensitive detector for this component.
    virtual void SetSensitiveDetector(G4VSensitiveDetector* s) {
        fSensitiveDetector = s;
    }
    
    /// Get the sensitive detector for this component.
    virtual G4VSensitiveDetector* GetSensitiveDetector(void) {
        return fSensitiveDetector;
    }

    /// Set the sensitive detector for this component by name.
    virtual void SetSensitiveDetector(G4String name, G4String type);
    
    /// Set the maximum sagitta for a track being added to a single hit
    /// segment.
    virtual void SetMaximumHitSagitta(double sagitta);

    /// Set the maximum length of a single hit segment.
    virtual void SetMaximumHitLength(double length);

    /// Return the messenger for this constructor
    G4UImessenger* GetMessenger(void) {return fMessenger;};

    /// Set the messenger for this constructor.
    void SetMessenger(G4UImessenger* m) {
        fMessenger = m;
    };

    /// Add a new sub constructor to the current geometry constructor.  This
    /// should be done in the derived class ctor.  All geometry constructors
    /// that will be used by the derived class should be added using this
    /// method.
    void AddBuilder(DSimBuilder* c) {
        if (fSubBuilders.find(c->GetLocalName()) 
            != fSubBuilders.end()) {
            std::cout << "Multiply defined constructor name " 
                      << c->GetName()
                      << std::endl;
            DSimThrow("DSimBuilder::AddBuilder(): Multiple defines");
        }
        fSubBuilders[c->GetLocalName()] = c;
    }

    /// Get a sub constructor by name and do the dynamic cast.  This will
    /// abort with an error message if you request an undefined name.
    template <class T> T& Get(G4String n) {
        std::map<G4String,DSimBuilder*>::iterator p 
            = fSubBuilders.find(n);
        if (p==fSubBuilders.end()) {
            std::cout << "Error in " << GetName() << std::endl;
            std::cout << "   Undefined constructor requested "
                      << n << std::endl;
            std::cout << "   The choices are: " << std::endl;
            for (p = fSubBuilders.begin();
                 p != fSubBuilders.end();
                 ++p) {
                std::cout << "        \"" << (*p).first << "\""
                          << " for constructor: " << (*p).second->GetName()
                          << std::endl;
            }
            DSimThrow(" Undefined constructor");
        }
        T* c = dynamic_cast<T*>((*p).second);
        if (!c) {
            std::cout << "Error in " << GetName() << std::endl;
            std::cout << "  Cannot type cast " << n 
                      << " to requested class" << std::endl;
            DSimThrow("DSimBuilder::Get<>:"
                        " Bad typecast");
        }
        return *c;
    }

    /// Find a sub constructor by name and do the dynamic cast.  This returns
    /// a pointer that will be NULL if you request an undefined name.
    template <class T> T* Find(G4String n) {
        std::map<G4String,DSimBuilder*>::iterator p 
            = fSubBuilders.find(n);
        if (p==fSubBuilders.end()) return NULL;
        T* c = dynamic_cast<T*>((*p).second);
        return c;
    }

    /// This returns true if the interior objects should be checked for
    /// overlaps.
    bool Check() {return fCheck;}

    /// Set the check value.
    void SetCheck(bool v) {fCheck = v;}

protected:
    G4Material* FindMaterial(G4String m);

    /// Takes logical volume and returns the visual attributes.  The optional
    /// argument specifies the log of the relative opacity used for this
    /// specific set of attributes.  If the relative opacity is 0, then the
    /// default alpha is used.  If the relative opacity is positive, then the
    /// alpha is increased, and if it's negative, the alpha is decreased.  If
    /// the relative opacity is 10, then the object will have an alpha of 1.0.
    /// If it's -10, then the object will have an alpha of zero
    /// (i.e. invisible).
    G4VisAttributes GetColor(G4LogicalVolume* volume, double opacity = 0.0);

    /// Takes a material and returns the attributes.  The optional argument
    /// specifies the log of the relative opacity used for this specific set
    /// of attributes.  If the relative opacity is 0, then the default alpha
    /// is used.  If the relative opacity is positive, then the alpha is
    /// increased, and if it's negative, the alpha is decreased.  If the
    /// relative opacity is 10, then the object will have an alpha of 1.0.  If
    /// it's -10, then the object will have an alpha of zero (i.e. invisible).
    G4VisAttributes GetColor(G4Material* volume, double opacity = 0.0);

private:
    /// The short local name of the constructor.
    G4String fLocalName;

    /// The name of the constructor.
    G4String fName;

    /// The G4VUserDetectorConstruction class that this is cooperating with.
    DSimUserDetectorConstruction* fConstruction;

    /// The parent of this constructor
    DSimBuilder* fParent;

    /// The user interface messenger that will provide values for this class. 
    G4UImessenger* fMessenger;

    /// The sensitive detector for this tracking component.
    G4VSensitiveDetector* fSensitiveDetector;

    /// The relative opacity of the constructed object.
    double fOpacity;

    /// If this is true, then check the constructed object for overlaps.
    bool fCheck;

    /// The sub constructors that might be used in this class.
    std::map<G4String,DSimBuilder*> fSubBuilders;

};

class DSimBuilderMessenger: public G4UImessenger {
private:
    DSimBuilder* fBuilder;

    /// The UI directory for this messenger.
    G4UIdirectory* fDirectory;

    /// The directory name for this messenger
    G4String fDirName;

    G4UIcmdWithADouble*        fOpacityCMD;
    G4UIcmdWithADouble*        fDaughterOpacityCMD;
    G4UIcmdWithABool*          fCheckCMD;
    G4UIcommand*               fSensitiveCMD;
    G4UIcmdWithADoubleAndUnit* fMaximumHitSagittaCMD;
    G4UIcmdWithADoubleAndUnit* fMaximumHitLengthCMD;

public:
    DSimBuilderMessenger(DSimBuilder* c, const char* guide=NULL);
    virtual ~DSimBuilderMessenger();

    /// Return the name of the directory.
    G4String GetDirectory(void) {return fDirName;};

    /// Build a command name with the directory prefix.
    G4String CommandName(G4String cmd) {
        G4String name = GetDirectory() + cmd;
        return name;
    };

    void SetNewValue(G4UIcommand *cmd, G4String val);
};
#endif
