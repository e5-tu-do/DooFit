#include "doofit/plotting/Plot/PlotSimultaneous.h"

// STL
#include <vector>
#include <string>

// boost

// ROOT
#include "TList.h"
#include "TIterator.h"

// from RooFit
#include "RooSimultaneous.h"
#include "RooAbsData.h"
#include "RooAbsRealLValue.h"
#include "RooAbsCategoryLValue.h"
#include "RooCatType.h"

// from DooCore
#include "doocore/io/MsgStream.h"

// from Project

using namespace ROOT;
using namespace RooFit;
using namespace doocore::io;

namespace doofit {
namespace plotting {

PlotSimultaneous::PlotSimultaneous(const PlotConfig& cfg_plot, const RooAbsRealLValue& dimension, const RooAbsData& dataset, const RooSimultaneous& pdf, const std::vector<std::string>& components, const std::string& plot_name)
: Plot(cfg_plot, dimension, dataset, pdf, components, plot_name),
  components_regexps_(components)
{
  
}

void PlotSimultaneous::PlotHandler(bool logy, const std::string& suffix) const {
  const RooSimultaneous& pdf = *dynamic_cast<const RooSimultaneous*>(pdf_);
  const RooAbsData& data     = *datasets_.front();
  
  const RooAbsCategoryLValue& sim_cat = pdf.indexCat();
  TList* data_split = data.split(sim_cat);
  std::string plot_name;
  
  RooCatType* sim_cat_type = NULL;
  TIterator* sim_cat_type_iter = sim_cat.typeIterator();
  while((sim_cat_type=dynamic_cast<RooCatType*>(sim_cat_type_iter->Next()))) {
    RooAbsPdf& sub_pdf = *(pdf.getPdf(sim_cat_type->GetName()));
    if (&sub_pdf != NULL) {
      RooAbsData& sub_data = *dynamic_cast<RooAbsData*>(data_split->FindObject(sim_cat_type->GetName()));
      
      double min,max;
      RooRealVar var(dynamic_cast<const RooRealVar&>(dimension_));
      sub_data.getRange(var, min,max);
      sdebug << "Range: " << min << "," << max << endmsg;
      
      //plot_name = std::string(dimension_.GetName()) + "_" + sim_cat_type->GetName();
      plot_name = plot_name_ + "_" + sim_cat_type->GetName();
      Plot plot(config_plot_, dimension_, sub_data, sub_pdf, components_regexps_, plot_name);
      plot.plot_args_ = this->plot_args_;
      plot.AddPlotArg(Range(min,max));

      plot.PlotHandler(logy, suffix);
    }
  }
  
//  plot_name = std::string(dimension_.GetName()) + "_summed";
  plot_name = plot_name_ + "_summed";
  Plot plot(config_plot_, dimension_, data, *pdf_, components_regexps_, plot_name);
  plot.plot_args_ = this->plot_args_;
  plot.AddPlotArg(ProjWData(sim_cat,data));
  plot.PlotHandler(logy, suffix);
  
  TIter next(data_split);
  TObject *obj = NULL;
  while ((obj = next())) {
    delete obj;
  }
}
  
} // namespace plotting
} // namespace doofit
