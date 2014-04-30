
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
// Want to look at the correlation between DOMs in terms //
// of these 'late' waveforms that are truncated.         //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//

#include "tools.C"

WaveFormChannel* chan = NULL;

string savedir = "../plots/DOMComparison/";

//------------------------------//
// Main
//------------------------------//
void DOMCorrelation()
{

  // Just look at one file for now
  TString lumi   = "100";
  TString infile = "../trees/SC2_filter"+lumi+"_cutNDOM400_TRCut_WaveCalib_Waveform_tree.root";

  // Make TChain object
  TChain* chain = new TChain("WaveFormTree");
  chain->Add(infile);
  
  // Pick two DOMs to look at
  vector<int> DOM1;    vector<int> DOM2;
  DOM1.push_back(43);  DOM2.push_back(42);
  DOM1.push_back(43);  DOM2.push_back(43);
  DOM1.push_back(42);  DOM2.push_back(44);
  DOM1.push_back(43);  DOM2.push_back(44);
  
  // Make sure no user error
  if(DOM1.size() != DOM2.size()){
    cout<<"Not equal number of doms to compare"<<endl;
    return;
  }

  // Loop and plot
  for(unsigned int i=0; i<DOM1.size(); ++i)
    plotDOMComp(chain, DOM1.at(i), DOM2.at(i),lumi);
  
  
}

//------------------------------//
// Look at two DOMs
//------------------------------//
void plotDOMComp(TChain* ch,
		 int DOM1,
		 int DOM2,
		 TString lumi)
{

  // Make Canvas
  TCanvas* c = makeCanvas("c");
  c->SetLogz();
  c->SetRightMargin(0.15);
  
  // Make the histogram
  TH2* h = new TH2F("h","h",55,0,550,55,0,550);
  stringstream xtitle;
  stringstream ytitle;
  xtitle << "DOM(" << DOM1<<") #Deltat [ns]";
  ytitle << "DOM(" << DOM2<<") #Deltat [ns]";
  
  setAtt(h,TString(xtitle.str().c_str()),
	 TString(ytitle.str().c_str()),
	 "Entries/bin");
  h->SetMinimum(0.8);

  // Set the waveform Tree
  WaveFormTree* tree = new WaveFormTree();
  ch->SetBranchAddress("waveform", &tree);

  // Loop over tree entries and plot
  int nEntries   = ch->GetEntries();
  int nBadEvents = 0;
  for(int evt=0; evt<nEntries; ++evt){
    ch->GetEvent(evt);
    
    // Get channels
    TClonesArray* chan_array = (TClonesArray*) tree->GetWaveForm();
    int nChan = chan_array->GetEntries();

    // Set variables to store result
    double dt_DOM1 = -999;
    double dt_DOM2 = -999;

    // Loop and get variables
    for(int j=0; j<nChan; ++j){
      chan = (WaveFormChannel*) chan_array->At(j);
      
      // Get the waveform
      TGraph* atwd = (TGraph*) chan->GetAtwdWaveForm();
      
      // Make sure it is right string
      int stringNum = chan->GetStringNumber();
      if( stringNum != 55 ) continue;
      
      // Check OM Number
      int OMNum = chan->GetOMNumber();
      if( OMNum == DOM1 )    dt_DOM1 = getDT(atwd, chan->GetAtwdStartTime());
      if( OMNum == DOM2)     dt_DOM2 = getDT(atwd, chan->GetAtwdStartTime());

    }// end loop over nChan

    if(dt_DOM1 >= 0 && dt_DOM1 >=0)
      h->Fill(dt_DOM1, dt_DOM2);
    else{
      //cout<<"We have event where one of the DOMs doesn't have waveform."<<endl;
      //cout<<"DOM1: "<<DOM1<<" dt: "<<dt_DOM1<<endl;
      //cout<<"DOM2: "<<DOM2<<" dt: "<<dt_DOM2<<endl;
      //cout<<"Not filling result"<<endl;
      nBadEvents++;
    }

  }// end loop over entries
  
  h->Draw("colz");

  // Set a label
  TLatex* lat = makeLatex();
  lat->DrawLatex(0.2,0.90,("Filter: "+lumi+"%").Data());

  cout<<"Didn't fill for "<<nBadEvents<<endl;
  
  // Save
  stringstream save;
  save << savedir << "dt_DOM" << DOM1 << "_DOM" << DOM2 << ".png";
  c->SaveAs(save.str().c_str());

  // clean up
  delete c;
  delete lat;
  delete h;
}

//------------------------------//
// Get dT
//------------------------------//
double getDT(TGraph* gr, double start)
{

  int np = gr->GetN();
  double LEtime = 0, V = 0;
  for(int n=0; n<np; ++n){
    gr->GetPoint(n,LEtime,V);
    if( V > 0.1 ) break;
  }

  return LEtime - start;
  
}
