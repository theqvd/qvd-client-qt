cmake_minimum_required(VERSION 3.16)

project(libqvdclient LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(USBIP_DATABASE "http://www.linux-usb.org/usb.ids")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Test REQUIRED)

add_library(qvdclient SHARED
    backends/qvdbackend.cpp
    backends/qvdnxbackend.cpp
    backends/xserverlauncher.cpp
    helpers/binaryfinder.cpp
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
    usbip/qvdusbip.cpp
    usbip/usbdevice.cpp
    usbip/usbdatabase.cpp
#    backends/qvdlibnxbackend.cpp
)

if (WIN32)
    set(EXTRA_LIBS wsock32 ws2_32 setupapi)
endif()

# We export this information so that other projects can use it
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR} CACHE INTERNAL "${PROJECT_NAME}: Include directories" FORCE)
set(${PROJECT_NAME}_LIB_DIRS ${PROJECT_BINARY_DIR} CACHE INTERNAL "${PROJECT_NAME}: Library directories" FORCE)


target_link_libraries(qvdclient PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network ${EXTRA_LIBS})
target_compile_definitions(qvdclient PRIVATE LIBQVDCLIENT_LIBRARY)

# Download database of USBIP ids
file(DOWNLOAD ${USBIP_DATABASE} "${CMAKE_CURRENT_BINARY_DIR}/usb.ids" SHOW_PROGRESS)

##########################################################################
# Testing
##########################################################################
enable_testing(true)
add_executable(test_linebuffer tests/tst_linebuffer.cpp)
add_executable(test_pulseaudio tests/tst_pulseaudio.cpp)
add_executable(test_pathtools  tests/tst_pathtools.cpp)
add_executable(test_usbip      tests/tst_usbip.cpp)

target_link_libraries(test_linebuffer PRIVATE Qt5::Test qvdclient)
target_link_libraries(test_pulseaudio PRIVATE Qt5::Test Qt5::Network qvdclient)
target_link_libraries(test_pathtools  PRIVATE Qt5::Test qvdclient)
target_link_libraries(test_usbip      PRIVATE Qt5::Test qvdclient)




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
