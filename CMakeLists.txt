project(QuickFPS)
cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

# if()

# set(CMAKE_PREFIX_PATH "D:/raylib" ${CMAKE_PREFIX_PATH})
# find_package(raylib REQUIRED)

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