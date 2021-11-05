Name:       libqnfcdc

Summary:    Qt interface to nfcd
Version:    1.0.0
Release:    1
License:    BSD
URL:        https://github.com/monich/libqnfcdc
Source0:    %{name}-%{version}.tar.bz2

%define libgnfcdc_version 1.0.6

Requires:       libgnfcdc >= %{libgnfcdc_version}
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(libglibutil)
BuildRequires:  pkgconfig(libgnfcdc) >= %{libgnfcdc_version}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}
%{!?qtc_make:%define qtc_make make}

%description
Qt interface to nfcd

%package devel
Summary:    Development files for %{name}
Requires:   %{name} = %{version}-%{release}
Requires:   %{name} = %{version}

%description devel
This package contains the development header files for %{name}

%prep
%setup -q -n %{name}-%{version}

%build
%qtc_qmake5
%qtc_make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake5_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/%{name}.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/%{name}.so
%{_libdir}/pkgconfig/qnfcdc.pc
%{_includedir}/qnfcdc/*.h
