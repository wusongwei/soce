cmake_minimum_required (VERSION 2.8)

PROJECT (cort-pool)

SET(EXE_NAME cort-pool)
SET(SOCE_ROOT "${PROJECT_SOURCE_DIR}/../../../")

option(soce_use_cort "use coroutine" OFF)

INCLUDE(${SOCE_ROOT}/soce-redis/CMakeLists.txt)
INCLUDE(${SOCE_ROOT}/transport/CMakeLists.txt)
INCLUDE(${SOCE_ROOT}/cort-engine/CMakeLists.txt)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}
)

SET(LIBRARY_DIRS
    ${PROJECT_SOURCE_DIR}/lib
    /usr/local/lib
)

AUX_SOURCE_DIRECTORY(. SRC_LIST)
SET(COMPILE_FLAG -g -std=c++11 -fno-strict-aliasing -Wcast-qual -Wpointer-arith -pipe -Wall -m64 -fPIC)
add_compile_options(${COMPILE_FLAG})

SET(COMPILE_FLAG -g -std=c++11 -fno-strict-aliasing -Wcast-qual -Wpointer-arith -pipe -Wall -m64 -fPIC)
add_compile_options(${COMPILE_FLAG})

ADD_EXECUTABLE(${EXE_NAME} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${EXE_NAME}
  ${LIBS}
)
