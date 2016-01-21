add_library(dfAnalysis SHARED profiles/LikelihoodProfiler.h profiles/LikelihoodProfiler.cpp
profiles/FeldmanCousinsProfiler.h profiles/FeldmanCousinsProfiler.cpp)

target_link_libraries(dfAnalysis Plotting Toy dfFitter Config ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS dfAnalysis DESTINATION lib)
install(FILES profiles/LikelihoodProfiler.h profiles/FeldmanCousinsProfiler.h DESTINATION
include/doofit/analysis/profiles)

