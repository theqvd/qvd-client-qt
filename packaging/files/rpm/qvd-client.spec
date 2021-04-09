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
mkdir -p %{buildroot}/usr/share/applications/
mkdir -p %{buildroot}/usr/share/pixmaps/
mkdir -p %{buildroot}/usr/bin/

install -m 755 gui/pixmaps/* %{buildroot}/usr/share/pixmaps/
make install_files DESTDIR=%{buildroot} PREFIX=/usr

cat > %{buildroot}/usr/share/applications/%{name}.desktop <<'EOF'
[Desktop Entry]
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

%post

touch -c /usr/share/icons/hicolor
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
  gtk-update-icon-cache -tq /usr/share/icons/hicolor 2>/dev/null ||:
fi

if command -v update-desktop-database >/dev/null 2>&1; then
  update-desktop-database -q /usr/share/applications 2>/dev/null ||:
fi

%clean
rm -rf %{buildroot}

%files
/usr/bin/QVD_Client
/usr/lib64/libqvdclient.so*
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/*

%changelog
