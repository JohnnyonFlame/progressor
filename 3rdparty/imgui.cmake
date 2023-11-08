# ImGui
set(IMGUI_LIBRARY "ImGui" CACHE INTERNAL "")
set(IMGUI_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

set(IMGUI_SOURCES
    "${IMGUI_SRC_PATH}/imgui.cpp"
    "${IMGUI_SRC_PATH}/imgui_draw.cpp"
    "${IMGUI_SRC_PATH}/imgui_tables.cpp"
    "${IMGUI_SRC_PATH}/imgui_widgets.cpp"
    "${IMGUI_SRC_PATH}/misc/freetype/imgui_freetype.cpp"
    "${IMGUI_SRC_PATH}/backends/imgui_impl_opengl3.cpp"
    "${IMGUI_SRC_PATH}/backends/imgui_impl_sdl2.cpp"
)

set(IMGUI_INCLUDE_DIR ${IMGUI_SRC_PATH} CACHE INTERNAL "")
set(IMGUI_USER_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/glad_imconfig.h" CACHE INTERNAL "")

# Import external SDL2 libraries
include_directories(
    ${CMAKE_SOURCE_DIR}/include/
    ${FREETYPE_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR})

add_library(${IMGUI_LIBRARY} OBJECT ${IMGUI_SOURCES})

target_link_libraries(${IMGUI_LIBRARY} ${GLAD_LIB_NAME} ${SDL2_LIBRARY})
target_include_directories(${IMGUI_LIBRARY} PUBLIC
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${IMGUI_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR})

target_compile_definitions(${IMGUI_LIBRARY}
    PRIVATE
        IMGUI_USER_CONFIG="${IMGUI_USER_CONFIG}"
)