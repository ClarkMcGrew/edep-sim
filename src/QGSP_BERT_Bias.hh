#ifndef QGSP_BERT_BIAS
#define QGSP_BERT_BIAS
#include <globals.hh>
#include <G4VModularPhysicsList.hh>

class QGSP_BERT_Bias: public G4VModularPhysicsList
{
public:
  QGSP_BERT_Bias(G4int ver = 1);
  virtual ~QGSP_BERT_Bias()=default;

  QGSP_BERT_Bias(const QGSP_BERT_Bias &) = delete;
  QGSP_BERT_Bias & operator=(const QGSP_BERT_Bias &)=delete;
  
};
#endif
