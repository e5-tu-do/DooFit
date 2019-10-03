#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition
# and dump it in the cache along with proper documentation, otherwise set
# CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#

set(CMAKE_CXX_FLAGS_DBG "-O0 -ggdb -pg" CACHE STRING "Debug options." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Debug options." FORCE)
SET(CMAKE_CXX_FLAGS_PROFILING "-O3 -pg" CACHE STRING "Debug options." FORCE)

#ADD_DEFINITIONS("-std=c++0x")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{DOOMODULESYS}/cmake/Modules/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{DOOCORESYS}/cmake/Modules/")

set(BASEPATH "${CMAKE_SOURCE_DIR}/src")
include_directories(${BASEPATH})
include_directories("${BASEPATH}/lhcb")

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# Project Definition
project(DooFit)
cmake_minimum_required(VERSION 2.6)

if(${APPLE})
  MESSAGE( STATUS "Building for Mac OS X, switching on C++11 flags for Mac OS X/clang" )
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -pedantic -Wall -Wextra")
endif(${APPLE})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  MESSAGE( STATUS "Building for Linux, switching on C++11 flags for Linux/gcc" )
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=c++14 -std=c++17 -pedantic -Wall -Wextra")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

install(FILES external/FindDooFit.cmake DESTINATION cmake/Modules)
install(PROGRAMS scripts/jobmonitor.sh DESTINATION bin)

FIND_PACKAGE(Boost 1.48 COMPONENTS program_options filesystem regex thread system random REQUIRED)
find_package(ROOT REQUIRED)
find_package(DooCore REQUIRED)
FIND_PACKAGE(GSL REQUIRED)

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

include_directories(${ROOT_INCLUDE_DIR})

include_directories(${DooCore_INCLUDE_DIR})
link_directories(${DooCore_LIBRARY_DIR})

include_directories(${GSL_INCLUDE_DIRS})
link_directories(${GSL_LIBRARY_DIRS})

set(ALL_LIBRARIES ${DooCore_LIBRARIES} ${ROOT_LIBRARIES} ${ROOFIT_LIBRARIES} ${Boost_LIBRARIES})

MESSAGE( STATUS "Creating lib directory at $ENV{DOOFITSYS}")
file(MAKE_DIRECTORY $ENV{DOOFITSYS}/lib)

add_subdirectory(src)
add_subdirectory(python)
add_subdirectory(TestWiese)
add_subdirectory(main)
#add_subdirectory(scripts)
