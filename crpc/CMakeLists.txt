if (__SOCE_CRPC__)
#nothing
else(__SOCE_CRPC__)
	set(__SOCE_CRPC__ 1)

	INCLUDE(${SOCE_ROOT}/utils/CMakeLists.txt)
	INCLUDE(${SOCE_ROOT}/fads/proto/CMakeLists.txt)
	INCLUDE(${SOCE_ROOT}/transport/CMakeLists.txt)
	INCLUDE(${SOCE_ROOT}/cort-engine/CMakeLists.txt)
	INCLUDE(${SOCE_ROOT}/soce-zk/CMakeLists.txt)

	set(SOCE_CRPC_ROOT ${SOCE_ROOT}/crpc)
	include_directories( ${SOCE_CRPC_ROOT} )

	aux_source_directory(${SOCE_CRPC_ROOT}/src SRC_LIST)
	aux_source_directory(${SOCE_CRPC_ROOT}/gen-cpp SRC_LIST)

endif(__SOCE_CRPC__)
