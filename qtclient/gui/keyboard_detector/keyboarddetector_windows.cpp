#include "keyboarddetector.h"
#include <QDebug>
#include <Windows.h>
#include <tchar.h>
#include <string>


KeyboardDetector::KeyboardDetector()
{

}



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

QString KeyboardDetector::getKeyboardLayout() {
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
}

