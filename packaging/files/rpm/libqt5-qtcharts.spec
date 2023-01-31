#
# spec file for package libqt5-qtcharts
#
# Copyright (c) 2017 SUSE LINUX GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#


%define qt5_snapshot 0
%define libname libQt5Charts5
%define base_name libqt5
%define real_version 5.6.1
%define so_version 5.6.1
%define tar_version qtcharts-opensource-src-5.6.1 
Name:           libqt5-qtcharts
Version:        5.6.1
Release:        0
Summary:        Qt 5 Charts Library
License:        GPL-3.0
Group:          Development/Libraries/C and C++
Url:            http://doc.qt.io/qt-5/qtcharts-index.html
Source:		https://download.qt.io/official_releases/qt/5.6/%{real_version}/submodules/%{tar_version}.tar.xz
BuildRequires:  xz
%requires_ge    libQtQuick5
%if %{qt5_snapshot}
#to create the forwarding headers
BuildRequires:  perl
%endif

%description
Qt Charts is a set of chart components.

Qt Charts module provides a set of easy to use chart components. It uses the Qt Graphics View Framework, therefore charts can be easily integrated to modern user interfaces. Qt Charts can be used as QWidgets, QGraphicsWidget, or QML types. Users can easily create impressive graphs by selecting one of the charts themes.

%prep
%setup -q -n qtcharts-opensource-src-%{real_version}

%package -n %{libname}
Summary:        Qt 5 Charts Library
Group:          System/Libraries
%requires_ge    libQt5Core5

%description -n %{libname}
Qt Charts module provides a set of chart components. It uses the Qt
Graphics View Framework, therefore charts can be integrated to user
interfaces. Qt Charts can be used as QWidgets, QGraphicsWidget, or
QML types. Users can create graphs by selecting one of the charts
themes.

This package contains a shared library for the QtChart.

%package -n %{libname}-devel
Summary:        Development files for the Qt 5 Charts Library
Group:          Development/Libraries/C and C++
Requires:       %{libname} = %{version}

%description -n %{libname}-devel
This package provides header files and shared libraries for development with
Qt Charts.

%post -n %{libname} -p /sbin/ldconfig
%postun -n %{libname} -p /sbin/ldconfig

%build
%if %{qt5_snapshot}
#force the configure script to generate the forwarding headers (it checks whether .git directory exists)
mkdir .git
%endif
%qmake5
%make_jobs
%qmake5
%make_jobs

%install
%qmake5_install

# kill .la files
rm -f %{buildroot}%{_libqt5_libdir}/lib*.la

%files -n %{libname}
%{_libqt5_libdir}/libQt5Charts.so.*

%files -n %{libname}-devel
%{_libqt5_libdir}/libQt5Charts.so
%{_libqt5_libdir}/libQt5Charts.prl
%{_libqt5_libdir}/cmake/Qt5Charts/
%{_libqt5_libdir}/pkgconfig/Qt5Charts.pc
%{_libqt5_libdir}/qt5/examples/
%{_libqt5_archdatadir}/mkspecs/modules/qt_lib_charts.pri
%{_libqt5_archdatadir}/mkspecs/modules/qt_lib_charts_private.pri
%{_libqt5_includedir}/QtCharts/
%exclude %{_libqt5_includedir}/QtCharts/

%changelog
