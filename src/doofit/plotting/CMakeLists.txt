add_library(Plotting SHARED Plot/Plot.h Plot/Plot.cpp)
target_link_libraries(Plotting Config Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS Plotting DESTINATION lib)
install(FILES Plot/Plot.h DESTINATION include/doofit/plotting/Plot)
