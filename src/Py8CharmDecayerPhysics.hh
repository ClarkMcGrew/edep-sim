#ifndef Py8CharmDecayerPhysics_H
#define Py8CharmDecayerPhysics_H

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

class G4Decay;

class Py8CharmDecayerPhysics: public G4VPhysicsConstructor
{
  public:
  Py8CharmDecayerPhysics(G4int verb=1);
    virtual ~Py8CharmDecayerPhysics();

  protected:
    // methods
          // construct particle and physics
    virtual void ConstructParticle();
    virtual void ConstructProcess();

  private:

/*
    // DO WE NEED TO IMPLEMENT THESE ?
    /// Not implemented
    Py8CharmDecayerPhysics(const Py8CharmDecayerPhysics& right);
    /// Not implemented
    Py8CharmDecayerPhysics& operator=(const Py8CharmDecayerPhysics& right);
*/

};



#endif
