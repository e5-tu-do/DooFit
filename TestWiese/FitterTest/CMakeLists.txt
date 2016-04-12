add_executable(TestFitter FitterTestMain.cpp RooMinimizerFcn2.cxx RooMinimizerFcn2.h)
#add_executable(TestToyBug ToyTestBugMain.cpp)
#add_executable(DeltaMToyStudy DeltaMToyStudy.cpp)

target_link_libraries(TestFitter Toy dfFitter Builder Plotting Config ${ALL_LIBRARIES}) 
#target_link_libraries(DeltaMToyStudy Toy Builder Config ${ALL_LIBRARIES}) 

