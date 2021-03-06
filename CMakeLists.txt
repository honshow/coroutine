#
#   Author  : github.com/luncliff (luncliff@gmail.com)
#   Note
#       CMake support for non-VisualStudio projects
#
#   Support
#       - MSVC  + Windows   (Visual Studio 2017 or later)
#       - Clang + Windows   (Ninja)
#       - Clang + MacOS     (Unix Makefiles, Ninja)
#       - Clang + Linux     (Unix Makefiles, Ninja) : WSL, Ubuntu 16.04 and later
#
#   Working
#       - GCC   + Linux     (Ninja, Unix Makefiles)
#
#   Options
#       - BUILD_TESTING
#
cmake_minimum_required(VERSION 3.12) # C++ 20
project(coroutine 
        LANGUAGES CXX 
        VERSION 1.5.0
)

if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS true)
endif()
if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

#
# import cmake code snippets. see 'cmake/'
#
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(check-compiler)
include(report-project-info)

#
# import external libraries. see 'external/'
#
include(external-gsl)

#
# targets for CMake. see 'modules/'
#
set(MODULE_INTERFACE_DIR ${PROJECT_SOURCE_DIR}/interface)
if(CMAKE_CXX_COMPILER_ID MATCHES Clang AND WIN32)
    set(USE_PORTABLE_HEADER true)
endif()
add_subdirectory(modules/portable)
add_subdirectory(modules/system)    # coroutine_system
add_subdirectory(modules/net)       # coroutine_net

#
# export declared cmake targets
#
# 'coroutine-targets' is indeed better name, but without using 'configure_file()'
# the exporting step will be more complicated for non-CMake users.
# just merge all contents into the file 'coroutine-config.cmake'
#
install(EXPORT      coroutine-config
        # NAMESPACE   coro::
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/coroutine
)

#
# generate/install config & version info
#
include(CMakePackageConfigHelpers)
set(VERSION_FILE_PATH   ${CMAKE_BINARY_DIR}/cmake/coroutine-config-version.cmake)
write_basic_package_version_file(${VERSION_FILE_PATH}
    VERSION             ${PROJECT_VERSION}
    COMPATIBILITY       SameMajorVersion
)
install(FILES           ${VERSION_FILE_PATH} 
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/share/coroutine
)

#
# for testing, CTest will be used
#
if(NOT BUILD_TESTING)
    message(STATUS "Test is disabled.")
    return()
elseif(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    message(STATUS "This is not a root project. Skipping the tests")
    return()
elseif(ANDROID OR IOS)
    return()
endif()

enable_testing()

# helper for test codes
set(BUILD_TESTING OFF)
add_subdirectory(external/latch)    
add_subdirectory(external/sockets)

add_subdirectory(test)
