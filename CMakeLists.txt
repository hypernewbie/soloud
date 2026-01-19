cmake_minimum_required(VERSION 3.16)

project(SoLoud VERSION 2020.2.0 LANGUAGES C CXX)

# --- Options ---
option(SOLOUD_BACKEND_MINIAUDIO "Include MiniAudio backend" ON)
option(SOLOUD_BACKEND_NULL "Include Null backend" ON)
option(SOLOUD_BACKEND_NOSOUND "Include NoSound backend" ON)

option(SOLOUD_BUILD_C_API "Build C API" ON)
option(SOLOUD_BUILD_DEMOS "Build Demos" OFF)
option(SOLOUD_BUILD_TESTS "Build Tests" OFF)

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

if(SOLOUD_BACKEND_MINIAUDIO)
    file(GLOB_RECURSE SRC "src/backend/miniaudio/*.cpp" "src/backend/miniaudio/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_BACKEND_SOURCES "miniaudio/miniaudio.c")
    list(APPEND SOLOUD_INCLUDE_DIRS "miniaudio")
    list(APPEND SOLOUD_DEFINES "WITH_MINIAUDIO")
endif()

if(SOLOUD_BACKEND_NULL)
    file(GLOB_RECURSE SRC "src/backend/null/*.cpp" "src/backend/null/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_NULL")
endif()

if(SOLOUD_BACKEND_NOSOUND)
    file(GLOB_RECURSE SRC "src/backend/nosound/*.cpp" "src/backend/nosound/*.c")
    list(APPEND SOLOUD_BACKEND_SOURCES ${SRC})
    list(APPEND SOLOUD_DEFINES "WITH_NOSOUND")
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

# Suppress multi-character constant warnings (user choice)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(soloud PRIVATE -Wno-multichar)
endif()

# Compiler flags for libc++ if user wants (handled via env usually, but we can verify)
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
endif()

# --- Tests ---
if(SOLOUD_BUILD_TESTS)
    enable_testing()
    add_executable(test_soloud tests/test_main.cpp)
    target_include_directories(test_soloud PRIVATE tests include)
    target_link_libraries(test_soloud PRIVATE soloud)
    add_test(NAME test_soloud COMMAND test_soloud)
endif()