
add_library(ConfigTest ConfigTest.cpp ConfigTest.h)

add_executable(TestConfig ConfigTestMain.cpp)

target_link_libraries(TestConfig ConfigTest Config Utils ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES}) 
