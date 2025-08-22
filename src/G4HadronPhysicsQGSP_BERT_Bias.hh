#ifndef G4HPQBB
#define G4HPQBB
#include <G4HadronPhysicsQGSP_BERT.hh>

class G4HadronPhysicsQGSP_BERT_Bias : public G4HadronPhysicsQGSP_BERT {

  public:
    G4HadronPhysicsQGSP_BERT_Bias(G4int);
    void SetPiPlusBias(double val = 1.) {fPiPlusBias = val;}
    void SetKPlusBias(double val = 1.) {fKPlusBias = val;}
  protected:
    void Pion() override;

    double fPiPlusBias = 1.;
    double fKPlusBias = 1.;
};
#endif
