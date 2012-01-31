#add_subdirectory(ToyFactoryStd)
#add_subdirectory(ToyStudyStd)

add_library(Toy SHARED ToyStudyStd/ToyStudyStd.cpp ToyStudyStd/ToyStudyStd.h ToyStudyStd/ToyStudyStdConfig.cpp ToyStudyStd/ToyStudyStdConfig.h
                ToyFactoryStd/ToyFactoryStd.cpp ToyFactoryStd/ToyFactoryStd.h ToyFactoryStd/ToyFactoryStdConfig.cpp ToyFactoryStd/ToyFactoryStdConfig.h)
target_link_libraries(Toy Config utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS Toy DESTINATION lib)
install(FILES ToyStudyStd/ToyStudyStd.h ToyStudyStd/ToyStudyStdConfig.h DESTINATION include/doofit/Toy/ToyStudyStd)
install(FILES ToyFactoryStd/ToyFactoryStd.h ToyFactoryStd/ToyFactoryStdConfig.h DESTINATION include/doofit/Toy/ToyFactoryStd)
