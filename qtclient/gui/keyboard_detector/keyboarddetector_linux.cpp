#include "keyboarddetector.h"

#include <QDebug>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XKBrules.h>


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


QString KeyboardDetector::getKeyboardLayout() {
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
}
