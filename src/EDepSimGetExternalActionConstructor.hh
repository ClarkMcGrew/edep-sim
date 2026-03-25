#ifndef EDepSimGetExternalActionConstructor_hh_seen
#define EDepSimGetExternalActionConstructor_hh_seen

#include <string>
#include <EDepSimLog.hh>

namespace EDepSim {

    /// A function to find a function in an shared library that will create the
    /// external constructor. This returns a nullptr if there is a problem
    /// finding the function, or it doesn't exist.  The user will need to cast
    //. the returned pointer before it can be called.
    void* GetExternalActionConstructor(std::string path, std::string symbol);

    /// Call an external constructor and return the result.  The constructor
    /// must have the signature `T* (*)(const char* option)`.  If there is a
    /// problem, the returned object pointer will be nullptr.
    template<typename T>
    T* CallExternalConstructor(std::string path,
                               std::string symbol,
                               std::string option) {
        EDepSimLog("Calling an external constructor");
        EDepSim::LogManager::IncreaseIndentation();
        EDepSimLog("Shared Library: " << path);
        EDepSimLog("Symbol:         " << symbol);
        EDepSimLog("Option String:  " << option);

        T* (*constructor)(const char*)
            = reinterpret_cast<T* (*)(const char*)>(
                GetExternalActionConstructor(path,symbol));
        if (constructor == nullptr) {
            EDepSimError("External constructor not found at"
                         << " " << path
                         << "::" << symbol
                         << "(const char*)");
            return nullptr;
        }
        EDepSim::LogManager::DecreaseIndentation();

        return constructor(option.c_str());
    }
};
#endif
