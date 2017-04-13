#ifdef __CINT__
#include "TG4Event.h"

#pragma link C++ class TG4PrimaryParticle+;
#pragma link C++ class std::vector<TG4PrimaryParticle>+;

#pragma link C++ class TG4PrimaryVertex+;
#pragma link C++ class std::vector<TG4PrimaryVertex>+;

#pragma link C++ class TG4TrajectoryPoint+;
#pragma link C++ class std::vector<TG4TrajectoryPoint>+;

#pragma link C++ class TG4Trajectory+;
#pragma link C++ class std::vector<TG4Trajectory>+;

#pragma link C++ class TG4HitSegment+;
#pragma link C++ class std::vector<TG4HitSegment>+;
#pragma link C++ class std::map<std::string,std::vector<TG4Trajectory> >+;

#pragma link C++ class TG4Event+;

#endif
