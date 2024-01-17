sl_gtest_prologue(v1.13.0)

cpmaddpackage(
        NAME fmt
        GIT_REPOSITORY "git@github.com:fmtlib/fmt.git"
        GIT_TAG 10.1.1)
sl_target_link_system_libraries(${PROJECT_NAME} 
        INTERFACE
        fmt::fmt)

sl_add_gtest(${PROJECT_NAME} field)
sl_add_gtest(${PROJECT_NAME} to)

