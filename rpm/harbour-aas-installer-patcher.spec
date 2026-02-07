Name:       harbour-aas-installer-patcher

Summary:    AAS Installer Patcher
Version:    0.9.0
Release:    1
License:    LICENSE
URL:        http://example.org/
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   sailfish-polkit-agent
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils

%description
An app making it possible to change the installer of Android apps.


%prep
%setup -q -n %{name}-%{version}

%build

%qmake5 

%make_build


%install
%qmake5_install


desktop-file-install --delete-original         --dir %{buildroot}%{_datadir}/applications                %{buildroot}%{_datadir}/applications/*.desktop

%post
# Reload unit files, then restart daemon so updated binaries/policy are effective.
if [ -x /usr/bin/systemctl ]; then
    /usr/bin/systemctl daemon-reload >/dev/null 2>&1 || :
    /usr/bin/systemctl restart harbour-aas-installer-patcher-daemon.service >/dev/null 2>&1 || :
fi

# Make D-Bus re-read system bus policy changes.
if [ -x /usr/bin/dbus-send ]; then
    /usr/bin/dbus-send --system --print-reply \
      --dest=org.freedesktop.DBus / org.freedesktop.DBus.ReloadConfig >/dev/null 2>&1 || :
fi

# Refresh polkit policy cache so authorization changes apply immediately.
if [ -x /usr/bin/pkill ]; then
    /usr/bin/pkill -HUP polkitd >/dev/null 2>&1 || :
fi

%postun
if [ -x /usr/bin/systemctl ]; then
    /usr/bin/systemctl daemon-reload >/dev/null 2>&1 || :
fi

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_libexecdir}/harbour-aas-installer-patcher-daemon
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/dbus-1/system-services/dev.chrastecky.aas_patcher.daemon.service
%{_datadir}/dbus-1/system.d/dev.chrastecky.aas_patcher.daemon.conf
%{_unitdir}/harbour-aas-installer-patcher-daemon.service
%{_datadir}/polkit-1/actions/dev.chrastecky.aas_patcher.daemon.policy
