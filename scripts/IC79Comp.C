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
void IC79Comp(int option)
{

  
  if(option == 0){
    cout<<endl;
    cout<<"***************************************"<<endl;
    cout<<"Plotting Data NPE vs MC NPE"<<endl;
    cout<<"***************************************"<<endl;
    cout<<endl;
    plotDatavsMC();
  }
  else if(option == 1){
    cout<<endl;
    cout<<"***************************************"<<endl;
    cout<<"Plotting NPE distributions for IC79 and IC86"<<endl;
    cout<<"***************************************"<<endl;
    cout<<endl;
    plotNPEComparison(false);
    //plotNPEComparison(true);
  }
  else{
    cout<<"Option not supported: "<<option<<endl;
    cout<<"Options are: "<<endl;
    cout<<"\t0 -- Data vs MC NPE comparison"<<endl;
    cout<<"\t1 -- Comparing NPE shapes"<<endl;
    return;
  }

}

//------------------------------------------------------//
// Plot Data vs MC NPE for My results and Keiichi's
//------------------------------------------------------//
void plotDatavsMC()
{

  // Load my graph dynamically from all of my Trees
  TGraphErrors* gr_IC86 = getMyGraph();
  setAtt(gr_IC86, kBlue, 20);

  // Copy graph with a naive correction
  TGraphErrors* gr_IC86_corr = getCorrectedGraph(gr_IC86);
  setAtt(gr_IC86_corr, kMagenta, 22);

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
  gr_IC86_corr->Draw("sameep");
  
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
  leg->AddEntry(gr_IC86_corr, "IC86 (naive correction)","ep");
  leg->SetHeader("#bf{Preliminary}");
  leg->Draw("same");
  
  //c->SaveAs("../plots/IC79_IC86_Comparison.png");
  c->SaveAs("../plots/IC79_IC86_Comparison_wCor.png");

}

//------------------------------------------------------//
// Plot NPE comparison
//------------------------------------------------------//
void plotNPEComparison(bool isData)
{

  // Luminosity settings
  vector<TString> lumis;
  //lumis.push_back("1");
  lumis.push_back("3");
  //lumis.push_back("10");
  //lumis.push_back("30");
  //lumis.push_back("51");
  //lumis.push_back("100");

  // Make canvas
  TCanvas* c = makeCanvas("c");
  
  // Tree name
  TString tName = isData ? "RealTree" : "JulietTree";

  // Define Cuts
  TCut nDomCut    = "DetectorResponseEvent_.totalNumberOfDom_>=100";
  TCut bestNPE    = "DetectorResponseEvent_.totalBestEstimatedNPE_>=10.0"; 
  TCut base       = (nDomCut && bestNPE);
  TCut less79     = "AtwdResponseChannels_.string_ <= 79"; 
  TCut greater79  = "AtwdResponseChannels_.string_ > 79"; 
  base = "";
  
  // Variable to draw:
  //TString var = "DetectorResponseEvent_.totalBestEstimatedNPE_";
  TString var = "DetectorResponseBaseTimeWindowEvent_.totalBestEstimatedNPE_";
  //TString var = "AtwdResponseChannels_.estimatedNPE_";

  // Loop plot and save
  for(unsigned int i=0; i<lumis.size(); ++i){
    
    TString lumi = lumis.at(i);

    // Get tree name
    TString fname = buildName(lumi, isData);
    TFile* file   = new TFile(("../trees/"+fname).Data());

    // Get Mase's tree name
    TString f_mName = buildMaseName(lumi, isData);
    TFile* f_mFile  = new TFile(f_mName.Data());

    // Get Trees
    TTree* tree   = (TTree*) file->Get(tName.Data());
    TTree* mTree  = (TTree*) f_mFile->Get("JulietTree"); // always same name for Mase

    // Make histograms
    TH1F* h_less79    = makeBestHist("less79",lumi);
    setAtt(h_less79, "Best Estimate NPE", "Normalized", kBlack, 20);
    TH1F* h_greater79 = makeBestHist("greater79",lumi);
    setAtt(h_greater79, "Best Estimate NPE", "Normalized", kBlue, 25);
    TH1F* h_mase79 = makeBestHist("mase79", lumi);
    setAtt(h_mase79, "Best Estimate NPE", "Normalized", kRed, 22);
    
    // Draw into histograms
    tree->Draw((var + " >> less79_"+lumi).Data(), (base&&less79), "p");
    tree->Draw((var + " >> greater79_"+lumi).Data(), (base&&greater79), "p");
    mTree->Draw((var + " >> mase79_"+lumi).Data(), (base), "p");
    

    // Scale and get maximum
    h_less79->Scale(1/h_less79->Integral());
    h_greater79->Scale(1/h_greater79->Integral());
    h_mase79->Scale(1/h_mase79->Integral());
    cout<<h_mase79->GetMean()<<endl;
    if(h_less79->GetMaximum() < h_greater79->GetMaximum())
      h_less79->SetMaximum(h_greater79->GetMaximum() * 1.2);
    if(h_less79->GetMaximum() < h_mase79->GetMaximum())
      h_less79->SetMaximum(h_mase79->GetMaximum() * 1.2);

    // Draw
    h_less79->Draw("");
    h_greater79->Draw("same");
    h_mase79->Draw("same");

  }// end loop
}

//------------------------------------------------------//
// Get best histogram for different binnings
//------------------------------------------------------//
TH1F* makeBestHist(TString name, TString percent)
{
  TString hname = name +"_"+ percent;

  if( percent == "1" )   return new TH1F(hname.Data(),"",250,54e3,80e3);
  if( percent == "3" )   return new TH1F(hname.Data(),"",200,90e3,110e3);
  if( percent == "10" )  return new TH1F(hname.Data(),"",200,175e3,220e3);
  if( percent == "30" )  return new TH1F(hname.Data(),"",200,320e3,450e3);
  if( percent == "51" )  return new TH1F(hname.Data(),"",100,400e3,500e3);
  if( percent == "100" ) return new TH1F(hname.Data(),"",200,650e3,900e3);

  return new TH1F(hname.Data(),"",500,54e3,1000e3);  

}

//------------------------------------------------------//
// Get tree names
//------------------------------------------------------//
TString buildName(TString percent, bool isData)
{

  if(isData){
    TString fname = "SC2_allNearbyDOM_SC";
    fname += percent;
    fname += "per_DOMcalib_before.tree.root";
    return fname;
  }
  
  TString fname = "SC2_spicemie_hole100cm_";
  fname += percent;
  fname += "per.root";
  return fname;

}

//------------------------------------------------------//
// Build file name for Mase-san's list
//------------------------------------------------------//
TString buildMaseName(TString percent, bool isData)
{

  if(isData){
    TString fname = "/misc/home/mase/work/I3/analysis/SC/process/V04-02-00RC1/SC2/data/";
    fname += percent +"Per/SC2_data" + percent + "per_.root";
    return fname;
  }
  
  TString fname = "/misc/home/mase/work/I3/analysis/SC/process/V04-02-00RC1/SC2/SPICEMIE/";
  fname += percent +"Per/SC2_spicemie_hole100cm_" + percent + "per.root";
  return fname;

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
  TString var = "(DetectorResponseBaseTimeWindowEvent_.totalBestEstimatedNPE_)";
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

//------------------------------------------------------//
// Get graph with naive correction
//------------------------------------------------------//
TGraphErrors* getCorrectedGraph(TGraphErrors* gr)
{

  // data correction
  double x_sf[5] = {0.96, 0.94, 0.91, 0.89, 0.85};
  
  // mc correction
  double y_sf[5] = {0.97, 0.97, 0.95, 0.94, 0.91};

  TGraphErrors* clone = gr->Clone("corrected_gr");
  double x = 0;
  double y = 0;
  int npoints = clone->GetN();
  for(int ip=0; ip<npoints; ++ip){
    clone->GetPoint(ip, x, y);
    clone->SetPoint(ip, x*x_sf[ip], y*y_sf[ip]);
  }

  return clone;

}

