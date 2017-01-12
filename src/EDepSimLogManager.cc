#include <vector>
#include <sstream>
#include <fstream>
#include <time.h>

#include "EDepSimLog.hh"

EDepSim::LogManager::ErrorPriority EDepSim::LogManager::fErrorPriority = EDepSim::LogManager::ErrorLevel;
EDepSim::LogManager::LogPriority EDepSim::LogManager::fLogPriority = EDepSim::LogManager::LogLevel;
std::ostream* EDepSim::LogManager::fDebugStream = NULL;
std::ostream* EDepSim::LogManager::fLogStream = NULL;
std::map<std::string,EDepSim::LogManager::ErrorPriority> EDepSim::LogManager::fErrorTraces;
std::map<std::string,EDepSim::LogManager::LogPriority> EDepSim::LogManager::fLogTraces;
int EDepSim::LogManager::fIndentation = 0;

EDepSim::LogManager::~LogManager() { }

void EDepSim::LogManager::SetDebugLevel(const char* trace, 
                              EDepSim::LogManager::ErrorPriority level) {
    fErrorTraces[trace] = level;
}

EDepSim::LogManager::ErrorPriority EDepSim::LogManager::GetDebugLevel(const char* trace) {
    std::map<std::string,ErrorPriority>::iterator elem = fErrorTraces.find(trace);
    if (elem == fErrorTraces.end()) return fErrorPriority;
    return elem->second;
}

namespace {
    std::string MakeTimeStamp() {
        std::string stamp = "Unknown Time";
        time_t t = time(NULL);
        struct tm *local = localtime(&t);
        if (!local) return stamp;
        char localTime[80];
        if (!strftime(localTime,sizeof(localTime),"%c",local)) return stamp;
        struct tm *utc = gmtime(&t);
        if (!utc) return stamp;
        char utcTime[80];
        if (!strftime(utcTime,sizeof(utcTime),"%Y-%m-%d %H:%M (UTC)",utc)) 
            return stamp;
        stamp = localTime;
        stamp += " [";
        stamp += utcTime;
        stamp += "]";
        return stamp;
    }
}

void EDepSim::LogManager::SetDebugStream(std::ostream* err) {
    EDepSim::LogManager::fDebugStream = err;
    if (!fDebugStream) return;
    std::ofstream* ofile = dynamic_cast<std::ofstream*>(err);
    if (ofile && !(ofile->is_open())) {
        fDebugStream = NULL;
        EDepSimSevere("Debug stream is not open.");
    }
    *fDebugStream << std::endl
                  << "##################################################" 
                  << std::endl
                  << "# ERROR LOG STARTS AT: " << MakeTimeStamp()
                  << std::endl
                  << "##################################################" 
                  << std::endl
                  << std::endl;
}

std::ostream& EDepSim::LogManager::GetDebugStream() {
    if (!EDepSim::LogManager::fDebugStream ) return GetLogStream();
    return *EDepSim::LogManager::fDebugStream;
}
    
void EDepSim::LogManager::SetLogLevel(const char* trace, 
                            EDepSim::LogManager::LogPriority level) {
    fLogTraces[trace] = level;
}

EDepSim::LogManager::LogPriority EDepSim::LogManager::GetLogLevel(const char* trace) {
    std::map<std::string,LogPriority>::iterator elem = fLogTraces.find(trace);
    if (elem == fLogTraces.end()) return fLogPriority;
    return elem->second;
}

void EDepSim::LogManager::SetLogStream(std::ostream* log) {
    EDepSim::LogManager::fLogStream = log;
    if (!fLogStream) return;
    std::ofstream* ofile = dynamic_cast<std::ofstream*>(log);
    if (ofile && !(ofile->is_open())) {
        fLogStream = NULL;
        EDepSimSevere("Log stream is not open.");
    }
    *fLogStream << std::endl
                << "##################################################" 
                << std::endl
                << "# LOG STARTS AT: " << MakeTimeStamp()
                << std::endl
                << "##################################################" 
                << std::endl
                << std::endl;
}

std::ostream& EDepSim::LogManager::GetLogStream() {
    if (!EDepSim::LogManager::fLogStream) return std::cout;
    return *EDepSim::LogManager::fLogStream;
}

void EDepSim::LogManager::SetIndentation(int i) {
    EDepSim::LogManager::fIndentation = std::max(i,0);
}

void EDepSim::LogManager::IncreaseIndentation() {
    ++EDepSim::LogManager::fIndentation;
}

void EDepSim::LogManager::DecreaseIndentation() {
    if (EDepSim::LogManager::fIndentation>0) --EDepSim::LogManager::fIndentation;
}
    
void EDepSim::LogManager::ResetIndentation() {
    EDepSim::LogManager::fIndentation = 0;
}


std::string EDepSim::LogManager::MakeIndent() {
    if (fIndentation<1) return "";
    std::string indent = "";
    for (int i=0; i<fIndentation; ++i) {
        indent += "..";
    }
    indent += " ";
    return indent;
}

namespace {
    bool TranslateLogLevel(const std::string& name, 
                           EDepSim::LogManager::LogPriority& level) {
        if (name == "QuietLevel") {
            level = EDepSim::LogManager::QuietLevel;
            return true;
        }
        if (name == "LogLevel") {
            level = EDepSim::LogManager::LogLevel;
            return true;
        }
        if (name == "InfoLevel") {
            level = EDepSim::LogManager::InfoLevel;
            return true;
        }
        if (name == "VerboseLevel") {
            level = EDepSim::LogManager::VerboseLevel;
            return true;
        }
        return false;
    }

    bool TranslateErrorLevel(const std::string& name, 
                             EDepSim::LogManager::ErrorPriority& level) {
        if (name == "SilentLevel") {
            level = EDepSim::LogManager::SilentLevel;
            return true;
        }
        if (name == "ErrorLevel") {
            level = EDepSim::LogManager::ErrorLevel;
            return true;
        }
        if (name == "SevereLevel") {
            level = EDepSim::LogManager::SevereLevel;
            return true;
        }
        if (name == "WarnLevel") {
            level = EDepSim::LogManager::WarnLevel;
            return true;
        }
        if (name == "DebugLevel") {
            level = EDepSim::LogManager::DebugLevel;
            return true;
        }
        if (name == "TraceLevel") {
            level = EDepSim::LogManager::TraceLevel;
            return true;
        }
        return false;
    }

    std::ostream* StreamPointer(const std::string& name) {
        if (name == "STDCOUT") return &std::cout;
        if (name == "STDCERR") return &std::cerr;
        if (name[0] != '"') return NULL;
        if (name[name.size()-1] != '"') return NULL;
        std::string file = name.substr(1,name.size()-2);
        std::ofstream* output = new std::ofstream(file.c_str(),
                                                  std::ios::out|std::ios::app);
        if (output->is_open()) return output;
        return NULL;
    }

    bool ReadConfigurationFile(const char* config) {
        std::ifstream input(config);
        if (!input.is_open()) return false;

        int inputLine = 0;
        for (;;) {
            std::string line;
            std::getline(input,line);
            if (input.eof()) break;

            // Save the current line number and cache the value so error
            // messages can be printed later.
            std::string cache(line);
            ++inputLine;
            
            // Strip the comments out of the file.
            std::string::size_type position = line.find("#");
            if (position != std::string::npos) line.erase(position);

            // Strip the white space at the beginning of the line.
            line.erase(0,line.find_first_not_of("\t "));

            // Skip lines that are too short.
            if (line.size()==0) continue;

            // Split the line into fields and a value.
            position = line.find("=");
            if (position == std::string::npos) {
                // Houston, we have a problem... There isn't a value.
                std::cerr << "WARNING: " << config << ":" << inputLine << ": "
                          << "Configuration line missing an '='"
                          << std::endl;
                std::cerr << "  Line: <" << cache << ">"
                          << std::endl;
                std::cerr << "  Configuration line has been skip" << std::endl;
                continue;
            }

            // Split the value off the end of the line.
            std::string value = line.substr(position+1);
            line.erase(position);

            // Strip the white space at the beginning of the value.
            value.erase(0,value.find_first_not_of("\t "));

            // Strip the white space at the end of the value.
            position = value.find_last_not_of("\t ");
            if (position != std::string::npos) value.erase(position+1);
            
            // Strip the white space at the end of the fields.
            position = line.find_last_not_of("\t ");
            if (position != std::string::npos) line.erase(position+1);
            
            // Split the remaining line in to fields.
            std::vector<std::string> fields;
            for (;;) {
                position = line.find(".");
                if (position == std::string::npos) {
                    fields.push_back(line);
                    break;
                }
                fields.push_back(line.substr(0,position));
                line.erase(0,position+1);
            } 
            
            // Process the fields and value.
            if (fields.size() == 2
                && fields[0] == "log"
                && fields[1] == "file") {
                // Set the log file name.
                std::ostream* str = StreamPointer(value);
                if (!str) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Cannot open log stream."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                EDepSim::LogManager::SetLogStream(str);
            }
            else if (fields.size() == 2
                     && fields[0] == "error"
                     && fields[1] == "file") {
                // Set the error file name.
                std::ostream* str = StreamPointer(value);
                if (!str) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Cannot open error stream."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                EDepSim::LogManager::SetDebugStream(str);
            }
            else if (fields.size() == 3
                     && fields[0] == "log"
                     && fields[1] == "default"
                     && fields[2] == "level") {
                // Set the default log level.
                EDepSim::LogManager::LogPriority level;
                if (!TranslateLogLevel(value,level)) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Unknown log level name."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                EDepSim::LogManager::SetLogLevel(level);
            }
            else if (fields.size() == 3
                     && fields[0] == "error"
                     && fields[1] == "default"
                     && fields[2] == "level") {
                // Set the default error level.
                EDepSim::LogManager::ErrorPriority level;
                if (!TranslateErrorLevel(value,level)) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Unknown error level name."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                EDepSim::LogManager::SetDebugLevel(level);
            }
            else if (fields.size() == 3
                     && fields[0] == "log"
                     && fields[2] == "level") {
                // Set the log level.
                EDepSim::LogManager::LogPriority level;
                if (!TranslateLogLevel(value,level)) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Unknown log level name."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                EDepSim::LogManager::SetLogLevel(fields[1].c_str(),level);
            }
            else if (fields.size() == 3
                     && fields[0] == "error"
                     && fields[2] == "level") {
                // Set the error level.
                EDepSim::LogManager::ErrorPriority level;
                if (!TranslateErrorLevel(value,level)) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Unknown error level name."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                EDepSim::LogManager::SetDebugLevel(fields[1].c_str(),level);
            }
            else {
                std::cerr << "WARNING: " << config << ":" << inputLine << ": "
                          << "Unknown command."
                          << std::endl;
                std::cerr << "  Line: <" << cache << ">"
                          << std::endl;
                std::cerr << "  Configuration line has been skip" << std::endl;
            }                
        }

        return true;
    }
}

void EDepSim::LogManager::Configure(const char* conf) {
    // Try to read a local configuration file.  
    ReadConfigurationFile("./edeplog.config");
    if (conf) {
        bool success = ReadConfigurationFile(conf);
        if (!success) EDepSimLog("EDepSim::Log configuration file was not read.");
    }
}
