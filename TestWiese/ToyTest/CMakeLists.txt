
add_executable(TestToy ToyTestMain.cpp)

target_link_libraries(TestToy CommonConfig Config Pdf2WsStd Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES}) 
