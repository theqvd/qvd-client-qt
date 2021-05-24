#include "keyboarddetector.h"
#include <QDebug>


#ifdef Q_OS_UNIX
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XKBrules.h>
#endif

#ifdef Q_OS_WIN32
#include <Windows.h>
#include <tchar.h>
#include <string>
#endif

KeyboardDetector::KeyboardDetector()
{

}


static QString getNext(unsigned char *buf, int *pos, int max_len) {
    if ( *pos == max_len-1)
        return QString();

    QString ret = QString((char*) &buf[*pos]);

    while( buf[*pos] != '\0' && *pos < max_len+1)
        (*pos)++;


    // Skip past the \0
    (*pos)++;

    return ret;
}

#ifdef Q_OS_WIN32
QString tchar_to_qstring(const TCHAR *str) {
    QString ret;

#ifdef UNICODE
    ret = QString::fromStdWString(std::wstring(str));
#else
    ret = QString::fromStdString(std::string(str));
#endif

    return ret;
}


QString get_windows_error() {
    DWORD err = GetLastError();
    LPVOID msgbuf;


    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&msgbuf, 0, NULL );
    return tchar_to_qstring((TCHAR*)msgbuf);
}
#endif

QString KeyboardDetector::getKeyboardLayout() {
#ifdef Q_OS_UNIX
    Display *disp = XOpenDisplay(nullptr);

     int format;
     unsigned long nitems, bytes_after;
     unsigned char *prop = nullptr;
     Atom xkb_rules_name, type;

     qInfo() << "Getting atom";
     xkb_rules_name = XInternAtom (disp, _XKB_RF_NAMES_PROP_ATOM, TRUE);
     if (xkb_rules_name == None) {
         qCritical() <<  "Could not get XKB rules atom";
         return QString();
     }


     qInfo() << "Getting property";
     if ( XGetWindowProperty(disp, DefaultRootWindow(disp), xkb_rules_name, 0, _XKB_RF_NAMES_PROP_MAXLEN, FALSE, XA_STRING,
                        &type, &format, &nitems, &bytes_after, &prop) != Success) {
         qWarning() << "Could not get _KXB_RF_NAMES_PROP_ATOM";
         XCloseDisplay(disp);
         return QString();
     }

     int pos = 0;

     QString rules   = getNext(prop, &pos, _XKB_RF_NAMES_PROP_MAXLEN);
     QString model   = getNext(prop, &pos, _XKB_RF_NAMES_PROP_MAXLEN);
     QString layouts = getNext(prop, &pos, _XKB_RF_NAMES_PROP_MAXLEN);
     QString variant = getNext(prop, &pos, _XKB_RF_NAMES_PROP_MAXLEN);
     QString options = getNext(prop, &pos, _XKB_RF_NAMES_PROP_MAXLEN);

     QStringList layout_list = layouts.split(",");
     QString layout;

     if ( layout_list.length() > 0 )
         layout = layout_list[0];

     qInfo() << "Rules  : " << rules;
     qInfo() << "Model  : " << model;
     qInfo() << "Layouts: " << layouts;
     qInfo() << "Layout : " << layout;
     qInfo() << "Variant: " << variant;
     qInfo() << "Options: " << options;

     XCloseDisplay(disp);

     qDebug() << "Returning detected layout: '" << model << "/" << layout << "'";
     return model + "/" + layout;
#endif
#ifdef Q_OS_WIN32
     TCHAR buf[KL_NAMELENGTH];
     TCHAR locale_name_buf[255];

     QString layout = "pc105/en";


     GetKeyboardLayoutName(buf);

     qDebug() << "Keyboard layout: " << buf;

     QString layout_id_str = tchar_to_qstring(buf+2); // skip past 0x
     bool ret;
     uint layout_id = layout_id_str.toUInt(&ret, 16);

     if ( ret ) {
         qDebug() << "Numeric id: " << layout_id;

         int gli_ret = GetLocaleInfo(layout_id, LOCALE_SISO639LANGNAME, locale_name_buf, sizeof(locale_name_buf) );
         if ( gli_ret != 0 ) {
             layout = tchar_to_qstring(locale_name_buf);
             qDebug() << "GetLocaleInfo returned '" << layout << "'";

             layout = "pc105/" + layout.toLower();
         } else {
             qCritical() << "GetLocaleInfo failed with error '" << gli_ret << "(" << get_windows_error() << "'. Can't determine keyboard layout.";
         }

     } else {
         qCritical() << "Failed to convert layout ID " << layout_id_str << " to uint";
     }


     qInfo() << "Final keyboard layout: " << layout;

     return layout;
#endif
}
