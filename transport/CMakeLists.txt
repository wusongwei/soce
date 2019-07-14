if(__SOCE_TRANSPORT__)
  #nothing
else(__SOCE_TRANSPORT__)
  set(__SOCE_TRANSPORT__ 1)
  include(${SOCE_ROOT}/utils/CMakeLists.txt)
  include(${SOCE_ROOT}/log4rel/CMakeLists.txt)

  set(SOCE_TRANSPORT_ROOT ${SOCE_ROOT}/transport)
  include_directories(
    ${SOCE_TRANSPORT_ROOT}
    )

  aux_source_directory(${SOCE_TRANSPORT_ROOT}/src/libevent SRC_LIST)

  list(APPEND LIBS event event_openssl pthread crypto ssl)
#  list(APPEND LIBS websockets)

endif(__SOCE_TRANSPORT__)
