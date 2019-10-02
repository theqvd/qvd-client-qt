#ifndef QVDSYSTEMINFO_H
#define QVDSYSTEMINFO_H

#include <QObject>


class QVDSystemInfo
{
public:
	QVDSystemInfo();

	static QString getKeyboardLayout();
};

#endif // QVDSYSTEMINFO_H
