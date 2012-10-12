#root_generate_dictionaries(AbsConfigDict AbsConfig.h)
#root_generate_dictionaries(CommonConfigDict CommonConfig.h)
#${ConfigDict}
#${CommonConfigDict}

# adding all config related stuff into *one* library (because that's what a library is for)
add_library(Config SHARED AbsConfig.cpp AbsConfig.h CommonConfig.cpp
CommonConfig.h AbsTypeCommaSeparated.cpp AbsTypeCommaSeparated.h
DiscreteProbabilityDistribution.h DiscreteProbabilityDistribution.cpp
ConfigCstStringVector.h ConfigCstStringVector.cpp CommaSeparatedPair.h
CommaSeparatedPair.cpp CommaSeparatedList.h CommaSeparatedList.cpp)

install(TARGETS Config DESTINATION lib)
install(FILES AbsConfig.h CommonConfig.h AbsTypeCommaSeparated.h
DiscreteProbabilityDistribution.h ConfigCstStringVector.h CommaSeparatedPair.h
CommaSeparatedList.h DESTINATION include/doofit/Config)

#add_library(CommonConfig CommonConfig.cpp CommonConfig.h)
target_link_libraries(Config ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

#root_generate_dictionaries(AbsTypeCommaSeparatedDict AbsTypeCommaSeparated.h)
#root_generate_dictionaries(DiscreteProbabilityDistributionDict DiscreteProbabilityDistribution.h) 
#root_generate_dictionaries(CommaSeparatedPairDict CommaSeparatedPair.h)
#root_generate_dictionaries(ConfigCstStringVectorDict ConfigCstStringVector.h)
#${AbsTypeCommaSeparatedDict}
#${DiscreteProbabilityDistributionDict}
#${ConfigCstStringVectorDict}
#${CommaSeparatedPairDict}

#add_library(ConfigHelper AbsTypeCommaSeparated.cpp AbsTypeCommaSeparated.h DiscreteProbabilityDistribution.h DiscreteProbabilityDistribution.cpp ConfigCstStringVector.h ConfigCstStringVector.cpp CommaSeparatedPair.h CommaSeparatedPair.cpp)


