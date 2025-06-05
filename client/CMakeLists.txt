cmake_minimum_required(VERSION 3.28)
project(QuickFPS)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

if (APPLE) 
    set(RAYLIB_DIR lib/raylib-5.5_macos)
elseif (UNIX) 
    set(RAYLIB_DIR lib/raylib-5.5_macos)
elseif (WIN32) 
    set(RAYLIB_DIR lib/raylib-5.5_win)
endif()

add_subdirectory(lib/spdlog)


file(GLOB GAME_SOURCES "src/Game/*.cpp")
file(GLOB GAME_INCLUDE "src/Game/*.h")
file(GLOB RENDERER_SOURCES "src/Renderer/*.cpp")
file(GLOB RENDERER_INCLUDE "src/Renderer/*.h")
file(GLOB SYNC_SOURCES "src/Sync/*.cpp")
file(GLOB SYNC_INCLUDE "src/Sync/*.h")
add_executable(Main 
                    ${GAME_SOURCES}
                    ${RENDERER_SOURCES}
                    ${SYNC_SOURCES}
                    src/main.cpp
              ) 

target_include_directories(Main PUBLIC ${RAYLIB_DIR}/include)
target_include_directories(Main PUBLIC lib/spdlog/include)
target_link_directories(Main PUBLIC ${RAYLIB_DIR}/lib)
target_link_libraries(Main PUBLIC raylib)
target_link_libraries(Main PUBLIC spdlog)



# Link for libwldcapi.a

set(CAPI_SOURCES
    src/CAPI/c_api.cpp 
    ${GAME_SOURCES}
    ${RENDERER_SOURCES}
    ${SYNC_SOURCES}
)

add_library(wldcapi STATIC ${CAPI_SOURCES})

target_include_directories(wldcapi PUBLIC ${RAYLIB_DIR}/include)
target_include_directories(wldcapi PUBLIC lib/spdlog/include)
target_link_directories(wldcapi PUBLIC ${RAYLIB_DIR}/lib)
target_link_libraries(wldcapi PUBLIC raylib)
target_link_libraries(wldcapi PUBLIC spdlog)

message(STATUS "CAPI library created successfully.")

# ==============================================================================
# === Post-build command to copy files for Go CGO package ===
# ==============================================================================
set(GO_CGO_PACKAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../server/wldlib) # 目标 Go 包目录

file(MAKE_DIRECTORY ${GO_CGO_PACKAGE_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE MATCHES "^Debug$") # Handle "Debug" or "DEBUG"
    set(SPDLOG_LIB_NAME "libspdlogd.a")
else()
    set(SPDLOG_LIB_NAME "libspdlog.a")
endif()
set(SPDLOG_LIB_SOURCE_PATH "${CMAKE_BINARY_DIR}/lib/spdlog/${SPDLOG_LIB_NAME}") # Adjust if spdlog outputs elsewhere
set(WDLCAPI_LIB_SOURCE_PATH "$<TARGET_FILE:wldcapi>")
set(RAYLIB_LIB_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${RAYLIB_DIR}/lib/libraylib.a") # RAYLIB_DIR was defined earlier
set(CAPI_HEADER_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/CAPI/c_api.h") # Assuming this is the correct path

add_custom_command(
    TARGET wldcapi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SPDLOG_LIB_SOURCE_PATH}" "${GO_CGO_PACKAGE_DIR}/${SPDLOG_LIB_NAME}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${WDLCAPI_LIB_SOURCE_PATH}" "${GO_CGO_PACKAGE_DIR}/libwldcapi.a"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RAYLIB_LIB_SOURCE_PATH}" "${GO_CGO_PACKAGE_DIR}/libraylib.a"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CAPI_HEADER_SOURCE_PATH}" "${GO_CGO_PACKAGE_DIR}/c_api.h"
    COMMENT "Copying CGO dependencies to ${GO_CGO_PACKAGE_DIR}"
    VERBATIM # Important for proper command quoting and execution
)

message(STATUS "Go CGO package directory set to: ${GO_CGO_PACKAGE_DIR}")
message(STATUS "Will attempt to copy ${SPDLOG_LIB_NAME}, libwldcapi.a, libraylib.a, c_api.h after wldcapi is built.")