add_library(${PROJECT_NAME}-protobuf INTERFACE)
target_include_directories(${PROJECT_NAME}-protobuf INTERFACE include)

set(protobuf_BUILD_TESTS OFF)
set(protobuf_INSTALL OFF)
cpmaddpackage(
        NAME protobuf
        GIT_REPOSITORY "git@github.com:protocolbuffers/protobuf.git"
        GIT_TAG v25.2)
unset(protobuf_BUILD_TESTS)
unset(protobuf_INSTALL)

sl_target_link_system_libraries(${PROJECT_NAME}-protobuf
        INTERFACE
        ${PROJECT_NAME}
        protobuf::libprotobuf)

if(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

