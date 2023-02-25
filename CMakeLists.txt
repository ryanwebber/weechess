cmake_minimum_required (VERSION 3.6)
project(Weechess VERSION 0.1.0)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Preventing an in-tree built.")
endif()

if (CMAKE_CXX_BYTE_ORDER STREQUAL "BIG_ENDIAN")
    message(FATAL_ERROR "Big endian systems are not supported due to bit twiddling performance optimizations.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "The default build type." FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

FetchContent_Declare(Catch2
    GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
    GIT_TAG         v3.0.1
    GIT_PROGRESS    TRUE
)

FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(ftxui
    GIT_REPOSITORY  https://github.com/ArthurSonzogni/ftxui
    GIT_TAG         v3.0.0
    GIT_PROGRESS    TRUE
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(spdlog
    GIT_REPOSITORY  https://github.com/gabime/spdlog
    GIT_TAG         v1.11.0
    GIT_PROGRESS    TRUE
)

FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

#
# Library
#

# Avoids linking or including spdlog in the library code
set(LIB_LOGGING_ENABLED ON)

set(LIB_TARGET weechess)
set(LIB_SOURCES
        lib/bit_board.cpp
        lib/board.cpp
        lib/fen.cpp
        lib/game_state.cpp
        lib/location.cpp
        lib/move_generator.cpp
        lib/move.cpp
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

#
# CLI
#

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

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(Catch)
include(CTest)

set(TEST_TARGET weechess-tests)
set(TEST_SOURCES
        tests/main.cpp
        tests/test_bit_board.cpp
        tests/test_fen.cpp
        tests/test_move_generation.cpp
        tests/test_move.cpp
        )

add_executable(${TEST_TARGET} ${TEST_SOURCES})

target_link_libraries(${TEST_TARGET}
        PRIVATE
            ${LIB_TARGET}
            Catch2::Catch2WithMain
            spdlog::spdlog
        )

catch_discover_tests(${TEST_TARGET})

cmake_policy(SET CMP0110 NEW)
add_test(NAME "CLI Sanity Check" COMMAND ${CLI_TARGET} --help)
