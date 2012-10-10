add_executable(TestToy ToyTestMain.cpp)
#add_executable(TestToyBug ToyTestBugMain.cpp)
add_executable(DeltaMToyStudy DeltaMToyStudy.cpp)

target_link_libraries(TestToy Toy BuilderStd BuilderStdConfig Pdf2WsStd Plotting Config ${ALL_LIBRARIES}) 
target_link_libraries(DeltaMToyStudy Toy BuilderStd BuilderStdConfig Pdf2WsStd Config ${ALL_LIBRARIES}) 

