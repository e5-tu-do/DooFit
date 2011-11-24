#ifndef TOYSTUDYSTD_h
#define TOYSTUDYSTD_h

// STL

// ROOT

// from RooFit

// from project
#include "Toy/ToyStudyStd/ToyStudyStdConfig.h"
#include "Config/CommonConfig.h"

// forward declarations

namespace Toy {
  /** @class ToyStudyStd
   *  @brief Standard toy study for DooFit to help conduct mass toy fits and evaluate
   *
   *  This class is designed to assist in both conducting mass toy fits as well 
   *  evaluating results from these fits. It's design is not to conduct the 
   *  generation or actual fit itself. Other modules like ToyFactoryStd and the 
   *  fitter module ... are well suited for that.
   * 
   */

  class ToyStudyStd {
   public:
    /**
     *  @brief Default constructor for ToyStudyStd
     *
     *  This is the default constructor for ToyStudyStd involving general 
     *  configuration @a cfg_com and specific configuration @a cfg_tstudy.
     *
     *  @param cfg_com CommonConfig for the general configuration.
     *  @param cfg_tfac ToyFactoryStdConfig for this specific toy factory.
     */
    ToyStudyStd(const CommonConfig& cfg_com, const ToyStudyStdConfig& cfg_tstudy);
    
    /**
     *  @brief Destructor for ToyStudyStd
     *
     *  Detailed description.
     *
     */
    ~ToyStudyStd();
    
   protected:
    
   private:
    /**
     *  \brief CommonConfig instance to use
     */
    const CommonConfig& config_common_;
    /**
     *  \brief ToyFactoryStdConfig instance to use
     */
    const ToyStudyStdConfig& config_toystudy_;
  };
}


#endif // TOYSTUDYSTD_h
