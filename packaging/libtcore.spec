#sbs-git:slp/pkgs/l/libtcore
Name: libtcore
Summary: Telephony-core library
Version: 0.1.114
Release:    1
Group:      System/Libraries
License:    Apache
Source0:    libtcore-%{version}.tar.gz
%ifarch %ix86
%if "%{simulator}" != "1"
patch0: 0001-hal-Add-set_sound_path-function.patch
patch1: 0002-hal-Add-new-entry-points-and-method-declarations.patch
patch2: 0003-hal-Add-new-method-definitions.patch
patch3: 0004-mux-Declare-new-public-API-for-HAL-plugin-manipulati.patch
patch4: 0005-mux-Remove-plateform-dependencies-from-core-multiple.patch
patch5: 0006-notification-Add-new-notifcation-type.patch
patch6: 0007-at-Add-Connect-to-success-responses.patch
patch7: 0008-hal-Add-new-setup_pdp-entry-point-declaration-and-DA.patch
patch8: 0009-hal-Add-tcore_hal_setup_pdp-definition-and-mana-DATA.patch
patch9: 0010-mux-Initialize-setup_pdp-entry-point.patch
%endif
%endif
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
%ifarch %ix86
%if "%{simulator}" != "1"
%patch0 -p1
%patch1 -p1
%patch2 -p1
%patch3 -p1
%patch4 -p1
%patch5 -p1
%patch6 -p1
%patch7 -p1
%patch8 -p1
%patch9 -p1
%endif
%endif

%build
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DVERSION=%{version}
make %{?jobs:-j%jobs}

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/usr/share/license

%files
%manifest libtcore.manifest
%defattr(-,root,root,-)
#%doc COPYING
%{_libdir}/libtcore*
#%{_bindir}/*
/usr/share/license/libtcore

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/tcore.pc
