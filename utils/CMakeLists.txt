if(__SOCE_UTILS__)
  #nothing
else(__SOCE_UTILS__)
  set(__SOCE_UTILS__ 1)
  set(SOCE_UTILS_ROOT ${SOCE_ROOT}/utils)

  include(${SOCE_ROOT}/log4rel/CMakeLists.txt)
  include(${SOCE_ROOT}/transport/CMakeLists.txt)
  include(${SOCE_ROOT}/cort-engine/CMakeLists.txt)

  include_directories(
    ${SOCE_UTILS_ROOT}
    )

  aux_source_directory(${SOCE_UTILS_ROOT}/src SRC_LIST)

endif(__SOCE_UTILS__)
