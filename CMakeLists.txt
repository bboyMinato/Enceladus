cmake_minimum_required(VERSION 3.21)

project(Enceladus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB_RECURSE ENCELADUS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp"
)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ImGui")
set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp"
)

add_executable(Enceladus
    main.cpp
    ${ENCELADUS_SOURCES}
    ${IMGUI_SOURCES}
)

target_compile_definitions(Enceladus PRIVATE
    $<$<CONFIG:Debug>:_DEBUG=1>
)

target_include_directories(Enceladus PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

target_link_libraries(Enceladus PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
    nlohmann_json::nlohmann_json
)

if(MSVC)
    target_compile_options(Enceladus PRIVATE /W3 /permissive-)
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY
        VS_STARTUP_PROJECT Enceladus
    )
    set_target_properties(Enceladus PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Enceladus>"
    )
else()
    target_compile_options(Enceladus PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(TARGET Enceladus POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
            "$<TARGET_FILE_DIR:Enceladus>/Assets"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
            "$<TARGET_FILE_DIR:Enceladus>/config.ini"
    COMMENT "Copying runtime assets"
)
