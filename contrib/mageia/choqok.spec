%define git_repo choqok
%define git_head HEAD

Name:		choqok
Version:	%git_get_ver
Release:	%mkrel %git_get_rel2
Summary:	KDE Micro-Blogging Client
Source:		%git_bs_source %{name}-%{version}.tar.gz
Source1:	%{name}-gitrpm.version
Source2:	%{name}-changelog.gitrpm.txt
License:	GPLv3
Group:		Networking/News
Url:		http://choqok.gnufolks.org/

BuildRequires:	kdelibs4-devel
BuildRequires:	pkgconfig(QJson)
BuildRequires:	pkgconfig(qoauth)
BuildRequires:	pkgconfig(libattica)
Requires:	kdebase4-runtime

%description
Choqok is a Free/Open Source micro-blogging client for K Desktop 

%files -f %name.lang
%_kde_bindir/choqok
%_kde_libdir/kde4/*.so
%_kde_datadir/applications/kde4/choqok.desktop
%_kde_appsdir/choqok*
%_kde_appsdir/khtml/kpartplugins/*
%_kde_datadir/config.kcfg/*.kcfg
%_kde_iconsdir/*/*/*/*
%_kde_services/choqok_*.desktop
%_kde_servicetypes/choqok*.desktop
%_kde_services/ServiceMenus/*.desktop
%_datadir/dbus-1/services/org.kde.choqok.service

#-------------------------------------------------------------------

%define choqok_major 1
%define libchoqok %mklibname choqok %{choqok_major}

%package -n %libchoqok
Summary: %name library
Group: System/Libraries

%description -n %libchoqok
%name library.

%files -n %libchoqok
%_kde_libdir/libchoqok.so.%{choqok_major}*

#-------------------------------------------------------------------

%define twitterapihelper_major 1
%define libtwitterapihelper %mklibname twitterapihelper %twitterapihelper_major

%package -n %libtwitterapihelper
Summary: %name library
Group: System/Libraries

%description -n %libtwitterapihelper
%name library.

%files -n %libtwitterapihelper
%_kde_libdir/libtwitterapihelper.so.%{twitterapihelper_major}*

#-------------------------------------------------------------------

%package devel
Summary: %name development files
Group: Development/KDE and Qt
Requires: %{libchoqok} = %{version}-%{release}
Conflicts: %{name} < 0.2.3

%description devel
This package contains header files needed if you wish to build applications
based on %name.

%files devel
%_kde_libdir/libchoqok.so
%_kde_libdir/libtwitterapihelper.so
%_kde_includedir/choqok
%_kde_appsdir/cmake/modules/*

#--------------------------------------------------------------------

%prep
%git_get_source
%setup -q

%build
%cmake_kde4
%make

%install
rm -rf %{buildroot}
%{makeinstall_std} -C build

%find_lang %name --with-html

