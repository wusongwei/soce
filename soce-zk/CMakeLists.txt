INCLUDE(${SOCE_ROOT}/log4rel/CMakeLists.txt)

if(__SOCE_ZK__)
  #nothing
else(__SOCE_ZK__)
  set(__SOCE_ZK__ 1)
  set(SOCE_ZK_ROOT ${SOCE_ROOT}/soce-zk)
  include_directories(
    ${SOCE_ZK_ROOT}
    )

  aux_source_directory(${SOCE_ZK_ROOT}/src SRC_LIST)
  LIST(APPEND LIBS zookeeper_mt pthread)
endif(__SOCE_ZK__)
