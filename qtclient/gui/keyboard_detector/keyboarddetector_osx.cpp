#include "keyboarddetector.h"

KeyboardDetector::KeyboardDetector()
{

}


QString KeyboardDetector::getKeyboardLayout() {
    // "empty/empty" is a magic value that turns on a transparent mode that so far
    // works fine on OSX.
    //
    // Attempts for native detection were explored but weren't very successful so far:
    //    * Asking XQuartz returns the empty/empty hardcoded value anyway. Just hardcoding
    //      it here avoids having to link against libX11.
    //    * One solution found relied on Carbon, which is deprecated, so it can't be implemented.
    //    * Parsing plist files returns the layouts that exist on the system, but doesn't return
    //      the currently active layout.
    //    * Asking Qt via QInputMethod::locale returns the layout, but breaks the keyboard
    //      handling in NX for some yet unknown reason.

    return "empty/empty";
}
