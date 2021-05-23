#ifndef KEYBOARDDETECTOR_H
#define KEYBOARDDETECTOR_H

#include <QString>

/**
 * @brief Detects the keyboard
 *
 * This is an OS-specific class.
 *
 * On \b{Linux} it requests the keyboard from the X server
 * On \b{Windows} it calls the WinAPI
 * On \b{OSX} it uses a hardcoded "empty/empty" value for reasons explained
 * in the implementation file.
 */

class KeyboardDetector
{
public:
    KeyboardDetector();
    static QString getKeyboardLayout();
};

#endif // KEYBOARDDETECTOR_H
