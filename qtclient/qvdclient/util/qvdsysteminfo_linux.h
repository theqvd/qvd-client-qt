#ifndef QVDSYSTEMINFO_LINUX_H
#define QVDSYSTEMINFO_LINUX_H


#include <QObject>

class QVDSystemInfo_Impl
{
public:
	QVDSystemInfo_Impl();
	static QString getKeyboardLayout();

};

#endif // QVDSYSTEMINFO_LINUX_H
