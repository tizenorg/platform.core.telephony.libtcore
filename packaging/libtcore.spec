%define major 3
%define minor 0
%define patchlevel 1
%define dbus_gen_code_path %{_includedir}/tel-headers/introspection

Name: libtcore
Summary: Telephony-core library
Version:    %{major}.%{minor}.%{patchlevel}
Release:    1
Group:      System/Libraries
License:    Apache-2.0
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

%package -n tel-headers
Summary:        Telephony-common header files
Group:          System/Libraries
BuildRequires:  python
BuildRequires:  python-xml
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gudev-1.0)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:  pkgconfig(gio-unix-2.0)
BuildRequires:  pkgconfig(gobject-2.0)

%description -n tel-headers
Telephony-common header files

%prep
%setup -q
cp %{SOURCE1001} .

%build
%cmake . -DDBUS_GEN_CODE_PATH=%{dbus_gen_code_path} -DVERSION=%{version}
make %{?jobs:-j%jobs}

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%install
%make_install
mkdir -p %{buildroot}/usr/share/license
cp LICENSE %{buildroot}/usr/share/license/%{name}

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libtcore*
/usr/share/license/%{name}

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/tcore.pc

%files -n tel-headers
%defattr(-,root,root,-)
%{_includedir}/tel-headers
%{dbus_gen_code_path}/tel_generated_code.h
%{_libdir}/pkgconfig/tel-headers.pc
%{_libdir}/libtel-headers*
