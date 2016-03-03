CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libtcore C)

#INCLUDE(FindPkgConfig)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR ${LIB_INSTALL_DIR})
SET(INCLUDEDIR "\${prefix}/include")

# Set required packages
INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED glib-2.0 gudev-1.0 libtzplatform-config)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${CMAKE_SOURCE_DIR}/include ${EXTRA_CFLAGS} -Werror -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wdeclaration-after-statement -Wmissing-declarations -Wredundant-decls -Wcast-align -O2 -Wall  -Wno-array-bounds -Wno-empty-body -Wno-ignored-qualifiers -Wshadow -Wwrite-strings -Wswitch-default -Wno-unused-but-set-variable")

ADD_DEFINITIONS("-DFEATURE_TLOG_DEBUG")
ADD_DEFINITIONS("-DTCORE_LOG_TAG=\"TCORE\"")
#ADD_DEFINITIONS("-DTCORE_HAL_DEBUG")
ADD_DEFINITIONS("-DTCORE_VERSION=\"${VERSION}\"")
ADD_DEFINITIONS("-DMODEMS_PATH=\"${LIBDIR}/telephony/plugins/modems/\"")

MESSAGE(${CMAKE_C_FLAGS})
MESSAGE(${CMAKE_EXE_LINKER_FLAGS})

IF (ARCH_EMUL)
	ADD_DEFINITIONS("-DEMULATOR")
ENDIF (ARCH_EMUL)

SET(SRCS
	src/server.c
	src/manager.c
	src/plugin.c
	src/hal.c
	src/at.c
	src/communicator.c
	src/storage.c
	src/queue.c
	src/user_request.c
	src/util.c
	src/core_object/core_object.c
	src/core_object/co_modem.c
	src/core_object/co_sim.c
	src/core_object/co_network.c
	src/core_object/co_call.c
	src/core_object/co_sms.c
	src/core_object/co_ss.c
	src/core_object/co_phonebook.c
	src/core_object/co_ps.c
	src/core_object/co_context.c
	src/core_object/co_sat.c
	src/core_object/co_sap.c
	src/core_object/co_gps.c
	src/core_object/co_custom.c
	src/mux.c
)



# library build
ADD_LIBRARY(tcore SHARED ${SRCS})
TARGET_LINK_LIBRARIES(tcore ${pkgs_LDFLAGS})
SET_TARGET_PROPERTIES(tcore PROPERTIES VERSION 0.0.0 SOVERSION 0 OUTPUT_NAME tcore)

# pkgconfig file
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
CONFIGURE_FILE(tcore.pc.in tcore.pc @ONLY)


# install
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include/tcore)
INSTALL(TARGETS tcore
		LIBRARY DESTINATION ${LIBDIR})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/tcore.pc DESTINATION ${LIBDIR}/pkgconfig)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION /usr/share/license RENAME libtcore)

#ADD_SUBDIRECTORY(unit-test)
