add_executable(TestToy ToyTestMain.cpp)

target_link_libraries(TestToy ToyFactoryStd BuilderStd BuilderStdConfig Pdf2WsStd CommonConfig Config ConfigHelper Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES}) 
