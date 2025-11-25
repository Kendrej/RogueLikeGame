cmake_minimum_required(VERSION 3.20)

# ===== Project =====
project(RogueLikeGame VERSION 1.0 LANGUAGES CXX)

# Przydatne dla indeksowania/clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Wyjścia binarek do build/<cfg>/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ===== Opcje jakości =====
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# ===== Opcjonalne zależności z vcpkg =====
# Automatycznie włącz, jeśli używany jest toolchain vcpkg; w przeciwnym razie wyłącz.
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    set(_USE_VCPKG_DEFAULT ON)
else()
    set(_USE_VCPKG_DEFAULT OFF)
endif()
option(ENABLE_VCPKG_DEPS "Find and link deps via vcpkg (GLFW, GLM, Vulkan, ImGui)" ${_USE_VCPKG_DEFAULT})

# ===== Sources =====
# Dodaj swoje pliki źródłowe tutaj (na start mamy tylko main.cpp)
add_executable(RogueLikeGame
        src/main.cpp
        src/engine/gfx/VulkanImGuiApp.cpp
        src/engine/vk/Window.cpp
        src/engine/vk/Instance.cpp
        src/engine/vk/Device.cpp
        src/engine/vk/Swapchain.cpp
        src/engine/vk/SyncAndDescriptors.cpp
        src/engine/gfx/ImGuiInit.cpp
        src/app/classes/entity.cpp
        src/engine/gfx/Assets.cpp
        src/app/GameSetup.cpp
        src/app/classes/Player.cpp
        src/app/classes/Map.cpp
        src/app/classes/npc/Npc.cpp
        src/app/classes/LivingEntity.cpp
        src/app/classes/World.cpp
        src/app/classes/StaticEntity.cpp
        src/app/classes/StaticEntity.h
        src/app/classes/npc/INpcController.cpp
        src/app/classes/npc/INpcController.h
        src/app/classes/npc/MeleeController.cpp
        src/app/classes/npc/MeleeController.h
        src/app/classes/MathUtils.h
        src/app/classes/AnimationController.cpp
        src/app/classes/npc/RangeController.cpp
        src/app/classes/npc/RangeController.h
        src/app/classes/Projectile.cpp
        src/app/classes/Projectile.h
        src/app/classes/CombatUtils.h
        src/app/classes/CombatUtils.cpp

)

# Jeśli masz własne nagłówki w ./include
target_include_directories(RogueLikeGame PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/app
    ${CMAKE_SOURCE_DIR}/src/app/classes
    ${CMAKE_SOURCE_DIR}/src/engine/gfx
)

# ===== Dependencies z vcpkg (opcjonalne) =====
if(ENABLE_VCPKG_DEPS)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(Vulkan REQUIRED)
    find_package(imgui CONFIG REQUIRED)

    # ZAMIENNIK za find_package(stb CONFIG REQUIRED):
    # Szukamy katalogu zawierającego stb_image.h (często jest to <vcpkg>/installed/<triplet>/include/stb)
    find_path(STB_INCLUDE_DIR
        NAMES stb_image.h
        PATH_SUFFIXES stb
    )
    if(NOT STB_INCLUDE_DIR)
        message(FATAL_ERROR "Nie znaleziono stb_image.h. Zainstaluj port 'stb' (vcpkg.json) i zrób reconfigure.")
    endif()

    target_link_libraries(RogueLikeGame PRIVATE
            imgui::imgui
            glfw
            Vulkan::Vulkan
            glm::glm
    )

    # Dodaj include dla stb i Vulkan/vcpkg (jawnie, dla VS)
    target_include_directories(RogueLikeGame PRIVATE ${STB_INCLUDE_DIR})
    if(DEFINED Vulkan_INCLUDE_DIR)
        target_include_directories(RogueLikeGame PRIVATE ${Vulkan_INCLUDE_DIR})
    endif()
    if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
        target_include_directories(RogueLikeGame PRIVATE ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
    endif()
else()
    message(STATUS "ENABLE_VCPKG_DEPS=OFF -> budujemy bez zewnętrznych bibliotek (Hello World)")
endif()

# ===== Warnings / kompilator =====
if(MSVC)
    target_compile_options(RogueLikeGame PRIVATE /W4 /permissive- /Zc:preprocessor)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(RogueLikeGame PRIVATE /WX)
    endif()
    # Do not use /external:I for include dirs; put all in AdditionalIncludeDirectories for compatibility
    set_property(TARGET RogueLikeGame PROPERTY MSVC_USE_EXTERNAL_INCLUDE_DIRS OFF)
else()
    target_compile_options(RogueLikeGame PRIVATE -Wall -Wextra -Wpedantic)
    if(WARNINGS_AS_ERRORS)
        target_compile_options(RogueLikeGame PRIVATE -Werror)
    endif()
endif()

# ===== Platformowe definicje =====
if(WIN32)
    target_compile_definitions(RogueLikeGame PRIVATE VK_USE_PLATFORM_WIN32_KHR NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

# ===== Testy (smoke test) =====
include(CTest)
if(BUILD_TESTING)
    enable_testing()
    # Uruchamia executable z parametrem --smoke
    add_test(NAME smoke
        COMMAND $<TARGET_FILE:RogueLikeGame> --smoke
        WORKING_DIRECTORY $<TARGET_FILE_DIR:RogueLikeGame>)
endif()

# ===== Windows: kopiowanie dll (opcjonalnie) =====
# Jeśli potrzeba, możesz dodać reguły kopiujące wymagane .dll do folderu bin.
# Na start zwykle nie jest to konieczne, bo glfw z vcpkg linkuje statycznie
# (w zależności od tripletu/ustawień).
if(WIN32 AND ENABLE_VCPKG_DEPS)
    # Copy all runtime DLLs from vcpkg installed bin folder to the target's output dir
    if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
        set(_VCPKG_BIN_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
        if(EXISTS "${_VCPKG_BIN_DIR}")
            add_custom_command(TARGET RogueLikeGame POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${_VCPKG_BIN_DIR}" "$<TARGET_FILE_DIR:RogueLikeGame>"
                COMMENT "Copying vcpkg runtime DLLs to output directory")
        endif()
    endif()
endif()

# ===== Kopiowanie assets do katalogu bin (runtime) =====
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET RogueLikeGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:RogueLikeGame>/assets"
        COMMENT "Copying assets to output directory")
endif()

