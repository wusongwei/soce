if(__SOCE_PROTO__)
  #nothing
else(__SOCE_PROTO__)
  set(__SOCE_PROTO__ 1)
  set(SOCE_PROTO_ROOT ${SOCE_ROOT}/fads/proto)
  include_directories(
    ${SOCE_PROTO_ROOT}
    )

  aux_source_directory(${SOCE_PROTO_ROOT}/src SRC_LIST)

endif(__SOCE_PROTO__)
