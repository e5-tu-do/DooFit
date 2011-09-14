root_generate_dictionaries(ConfigDict Config.h)
root_generate_dictionaries(CommonConfigDict CommonConfig.h)
add_library(Config Config.cpp Config.h ${ConfigDict})
add_library(CommonConfig CommonConfig.cpp CommonConfig.h ${CommonConfigDict})


