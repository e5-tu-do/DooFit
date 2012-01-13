add_executable(TestToy ToyTestMain.cpp)
add_executable(TestToyBug ToyTestBugMain.cpp)

target_link_libraries(TestToy ToyFactoryStd ToyStudyStd BuilderStd BuilderStdConfig Pdf2WsStd CommonConfig Config ConfigHelper Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES}) 
target_link_libraries(TestToyBug ToyFactoryStd ToyStudyStd BuilderStd BuilderStdConfig Pdf2WsStd CommonConfig Config ConfigHelper Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES}) 

