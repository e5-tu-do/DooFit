add_executable(TestToy ToyTestMain.cpp)
#add_executable(TestToyBug ToyTestBugMain.cpp)
add_executable(DeltaMToyStudy DeltaMToyStudy.cpp)

 
target_link_libraries(TestToy Toy BuilderStd BuilderStdConfig Pdf2WsStd Config Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES}) 
target_link_libraries(DeltaMToyStudy Toy BuilderStd BuilderStdConfig Pdf2WsStd Config Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES}) 

