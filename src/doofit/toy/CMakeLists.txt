#add_subdirectory(ToyFactoryStd)
#add_subdirectory(ToyStudyStd)

add_library(Toy STATIC ToyStudyStd/ToyStudyStd.cpp ToyStudyStd/ToyStudyStd.h ToyStudyStd/ToyStudyStdConfig.cpp ToyStudyStd/ToyStudyStdConfig.h
                ToyFactoryStd/ToyFactoryStd.cpp ToyFactoryStd/ToyFactoryStd.h ToyFactoryStd/ToyFactoryStdConfig.cpp ToyFactoryStd/ToyFactoryStdConfig.h)
target_link_libraries(Toy Builder Config dfFitter ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES} ${GSL_LIBRARIES})

install(TARGETS Toy DESTINATION lib)
install(FILES ToyStudyStd/ToyStudyStd.h ToyStudyStd/ToyStudyStdConfig.h DESTINATION include/doofit/toy/ToyStudyStd)
install(FILES ToyFactoryStd/ToyFactoryStd.h ToyFactoryStd/ToyFactoryStdConfig.h DESTINATION include/doofit/toy/ToyFactoryStd)
