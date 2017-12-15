cmake_minimum_required(VERSION 3.8)
project(OSlabs)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS  "-pthread -lboost_system")
SET(CMAKE_C_FLAGS "-pthread -lboost_system")

set(SOURCE_FILES lab1.cpp)
add_executable(lab1 ${SOURCE_FILES})


set(SOURCE_FILES lab2.cpp)
add_executable(lab2 ${SOURCE_FILES})

set(SOURCE_FILES lab4.cpp)
add_executable(lab4 ${SOURCE_FILES})

set(CMAKE_C_STANDARD 99)
set(SOURCE_FILES lab3.c)
add_executable(lab3 ${SOURCE_FILES})

set(SOURCE_FILES lab4.c)
add_executable(lab4_new ${SOURCE_FILES})

find_package(Boost 1.60.0 COMPONENTS filesystem system REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
TARGET_LINK_LIBRARIES(lab4 LINK_PUBLIC ${Boost_LIBRARIES} )
