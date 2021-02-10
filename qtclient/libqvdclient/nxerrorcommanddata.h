#ifndef NXERRORCOMMANDDATA_H
#define NXERRORCOMMANDDATA_H

#include <QObject>

class NXErrorCommandData
{
public:
    enum DialogType {
        Unknown,
        OK,
        Error,
        YesNo,
        YesNoSuspend,
        Panic,
        Pulldown,
        Quit
    };

    NXErrorCommandData();

    QString    Caption{};
    QString    Message{};
    QString    Window{};
    bool       IsLocal{false};
    int        ParentPID{0};
    QString    Display{};
    DialogType Type{DialogType::Unknown};


    bool setTypeFromQString(const QString &type);
    bool isComplete() const {
        return !Message.isEmpty() && Type != DialogType::Unknown;
    }
};

#endif // NXERRORCOMMANDDATA_H
