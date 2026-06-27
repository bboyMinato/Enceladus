cmake_minimum_required(VERSION 3.21)
project(Enceladus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

# ---------------------------------------------------------------------------
# ImGui (vendored source, built as a static lib)
# ---------------------------------------------------------------------------
add_library(imgui STATIC
    ThirdParty/ImGui/imgui.cpp
    ThirdParty/ImGui/imgui_demo.cpp
    ThirdParty/ImGui/imgui_draw.cpp
    ThirdParty/ImGui/imgui_tables.cpp
    ThirdParty/ImGui/imgui_widgets.cpp
    ThirdParty/ImGui/backends/imgui_impl_sdl2.cpp
    ThirdParty/ImGui/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(imgui PUBLIC
    ThirdParty/ImGui
    ThirdParty/ImGui/backends
)

target_link_libraries(imgui PUBLIC
    SDL2::SDL2
)

# ---------------------------------------------------------------------------
# Enceladus (main executable)
# ---------------------------------------------------------------------------
add_executable(Enceladus
    main.cpp

    Core/Engine.cpp

    Core/Config/AppSetting.cpp

    Core/Managers/SoundManager.cpp
    Core/Managers/StateManager.cpp
    Core/Managers/TextManager.cpp
    Core/Managers/TextureManager.cpp

    Core/States/MainMenuState.cpp
    Core/States/PlayState.cpp

    Core/Systems/AnimationStateSystem.cpp
    Core/Systems/CameraSystem.cpp
    Core/Systems/CollisionSystem.cpp
    Core/Systems/ControllerSystem.cpp
    Core/Systems/InputSystem.cpp
    Core/Systems/MapConstraintSystem.cpp
    Core/Systems/MovementSystem.cpp
    Core/Systems/RenderSystem.cpp
    Core/Systems/SpriteAnimationSystem.cpp

    Core/UI/Button.cpp
    Core/UI/Menu.cpp

    Core/Utility/AnimationUtils.cpp

    Core/World/SceneLoader.cpp
    Core/World/TileMap.cpp
)

target_include_directories(Enceladus PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(Enceladus PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    imgui
)