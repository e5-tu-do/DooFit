
add_executable(TestToy ToyTestMain.cpp)

target_link_libraries(TestToy CommonConfig Config ConfigDict ToyFactoryStd BuilderStd BuilderStdConfig Pdf2WsStd Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES}) 
