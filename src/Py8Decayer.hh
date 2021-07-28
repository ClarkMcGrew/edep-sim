#ifndef Py8Decayer_H
#define Py8Decayer_H

#include "G4VExtDecayer.hh"
#include "globals.hh"

#include "Pythia8/Pythia.h"

// fwd declaration
//
//class Pythia8
//{
//   Pythia;
//}
class G4Track;
class G4DecayProducts;

class Py8Decayer : public G4VExtDecayer
{
  
   public:

      //ctor & dtor
      Py8Decayer();
      virtual ~Py8Decayer();

      virtual G4DecayProducts* ImportDecayProducts(const G4Track&);
    
   private:
   
/*
      // Do we need to implement the following ???
      /// Not implemented
      Py8Decayer(const Py8Decayer& right);
      /// Not implemented
      Py8Decayer& operator=(const Py8Decayer& right);
*/
      // data members
      Pythia8::Pythia* fDecayer;

};

#endif
