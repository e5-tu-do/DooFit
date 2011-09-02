/** \class ToyFactoryStd
 *  \brief Standard toy factory for DooFit generating toy samples
 *
 *  This class handles generation of toy samples for a given pdf. Generation 
 *  should be as smart as possible involving automatic decomposition of more 
 *  complex pdfs and generation of proto datasets based on given distributions.
 */

#ifndef TOYFACTORYSTD_h
#define TOYFACTORYSTD_h

// forward declarations
class CommonConfig; 
class ToyFactoryStdConfig;
class RooDataSet;

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
   *  \brief CommonConfig instance to use
   */
  const CommonConfig& config_common_;
  /**
   *  \brief ToyFactoryStdConfig instance to use
   */
  const ToyFactoryStdConfig& config_toyfactory_;
};


#endif // TOYFACTORYSTD_h
