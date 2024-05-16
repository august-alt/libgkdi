%define _unpackaged_files_terminate_build 1
%set_verify_elf_method unresolved=relaxed

Name: libgkdi
Version: 0.0.1
Release: alt1

Summary: Client library for MS-GKDI
License: GPLv2+
Group: Development
Url: https://github.com/august-alt/libgkdi

BuildRequires: rpm-macros-cmake cmake cmake-modules gcc-c++
BuildRequires: samba-devel samba-common-libs
BuildRequires: doxygen

Requires: cmake

Source0: %name-%version.tar

%description
Client library for MS-GKDI

%package devel
Summary: MS-GKDI Libraries and Header Files
Group: Development/C
Requires: %name = %version-%release

%description devel
%name-devel contains the libraries and header files needed to
develop programs which make use of %name

%prep
%setup -q

%build
%cmake
%cmake_build

%install
%cmakeinstall_std

%files
%_libdir/libgkdiclient.so.*

%files devel
%doc README.md
%doc INSTALL.md

%_includedir/gkdi/*.acf
%_includedir/gkdi/*.idl
%_includedir/gkdi/*.h

%_libdir/libgkdiclient.so

%_libdir/gkdi/GkdiConfig.cmake

%changelog
* Tue May 7 2024 Vladimir Rubanov <august@altlinux.org> 0.0.1-alt1
- 0.0.1-alt1
- Initial build
