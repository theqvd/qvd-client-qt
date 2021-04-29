cmake_minimum_required(VERSION 3.14)

project(libqvdclient LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Test REQUIRED)



if (WIN32)
    set(PLATFORM_SRC
        backends/xserverlauncher_windows.cpp
        slaveclient/commands/slavesharefolderwithvm_win.cpp
        pulseaudio/pulseaudio_win.cpp
        )
elseif(APPLE)
    set(PLATFORM_SRC
        backends/xserverlauncher_osx.cpp
        slaveclient/commands/slavesharefolderwithvm.cpp
        pulseaudio/pulseaudio.cpp
        )
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_SRC
        backends/xserverlauncher_linux.cpp
        slaveclient/commands/slavesharefolderwithvm.cpp
        pulseaudio/pulseaudio.cpp
        )
endif()


add_library(qvdclient SHARED
    backends/qvdbackend.cpp
#    backends/qvdlibnxbackend.cpp
    backends/qvdnxbackend.cpp
    helpers/binaryfinder.cpp
    helpers/linebuffer.cpp
    helpers/portallocator.cpp
    helpers/qvdfilehelpers.cpp
    nxerrorcommanddata.cpp
    qvdconnectionparameters.cpp
    qvdhttpserver.cpp
    qvdhttp.cpp
    qvdnetworkreply.cpp
    slaveclient/commands/connectaudio.cpp
    slaveclient/commands/qvdslavecommand.cpp
    slaveclient/commands/shareusbdevice.cpp
    slaveclient/qvdslaveclient.cpp
    socketforwarder.cpp
    qvdclient.cpp
    usbip/qvdusbip.cpp
    usbip/usbdevice.cpp
    ${PLATFORM_SRC}
)


# We export this information so that other projects can use it
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR} CACHE INTERNAL "${PROJECT_NAME}: Include directories" FORCE)
set(${PROJECT_NAME}_LIB_DIRS ${PROJECT_BINARY_DIR} CACHE INTERNAL "${PROJECT_NAME}: Library directories" FORCE)


target_link_libraries(qvdclient PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_compile_definitions(qvdclient PRIVATE LIBQVDCLIENT_LIBRARY)


##########################################################################
# Testing
##########################################################################
enable_testing(true)
add_executable(test_linebuffer tests/tst_linebuffer.cpp)
add_executable(test_pulseaudio tests/tst_pulseaudio.cpp)
add_executable(test_pathtools  tests/tst_pathtools.cpp)

target_link_libraries(test_linebuffer PRIVATE Qt5::Test qvdclient)
target_link_libraries(test_pulseaudio PRIVATE Qt5::Test Qt5::Network qvdclient)
target_link_libraries(test_pathtools  PRIVATE Qt5::Test qvdclient)




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
