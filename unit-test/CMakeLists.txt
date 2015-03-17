CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libtcore-unit-test C)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR "\${prefix}/lib")
SET(INCLUDEDIR "\${prefix}/include")
REMOVE_DEFINITIONS("-DFEATURE_TLOG_DEBUG")

SET(TESTS
		test-at
		test-co
		test-queue
		test-dcs
		test-hal
		test-util
		test-network
)

FOREACH(test ${TESTS})
	ADD_EXECUTABLE(${test} ${test}.c log.c)
	TARGET_LINK_LIBRARIES(${test} ${pkgs_LDFLAGS} "-L${CMAKE_BINARY_DIR} -ltcore -ldl")
#	INSTALL(TARGETS ${test} RUNTIME DESTINATION bin/)
	ADD_DEPENDENCIES(${test} run.sh)
	SET(TEST_LIST "${TEST_LIST} ${test}")
ENDFOREACH(test)


ADD_CUSTOM_TARGET(run.sh)
ADD_CUSTOM_COMMAND(
	TARGET run.sh
	POST_BUILD
	COMMAND echo "gtester ${TEST_LIST} -o report.xml" > run.sh
	COMMAND chmod +x run.sh
	COMMENT "Generating gtester script"
	VERBATIM # for double-quoted strings..
	)
ADD_DEPENDENCIES(run.sh tcore)
