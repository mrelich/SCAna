//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// In Aya's script there are several hard coded values  //
// used for the filtering. I want to understand if they //
// are still appropriate for the latest SC run.         //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "../scripts/tools.C"
#include "TTree.h"
#include "TCut.h"
#include "TLine.h"

// Decide if we will look at SC1 or SC2
//bool isSC2 = true;
bool isSC2 = false;

// directory to save plots in
TString savedir = "../plots/Unfiltered/";

//------------------------------------------//
// Main
//------------------------------------------//
void DefineFilter(int opt)
{

  // Specify the file you want to examine
  TString dir = "../trees/";
  TFile* infile = NULL;
  if(isSC2) infile = new TFile(dir+"SC2_allNearbyDOM_SC100per_DOMcalib_before.tree.root");
  else      infile = new TFile(dir+"SC1_allNearbyDOM_SC100per_DOMcalib_before.tree.root");
  
  // Give the tree name here
  TString treename = "RealTree";

  // Load the tree
  TTree* tree = (TTree*) infile->Get(treename.Data());

  // update save dir
  if(!isSC2) savedir = "../plots/UnfilteredSC1/";

  // Options to run here:
  if(opt == 0)      checkUTCTimes(tree);
  else if(opt == 1) plotNDOM(tree);
  else if(opt == 2) checkNDOM(tree);
  else if(opt == 3) checkWaveformFormat();
  
  
}

//------------------------------------------//
// Problem 1: there is a function called
// utctimes(...).  I want to figure out 
// where I should place cuts to seperate
// the various luminosities
//------------------------------------------//
void checkUTCTimes(TTree* tree)
{

  // Variables to plot
  TString varx = "startUTCDaqTime_/1.e15"; // scale by 1e15
  TString vary = "DetectorResponseEvent_.totalBestEstimatedNPE_/1000.";
  //TString vary = "AtwdResponseChannels_.estimatedNPE_";

  // Make canvas
  TCanvas* c = makeCanvas("c");
  
  // Make a histogram to save
  float xmin = isSC2 ? 279.98 : 276.67;
  float xmax = isSC2 ? 280.15 : 276.84;
  float ymin = 0;
  float ymax = isSC2 ? 900 : 160;
  TH2F* frame = new TH2F("frame","",1, xmin,xmax,1,ymin,ymax);
  setAtt(frame,"Start DAQ Time / 10^{14}","Total Best Estimated NPE / 10^{3}");
  frame->Draw();

  // Draw from tree
  tree->Draw((vary+":"+varx).Data(),"","same");
  
  // Add constant functions of time
  vector<double> times = isSC2 ? getTimesSC2() : getTimesSC1();
  TLine* line = new TLine(); 
  line->SetLineWidth(1);
  line->SetLineColor(kBlue);
  for(unsigned int t=0; t<times.size(); ++t){
    double time = times.at(t);
    line->DrawLine(time,ymin,time,ymax);
    line->Draw("same");
  }

  // Save the plot
  TString pname = "timeVsAmp.png";
  c->SaveAs((savedir+pname).Data());
  
}

//------------------------------------------//
// Plot nDOM with out any other reqs.
//------------------------------------------//
void plotNDOM(TTree* tree)
{

  // Plot the number of DOM's per event
  TString var = "DetectorResponseEvent_.totalNumberOfDom_";

  // Make canvas
  TCanvas* c  = makeCanvas("c");

  // Create histogram holders
  int nbins = 100;
  int min   = 0;
  int max   = 2000;
  TString xtitle = "nDOMs";
  TString ytitle = "Entries";
  TH1F* hist = makeFrame("hist",nbins,min,max,
			 xtitle,ytitle);

  // Make legend
  TLegend* leg = makeLegend(0.7,0.8,0.8,0.9);
  leg->SetHeader("Luminosity");

  // Draw
  tree->Draw((var+">>hist").Data(),"","hist");

  // Add to legend
  leg->AddEntry(hist, "All", "l");

  // Now draw the figures
  hist->Draw("hist");
  leg->Draw("same");

  c->SaveAs((savedir+"nDoms_total.png").Data());
}

//------------------------------------------//
// Problem 2: There is a requirement that 
// at least 400 DOMs launch.  I want to look 
// at the average, and maybe also the average 
// for given event range.
//------------------------------------------//
void checkNDOM(TTree* tree)
{

  // Plot the number of DOM's per event
  TString var = "DetectorResponseEvent_.totalNumberOfDom_";

  // Make canvas
  TCanvas* c  = makeCanvas("c");

  // Make a vector of TCuts
  vector<TString> cuts;
  cuts.push_back("1");   // 1%
  cuts.push_back("3");   // 3%
  cuts.push_back("10");  // 10%
  cuts.push_back("30");  // 30%
  cuts.push_back("51");  // 51%
  cuts.push_back("100"); // 100%

  // Create histogram holders
  TH1F* hists[6];
  int nbins = 100;
  int min   = 0;
  int max   = 2000;
  TString xtitle = "nDOMs";
  TString ytitle = "Entries";
  
  // Make legend
  TLegend* leg = makeLegend(0.7,0.8,0.6,0.9);
  leg->SetHeader("Luminosity");

  // Loop and draw
  float maximum = -999;
  for(unsigned int i=0; i<cuts.size(); ++i){
    // get cut value
    TString cutval = cuts.at(i); 
    // create histogram
    hists[i] = makeFrame("hist_"+cutval,nbins,min,max,
			 xtitle,ytitle);

    // cut the TCut object from value
    TCut cut = lumiCut(cutval);

    // Draw and set att
    tree->Draw((var+">>hist_"+cutval).Data(),cut,"hist");
    setAtt(hists[i],xtitle,ytitle,m_colors[i],m_markers[i]);
    setMax(hists[i],maximum);

    // Add to legend
    leg->AddEntry(hists[i],(cutval+"%").Data(),"l");
  }

  // Now draw the figures
  hists[0]->SetMaximum(maximum*1.1);
  hists[0]->Draw("hist");
  for(unsigned int i=1; i<cuts.size(); ++i)
    hists[i]->Draw("samehist");
  leg->Draw("same");

  c->SaveAs((savedir+"nDoms_perLumi.png").Data());
}

//------------------------------------------//
// Get vector of times so I can draw where
// the cuts are going to be
//------------------------------------------//
vector<double> getTimesSC2()
{
  
  vector<double> times;
  times.push_back(280.001);
  times.push_back(280.022);
  times.push_back(280.0432);
  times.push_back(280.0645);
  times.push_back(280.086);
  times.push_back(280.107);
  times.push_back(280.128);

  return times;
}
//------------------------------------------//
vector<double> getTimesSC1()
{
  
  vector<double> times;
  times.push_back(276.679);
  times.push_back(276.702);
  times.push_back(276.723);
  times.push_back(276.7449);
  times.push_back(276.7655);
  times.push_back(276.787);
  times.push_back(276.8069);
  times.push_back(276.829);

  return times;
}


//------------------------------------------//
// Method to place utc time cuts for a 
// given luminosity.  
//------------------------------------------//
TCut lumiCut(TString lumi)
{

  TString low  = 0;
  TString high = 100;

  if(lumi == "1"){
    low = "280.001*10e14";
    high = "280.022*10e14";
  }
  else if(lumi == "3"){
    low = "280.022*10e14";
    high = "280.0432*10e14";
  }
  else if(lumi == "10"){
    low = "280.0432*10e14";
    high = "280.0645*10e14";
  }
  else if(lumi == "30"){
    low = "280.0645*10e14";
    high = "280.086*10e14";
  }
  else if(lumi == "51"){
    low = "280.086*10e14";
    high = "280.107*10e14";
  }
  else if(lumi == "100"){
    low = "280.107*10e14";
    high = "280.128*10e14";
  }
  else
    cout<<"Lumi not known"<<endl;
  
  // Construct TCut
  TString var = "startUTCDaqTime_";
  TCut cut = TCut((low+"<"+var+"&&"+var+"<"+high).Data());
  return cut;

}


//------------------------------------------//
// Problem 3: There is a requrement on the
// cutwavetime(...) which I do not fully 
// understand.  However, I may not be able
// to easily do this from the output tree.
// Therefore I will first try to look at it
// with a python script looping over I3file
// that was output with the tree.  
//
// Now have output from CheckWaveform.py in 
// root histogram format.  I will format the
// plots here.
//------------------------------------------//
void checkWaveformFormat()
{

  // In this case the input file is output from
  // CheckWaveform.py which just saves the necessary
  // variables from checkwavetime(...) function.
  //TFile* file = new TFile("../plots/RootPlots/cutWaveTimeVars1.root");
  //TFile* file = new TFile("../plots/RootPlots/cutWaveTimeVars.root");
  TFile* file = new TFile("../plots/RootPlots/cutWaveTimeVars_refined.root");
  
  // There are three essential plots I want to look at.
  // 1.) amplitude
  // 2.) # Pulses
  // 3.) deltaT
  // All of these have the full plot and then the ones with
  // the time constraint.

  // Save the percentages
  vector<TString> filters;
  filters.push_back("1");
  filters.push_back("3");
  filters.push_back("10");
  filters.push_back("30");
  filters.push_back("51");
  filters.push_back("100");

  // Call methods to make plots
  //basicPlot(file);
  //ampPlot(file, filters);
  //npulsePlot(file, filters);
  //tDiffPlot(file, filters);
  tDiffMaxPlot(file, filters);

}

//------------------------------------------//
// Basic plot
//------------------------------------------//
void basicPlot(TFile* file)
{

  // There are three basic plots to make
  vector<TString> pnames;
  pnames.push_back("amp");
  pnames.push_back("npulse");
  pnames.push_back("tDiff");
  pnames.push_back("maxTDiff");

  // Set x-titles
  vector<TString> xtitles;
  xtitles.push_back("Amplitude [V]");
  xtitles.push_back("Number of Pulses");
  xtitles.push_back("LEtime - wavetime [ns]");
  xtitles.push_back("Max(LEtime - wavetime) [ns]");

  // Only one ytitle
  TString ytitle = "Entries";

  // Make Canvas
  TCanvas* c = makeCanvas("c");

  // Loop and plot
  TH1* hist = NULL;
  for(unsigned int i=0; i<pnames.size(); ++i){

    // Get Hist
    TString pname = "h_"+pnames.at(i);
    hist = getHist(file,pname,xtitles[i],ytitle,kBlack,20);

    // Plot
    hist->Draw();

    // Save
    c->SaveAs((savedir+pnames[i]+"_total.png").Data());

  }// end loop

  delete hist;
  delete c;
    
}

//------------------------------------------//
// Format amplitude plots
//------------------------------------------//
void ampPlot(TFile* file, vector<TString> filters)
{

  // Specify the titles
  TString xtitle = "Amplitude [v]";
  TString ytitle = "Entries";

  // Histogram name
  TString pname = "h_amp";

  // Create canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogx();
  c->SetLogy();

  // Make legend
  TLegend* leg = makeLegend(0.7,0.8,0.8,0.9);

  // Loop and plot
  TH1* hist = NULL;
  for(unsigned int i=0; i<filters.size(); ++i){
    leg->Clear();
    leg->SetHeader("Filter");
    
    TString filter = filters.at(i);
    
    hist = getHist(file,pname+"_"+filter,xtitle,ytitle,
		   m_colors[i], m_markers[i]);
    
    leg->AddEntry(hist,(filter+"%").Data(),"l");
    hist->Draw();

    // Draw legend
    leg->Draw("same");

    c->SaveAs((savedir+"amp_filter"+filter+".png").Data());

  }// end loop over filters


	       
}

//------------------------------------------//
// Format amplitude plots
//------------------------------------------//
void npulsePlot(TFile* file, vector<TString> filters)
{

  // Specify the titles
  TString xtitle = "Number of Pulses";
  TString ytitle = "Entries";

  // Histogram name
  TString pname = "h_npulse";

  // Create canvas
  TCanvas* c = makeCanvas("c");
  
  // Make legend
  TLegend* leg = makeLegend(0.7,0.8,0.6,0.9);
  //TLegend* leg = makeLegend(0.7,0.8,0.8,0.9);

  // Loop and plot
  TH1* hist[6];
  float maximum = -999;
  for(unsigned int i=0; i<filters.size(); ++i){
    TString filter = filters.at(i);
    
    hist[i] = getHist(file,pname+"_"+filter,xtitle,ytitle,
		   m_colors[i], m_markers[i]);
    
    leg->AddEntry(hist[i],(filter+"%").Data(),"l");

    if( maximum < hist[i]->GetMaximum() )
      maximum = hist[i]->GetMaximum();

  }// end loop over filters
  /*
  for(unsigned int i=0; i<filters.size(); ++i){
    leg->Clear();
    leg->SetHeader("Filter");
    leg->AddEntry(hist[i],(filters.at(i)+"%").Data(),"l");
    hist[i]->Draw();
    leg->Draw("same");
    c->SaveAs((savedir+"npulse_filter"+filters.at(i)+".png").Data());
  }

  return;
  */

  // Set maximum correctly
  hist[0]->SetMaximum(1.1*maximum);
  hist[0]->Draw();
  for(unsigned int i=1; i<filters.size(); ++i){
    hist[i]->Draw("same");
  }

  // Draw legend
  leg->SetHeader("Filter");
  leg->Draw("same");
  
  c->SaveAs((savedir+"npulse_perLumi.png").Data());

}

//------------------------------------------//
// Format deltaT
//------------------------------------------//
void tDiffPlot(TFile* file, vector<TString> filters)
{

  // Specify the titles
  TString xtitle = "LEtime - wavetime [ns]";
  TString ytitle = "Entries";

  // Histogram name
  TString pname = "h_tDiff";

  // Create canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogy();
  
  // Make legend
  TLegend* leg = makeLegend(0.7,0.8,0.8,0.9);

  // Loop and plot
  TH1* hist = NULL;
  for(unsigned int i=0; i<filters.size(); ++i){
    leg->Clear();
    leg->SetHeader("Filter");
    TString filter = filters.at(i);
    
    hist = getHist(file,pname+"_"+filter,xtitle,ytitle,
		   m_colors[i], m_markers[i]);
    
    leg->AddEntry(hist,(filter+"%").Data(),"l");
    hist->Draw();

    // Draw legend
    leg->Draw("same");

    // Save 
    c->SaveAs((savedir+"timeDiff_filter"+filter+".png").Data());

  }// end loop over filters

	       
}

//------------------------------------------//
// Format deltaT_max
//------------------------------------------//
void tDiffMaxPlot(TFile* file, vector<TString> filters)
{

  // Specify the titles
  TString xtitle = "Max(LEtime - wavetime) [ns]";
  TString ytitle = "Entries";

  // Histogram name
  TString pname = "h_maxTDiff";

  // Create canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogy();
  
  // Make legend
  TLegend* leg = makeLegend(0.7,0.8,0.8,0.9);

  // Loop and plot
  TH1* hist = NULL;
  for(unsigned int i=0; i<filters.size(); ++i){
    leg->Clear();
    leg->SetHeader("Filter");
    TString filter = filters.at(i);
    
    hist = getHist(file,pname+"_"+filter,xtitle,ytitle,
		   m_colors[i], m_markers[i]);
    
    leg->AddEntry(hist,(filter+"%").Data(),"l");
    hist->Draw();

    // Draw legend
    leg->Draw("same");

    // Save 
    c->SaveAs((savedir+"maxTimeDiff_filter"+filter+".png").Data());

  }// end loop over filters

	       
}
