#ifndef DSimException_hh_seen
#define DSimException_hh_seen
#include <DSimLog.hh>
#include <exception>

/// The standard exception from detsim.  This is usually thrown by the
/// DSimThrow macro.
class DSimException : public std::exception {
public:
    DSimException() {}
    virtual ~DSimException() throw() {}
    const char* what(void) const throw() {return "DSimException";}
};

/// Print an error message, and then throw an exception.
#define DSimThrow(message) {DSimError(message);throw DSimException();} 
#endif
