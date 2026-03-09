#ifndef EDepSim_Backtrace_hh_seen
#define EDepSim_Backtrace_hh_seen

#include <ostream>

// Backtrace depends on execinfo.h being available.  This is only available on
// linux.
//
// Note: Rumor has it that this can be tested using CMake (FindBacktrace), but
// that has not been well tested (remove this comment if it gets implemented)!
//
// Note: Rumor has it that Macs have it after Mac OS X 10.5, but I can't test
// that so for now it's included.
#if defined(__linux__) || defined(__APPLE__) || defined(__MACH__)
#include <execinfo.h>
#include <cstdlib>
#else
// Backtrace is not available on non-linux builds
#endif

namespace EDepSim {
    // When possible, print a backtrace for the current call stack to the
    // standard output stream.
    template<class CharT, class Traits>
    std::basic_ostream<CharT,Traits>&
    Backtrace(std::basic_ostream<CharT, Traits>& out) {
#if defined(__linux__) || defined(__APPLE__) || defined(__MACH__)
        void *buffer[100];
        int nptrs = backtrace(buffer,100);
        out << "Backtrace of " << nptrs << " calls" << std::endl;

        // This uses malloc to allocate memory for strings, and passes
        // ownership to the caller.
        char **strings = backtrace_symbols(buffer,nptrs);
        if (strings == nullptr) {
            out << "Error generating backtrace" << std::endl;
            return out;
        }

        for (int i=0; i< nptrs; ++i) out << strings[i] << std::endl;

        // Free the strings pointer that was allocated with malloc.
        std::free(strings);
#else
        out << "Backtrace not available" << std::endl;
#endif
        return out;
    }
};

#endif
