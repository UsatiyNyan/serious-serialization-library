option(SL_SRLZ_NLOHMANN_JSON "${PROJECT_NAME}-nlohmann-json extension" OFF)
if(SL_SRLZ_NLOHMANN_JSON)
    add_subdirectory(nlohmann_json)
endif()

