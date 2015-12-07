add_library(dfFitter SHARED 
  splot/SPlotFit2.h       splot/SPlotFit2.cpp
  easyfit/EasyFit.h       easyfit/EasyFit.cpp
  easyfit/EasyFitResult.h easyfit/EasyFitResult.cpp
  AbsFitter.h             AbsFitter.cpp
)

target_link_libraries(dfFitter ${ALL_LIBRARIES} ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS dfFitter DESTINATION lib)
install(FILES splot/SPlotFit2.h DESTINATION include/doofit/fitter/splot)
install(FILES easyfit/EasyFit.h DESTINATION include/doofit/fitter/easyfit)
install(FILES easyfit/EasyFitResult.h DESTINATION include/doofit/fitter/easyfit)
install(FILES AbsFitter.h DESTINATION include/doofit/fitter)

