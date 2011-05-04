FIND_PACKAGE(Boost 1.40 COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
link_directories("${Boost_LIBRARY_DIR}")

add_executable(ProgramOptionsTest program_options_test.cpp)

target_link_libraries(ProgramOptionsTest ${Boost_LIBRARIES}) 
