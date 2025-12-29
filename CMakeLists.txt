cmake_minimum_required(VERSION 3.16)

project(aex402_sdk
    VERSION 1.0.0
    DESCRIPTION "AeX402 AMM C++ SDK for Solana"
    LANGUAGES CXX
)

# Require C++17 or later
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(AEX402_BUILD_EXAMPLES "Build example programs" ON)
option(AEX402_BUILD_TESTS "Build test programs" OFF)
option(AEX402_ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wno-unused-parameter
    )

    if(AEX402_ENABLE_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
endif()

# Header-only library
add_library(aex402_sdk INTERFACE)

target_include_directories(aex402_sdk INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Create alias for consistent naming
add_library(aex402::sdk ALIAS aex402_sdk)

# ============================================================================
# Examples
# ============================================================================

if(AEX402_BUILD_EXAMPLES)
    add_executable(aex402_example example.cpp)
    target_link_libraries(aex402_example PRIVATE aex402_sdk)
endif()

# ============================================================================
# Tests
# ============================================================================

if(AEX402_BUILD_TESTS)
    enable_testing()

    add_executable(aex402_test_math test_math.cpp)
    target_link_libraries(aex402_test_math PRIVATE aex402_sdk)
    add_test(NAME math_tests COMMAND aex402_test_math)

    add_executable(aex402_test_accounts test_accounts.cpp)
    target_link_libraries(aex402_test_accounts PRIVATE aex402_sdk)
    add_test(NAME account_tests COMMAND aex402_test_accounts)
endif()

# ============================================================================
# Installation
# ============================================================================

include(GNUInstallDirs)

install(TARGETS aex402_sdk
    EXPORT aex402_sdk_targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
    aex402.hpp
    constants.hpp
    types.hpp
    accounts.hpp
    instructions.hpp
    math.hpp
    pda.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/aex402
)

install(EXPORT aex402_sdk_targets
    FILE aex402_sdk-targets.cmake
    NAMESPACE aex402::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aex402_sdk
)

# Generate config file
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/aex402_sdk-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/aex402_sdk-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aex402_sdk
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/aex402_sdk-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/aex402_sdk-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/aex402_sdk-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aex402_sdk
)

# ============================================================================
# Package information
# ============================================================================

message(STATUS "AeX402 SDK v${PROJECT_VERSION}")
message(STATUS "  Build examples: ${AEX402_BUILD_EXAMPLES}")
message(STATUS "  Build tests: ${AEX402_BUILD_TESTS}")
message(STATUS "  Sanitizers: ${AEX402_ENABLE_SANITIZERS}")
