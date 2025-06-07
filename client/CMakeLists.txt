cmake_minimum_required(VERSION 3.28)
project(QuickFPS)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(GENERATE_PROTO_TOGETHER ON)
set(CMAKE_BUILD_TYPE Debug)

message(STATUS "Current System Name: ${CMAKE_SYSTEM_NAME}")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Configuring for Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "Targeting Windows 64-bit")
        set(RAYLIB_DIR lib/raylib/raylib-5.5_win64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        message(STATUS "Targeting Windows 32-bit")
        set(RAYLIB_DIR lib/raylib/raylib-5.5_win32)
        # 设置 32 位特定的东西
    endif()
    if(MINGW)
        message(STATUS "Using MinGW toolchain")
    elseif(MSVC)
        message(STATUS "Using MSVC toolchain")
        message(FATAL_ERROR "MSVC toolchain is not supported yet. Please use MinGW or another compatible toolchain.")
    endif()
elseif(APPLE) 
    message(STATUS "Configuring for macOS")
    set(RAYLIB_DIR lib/raylib/raylib-5.5_macos)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Configuring for Linux")
    meesage(FATAL_ERROR "Linux is not supported yet. Please use Windows with MinGW.")
endif()

add_subdirectory(lib/spdlog)

# -------------Public Includes --------------------
include_directories(lib/spdlog/include)
include_directories(lib/concurrentqueue/include)


# -------------Game Logic --------------------

file(GLOB GAME_SOURCES "src/Game/*.cpp")
file(GLOB UTIL_SOURCES "src/Util/*.cpp")
file(GLOB INPUT_SOURCES "src/Input/*.cpp")
file(GLOB RENDERER_SOURCES "src/Renderer/*.cpp")

add_library(GameLogic STATIC 
    ${UTIL_SOURCES}
    ${GAME_SOURCES}
)
target_link_libraries(GameLogic PUBLIC spdlog)
target_include_directories(GameLogic PUBLIC src/Util)

# -------------NanoPB for Network --------------------

if(GENERATE_PROTO_TOGETHER)
    # message(STATUS "Generating protobuf files together with NanoPB")
    # set(Nanopb_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/nanopb/extra")
    # set(NANOPB_GENERATOR "${NANOPB_DIR}/generator/nanopb_generator.py")

    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/nanopb/extra")
    find_package(Nanopb REQUIRED)
    
    set(PFILE_NAME "gamedata")
    set(PROTO_DIR "${CMAKE_CURRENT_LIST_DIR}/src/Network/Protos")
    set(PROTO_FILE ${PROTO_DIR}/${PFILE_NAME}.proto)
    set(PROTO_OPTION_FILE ${PROTO_DIR}/${PFILE_NAME}.options)

    set(PROTO_GENE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/Network/c_protos")
    # set(PROTO_GENE_C ${PROTO_GENE_DIR}/sensor.pb.c)
    # set(PROTO_GENE_H ${PROTO_GENE_DIR}/sensor.pb.h)

    # add_custom_command(
    #     OUTPUT ${PROTO_GENE_C} ${PROTO_GENE_H}
    #     COMMAND ${Python_EXECUTABLE} ${NANOPB_GENERATOR} -D ${PROTO_GENE_DIR} -f ${PROTO_FILE} ${PROTO_OPTION_FILE}
    #     # DEPENDS ${PROTO_FILE} ${PROTO_OPTION_FILE}  # 当.proto或.options文件改变时，重新生成
    #     # COMMENT "Generating C sources from ${PROTO_FILE} with options ${OPTIONS_FILE}"
    # )

    nanopb_generate_cpp(TARGET proto 
        RELPATH ${PROTO_GENE_DIR}
        ${PROTO_FILE}
        )
    add_custom_target(
        generate_nanopb_protos ALL
        DEPENDS proto
    )
    add_custom_command(
        TARGET proto POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/${PFILE_NAME}.pb.c" "${PROTO_GENE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/${PFILE_NAME}.pb.h" "${PROTO_GENE_DIR}"
        COMMENT "Copying proto c/h to ${PROTO_GENE_DIR}"
        VERBATIM # Important for proper command quoting and execution
    )
endif()



# -------------Network --------------------
file(GLOB NETWORK_SOURCES "src/Network/*.cpp")
file(GLOB PROTO_SOURCE_FILES "lib/nanopb/*.c")
file(GLOB PROTO_C_FILES "src/Network/c_protos/*.c")
add_library(Network STATIC
    ${PROTO_SOURCE_FILES}
    ${PROTO_C_FILES}
    ${NETWORK_SOURCES}
)
target_include_directories(Network PUBLIC lib/asio/include)
target_include_directories(Network PUBLIC lib/nanopb/)
target_include_directories(Network PUBLIC src/Network/c_proto)
target_link_libraries(Network PUBLIC spdlog)
target_link_libraries(Network PUBLIC GameLogic)

target_compile_definitions(
    Network PUBLIC 
    ASIO_STANDALONE
    ASIO_HEADER_ONLY
)

# -------------QuickFPS --------------------
# Include : INPUT / RENDERER /Network / Game Logic

add_executable(QuickFPS 
                    ${INPUT_SOURCES}
                    ${RENDERER_SOURCES}
                    src/main.cpp
              ) 

target_include_directories(QuickFPS PUBLIC ${RAYLIB_DIR}/include)
target_link_directories(QuickFPS PUBLIC ${RAYLIB_DIR}/lib)
target_link_libraries(QuickFPS PUBLIC raylib)
# target_link_libraries(QuickFPS PUBLIC GameLogic) // Game Logic is already linked via Network
target_link_libraries(QuickFPS PUBLIC Network)


#  ---------------libwldcapi --------------------

set(CAPI_SOURCES
    src/CAPI/c_api.cpp 
)

add_library(wldcapi STATIC ${CAPI_SOURCES})

target_link_libraries(wldcapi PUBLIC GameLogic)

message(STATUS "CAPI library created successfully.")


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(Network PUBLIC ws2_32)
    target_link_libraries(QuickFPS PUBLIC winmm) # Windows Multimedia API for time functions
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    endif()
    if(MINGW)
    elseif(MSVC)
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
endif()

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
set(GAMELOGIC_LIB_SOURCE_PATH "$<TARGET_FILE:GameLogic>") # Assuming GameLogic is built as a target
set(WDLCAPI_LIB_SOURCE_PATH "$<TARGET_FILE:wldcapi>")
# set(RAYLIB_LIB_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${RAYLIB_DIR}/lib/libraylib.a") # RAYLIB_DIR was defined earlier
set(CAPI_HEADER_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/CAPI/c_api.h") # Assuming this is the correct path

add_custom_command(
    TARGET wldcapi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SPDLOG_LIB_SOURCE_PATH}" "${GO_CGO_PACKAGE_DIR}/${SPDLOG_LIB_NAME}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${WDLCAPI_LIB_SOURCE_PATH}" "${GO_CGO_PACKAGE_DIR}/libwldcapi.a"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GAMELOGIC_LIB_SOURCE_PATH}" "${GO_CGO_PACKAGE_DIR}/libGameLogic.a"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CAPI_HEADER_SOURCE_PATH}" "${GO_CGO_PACKAGE_DIR}/c_api.h"
    COMMENT "Copying CGO dependencies to ${GO_CGO_PACKAGE_DIR}"
    VERBATIM # Important for proper command quoting and execution
)

message(STATUS "Go CGO package directory set to: ${GO_CGO_PACKAGE_DIR}")
message(STATUS "Will attempt to copy ${SPDLOG_LIB_NAME}, libwldcapi.a, libraylib.a, c_api.h after wldcapi is built.")