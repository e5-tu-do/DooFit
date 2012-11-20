#include "doofit/toy/ToyFactoryStd/ToyFactoryStdConfig.h"

// STL
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <map>
#include <utility>

// Boost

// ROOT
#include "TIterator.h"
#include "TMath.h"
#include "TFile.h"
#include "TClass.h"

// RooFit
#include "RooAbsPdf.h"
#include "RooAbsArg.h"
#include "RooWorkspace.h"

// from project
#include "doocore/io/MsgStream.h"

using namespace std;
namespace po = boost::program_options;
using namespace doocore::io;

namespace doofit {
namespace toy {
  ToyFactoryStdConfig::ToyFactoryStdConfig() : 
  config::AbsConfig("empty_name"),
  generation_pdf_(NULL),
  expected_yield_(0),
  argset_generation_observables_(NULL),
  workspace_(NULL),
  generation_pdf_workspace_(""),
  argset_generation_observables_workspace_(""),
  random_seed_(0),
  argset_constraining_pdfs_(NULL),
  ws_file_(NULL)
  {
    swarn << "Usage of ToyFactoryStdConfig::ToyFactoryStdConfig() is not recommended!" <<endmsg;
  }
  
  ToyFactoryStdConfig::ToyFactoryStdConfig(const std::string& name) :
  config::AbsConfig(name),
  generation_pdf_(NULL),
  expected_yield_(0),
  argset_generation_observables_(NULL),
  workspace_(NULL),
  generation_pdf_workspace_(""),
  argset_generation_observables_workspace_(""),
  random_seed_(0),
  argset_constraining_pdfs_(NULL),
  ws_file_(NULL)
  {
  }
  
  ToyFactoryStdConfig::~ToyFactoryStdConfig() {
    if (ws_file_ != NULL) {
      if (workspace_ != NULL) {
        delete workspace_;
      }
      ws_file_->Close();
      delete ws_file_;
    }
  }
  
  RooWorkspace* ToyFactoryStdConfig::workspace() const {
    if (workspace_ != NULL) {
      return workspace_;
    }
    
    if (workspace_filename_name_.first().length() > 0) {
      // need to load workspace (at least try)
      ws_file_ = new TFile(workspace_filename_name_.first().c_str(),"read");
      
      if (ws_file_->IsZombie() || !ws_file_->IsOpen()) {
        ws_file_->Close();
        delete ws_file_;
        
        serr << "Cannot open file " << workspace_filename_name_.first() << endmsg;
        throw WorkspaceNotSetException();
      }
      
      workspace_ = (RooWorkspace*)ws_file_->Get(workspace_filename_name_.second().c_str());
      if (workspace_ == NULL) {
        ws_file_->Close();
        delete ws_file_;
        
        serr << "Cannot load workspace " << workspace_filename_name_.second() << " from valid file " << workspace_filename_name_.first() << endmsg;
        throw WorkspaceNotSetException();
      }
      return workspace_;
    } else {
      return NULL;
    }
  }
  
  RooAbsPdf* ToyFactoryStdConfig::generation_pdf() const {
    if (generation_pdf_) { 
      return generation_pdf_;
    } else if (generation_pdf_workspace_.length() > 0 && workspace() && workspace()->pdf(generation_pdf_workspace_.c_str())) {
      return workspace()->pdf(generation_pdf_workspace_.c_str());
    } else {
      throw PdfNotSetException();
    }  
  }
  
  const RooArgSet* ToyFactoryStdConfig::argset_generation_observables() const {
    if (argset_generation_observables_) { 
      return argset_generation_observables_;
    } else if (argset_generation_observables_workspace_.length() > 0 && workspace() && workspace()->set(argset_generation_observables_workspace_.c_str())) {
      return workspace()->set(argset_generation_observables_workspace_.c_str());
    } else {
      throw ArgSetNotSetException();
    }  
  }
  
  const RooArgSet* ToyFactoryStdConfig::argset_constraining_pdfs() const {
    if (argset_constraining_pdfs_) { 
      return argset_constraining_pdfs_;
    } else if (argset_constraining_pdfs_workspace_.length() > 0 && workspace() && workspace()->set(argset_constraining_pdfs_workspace_.c_str())) {
      return workspace()->set(argset_constraining_pdfs_workspace_.c_str());
    } else {
      return NULL;
    }  
  }
  
  void ToyFactoryStdConfig::set_generation_pdf_workspace(const std::string& name) {
    generation_pdf_workspace_ = name;
    if (generation_pdf_) {
      swarn << "WARNING: Setting PDF to be loaded from workspace although it is already directly specified. Will ignore workspace." << endmsg;
    }
  }
  
  void ToyFactoryStdConfig::set_argset_generation_observables_workspace(const std::string& name) {
    argset_generation_observables_workspace_ = name;
    if (argset_generation_observables_) {
      swarn << "WARNING: Setting argument set to be loaded from workspace although it is already directly specified. Will ignore workspace." << endmsg;
    }
  }
  
  void ToyFactoryStdConfig::set_argset_constraining_pdfs_workspace(const std::string& name) {
    argset_constraining_pdfs_workspace_ = name;
    
    if (argset_constraining_pdfs_) {
      swarn << "WARNING: Setting argument set for constraining PDFs to be loaded from workspace although it is already directly specified. Will ignore workspace." << endmsg;
    }
  }
  
  void ToyFactoryStdConfig::PrintOptions() const {
    scfg << "Random seed:               " << random_seed() << endmsg;
    
    try {
      scfg << "Generation PDF:            " << generation_pdf()->IsA()->GetName() << ":"
      << generation_pdf()->GetName() << " (" << generation_pdf()->GetTitle()
      << ")" << endmsg;
    } catch (const PdfNotSetException& e) {
      scfg << "Generation PDF not set." << endmsg;
    }
    
    scfg << "Expected yield:            ";
    if (expected_yield_ == 0) {
      scfg << "(determined by PDF itself)" << endmsg;
    } else {
      scfg << expected_yield_ << endmsg;
    }
    
    scfg << "Observables to generate directly: ";
    try {
      TIterator* arg_it = argset_generation_observables()->createIterator();
      RooAbsArg* arg = NULL;
      bool first = true;
      
      while ((arg = (RooAbsArg*)arg_it->Next())) {
        if (!first) {
          scfg << ", ";
        } else {
          first = false;
        }
        
        scfg << arg->GetName();
      }
      scfg << endmsg;
      delete arg_it;
    } catch (const ArgSetNotSetException& e) {
      scfg << "(not set)" << endmsg;
    }
    
    scfg << "Fixed size dataset:        " << dataset_size_fixed_ << endmsg;
    
    for (vector<config::DiscreteProbabilityDistribution>::const_iterator it = discrete_probabilities_.begin(); it != discrete_probabilities_.end(); ++it) {
      scfg << "Discrete probability:      " << *it << endmsg;
    }
    for (vector<config::CommaSeparatedPair>::const_iterator it = proto_sections_.begin(); it != proto_sections_.end(); ++it) {
      scfg << "Proto dataset section:     " << *it << endmsg;
    }
    
    scfg << "Constraining PDFs:         ";
    if (argset_constraining_pdfs()) {
      TIterator* arg_it = argset_constraining_pdfs()->createIterator();
      RooAbsArg* arg = NULL;
      bool first = true;
      
      while ((arg = (RooAbsArg*)arg_it->Next())) {
        if (!first) {
          scfg << ", ";
        } else {
          first = false;
        }
        
        scfg << arg->GetName();
      }
      scfg << endmsg;
      delete arg_it;
    }
    
    scfg << "Workspace:                 ";
    if (workspace()) {
      scfg << "(is set)" << endmsg;
      
      scfg << " PDF name (workspace):     " << generation_pdf_workspace_ << endmsg;
      scfg << " Argset name observables (workspace):       " << argset_generation_observables_workspace_ << endmsg;
      scfg << " Argset name constraining PDFs (workspace): " << argset_constraining_pdfs_workspace_ << endmsg;
    } else {
      scfg << "(is not set)" << endmsg;
    }

    if (workspace_filename_name_.first().length() > 0) {
      scfg << "Workspace from file:       " << workspace_filename_name_ << endmsg;
    }
    if (dataset_filename_name_.first().length() > 0) {
      scfg << "File to save dataset to:   " << dataset_filename_name_ << endmsg;
    }
    
    if (parameter_read_file_.length() > 0) {
      scfg << "Read parameters from:      " << parameter_read_file_ << endmsg;
    }
    if (parameter_save_file_.length() > 0) {
      scfg << "Save parameters to:        " << parameter_save_file_ << endmsg;
    }
  }
  
  void ToyFactoryStdConfig::DefineOptions() {
    po::options_description* generation = new po::options_description("Toy generation options");
    
    generation->add_options()
    (GetOptionString("random_seed").c_str(), po::value<int>(&random_seed_)->default_value(0),
     "Random seed to use for the toy generation (set to 0 for random seed)")
    (GetOptionString("expected_yield").c_str(), po::value<double>(&expected_yield_)->default_value(0),
     "Expected yield for toy sample (set to 0 for usage of expected yield in extended likelihood PDF)")
    (GetOptionString("pdf_name_ws").c_str(), po::value<string>(&generation_pdf_workspace_),
     "Name of PDF to generate for on linked workspace")
    (GetOptionString("argset_name_ws").c_str(), po::value<string>(&argset_generation_observables_workspace_),
     "Name of variables argset to generate for on linked workspace")
    (GetOptionString("argset_constraining_name_ws").c_str(), po::value<string>(&argset_constraining_pdfs_workspace_),
     "Name of constraining PDFs argset to use on linked workspace")
    (GetOptionString("discrete_probabilities").c_str(), po::value<vector<config::DiscreteProbabilityDistribution> >(&discrete_probabilities_)->composing(), "Discrete probability distribution for variables (can be multiply defined). The string representation is var_name,value1,prob1,value2,prob2,...,valueN,probN")
    (GetOptionString("proto_section").c_str(), po::value<vector<config::CommaSeparatedPair> >(&proto_sections_)->composing(), "Proto dataset generation section. Specify sub PDF name and config section to use for proto data for this PDF. String representation is pdf_name,section")
    (GetOptionString("dataset_size_fixed").c_str(), po::value<bool>(&dataset_size_fixed_)->default_value(false),"Set to true to generate a fixed size dataset (instead of poisson distributed size which is default)")
    (GetOptionString("workspace_filename_name").c_str(), po::value<config::CommaSeparatedPair>(&workspace_filename_name_),"Filename to load workspace from (if not set directly) and name of workspace in file (set as filename,workspace_name)")
    (GetOptionString("dataset_filename_name").c_str(), po::value<config::CommaSeparatedPair>(&dataset_filename_name_),"Filename to save generated dataset to and name of dataset in file (set as filename,dataset_name)")
    (GetOptionString("parameter_read_file").c_str(), po::value<string>(&parameter_read_file_),"Filename to read parameters from before generation")
    (GetOptionString("parameter_save_file").c_str(), po::value<string>(&parameter_save_file_),"Filename to save parameters to after generation");
    
    descs_visible_.push_back(generation);
  }
  
  void ToyFactoryStdConfig::LoadOptions() {
  }
} // namespace toy
} // namespace doofit

