cmake_minimum_required(VERSION 3.5)

project(QVD_Client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
find_package(X11 REQUIRED)


#set(TS_FILES QVD_Client_en_US.ts)

file(GLOB_RECURSE COMMON_SOURCES *cpp)
list(FILTER COMMON_SOURCES EXCLUDE REGEX "(windows|osx|linux)")

set(COMMON_SOURCES
    commandlineparser.cpp
    configloader.cpp
    listdialog.cpp
    main.cpp
    mainwindow.cpp
    qvdchecklistview.cpp
    sslerrordialog.cpp
    usbdevicelistmodel.cpp
    versioninfo.cpp
    )


if (WIN32)
    set(PLATFORM_SOURCES keyboard_detector/keyboarddetector_windows.cpp)
elsif(APPLE)
    set(PLATFORM_SOURCES keyboard_detector/keyboarddetector_osx.cpp)
else()
    set(PLATFORM_SOURCES keyboard_detector/keyboarddetector_linux.cpp)
endif()


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


# Link directly against libX11 on OSX, rather than specifying a library path.
# If we use -L/opt/X11/lib here, it may use /opt/X11/lib/libGL.dylib instead
# of /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib
#
# This causes an error of 'dyld: Symbol not found: _gll_noop'.

if (APPLE)
    target_link_libraries(QVD_Client PRIVATE /opt/X11/lib/libX11.dylib)
endif()


target_include_directories(QVD_Client PRIVATE ${libqvdclient_INCLUDE_DIRS} ${X11_INCLUDE_DIR})
target_link_directories(QVD_Client PRIVATE ${libqvdclient_LIB_DIRS})

target_link_libraries(QVD_Client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network ${X11_LIBRARIES} qvdclient)

