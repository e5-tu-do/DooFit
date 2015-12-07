add_library(Plotting SHARED Plot/Plot.h Plot/Plot.cpp Plot/PlotSimultaneous.h
Plot/PlotSimultaneous.cpp Plot/PlotConfig.h Plot/PlotConfig.cpp
correlations/CorrelationPlot.h correlations/CorrelationPlot.cpp
fitresult/FitResultPrinter.h fitresult/FitResultPrinter.cpp
profiles/LikelihoodProfiler.h profiles/LikelihoodProfiler.cpp
profiles/FeldmanCousinsProfiler.h profiles/FeldmanCousinsProfiler.cpp)

target_link_libraries(Plotting Toy dfFitter Config ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS Plotting DESTINATION lib)
install(FILES Plot/Plot.h Plot/PlotSimultaneous.h Plot/PlotConfig.h DESTINATION include/doofit/plotting/Plot)

install(FILES correlations/CorrelationPlot.h DESTINATION
include/doofit/plotting/correlations)

install(FILES fitresult/FitResultPrinter.h DESTINATION
include/doofit/plotting/fitresult)

install(FILES profiles/LikelihoodProfiler.h profiles/FeldmanCousinsProfiler.h DESTINATION
include/doofit/plotting/profiles)

