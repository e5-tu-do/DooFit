add_executable(TestToy ToyTestMain.cpp)
#add_executable(TestToyBug ToyTestBugMain.cpp)
#add_executable(DeltaMToyStudy DeltaMToyStudy.cpp)

target_link_libraries(TestToy Toy Builder Plotting dfFitter Config ${ALL_LIBRARIES}) 
#target_link_libraries(DeltaMToyStudy Toy Builder Config ${ALL_LIBRARIES}) 

