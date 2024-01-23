sl_gtest_prologue(v1.13.0)

include(${protobuf_SOURCE_DIR}/cmake/protobuf-generate.cmake)
protobuf_generate(
    LANGUAGE cpp
    OUT_VAR test_struct_proto_srcs
    PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
    PROTOS src/test_struct.proto
    DEPENDENCIES ${PROJECT_NAME}-protobuf)
target_sources(${PROJECT_NAME}-protobuf
    INTERFACE
    ${test_struct_proto_srcs})
target_include_directories(${PROJECT_NAME}-protobuf
    INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR})

sl_add_gtest(${PROJECT_NAME}-protobuf to)
sl_add_gtest(${PROJECT_NAME}-protobuf from)

