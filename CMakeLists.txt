cmake_minimum_required(VERSION 3.21)

include(FetchContent)
FetchContent_Declare(
        serious-cmake-library
        GIT_REPOSITORY git@github.com:UsatiyNyan/serious-cmake-library.git
        GIT_TAG        main
)
FetchContent_MakeAvailable(serious-cmake-library)

sl_project_prologue(CXX_STANDARD 20)
project(
        serious-serialization-library
        VERSION 0.0.1
        DESCRIPTION "extensible serialization library"
        HOMEPAGE_URL "TODO"
        LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

add_subdirectory(dependencies)
add_subdirectory(extensions)

if(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

