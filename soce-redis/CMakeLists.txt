if (__SOCE_REDIS__)
#nothing
else(__SOCE_REDIS__)
	set(__SOCE_REDIS__ 1)
	INCLUDE(${SOCE_ROOT}/utils/CMakeLists.txt)
	if(${soce_use_cort} STREQUAL "ON")
	  INCLUDE(${SOCE_ROOT}/cort-engine/CMakeLists.txt)
	  INCLUDE(${SOCE_ROOT}/transport/CMakeLists.txt)
	  add_definitions("-DSOCE_USE_COROUTINE")
	endif()

	set(SOCE_REDIS_ROOT ${SOCE_ROOT}/soce-redis)
	include_directories( ${SOCE_REDIS_ROOT} )

	aux_source_directory(${SOCE_REDIS_ROOT}/src SRC_LIST)
	if(${soce_use_cort} STREQUAL "ON")
	  aux_source_directory(${SOCE_REDIS_ROOT}/src/cort SRC_LIST)
	endif()

	list(APPEND LIBS hiredis)
endif(__SOCE_REDIS__)
