#ifndef EDepSim_Exception_hh_seen
#define EDepSim_Exception_hh_seen
#include <EDepSimLog.hh>
#include <exception>

namespace EDepSim {class Exception;}
/// The standard exception from detsim.  This is usually thrown by the
/// EDepSimThrow macro.
class EDepSim::Exception : public std::exception {
public:
    Exception() {}
    virtual ~Exception() throw() {}
    const char* what(void) const throw() {return "EDepSim::Exception";}
};

/// Print an error message, and then throw an exception.
#define EDepSimThrow(message) {EDepSimError(message);throw EDepSim::Exception();} 
#endif
