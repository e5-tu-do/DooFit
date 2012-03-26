#ifndef DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ELEMENTS_FACTORY_H
#define DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ELEMENTS_FACTORY_H

namespace doofit {
namespace builder {
namespace numerobis {
namespace elements {

// forward declarations
class Registrar;

class Factory {
 public:
  Factory(Registrar& registrar);
  virtual ~Factory ();
 protected:
  
 private:
  
};

} // namespace elements 
} // namespace numerobis 
} // namespace builder 
} // namespace doofit 

#endif // DOOFIT_BUILDER_NUMEROBIS_BLUEPRINT_ELEMENTS_FACTORY_H
