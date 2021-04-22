cmake_minimum_required(VERSION 3.5)
project(QVD_Client)

# You can tweak some common (for all subprojects) stuff here. For example:
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ON)
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



# When done tweaking common stuff, configure the components (subprojects).
# NOTE: The order matters! The most independent ones should go first.
add_subdirectory(libqvdclient) # B is a static library (depends on Boost)
add_subdirectory(gui) # C is a shared library (depends on B and external XXX)
