cmake_minimum_required(VERSION 3.16)

project(SoLoud VERSION 2020.2.0 LANGUAGES C CXX)

# --- Options ---
option(SOLOUD_BACKEND_ALSA "Include ALSA backend" ON)
option(SOLOUD_BACKEND_OSS "Include OSS backend" ON)
option(SOLOUD_BACKEND_SDL2 "Include SDL2 backend" OFF)
option(SOLOUD_BACKEND_PORTAUDIO "Include PortAudio backend" OFF)
option(SOLOUD_BACKEND_OPENAL "Include OpenAL backend" OFF)
option(SOLOUD_BACKEND_JACK "Include JACK backend" OFF)
option(SOLOUD_BACKEND_MINIAUDIO "Include MiniAudio backend" OFF)
option(SOLOUD_BACKEND_NOSOUND "Include NoSound backend" OFF)
option(SOLOUD_BACKEND_NULL "Include Null backend" ON)
option(SOLOUD_BACKEND_COREAUDIO "Include CoreAudio backend" OFF)
option(SOLOUD_BACKEND_WASAPI "Include WASAPI backend" OFF)
option(SOLOUD_BACKEND_XAUDIO2 "Include XAudio2 backend" OFF)
option(SOLOUD_BACKEND_WINMM "Include WinMM backend" OFF)

option(SOLOUD_BUILD_C_API "Build C API" ON)
option(SOLOUD_BUILD_DEMOS "Build Demos" OFF)

# --- Dependencies ---
if(SOLOUD_BACKEND_ALSA)
    find_package(ALSA REQUIRED)
endif()

if(SOLOUD_BACKEND_SDL2)
    find_package(SDL2 REQUIRED)
endif()

if(SOLOUD_BACKEND_OPENAL)
    find_package(OpenAL REQUIRED)
endif()

if(SOLOUD_BACKEND_PORTAUDIO OR SOLOUD_BACKEND_JACK)
    find_package(PkgConfig REQUIRED)
endif()

if(SOLOUD_BACKEND_PORTAUDIO)
    pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
endif()

if(SOLOUD_BACKEND_JACK)
    pkg_check_modules(JACK REQUIRED jack)
endif()

# --- Sources ---
# Core sources
file(GLOB_RECURSE SOLOUD_CORE_SOURCES
    "src/core/*.cpp" "src/core/*.c"
    "src/filter/*.cpp" "src/filter/*.c"
    "src/audiosource/*.cpp" "src/audiosource/*.c"
)

set(SOLOUD_BACKEND_SOURCES "")
set(SOLOUD_DEFINES "")
set(SOLOUD_LIBRARIES "")
set(SOLOUD_INCLUDE_DIRS "")

# --- Backend Logic ---

if(SOLOUD_BACKEND_ALSA)
    file(GLOB_RECURSE SRC "src/backend/alsa/*.cpp" "src/backend/alsa/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_ALSA")
    list(APPEND SOLOUD_LIBRARIES ALSA::ALSA)
endif()

if(SOLOUD_BACKEND_OSS)
    file(GLOB_RECURSE SRC "src/backend/oss/*.cpp" "src/backend/oss/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_OSS")
endif()

if(SOLOUD_BACKEND_SDL2)
    # genie.lua maps WITH_SDL2 to src/backend/sdl
    file(GLOB_RECURSE SRC "src/backend/sdl/*.cpp" "src/backend/sdl/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_SDL2")
    
    if(TARGET SDL2::SDL2)
        list(APPEND SOLOUD_LIBRARIES SDL2::SDL2)
    else()
        list(APPEND SOLOUD_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
        list(APPEND SOLOUD_LIBRARIES ${SDL2_LIBRARIES})
    endif()
endif()

if(SOLOUD_BACKEND_PORTAUDIO)
    file(GLOB_RECURSE SRC "src/backend/portaudio/*.cpp" "src/backend/portaudio/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_PORTAUDIO")
    list(APPEND SOLOUD_INCLUDE_DIRS ${PORTAUDIO_INCLUDE_DIRS})
    list(APPEND SOLOUD_LIBRARIES ${PORTAUDIO_LIBRARIES})
endif()

if(SOLOUD_BACKEND_OPENAL)
    file(GLOB_RECURSE SRC "src/backend/openal/*.cpp" "src/backend/openal/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_OPENAL")
    list(APPEND SOLOUD_LIBRARIES OpenAL::OpenAL)
endif()

if(SOLOUD_BACKEND_JACK)
    file(GLOB_RECURSE SRC "src/backend/jack/*.cpp" "src/backend/jack/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_JACK")
    list(APPEND SOLOUD_INCLUDE_DIRS ${JACK_INCLUDE_DIRS})
    list(APPEND SOLOUD_LIBRARIES ${JACK_LIBRARIES})
endif()

if(SOLOUD_BACKEND_MINIAUDIO)
    file(GLOB_RECURSE SRC "src/backend/miniaudio/*.cpp" "src/backend/miniaudio/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_MINIAUDIO")
endif()

if(SOLOUD_BACKEND_NOSOUND)
    file(GLOB_RECURSE SRC "src/backend/nosound/*.cpp" "src/backend/nosound/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_NOSOUND")
endif()

if(SOLOUD_BACKEND_NULL)
    file(GLOB_RECURSE SRC "src/backend/null/*.cpp" "src/backend/null/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_NULL")
endif()

if(SOLOUD_BACKEND_COREAUDIO)
    file(GLOB_RECURSE SRC "src/backend/coreaudio/*.cpp" "src/backend/coreaudio/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_COREAUDIO")
    if(APPLE)
        find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
        list(APPEND SOLOUD_LIBRARIES ${AUDIOTOOLBOX_FRAMEWORK})
    endif()
endif()

if(SOLOUD_BACKEND_XAUDIO2 AND WIN32)
    file(GLOB_RECURSE SRC "src/backend/xaudio2/*.cpp" "src/backend/xaudio2/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_XAUDIO2")
endif()

if(SOLOUD_BACKEND_WINMM AND WIN32)
    file(GLOB_RECURSE SRC "src/backend/winmm/*.cpp" "src/backend/winmm/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_WINMM")
    list(APPEND SOLOUD_LIBRARIES winmm)
endif()

if(SOLOUD_BACKEND_WASAPI AND WIN32)
    file(GLOB_RECURSE SRC "src/backend/wasapi/*.cpp" "src/backend/wasapi/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_WASAPI")
endif()

# --- C API ---
if(SOLOUD_BUILD_C_API)
    file(GLOB_RECURSE C_API_SRC "src/c_api/*.cpp" "src/c_api/*.c")
    list(APPEND SOLOUD_CORE_SOURCES ${C_API_SRC})
endif()

# --- Library Target ---
add_library(soloud ${SOLOUD_CORE_SOURCES} ${SOLOUD_BACKEND_SOURCES})
add_library(SoLoud::SoLoud ALIAS soloud)

target_include_directories(soloud PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(SOLOUD_INCLUDE_DIRS)
    target_include_directories(soloud PRIVATE ${SOLOUD_INCLUDE_DIRS})
endif()

target_compile_definitions(soloud PRIVATE ${SOLOUD_DEFINES})
target_link_libraries(soloud PRIVATE ${SOLOUD_LIBRARIES})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(soloud PRIVATE pthread dl)
endif()

# Compiler flags for libc++ if user wants (handled via env usually, but we can verify)
# Note: The user requested "use clang and libc++ when compiling". 
# This CMake file supports that if configured with CXX=clang++ and CXXFLAGS=-stdlib=libc++
# However, if we want to enforce standards:
set_property(TARGET soloud PROPERTY CXX_STANDARD 11)
set_property(TARGET soloud PROPERTY CXX_STANDARD_REQUIRED ON)

# --- Install ---
include(GNUInstallDirs)

install(TARGETS soloud EXPORT SoLoudTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT SoLoudTargets
    FILE SoLoudTargets.cmake
    NAMESPACE SoLoud::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SoLoud
)

# --- Demos ---
if(SOLOUD_BUILD_DEMOS)
    add_executable(simplest demos/simplest/main.cpp)
    target_link_libraries(simplest PRIVATE soloud)
    
    # Add other demos here if dependencies allow
endif()
