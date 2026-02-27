#include "G4HadronPhysicsQGSP_BERT_Bias.hh"
#include "G4BertiniPiKBuilderBias.hh"

#include <G4PiKBuilder.hh>
#include <G4QGSPPiKBuilder.hh>
#include <G4FTFPPiKBuilder.hh>
//#include <G4BertiniPiKBuilder.hh>

G4HadronPhysicsQGSP_BERT_Bias::G4HadronPhysicsQGSP_BERT_Bias(G4int)
    : G4HadronPhysicsQGSP_BERT("hInelastic_pion_bias QGSP_BERT_Bias",true)
  {}


void G4HadronPhysicsQGSP_BERT_Bias::Pion()
{
  auto pik = new G4PiKBuilder;
  AddBuilder(pik);
  auto qgs = new G4QGSPPiKBuilder(QuasiElasticQGS);
  AddBuilder(qgs);
  qgs->SetMinEnergy(minQGSP_pik);
  pik->RegisterMe(qgs);
  auto ftf = new G4FTFPPiKBuilder(QuasiElasticFTF);
  AddBuilder(ftf);
  ftf->SetMinEnergy(minFTFP_pik);
  ftf->SetMaxEnergy(maxFTFP_pik);
  pik->RegisterMe(ftf);
  auto bert = new G4BertiniPiKBuilderBias(fPiPlusBias, fKPlusBias);
  //auto bert = new G4BertiniPiKBuilder;
  AddBuilder(bert);
  bert->SetMinEnergy(minBERT_pik);
  bert->SetMaxEnergy(maxBERT_pik);
  pik->RegisterMe(bert);
  pik->Build();
}
