/** \class ToyFactoryStd
 *  \brief Standard toy factory for DooFit generating toy samples
 *
 *  This class handles generation of toy samples for a given pdf. Generation 
 *  should be as smart as possible involving automatic decomposition of more 
 *  complex pdfs and generation of proto datasets based on given distributions.
 */

#ifndef TOYFACTORYSTD_h
#define TOYFACTORYSTD_h

// STL
#include <cstring>

// ROOT
#include "TClass.h"

// from RooFit
#include "RooAbsPdf.h"

// forward declarations
class CommonConfig; 
class ToyFactoryStdConfig;
class RooDataSet;
class RooArgSet;

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
   *  \brief Generate a toy sample for a given PDF.
   *
   *  Helper function to be used by ToyFactoryStd::Generate().
   *
   *  @param pdf PDF to generate sample for
   *  @param yield Expected yield to generate (if equals 0, then get yield from
   *         PDF itself)
   *  \return Pointer to the generated sample. ToyFactoryStd does not assume 
   *          ownership of this sample. Therefore, the invoker of this function
   *          must take care of proper deletion afterwards.
   */
  RooDataSet* GenerateForPdf(RooAbsPdf& pdf, const RooArgSet& argset_generation_observables, double expected_yield=0);
  
  /**
   *  \brief CommonConfig instance to use
   */
  const CommonConfig& config_common_;
  /**
   *  \brief ToyFactoryStdConfig instance to use
   */
  const ToyFactoryStdConfig& config_toyfactory_;
};


#endif // TOYFACTORYSTD_h
