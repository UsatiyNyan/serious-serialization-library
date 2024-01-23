option(SL_SRLZ_NLOHMANN_JSON "${PROJECT_NAME}-nlohmann-json extension" OFF)
option(SL_SRLZ_PROTOBUF "${PROJECT_NAME}-protobuf extension" OFF)

if(SL_SRLZ_NLOHMANN_JSON)
    add_subdirectory(nlohmann_json)
endif()

if(SL_SRLZ_PROTOBUF)
    add_subdirectory(protobuf)
endif()

