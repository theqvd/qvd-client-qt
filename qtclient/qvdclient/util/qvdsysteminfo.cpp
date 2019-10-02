#include "qvdsysteminfo.h"
#include "qvdsysteminfo_linux.h"


QVDSystemInfo::QVDSystemInfo()
{

}

QString QVDSystemInfo::getKeyboardLayout()
{
	return QVDSystemInfo_Impl::getKeyboardLayout();
}
