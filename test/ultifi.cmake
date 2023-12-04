add_executable(json_serializer_test json/json_serializer_test.cpp)
target_link_libraries(json_serializer_test cloudevent_proto RapidJSON::RapidJSON cgreen::cgreen uprotocol-cpp)

add_executable(binary_serializer_test binary/binary_test.cpp)
target_link_libraries(binary_serializer_test cloudevent_proto cgreen::cgreen uprotocol-cpp)
add_test("t-01-binary-serializer" binary_serializer_test)

add_executable(priority_enum_test model/priority_test.cpp)
target_link_libraries(priority_enum_test cgreen::cgreen uprotocol-cpp)
add_test("t-02-priority-enum-test" priority_enum_test)

add_executable(attribute_tests model/attributes_test.cpp)
target_link_libraries(attribute_tests  cgreen::cgreen uprotocol-cpp)
add_test("t-03-attribute-test" attribute_tests)

add_executable(base64_test tools/base64_test.cpp)
target_link_libraries(base64_test OpenSSL::SSL  cgreen::cgreen uprotocol-cpp GTest::GTest)
add_test("t-06-base64-test" base64_test)

add_test("t-07-ce-factory-test" base64_test)
add_executable(service_type_test model/serviceType_test.cpp)
target_link_libraries(service_type_test  cgreen::cgreen uprotocol-cpp)

add_test("t-08-service-type-test" service_type_test)

add_executable(UAuthorityTest uri/UAuthorityTest.cpp)
target_link_libraries(UAuthorityTest  cgreen::cgreen uprotocol-cpp)
add_test("t-09-UAuthorityTest" UAuthorityTest)

add_executable(UEntityTest uri/UEntityTest.cpp)
target_link_libraries(UEntityTest  cgreen::cgreen uprotocol-cpp)
add_test("t-10-UEntityTest" UEntityTest)

add_executable(UResourceTest uri/UResourceTest.cpp)
target_link_libraries(UResourceTest  cgreen::cgreen uprotocol-cpp)
add_test("t-11-UResourceTest" UResourceTest)

add_executable(UUriTest uri/UUriTest.cpp)
target_link_libraries(UUriTest  cgreen::cgreen uprotocol-cpp)
add_test("t-12-UUriTest" UUriTest)

add_executable(LongUriSerializerTest uri/serializer/LongUriSerializerTest.cpp)
target_compile_options(LongUriSerializerTest PRIVATE -g -O0)
target_link_libraries(LongUriSerializerTest  cgreen::cgreen uprotocol-cpp)
add_test("t-13-LongUriSerializerTest"  LongUriSerializerTest)

add_executable(MicroUriSerializerTest uri/MicroUriSerializerTest.cpp)
target_compile_options(MicroUriSerializerTest PRIVATE -g -O0)
target_link_libraries(MicroUriSerializerTest  cgreen::cgreen uprotocol-cpp)
add_test("t-14-MicroUriSerializerTest"  MicroUriSerializerTest)

add_executable(umessagetypes_test utransport/umessagetypes_test.cpp)
target_link_libraries(umessagetypes_test cgreen::cgreen uprotocol-cpp GTest::GTest)

add_executable(upayload_test utransport/upayload_test.cpp)
target_link_libraries(upayload_test cgreen::cgreen uprotocol-cpp GTest::GTest)

add_executable(upriority_test utransport/upriority_test.cpp)
target_link_libraries(upriority_test cgreen::cgreen uprotocol-cpp GTest::GTest)

add_executable(userializationhint_test utransport/userializationhint_test.cpp)
target_link_libraries(userializationhint_test cgreen::cgreen uprotocol-cpp GTest::GTest)
