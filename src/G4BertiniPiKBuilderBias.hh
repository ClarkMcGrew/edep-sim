#ifndef G4BPKBB
#define G4BPKBB
#include <globals.hh>

#include <G4HadronElasticProcess.hh>
#include <G4HadronFissionProcess.hh>
#include <G4HadronCaptureProcess.hh>
#include <G4NeutronInelasticProcess.hh>
#include <G4VPiKBuilder.hh>
#include <G4CascadeInterface.hh>   


class G4BertiniPiKBuilderBias : public G4VPiKBuilder
{
  public: 
    G4BertiniPiKBuilderBias(double pion_plus_bias = 1.,
                            double kaon_plus_bias = 1.);
    virtual ~G4BertiniPiKBuilderBias() {}

    virtual void Build(G4HadronElasticProcess *) final override {}
    virtual void Build(G4PionPlusInelasticProcess * aP) final override;
    virtual void Build(G4PionMinusInelasticProcess * aP) final override;
    virtual void Build(G4KaonPlusInelasticProcess * aP) final override;
    virtual void Build(G4KaonMinusInelasticProcess * aP) final override;
    virtual void Build(G4KaonZeroLInelasticProcess * aP) final override;
    virtual void Build(G4KaonZeroSInelasticProcess * aP) final override;
    
    virtual void SetMinEnergy(G4double aM) final override {theMin = aM;}
    virtual void SetMaxEnergy(G4double aM) final override {theMax = aM;}

    using G4VPiKBuilder::Build; //Prevent Compilation warning

  private:
    G4VCrossSectionDataSet * kaonxs;
    G4CascadeInterface * theModel;    
    G4double theMin;
    G4double theMax;
    double fPionPlusBias;
    double fKaonPlusBias;
};
#endif
