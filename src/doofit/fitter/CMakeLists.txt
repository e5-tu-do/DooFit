add_library(dfFitter SHARED splot/SPlotFit2.h splot/SPlotFit2.cpp)

target_link_libraries(dfFitter ${ALL_LIBRARIES} ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS dfFitter DESTINATION lib)
install(FILES splot/SPlotFit2.h DESTINATION include/doofit/fitter/splot)

