add_library(Plotting SHARED Plot/Plot.h Plot/Plot.cpp Plot/PlotSimultaneous.h
Plot/PlotSimultaneous.cpp Plot/PlotConfig.h Plot/PlotConfig.cpp
correlations/CorrelationPlot.h correlations/CorrelationPlot.cpp)
target_link_libraries(Plotting Config ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS Plotting DESTINATION lib)
install(FILES Plot/Plot.h Plot/PlotSimultaneous.h Plot/PlotConfig.h DESTINATION include/doofit/plotting/Plot)

install(FILES correlations/CorrelationPlot.h DESTINATION
include/doofit/plotting/correlations)

