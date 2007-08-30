Name:		ppl
Version:	0.10
Release:	1%{?dist}

Summary:	The Parma Polyhedra Library: a library of numerical abstractions
Group:		Development/Libraries
License:	GPL
URL:		http://www.cs.unipr.it/ppl/
Source:		ftp://ftp.cs.unipr.it/pub/ppl/releases/%{version}/%{name}-%{version}.tar.gz
#Patch0:		ppl-0.10-docfiles.patch
#Patch1:		ppl-0.10-configure.patch
#Patch2:		ppl-0.10-makefiles.patch
#Icon:
#Requires:
BuildRequires:	gmp-devel >= 4.1.3
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root
#Prefix:	/usr

%description
The Parma Polyhedra Library (PPL) is a library for the manipulation of
(not necessarily closed) convex polyhedra and other numerical
abstractions.  The applications of convex polyhedra include program
analysis, optimized compilation, integer and combinatorial
optimization and statistical data-editing.  The Parma Polyhedra
Library comes with several user friendly interfaces, is fully dynamic
(available virtual memory is the only limitation to the dimension of
anything), written in accordance to all the applicable standards,
exception-safe, rather efficient, thoroughly documented, and free
software.  This package provides all what is necessary to run
applications using the PPL through its C and C++ interfaces.

%package devel
Summary:	Development tools for the Parma Polyhedra Library C and C++ interfaces
Group:		Development/Libraries
Requires:	ppl = %{version}-%{release}, gmp-devel >= 4.1.3
%description devel
The header files, Autoconf macro, documentation, and ppl-config tool
developing applications using the Parma Polyhedra Library through its
C and C++ interfaces.

%package static
Summary:	Static archives for the Parma Polyhedra Library C and C++ interfaces
Group:		Development/Libraries
Requires:	ppl-devel = %{version}-%{release}
%description static
The static archives for the Parma Polyhedra Library C and C++ interfaces.

%package utils
Summary:	Utilities using the Parma Polyhedra Library
Group:		Development/Libraries
Requires:	ppl = %{version}-%{release}
BuildRequires:	glpk-devel >= 4.13
%description utils
This package contains the (mixed integer) linear programming solver ppl_lpsol
and the program ppl_lcdd for vertex/facet enumeration of convex polyhedra.

%ifnarch ppc64
%package gprolog
# The `gprolog' package is not available on ppc64:
# the GNU Prolog interface must thus be disabled for that architecture.
Summary:	The GNU Prolog interface of the Parma Polyhedra Library
Group:		Development/Libraries
BuildRequires:	gprolog >= 1.2.19
Requires:	ppl = %{version}-%{release}, ppl-pwl = %{version}-%{release}, gprolog >= 1.2.19
%description gprolog
This package adds GNU Prolog support to the Parma Polyhedra Library (PPL).
Install this package if you want to use the library in GNU Prolog programs.
%endif

%ifnarch ppc64
%package gprolog-static
Summary:	The static archive for the GNU Prolog interface of the Parma Polyhedra Library
Group:		Development/Libraries
Requires:	ppl-gprolog = %{version}-%{release}
%description gprolog-static
This package contains the static archive for the GNU Prolog interface
of the Parma Polyhedra Library.
%endif

%package swiprolog
Summary:	The SWI-Prolog interface of the Parma Polyhedra Library
Group:		Development/Libraries
BuildRequires:	pl >= 5.6.0
Requires:	ppl = %{version}-%{release}, ppl-pwl = %{version}-%{release}, pl >= 5.6.0, readline-devel
%description swiprolog
This package adds SWI-Prolog support to the Parma Polyhedra Library.
Install this package if you want to use the library in SWI Prolog programs.

%package swiprolog-static
Summary:	The static archive for the SWI-Prolog interface of the Parma Polyhedra Library
Group:		Development/Libraries
BuildRequires:	pl >= 5.6.0
Requires:	ppl-swiprolog = %{version}-%{release}
%description swiprolog-static
This package contains the static archive for the SWI-Prolog interface
of the Parma Polyhedra Library.

%package yap
Summary:	The YAP Prolog interface of the Parma Polyhedra Library
Group:		Development/Libraries
BuildRequires:	yap-devel >= 5.1.1
Requires:	ppl = %{version}-%{release}, ppl-pwl = %{version}-%{release}, yap >= 5.1.1
%description yap
This package adds YAP Prolog support to the Parma Polyhedra Library (PPL).
Install this package if you want to use the library in YAP Prolog programs.

%package yap-static
Summary:	The static archive for the YAP Prolog interface of the Parma Polyhedra Library
Group:		Development/Libraries
BuildRequires:	yap-devel >= 5.1.1
Requires:	ppl-yap = %{version}-%{release}
%description yap-static
This package contains the static archive for the YAP Prolog interface
of the Parma Polyhedra Library.

%package docs
Summary:	Documentation for the Parma Polyhedra Library
Group:		Documentation
Requires:	ppl = %{version}-%{release}
%description docs
This package contains all the documentations required by programmers
using the Parma Polyhedra Library (PPL).
Install this package if you want to program with the PPL.

%package pwl
Summary:	The Parma Watchdog Library: a C++ library for watchdog timers
Group:		Development/Libraries
%description pwl
The Parma Watchdog Library (PWL) provides support for multiple,
concurrent watchdog timers on systems providing setitimer(2).  This
package provides all what is necessary to run applications using the
PWL.  The PWL is currently distributed with the Parma Polyhedra
Library, but is totally independent from it.

%package pwl-devel
Summary:	Development tools for the Parma Watchdog Library
Group:		Development/Libraries
Requires:	ppl-pwl = %{version}-%{release}
%description pwl-devel
The header files, documentation and static libraries for developing
applications using the Parma Watchdog Library.

%package pwl-static
Summary:	Static archive for the Parma Watchdog Library
Group:		Development/Libraries
Requires:	ppl-pwl-devel = %{version}-%{release}
%description pwl-static
This package contains the static archive for the Parma Watchdog Library.


%prep
%setup -q
%patch0 -p1
%patch1 -p1
%patch2 -p1

%build
CPPFLAGS="-I%{_includedir}/glpk"
%ifnarch ppc64
CPPFLAGS="$CPPFLAGS -I%{_libdir}/gprolog-`gprolog --version 2>&1 | head -1 | sed -e "s/.* \([^ ]*\)$/\1/g"`/include"
%endif
CPPFLAGS="$CPPFLAGS -I%{_includedir}/Yap"
%configure --enable-shared --disable-rpath CPPFLAGS="$CPPFLAGS"
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} INSTALL="%{__install} -p" install
rm -f %{buildroot}%{_libdir}/*.la %{buildroot}%{_libdir}/%{name}/*.la

%files
%defattr(-,root,root,-)
%doc %{_datadir}/doc/%{name}/BUGS
%doc %{_datadir}/doc/%{name}/COPYING
%doc %{_datadir}/doc/%{name}/CREDITS
%doc %{_datadir}/doc/%{name}/ChangeLog
%doc %{_datadir}/doc/%{name}/NEWS
%doc %{_datadir}/doc/%{name}/README
%doc %{_datadir}/doc/%{name}/TODO
%doc %{_datadir}/doc/%{name}/gpl.*
%{_libdir}/libppl.so.*
%{_libdir}/libppl_c.so.*

%dir %{_libdir}/%{name}
%dir %{_datadir}/doc/%{name}

%files devel
%defattr(-,root,root,-)
%doc %{_datadir}/doc/%{name}/README.configure
%{_includedir}/ppl.hh
%{_includedir}/ppl_c.h
%{_libdir}/libppl.so
%{_libdir}/libppl_c.so
%{_bindir}/ppl-config
%{_mandir}/man1/ppl-config.1.gz
%{_datadir}/aclocal/ppl.m4

%files static
%defattr(-,root,root,-)
%{_libdir}/libppl.a
%{_libdir}/libppl_c.a

%files utils
%defattr(-,root,root,-)
%{_bindir}/ppl_lcdd
%{_bindir}/ppl_lpsol
%{_mandir}/man1/ppl_lcdd.1.gz
%{_mandir}/man1/ppl_lpsol.1.gz

%ifnarch ppc64
%files gprolog
%defattr(-,root,root,-)
%doc interfaces/Prolog/GNU/README.gprolog
%{_bindir}/ppl_gprolog
%{_libdir}/%{name}/ppl_gprolog.pl
%{_libdir}/%{name}/libppl_gprolog.so
%endif

%ifnarch ppc64
%files gprolog-static
%defattr(-,root,root,-)
%{_libdir}/%{name}/libppl_gprolog.a
%endif

%files swiprolog
%defattr(-,root,root,-)
%doc interfaces/Prolog/SWI/README.swiprolog
%{_bindir}/ppl_pl
%{_libdir}/%{name}/libppl_swiprolog.so
%{_libdir}/%{name}/ppl_swiprolog.pl

%files swiprolog-static
%defattr(-,root,root,-)
%{_libdir}/%{name}/libppl_swiprolog.a

%files yap
%defattr(-,root,root,-)
%doc interfaces/Prolog/YAP/README.yap
%{_libdir}/%{name}/ppl_yap.pl
%{_libdir}/%{name}/ppl_yap.so

%files yap-static
%defattr(-,root,root,-)
%{_libdir}/%{name}/ppl_yap.a

%files docs
%defattr(-,root,root,-)
%doc %{_datadir}/doc/%{name}/README.doc
%doc %{_datadir}/doc/%{name}/ppl-user-0.10-html/
%doc %{_datadir}/doc/%{name}/fdl.*
%doc %{_datadir}/doc/%{name}/ppl-user-0.10.pdf
%doc %{_datadir}/doc/%{name}/ppl-user-0.10.ps.gz

%files pwl
%defattr(-,root,root,-)
%doc Watchdog/BUGS Watchdog/COPYING Watchdog/CREDITS Watchdog/ChangeLog Watchdog/NEWS Watchdog/README
%{_libdir}/libpwl.so.*

%files pwl-devel
%defattr(-,root,root,-)
%doc Watchdog/README.doc
%{_includedir}/pwl.hh
%{_libdir}/libpwl.so

%files pwl-static
%defattr(-,root,root,-)
%{_libdir}/libpwl.a

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig
%post pwl -p /sbin/ldconfig
%postun pwl -p /sbin/ldconfig

%clean
rm -rf %{buildroot}

%changelog
* Thu Aug 30 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.10-1
- Started working on the PPL 0.10 spec file.

* Fri Jul 06 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-13
- Bug 246815 had been fixed: YAP support enabled again.

* Thu Jul 05 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-12
- Disable YAP support until bug 246815 is fixed.
- Bug 243084 has been fixed: `ppl-swiprolog' dependency on `readline-devel'
  removed.

* Thu Jul 05 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-11
- The `gprolog' package is not available on the ppc64 architecture:
  so do `ppl-gprolog' and `ppl-gprolog-static'.

* Tue Jul 03 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-10
- Use `%%{buildroot}' consistently, instead of  `$RPM_BUILD_ROOT'.

* Mon Jul 02 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-9
- Patch NEWS, TODO and doc/definitions.dox so as to use the
  UTF-8 encoding instead of ISO-8859.

* Tue Jun 12 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-8
- Patch the `libtool' script after `%%configure' so as to fix
  the rpath issue.
- Revised the description of the `devel' package.
- Include also the `TODO' file in the documentation of the main package.

* Thu Jun 07 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-7
- `%%install' commands revised.

* Thu Jun 07 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-6
- All the static archives are now in `*-static' packages.
- Packages `ppl-gprolog-devel', `ppl-swiprolog-devel' and `ppl-yap-devel'
  renamed `ppl-gprolog', `ppl-swiprolog' and `ppl-yap',
  respectively.
- As a workaround for a bug in the `pl' package (Bugzilla Bug 243084),
  `ppl-swiprolog' is now dependent on `readline-devel'.
- Added `%%dir %%{_datadir}/doc/%%{name}'.
- The `ppl-user-0.9-html' documentation directory is now properly listed.
- Remove installed *.la files.
- Added a `ppl-0.9-configure.patch' to avoid overriding CFLAGS and CXXFLAGS.

* Wed Jun 06 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-5
- Use `%%{_includedir}' and `%%{_libdir}' instead of `/usr/include'
  and `/usr/lib', respectively.
- Use `%%{_datadir}/doc/%%{name}' instead of `/usr/share/doc/ppl'.
- Replaced `%%defattr(-,root,root)' with `%%defattr(-,root,root,-)'.

* Fri Feb 23 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-4
- The user manual (in various formats) is now in the `docs' package.

* Thu Feb 22 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-3
- Dependencies for YAP fixed.
- Make sure the header files of GNU Prolog and YAP are found.

* Wed Feb 21 2007 Roberto Bagnara <bagnara@cs.unipr.it>
- Added missing dependencies.

* Sun Feb 18 2007 Roberto Bagnara <bagnara@cs.unipr.it>
- `%%doc' tags corrected for the Prolog interfaces.
- Tabs used consistently instead of spaces.

* Sat Feb 17 2007 Roberto Bagnara <bagnara@cs.unipr.it>
- Make `swiprolog-devel' depend on `pl' (at leat 5.6); documentation added.
- The `yap' package has been renamed `yap-devel' and completed.
- The `gprolog' package has been renamed `gprolog-devel' and completed.
- The `ppl_lcdd' and `ppl_lpsol' programs are now in a new `utils' package.
- The `ppl-config' program is now in the `devel' package.
- Modified the configuration command so that the `glpk-devel' include files
  are found.

* Sun Feb 11 2007 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-2
- The `%%_libdir/ppl' is no longer orphaned.
- Use `make %%{?_smp_mflags}' for building.
- The `swi' package has been renamed `swiprolog-devel'.

* Sat Feb 10 2007 Roberto Bagnara <bagnara@cs.unipr.it>
- Added the `%%changelog' section.
- `Release' set to 2.
- `Packager' and `Vendor' tags removed.
- `Summary' fields are no longer ended with a dot.
- The value of the `License' tag is now `GPL'.
- Removed unused definition of `builddir'.
- The `Name', `Version' and `Release' tags are now directly defined.
- Commented out the efinitions of the `Require' and `Prefix' tags.
- Set the `BuildRequires' tag to `gmp-devel'.
- Exploit the features of `%%setup', `%%configure', `%%install',
  `%%post' and `%%postun'.
- Mixed use of spaces and tabs avoided.
- Do configure with the --disable-rpath option so as to avoid
  hardcoding the path to search libraries.
- Do not include libtool archive files.
- Packages reorganized.

* Mon Jan 16 2006 Roberto Bagnara <bagnara@cs.unipr.it> 0.9-1
- Install gzipped man pages.
- The `Copyright' tag is no longer supported: use `License' instead.

* Wed Jan 11 2006 Roberto Bagnara <bagnara@cs.unipr.it>
- Include `ppl-config' in `%%{_bindir}' and the man pages in
  `%%{_mandir}/man1'.

* Tue Jan 10 2006 Roberto Bagnara <bagnara@cs.unipr.it>
- Require gcc-c++ to be at least 4.0.2.
- Distribute also `ppl_lpsol'.

* Tue Mar 01 2005 Roberto Bagnara <bagnara@cs.unipr.it>
- Wrong dependency fixed.

* Mon Feb 28 2005 Roberto Bagnara <bagnara@cs.unipr.it>
- URL for the source fixed.

* Fri Dec 24 2004 Roberto Bagnara <bagnara@cs.unipr.it>
- Sentence fixed.

* Thu Dec 23 2004 Roberto Bagnara <bagnara@cs.unipr.it>
- The file doc/README has been renamed README.doc so as not to conflict
  with the library's main README file.
- Require gcc-c++ to be exactly version 3.4.1.
- `Summary' updated to reflect the fact that the library now provides
  numerical abstractions other than convex polyhedra.

* Wed Aug 18 2004 Roberto Bagnara <bagnara@cs.unipr.it>
- Distribute more documentation.

* Mon Aug 16 2004 Roberto Bagnara <bagnara@cs.unipr.it>
- Added the `ppl_lcdd' program to the main package.
- Require gcc-c++ to be exactly version 3.4.1.
- We require gmp at least 4.1.3.

* Wed Jul 30 2003 Roberto Bagnara <bagnara@cs.unipr.it>
- Build an RPM package also for the PWL.
- The Prolog interfaces depend on the PWL.

* Tue Mar 04 2003 Roberto Bagnara <bagnara@cs.unipr.it>
- We require gmp at least 4.1.2.

* Fri Oct 04 2002 Roberto Bagnara <bagnara@cs.unipr.it>
- Require gcc-c++ 3.2 or later version.
- Require gmp 4.1 or later version.

* Sun Jun 30 2002 Roberto Bagnara <bagnara@cs.unipr.it>
- Mention not necessarily closed convex polyhedra in the main `%%description'.

* Tue Jun 25 2002 Roberto Bagnara <bagnara@cs.unipr.it>
- `%%files' section for gprolog package fixed.

* Mon Jun 24 2002 Roberto Bagnara <bagnara@cs.unipr.it>
- `%%files' section fixed for the yap package.
- The `%%files' sections of each package are now complete.

* Wed Jun 12 2002 Roberto Bagnara <bagnara@cs.unipr.it>
- Added file list for package gprolog.
- Updated file list for package swi.

* Thu Jun 06 2002 Roberto Bagnara <bagnara@cs.unipr.it>
- The `swi' package has now its `%%files' section.

* Wed Jun 05 2002 Roberto Bagnara <bagnara@cs.unipr.it>
- We will build several RPM packages out of our source tree.

* Mon Mar 04 2002 Roberto Bagnara <bagnara@cs.unipr.it>
- Require gcc-c++ 3.0.4 or later version.
- Require gmp 4.0.1 or later version.

* Sun Jan 27 2002 Roberto Bagnara <bagnara@cs.unipr.it>
- The move to libtool is complete: we can now build and distribute
  (with, e.g., RPM) static and dynamic versions of the library.

* Tue Oct 16 2001 Roberto Bagnara <bagnara@cs.unipr.it>
- Changed `Summary'.
- Changed `Packager' in view of PGP signatures.
- Changed `Group' to `Development/Libraries'.
- Require gcc-c++ 2.96-85 or later version.

* Mon Oct 15 2001 Roberto Bagnara <bagnara@cs.unipr.it>
- Now we build a relocatable package.

* Mon Oct 15 2001 Roberto Bagnara <bagnara@cs.unipr.it>
- A first cut at a working RPM spec file.
