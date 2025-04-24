cmake_minimum_required(VERSION 3.10)

# Nom du projet
project(VulkanVideos LANGUAGES CXX)

# Définir la version de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Inclure les fichiers sources
set(SOURCES
    VulkanVideos/main.cpp
    VulkanVideos/first_app.cpp
    VulkanVideos/model.cpp
    VulkanVideos/point_light_system.cpp
    VulkanVideos/GameObject.cpp
    VulkanVideos/SceneManager.cpp
    VulkanVideos/TriangleMesh.cpp
    VulkanVideos/Material.cpp
)

# Inclure les fichiers headers
set(HEADERS
    VulkanVideos/first_app.hpp
    VulkanVideos/GameObject.hpp
)

# Ajouter l'exécutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Inclure les répertoires d'en-têtes
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/VulkanVideos)

# Trouver Vulkan
find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    message(STATUS "Vulkan found: ${Vulkan_INCLUDE_DIRS}")
    target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
else()
    message(FATAL_ERROR "Vulkan SDK not found!")
endif()

# Inclure les headers GLFW
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/glfw/include)

# Ajouter le dossier contenant les .lib
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/glfw/lib-vc2022)

# Linker le fichier .lib de GLFW
target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)

# Inclure les headers GLM (header-only)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/glm)

# Options supplémentaires pour Vulkan
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_FORCE_RADIANS GLM_FORCE_DEPTH_ZERO_TO_ONE)

# Définir les options de compilation
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
