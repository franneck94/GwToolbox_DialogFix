macro(add_tb_plugin PLUGIN)
    add_library(${PLUGIN} SHARED)

    file(GLOB SOURCES
        "src/${PLUGIN}/*.h"
        "src/${PLUGIN}/*.cpp"
        "src/${PLUGIN}/*.ixx"
        ${gwtoolboxpp_SOURCE_DIR}/plugins/Base/*.h
        ${gwtoolboxpp_SOURCE_DIR}/plugins/Base/*.ixx
        ${gwtoolboxpp_SOURCE_DIR}/plugins/Base/*.cpp
        )

    target_include_directories(${PLUGIN} PRIVATE
        ${gwtoolboxpp_SOURCE_DIR}/plugins/Base
        ${gwtoolboxpp_SOURCE_DIR}/GWToolboxdll
        )
    target_link_libraries(${PLUGIN} PUBLIC
        RestClient
        imgui
        directxtex
        gwca
        earcut
        easywsclient
        discord_game_sdk
        nlohmann_json::nlohmann_json
        simpleini
        imgui::fonts
        IconFontCppHeaders
        nativefiledialog
        wintoast
        helper_utils
        Stdafx
        fmt::fmt)
    target_link_options(${PLUGIN} PRIVATE
        "$<$<CONFIG:DEBUG>:/NODEFAULTLIB:LIBCMT>"
        )
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
    target_sources(${PLUGIN} PRIVATE ${SOURCES})
    target_compile_definitions(${PLUGIN} PRIVATE
        "NOMINMAX"
        "_WIN32_WINNT=_WIN32_WINNT_WIN7"
        "WIN32_LEAN_AND_MEAN"
        "VC_EXTRALEAN"
        "BUILD_DLL")

    set_target_properties(${PLUGIN} PROPERTIES FOLDER "src")
endmacro()
