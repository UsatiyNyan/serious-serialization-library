sl_gtest_prologue(v1.13.0)

cpmaddpackage(
        NAME fmt
        GIT_REPOSITORY "git@github.com:fmtlib/fmt.git"
        GIT_TAG 10.1.1)
cpmaddpackage(
        NAME range-v3
        GIT_REPOSITORY "git@github.com:ericniebler/range-v3.git"
        GIT_TAG 0.12.0)
sl_target_link_system_libraries(${PROJECT_NAME} 
        INTERFACE
        fmt::fmt
        range-v3::range-v3)

sl_add_gtest(${PROJECT_NAME} field)
sl_add_gtest(${PROJECT_NAME} to)

