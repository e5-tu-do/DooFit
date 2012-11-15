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
  components_(components)
{
  
}

void PlotSimultaneous::PlotHandler(bool logy, const std::string& suffix) const {
  const RooSimultaneous& pdf = *dynamic_cast<const RooSimultaneous*>(pdfs_.first());
  const RooAbsData& data     = *datasets_.front();
  
  const RooAbsCategoryLValue& sim_cat = pdf.indexCat();
  TList* data_split = data.split(sim_cat);
  
  RooCatType* sim_cat_type = NULL;
  TIterator* sim_cat_type_iter = sim_cat.typeIterator();
  while((sim_cat_type=dynamic_cast<RooCatType*>(sim_cat_type_iter->Next()))) {
    RooAbsPdf& sub_pdf = *(pdf.getPdf(sim_cat_type->GetName()));
    if (&sub_pdf != NULL) {
      RooAbsData& sub_data = *dynamic_cast<RooAbsData*>(data_split->FindObject(sim_cat_type->GetName()));
      
      std::string plot_name;
      
      plot_name = std::string(dimension_.GetName()) + "_" + sim_cat_type->GetName();
      Plot plot(config_plot_, dimension_, sub_data, sub_pdf, components_, plot_name);
      plot.PlotHandler(logy, suffix);
    }
  }
  
  TIter next(data_split);
  TObject *obj = NULL;
  while ((obj = next())) {
    delete obj;
  }
}
  
} // namespace plotting
} // namespace doofit
