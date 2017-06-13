# first two digits of version
%define release_version %(echo %{version} | awk -F. '{print $1"."$2}')

Name:           grapevine-foundation
Version:        0.1
Release:        1%{?dist}
Summary:        Grapevine Foundation components

License:        Closed-source
Source0:        grapevine-foundation-%{version}.tar.bz2

BuildRequires:  autoconf
BuildRequires:  automake
BuildRequires:  libtool
BuildRequires:  tar
BuildRequires:  bzip2
BuildRequires:  libsigc++-devel
BuildRequires:  glib2-devel
BuildRequires:  libpng-devel
BuildRequires:  libjpeg-turbo-devel
BuildRequires:  libhybris-libGLESv2-devel
BuildRequires:  libhybris-libEGL-devel
BuildRequires:  libseppo-common-devel
Requires:  libsigc++
Requires:  glib2
Requires:  libpng
Requires:  libjpeg-turbo
Requires:  libhybris-libGLESv2
Requires:  libhybris-libEGL
Requires:  libseppo-common

%description
Grapevine Foundation components

%package devel
Summary:        Library for Grapevine Foundation components
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description devel
The %{name}-devel package contains the static libraries and header files
needed for development with %{name}.


%prep


%setup -q -n %{name}-%{version}


%build
autoreconf -i
%configure --disable-desktoplinux --enable-eglnative CFLAGS="-DTSAIKKA -O3 -DDEBUG" CXXFLAGS="-std=c++11 -O3 -DTSAIKKA -DDEBUG"
make %{?_smp_mflags}


%install
%make_install
find $RPM_BUILD_ROOT -type f -name "*.la" -exec rm -f {} ';'


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/*.so

%files devel
%{_includedir}/*
%{_libdir}/pkgconfig/*.pc


%changelog
* Fri Dec 20 2016 Csaba Kertesz <csaba.kertesz@vincit.fi> - 0.1-1
- Initial release
