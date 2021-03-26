cmake_minimum_required(VERSION 3.18)
project(GrassRenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)

find_package(GPUEngine COMPONENTS REQUIRED geGL geUtil)
find_package(Qt5 COMPONENTS REQUIRED Gui Widgets)

set(sources
    src/main.cpp
    src/OpenGLWindow.cpp src/OpenGLWindow.hpp
    src/SettingsWidget.cpp src/SettingsWidget.hpp
    src/Camera.cpp src/Camera.hpp
    src/GrassField.cpp src/GrassField.hpp)

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
find_file(debugTexture debug_texture.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(grassAlpha grass_alpha.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)
find_file(heightMap height_map.png
    HINTS ${CMAKE_CURRENT_LIST_DIR}/res
)

add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Widgets geGL geUtil)
target_compile_definitions(${PROJECT_NAME} PUBLIC   "GRASS_VS=\"${grassVS}\"" "GRASS_FS=\"${grassFS}\"" "GRASS_TCS=\"${grassTCS}\"" "GRASS_TES=\"${grassTES}\""
                                                    "TERRAIN_VS=\"${terrainVS}\"" "TERRAIN_FS=\"${terrainFS}\""
                                                    "DUMMY_VS=\"${dummyVS}\"" "DUMMY_FS=\"${dummyFS}\""
                                                    "DEBUG_TEXTURE=\"${debugTexture}\"" "GRASS_ALPHA=\"${grassAlpha}\"" "HEIGHT_MAP=\"${heightMap}\"")

# setting up the MSVC helper var
get_target_property(Qt5dllPath Qt5::Gui IMPORTED_LOCATION_RELEASE)
get_filename_component(Qt5dllDir ${Qt5dllPath} DIRECTORY)
get_target_property(GPUEdllPath geGL IMPORTED_LOCATION_RELEASE)
get_filename_component(GPUEdllDir ${GPUEdllPath} DIRECTORY)
set_target_properties(${PROJECT_NAME} PROPERTIES VS_GLOBAL_DllPath "${GPUEdllDir};${Qt5dllDir}")
SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})