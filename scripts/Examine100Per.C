
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
// Temporary script to analyze the weird double peak //
// that is found for 100% luminosity filter.         //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//

#include "tools.C"

TString indir = "../trees/";
TString savedir = "../plots/Filtering2/";

// Divide the two regions
double sepNPE = 5.88;
TString lumi  = "100";
int tcutval   = 150;
double thresh = 0.1;

// DOM for specific waveform plots
int DOM = 42;

// binning for NPE plots
int nbins = 100;
float xmin = 0;
float xmax = 6;

//----------------------------//
// Main
//----------------------------//
void Examine100Per(int fopt, int ropt)
{

  // fopt = file option
  // ropt = run option

  TString fname = indir;
  if(fopt == 0){
    lumi = "100";
    fname += "SC2_filter100_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";
    sepNPE  = 5.88;
    tcutval = 150;
    thresh  = 0.1;
    xmin = 5.82;   
    xmax = 5.95;    
  }
  else if(fopt == 1){
    lumi = "51";
    fname += "SC2_filter51_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";
    sepNPE = 5.62;
    //tcutval = 300;
    tcutval = 250;
    thresh  = 0.075;
    xmin = 5.52;
    xmax = 5.7;
  }
  else if(fopt == 2){
    lumi = "30";
    fname += "SC2_filter30_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";
    sepNPE = 5.535;
    tcutval = 200;
    thresh  = 0.05;
    xmin = 5.45;
    xmax = 5.6;
  }
  else if(fopt == 3){
    lumi = "10";
    fname += "SC2_filter10_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";
    sepNPE = 5.26;
    tcutval = 250;
    thresh  = 0.02;
    xmin = 5.2;
    xmax = 5.3;
  }
  else if(fopt == 4){
    lumi = "3";
    fname += "SC2_filter3_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";
    xmin = 4.9;
    xmax = 5.025;
  }
  else if(fopt == 5){
    lumi = "1";
    fname += "SC2_filter1_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";
    xmin = 4.7;
    xmax = 4.82;
  }
  else{
    cout<<"File option not supported"<<endl;
    cout<<"\t0 -- 100%"<<endl;
    cout<<"\t1 -- 51%"<<endl;
    cout<<"\t2 -- 30%"<<endl;
    cout<<"\t3 -- 10%"<<endl;
    cout<<"\t4 -- 3%"<<endl;
    cout<<"\t5 -- 1%"<<endl;
  }

  // Get the TChain
  TChain* ch = new TChain("WaveFormTree");
  ch->Add(fname);

  // Plot log NPE
  if(ropt == 0)
    plotNPE(ch,false);

  if(ropt == 1)
    plotNPE(ch,true);
  
  else if(ropt == 2)
    plotTiming(ch);
}

//----------------------------//
// Plot NPE
//----------------------------//
void plotNPE(TChain* ch, bool doFilter)
{

  // Make canvas
  TCanvas* c = makeCanvas("c");
  
  // Make histogram
  TH1F* h = makeFrame("h",nbins,xmin,xmax,"log_{10}(NPE)","Entries/bin");

  // Make Legend
  TLegend* leg = makeLegend(0.15,0.3,0.8,0.93);

  // Set waveform tree
  WaveFormTree* tree = new WaveFormTree();
  ch->SetBranchAddress("waveform",&tree);
  
  // Loop over entries and fill hist
  int nEntries = ch->GetEntries();
  for(int evt=0; evt<nEntries; ++evt){
    ch->GetEvent(evt);
    
    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    
    if(doFilter && !passFilter(chan_array)) continue;
    
    h->Fill(tree->totalBestNpeLog_);
  }
  
  h->Draw();
  cout<<"Entries: "<<h->GetEntries()<<endl;;

  leg->AddEntry(h, ("Filter: " + lumi + "%").Data(),"l");
  leg->Draw("same");
 
  TString save = savedir + "lognpe_lumi" + lumi;
  if( doFilter ) save += "_filtered";
  else           save += "_noFilter";
  save += ".png";

  c->SaveAs(save.Data());

}

//----------------------------//
// Plot NPE
//----------------------------//
void plotTiming(TChain* ch)
{

  /*const int nDOMs = 11;
  int DOMs[nDOMs];
  for(int i=0; i<nDOMs; ++i)
    DOMs[i] = 37+i;
  */
  const int nDOMs = 11;
  int DOMs[nDOMs];
  for(int i=0; i<nDOMs; ++i)
    DOMs[i] = 37+i;

  // Make canvas
  TCanvas* c = makeCanvas("c");
  
  // Make histogram
  TH1F* good = makeFrame("good",50,0,500,"#Deltat","Entries/bin");
  TH1F* bad = makeFrame("bad",50,0,500,"#Deltat","Entries/bin");
  bad->SetLineColor(kRed);

  // Set waveform tree
  WaveFormTree* tree = new WaveFormTree();
  ch->SetBranchAddress("waveform",&tree);
  
  // Make legend
  TLegend* leg = makeLegend(0.6,0.75,0.8,0.94);
  leg->SetHeader(("Filter: "+lumi+"%").Data());

  // Loop over entries and fill hist
  int nEntries = ch->GetEntries();
  double minDT = 0;
  for(int evt=0; evt<nEntries; ++evt){
    minDT = 0;
    ch->GetEvent(evt);

    double npe = tree->totalBestNpeLog_;
    
    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    int nChannels = chan_array->GetEntries();
    for(int j=0; j<nChannels; ++j){
      chan = (WaveFormChannel*) chan_array->At(j);      

      // Get Graph and plot                                                                       
      TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();

      // make sure right string 
      int stringNum = chan->GetStringNumber();
      if(stringNum != 55) continue;

      // Check OM number
      int OMNum = chan->GetOMNumber();
      bool passed = false;
      for(int d=0; d<nDOMs; ++d){
	int DOM = DOMs[d];
	if( OMNum == DOM ) passed = true;
      }
      if(!passed) continue;

      // Get delta T
      double start = chan->GetAtwdStartTime();
      int np = atwd->GetN();
      double LEtime = 0, V = 0;
      for(int n=0; n<np; ++n){
	atwd->GetPoint(n,LEtime,V);
	//if( V > 0.1 ) break;
	//if( V > 0.05 ) break;
	//if( V > 0.075 ) break;
	if( V > thresh ) break;
      }

      if( minDT < LEtime - start )
	minDT = LEtime - start;

      
    }// end loop over channels
    
    
    if( npe > sepNPE ) good->Fill(minDT);
    else               bad->Fill(minDT);
  }// end loop over entries
  
  good->Draw();
  bad->Draw("same");

  // Number of Entries
  int up = good->FindBin(tcutval);
  double tot_good = good->Integral();
  double reg_good = good->Integral(0,up);
  double tot_bad  = bad->Integral();
  double reg_bad  = bad->Integral(0,up);
  cout<<"Good: "<<tot_good<<" in region: "<<reg_good<<" rejection: "<<(1-reg_good/tot_good)<<endl;
  cout<<"Bad: "<<tot_bad<<" in region: "<<reg_bad<<" rejection: "<<(1-reg_bad/tot_bad)<<endl;

  leg->AddEntry(good,Form("log(NPE) > %2.3f",sepNPE),"l");
  leg->AddEntry(bad,Form("log(NPE) < %2.3f",sepNPE),"l");
  leg->Draw("same");
  
  TString save = savedir + "deltaT_lumi"+lumi+".png";
  c->SaveAs(save.Data());
  

}

//----------------------------//
// Plot Waveform for two 
// separate regions
//----------------------------//
void plotWaveform(TChain* ch)
{

  // Canvas
  TCanvas* c = makeCanvas("c");

  // Create a waveform tree instance
  WaveFormTree* tree = new WaveFormTree();
  ch->SetBranchAddress("waveform",&tree);

  //Make a frame for histogram
  TH1F* frame = makeFrame("h",1,0,500,"time [ns]","Voltage [V]");
  frame->SetMinimum(0);
  frame->SetMaximum(7.5);
  frame->Draw();

  int nEntries = ch->GetEntries();
  int good = 0;
  int bad  = 0;
  for(int evt=0; evt<nEntries; ++evt){
    ch->GetEvent(evt);

    double curNPE = tree->totalBestNpeLog_;

    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    int nChannels = chan_array->GetEntries();

    //if(!passFilter(chan_array)) continue;
    for(int j=0; j<nChannels; ++j){
      chan = (WaveFormChannel*) chan_array->At(j);      

      // Get Graph and plot                                                                       
      TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
      if( curNPE < sepNPE ) atwd->SetLineColor(kRed);
      else                  atwd->SetLineColor(kBlack);
      atwd->SetLineWidth(1.5);
      
      // Check OM number
      int OMNum = chan->GetOMNumber();
      int stringNum = chan->GetStringNumber();
      //if( !passWavetime(atwd,OMNum) ) continue;
      if( !(stringNum == 55 && OMNum == DOM) ) continue;


      // reset time
      resetTime(atwd,0);

      // Draw
      atwd->DrawClone("LPSame");

      // Count
      if( curNPE < sepNPE ) bad++;
      else                  good++;
      
    }// end loop over channels
  }// end loop over events

  cout<<"Number of events "<<good+bad<<endl;
  cout<<"\tGood: "<<good<<endl;
  cout<<"\tBad: "<<bad<<endl;

}

//-------------------------------//
// Second filtering method
//-------------------------------//
bool passFilter(TClonesArray* chArray)
{

  // DOMs to consider
  const int nDOMs = 11;
  int start = 37;
  int DOMs[nDOMs];
  for(int i=0; i<nDOMs; ++i)
    DOMs[i] = start + i;
  

  // Loop over the channels and make sure
  // the start time is within threshold
  int nChannels = chArray->GetEntries();
  double maxLE = 0;
  for(int j=0; j<nChannels; ++j){
    chan = (WaveFormChannel*) chArray->At(j);      

    int OMNum = chan->GetOMNumber();
    int stringNum = chan->GetStringNumber();
    if(stringNum !=55) continue;
    bool hasDOM = false;
    for(int d=0; d<nDOMs; ++d)
      if(OMNum == DOMs[d])
	hasDOM = true;

    if(!hasOM) continue;

    // Get Graph and plot                                                                       
    TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();

    // Get the start time
    double begin = chan->GetAtwdStartTime();
    
    // Get time when leading edge crosses threshold
    int np = atwd->GetN();
    double LEtime = 0, V = 0;
    for(int n=0; n<np; ++n){
      atwd->GetPoint(n,LEtime,V);
      //if( V > 0.1 ) break;
      if( V > 0.01 ) break;
    }	

    if( maxLE < LEtime - begin )
      maxLE = LEtime - begin;
  }// end loop over channels


  if( maxLE > tcutval ) return false;

  return true;

}

//-------------------------------//
// Method to filter waveforms
//-------------------------------//
bool passWavetime(TGraph* gr, int DOM, double begin)
{

  if( !(DOM == 43 || DOM == 44 || DOM == 42) ) return true;

  // Get the max time and end time
  double np  = gr->GetN();
  double V   = 0;
  double end = 0;
  double max = findLocationMax(gr);
  gr->GetPoint(np-1, end, V);

  if( end - max < 300 ) return false;

  return true;


}

//-------------------------------//
// Plot LE time of SC strings
// vs. UTC time.
//-------------------------------//
void timingPlot(TChain* ch)
{

  // Canvas
  TCanvas* c = makeCanvas("c");
  
  // Create a waveform tree instance
  WaveFormTree* tree = new WaveFormTree();
  ch->SetBranchAddress("waveform",&tree);

  int colors[] = {kBlack, kBlue, kRed, kYellow+2, kGreen+2,
                  kCyan+1, kMagenta, kOrange+8, kGreen-1, kBlue +2,
                  kViolet+3, kPink+7, kOrange+2, kSpring-5, kAzure+9};

  // DOMs to consider
  const int NDOMs = 5;
  int DOMs[NDOMs];
  int point[NDOMs];
  double xpts[NDOMs][10000];
  double ypts[NDOMs][10000];
  for(int i=0; i<NDOMs; ++i){
    DOMs[i]   = 40 + i;
    point[i]  = 0;
  }


  // Loop over entries
  int nEntries = ch->GetEntries();
  for(int evt=0; evt<nEntries; ++evt){
    ch->GetEvent(evt);

    if(tree->totalBestNpeLog_ > sepNPE) continue;

    double utctime = tree->startUTCDaqTime_;

    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    int nChannels = chan_array->GetEntries();
    for(int j=0; j<nChannels; ++j){
      chan = (WaveFormChannel*) chan_array->At(j);      


      // Check OM number
      int OMNum = chan->GetOMNumber();
      int stringNum = chan->GetStringNumber();

      if( stringNum != 55 ) continue;

      // Check if this is one of our doms
      for(int d=0; d<NDOMs; ++d){
	DOM = DOMs[d];

	if( OMNum != DOM ) continue;
	
	// Get maximum time
	TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
	double time  = findLocationMax(atwd);
	double end = 0, V = 0;
	atwd->GetPoint(atwd->GetN()-1,end,V);

	xpts[d][point[d]] = utctime;
	ypts[d][point[d]] = end - time;
	point[d]++;

      }// end loop over doms

    }// end loop over channels

  }// end loop over entries

  // Now we can plot
  TGraph* graph = new TGraph(point[0],xpts[0],ypts[0]);
  graph->SetMarkerColor(colors[0]);
  graph->SetLineColor(colors[0]);
  graph->Draw("ALP");
  for(int i=1; i<NDOMs; ++i){
    graph = new TGraph(point[i], xpts[i], ypts[i]);
    graph->SetMarkerColor(colors[i]);
    graph->SetLineColor(colors[i]);
    graph->Draw("same");
  }


}
