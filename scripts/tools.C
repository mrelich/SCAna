
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//           A class to house basic tools useful for plotting             //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TString.h"
#include "TFile.h"

#include <vector>

//--------------------------------------------------------//
// Some constants
//--------------------------------------------------------//

const int m_colors[5]  = {kBlack, kBlue, kRed, kGreen+2, kMagenta};
const int m_markers[5] = {20, 25, 24, 26, 22};

//--------------------------------------------------------//
// Retrieve a TH1 from file
//--------------------------------------------------------//
TH1* getHist(TFile* file, TString pname, TString xtitle,
	     TString ytitle, int color, int marker)
{
  
  TH1* hist = (TH1*) file->Get(pname.Data())->Clone(Form("%s_%i",pname.Data(),color));
  setAtt(hist, xtitle, ytitle, color, marker);

}

//--------------------------------------------------------//
// Set histogram attributes
//--------------------------------------------------------//
void setAtt(TH1* &h, TString xtitle, TString ytitle, 
	    int color, int marker)
{

  h->GetXaxis()->SetTitle(xtitle.Data());
  h->GetYaxis()->SetTitle(ytitle.Data());
  h->SetLineColor(color);
  h->SetMarkerColor(color);
  h->SetMarkerStyle(marker);
  h->SetMarkerSize(0.5);
  h->SetTitle("");
  h->SetStats(0);

  h->GetYaxis()->SetTitleOffset(1.5);
  h->SetLineWidth(2);

}

//--------------------------------------------------------//
// Set histogram attributes
//--------------------------------------------------------//
void setAtt(TH2* &h, TString xtitle, TString ytitle)
{

  h->GetXaxis()->SetTitle(xtitle.Data());
  h->GetYaxis()->SetTitle(ytitle.Data());
  h->SetTitle("");
  h->SetStats(0);
  h->GetYaxis()->SetTitleOffset(1.5);

}

//--------------------------------------------------------//
// Set graph Attributes
//--------------------------------------------------------//
void setAtt(TGraph* &gr, int color, int marker)
{

  gr->SetMarkerStyle(marker);
  gr->SetMarkerColor(color);
  gr->SetLineColor(color);
  gr->SetMarkerSize(1.);


}

//--------------------------------------------------------//
// Make Canvas
//--------------------------------------------------------//
TCanvas* makeCanvas(TString name)
{

  TCanvas* c = new TCanvas(name.Data(),name.Data(),700,600);
  c->SetTopMargin(0.05);
  c->SetRightMargin(0.08);
  c->SetLeftMargin(0.12);
  c->SetBottomMargin(0.10);

  return c;

}

//--------------------------------------------------------//
// Make legend
//--------------------------------------------------------//
TLegend* makeLegend(float x0, float x1, float y0, float y1)
{

  TLegend* leg = new TLegend(x0,y0,x1,y1);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetLineColor(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  return leg;

}

//--------------------------------------------------------//
// Latex object
//--------------------------------------------------------//
TLatex* makeLatex()
{

  TLatex* lat = new TLatex();
  lat->SetTextSize(0.04);
  lat->SetNDC();
  lat->SetTextFont(42);
  lat->SetTextColor(kBlack);
  return lat;

}

//--------------------------------------------------------//
// Make frame histogram
//--------------------------------------------------------//
TH1F* makeFrame(TString name, int nbins, float xmin, float xmax,
		TString xtitle, TString ytitle)
{

  TH1F* h = new TH1F(name.Data(),"",nbins,xmin,xmax);
  setAtt(h, xtitle, ytitle, kBlack, 20);
  return h;

}

