CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libtcore C)

#INCLUDE(FindPkgConfig)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR "\${prefix}/lib")
SET(INCLUDEDIR "\${prefix}/include")

# Set required packages
INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED glib-2.0 dlog gudev-1.0)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${CMAKE_SOURCE_DIR}/include ${EXTRA_CFLAGS} -Werror -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wdeclaration-after-statement -Wmissing-declarations -Wredundant-decls -Wcast-align")

ADD_DEFINITIONS("-DFEATURE_DLOG_DEBUG")
ADD_DEFINITIONS("-DTCORE_LOG_TAG=\"TCORE\"")

MESSAGE(${CMAKE_C_FLAGS})
MESSAGE(${CMAKE_EXE_LINKER_FLAGS})

SET(SRCS
		src/server.c
		src/plugin.c
		src/hal.c
		src/at.c
		src/udev.c
		src/communicator.c
		src/storage.c
		src/queue.c
		src/user_request.c
		src/util.c
		src/core_object.c
		src/co_modem.c
		src/co_network.c
		src/co_ps.c
		src/co_context.c
		src/co_sim.c
		src/co_sat.c
		src/co_sap.c
		src/co_call.c
		src/co_ss.c
		src/co_sms.c
		src/co_phonebook.c
		src/co_gps.c
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
		LIBRARY DESTINATION lib)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/tcore.pc DESTINATION lib/pkgconfig)

#ADD_SUBDIRECTORY(unit-test)
