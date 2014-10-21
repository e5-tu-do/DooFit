add_library(dfTools SHARED scanning/ParameterScanner.h scanning/ParameterScanner.cpp scanning/ParameterScannerConfig.h scanning/ParameterScannerConfig.cpp)
target_link_libraries(dfTools Config ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS dfTools DESTINATION lib)
install(FILES scanning/ParameterScanner.h scanning/ParameterScannerConfig.h DESTINATION include/doofit/tools/scanning)
