cmake_minimum_required(VERSION 3.16)

project(QVD_Client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network Charts REQUIRED)

file(GLOB TS_FILES "${PROJECT_SOURCE_DIR}/i18n/*.ts")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)

set(COMMON_SOURCES
    commandlineparser.cpp
    configloader.cpp
    connectionstatistics.cpp
    connectionstatistics.ui
    connectionstatisticschart.cpp
    images.qrc
    listdialog.cpp
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    movingaverage.cpp
    qvdchecklistview.cpp
    sslerrordialog.cpp
    totpenrollment.cpp
    usbdevicelistmodel.cpp
    versioninfo.cpp
    mainwindow.ui
    totpenrollment.ui
    icon.rc
    images.qrc
    )

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/i18n")
qt5_add_translation(QM_FILES ${TS_FILES})
list(LENGTH TS_FILES LANGCOUNT)

message(STATUS "Will generate translations for ${LANGCOUNT} languages.")

set(PROJECT_SOURCES
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
    ${QM_FILES}
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QVD_Client
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(QVD_Client SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(QVD_Client WIN32
            ${PROJECT_SOURCES}
        )
    endif()
endif()

set_property(TARGET QVD_Client PROPERTY CXX_STANDARD 17)
set_property(TARGET QVD_Client PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(QVD_Client PRIVATE ${libqvdclient_INCLUDE_DIRS})
target_link_directories(QVD_Client PRIVATE ${libqvdclient_LIB_DIRS})
target_link_libraries(QVD_Client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Charts qvdclient)

install(TARGETS QVD_Client
       RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

   install(FILES "pixmaps/qvd.icns"
       TYPE DATA
       )

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QVD Client")
set(CPACK_PACKAGE_VENDOR "Qindel Group")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../README.md")
set(CPACK_PACKAGE_FILE_NAME "QVD Client")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/pixmaps/qvd.icns")
set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/pixmaps/qvd.icns")
set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${QVD_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${QVD_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${QVD_VERSION_REVISION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "QVD_Client")
set(CPACK_PACKAGE_EXECUTABLES "QVD_Client" "QVD_Client")
set(CPACK_THREADS 0)
set(CPACK_GENERATOR "Bundle;TXZ")

set(CPACK_BUNDLE_NAME "QVD Client")
#install(CODE "
#include(InstallRequiredSystemLibraries)
#include(BundleUtilities)
#message(status, \"Fixup: ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}\")
#FIXUP_BUNDLE(\"${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}\" \"\" \"\")
#")

include(CPack)

