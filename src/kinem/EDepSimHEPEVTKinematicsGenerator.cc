#include "kinem/EDepSimHEPEVTKinematicsGenerator.hh"
#include "EDepSimException.hh"


#include <G4Event.hh>
#include <G4SystemOfUnits.hh>
#include <G4LorentzVector.hh>
#include <G4PrimaryVertex.hh>

#ifdef USE_G4HEPEvtInterface
#include <G4HEPEvtInterface.hh>
#endif

#include <memory>

EDepSim::HEPEVTKinematicsGenerator::HEPEVTKinematicsGenerator(
    const G4String& name, const G4String& fileName, int verbosity)
    : EDepSim::VKinematicsGenerator(name), fGenerator(NULL),
      fFileName(fileName), fVerbosity(verbosity) { }

EDepSim::HEPEVTKinematicsGenerator::~HEPEVTKinematicsGenerator() {
#ifdef USE_G4HEPEvtInterface
    if (fGenerator) delete fGenerator;
#endif
}

int EDepSim::HEPEVTKinematicsGenerator::GetTokens(
    std::vector<std::string>& tokens) {

    tokens.clear();
    if (!fInput.is_open() || fInput.eof()) {
        EDepSimError("No more events: " << fFileName);
        return 0;
    }

    std::string line;
    while (std::getline(fInput,line)) {
        ++fCurrentLine;
        // Strip out comments.
        line = line.substr(0,line.find_first_of("#"));
        // Break into tokens.
        std::istringstream input(line);
        std::string token;
        tokens.clear();
        while (input >> token) tokens.push_back(token);
        if (!tokens.empty()) return tokens.size();
    }

    EDepSimLog("No more data in " << fFileName);
    return 0;
}

int EDepSim::HEPEVTKinematicsGenerator::AsInteger(const std::string& token) {
    std::istringstream input(token);
    int value;
    input >> value;
    std::string trailer;
    if (input >> trailer) {
        EDepSimThrow(fFileName << ":" << fCurrentLine
                     << " -- Invalid integer: |" << token<< "|");
    }
    return value;
}

double EDepSim::HEPEVTKinematicsGenerator::AsReal(const std::string& token) {
    std::istringstream input(token);
    double value;
    input >> value;
    std::string trailer;
    if (input >> trailer) {
        EDepSimThrow(fFileName << ":" << fCurrentLine
                     << " -- Invalid real: |" << token << "|");
    }
    return value;
}

EDepSim::VKinematicsGenerator::GeneratorStatus
EDepSim::HEPEVTKinematicsGenerator::GeneratePrimaryVertex(
    G4Event* evt, const G4LorentzVector& /* position */) {
#ifdef USE_G4HEPEvtInterface
    if (!fGenerator) {
        fGenerator = new G4HEPEvtInterface(fFileName,fVerbosity);
    }
    try {
        fGenerator->GeneratePrimaryVertex(evt);
    }
    catch (...) {
        EDepSimError("No more events.");
        throw EDepSim::NoMoreEvents();
    }
#else
    if (!fInput.is_open()) {
        fInput.open(fFileName,std::ifstream::in);
        fCurrentLine = 0;
        if (!fInput.is_open()) {
            EDepSimThrow("File not open: " << fFileName);
        }
        EDepSimLog("Open HEPEVT file: " << fFileName);
    }

    std::vector<std::string> tokens;
    int lines = 0;
    double vtxX = 0;
    double vtxY = 0;
    double vtxZ = 0;
    double vtxT = 0;

    // Parse the first line of an event.  It can have either 1 integer
    // (i.e. the standard syntax for the file), or 1 integer and 4 reals (my
    // special extension).  The 4 reals are the position of the vertex in
    // cm (xyz) and ns (t).
    if (!GetTokens(tokens)) {
        EDepSimLog("No more events in " << fFileName
                   << " after " << fCurrentLine << " lines");
        throw EDepSim::NoMoreEvents();
    }
    switch (tokens.size()) {
    case 1:
        lines = AsInteger(tokens[0]);
        break;
    case 5:
        vtxX = AsReal(tokens[1])*cm;
        vtxY = AsReal(tokens[2])*cm;
        vtxZ = AsReal(tokens[3])*cm;
        vtxT = AsReal(tokens[4])*ns;
        lines = AsInteger(tokens[0]);
        break;
    default:
        EDepSimError("Syntax error at " << fFileName << ":" << fCurrentLine);
        throw EDepSim::NoMoreEvents();
    }

    if (fVerbosity>0) {
        EDepSimNamedLog("HEPEVT", "Reading " << lines << " particles");
    }
    if (fVerbosity>1) {
        EDepSim::LogManager::IncreaseIndentation();
        EDepSimNamedLog("HEPEVT",
                        "Vertex "
                        << vtxX << " mm, "
                        << vtxY << " mm, "
                        << vtxZ << " mm, "
                        << vtxT << " ns" );
        EDepSim::LogManager::DecreaseIndentation();
    }

    std::unique_ptr<G4PrimaryVertex> vertex(
        new G4PrimaryVertex(vtxX,vtxY,vtxZ,vtxT));

    // Parse the particle lines for the event.  This loses some of the context
    // information for now and only adds the particles that should be tracked.
    for (int line = 0; line < lines; ++line) {
        if (!GetTokens(tokens)) {
            EDepSimError("Input truncated in " << fFileName
                       << " after " << fCurrentLine << " lines");
            throw EDepSim::NoMoreEvents();
        }
        if (tokens.size() != 8) {
            EDepSimError("Invalid input at " << fFileName
                         << ":" << fCurrentLine
                         << " -- Wrong number of entries on line");
            throw EDepSim::NoMoreEvents();
        }
        int status = AsInteger(tokens[0]);
        if (status != 1) continue;
        int pid = AsInteger(tokens[1]);
        // int daugter1 = AsInteger(tokens[2]);  // ignored, here for docu.
        // int daughter2 = AsInteger(tokens[3]); // ignored, here for docu.
        double momX = AsReal(tokens[4])*GeV;
        double momY = AsReal(tokens[5])*GeV;
        double momZ = AsReal(tokens[6])*GeV;
        // double mass = AsReal(tokens[7])*GeV;  // ignored, here for docu.
        std::unique_ptr<G4PrimaryParticle> part(
            new G4PrimaryParticle(pid, momX, momY, momZ));
        vertex->SetPrimary(part.release());
        if (fVerbosity>1) {
            EDepSim::LogManager::IncreaseIndentation();
            EDepSimNamedLog("HEPEVT",
                            "Primary "
                            << pid << " " << momX << " MeV/c, "
                            << momY << " MeV/c, "
                            << momZ << " MeV/c" );
            EDepSim::LogManager::DecreaseIndentation();
        }
    }
    evt->AddPrimaryVertex(vertex.release());
#endif

    return kSuccess;
}
