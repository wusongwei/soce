cmake_minimum_required (VERSION 2.8)

PROJECT (fadsdb)

SET(EXE_NAME fadsdb)
SET(SOCE_ROOT "${PROJECT_SOURCE_DIR}/../..")

INCLUDE(${SOCE_ROOT}/fadsdb/CMakeLists.txt)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
)

SET(LIBRARY_DIRS
    /usr/local/lib
)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
LIST(APPEND SRC_LIST gen-cpp/test.cpp)

SET(COMPILE_FLAG -g -std=c++11 -fno-strict-aliasing -Wcast-qual -Wpointer-arith -pipe -Wall -m64 -fPIC)
add_compile_options(${COMPILE_FLAG})

ADD_EXECUTABLE(${EXE_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${EXE_NAME}
  ${LIBS}
)
