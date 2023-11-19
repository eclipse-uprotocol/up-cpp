set(TESTS_PATH ../test)

include_directories(../uprotocol/proto)

add_executable(json_serializer_test
			   ${TESTS_PATH}/json/json_serializer_test.cpp
			   ${CGREEN_INCLUDE_DIR}
			   ${SRC_HDR_MODEL}
			   ${SRC_HDR_JSON}
			   ${PROTO_HDR_FILE}
			   ${PROTO_CPP_FILE})

add_dependencies(json_serializer_test uprotocol-core-api)
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

add_dependencies(binary_serializer_test uprotocol-core-api)
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

add_executable(base64_test 
	${TESTS_PATH}/tools/base64_test.cpp)
target_link_libraries(base64_test gtest gtest_main uprotocolsdk)
add_test("t-06-base64-test" base64_test)	

add_executable(ce_factory_test 	${CGREEN_INCLUDE_DIR} ${TEST_PROTO_SRCS} ${TEST_PROTO_HDRS}
		${TESTS_PATH}/factory/cloud_event_factory_test.cpp)

add_dependencies(ce_factory_test uprotocol-core-api)

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(uattributes_test
	${TESTS_PATH}/utransport/uattributes_test.cpp)
target_link_libraries(uattributes_test ${CGREEN_LIBRARY} uprotocolsdk gtest gtest_main)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(umessagetypes_test
	${TESTS_PATH}/utransport/umessagetypes_test.cpp)
target_link_libraries(umessagetypes_test ${CGREEN_LIBRARY} uprotocolsdk gtest gtest_main)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(upayload_test
	${TESTS_PATH}/utransport/upayload_test.cpp)
target_link_libraries(upayload_test ${CGREEN_LIBRARY} uprotocolsdk gtest gtest_main)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(upriority_test
	${TESTS_PATH}/utransport/upriority_test.cpp)
target_link_libraries(upriority_test ${CGREEN_LIBRARY} uprotocolsdk gtest gtest_main)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(userializationhint_test
	${TESTS_PATH}/utransport/userializationhint_test.cpp)
target_link_libraries(userializationhint_test ${CGREEN_LIBRARY} uprotocolsdk gtest gtest_main)

