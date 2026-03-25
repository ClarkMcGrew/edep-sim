/////////////////////////////////////////////////////////////////////
// A function to find a function in an shared library that will create
// the external constructor. This returns a nullptr if there is a problem
// finding the function, or it doesn't exist.  The user will need to cast the
// returned pointer before it can be called.

#include "EDepSimGetExternalActionConstructor.hh"
#include "EDepSimLog.hh"

#include <dlfcn.h>
#include <wordexp.h>

void* EDepSim::GetExternalActionConstructor(std::string path,
                                            std::string symbol) {
    // Expand an environment variables in the path.
    std::string libraryPath;
    wordexp_t expanded;
    if (wordexp(path.c_str(), &expanded, WRDE_NOCMD)) {
        EDepSimError("Library path syntax error: Invalid path name");
        return nullptr;
    }
    for (std::size_t i = 0; i<expanded.we_wordc; ++i) {
        libraryPath += expanded.we_wordv[i];
    }
    wordfree(&expanded);

    EDepSimLog("Full path:      " << libraryPath);

    // Get the library. This doesn't need to check if the library is already
    // loaded because dlopen takes care of that.
    void* handle = dlopen(libraryPath.c_str(),RTLD_LAZY);
    if (handle == nullptr) {
        EDepSimError(dlerror());
        return nullptr;
    }
    dlerror();

    void* constructor = dlsym(handle,symbol.c_str());

    return constructor;
}
