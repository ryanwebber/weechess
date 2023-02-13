cmake_minimum_required (VERSION 3.6)
project(Weechess VERSION 0.1.0)

include(FetchContent)
include(CTest)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Preventing an in-tree built.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "The default build type." FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

#
# Library
#

# Avoids linking or including spdlog in the library code
set(LIB_LOGGING_ENABLED ON)

set(LIB_TARGET weechess)
set(LIB_SOURCES
        lib/board.cpp
        lib/fen.cpp
        lib/game_state.cpp
        lib/location.cpp
        lib/piece.cpp
        )

ADD_LIBRARY(${LIB_TARGET} ${LIB_SOURCES})

target_include_directories(${LIB_TARGET}
        PRIVATE
            "lib"
        PUBLIC
            "include"
        )

target_compile_features(${LIB_TARGET} PUBLIC cxx_std_20)

if (LIB_LOGGING_ENABLED)
    target_compile_definitions(${LIB_TARGET}
            PRIVATE
                WEECHESS_LOGGING_ENABLED
            )
    
    target_link_libraries(${LIB_TARGET}
            PRIVATE
                spdlog::spdlog
            )
endif()

# CLI
#

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v3.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG v1.11.0
)

FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

set(CLI_TARGET weechess-cli)
set(CLI_SOURCES
        src/application/app_controller.cpp
        src/board_printer.cpp
        src/console.cpp
        src/log.cpp
        src/main.cpp
        src/string_utils.cpp
        )

add_executable(${CLI_TARGET} ${CLI_SOURCES})

target_link_libraries(${CLI_TARGET}
        PRIVATE
            ${LIB_TARGET}
            ftxui::screen
            ftxui::dom
            ftxui::component
            spdlog::spdlog
            )

target_include_directories(${CLI_TARGET}
        PRIVATE
            "include"
        )

target_compile_definitions(${CLI_TARGET}
        PRIVATE
            WEECHESS_LOG_FILE="weechess-cli.log"
        )

#
# Tests
#

add_test(NAME SanityCheck COMMAND ${CLI_TARGET} --h)
