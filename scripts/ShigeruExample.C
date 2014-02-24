#include "TMath.h"
//#include "TChain.h"

bool DrawNPEofSC(){

  //
  //options
  //
  bool doChannelWise    = false; // draw npe's received in each DOM 
  bool doWrite = false;
  string histogramsStoreFileName = "NPEonString55_SC10per.root";
  //string histogramsStoreFileName = "totalNPE_SC10per.root";
  TCut stringCut = "AtwdResponseChannels_.string_==55"; // String 57 is where the SC2 was installed.
  //
  // style 
  //
  SetStyle();
  gROOT->ForceStyle();
  
  string indirMC                 = "./splinetables/"; 
  //string indirMC                 = "./root_files/standardtables/"; 
  //string indirData                 = "./root_files/realdata/"; 
  string indirData                 = "./data/"; 
  string mieFilename           = "SC2_spicemie_hole100cm_10per.root";
  //string mieFilename           = "SC2_aha_hole100cm_10per_woSquare.root";
  string spiceFilename           = "SC2_spice1_hole100cm_10per.root";
  //string spiceFilename           = "SC2_spice_hole100cm_10per_woSquare.root";
  //string dataFilename           = "SC10per_Wave_DroopUncorrect.tree.root";
  string dataFilename           = "testing.root";
  //gSystem->Load("libtree-maker.so");  

/* Read in the Mie MC file */
  TFile *fileMie = (TFile *) TFile::Open((indirMC+mieFilename).c_str());
  TTree *mieTree = (TTree *) fileMie->Get("JulietTree");
  TFile *fileSpice = (TFile *) TFile::Open((indirMC+spiceFilename).c_str());
  TTree *spiceTree = (TTree *) fileSpice->Get("JulietTree");
  TFile *fileData = (TFile *) TFile::Open((indirData+dataFilename).c_str());
  TTree *dataTree = (TTree *) fileData->Get("RealTree");

  cout << "*************************************************" << endl;
  cout << "SPice-Mie has " << mieTree->GetEntries() << " entries." <<endl;
  cout << "Spice has " << spiceTree->GetEntries() << " entries." <<endl;
  cout << "Data has " << dataTree->GetEntries() << " entries." <<endl;
  cout << "*************************************************" << endl;

  // The lowest level of cut - base cut
  TCut minimum_dom           = "DetectorResponseEvent_.totalNumberOfDom_>=100";
  TCut minimum_npe           = "DetectorResponseEvent_.totalBestEstimatedNPE_>=10.0"; 
  TCut basecut               = (minimum_dom&&minimum_npe);
  TCut atwdLargerThanFadc    = "AtwdResponseChannels_.estimatedNPE_>=FadcResponseChannels_.estimatedNPE_";
  //TCut atwdLargerThanFadc    = "AtwdResponseChannels_.estimatedNPE_>0.0";



  // Histograms definition
  if(!doChannelWise){  // Event-wise total NPE
    //x-axis
    //int    npe_nbin        =  1300;
    //double npe_min       =  7.0e4;
    //double npe_max       =  2.0e5;
    int    npe_nbin        =  440;
    //double npe_min       =  1.4e5;
    double npe_min       =  0;
    double npe_max       =  3.6e5;

    TH1D* mieNPE = new TH1D("mieNPE","total NPE", npe_nbin, npe_min, npe_max);
    TH1D* spiceNPE = new TH1D("spiceNPE","total NPE", npe_nbin, npe_min, npe_max);
    TH1D* dataNPE = new TH1D("dataNPE","total NPE", npe_nbin, npe_min, npe_max);


    TAxis* xaxis = mieNPE->GetXaxis();
    TAxis* yaxis = mieNPE->GetYaxis();
    //mieNPE->SetContour(60);
    xaxis->SetTitle("Estimated event-sum NPE");
    yaxis->SetTitle("Relative Number of Events"); 
    spiceNPE->GetXaxis()->SetTitle("Estimated event-sum NPE");
    spiceNPE->GetYaxis()->SetTitle("Relative Number of Events"); 
    dataNPE->GetXaxis()->SetTitle("Estimated event-sum NPE");
    dataNPE->GetYaxis()->SetTitle("Relative Number of Events"); 

  } else {  // NPE along the specified string
    //y-axis
    int    npe_nbin        =  1000;
    double npe_min       =  0.0;
    double npe_max       =  5.0e4;
  }

  /////////////////
  //Start Drawing
  /////////////////


  if(doChannelWise){ // draw NPE along the specified string

    ostringstream name;
    ostringstream histname;
    TH1D* histDomWiseNpeMie[60];
    TH1D* histDomWiseNpeSpice[60];
    TH1D* histDomWiseNpeData[60];

    // Build NPE histogram for each DOMs at the specific string
    TCanvas *canvas_DOMNPE = new TCanvas("DOMNPE","DOM-wise Npe", 500,500);
    canvas_DOMNPE->cd(1);
    SetgPad();
    for(int nDOM = 1; nDOM<=60; nDOM++){  // DOM loop
      name.str(""); name.clear(stringstream::goodbit);
      name << "AtwdResponseChannels_.omNumber_=="<<nDOM;
      const char *omNumber = name.str().c_str();
      TCut omSelection = omNumber;

      // Spice-Mie MC
      histname.str(""); histname.clear(stringstream::goodbit);
      histname << "mieNPE" << nDOM;
      histDomWiseNpeMie[nDOM-1] = new TH1D(histname.str().c_str(),"DOM-wise NPE", npe_nbin, npe_min, npe_max); 
      string drawoption = "AtwdResponseChannels_.estimatedNPE_>> " + histname.str();
      //cout << drawoption << endl;
      mieTree->Draw(drawoption.c_str(), (basecut&&omSelection&&stringCut&&atwdLargerThanFadc), "hist");
      drawoption = "FadcResponseChannels_.estimatedNPE_>>+ " + histname.str();
      mieTree->Draw(drawoption.c_str(), (basecut&&omSelection&&stringCut&&(!atwdLargerThanFadc)), "hist");

      // Spice MC
      histname.str(""); histname.clear(stringstream::goodbit);
      histname << "spiceNPE" << nDOM;
      histDomWiseNpeSpice[nDOM-1] = new TH1D(histname.str().c_str(),"DOM-wise NPE", npe_nbin, npe_min, npe_max); 
      string drawoption = "AtwdResponseChannels_.estimatedNPE_>> " + histname.str();
      //cout << drawoption << endl;
      spiceTree->Draw(drawoption.c_str(), (basecut&&omSelection&&stringCut&&atwdLargerThanFadc), "hist");
      drawoption = "FadcResponseChannels_.estimatedNPE_>>+ " + histname.str();
      spiceTree->Draw(drawoption.c_str(), (basecut&&omSelection&&stringCut&&(!atwdLargerThanFadc)), "hist");

      // data
      histname.str(""); histname.clear(stringstream::goodbit);
      histname << "dataNPE" << nDOM;
      histDomWiseNpeData[nDOM-1] = new TH1D(histname.str().c_str(),"DOM-wise NPE", npe_nbin, npe_min, npe_max); 
      drawoption = "AtwdResponseChannels_.estimatedNPE_>> " + histname.str();
      //cout << drawoption << endl;
      dataTree->Draw(drawoption.c_str(), (basecut&&omSelection&&stringCut&&atwdLargerThanFadc), "hist");
      drawoption = "FadcResponseChannels_.estimatedNPE_>>+ " + histname.str();
      dataTree->Draw(drawoption.c_str(), (basecut&&omSelection&&stringCut&&(!atwdLargerThanFadc)), "hist");
      cout << nDOM << endl;
    }

    //
    // summarize the NPE - DOM# information into TGraph
    //
    TCanvas *canvas_StringNPE = new TCanvas("StringNPE","NPE stingwise", 800,500);
    canvas_StringNPE->cd(1);
    SetgPad();

    // PPC-Mie MC
    Double_t npe_mean[60];
    Double_t npe_rms[60];
    Double_t dom_number[60];
    Double_t dom_rms[60]; // dummy
    for(int nDOM = 1; nDOM<=60; nDOM++){  // DOM loop
      npe_mean[nDOM-1] =histDomWiseNpeMie[nDOM-1]->GetMean(); 
      npe_rms[nDOM-1] =histDomWiseNpeMie[nDOM-1]->GetRMS(); 
      dom_number[nDOM-1] = (Double_t )nDOM;
      dom_rms[nDOM-1] = 0.0;
      //cout << npe_mean[nDOM-1] << " +-" << npe_rms[nDOM-1] << endl;
    }

    TGraphErrors* npeAlongStringMie = new TGraphErrors(60,dom_number,npe_mean,dom_rms,npe_rms);
    npeAlongStringMie->SetTitle("NPE on the String 55");
    npeAlongStringMie->SetMarkerColor(kBlue);
    npeAlongStringMie->SetMarkerStyle(21);
    npeAlongStringMie->GetYaxis()->SetRangeUser(npe_min,npe_max);
    npeAlongStringMie->GetYaxis()->SetTitle("NPE received in each DOM");
    npeAlongStringMie->GetXaxis()->SetTitle("DOM number");
    npeAlongStringMie->Draw("ALP");

    // phtonics-Spice MC
    for(int nDOM = 1; nDOM<=60; nDOM++){  // DOM loop
      npe_mean[nDOM-1] =histDomWiseNpeSpice[nDOM-1]->GetMean(); 
      npe_rms[nDOM-1] =histDomWiseNpeSpice[nDOM-1]->GetRMS(); 
      dom_number[nDOM-1] = (Double_t )nDOM;
      dom_rms[nDOM-1] = 0.0;
      //cout << npe_mean[nDOM-1] << " +-" << npe_rms[nDOM-1] << endl;
    }

    TGraphErrors* npeAlongStringSpice = new TGraphErrors(60,dom_number,npe_mean,dom_rms,npe_rms);
    npeAlongStringSpice->SetTitle("NPE on the String 55");
    npeAlongStringSpice->SetMarkerColor(kRed);
    //npeAlongStringSpice->SetMarkerColor(kGreen);
    npeAlongStringSpice->SetMarkerStyle(21);
    npeAlongStringSpice->GetYaxis()->SetRangeUser(npe_min,npe_max);
    npeAlongStringSpice->GetYaxis()->SetTitle("NPE received in each DOM");
    npeAlongStringSpice->GetXaxis()->SetTitle("DOM number");
    npeAlongStringSpice->Draw("LP");

    // data
    for(int nDOM = 1; nDOM<=60; nDOM++){  // DOM loop
      npe_mean[nDOM-1] =histDomWiseNpeData[nDOM-1]->GetMean(); 
      npe_rms[nDOM-1] =histDomWiseNpeData[nDOM-1]->GetRMS(); 
    }
    TGraphErrors* npeAlongStringData = new TGraphErrors(60,dom_number,npe_mean,dom_rms,npe_rms);
    npeAlongStringData->SetTitle("NPE on the String 55");
    npeAlongStringData->SetMarkerColor(kBlack);
    npeAlongStringData->SetMarkerStyle(21);
    npeAlongStringData->GetYaxis()->SetRangeUser(npe_min,npe_max);
    npeAlongStringData->Draw("LP");
    canvas_StringNPE->Update();

    if(doWrite){
      TFile *histFile = new TFile(histogramsStoreFileName.c_str(),"NEW");
      npeAlongStringMie->Write("npeAlongString55MieSC10per");
      npeAlongStringSpice->Write("npeAlongString55SpiceSC10per");
      npeAlongStringData->Write("npeAlongString55DataSC10per");
      histFile->Close();
    }

  } else { // event wise
    TCanvas *canvas_NPE = new TCanvas("NPE","Npe", 800,500);
    canvas_NPE->cd(1);
    canvas_NPE->SetHighLightColor(10);
    SetgPad();

    //mieTree->Draw("(DetectorResponseEvent_.totalEstimatedAtwdNPE_) >> mieNPE", (basecut), "hist");
    //spiceTree->Draw("(DetectorResponseEvent_.totalEstimatedAtwdNPE_) >> spiceNPE", (basecut), "hist");
    //dataTree->Draw("(DetectorResponseEvent_.totalEstimatedAtwdNPE_) >> dataNPE", (basecut), "hist");
    mieTree->Draw("(DetectorResponseEvent_.totalBestEstimatedNPE_) >> mieNPE", (basecut), "hist");
    spiceTree->Draw("(DetectorResponseEvent_.totalBestEstimatedNPE_) >> spiceNPE", (basecut), "hist");
    dataTree->Draw("(DetectorResponseEvent_.totalBestEstimatedNPE_) >> dataNPE", (basecut), "hist");
    cout<<endl;
    cout<<endl;
    cout<<"Number of entries: "<<dataNPE->GetEntries()<<endl;
    cout<<"\t Mean: "<<dataNPE->GetMean()<<endl;
    cout<<"\t Integral: "<<dataNPE->Integral()<<endl;
    cout<<endl;
    cout<<endl;
    Double_t scale = 1.0/mieNPE->Integral();
    mieNPE->Scale(scale);
    mieNPE->SetLineColor(kBlue);
    mieNPE->SetLineWidth(2);
    mieNPE->GetYaxis()->SetRangeUser(0.0,0.4);
    mieNPE->Draw("hist");
    scale = 1.0/spiceNPE->Integral();
    spiceNPE->Scale(scale);
    spiceNPE->SetLineColor(kRed);
    //spiceNPE->SetLineColor(kGreen);
    spiceNPE->SetLineWidth(2);
    spiceNPE->GetYaxis()->SetRangeUser(0.0,0.4);
    spiceNPE->Draw("same");
    scale = 1.0/dataNPE->Integral();
    dataNPE->Scale(scale);
    dataNPE->SetLineColor(kBlack);
    dataNPE->SetLineWidth(2);
    dataNPE->GetYaxis()->SetRangeUser(0.0,0.4);
    dataNPE->Draw("same");
    canvas_NPE->Update();

    cout << "mie mean NPE(" << mieNPE->GetMean() << ")" << endl;
    cout << "spice mean NPE(" << spiceNPE->GetMean() << ")" << endl;
    cout << "data mean NPE(" << dataNPE->GetMean() << ")" << endl;

    if(doWrite){
      TFile *histFile = new TFile(histogramsStoreFileName.c_str(),"NEW");
      mieNPE->Write("totalNpeMieSC10per");
      spiceNPE->Write("totalNpeSpiceSC10per");
      dataNPE->Write("totalNpeDataSC10per");
      histFile->Close();
    }
  }

 
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////

 cout << "exit" << endl;

}
////////////////////////////////
void SetStyle(){

  //gStyle->SetOptStat(1111);
  gStyle->SetOptStat(0000);

   gStyle->SetPadGridX(true);
   gStyle->SetPadGridY(true);

   gStyle->SetStatW(0.4);
   gStyle->SetStatFontSize(0.03);
   gStyle->SetTitleFontSize(18);
   gStyle->SetLabelSize(0.03,"x");
   gStyle->SetLabelSize(0.03,"y");
   gStyle->SetLabelSize(0.03,"z");
   gStyle->SetLabelOffset(0.002,"x");
   gStyle->SetNdivisions(1025,"x");
   gStyle->SetNdivisions(1010,"y");
   gStyle->SetTitleOffset(1.4,"y");
   gStyle->SetTitleOffset(0.8,"x");
   
   gStyle->SetTitleBorderSize(0);
   gStyle->SetTitleW(0.6);
   gStyle->SetTitleH(0.06);
   gStyle->SetOptTitle(1);
   gStyle->SetPalette(1);
   
}
void SetgPad(){
  double right  = 0.12;
  //double right  = 0.05;
  double left   = 0.12;
  double top    = 0.1;
  double bottom = 0.08;
  gPad->SetRightMargin(right);
  gPad->SetLeftMargin(left);
  gPad->SetTopMargin(top);
  gPad->SetBottomMargin(bottom);
}
