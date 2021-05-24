#ifndef QTCLIENT_GLOBAL_H
#define QTCLIENT_GLOBAL_H

// https://doc.qt.io/qt-5/sharedlibrary.html

#include <QtCore/QtGlobal>

#if defined(LIBQVDCLIENT_LIBRARY)
#   define LIBQVDCLIENT_EXPORT Q_DECL_EXPORT
#else
#   define LIBQVDCLIENT_EXPORT Q_DECL_IMPORT
#endif

#endif // QTCLIENT_GLOBAL_H
