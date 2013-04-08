#ifndef DSimException_hh_seen
#define DSimException_hh_seen
#include <TCaptLog.hxx>
#include <exception>

/// The standard exception from detsim.  This is usually thrown by the
/// DSimError macro.
class DSimException : public std::exception {
public:
    DSimException() {}
    virtual ~DSimException() throw() {}
    const char* what(void) const throw() {return "DSimException";}
};

/// Print an error message, and then throw an exception.
#define DSimError(message) {CaptError(message);throw DSimException();} 
#endif
