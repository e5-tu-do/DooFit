#root_generate_dictionaries(AbsConfigDict AbsConfig.h)
#root_generate_dictionaries(CommonConfigDict CommonConfig.h)
#${ConfigDict}
#${CommonConfigDict}

add_library(Config AbsConfig.cpp AbsConfig.h)
add_library(CommonConfig CommonConfig.cpp CommonConfig.h)

#root_generate_dictionaries(AbsTypeCommaSeparatedDict AbsTypeCommaSeparated.h)
#root_generate_dictionaries(DiscreteProbabilityDistributionDict DiscreteProbabilityDistribution.h) 
#root_generate_dictionaries(CommaSeparatedPairDict CommaSeparatedPair.h)
#root_generate_dictionaries(ConfigCstStringVectorDict ConfigCstStringVector.h)
#${AbsTypeCommaSeparatedDict}
#${DiscreteProbabilityDistributionDict}
#${ConfigCstStringVectorDict}
#${CommaSeparatedPairDict}

add_library(ConfigHelper AbsTypeCommaSeparated.cpp AbsTypeCommaSeparated.h DiscreteProbabilityDistribution.h DiscreteProbabilityDistribution.cpp ConfigCstStringVector.h ConfigCstStringVector.cpp CommaSeparatedPair.h CommaSeparatedPair.cpp)


