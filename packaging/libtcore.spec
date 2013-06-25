Name: libtcore
Summary: Telephony-core library
Version: 0.1.84
Release:    1
Group:      System/Libraries
License:    Apache
Source0:    libtcore-%{version}.tar.gz
Source1001: 	libtcore.manifest
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gudev-1.0)

%description
Telephony-core library

%package devel
Summary:        Telephony-core library (Development)
Requires:       %{name} = %{version}
Group:          Development/Libraries

%description devel
Telephony-core library (Development)

%prep
%setup -q
cp %{SOURCE1001} .

%build
%cmake . -DVERSION=%{version}
make %{?jobs:-j%jobs}

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%install
%make_install

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libtcore*
/usr/share/license/libtcore

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/tcore.pc
