add_library(${PROJECT_NAME}-nlohmann-json INTERFACE)
target_include_directories(${PROJECT_NAME}-nlohmann-json INTERFACE include)

cpmaddpackage(
        NAME nlohmann_json
        GIT_REPOSITORY "git@github.com:nlohmann/json.git"
        GIT_TAG v3.11.3)

sl_target_link_system_libraries(${PROJECT_NAME}-nlohmann-json
        INTERFACE
        ${PROJECT_NAME}
        nlohmann_json::nlohmann_json)

if(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

