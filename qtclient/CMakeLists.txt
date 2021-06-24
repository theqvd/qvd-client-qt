cmake_minimum_required(VERSION 3.16)
project(QVD_Client)

# You can tweak some common (for all subprojects) stuff here. For example:
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)


if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_COLOR_MAKEFILE   ON)

# Remove 'lib' prefix for shared libraries on Windows
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()


if (SANITIZE)
    set(SANITIZE_FLAGS "-fsanitize=address -fsanitize=undefined")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZE_FLAGS}")
endif()

if(DEFINED ENV{QVD_VERSION_MAJOR})
    set(QVD_VERSION_MAJOR "$ENV{QVD_VERSION_MAJOR}")
endif()

if(DEFINED ENV{QVD_VERSION_MINOR})
    set(QVD_VERSION_MINOR "$ENV{QVD_VERSION_MINOR}")
endif()

if(DEFINED ENV{QVD_VERSION_REVISION})
    set(QVD_VERSION_REVISION "$ENV{QVD_VERSION_REVISION}")
endif()

if(DEFINED ENV{QVD_BUILD})
    set(QVD_BUILD "$ENV{QVD_BUILD}")
endif()

if(DEFINED ENV{QVD_VERSION_FULL})
    set(QVD_VERSION_FULL "$ENV{QVD_VERSION_FULL}")
endif()


if(DEFINED ENV{GIT_COMMIT})
    set(GIT_COMMIT "$ENV{GIT_COMMIT}")
endif()


if (NOT QVD_VERSION_MAJOR)
    set (QVD_VERSION_MAJOR 4)
endif()

if (NOT QVD_VERSION_MINOR)
    set (QVD_VERSION_MINOR 2)
endif()

if (NOT QVD_VERSION_REVISION)
    set (QVD_VERSION_REVISION 0)
endif()

if (NOT QVD_BUILD)
    set (QVD_BUILD 0)
endif()

set (QVD_VERSION "${QVD_VERSION_MAJOR}.${QVD_VERSION_MINOR}.${QVD_VERSION_REVISION}")

if (NOT QVD_VERSION_FULL)
    # QVD_VERSION_FULL is what we get from 'git describe', containing any additional
    # suffixes to the version, eg '4.2.0-rc1'.
    #
    # This can only be used in places where such version strings are acceptable.
    set(QVD_VERSION_FULL "${QVD_VERSION}")
endif()

execute_process(
    COMMAND git rev-parse --short HEAD
    RESULT_VARIABLE GIT_COMMIT_RESULT
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

message(STATUS "Building QVD with version ${QVD_VERSION_FULL} (${QVD_VERSION})")
message(STATUS "Build ${QVD_BUILD}")
message(STATUS "Git commit ${GIT_COMMIT}")

add_compile_definitions(QVD_VERSION=${QVD_VERSION})
add_compile_definitions(QVD_VERSION_MAJOR=${QVD_VERSION_MAJOR})
add_compile_definitions(QVD_VERSION_MINOR=${QVD_VERSION_MINOR})
add_compile_definitions(QVD_VERSION_REVISION=${QVD_VERSION_REVISION})
add_compile_definitions(QVD_BUILD=${QVD_BUILD})
add_compile_definitions(GIT_COMMIT="${GIT_COMMIT}")

# NOTE: The order matters! The most independent ones should go first.
add_subdirectory(libqvdclient) # B is a static library (depends on Boost)
add_subdirectory(gui) # C is a shared library (depends on B and external XXX)
