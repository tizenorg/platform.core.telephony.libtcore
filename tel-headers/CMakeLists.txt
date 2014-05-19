CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(tel-headers C)

#SET(EXEC_PREFIX "\${prefix}")
#SET(LIBDIR "\${prefix}/lib")
#SET(INCLUDEDIR "\${prefix}/include")

# Set required packages
INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED glib-2.0 gobject-2.0 gio-2.0 gio-unix-2.0)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Wextra -Wall -Wno-unused-parameter -Wno-missing-field-initializers")

#Garbage collection - Unused code
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -Wl,--gc-sections")

INCLUDE_DIRECTORIES(
	${CMAKE_BINARY_DIR}
	${CMAKE_BINARY_DIR}/tel-headers
	${CMAKE_SOURCE_DIR}/tel-headers/include
	${CMAKE_SOURCE_DIR}/tel-headers/introspection
)

MESSAGE(${CMAKE_C_FLAGS})
MESSAGE(${CMAKE_EXE_LINKER_FLAGS})

SET(INTROSPECTION_PATH ${CMAKE_SOURCE_DIR}/tel-headers/introspection)

ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_BINARY_DIR}/tel-headers/tel_generated_code.c
	COMMAND gdbus-codegen
	--interface-prefix org.tizen.telephony.
	--generate-c-code tel_generated_code
	--c-namespace Telephony
	--c-generate-object-manager
	${INTROSPECTION_PATH}/manager.xml
	${INTROSPECTION_PATH}/network.xml
	${INTROSPECTION_PATH}/sim.xml
	${INTROSPECTION_PATH}/phonebook.xml
	${INTROSPECTION_PATH}/sap.xml
	${INTROSPECTION_PATH}/sat.xml
	${INTROSPECTION_PATH}/sms.xml
	${INTROSPECTION_PATH}/call.xml
	${INTROSPECTION_PATH}/ss.xml
	${INTROSPECTION_PATH}/modem.xml
	${INTROSPECTION_PATH}/gps.xml
	COMMENT "Generating GDBus .c/.h")

# pkgconfig file
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
CONFIGURE_FILE(tel-headers.pc.in tel-headers.pc @ONLY)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${CMAKE_BINARY_DIR}/tel-headers/tel_generated_code.c)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION 0.0.0 SOVERSION 0 OUTPUT_NAME ${PROJECT_NAME})

# install
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/tel-headers/include/ DESTINATION include/tel-headers/include)
INSTALL(FILES ${CMAKE_BINARY_DIR}/tel-headers/tel_generated_code.h DESTINATION ${DBUS_GEN_CODE_PATH})
INSTALL(TARGETS tel-headers LIBRARY DESTINATION ${LIB_INSTALL_DIR})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/tel-headers.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
