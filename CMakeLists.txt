cmake_minimum_required(VERSION 3.20)
project(GwatatoEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Dépendances (via vcpkg manifest mode) ----
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

find_path(GLM_ROOT_INCLUDE_DIR glm.hpp PATH_SUFFIXES glm REQUIRED)
find_path(GLFW_ROOT_INCLUDE_DIR glfw3.h PATH_SUFFIXES GLFW REQUIRED)

# ---- Sources ----
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/Engine/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Engine/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/Engine/*.h"
)
file(GLOB_RECURSE RENDERER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/Renderer/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Renderer/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/Renderer/*.h"
)
file(GLOB_RECURSE VULKAN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/Vulkan/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Vulkan/*.hpp"
)
file(GLOB UTILS_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/Utils/*.hpp"
)
source_group(TREE "${CMAKE_SOURCE_DIR}/src" PREFIX "Source Files"
    FILES ${ENGINE_SOURCES} ${RENDERER_SOURCES} ${VULKAN_SOURCES} ${UTILS_HEADERS}
)

add_executable(${PROJECT_NAME}
    ${ENGINE_SOURCES}
    ${RENDERER_SOURCES}
    ${VULKAN_SOURCES}
    ${UTILS_HEADERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/imageLoader
    ${GLM_ROOT_INCLUDE_DIR}
    ${GLFW_ROOT_INCLUDE_DIR}
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
    assimp::assimp
    nlohmann_json::nlohmann_json
    imgui::imgui
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# Pour que F5 dans Visual Studio marche avec tes chemins relatifs "models/..." et "shaders/..."
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
)

# ---- Compilation des shaders (remplace compile_shader.bat / le system() dans App.cpp) ----
find_program(GLSLC_EXECUTABLE glslc HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin")
if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc introuvable — vérifie que le Vulkan SDK est installé et que VULKAN_SDK est défini")
endif()

file(GLOB SHADER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/shaders/*.vert"
    "${CMAKE_SOURCE_DIR}/src/shaders/*.frag"
)

target_sources(${PROJECT_NAME} PRIVATE ${SHADER_SOURCES})
set_source_files_properties(${SHADER_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Shaders" FILES ${SHADER_SOURCES})

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SHADER_SPIRV "${CMAKE_SOURCE_DIR}/src/shaders/${SHADER_NAME}.spv")
    add_custom_command(
        OUTPUT ${SHADER_SPIRV}
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SHADER_SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${SHADER_NAME}"
    )
    list(APPEND SHADER_SPIRV_FILES ${SHADER_SPIRV})
endforeach()

add_custom_target(Shaders DEPENDS ${SHADER_SPIRV_FILES})
add_dependencies(${PROJECT_NAME} Shaders)