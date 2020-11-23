Name:       qvd-client-qt
Version:    4.3
Release:    1
Summary:    QVD Client
License:    GPL-3
Source0:    qvd-client-qt_%{version}.orig.tar.xz
%global debug_package %{nil}

%description
This is the client for Qindel's QVD virtual desktop solution


%prep
%setup

%build
make %{?_smp_mflags}

%install
mkdir -p %{buildroot}/usr/bin/
make install_files DESTDIR=%{buildroot} PREFIX=/usr


%files
/usr/bin/QVD_Client
/usr/lib64/libqvdclient.so*

%changelog
