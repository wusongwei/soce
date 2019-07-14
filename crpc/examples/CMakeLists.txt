cmake_minimum_required (VERSION 2.8)

PROJECT (crpc)

SET(EXE_NAME crpc)
SET(SOCE_ROOT "${PROJECT_SOURCE_DIR}/../..")

INCLUDE(${SOCE_ROOT}/crpc/CMakeLists.txt)

SET(LIBRARY_DIRS
    ${PROJECT_SOURCE_DIR}/lib
    /usr/local/lib
)

AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(./gen-cpp SRC_LIST)

SET(COMPILE_FLAG -g -std=c++11 -fno-strict-aliasing -Wcast-qual -Wpointer-arith -pipe -Wall -m64 -fPIC)
add_compile_options(${COMPILE_FLAG})

ADD_EXECUTABLE(${EXE_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${EXE_NAME}
  ${LIBS}
)
