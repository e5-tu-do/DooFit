root_generate_dictionaries(ToyFactoryStdConfigDict ToyFactoryStdConfig.h)
#message(SEND_ERROR "Bla ${ToyFactoryStdConfigDict}")
add_library(ToyFactoryStd ToyFactoryStd.cpp ToyFactoryStd.h ToyFactoryStdConfig.cpp ToyFactoryStdConfig.h ${ToyFactoryStdConfigDict})

