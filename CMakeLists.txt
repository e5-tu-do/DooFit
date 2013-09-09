#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition
# and dump it in the cache along with proper documentation, otherwise set
# CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#

#IF(DEFINED CMAKE_BUILD_TYPE)
#   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
#build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
#RelWithDebInfo MinSizeRel.")
#ELSE()
#   SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options
#are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo
#MinSizeRel.")
#ENDIF()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -pg -std=c++11 -stdlib=libc++" CACHE STRING "Debug options." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++11 -stdlib=libc++" CACHE STRING "Debug options." FORCE)

SET(CMAKE_BUILD_TYPE Profiling)
SET(CMAKE_CXX_FLAGS_PROFILING "-O3 -pg -std=c++11 -stdlib=libc++" CACHE STRING "Debug options." FORCE)

#ADD_DEFINITIONS("-std=c++0x")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{DOOCORESYS}/cmake/Modules/")

set(BASEPATH "${CMAKE_SOURCE_DIR}/src")
include_directories(${BASEPATH})

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# Project Definition
project(DooFit)
cmake_minimum_required(VERSION 2.6)

install(FILES external/FindDooFit.cmake DESTINATION cmake/Modules)

FIND_PACKAGE(Boost 1.48 COMPONENTS program_options filesystem regex thread system random REQUIRED)
find_package(ROOT REQUIRED)
find_package(DooCore REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(${ROOT_INCLUDE_DIR})

include_directories(${DooCore_INCLUDE_DIR})
link_directories(${DooCore_LIBRARY_DIR})

set(ALL_LIBRARIES ${DooCore_LIBRARIES} ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

add_subdirectory(src)
add_subdirectory(python)
add_subdirectory(TestWiese)
add_subdirectory(main)
