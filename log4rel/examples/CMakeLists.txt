cmake_minimum_required (VERSION 2.8)

PROJECT (log4rel)
SET(EXE_NAME log4rel)

set(CMAKE_BUILD_TYPE "Debug")

SET(SOCE_ROOT "${PROJECT_SOURCE_DIR}/../../")
INCLUDE(${SOCE_ROOT}/log4rel/CMakeLists.txt)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
)
option (soce_disable_log4rel "disable log4rel" OFF)
SET(LIBRARY_DIRS
    ${PROJECT_SOURCE_DIR}/lib
    /usr/local/lib
)
AUX_SOURCE_DIRECTORY(. SRC_LIST)

if(${soce_disable_log4rel} STREQUAL "ON")
  add_definitions("-DSOCE_DISABLE_LOG4REL")
endif()

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Wno-unused-variable -pthread")
else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  set(CMAKE_BUILD_TYPE "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -pthread -fopenmp")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )
message("Build Type:" ${CMAKE_BUILD_TYPE} ${CMAKE_CXX_FLAGS})

LIST (APPEND LIBS pthread)
ADD_EXECUTABLE(${EXE_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${EXE_NAME}
  ${LIBS}
)
