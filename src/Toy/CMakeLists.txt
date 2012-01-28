#add_subdirectory(ToyFactoryStd)
#add_subdirectory(ToyStudyStd)

add_library(Toy ToyStudyStd/ToyStudyStd.cpp ToyStudyStd/ToyStudyStd.h ToyStudyStd/ToyStudyStdConfig.cpp ToyStudyStd/ToyStudyStdConfig.h
                ToyFactoryStd/ToyFactoryStd.cpp ToyFactoryStd/ToyFactoryStd.h ToyFactoryStd/ToyFactoryStdConfig.cpp ToyFactoryStd/ToyFactoryStdConfig.h)
