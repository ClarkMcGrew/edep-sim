#ifndef Py8TauDecayerPhysics_H
#define Py8TauDecayerPhysics_H

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

class G4Decay;

class Py8TauDecayerPhysics: public G4VPhysicsConstructor
{
  public:
  Py8TauDecayerPhysics(G4int verb=1);
    virtual ~Py8TauDecayerPhysics();

  protected:
    // methods
          // construct particle and physics
    virtual void ConstructParticle();
    virtual void ConstructProcess();

  private:

/*
    // DO WE NEED TO IMPLEMENT THESE ?
    /// Not implemented
    Py8TauDecayerPhysics(const Py8TauDecayerPhysics& right);
    /// Not implemented
    Py8TauDecayerPhysics& operator=(const Py8TauDecayerPhysics& right);
*/

};



#endif
