//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// This script will make a quick comparison of Data Vs. MC    //
// for Keiichi's points from IC79 and my preliminary results  //
// from IC86.                                                 //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "../scripts/tools.C"

// range to be plotted
float xmin = 5.e4;
float xmax = 1.e6;

//------------------------------------------------------//
// Main
//------------------------------------------------------//
void IC79Comp()
{

  // Load my graph dynamically from all of my Trees
  TGraphErrors* gr_IC86 = getMyGraph();
  setAtt(gr_IC86, kBlue, 20);
  
  // Keiichi's results are hard-coded from his script
  double npe_data[10] = {9.51576e+04, 1.76736e+05, 3.24418e+05, 3.89808e+05, 6.72254e+05};
  double npe_error_data[10] = {2.60323e+01, 4.85111e+01, 7.64120e+01, 1.06911e+02, 2.09812e+02};
  double npe_mc_spicemie[10] = { 9.82845e+04, 1.86842e+05, 3.41306e+05, 4.08625e+05, 6.58026e+05};
  double npe_error_mc_spicemie[10] = { 1.25089e+01, 1.92964e+01, 3.35455e+01, 4.20338e+01, 5.11765e+01};
  
  TGraphErrors* gr_IC79 = new TGraphErrors(6, npe_data, npe_mc_spicemie,
					   npe_error_data, npe_error_mc_spicemie);
  
  setAtt(gr_IC79, kRed, 25);

  // Make Canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogy();
  c->SetLogx();

  // Make a TH1F frame
  TH1F* frame = makeFrame("frame",2, xmin, xmax, "total NPE (data)",
			  "total NPE (Spice-Mie)");
  frame->SetMinimum(xmin);
  frame->SetMaximum(xmax);
  frame->Draw();
  
  // Draw graphs
  gr_IC86->Draw("sameep");
  gr_IC79->Draw("sameep");
  
  // Make a y=x line
  TF1* f = new TF1("f","x",xmin,xmax);
  f->SetLineColor(kBlack);
  f->SetLineStyle(2);
  f->Draw("same");

  // Make a legend
  TLegend* leg = makeLegend(0.15,0.35,0.7,0.92);
  leg->AddEntry(f, "y = x", "l");
  leg->AddEntry(gr_IC86, "IC86 (M. Relich)", "ep");
  leg->AddEntry(gr_IC79, "IC79 (K. Mase)", "ep");
  leg->SetHeader("#bf{Preliminary}");
  leg->Draw("same");

  c->SaveAs("../plots/IC79_IC86_Comparison.png");

}

//------------------------------------------------------//
// Get IC86 results
//------------------------------------------------------//
TGraphErrors* getMyGraph()
{
  
  // For this loop over the input files and store the mean
  // and the error on the mean for data and simulation
  int nPoints = 0;
  double data_npe[10];
  double data_err[10];
  double mc_npe[10];
  double mc_err[10];

  // Set Data
  vector<TString> d_files;
  //d_files.push_back("../trees/SC2_allNearbyDOM_SC1per_DOMcalib_before.tree.root");
  d_files.push_back("../trees/SC2_allNearbyDOM_SC3per_DOMcalib_before.tree.root");
  d_files.push_back("../trees/SC2_allNearbyDOM_SC10per_DOMcalib_before.tree.root");
  d_files.push_back("../trees/SC2_allNearbyDOM_SC30per_DOMcalib_before.tree.root");
  d_files.push_back("../trees/SC2_allNearbyDOM_SC51per_DOMcalib_before.tree.root");
  d_files.push_back("../trees/SC2_allNearbyDOM_SC100per_DOMcalib_before.tree.root");

  // Set MC
  vector<TString> mc_files;
  //mc_files.push_back("../trees/SC2_spicemie_hole100cm_1per.root");
  mc_files.push_back("../trees/SC2_spicemie_hole100cm_3per.root");
  mc_files.push_back("../trees/SC2_spicemie_hole100cm_10per.root");
  mc_files.push_back("../trees/SC2_spicemie_hole100cm_30per.root");
  mc_files.push_back("../trees/SC2_spicemie_hole100cm_51per.root");
  mc_files.push_back("../trees/SC2_spicemie_hole100cm_100per.root");

  if(mc_files.size() != d_files.size()){
    cout<<"Don't have equal number of data and mc files"<<endl;
    cout<<"Program will crash"<<endl;
    return NULL;
  }
  
  // Variables for plotting
  TString var = "(DetectorResponseEvent_.totalBestEstimatedNPE_)";
  TCut minimum_dom           = "DetectorResponseEvent_.totalNumberOfDom_>=100";
  TCut minimum_npe           = "DetectorResponseEvent_.totalBestEstimatedNPE_>=10.0";
  TCut basecut               = (minimum_dom&&minimum_npe);
  
  for(unsigned int i=0; i<d_files.size(); ++i){
    
    // Get mean err for data
    getMeanAndError(d_files.at(i), "RealTree", var, basecut, data_npe[i], data_err[i]);
    // Get mean err for mc
    getMeanAndError(mc_files.at(i), "JulietTree", var, basecut, mc_npe[i], mc_err[i]);

    nPoints++;
  }
  
  // Make TGraph
  TGraphErrors* gr = new TGraphErrors(nPoints, data_npe, mc_npe, data_err, mc_err);
  
  return gr;

}

//------------------------------------------------------//
// Get Mean and error
//------------------------------------------------------//
void getMeanAndError(TString fname, TString treeName, TString var,
		     TCut cuts,double &mean, double &error)		    
{

  // Get histogram
  TFile* file = new TFile(fname.Data());
  TTree* tree = (TTree*) file->Get(treeName.Data());
  TH1F* h = new TH1F("npe","",(int) xmax-xmin, xmin, xmax);  
  tree->Draw((var + ">> npe").Data(), cuts, "hist");
  
  // Save mean and error
  mean  = h->GetMean();
  error = h->GetMeanError();

  // Now clean up
  delete h;
  delete tree;
  delete file;
  
}


