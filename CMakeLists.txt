cmake_minimum_required(VERSION 3.9) # CMP0069 NEW
project(progressor)

# Options
option(STATIC_LIBCPP "Use static libstdc++" OFF)
option(STATIC_FREETYPE "Link static FreeType" OFF)
option(USE_LTO "Compile with Link-time Optimizations" ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${USE_LTO})

# Macros for packages
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
macro(find_package_specify STATIC)
    set(BKP_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    if(${STATIC})
        set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_LIBRARY_SUFFIX})
    endif()
    find_package(${ARGN})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${BKP_CMAKE_FIND_LIBRARY_SUFFIXES})
endmacro()

find_package_specify(OFF SDL2 REQUIRED)
find_package_specify(${STATIC_FREETYPE} Freetype REQUIRED)
find_package_specify(${STATIC_FREETYPE} PNG REQUIRED)

if(${STATIC_FREETYPE})
    set(FREETYPE_LIBRARIES "${FREETYPE_LIBRARIES};-lz")
endif()

add_definitions("-Wno-psabi -Wall -Wextra -fno-rtti -fno-stack-protector")
add_link_options($<$<CONFIG:RELEASE>:-s>)
add_link_options($<$<CONFIG:MINSIZEREL>:-s>)
add_link_options("-Wl,--gc-sections")

# Add 3rd party built-in libraries
add_subdirectory(3rdparty)

add_executable(${CMAKE_PROJECT_NAME}
    src/glad.c
    src/main.cpp
    src/style.cpp
    src/log.cpp
    src/process.cpp
    src/jet.cpp
)

# Set CPP std version
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

target_compile_definitions(${CMAKE_PROJECT_NAME}
    PRIVATE
        IMGUI_USER_CONFIG="${IMGUI_USER_CONFIG}"
)

# Add include directories
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC external
    "include/"
    "src/"
    ${IMGUI_INCLUDE_DIR}
    ${NLOHMANN_INC_PATH}
    ${SDL2_INCLUDE_DIR})

# Force static libg++
if(STATIC_LIBCPP)
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -static-libstdc++)
endif()

# Link necessary libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    ${PNG_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${SDL2_LIBRARY}
    ${IMGUI_LIBRARY})

if(MSVC)
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE /SUBSYSTEM:CONSOLE)
elseif(MINGW)
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -mconsole)
endif()