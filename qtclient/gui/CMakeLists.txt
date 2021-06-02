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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network REQUIRED)

#set(TS_FILES QVD_Client_en_US.ts)

set(COMMON_SOURCES
    commandlineparser.cpp
    configloader.cpp
    images.qrc
    listdialog.cpp
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    qvdchecklistview.cpp
    sslerrordialog.cpp
    usbdevicelistmodel.cpp
    versioninfo.cpp
    mainwindow.ui
    images.qrc
    )

set(PROJECT_SOURCES
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QVD_Client
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_BUILD_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(QVD_Client SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(QVD_Client
            ${PROJECT_SOURCES}
        )
    endif()

#    qt5_create_translation(QM_FILES ${CMAKE_BINARY_DIR} ${TS_FILES})
endif()


target_include_directories(QVD_Client PRIVATE ${libqvdclient_INCLUDE_DIRS})
target_link_directories(QVD_Client PRIVATE ${libqvdclient_LIB_DIRS})
target_link_libraries(QVD_Client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network qvdclient)

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

