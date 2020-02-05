cmake_minimum_required(VERSION 3.12)
project(Z_MODULE
    DESCRIPTION "C++20 library for manipulating modular rings"
    LANGUAGES CXX
)

set(Z_MODULE_DETAIL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/detail/common_type.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/detail/prime_check.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/detail/io_helper.hpp
)
set(Z_MODULE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/z_module.hpp
)

add_library(z_module INTERFACE)
add_library(z_module::z_module ALIAS z_module)
target_sources(z_module INTERFACE "$<BUILD_INTERFACE:${Z_MODULE_DETAIL_HEADERS};${Z_MODULE_HEADERS}>")
target_include_directories(z_module INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(z_module SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

# Options to enable features at compile time
option(FGS_EXCEPTIONS_SUPPORT "whether or not to support exceptions" OFF)
option(FGS_PRIME_CHECK_SUPPORT "whether or not to support prime checking" OFF)
option(FGS_UNICODE_SUPPORT "whether or not to support unicode formatting" OFF)

if(${FGS_EXCEPTIONS_SUPPORT})
    target_compile_definitions(z_module INTERFACE -DFGS_EXCEPTIONS_SUPPORT)

    # This option is useless unless we have exceptions enabled
    if(${FGS_PRIME_CHECK_SUPPORT})
        target_compile_definitions(z_module INTERFACE -DFGS_PRIME_CHECK_SUPPORT)
    endif()
endif()
if(${FGS_UNICODE_SUPPORT})
    target_compile_definitions(z_module INTERFACE -DFGS_UNICODE_SUPPORT)
endif()

# Option for static analyzer (waiting for concepts in clang)
option(ENABLE_CLANG_TIDY "Enable testing with clang-tidy" OFF)
option(ENABLE_CPPCHECK "Enable testing with cppcheck" OFF)

option(BUILD_EXAMPLES   "Builds examples"   OFF)
option(BUILD_TESTS      "Builds tests"      OFF)
#option(BUILD_BENCHMARKS "Builds benchmarks" OFF)

if(BUILD_EXAMPLES OR BUILD_TESTS) #OR BUILD_BENCHMARKS)
    # Use ccache to speed up compilation if possible
    find_program(CCACHE ccache)
    if(CCACHE)
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
    endif()

    # Set a default build type if none was specified
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Setting build type to 'Debug' as none was specified.")
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)

        # Set the possible values of build type for cmake-gui
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug"
            "Release"
            "MinSizeRel"
            "RelWithDebInfo"
        )
    endif()

    # Link this 'library' to set the c++ standard / compile-time options requested
    add_library(project_options INTERFACE)
    # Use C++20 standar
    target_compile_features(project_options INTERFACE cxx_std_20)

    # Link this 'library' to use the standard warnings
    add_library(project_warnings INTERFACE)
    target_compile_options(project_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wold-style-cast
        -Wcast-align
        -Wnon-virtual-dtor
        -Woverloaded-virtual
    )

    if(ENABLE_CLANG_TIDY)
        find_program(CLANG_TIDY clang-tidy)
        if(CLANG_TIDY)
            set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
        else()
            message(SEND_ERROR "clang-tidy requested but executable not found")
        endif()
    endif()

    if(ENABLE_CPPCHECK)
        find_program(CPPCHECK cppcheck)
        if(CPPCHECK)
            set(CMAKE_CXX_CPPCHECK ${CPPCHECK}
                --enable=warning,performance
                --inconclusive
                --force
                --inline-suppr
        )
        else()
            message(SEND_ERROR "cppcheck requested but executable not found")
        endif()
    endif()

    # Download automatically, you can also just copy the conan.cmake file
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
       message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
       file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                     "${CMAKE_BINARY_DIR}/conan.cmake")
    endif()

    include(${CMAKE_BINARY_DIR}/conan.cmake)
    conan_check(REQUIRED)
    conan_cmake_run(
        CONANFILE conanfile.txt
        BASIC_SETUP
        CMAKE_TARGETS
        BUILD missing
    )

    if(BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
    if(BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()
    #if(BUILD_BENCHMARKS)
    #    add_subdirectory(benchmarks)
    #endif()
endif()
