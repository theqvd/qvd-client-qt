#
# Regular cron jobs for the qvd-client-qt package
#
0 4	* * *	root	[ -x /usr/bin/qvd-client-qt_maintenance ] && /usr/bin/qvd-client-qt_maintenance
