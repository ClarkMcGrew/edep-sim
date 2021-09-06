#ifndef EDepSim_HEPEVTKinematicsGenerator_hh_Seen
#define EDepSim_HEPEVTKinematicsGenerator_hh_Seen

#include <globals.hh>
#include <G4LorentzVector.hh>

#include "kinem/EDepSimVKinematicsGenerator.hh"

#include <string>
#include <fstream>

class G4Event;
class G4VPrimaryGenerator;

/// A class to read a HEPEVT file.  This normally uses an internal processor,
/// but can be forced (using #defines) to use the G4HEPEvtInterface.
namespace EDepSim {class HEPEVTKinematicsGenerator;}
class EDepSim::HEPEVTKinematicsGenerator :
    public EDepSim::VKinematicsGenerator {
public:
    HEPEVTKinematicsGenerator(const G4String& name,
                              const G4String& fileName,
                              int verbosity = 0);
    virtual ~HEPEVTKinematicsGenerator();

    /// Add a primary vertex to the event.
    virtual GeneratorStatus
    GeneratePrimaryVertex(G4Event* evt,
                          const G4LorentzVector& position);

private:
    /// The primary generator when G4HEPEvtInterface is used (usually NULL).
    G4VPrimaryGenerator* fGenerator;

    /// The name of the input file (used for error messages)
    std::string fFileName;

    /// The number of lines read from the input file (used for error messages)
    int fCurrentLine;

    /// The verbosity of the output
    int fVerbosity;

    /// The input stream to be read.
    std::ifstream fInput;

    /// Get the next line of "tokens" from the input file.  This returns the
    /// number of tokens that were read from the line, and zero when the file
    /// is empty.  Comments in the file are prefixed by "#", and tokens are
    /// separated by white space.  The expected format is
    /// \code
    /// token1 token2 token3 and so on # comments
    /// \endcode
    /// Lines that are empty, or only have comments are skipped,
    int GetTokens(std::vector<std::string>& tokens);

    /// Parse a token as an integer.  This will throw an error if the string
    /// is not a valid integer.
    int AsInteger(const std::string& token);

    /// Parse a token as a real number This will throw an error if the string
    /// is not a valid floating point number.
    double AsReal(const std::string& token);
};
#endif
