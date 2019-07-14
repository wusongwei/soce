cmake_minimum_required (VERSION 2.8)

PROJECT (utils)

SET(EXE_NAME utils)
SET(SOCE_ROOT "${PROJECT_SOURCE_DIR}/../..")

INCLUDE(${SOCE_ROOT}/utils/CMakeLists.txt)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
)

SET(LIBRARY_DIRS
    /usr/local/lib
)
AUX_SOURCE_DIRECTORY(. SRC_LIST)

SET(COMPILE_FLAG -g -std=c++11 -fno-strict-aliasing -Wcast-qual -Wpointer-arith -pipe -Wall -m64 -fPIC)
add_compile_options(${COMPILE_FLAG})

ADD_EXECUTABLE(${EXE_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${EXE_NAME}
  ${LIBS}
)
