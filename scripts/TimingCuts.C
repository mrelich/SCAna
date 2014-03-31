//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
// This method will contain the timing cuts for SC. It is      //
// useful for making some plots to investigate a better filter //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//

#include "TCut.h"

//----------------------------------------------//
// Method to get the luminosity cuts for SC2
//----------------------------------------------//
TCut lumiCutSC2(TString lumi)
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

  // Construct the TCut
  TString var = "startUTCDaqTime_";
  TCut cut = TCut((low+"<"+var+"&&"+var+"<"+high).Data());
  return cut;

}

//----------------------------------------------//
// Method to get the lumi cuts for SC1
// This is actually less clear because I don't
// know how to associate them with a given lumi
// so I will call them reg1, reg2, etc.
//----------------------------------------------//
TCut lumiCutSC1(TString lumi)
{

  TString low  = "0";
  TString high = "100";
  float factor = "*10e14"

  if(lumi == "reg1"){
    low  = "276.679"+factor; 
    high = "276.702"+factor;
  }
  else if(lumi == "reg2"){
    low  = "276.702"+factor;
    high = "276.723"+factor;
  }
  else if(lumi == "reg3"){
    low  = "276.723"+factor;
    high = "276.7449"+factor;
  }
  else if(lumi == "reg4"){
    low  = "276.7449"+factor;
    high = "276.7655"+factor;
  }
  else if(lumi == "reg5"){
    low  = "276.7655"+factor;
    high = "276.787"+ factor;
  }
  else if(lumi == "reg6"){
    low  = "276.787"+ factor;
    high = "276.8069"+factor;
  }
  else if(lumi == "reg7"){
    low  = "276.8069"+factor;
    high = "276.829"+factor;
  }
  else
    cout<<"Lumi not known"<<endl;

  // Construct the TCut
  TString var = "startUTCDaqTime_";
  TCut cut = TCut((low+"<"+var+"&&"+var+"<"+high).Data());
  return cut;

}
