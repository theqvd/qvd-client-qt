#ifndef KEYBOARDDETECTOR_H
#define KEYBOARDDETECTOR_H

#include <QString>
#include "qvdclient_global.h"


/**
 * @brief Detects the keyboard
 *
 * This is an OS-specific class.
 *
 * On \b{Linux} it requests the keyboard from the X server
 * On \b{Windows} it calls the WinAPI
 * On \b{OSX} it uses a hardcoded "empty/empty" value unless CMake is explicitly asked to build with libX11 support.
 *
 *
 * @note On OSX, there are two issues:
 *
 * * Linking to libX11 is somewhat troublesome, as it can link to Homebrew's library by accident, which causes
 * weird issues with OpenGL.
 * * It doesn't do much good anyway, since the check just returns empty/empty.
 *
 */

class LIBQVDCLIENT_EXPORT KeyboardDetector
{
public:
    KeyboardDetector();
    static QString getKeyboardLayout();
};

#endif // KEYBOARDDETECTOR_H
