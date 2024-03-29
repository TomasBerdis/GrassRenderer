cmake_minimum_required(VERSION 3.18)
project(GrassRenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)

find_package(GPUEngine COMPONENTS REQUIRED geGL geUtil)
find_package(Qt5 COMPONENTS REQUIRED Gui Widgets)

set(sources
    src/main.cpp
    src/OpenGLWindow.cpp src/OpenGLWindow.hpp
    src/Camera.cpp src/Camera.hpp
    src/GrassField.cpp src/GrassField.hpp
    src/Terrain.cpp src/Terrain.hpp
    3rdparty/imgui/imconfig.h
    3rdparty/imgui/imgui.cpp
    3rdparty/imgui/imgui.h
    3rdparty/imgui/ImGuiRenderer.cpp
    3rdparty/imgui/ImGuiRenderer.h
    3rdparty/imgui/imgui_demo.cpp
    3rdparty/imgui/imgui_draw.cpp
    3rdparty/imgui/imgui_internal.h
    3rdparty/imgui/imgui_tables.cpp
    3rdparty/imgui/imgui_widgets.cpp
    3rdparty/imgui/imstb_rectpack.h
    3rdparty/imgui/imstb_textedit.h
    3rdparty/imgui/imstb_truetype.h
    3rdparty/imgui/QtImGui.cpp
    3rdparty/imgui/QtImGui.h
    )

find_file(grassVS grassVS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(grassFS grassFS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(grassTCS grassTCS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(grassTES grassTES.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(terrainVS terrainVS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(terrainFS terrainFS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(dummyVS dummyVS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(dummyFS dummyFS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(skyboxVS skyboxVS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(skyboxFS skyboxFS.glsl
    HINTS ${CMAKE_CURRENT_LIST_DIR}/shaders
)
find_file(debugTexture debug_texture.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(grassAlpha grass_alpha.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(heightMap height_map.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(skyboxTop skybox_top.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(skyboxBottom skybox_bottom.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(skyboxFront skybox_front.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(skyboxBack skybox_back.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(skyboxLeft skybox_left.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(skyboxRight skybox_right.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)

add_executable(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/3rdparty/imgui)
target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Widgets geGL geUtil)
target_compile_definitions(${PROJECT_NAME} PUBLIC   "GRASS_VS=\"${grassVS}\""           "GRASS_FS=\"${grassFS}\""           "GRASS_TCS=\"${grassTCS}\""     "GRASS_TES=\"${grassTES}\""
                                                    "TERRAIN_VS=\"${terrainVS}\""       "TERRAIN_FS=\"${terrainFS}\""
                                                    "DUMMY_VS=\"${dummyVS}\""           "DUMMY_FS=\"${dummyFS}\""
                                                    "SKYBOX_VS=\"${skyboxVS}\""         "SKYBOX_FS=\"${skyboxFS}\""
                                                    "DEBUG_TEXTURE=\"${debugTexture}\"" "GRASS_ALPHA=\"${grassAlpha}\""     "HEIGHT_MAP=\"${heightMap}\""
                                                    "SKYBOX_TOP=\"${skyboxTop}\""       "SKYBOX_BOTTOM=\"${skyboxBottom}\""
                                                    "SKYBOX_FRONT=\"${skyboxFront}\""   "SKYBOX_BACK=\"${skyboxBack}\""
                                                    "SKYBOX_LEFT=\"${skyboxLeft}\""     "SKYBOX_RIGHT=\"${skyboxRight}\"")

# setting up the MSVC helper var
get_target_property(Qt5dllPath Qt5::Gui IMPORTED_LOCATION_RELEASE)
get_filename_component(Qt5dllDir ${Qt5dllPath} DIRECTORY)
get_target_property(GPUEdllPath geGL IMPORTED_LOCATION_RELEASE)
get_filename_component(GPUEdllDir ${GPUEdllPath} DIRECTORY)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_GLOBAL_DllPath "${GPUEdllDir};${Qt5dllDir}")
SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})