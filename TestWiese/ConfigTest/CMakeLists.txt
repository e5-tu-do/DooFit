FIND_PACKAGE(Boost 1.40 COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_library(ConfigTest ConfigTest.cpp ConfigTest.h)

add_executable(TestConfig ConfigTestMain.cpp)

target_link_libraries(TestConfig ConfigTest Config ${Boost_LIBRARIES}) 
