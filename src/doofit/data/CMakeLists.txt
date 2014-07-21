add_library(dfData SHARED general.h general.cpp)
target_link_libraries(dfData ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS dfData DESTINATION lib)
install(FILES general.h DESTINATION include/doofit/general)

