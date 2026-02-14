Name:       harbour-aas-installer-patcher

Summary:    AAS Installer Patcher
Version:    1.0.2
Release:    1
License:    MIT
URL:        https://github.com/RikudouSage/harbour-aas-installer-patcher
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

%qmake5 harbour-aas-installer-patcher.pro

%make_build


%install
%qmake5_install


desktop-file-install --delete-original         --dir %{buildroot}%{_datadir}/applications                %{buildroot}%{_datadir}/applications/*.desktop

%post
systemctl daemon-reload || :
if [ "$1" = "2" ]; then
  #upgrade
  systemctl stop harbour-aas-installer-patcher-daemon.service || :
fi

%preun
if [ "$1" = "0" ]; then
  systemctl daemon-reload || :
  systemctl stop harbour-aas-installer-patcher-daemon.service || :
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
%{_datadir}/polkit-1/actions/dev.chrastecky.aas_patcher.daemon*.policy
