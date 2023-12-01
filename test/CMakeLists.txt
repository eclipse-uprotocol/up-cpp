set(TESTS_PATH ../test)
set(GTEST_LIB gtest)
## setting the compiler
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

add_executable(uuid_test
	${TESTS_PATH}/uuid/uuid_test.cpp)
target_link_libraries(uuid_test ${GTEST_LIB} uprotocolsdk)
add_test("t-04-uuid-test" uuid_test)

add_executable(base64_test ${TESTS_PATH}/tools/base64_test.cpp)
target_link_libraries(base64_test ${OPENSSL_LIBRARIES} ${GTEST_LIB} gtest_main uprotocolsdk)
add_test("t-05-base64-test" base64_test)

add_test("t-06-ce-factory-test" base64_test)
add_executable(service_type_test ${TESTS_PATH}/model/serviceType_test.cpp)
target_link_libraries(service_type_test ${CGREEN_LIBRARY} uprotocolsdk)

add_test("t-07-service-type-test" service_type_test)

add_executable(UAuthorityTest
	${TESTS_PATH}/uri/datamodel/UAuthorityTest.cpp)
target_link_libraries(UAuthorityTest ${GTEST_LIB} uprotocolsdk)
add_test("t-08-UAuthorityTest" UAuthorityTest)

add_executable(UEntityTest
		${TESTS_PATH}/uri/datamodel/UEntityTest.cpp)
target_link_libraries(UEntityTest ${GTEST_LIB} uprotocolsdk)
add_test("t-9-UEntityTest" UEntityTest)

add_executable(UResourceTest
	${TESTS_PATH}/uri/datamodel/UResourceTest.cpp)
target_link_libraries(UResourceTest ${GTEST_LIB} uprotocolsdk)
add_test("t-10-UResourceTest" UResourceTest)

add_executable(UUriTest
	${TESTS_PATH}/uri/datamodel/UUriTest.cpp)
target_link_libraries(UUriTest ${GTEST_LIB} uprotocolsdk)
add_test("t-11-UUriTest" UUriTest)

add_executable(LongUriSerializerTest
	${TESTS_PATH}/uri/serializer/LongUriSerializerTest.cpp)
target_compile_options(LongUriSerializerTest PRIVATE -g -O0)
target_link_libraries(LongUriSerializerTest ${GTEST_LIB} uprotocolsdk)
add_test("t-12-LongUriSerializerTest"  LongUriSerializerTest)

add_executable(MicroUriSerializerTest
	${TESTS_PATH}/uri/serializer/MicroUriSerializerTest.cpp)
target_compile_options(MicroUriSerializerTest PRIVATE -g -O0)
target_link_libraries(MicroUriSerializerTest ${GTEST_LIB} uprotocolsdk)
add_test("t-13-MicroUriSerializerTest"  MicroUriSerializerTest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(uattributes_test
	${TESTS_PATH}/utransport/uattributes_test.cpp)
target_link_libraries(uattributes_test ${CGREEN_LIBRARY} uprotocolsdk ${GTEST_LIB}  gtest_main)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(umessagetypes_test
	${TESTS_PATH}/utransport/umessagetypes_test.cpp)
target_link_libraries(umessagetypes_test ${CGREEN_LIBRARY} uprotocolsdk ${GTEST_LIB}  gtest_main)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(upayload_test
	${TESTS_PATH}/utransport/upayload_test.cpp)
target_link_libraries(upayload_test ${CGREEN_LIBRARY} uprotocolsdk ${GTEST_LIB}  gtest_main)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(upriority_test
	${TESTS_PATH}/utransport/upriority_test.cpp)
target_link_libraries(upriority_test ${CGREEN_LIBRARY} uprotocolsdk ${GTEST_LIB}  gtest_main)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
add_executable(userializationhint_test
	${TESTS_PATH}/utransport/userializationhint_test.cpp)
target_link_libraries(userializationhint_test ${CGREEN_LIBRARY} uprotocolsdk ${GTEST_LIB}  gtest_main)
