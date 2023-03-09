cmake_minimum_required (VERSION 3.6)
project(weechess VERSION 0.1.0)

set(PROJECT_AUTHOR "Ryan Webber")

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Preventing an in-tree build.")
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wno-unused-function -Wno-unused-but-set-variable -Wno-deprecated-declarations")
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
    GIT_TAG         v4.0.0
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
        lib/generated/book_data.cpp
        lib/attack_maps.cpp
        lib/bit_board.cpp
        lib/board.cpp
        lib/book.cpp
        lib/engine.cpp
        lib/evaluator.cpp
        lib/fen.cpp
        lib/game_state.cpp
        lib/location.cpp
        lib/move_generator.cpp
        lib/move_query.cpp
        lib/move_sorter.cpp
        lib/move.cpp
        lib/piece.cpp
        lib/searcher.cpp
        lib/threading.cpp
        lib/transposition_table.cpp
        lib/zobrist.cpp
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
# Tools
#

set(TOOL_BOOKC_TARGET weechess-bookc)
add_executable(${TOOL_BOOKC_TARGET}
        tools/bookc/main.cpp
        )

target_include_directories(${TOOL_BOOKC_TARGET}
        PRIVATE
            "include"
        )

target_link_libraries(${TOOL_BOOKC_TARGET}
        PRIVATE
            ${LIB_TARGET}
            )

#
# Entrypoint
#

SET(ENTRYPOINT_TARGET weechess-entrypoint)
SET(ENTRYPOINT_SOURCES
        src/main.cpp
        )

add_executable(${ENTRYPOINT_TARGET} ${ENTRYPOINT_SOURCES})

target_include_directories(${ENTRYPOINT_TARGET}
        PRIVATE
            "include"
        )

set_target_properties(${ENTRYPOINT_TARGET} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        )

#
# Play
#

set(CMD_PLAY_TARGET weechess-play)
set(CMD_PLAY_SOURCES
        src/play/app_controller.cpp
        src/play/board_printer.cpp
        src/play/console.cpp
        src/play/log.cpp
        src/play/main.cpp
        src/play/string_utils.cpp
        )

add_executable(${CMD_PLAY_TARGET} ${CMD_PLAY_SOURCES})

target_link_libraries(${CMD_PLAY_TARGET}
        PRIVATE
            ${LIB_TARGET}
            ftxui::screen
            ftxui::dom
            ftxui::component
            spdlog::spdlog
            )

target_include_directories(${CMD_PLAY_TARGET}
        PRIVATE
            "include"
        )

#
# UCI
#

SET(CMD_UCI_TARGET weechess-uci)
SET(CMD_UCI_SOURCES
        src/uci/log.cpp
        src/uci/main.cpp
        )

add_executable(${CMD_UCI_TARGET} ${CMD_UCI_SOURCES})

target_link_libraries(${CMD_UCI_TARGET}
        PRIVATE
            ${LIB_TARGET}
            ftxui::screen
            ftxui::dom
            ftxui::component
            spdlog::spdlog
            )

target_include_directories(${CMD_UCI_TARGET}
        PRIVATE
            "include"
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
        tests/test_board.cpp
        tests/test_evaluator.cpp
        tests/test_fen.cpp
        tests/test_game_state.cpp
        tests/test_move_generation.cpp
        tests/test_move_query.cpp
        tests/test_move.cpp
        tests/test_searching.cpp
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
add_test(NAME "CLI Sanity Check" COMMAND ${CMD_PLAY_TARGET} --help)

add_compile_definitions(
    PRIVATE
        WEECHESS_LOG_FILE="weechess.log"
        WEECHESS_PROJECT_VERSION="${PROJECT_VERSION}"
        WEECHESS_PROJECT_AUTHOR="${PROJECT_AUTHOR}"
)
