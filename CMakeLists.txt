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

#
# Library
#

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

#
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

set(CLI_TARGET weechess-cli)
set(CLI_SOURCES
        src/application/app_controller.cpp
        src/board_printer.cpp
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
        )

target_include_directories(${CLI_TARGET}
        PRIVATE
            "include"
        )

#
# Tests
#

add_test(NAME SanityCheck COMMAND ${CLI_TARGET} --h)
