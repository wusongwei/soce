if(__SOCE_LOG4REL__)
  #nothing
else(__SOCE_LOG4REL__)
  set(__SOCE_LOG4REL__ 1)
  include(${SOCE_ROOT}/utils/CMakeLists.txt)

  set(SOCE_LOG4REL_ROOT ${SOCE_ROOT}/log4rel)
  include_directories(
    ${SOCE_LOG4REL_ROOT}
    )

  aux_source_directory(${SOCE_LOG4REL_ROOT}/src SRC_LIST)
  aux_source_directory(${SOCE_LOG4REL_ROOT}/log4rel SRC_LIST)
  list(APPEND LIBS pthread)
endif(__SOCE_LOG4REL__)
