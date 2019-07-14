cmake_minimum_required (VERSION 2.8)

PROJECT (fadsgen)
SET(EXE_NAME fadsgen)

set(CMAKE_BUILD_TYPE "Debug")

set(SOCE_ROOT ${PROJECT_SOURCE_DIR}/../..)
include(${SOCE_ROOT}/log4rel/CMakeLists.txt)
include(${SOCE_ROOT}/utils/CMakeLists.txt)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/core
    ${PROJECT_SOURCE_DIR}/cpp
    ${PROJECT_SOURCE_DIR}/java
)

SET(LIBRARY_DIRS
    ${PROJECT_SOURCE_DIR}/lib
    /usr/local/lib
)
AUX_SOURCE_DIRECTORY(./core SRC_LIST)
AUX_SOURCE_DIRECTORY(./cpp SRC_LIST)
AUX_SOURCE_DIRECTORY(./java SRC_LIST)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Wno-unused-variable -pthread")
else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  set(CMAKE_BUILD_TYPE "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -pthread -fopenmp")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )
message("Build Type:" ${CMAKE_BUILD_TYPE} ${CMAKE_CXX_FLAGS})

ADD_EXECUTABLE(${EXE_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${EXE_NAME}
  ${LIBS}
)
