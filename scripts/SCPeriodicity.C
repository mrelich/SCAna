
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
// This is a quick check on the periodicity of the //
// standard candle.  From talking with Shigeru, it //
// should be about 10 Hz, but let's see.           //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//

#include "tools.C"
#include "TH2.h"

// Get waveform channel
WaveFormChannel* chan = new WaveFormChannel();

// Specify savedir
TString savedir = "../plots/Period/";

//------------------------------//
// Main
//------------------------------//
void SCPeriodicity()
{

  // Specify the luminosity
  vector<TString> lumis;
  lumis.push_back("1");
  lumis.push_back("3");
  lumis.push_back("10");
  lumis.push_back("30");
  lumis.push_back("51");
  lumis.push_back("100");

  // Plot periods
  for(unsigned int l=0; l<lumis.size(); ++l)
    //plotPeriod(lumis.at(l));
    plotProblemPeriod(lumis.at(l));

}

//------------------------------//
// Plot period
//------------------------------//
void plotPeriod(TString lumi)
{

  // Binning
  int nbins = 310;
  float xmin  = -1;
  float xmax  = 3;
  
  // There is a conversion factor to go 
  // from 10*ns to s
  double conv = 10*pow(10,9);

  // Make Canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogy();
  
  // Make legend
  TLegend* leg = makeLegend(0.65,0.9,0.8,0.9);

  // Set file name
  TString fname = "SC2_filter"+lumi+"_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";
  
  // Make chain and add file
  TChain* ch = new TChain("WaveFormTree");
  ch->Add(("../trees/"+fname).Data());
  int nEntries = ch->GetEntries();

  // Make Tree
  WaveFormTree* tree = new WaveFormTree();
  ch->SetBranchAddress("waveform",&tree);
    
  // create histogram
  TH1F* hist = makeFrame("hist_"+lumi,nbins,xmin,xmax,
			 "#Deltat Event Start Times [s]",
			 "Entries/bin");
  leg->AddEntry(hist,("Filter: " + lumi + "%").Data(),"le");

  // Loop and plot
  double prevTime = -999;
  for(int evt=0; evt<nEntries; ++evt){
    
    // Periodic print
    if( evt % 100 == 0 )
      cout<<"Getting: "<<evt<<endl;
    
    // Get Event
    ch->GetEvent(evt);
    
    // Get time
    double curTime = tree->startUTCDaqTime_/conv;
      
    // Check delta
    if( prevTime > 0 )
      hist->Fill(curTime-prevTime);
    
    
    // update time
    prevTime = curTime;
    
  }// end loop over events
    
  
  // Draw
  hist->Draw();
  leg->Draw("same");
  
  // Save
  TString save = savedir + "Period_filter" + lumi +".png";
  c->SaveAs(save.Data());

  // clean up
  delete ch;
  delete tree;
  delete hist;
  delete c;
  
  
}

//------------------------------//
// Problematic period check
//------------------------------//
void plotProblemPeriod(TString lumi)
{

  // Binning
  int nbins = 35;
  double xmin  = -0.5;
  double xmax  = 3;
  
  // There is a conversion factor to go 
  // from 10*ns to s
  double conv = 10*pow(10,9);

  // Make Canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogz();
  c->SetRightMargin(0.15);
  
  // Set file name
  TString fname = "SC2_filter"+lumi+"_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";
  
  // Make chain and add file
  TChain* ch = new TChain("WaveFormTree");
  ch->Add(("../trees/"+fname).Data());
  int nEntries = ch->GetEntries();

  // Make Tree
  WaveFormTree* tree = new WaveFormTree();
  ch->SetBranchAddress("waveform",&tree);
    
  // create histogram
  TH2F* hist = new TH2F("hist","",
		      nbins,xmin,xmax,
		      nbins,xmin,xmax);
  setAtt(hist, "#Deltat Previos Event Start Times [s]", 
	 "#Deltat Event Start Times [s]",
	 "Entries/bin");

  // Loop and plot
  double prevTime  = -999;
  double prevDelta = -999;
  for(int evt=0; evt<nEntries; ++evt){
    
    // Periodic print
    if( evt % 100 == 0 )
      cout<<"Getting: "<<evt<<endl;
    
    // Get Event
    ch->GetEvent(evt);
    
    // Get time
    double curTime = tree->startUTCDaqTime_/conv;
      
    // Check delta
    double curDelta = curTime - prevTime;
    if( prevDelta > 0 )
      hist->Fill(prevDelta,curDelta);

    // update time
    prevTime  = curTime;
    prevDelta = curDelta;
    
  }// end loop over events
    
  
  // Draw
  hist->Draw("colz");
  
  // Draw latex label
  TLatex* lat = makeLatex();
  lat->DrawLatex(0.2,0.85, ("Filter: " + lumi + "%").Data());
  
  // Save
  TString save = savedir + "dTComparison_filter" + lumi +".png";
  c->SaveAs(save.Data());

  // clean up
  delete ch;
  delete tree;
  delete hist;
  delete c;
  delete lat;
  
}
