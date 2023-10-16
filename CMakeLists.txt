#--- Sets minimum cmake version
cmake_minimum_required(VERSION 3.10)

#--- Sets up project
project(ARAP)

#--- Sets root of the project
set(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

#--- Sets shared libraries output to .so instead of .dylib if MacOS
if(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for Mac OS X" FORCE)
	set(CMAKE_SHARED_LIBRAY ".so")
endif(APPLE)

#--- Loads the common configuration
include(common/config.cmake)

#--- Load external packages
include(common/ITK.cmake)
include(common/Eigen.cmake)
include(common/Python.cmake)

#--- Finds all source and header files
get_filename_component(PROJECTNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB_RECURSE HEADERS "include/*.h")

message(STATUS "Yo ${SOURCES} ${HEADERS}")

#--- Adds executable
add_executable(${PROJECTNAME} ${SOURCES} ${HEADERS})

#--- Add include and libraries directories
target_link_libraries(${PROJECTNAME} PUBLIC ${COMMON_LIBS} ${ITK_LIBRARIES})

#--- C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
