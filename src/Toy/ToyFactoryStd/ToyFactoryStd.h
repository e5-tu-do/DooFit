#ifndef TOYFACTORYSTD_h
#define TOYFACTORYSTD_h

/** @namespace Toy
 *
 *  @brief Toy subsystem namespace.
 *
 *  This namespace is responsible for toy sample creation and toy studies.
 *
 */

// STL
#include <cstring>

// ROOT
#include "TClass.h"

// from RooFit
#include "RooAbsPdf.h"

// from project
#include "Toy/ToyFactoryStd/ToyFactoryStdConfig.h"

// forward declarations
class CommonConfig; 
class RooDataSet;
class RooArgSet;

namespace Toy {
  /** \class ToyFactoryStd
   *  \brief Standard toy factory for DooFit generating toy samples
   *
   *  This class handles generation of toy samples for a given pdf. Generation 
   *  should be as smart as possible involving automatic decomposition of more 
   *  complex pdfs and generation of proto datasets based on given distributions.
   * 
   *  @section general General usage
   *
   *  General configuration is handled via Toy::ToyFactoryStdConfig. One needs
   *  to set the PDF for generation (either via pointer to a RooAbsPdf or via 
   *  combination of workspace pointer and PDF name on workspace). The set of 
   *  observables to generate the toy dataset needs to be set as well (again 
   *  either directly or via name on workspace). 
   *
   *  Generation of the dataset can be invoked via Toy::ToyFactory::Generate().
   *
   *  @section proto-sets Proto dataset generation
   *
   *  The need for proto datasets cannot be detected automatically. Therefore, 
   *  one needs to specify for each sub PDF a section name. This section has to
   *  be included in the config file used to configure the toy factory. The toy
   *  factory will read the specified section and create a new toy factory based
   *  on the settings in this section to create the proto data. The generated 
   *  proto data will then be used for the specified sub PDF.
   *
   *  @see Toy::ToyFactoryStdConfig::set_proto_sections(const std::vector<Config::CommaSeparatedPair>&)
   *  @see Toy::ToyFactoryStdConfig::AddProtoSections(const Config::CommaSeparatedPair&)
   */
  
  class ToyFactoryStd {
  public:
    /**
     *  \brief Default constructor for ToyFactoryStd
     *
     *  This is the default constructor for ToyFactoryStd involving general 
     *  configuration @a cfg_com and specific configuration @a cfg_tfac.
     *
     *  @param cfg_com CommonConfig for the general configuration.
     *  @param cfg_tfac ToyFactoryStdConfig for this specific toy factory.
     */
    ToyFactoryStd(const CommonConfig& cfg_com, const ToyFactoryStdConfig& cfg_tfac);
    
    /**
     *  \brief Destructor for ToyFactoryStd
     */
    ~ToyFactoryStd();
    
    /**
     *  \brief Generate a toy sample
     *
     *  Based on all settings in the supplied ToyFactoryStdConfig object, generate
     *  a toy sample. Please note comment on ownership of returned dataset.
     *
     *  \return Pointer to the generated sample. ToyFactoryStd does not assume 
     *          ownership of this sample. Therefore, the invoker of this function
     *          must take care of proper deletion afterwards.
     */
    RooDataSet* Generate();
    
  protected:
    
  private:
    /** @name PDF type functions
     *  Functions to check for specific PDF types.
     */
    ///@{
    /**
     *  @brief Checks if a RooAbsPdf is decomposable
     *
     *  @param pdf pdf to check for decomposability
     *  @return whether pdf is decomposable or not
     */
    bool PdfIsDecomposable(const RooAbsPdf& pdf) const;
    
    /**
     *  @brief Checks if a RooAbsPdf is an extended pdf
     *
     *  @param pdf pdf to check for extended pdf
     *  @return whether pdf is extended or not
     */
    bool PdfIsExtended(const RooAbsPdf& pdf) const {
      if (std::strcmp(pdf.IsA()->GetName(),"RooExtendPdf") == 0) {
        return true;
      } else {
        return false;
      }
    }
    
    /**
     *  @brief Checks if a RooAbsPdf is an added pdf
     *
     *  @param pdf pdf to check for added pdf
     *  @return whether pdf is added or not
     */
    bool PdfIsAdded(const RooAbsPdf& pdf) const {
      if (std::strcmp(pdf.IsA()->GetName(),"RooAddPdf") == 0) {
        return true;
      } else {
        return false;
      }
    }
    
    /**
     *  @brief Checks if a RooAbsPdf is a product pdf
     *
     *  @param pdf pdf to check for product pdf
     *  @return whether pdf is product or not
     */
    bool PdfIsProduct(const RooAbsPdf& pdf) const {
      if (std::strcmp(pdf.IsA()->GetName(),"RooProdPdf") == 0) {
        return true;
      } else {
        return false;
      }
    }
    ///@}
    
    /** @name Generator functions
     *  Functions to generate toy samples for specific PDF (types).
     */
    ///@{
    /**
     *  @brief Generate a toy sample for a given PDF.
     *
     *  Helper function to be used by ToyFactoryStd::Generate().
     *
     *  @param pdf PDF to generate sample for
     *  @param argset_generation_observables argument set with variables in which 
     *                                       dimensions to generate
     *  @param expected_yield Expected yield to generate (if equals 0, then get 
     *                        yield from PDF itself)
     *  @return Pointer to the generated sample. ToyFactoryStd does not assume 
     *          ownership of this sample. Therefore, the invoker of this function
     *          must take care of proper deletion afterwards.
     */
    RooDataSet* GenerateForPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield=0, bool extended=true);
    
    /**
     *  @brief Generate a toy sample for a given RooAddPdf.
     *
     *  Helper function to be used by ToyFactoryStd::GenerateForPdf().
     *
     *  @param pdf PDF to generate sample for
     *  @param argset_generation_observables argument set with variables in which 
     *                                       dimensions to generate
     *  @param expected_yield Expected yield to generate (if equals 0, then get 
     *                        yield from PDF itself)
     *  @return Pointer to the generated sample. ToyFactoryStd does not assume 
     *          ownership of this sample. Therefore, the invoker of this function
     *          must take care of proper deletion afterwards.
     */
    RooDataSet* GenerateForAddedPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield=0, bool extended=true);
    
    /**
     *  @brief Generate a toy sample for a given RooProdPdf.
     *
     *  Helper function to be used by ToyFactoryStd::GenerateForPdf().
     *
     *  @param pdf PDF to generate sample for
     *  @param argset_generation_observables Argument set with variables in which 
     *                                       dimensions to generate
     *  @param expected_yield Expected yield to generate (if equals 0, then get 
     *                        yield from PDF itself)
     *  @return Pointer to the generated sample. ToyFactoryStd does not assume 
     *          ownership of this sample. Therefore, the invoker of this function
     *          must take care of proper deletion afterwards.
     *  @todo check for PDF arguments to be uncorrelated
     */
    RooDataSet* GenerateForProductPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield=0, bool extended=true);
    
    /**
     *  @brief Generate a toy sample for discrete variables.
     *
     *  Helper function to be used by ToyFactoryStd::Generate().
     *
     *  @param discrete_probabilities Vector of DiscreteProbabilityDistribution to 
     *                                generate sample for
     *  @param argset_generation_observables Argument set with variables in which 
     *                                       dimensions to generate
     *  @param argset_already_generated Argument set with variables already 
     *                                  generated
     *  @param yield Yield to generate
     *  @return Pointer to the generated sample. ToyFactoryStd does not assume 
     *          ownership of this sample. Therefore, the invoker of this function
     *          must take care of proper deletion afterwards.
     *  @todo check for PDF arguments to be uncorrelated
     */
    RooDataSet* GenerateDiscreteSample(const std::vector<Config::DiscreteProbabilityDistribution>& discrete_probabilities, const RooArgSet& argset_generation_observables, const RooArgSet& argset_already_generated, int yield);
    ///@}
    
    /**
     *  \brief CommonConfig instance to use
     */
    const CommonConfig& config_common_;
    /**
     *  \brief ToyFactoryStdConfig instance to use
     */
    const ToyFactoryStdConfig& config_toyfactory_;
  };
}

#endif // TOYFACTORYSTD_h
