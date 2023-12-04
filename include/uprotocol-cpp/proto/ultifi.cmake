cmake_minimum_required(VERSION 3.18)
project(cloudevent_proto LANGUAGES CXX)

find_package(protobuf CONFIG REQUIRED)

add_library(${PROJECT_NAME} cloudevents.proto)
target_include_directories(${PROJECT_NAME}  INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)
if (protobuf_LITE)
    target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotobuf-lite)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotobuf)
endif()

if(TARGET protobuf::libprotoc)
    target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotoc)
endif()

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS TARGET ${PROJECT_NAME})
protobuf_generate(LANGUAGE cpp TARGET ${PROJECT_NAME} PROTOS cloudevents.proto)
