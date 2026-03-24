#ifndef EDepSimGetExternalActionConstructor_hh_seen
#define EDepSimGetExternalActionConstructor_hh_seen

#include <string>

namespace EDepSim {
    /////////////////////////////////////////////////////////////////////
    // A function to find a function in an shared library that will create the
    // external constructor. This returns a nullptr if there is a problem
    // finding the function, or it doesn't exist.  The user will need to cast
    // the returned pointer before it can be called.
    void* GetExternalActionConstructor(std::string path, std::string symbol);

};
#endif
