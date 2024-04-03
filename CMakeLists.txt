cmake_minimum_required(VERSION 3.20)
project("nc-tools" VERSION "4.0.0" LANGUAGES CXX)

if(${PROJECT_SOURCE_DIR} EQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "Don't be a fool, out-of-source build with your tool.")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(NC_TOOLS_BUILD_CONVERTER "Build nc-convert executable" ON)
option(NC_TOOLS_BUILD_TESTS "Include tests in build" ON)
option(NC_TOOLS_STATIC_ANALYSIS "Enable static analysis (MSVC Only)" OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(NC_TOOLS_COMPILE_OPTIONS
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wfatal-errors
        -Wno-cast-function-type
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "CLANG" OR CMAKE_CXX_COMPILER_ID STREQUAL "APPLECLANG")
    set(NC_TOOLS_COMPILE_OPTIONS
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(NC_TOOLS_COMPILE_OPTIONS
        /W4
        /WX
        /MP
    )
    if(${NC_TOOLS_STATIC_ANALYSIS})
        set(NC_TOOLS_COMPILE_OPTIONS
            ${NC_TOOLS_COMPILE_OPTIONS}
            /analyze
            /analyze:external-
            /external:W0
            /external:I${PROJECT_BINARY_DIR}/_deps
            /external:I${PROJECT_SOURCE_DIR}/source/external
            /wd6326 # Suppress 'potential comparison of constants'
        )
    endif()
endif()

add_definitions(-DNC_ASSERT_ENABLED)

include(FetchContent)

FetchContent_Declare(NcCommon
                     GIT_REPOSITORY https://github.com/NcStudios/NcCommon.git
                     GIT_TAG        v1.1.0
                     GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(NcCommon)

add_subdirectory(source)

if(NC_TOOLS_BUILD_TESTS)
    add_subdirectory(test)
endif()
