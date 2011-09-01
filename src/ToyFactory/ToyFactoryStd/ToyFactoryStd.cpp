#include "ToyFactory/ToyFactoryStd/ToyFactoryStd.h"

// from Project
#include "Config/CommonConfig.h"
#include "ToyFactory/ToyFactoryStd/ToyFactoryStdConfig.h"

ToyFactoryStd::ToyFactoryStd(const CommonConfig& cfg_com, const ToyFactoryStdConfig& cfg_tfac) :
config_common_(cfg_com),
config_toyfactory_(cfg_tfac)
{
  
}

ToyFactoryStd::~ToyFactoryStd(){
  
}