%define major 0
%define minor 2
%define patchlevel 63

Name:           libtcore
Version:        %{major}.%{minor}.%{patchlevel}
Release:        1
License:        Apache
Summary:        Telephony-core library
Group:          System/Libraries
Source0:        libtcore-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gudev-1.0)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Telephony-core library

%package devel
Summary:        Telephony-core library (Development)
Group:          Development/Libraries
Requires:       %{name} = %{version}

%description devel
Telephony-core library (Development)

%prep
%setup -q

%build
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DLIB_INSTALL_DIR=%{_libdir} \
	-DVERSION=%{version} \
%ifarch %{arm}
%else
	-DARCH_EMUL=1 \
%endif

make %{?_smp_mflags}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%install
%make_install
mkdir -p %{buildroot}%{_datadir}/license

%files
%manifest libtcore.manifest
%defattr(-,root,root,-)
#%doc COPYING
%{_libdir}/libtcore*
#%{_bindir}/*
%{_datadir}/license/libtcore

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/tcore.pc

