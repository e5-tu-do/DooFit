root_generate_dictionaries(ConfigDict Config.h)
#message(SEND_ERROR "Bla ${ConfigDict}")
add_library(Config Config.cpp Config.h ${ConfigDict})
add_library(CommonConfig CommonConfig.cpp CommonConfig.h)


