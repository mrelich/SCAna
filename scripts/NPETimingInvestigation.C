
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// When designing the filtering script I made a plot that   //
// showed the NPE vs UTC time and it was noticed that for   //
// the distinct luminosity filter settings, there were some //
// events that appear to have NPE consistent with the 100%  //
// luminosity filter. This script will make some plots to   //
// investigate that.                                        //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "tools.C"

TString savedir = "../plots/";

//-----------------------------------//
// Main
//-----------------------------------//
void NPETimingInvestigation()
{
  
  // Specify the data files
  TString indir = "../trees/";
  vector<TString> fnames;
  fnames.push_back(indir+"SC2_filter1_cutNDOM400_WaveCalib_Waveform_tree.root");
  fnames.push_back(indir+"SC2_filter3_cutNDOM400_WaveCalib_Waveform_tree.root");
  fnames.push_back(indir+"SC2_filter10_cutNDOM400_WaveCalib_Waveform_tree.root");
  fnames.push_back(indir+"SC2_filter30_cutNDOM400_WaveCalib_Waveform_tree.root");
  fnames.push_back(indir+"SC2_filter51_cutNDOM400_WaveCalib_Waveform_tree.root");
  fnames.push_back(indir+"SC2_filter100_cutNDOM400_WaveCalib_Waveform_tree.root");

  // Give some names for plotting NPE
  vector<TString> filters;
  filters.push_back("1");
  filters.push_back("3");
  filters.push_back("10");
  filters.push_back("30");
  filters.push_back("51");
  filters.push_back("100");


  // Plot LogNPE
  //plotLogNPE(fnames,filters);

  //for(unsigned int f=0; f<1; ++f)
  //plotTimeVsNPE(fnames.at(f),filters.at(f));
  plotTimeVsNPE(fnames,filters);
  
}

//-----------------------------------//
// Plot the start time of wave vs.
// NPE
//-----------------------------------//
void plotTimeVsNPE(vector<TString> fnames, 
		   vector<TString> filters)
{

  // TODO Make options
  int scstring = 55;
  int dom      = 40;
  
  // Make a generic canvas
  TCanvas* c = makeCanvas("c");
  
  // Create TChain
  TChain* chain = new TChain("WaveFormTree");
  for(unsigned int i=0; i<fnames.size(); ++i)
    chain->Add(fnames.at(i).Data());
  //chain->Add(fname.Data());
  Long64_t ch_entries = chain->GetEntries();
  cout<<"There are: "<<ch_entries<<" entries"<<endl;
  
  // Set Tree Address
  WaveFormTree* tree = new WaveFormTree();
  chain->SetBranchAddress("waveform",&tree);

  // Create a waveform channel
  WaveFormChannel* chan = new WaveFormChannel();

  // Loop over entries and draw
  double x[50000];
  double y[50000];
  int np = 0;
  float prev = 0;
  for(int entry=0; entry<ch_entries; ++entry){
    chain->GetEvent(entry);
    

    // Apply the filter
    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    int nChannels = chan_array->GetEntries();
    bool skipEvent = false;
    for(int j=0; j<nChannels; ++j){
      chan = (WaveFormChannel*) chan_array->At(j);

      // Make sure we cut on string 55, DOM 43
      int OMNum = chan->GetOMNumber();
      int stringNum = chan->GetStringNumber();
      if( stringNum != 55) continue;
      if( !(OMNum == 41 || OMNum == 42 || 
	    OMNum == 43 || OMNum == 44) ) continue;

      // Get Waveform
      TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();

      // Place requirement
      double end, V;
      int npoints = atwd->GetN();
      atwd->GetPoint(npoints-1,end,V);
      double maxloc = findLocationMax(atwd);
      //if( end - maxloc < 300 ){
      if( end - maxloc < 250 ){
	skipEvent = true;
	break;
      }
    }// end loop over channels

    if(skipEvent) continue;

    double npe  = tree->totalBestNpeLog_;
    double time = tree->startUTCDaqTime_;
    
    x[np] = time;
    y[np] = npe;
    np++;
  }// end loop over events

  // Make tgraph
  TGraph* gr = new TGraph(np, x, y);
  gr->SetLineColor(kBlack);
  gr->SetMarkerColor(kBlue);
  gr->SetMarkerSize(1.5);
  gr->SetLineWidth(1.0);
  gr->SetTitle("");
  gr->GetXaxis()->SetTitle("UTC Daq Time");
  gr->GetYaxis()->SetTitle("log_{10}(NPE)");
  gr->Draw("ALP");

  //c->SaveAs((savedir+"NPE_Timing/time_vs_NPE.png").Data());

}

//-----------------------------------//
// Plot Log NPE for confirmation
//-----------------------------------//
void plotLogNPE(vector<TString> fnames, 
		vector<TString> filters)
{

  // Make canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogy();

  // Get Legend
  TLegend* leg = makeLegend(0.15,0.3,0.68,0.93);
  leg->SetHeader("Filter");
  
  // The variable to draw
  TString var = "totalBestNpeLog_";

  // Loop over and load the histograms
  float maximum = 0;
  TH1D* hists[6];
  for(unsigned int i=0; i<fnames.size(); ++i){
    TFile* file = new TFile(fnames.at(i).Data());
    TTree* tree = (TTree*) file->Get("WaveFormTree");
    
    // Make histogram
    TString hname = "h_"+filters.at(i);
    hists[i] = new TH1D(hname.Data(),"",100,4,6);
    setAtt(hists[i],"Log(Best NPE)","Entries",m_colors[i],20);

    // Draw
    tree->Draw((var + " >> " + hname).Data());

    // Store maximum for later
    if(maximum < hists[i]->GetMaximum())
      maximum = hists[i]->GetMaximum();
    
    leg->AddEntry(hists[i],(filters.at(i)+"%").Data(),"l");
  }
  
  // Now loop and draw
  hists[0]->SetMaximum(1.2*maximum);
  hists[0]->Draw();
  for(unsigned int i=1; i<fnames.size(); ++i)
    hists[i]->Draw("same");
  
  // Draw legend
  leg->Draw("same");
  
  // Save the plot
  TString save = savedir + "NPE_Timing/logNPE.png";
  c->SaveAs(save.Data());

}
