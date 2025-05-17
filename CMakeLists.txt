project(QuickFPS)
cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
add_executable(Main 
                    ${GAME_SOURCES}
                    src/main.cpp
              ) 

target_include_directories(Main PUBLIC ${RAYLIB_DIR}/include)
target_include_directories(Main PUBLIC lib/spdlog/include)
target_link_directories(Main PUBLIC ${RAYLIB_DIR}/lib)
target_link_libraries(Main PUBLIC raylib)
target_link_libraries(Main PUBLIC spdlog)