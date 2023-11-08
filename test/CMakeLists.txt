set(TESTS_PATH ../test)
## setting the compiler
if(CMAKE_COMPILER_IS_GNUCXX)

	find_program(GCOV_TOOL gcov-11)
	if (NOT GCOV_TOOL)
    	message(FATAL_ERROR "Impossible to generate coverge report. Install gcov-11.")
	endif()
    add_custom_target("coverage"
        COMMAND "lcov" --directory ${TESTS_PATH} --zerocounters
        COMMAND ctest --test-dir ${TESTS_PATH}
        COMMAND "lcov" --directory ${TESTS_PATH} --capture --output-file coverage.info --gcov-tool ${GCOV_TOOL}
        COMMAND "lcov" --remove coverage.info "'/t/*'" "'/usr/*'" --output-file coverage.info.cleaned
        COMMAND "rm" coverage.info

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_custom_target("coverage-report"
        COMMAND "genhtml" -o coverage coverage.info.cleaned
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS "coverage"
    )
endif()


include_directories(../proto)
add_executable(json_serializer_test
		${TESTS_PATH}/json/json_serializer_test.cpp
		${CGREEN_INCLUDE_DIR}
		${SRC_HDR_MODEL}
		${SRC_HDR_JSON}
		${PROTO_HDR_FILE}
		${PROTO_CPP_FILE})

target_link_libraries(json_serializer_test ${CGREEN_LIBRARY} ${RapidJSON_LIBRARIES} uprotocolsdk)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(TEST_PROTO_SRCS TEST_PROTO_HDRS ${TESTS_PATH}/factory/test.proto)

add_executable(binary_serializer_test
		${TESTS_PATH}/binary/binary_test.cpp
		${SRC_HDR_MODEL}
		${SRC_HDR_BINARY}
		${PROTO_HDR_FILE}
		${PROTO_CPP_FILE}
		)

target_link_libraries(binary_serializer_test ${CGREEN_LIBRARY} uprotocolsdk)
add_test("t-01-binary-serializer" binary_serializer_test)

add_executable(priority_enum_test
		${TESTS_PATH}/model/priority_test.cpp)
target_link_libraries(priority_enum_test ${CGREEN_LIBRARY} uprotocolsdk)
add_test("t-02-priority-enum-test" priority_enum_test)

add_executable(attribute_tests
		${TESTS_PATH}/model/attributes_test.cpp)
target_link_libraries(attribute_tests ${CGREEN_LIBRARY} uprotocolsdk)
add_test("t-03-attribute-test" attribute_tests)

add_executable(uuid_v8_tests
	${TESTS_PATH}/uuid/uuid_v8_test.cpp ${SRC_HDR_UUID})

target_link_libraries(uuid_v8_tests ${CGREEN_LIBRARY} uprotocolsdk)
add_test("t-04-uuid-v8" uuid_v8_tests)

add_executable(uuid_gen_test 
	${TESTS_PATH}/uuid/uuid_gen_test.cpp)
target_link_libraries(uuid_gen_test ${CGREEN_LIBRARY} uprotocolsdk)
add_test("t-05-uuid-gen-test" uuid_gen_test)

add_executable(base64_test 
	${TESTS_PATH}/tools/base64_test.cpp)
target_link_libraries(base64_test gtest gtest_main uprotocolsdk)
add_test("t-06-base64-test" base64_test)	

add_executable(ce_factory_test 	${CGREEN_INCLUDE_DIR} ${TEST_PROTO_SRCS} ${TEST_PROTO_HDRS}
		${TESTS_PATH}/factory/cloud_event_factory_test.cpp)
target_link_libraries(ce_factory_test ${OPENSSL_LIBRARIES} ${CGREEN_LIBRARY} uprotocolsdk)

add_test("t-07-ce-factory-test" base64_test)
add_executable(service_type_test ${TESTS_PATH}/model/serviceType_test.cpp)
target_link_libraries(service_type_test ${CGREEN_LIBRARY} uprotocolsdk)

add_test("t-09-service-type-test" service_type_test)

add_executable(uri_authority_test
	${TESTS_PATH}/uri/uri_authority_test.cpp)
target_link_libraries(uri_authority_test ${CGREEN_LIBRARY} uprotocolsdk)

add_test("t-10-uri-authority-test" uri_authority_test)

add_executable(uri_entity_test
		${TESTS_PATH}/uri/uri_entity_test.cpp)
target_link_libraries(uri_entity_test ${CGREEN_LIBRARY} uprotocolsdk)
add_test("t-11-uri-entity-test" uri_entity_test)

add_executable(up_uri_test
	${TESTS_PATH}/uri/up_uri_test.cpp)
target_link_libraries(up_uri_test ${CGREEN_LIBRARY} uprotocolsdk)
add_test("t-12-up-uri-test" up_uri_test)

add_executable(up_uri_factory_test
	${TESTS_PATH}/uri/up_uri_factory_test.cpp)
target_compile_options(up_uri_factory_test PRIVATE -g -O0)
target_link_libraries(up_uri_factory_test ${CGREEN_LIBRARY} uprotocolsdk)
add_test("t-13-up-uri_factory-test"  up_uri_factory_test)
