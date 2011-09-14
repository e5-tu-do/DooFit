
root_generate_dictionaries(ConfigTestDict ConfigTest.h)
add_library(ConfigTest ConfigTest.cpp ConfigTest.h ${ConfigTestDict})

add_executable(TestConfig ConfigTestMain.cpp)

target_link_libraries(TestConfig ConfigTest CommonConfig Config Pdf2WsStd Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES}) 

