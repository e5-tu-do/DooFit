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

class ToyFactoryStd {
 public:
  /**
   *  \brief Default constructor for ToyFactoryStd
   *
   *  Detailed description.
   *
   *  @param cfg_com CommonConfig for the general configuration.
   *  @param cfg_tfac ToyFactoryStdConfig for this specific toy factory.
   */
  ToyFactoryStd(const CommonConfig& cfg_com, const ToyFactoryStdConfig& cfg_tfac);
  
  /**
   *  \brief Destructor for ToyFactoryStd
   */
  ~ToyFactoryStd();
  
 protected:
  
 private:
};


#endif // TOYFACTORYSTD_h
