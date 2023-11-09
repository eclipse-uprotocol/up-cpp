cmake_minimum_required(VERSION 3.18)

INCLUDE(FindProtobuf)
FIND_PACKAGE(Protobuf REQUIRED)
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER cloudevents.proto)
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER uuid.proto)
ADD_LIBRARY(proto ${PROTO_HEADER} ${PROTO_SRC})
target_link_libraries(proto ${Protobuf_LIBRARIES})