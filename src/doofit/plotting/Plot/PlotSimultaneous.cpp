#include "doofit/plotting/Plot/PlotSimultaneous.h"

// STL
#include <vector>
#include <string>

// boost

// ROOT
#include "TList.h"
#include "TIterator.h"
#include "TCanvas.h"
#include "TPaveText.h"

// from RooFit
#include "RooSimultaneous.h"
#include "RooAbsData.h"
#include "RooAbsRealLValue.h"
#include "RooAbsCategoryLValue.h"
#include "RooCatType.h"
#include "RooDataHist.h"
#include "RooSuperCategory.h"
#include "RooCategory.h"

// from DooCore
#include "doocore/io/MsgStream.h"
#include "doocore/lutils/lutils.h"

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

void PlotSimultaneous::PlotHandler(ScaleType sc_y, std::string suffix) const {
  const RooSimultaneous& pdf = *dynamic_cast<const RooSimultaneous*>(pdf_);
  const RooAbsData& data     = *datasets_.front();
  RooAbsCategoryLValue& sim_cat = const_cast<RooAbsCategoryLValue&>(pdf.indexCat());
  TList* data_split = data.split(sim_cat);
  std::string plot_name;
  
  RooCatType* sim_cat_type = NULL;
  TIterator* sim_cat_type_iter = sim_cat.typeIterator();
  int num_slices = 0;
  while((sim_cat_type=dynamic_cast<RooCatType*>(sim_cat_type_iter->Next()))) {
    RooAbsPdf& sub_pdf = *(pdf.getPdf(sim_cat_type->GetName()));
    if (&sub_pdf != NULL) {
      RooAbsData& sub_data = *dynamic_cast<RooAbsData*>(data_split->FindObject(sim_cat_type->GetName()));
      
      
      sim_cat.setIndex(sim_cat_type->getVal());
      
      std::string cut_string = "";
      const RooSuperCategory* super_cat = dynamic_cast<const RooSuperCategory*>(&sim_cat);
      const RooCategory* std_cat        = dynamic_cast<const RooCategory*>(&sim_cat);
      if (super_cat != NULL) {
        RooLinkedListIter* it  = (RooLinkedListIter*)super_cat->inputCatList().createIterator();
        RooAbsArg*         arg = NULL;
        
        while ((arg=(RooAbsArg*)it->Next())) {
          RooCategory* cat = dynamic_cast<RooCategory*>(arg);
          if (cat != NULL) {
            if (cut_string.length() > 0) cut_string = cut_string + "&&";
            cut_string = cut_string + cat->GetName() + "==" + std::to_string(cat->getIndex());
          } else {
            serr << "Error in PlotSimultaneous::PlotHandler(...): Cannot handle category component " << arg->GetName() << endmsg;
          }
        }
        
        sdebug << "Cut string: " << cut_string << endmsg;
        
        delete it;
      } else if (std_cat != NULL) {
        cut_string = std::string(std_cat->GetName()) + "==" + std::to_string(std_cat->getIndex());
        sdebug << "Cut string: " << cut_string << endmsg;
      }

      RooAbsData& sub_data2 = data.reduce(Cut(cut_string.c_str()));
      sub_data.Print();
      sub_data2.Print();
      
      if (&sub_data == NULL) {
        serr << "PlotSimultaneous::PlotHandler(...): sub dataset for category " << sim_cat_type->GetName() << " empty. Will not plot. " << endmsg;
      } else {
        double min,max;
        RooRealVar var(dynamic_cast<const RooRealVar&>(dimension_));
        sub_data.getRange(var, min, max);
        //sdebug << "Range: " << min << "," << max << endmsg;
        
        //plot_name = std::string(dimension_.GetName()) + "_" + sim_cat_type->GetName();
        plot_name = plot_name_ + "_" + sim_cat_type->GetName();
        Plot plot(config_plot_, dimension_, sub_data, sub_pdf, components_regexps_, plot_name);
        plot.plot_args_ = this->plot_args_;
        plot.plot_range_ = this->plot_range_;
        
        // 20130905 FK: deactivated this manual setting of the plot range as it
        //              can dramatically increase plot time. Maybe need to
        //              rethink that later
        //plot.AddPlotArg(Range(min,max));
        
        // go through supplied cmd args and if necessary adapt ProjWData argument
        bool project_arg_found         = false;
        RooArgSet* set_project         = NULL;
        RooAbsData* data_reduced       = NULL;
        const RooAbsData* data_project = NULL;
        bool binned_projection         = false;
        for (std::vector<RooCmdArg>::iterator it = plot.plot_args_.begin();
             it != plot.plot_args_.end(); ++it) {
          if (std::string(it->GetName()) == "ProjData") {
            sinfo << "Found ProjWData() argument. Will change projection dataset accordingly." << endmsg;
            project_arg_found = true;
            binned_projection = it->getInt(0);
            
            // check for binned projection and if so generate binned dataset here to
            // accelerate projection
            if (binned_projection) {
              set_project = new RooArgSet(*dynamic_cast<const RooArgSet*>(it->getObject(0)));
              
              sinfo << " Binned projection is requested. Will generate a binned dataset to accelerate projection." << endmsg;


              data_reduced = sub_data.reduce(*set_project);
              std::string name_data_hist = std::string(sub_data.GetName()) + "_hist" + sim_cat_type->GetName();
              data_project = new RooDataHist(name_data_hist.c_str(), "binned projection dataset", *data_reduced->get(), *data_reduced);
              
              sinfo << " Created binned dataset with " << data_project->numEntries() << " bins." << endmsg;
            } else {
              data_project = &sub_data;
            }

            
            // create the new projection argument
            *it = ProjWData(*dynamic_cast<const RooArgSet*>(it->getObject(0)),
                            *data_project,
                            it->getInt(0));
          }
        }
        
        doocore::lutils::setStyle("LHCb");
        config_plot_.OnDemandOpenPlotStack();
        TCanvas c1("c1","c1",900,900);
        std::string label_text1 = std::string("Next plots: dimension ") + dimension_.GetName() + ", category " + sim_cat_type->GetName();
        std::string label_text2 = std::string(" (") + plot_name + ")";
        TPaveText label(0.1, 0.25, 0.9, 0.75);
        label.SetTextSize(0.03);
        label.AddText(label_text1.c_str());
        label.AddText(label_text2.c_str());
        label.SetLineWidth(0.0);
        label.SetTextAlign(13);
        label.SetFillColor(0);
        label.Draw();
        c1.Print(std::string(config_plot_.plot_directory()+"/pdf/AllPlots"+config_plot_.plot_appendix()+".pdf").c_str());
        
        plot.PlotHandler(sc_y, suffix);
        
        if (set_project != NULL) delete set_project;
        if (binned_projection) {
          delete data_project;
          delete data_reduced;
        }
        
        ++num_slices;
      }
    }
  }
  
  if (num_slices > 1) {
    TCanvas c1("c1","c1",900,900);
    std::string label_text1 = std::string("Next plots: dimension ") + dimension_.GetName() + ", summed all categories ";
    std::string label_text2 = std::string(" (") + plot_name + ")";
    TPaveText label(0.1, 0.25, 0.9, 0.75);
    label.SetTextSize(0.03);
    label.AddText(label_text1.c_str());
    label.AddText(label_text2.c_str());
    label.SetLineWidth(0.0);
    label.SetTextAlign(13);
    label.SetFillColor(0);
    label.Draw();
    c1.Print(std::string(config_plot_.plot_directory()+"/pdf/AllPlots"+config_plot_.plot_appendix()+".pdf").c_str());
    
    //  plot_name = std::string(dimension_.GetName()) + "_summed";
    plot_name = plot_name_ + "_summed";
    Plot plot(config_plot_, dimension_, data, *pdf_, components_regexps_, plot_name);
    plot.plot_args_ = this->plot_args_;
    plot.plot_range_ = this->plot_range_;
    
    // go through supplied cmd args and if necessary merge ProjWData arguments
    bool project_arg_found         = false;
    RooArgSet* set_project         = NULL;
    RooAbsData* data_reduced       = NULL;
    const RooAbsData* data_project = NULL;
    bool binned_projection         = false;
    for (std::vector<RooCmdArg>::iterator it = plot.plot_args_.begin();
         it != plot.plot_args_.end(); ++it) {
      if (std::string(it->GetName()) == "ProjData") {
        sinfo << "Found ProjWData() argument. Will join with " << sim_cat.GetName() << " on same dataset." << endmsg;
        project_arg_found = true;
        binned_projection = it->getInt(0);
        
        // copy argset for projection and add simultaneous category variables
        set_project = new RooArgSet(*dynamic_cast<const RooArgSet*>(it->getObject(0)));
        const RooSuperCategory* super_sim_cat = &dynamic_cast<const RooSuperCategory&>(sim_cat);
        if (super_sim_cat) {
          // The simultaneous category is a super category and will not be in the
          // supplied dataset. Add input categories instead.
          set_project->add(super_sim_cat->inputCatList());
        } else {
          set_project->add(sim_cat);
        }
        
        // check for binned projection and if so generate binned dataset here to
        // accelerate projection
        if (binned_projection) {
          sinfo << " Binned projection is requested. Will generate a binned dataset to accelerate projection." << endmsg;
          
          RooAbsData* data_proj = const_cast<RooAbsData*>(dynamic_cast<const RooAbsData*>(it->getObject(1)));
          std::string name_data_hist = std::string(data_proj->GetName()) + "_hist";
          data_reduced = data_proj->reduce(*set_project);
          data_project = new RooDataHist(name_data_hist.c_str(), "binned projection dataset", *data_reduced->get(), *data_reduced);
          
          sinfo << " Created binned dataset with " << data_project->numEntries() << " bins." << endmsg;
        } else {
          data_project = dynamic_cast<const RooAbsData*>(it->getObject(1));
        }
        
        // create the new projection argument
        *it = ProjWData(*set_project,
                        *data_project,
                        it->getInt(0));
      }
    }
    if (!project_arg_found) plot.AddPlotArg(ProjWData(sim_cat,data));
    
    plot.PlotHandler(sc_y, suffix);

    if (set_project != NULL) delete set_project;
    if (binned_projection) {
      delete data_reduced;
      delete data_project;
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
