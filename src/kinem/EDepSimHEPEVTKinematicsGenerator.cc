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
    const G4String& name, const G4String& fileName, 
    const G4String& flavor, int verbosity)
    : EDepSim::VKinematicsGenerator(name), fGenerator(NULL),
      fFileName(fileName), fFlavor(flavor), 
      fVerbosity(verbosity) {}

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

    EDepSimLog("Using HEPEVT input flavor " << fFlavor);

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
    // For pythia flavor input, the number of particle in the event 
    // is the only token.
    case 1:
        lines = AsInteger(tokens[0]);
        break;
    // For marley flavor input, there are two numbers: the event 
    // number (largely irrelevant) and the number of particles in 
    // the event. Here, we want the latter for the number of particles.
    // The marley flavor also defines the vertex position and generation
    // time, so set those here as well.
    case 2:
        lines = AsInteger(tokens[1]);
        break;
    case 5:
        vtxX = AsReal(tokens[1])*cm;
        vtxY = AsReal(tokens[2])*cm;
        vtxZ = AsReal(tokens[3])*cm;
        vtxT = AsReal(tokens[4])*ns;
        lines = AsInteger(tokens[0]);
        break;
    case 15:
        EDepSimLog("HEPEVT Case 15");
        vtxX   = AsReal(tokens[11])*cm;
        vtxY   = AsReal(tokens[12])*cm;
        vtxZ   = AsReal(tokens[13])*cm;
        EDepSimLog("HEPEVT Vertex: " << vtxX << ", " << vtxY << ", " << vtxZ);
        vtxT   = AsReal(tokens[14])*ns;
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
    EDepSimLog("HEPEVT Looping " << lines << " lines");
    for (int line = 0; line < lines; ++line) {
        EDepSimLog("HEPEVT On line " << line);
        EDepSimLog("HEPEVT Tokens size " << tokens.size() );
        if (!GetTokens(tokens)) {
            EDepSimError("Input truncated in " << fFileName
                       << " after " << fCurrentLine << " lines");
            throw EDepSim::NoMoreEvents();
        }
        if (tokens.size() != 8 && fFlavor == "pythia") {
            EDepSimError("Invalid input at " << fFileName
                         << ":" << fCurrentLine
                         << " -- Wrong number of entries on line");
            throw EDepSim::NoMoreEvents();
        }
        if (tokens.size() != 15 && fFlavor == "marley") {
            EDepSimError("Invalid input at " << fFileName
                         << ":" << fCurrentLine
                         << " -- Wrong number of entries on line");
            throw EDepSim::NoMoreEvents();
        }
        int status = AsInteger(tokens[0]);
        if (status != 1) {
            EDepSimLog("HEPEVT Skipping line with status " << status);
            continue;
        }
        int pid       = AsInteger(tokens[1]);
        //int mother1, mother2, daughter1, daughter2;
        double momX, momY, momZ;
        //double mass, energy;
        // Read in pythia (8-number) input flavor
        // This is the default used in Geant4; see 
        // https://apc.u-paris.fr/~franco/g4doxy4.10/html/class_g4_h_e_p_evt_interface.html
        if (fFlavor == "pythia") {
            //daughter1 = AsInteger(tokens[2]);  // ignored, here for docu.
            //daughter2 = AsInteger(tokens[3]); // ignored, here for docu.
            momX   = AsReal(tokens[4])*GeV;
            momY   = AsReal(tokens[5])*GeV;
            momZ   = AsReal(tokens[6])*GeV;
            //mass = AsReal(tokens[7])*GeV;  // ignored, here for docu.
        }
        // Read Marley (15-number) input flavor, also used by LArSoft's
        // TextFileGen_module.cc; see
        // https://internal.dunescience.org/doxygen/TextFileGen__module_8cc_source.html
        else if (fFlavor == "marley") {
            EDepSimLog("HEPEVT Reading marley line " << line);
            //mother1   = AsInteger(tokens[2]); // ignored, here for docu.
            //mother2   = AsInteger(tokens[3]); // ignored, here for docu.
            //daughter1 = AsInteger(tokens[4]); // ignored, here for docu.
            //daughter2 = AsInteger(tokens[5]); // ignored, here for docu.
            momX   = AsReal(tokens[6])*GeV;
            momY   = AsReal(tokens[7])*GeV;
            momZ   = AsReal(tokens[8])*GeV;
            EDepSimLog("HEPEVT Momentum: " << momX << ", " << momY << ", " << momZ);
            //energy = AsReal(tokens[9])*GeV;   // ignored, here for docu.
            //mass   = AsReal(tokens[10])*GeV;  // ignored, here for docu.
            vtxX   = AsReal(tokens[11])*cm;
            vtxY   = AsReal(tokens[12])*cm;
            vtxZ   = AsReal(tokens[13])*cm;
            EDepSimLog("HEPEVT Vertex: " << vtxX << ", " << vtxY << ", " << vtxZ);
            vtxT   = AsReal(tokens[14])*ns;
            EDepSimLog("HEPEVT T0: " << vtxT);
            vertex->SetPosition(vtxX, vtxY, vtxZ);
            vertex->SetT0(vtxT);
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
        }
        else {
            // TODO Write a proper exception for this case
            EDepSimError("Unrecognized HEPEVT input flavor " << fFlavor);
            throw EDepSim::NoMoreEvents();
        }

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
