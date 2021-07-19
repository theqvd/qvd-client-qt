cmake_minimum_required(VERSION 3.16)

project(libqvdclient LANGUAGES CXX)



set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(USBIP_DATABASE "http://www.linux-usb.org/usb.ids")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Test REQUIRED)
if (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    add_compile_definitions(USE_LIB_X11_KEYBOARD_DETECTION=1)
    message(STATUS "Using X11 based keyboard detection (Linux)")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)


add_library(qvdclient SHARED
    backends/qvdbackend.cpp
    backends/qvdnxbackend.cpp
    backends/xserverlauncher.cpp
    helpers/pathtools.cpp
    helpers/keyboarddetector.cpp
    helpers/linebuffer.cpp
    helpers/portallocator.cpp
    helpers/qvdfilehelpers.cpp
    nxerrorcommanddata.cpp
    pulseaudio/pulseaudio.cpp
    qvdclient.cpp
    qvdconnectionparameters.cpp
    qvdhttp.cpp
    qvdhttpserver.cpp
    qvdnetworkreply.cpp
    slaveclient/commands/connectaudio.cpp
    slaveclient/commands/qvdslavecommand.cpp
    slaveclient/commands/shareusbdevice.cpp
    slaveclient/commands/slavesharefolderwithvm.cpp
    slaveclient/qvdslaveclient.cpp
    socketforwarder.cpp
    usbip/usbdevicelist.cpp
    usbip/usbdevice.cpp
    usbip/usbdatabase.cpp
#    backends/qvdlibnxbackend.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)

set_property(TARGET qvdclient PROPERTY CXX_STANDARD 17)
set_property(TARGET qvdclient PROPERTY CXX_STANDARD_REQUIRED ON)


if (WIN32)
    set(EXTRA_LIBS wsock32 ws2_32 setupapi)
endif()

if (APPLE)
    if (USE_LIBX11_ON_MAC)
        # Link directly against libX11 on OSX, rather than specifying a library path.
        #
        # It seems Homebrew may be detected by find_package, which ends up linking against
        # the wrong libGL in /opt/X11/lib/libGL.dylib instead of using the correct
        # /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib
        #
        # This causes an error of 'dyld: Symbol not found: _gll_noop'.

        message(STATUS "Using libX11 on Mac OS with hardcoded paths")
        target_link_libraries(qvdclient PUBLIC /opt/X11/lib/libX11.dylib /opt/X11/lib/libGL.dylib)
        set(X11_INCLUDE_DIR "/opt/X11/include/")

        add_compile_definitions(USE_LIB_X11_KEYBOARD_DETECTION=1)
    else()
        message(STATUS "Not using X11 keyboard detection on Mac OS")
        message(STATUS "Configure with -DUSE_LIBX11_ON_MAC:bool=true to enable.")
    endif()
endif()

# We export this information so that other projects can use it
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR} CACHE INTERNAL "${PROJECT_NAME}: Include directories" FORCE)
set(${PROJECT_NAME}_LIB_DIRS ${PROJECT_BINARY_DIR} CACHE INTERNAL "${PROJECT_NAME}: Library directories" FORCE)

target_include_directories(qvdclient PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(qvdclient PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network ${EXTRA_LIBS} ${X11_LIBRARIES})
target_compile_definitions(qvdclient PRIVATE LIBQVDCLIENT_LIBRARY)

# Download database of USBIP ids
if (DOWNLOAD_USB_DATABASE)
    message(STATUS "Downloading USB database from ${USBIP_DATABASE}")
    file(DOWNLOAD ${USBIP_DATABASE} "${CMAKE_CURRENT_BINARY_DIR}/usb.ids")
else()
    message(STATUS "Not downloading USB database.")
endif()

##########################################################################
# Testing
##########################################################################
enable_testing(true)
add_executable(test_linebuffer        tests/tst_linebuffer.cpp)
add_executable(test_pulseaudio        tests/tst_pulseaudio.cpp)
add_executable(test_pathtools         tests/tst_pathtools.cpp)
add_executable(test_usbip             tests/tst_usbip.cpp)
add_executable(test_keyboard_detector tests/tst_keyboard_detector.cpp)

target_link_libraries(test_linebuffer        PRIVATE Qt5::Test qvdclient)
target_link_libraries(test_pulseaudio        PRIVATE Qt5::Test Qt5::Network qvdclient)
target_link_libraries(test_pathtools         PRIVATE Qt5::Test qvdclient)
target_link_libraries(test_usbip             PRIVATE Qt5::Test qvdclient)
target_link_libraries(test_keyboard_detector PRIVATE Qt5::Test qvdclient)




install(TARGETS qvdclient
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(TARGETS qvdclient
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(TARGETS qvdclient
       PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDE_DIR}/qvdclient
       )

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QVD Client library")
set(CPACK_PACKAGE_VENDOR "Qindel Group")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${QVD_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${QVD_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${QVD_VERSION_REVISION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "QVD_Client")
set(CPACK_PACKAGE_EXECUTABLES "QVD_Client" "QVD_Client")

include(CPack)
include(InstallRequiredSystemLibraries)
