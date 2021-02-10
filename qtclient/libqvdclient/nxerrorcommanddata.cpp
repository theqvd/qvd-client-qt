#include "nxerrorcommanddata.h"

NXErrorCommandData::NXErrorCommandData()
{

}

bool NXErrorCommandData::setTypeFromQString(const QString &type)
{
    QString tmp = type.toLower().trimmed();

    if ( tmp == "ok") {
        Type = DialogType::OK;
    } else if ( tmp == "yesno" ) {
        Type = DialogType::YesNo;
    } else if ( tmp == "error" ) {
        Type = DialogType::Error;
    } else if ( tmp == "panic" ) {
        Type = DialogType::Panic;
    } else if ( tmp == "pulldown") {
        Type = DialogType::Pulldown;
    } else if ( tmp == "yesnosuspend") {
        Type = DialogType::YesNoSuspend;
    } else if ( tmp == "quit") {
        Type = DialogType::Quit;
    } else {
        return false;
    }

    return true;
}
