#include "CorrelationPlot.h"

// STL
#include <string>

// from ROOT
#include "TROOT.h"
#include "TH2.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TPaletteAxis.h"
#include "TColor.h"
#include "TStyle.h"
#include "TLatex.h"

// from RooFit
#include "RooFitResult.h"
#include "RooArgList.h"

// from DooCore
#include <doocore/lutils/lutils.h>
#include <doocore/io/MsgStream.h>

doofit::plotting::correlations::CorrelationPlot::CorrelationPlot(const RooFitResult& fit_result)
: fit_result_(fit_result)
{
}

void doofit::plotting::correlations::CorrelationPlot::PlotHandler(const std::string& plot_path) const {
  namespace lu = doocore::lutils;
  using namespace doocore::io;
  
  gROOT->SetStyle("Plain");
  lu::setStyle();

  RooArgList par_list_float_final = fit_result_.floatParsFinal();
  TH2* hist_corr = fit_result_.correlationHist();
  
  double max_xsize = 0.0;
  TAxis* axis = NULL;
  axis = hist_corr->GetXaxis();
  axis->SetLabelSize(0.02);
  axis->SetLabelFont(42);
  axis->LabelsOption("v");
  RenameAxisLabel(axis, par_list_float_final);
  axis = hist_corr->GetYaxis();
  axis->SetLabelSize(0.02);
  axis->SetLabelFont(42);
  axis->LabelsOption("h");
  max_xsize = RenameAxisLabel(axis, par_list_float_final);
  
//  sdebug << "max_xsize = " << max_xsize << endmsg;
  
  TCanvas canvas("canvas","canvas",800,600);
  canvas.SetRightMargin(0.1);
  canvas.SetLeftMargin(max_xsize*0.4);
  canvas.SetTopMargin(0.05);
  canvas.SetBottomMargin(max_xsize*0.5);
  
  TPaletteAxis *pal = NULL;
  
  lu::setRedBluePalette(hist_corr);
  hist_corr->Draw("COLZ");
  canvas.Update();
  pal = (TPaletteAxis*)(hist_corr->GetListOfFunctions()->FindObject("palette"));
  pal->GetAxis()->SetLabelSize(0.03);
  lu::printPlot(&canvas,"FitResultsCorrMatrix_RedBlue",plot_path);
  
//  lu::setHotColdPalette(hist_corr);
//  hist_corr->Draw("COLZ");
//  canvas.Update();
//  pal = (TPaletteAxis*)(hist_corr->GetListOfFunctions()->FindObject("palette"));
//  pal->GetAxis()->SetLabelSize(0.02);
//  lu::printPlot(&canvas,"FitResultsCorrMatrix_HotCold",plot_path);
  
  const Int_t NRGBs = 9;
  const Int_t NCont = 40;
  
  Double_t stops[NRGBs] = { 0.00 , 0.25 , 0.35 , 0.48 , 0.50 , 0.52 , 0.65 , 0.75 , 1.00 };
  Double_t red[NRGBs]   = { 0.00 , 0.00 , 0.20 , 1.00 , 1.00 , 1.00 , 1.00 , 1.00 , 0.20 };
  Double_t green[NRGBs] = { 0.00 , 0.00 , 0.20 , 1.00 , 1.00 , 1.00 , 0.20 , 0.00 , 0.00 };
  Double_t blue[NRGBs]  = { 0.20 , 1.00 , 1.00 , 1.00 , 1.00 , 1.00 , 0.20 , 0.00 , 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
  gStyle->SetPaintTextFormat(".1f");
  
  float max = hist_corr->GetBinContent(hist_corr->GetMaximumBin());
  float min = hist_corr->GetBinContent(hist_corr->GetMinimumBin());
  float range = lu::Max(max, min);
  hist_corr->SetAxisRange(-range, range, "Z");
  hist_corr->Draw("COLZ");
  canvas.Update();
  pal = (TPaletteAxis*)(hist_corr->GetListOfFunctions()->FindObject("palette"));
  pal->GetAxis()->SetLabelSize(0.03);
  lu::printPlot(&canvas,"FitResultsCorrMatrix_RedBlueDiscete",plot_path);
  
  hist_corr->SetMarkerSize(0.6);
  hist_corr->Draw("COLZTEXT");
  canvas.Update();
  pal = (TPaletteAxis*)(hist_corr->GetListOfFunctions()->FindObject("palette"));
  pal->GetAxis()->SetLabelSize(0.03);
  lu::printPlot(&canvas,"FitResultsCorrMatrix_RedBlueDiscete_wText",plot_path);
}

double doofit::plotting::correlations::CorrelationPlot::RenameAxisLabel(TAxis* axis, const RooArgList& arg_list) const {
  using namespace doocore::io;
  
  double max_xsize = 0.0;
  
  for ( int bin = axis->GetFirst(); bin <= axis->GetLast(); ++bin ){
    TString bin_label = axis->GetBinLabel(bin);
    
    RooAbsArg* label_arg = arg_list.find(bin_label);
    if (label_arg == NULL ) {
      continue;
    } else {
      bin_label = label_arg->GetTitle();
    }
    
    TCanvas canvas("canvas","canvas",800,600);
    TLatex label(0,0,bin_label);
    label.Draw();
    double xsize = label.GetXsize();
    if (max_xsize <= xsize) max_xsize = xsize;
    
    axis->SetBinLabel(bin, bin_label);
  }
  
  return max_xsize;
}