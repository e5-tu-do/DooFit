#include "CorrelationPlot.h"

// STL
#include <string>

// from Boost
#include <boost/filesystem.hpp>

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
#include <doocore/config/Summary.h>

doofit::plotting::correlations::CorrelationPlot::CorrelationPlot(const RooFitResult& fit_result, const std::vector<std::string>& plot_pars, double correlation_threshold, double label_scale, double marker_scale)
: fit_result_(fit_result),
  plot_pars_(plot_pars),
  correlation_threshold_(correlation_threshold),
  label_scale_(label_scale),
  marker_scale_(marker_scale)
{
}

void doofit::plotting::correlations::CorrelationPlot::PlotHandler(const std::string& plot_path) const {
  namespace lu = doocore::lutils;
  using namespace doocore::io;

  if (&fit_result_ == nullptr) {
    serr << "Error in CorrelationPlot::PlotHandler(...): Fit result invalid." << endmsg;
  } else {
    
    gROOT->SetStyle("Plain");
    lu::setStyle("LHCbOptimized");

    RooArgList par_list_float_final = fit_result_.floatParsFinal();
    TH2* hist_corr = fit_result_.correlationHist();
    std::string use_selected_pars_appendix = "";

    // only plot selected pars
    std::vector<std::string> correlated_pars;
    if (!plot_pars_.empty()){ // vector is not empty
      use_selected_pars_appendix = "_selected";
      correlated_pars = plot_pars_;
      // add all pars that have a correlation above the
      // 'correlation_threshold_' to the list of selected pars
      if (correlation_threshold_ > 0.0){
        for (unsigned int i = 0; i < par_list_float_final.getSize(); i++){
          std::string par_name_i = par_list_float_final.at(i)->GetName();
          for (auto par_name : plot_pars_){
            if ((fit_result_.correlation(par_name.c_str(), par_name_i.c_str()) > correlation_threshold_ ) && (std::find(plot_pars_.begin(), plot_pars_.end(), par_name_i) == plot_pars_.end())) correlated_pars.push_back(par_name_i);
          }
        }
      }
      // sinfo << correlated_pars << endmsg;

      // create new RooArgList with all selected pars
      RooArgList par_list_float_selected;
      for (auto name : correlated_pars){
        if (par_list_float_final.find(name.c_str())!=nullptr){
          // sinfo << "adding " << name << endmsg;
          par_list_float_selected.add(*par_list_float_final.find(name.c_str()));
        }
      }
      // par_list_float_selected.Print();

      // create histogram
      int nbins = par_list_float_selected.getSize();
      TH2D* hist_selected = new TH2D("hist_selected", "hist_selected", nbins, 0, nbins, nbins, 0, nbins);

      int bin_x = 1;
      int bin_y = correlated_pars.size();
      for (auto var_x : correlated_pars){
        for (auto var_y : correlated_pars){
          double correlation = fit_result_.correlation(var_x.c_str(), var_y.c_str());
          // sinfo << bin_x << " " << bin_y << " " << correlation << endmsg;
          hist_selected->SetBinContent(bin_x, bin_y, correlation);
          // hist_selected->Print();

          hist_selected->GetXaxis()->SetBinLabel(bin_x,var_x.c_str()) ;
          hist_selected->GetYaxis()->SetBinLabel(bin_y,var_y.c_str()) ;


          bin_y--;
        }
        bin_y = correlated_pars.size();
        bin_x++;
      }
      // hist_corr->Print();
      delete hist_corr; hist_corr = nullptr;
      hist_corr = hist_selected;
      // hist_corr->Print();
      par_list_float_final.removeAll();
      par_list_float_final.add(par_list_float_selected);
    }

    // fit_result_.Print();
    // hist_corr->Print();
    // par_list_float_final.Print();
    
    // label sizes
    // double nbins = hist_corr->GetNbinsX();
    double label_size_x_axis = 0.02 * label_scale_;
    double label_size_y_axis = 0.02 * label_scale_;
    double label_size_z_axis = 0.03;
    double marker_size = 0.6 * marker_scale_;

    double max_xsize = 0.0;
    TAxis* axis = NULL;
    axis = hist_corr->GetXaxis();
    axis->SetLabelSize(label_size_x_axis);
    axis->SetLabelFont(42); 
    axis->LabelsOption("v");
    axis->SetLabelOffset(0.005);
    RenameAxisLabel(axis, par_list_float_final);
    axis = hist_corr->GetYaxis();
    axis->SetLabelSize(label_size_y_axis);
    axis->SetLabelFont(42);
    axis->LabelsOption("h");
    axis->SetLabelOffset(0.005);
    max_xsize = RenameAxisLabel(axis, par_list_float_final) * label_scale_;
    
  //  sdebug << "max_xsize = " << max_xsize << endmsg;
    
    TCanvas canvas("canvas","canvas",800,600);
    canvas.SetRightMargin(0.15);
    canvas.SetLeftMargin(max_xsize*0.35); 
    canvas.SetTopMargin(0.05);
    canvas.SetBottomMargin(max_xsize*0.45);
    
    TPaletteAxis *pal = NULL;
    
    lu::setRedBluePalette(hist_corr);
    hist_corr->Draw("COLZ");
    canvas.Update();
    pal = (TPaletteAxis*)(hist_corr->GetListOfFunctions()->FindObject("palette"));
    pal->GetAxis()->SetLabelSize(label_size_z_axis);
    lu::printPlot(&canvas,"FitResultsCorrMatrix_RedBlue"+use_selected_pars_appendix,plot_path);
    
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

    if (hist_corr->GetNbinsX() > 15) {
      gStyle->SetPaintTextFormat(".1f");
    } else {
      gStyle->SetPaintTextFormat(".2f");
    }
    
    float max = hist_corr->GetBinContent(hist_corr->GetMaximumBin());
    float min = hist_corr->GetBinContent(hist_corr->GetMinimumBin());
    float range = lu::Max(max, min);
    hist_corr->SetAxisRange(-range, range, "Z");
    hist_corr->Draw("COLZ");
    canvas.Update();
    pal = (TPaletteAxis*)(hist_corr->GetListOfFunctions()->FindObject("palette"));
    pal->GetAxis()->SetLabelSize(label_size_z_axis);
    lu::printPlot(&canvas,"FitResultsCorrMatrix_RedBlueDiscrete"+use_selected_pars_appendix,plot_path);
    
    hist_corr->SetMarkerSize(marker_size);
    hist_corr->Draw("COLZTEXT");
    canvas.Update();
    pal = (TPaletteAxis*)(hist_corr->GetListOfFunctions()->FindObject("palette"));
    pal->GetAxis()->SetLabelSize(label_size_z_axis);
    lu::printPlot(&canvas,"FitResultsCorrMatrix_RedBlueDiscrete_wText"+use_selected_pars_appendix,plot_path);
    
    namespace fs = boost::filesystem;
    doocore::config::Summary::GetInstance().AddFile(fs::path(plot_path) / fs::path("pdf/FitResultsCorrMatrix_RedBlueDiscrete_wText"+use_selected_pars_appendix+".pdf"));

    delete hist_corr;
  }
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
