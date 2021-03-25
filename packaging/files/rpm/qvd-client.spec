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

mkdir -p %{buildroot}/usr/share/applications/

cat > %{buildroot}/usr/share/applications/%{name}.desktop <<'EOF'
Version=4.2
Name=QVD Client
Name[en_US]=QVD Client
Name[es_MX]=Cliente QVD
GenericName=QVD Desktop Client
GenericName[en_US]=QVD Desktop Client
GenericName[es_MX]=Cliente de escritorio de QVD
Comment=Virtual Desktop Interface
Comment[en_US]=Virtual Desktop Interface
Comment[es_MX]=Escritorio remoto virtual
Exec=QVD_Client
Terminal=false
X-MultipleArgs=false
Type=Application
Icon=qvd.svg
Categories=Application;Desktop;RemoteAccess;Office;
MimeType=application/xhtml_xml;x-scheme-handler/http;x-scheme-handler/https;
StartupNotify=true
X-AppInstall-Package=QVD_Client
EOF


%files
/usr/bin/QVD_Client
/usr/lib64/libqvdclient.so*
%{_datadir}/applications/%{name}.desktop

%changelog
