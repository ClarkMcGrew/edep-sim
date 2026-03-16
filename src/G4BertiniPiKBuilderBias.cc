#include "G4BertiniPiKBuilderBias.hh"
#include <G4SystemOfUnits.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4ProcessManager.hh>
#include <G4BGGPionInelasticXS.hh>
#include <G4ComponentGGHadronNucleusXsc.hh>
#include <G4CrossSectionInelastic.hh>
#include <G4HadronicParameters.hh>


G4BertiniPiKBuilderBias::
G4BertiniPiKBuilderBias(double pion_plus_bias, double kaon_plus_bias)
  : fPionPlusBias(pion_plus_bias), fKaonPlusBias(kaon_plus_bias) {
   kaonxs = new G4CrossSectionInelastic( new G4ComponentGGHadronNucleusXsc );
   theMin = 0.0;
   theMax = G4HadronicParameters::Instance()->GetMaxEnergyTransitionFTF_Cascade();
   theModel = new G4CascadeInterface;
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax); 
 }

void G4BertiniPiKBuilderBias::
Build(G4PionPlusInelasticProcess * aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->AddDataSet( new G4BGGPionInelasticXS( G4PionPlus::Definition() ) );
   std::cout << "Biasing PiPlus: " << fPionPlusBias << std::endl;
   aP->MultiplyCrossSectionBy(fPionPlusBias);
   aP->RegisterMe(theModel);
 }

void G4BertiniPiKBuilderBias::
Build(G4PionMinusInelasticProcess * aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->AddDataSet( new G4BGGPionInelasticXS( G4PionMinus::Definition() ) );
   aP->RegisterMe(theModel);
 }

void G4BertiniPiKBuilderBias::
Build(G4KaonPlusInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->AddDataSet(kaonxs);
   std::cout << "Multiplying kaon by " << fKaonPlusBias << std::endl;
   aP->MultiplyCrossSectionBy(fKaonPlusBias);
 }

void G4BertiniPiKBuilderBias::
Build(G4KaonMinusInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->AddDataSet(kaonxs);
 }

void G4BertiniPiKBuilderBias::
Build(G4KaonZeroLInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->AddDataSet(kaonxs);
 }

void G4BertiniPiKBuilderBias::
Build(G4KaonZeroSInelasticProcess * aP)
 {
   aP->RegisterMe(theModel);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->AddDataSet(kaonxs);
 }
