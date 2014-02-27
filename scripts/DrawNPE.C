
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// This plot will draw the number of photo electrons for data and //
// simulation in order to determine that shift of data w.r.t.     //
// the simulated events.                                          //
// -------------------------------------------------------------- //
// NOTE: Right now just plotting data because I don't have MC     //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//


// My plotting tools
#include "../scripts/tools.C"

#include "TCut.h"

// Some constants for plots
int nbins = 10000;
double bmin = 0;
double bmax = 1.e6;
int lumi = 0;

//----------------------------------------------------//
// Main
//----------------------------------------------------//
void DrawNPE(int option)
{

  //
  // Specify files
  //

  vector<TFile*> files;
  TString savename = "";
  if(option == 0){
    cout<<"Drawing 1% luminosity"<<endl;
    files.push_back(new TFile("../trees/SC2_allNearbyDOM_SC1per_DOMcalib_before.tree.root"));
    //files.push_back(new TFile("../trees/SC2_spicemie_hole100cm_1per.root"));
    files.push_back(new TFile("../../SCAna_oldLumi/trees/SC2_spicemie_hole100cm_1per.root"));
    nbins = 250;
    bmin = 54e3;
    //bmax = 80e3;
    bmax = 120e3;
    savename = "SC2_comparison_1per.png";
    lumi = 1;
  }
  else if(option == 1){
    cout<<"Drawing 3% luminosity"<<endl;
    files.push_back(new TFile("../trees/SC2_allNearbyDOM_SC3per_DOMcalib_before.tree.root"));
    //files.push_back(new TFile("../trees/SC2_spicemie_hole100cm_3per.root"));
    files.push_back(new TFile("../../SCAna_oldLumi/trees/SC2_spicemie_hole100cm_3per.root"));
    nbins = 200;
    bmin = 90e3;
    //bmax = 110e3;
    bmax = 200e3;
    savename = "SC2_comparison_3per.png";
    lumi = 3;
  }
  else if(option == 2){
    cout<<"Drawing 10% luminosity"<<endl;
    files.push_back(new TFile("../trees/SC2_allNearbyDOM_SC10per_DOMcalib_before.tree.root"));
    //files.push_back(new TFile("../trees/SC2_spicemie_hole100cm_10per.root"));
    files.push_back(new TFile("../../SCAna_oldLumi/trees/SC2_spicemie_hole100cm_10per.root"));
    nbins = 200;
    bmin = 175e3;
    //bmax = 220e3;
    bmax = 400e3;
    savename = "SC2_comparison_10per.png";
    lumi = 10;
  }
  else if(option == 3){
    cout<<"Drawing 30% luminosity"<<endl;
    files.push_back(new TFile("../trees/SC2_allNearbyDOM_SC30per_DOMcalib_before.tree.root"));
    //files.push_back(new TFile("../trees/SC2_spicemie_hole100cm_30per.root"));
    files.push_back(new TFile("../../SCAna_oldLumi/trees/SC2_spicemie_hole100cm_30per.root"));
    nbins = 200;
    bmin = 320e3;
    //bmax = 450e3;
    bmax = 900e3;
    savename = "SC2_comparison_30per.png";
    lumi = 30;
  }
  else if(option == 4){
    cout<<"Drawing 51% luminosity"<<endl;
    files.push_back(new TFile("../trees/SC2_allNearbyDOM_SC51per_DOMcalib_before.tree.root"));
    //files.push_back(new TFile("../trees/SC2_spicemie_hole100cm_51per.root"));
    files.push_back(new TFile("../../SCAna_oldLumi/trees/SC2_spicemie_hole100cm_51per.root"));
    nbins = 100;
    bmin = 400e3;
    //bmax = 500e3;
    bmax = 700e3;
    savename = "SC2_comparison_51per.png";
    lumi = 51;
  }
  else if(option == 5){
    cout<<"Drawing 100% luminosity"<<endl;
    files.push_back(new TFile("../trees/SC2_allNearbyDOM_SC100per_DOMcalib_before.tree.root"));
    //files.push_back(new TFile("../trees/SC2_spicemie_hole100cm_100per.root"));
    files.push_back(new TFile("../../SCAna_oldLumi/trees/SC2_spicemie_hole100cm_100per.root"));
    nbins = 200;
    bmin = 650e3;
    bmax = 900e3;
    savename = "SC2_comparison_100per.png";
    lumi = 100;
  }
  else{
    cout<<endl;
    cout<<"Option: "<<option<<" is not currently supported"<<endl;
    cout<<"The current choices are:"<<endl;
    cout<<"\t0 -- 1% luminosity"<<endl;
    cout<<"\t1 -- 3% luminosity"<<endl;
    cout<<"\t2 -- 10% luminosity"<<endl;
    cout<<"\t3 -- 30% luminosity"<<endl;
    cout<<"\t4 -- 51% luminosity"<<endl;
    cout<<"\t5 -- 100% luminosity"<<endl;
    cout<<endl;
    return;
  }

  vector<TString> treeNames;
  treeNames.push_back("RealTree");
  treeNames.push_back("JulietTree");

  vector<TString> fnames;
  fnames.push_back("SC2 Data");
  fnames.push_back("SC2 Spice-Mie");
  
  //
  // Load the trees
  //

  vector<TTree*> trees;
  cout<<"*********************************************"<<endl;
  for(unsigned int f=0; f<files.size(); ++f){
    trees.push_back( (TTree*) files.at(f)->Get(treeNames.at(f).Data()) );
    TTree* temp = trees.back();
    cout<<"File: "<<fnames.at(f)<<" has "<<temp->GetEntries()<<endl;
  }
  cout<<"*********************************************"<<endl;

  // Plot NPE
  plot(trees, fnames, savename);

}

//----------------------------------------------------//
// Plotting method.
// Currently only one now, but could expand.
//----------------------------------------------------//
void plot(vector<TTree*> trees, vector<TString> fnames, TString savename)
{
  
  // Define requirements
  TCut minimum_dom           = "DetectorResponseEvent_.totalNumberOfDom_>=100";
  TCut minimum_npe           = "DetectorResponseEvent_.totalBestEstimatedNPE_>=10.0";
  TCut basecut               = (minimum_dom&&minimum_npe);
  
  // Variable to plot
  TString var = "(DetectorResponseEvent_.totalBestEstimatedNPE_)";

  // Canvas
  TCanvas* c = makeCanvas("c");

  // Vector to hold histograms
  vector<TH1D*> histograms;

  // Latex object
  TLatex* m_lat = new TLatex();
  m_lat->SetTextSize(0.04);
  m_lat->SetNDC();
  m_lat->SetTextFont(42);
  m_lat->SetTextColor(kBlack);

  // Legend
  TLegend* leg = makeLegend(0.6,0.9,0.7,0.9);

  // Loop over trees and load
  float maximum = -999;
  for(unsigned int it=0; it<trees.size(); ++it){
    TString hname = "h_"+fnames.at(it);
    TH1D* temp = new TH1D(hname.Data(),"",nbins,bmin,bmax);
    setAtt(temp, "Best Estimated NPE", "Normalized", m_colors[it], m_markers[it]);
    trees.at(it)->Draw((var + ">>" + hname).Data(), (basecut), "hist");
    float scale = temp->Integral();
    temp->Scale(1/scale);
    if( maximum < temp->GetMaximum() )
      maximum = temp->GetMaximum();
    histograms.push_back(temp);
    leg->AddEntry(temp, fnames.at(it).Data(), "l");
  }
  
  // Now loop and plot
  histograms.at(0)->SetMaximum(maximum*1.2);
  histograms.at(0)->Draw();
  for(unsigned int ih=1; ih<histograms.size(); ++ih)
    histograms.at(ih)->Draw("same");

  // Draw Legend
  leg->Draw("same");

  // Add Decorations
  m_lat->DrawLatex(0.16,0.88,"#bf{IC86 Preliminary}");
  m_lat->DrawLatex(0.16,0.83,Form("Luminosity: %i%s",lumi,"%"));

  // Save
  //c->SaveAs(("../plots/"+savename).Data());
  c->SaveAs(("../plots/oldLumi_"+savename).Data());

  // Dump the peak information. Assumes Data is first
  // if you want Data/MC.  Otherwise you may have to 
  // modify how this is done to get Data/MC
  double max0 = getMeanNPE(histograms.at(0));
  double max1 = getMeanNPE(histograms.at(1));
  cout<<endl;
  cout<<"----------------------------------"<<endl;
  cout<<fnames.at(0)<<" \t"<<max0<<endl;
  cout<<fnames.at(1)<<" \t"<<max1<<endl;
  cout<<"Ratio: \t"<<Form("%1.3f",max0/max1)<<endl;

  
  cout<<"----------------------------------"<<endl;
  cout<<endl;
}

//----------------------------------------------------//
// Get Mean NPE number
//----------------------------------------------------//
double getMeanNPE(TH1D* hist)
{

  //return hist->GetBinCenter(hist->GetMaximumBin());
  return hist->GetMean();

}
