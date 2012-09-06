#sbs-git:slp/pkgs/l/libtcore
Name: libtcore
Summary: Telephony-core library
Version: 0.1.61
Release:    1
Group:      System/Libraries
License:    Apache
Source0:    libtcore-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
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

%build
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}
make %{?jobs:-j%jobs}

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%install
rm -rf %{buildroot}
%make_install

%files
%defattr(-,root,root,-)
#%doc COPYING
%{_libdir}/libtcore*

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/tcore.pc
