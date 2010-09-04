#
# SPEC file for the cc65 C compiler and RedHat 7.x
#
# 03/08/2001, Ullrich von Bassewitz (uz@cc65.org)
#

###############################################################################
#				 Main package				      #
###############################################################################

Summary: Crosscompiler/Crossassembler for 6502 systems
%define name cc65
Name: %{name}
%define version 2.13.1
Version: %{version}
Release: 1
License: Freeware with exceptions - see docs
Group: Development/Languages
Source: ftp://ftp.musoftware.de/uz/cc65/%{name}-sources-%{version}.tar.bz2
Vendor: MU Softwareentwicklung
Packager: Ullrich von Bassewitz <uz@musoftware.de>
URL: http://www.cc65.org/
%ifos linux
BuildRequires: perl
BuildRequires: sgml-tools
%endif
BuildRoot: /var/tmp/%{name}-%{version}

%description
A C crosscompiler for 6502 systems, including a macroassembler that
supports 6502, 65SC02 and 65816 CPUs, a linker, an archiver and some
other tools. To create programs for one of the supported target
machines, you have to install at least one of the target platform
packages in addition to the main package.

%files
%defattr(644,root,root,755)
%doc announce.txt
%doc doc/BUGS
%doc doc/CREDITS
%doc doc/readme.1st
%attr(755,root,root)   		/usr/bin/ar65
%attr(755,root,root)   		/usr/bin/ca65
%attr(755,root,root) 		/usr/bin/ca65html
%attr(755,root,root)   		/usr/bin/cc65
%attr(755,root,root)   		/usr/bin/cl65
%attr(755,root,root)   		/usr/bin/co65
%attr(755,root,root) 		/usr/bin/da65
%attr(755,root,root)   		/usr/bin/ld65
%attr(755,root,root)   		/usr/bin/od65
%attr(755,root,root) %dir	/usr/lib/cc65
%attr(755,root,root) %dir	/usr/lib/cc65/cfg
%attr(755,root,root) %dir	/usr/lib/cc65/emd
%attr(755,root,root) %dir	/usr/lib/cc65/joy
%attr(755,root,root) %dir	/usr/lib/cc65/lib
%attr(755,root,root) %dir	/usr/lib/cc65/obj
%attr(755,root,root) %dir	/usr/lib/cc65/ser
%attr(755,root,root) %dir	/usr/lib/cc65/tgi
%attr(755,root,root) %dir	/usr/lib/cc65/include
%attr(644,root,root)   		/usr/lib/cc65/include/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/include/em
%attr(644,root,root)   		/usr/lib/cc65/include/em/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/include/joystick
%attr(644,root,root)   		/usr/lib/cc65/include/joystick/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/include/mouse
%attr(644,root,root)   		/usr/lib/cc65/include/mouse/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/include/sys
%attr(644,root,root)   		/usr/lib/cc65/include/sys/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/include/tgi
%attr(644,root,root)   		/usr/lib/cc65/include/tgi/*.h
%attr(755,root,root) %dir	/usr/lib/cc65/asminc
%attr(644,root,root)   		/usr/lib/cc65/asminc/*.inc



###############################################################################
#                          Documentation subpackage                           #
###############################################################################

%package doc

Requires: %{name} = %{version}
License: Freeware
Summary: Documentation for the cc65 C compiler
Group: Development/Languages

%description doc
This package contains the documentation for the cc65 C compiler, mostly
in HTML format. Other formats can be generated from the sources.

%files doc
%doc doc/*.html
%doc doc/compile.txt
%doc doc/internal.txt
%doc doc/newvers.txt
%doc samples



###############################################################################
#		     	      Apple ][ subpackage     			      #
###############################################################################

%package apple2
Requires: %{name} = %{version}
License: Freeware
Summary: Apple ][ specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description apple2
This package contains the header files and libraries needed to write
programs for the Apple ][ using the cc65 crosscompiler.

%files apple2
%attr(644,root,root) 		/usr/lib/cc65/lib/apple2.lib
%attr(644,root,root) 		/usr/lib/cc65/obj/apple2-*.o
%attr(644,root,root) 		/usr/lib/cc65/emd/a2.*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/a2.*.joy
%attr(644,root,root) 		/usr/lib/cc65/mou/a2.*.mou
%attr(644,root,root) 		/usr/lib/cc65/ser/a2.*.ser
%attr(644,root,root) 		/usr/lib/cc65/tgi/a2.*.tgi
%attr(644,root,root)   	       	/usr/lib/cc65/cfg/apple2-*.cfg



###############################################################################
#                            Apple //e subpackage                             #
###############################################################################

%package apple2enh
Requires: %{name} = %{version}
License: Freeware
Summary: Apple //e specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description apple2enh
This package contains the header files and libraries needed to write
programs for the Apple //e using the cc65 crosscompiler.

%files apple2enh
%attr(644,root,root) 		/usr/lib/cc65/lib/apple2enh.lib
%attr(644,root,root) 		/usr/lib/cc65/obj/apple2enh-*.o
%attr(644,root,root) 		/usr/lib/cc65/emd/a2e.*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/a2e.*.joy
%attr(644,root,root) 		/usr/lib/cc65/mou/a2e.*.mou
%attr(644,root,root) 		/usr/lib/cc65/ser/a2e.*.ser
%attr(644,root,root) 		/usr/lib/cc65/tgi/a2e.*.tgi
%attr(644,root,root)   	       	/usr/lib/cc65/cfg/apple2enh-*.cfg



###############################################################################
#	   	      	       Atari subpackage	 	      		      #
###############################################################################

%package atari
Requires: %{name} = %{version}
License: Freeware
Summary: Atari specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description atari
This package contains the header files and libraries needed to write
programs for the 8 bit Atari machines using the cc65 crosscompiler.

%files atari
%attr(644,root,root)  	       	/usr/lib/cc65/lib/atari.lib
%attr(644,root,root) 	       	/usr/lib/cc65/joy/atari*.joy



###############################################################################
#                            Oric Atmos subpackage                            #
###############################################################################

%package atmos
Requires: %{name} = %{version}
License: Freeware
Summary: Oric Atmos specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description atmos
This package contains the header files and libraries needed to write
programs for the Oric Atmos using the cc65 crosscompiler.

%files atmos
%attr(644,root,root)		/usr/lib/cc65/lib/atmos.lib
%attr(644,root,root) 		/usr/lib/cc65/tgi/atmos-*.tgi



###############################################################################
#		     		C128 subpackage	 			      #
###############################################################################

%package c128
Requires: %{name} = %{version}
License: Freeware
Summary: C128 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description c128
This package contains the header files and libraries needed to write
programs for the Commodore C128 using the cc65 crosscompiler.

%files c128
%attr(644,root,root) 		/usr/lib/cc65/lib/c128.lib
%attr(644,root,root) 		/usr/lib/cc65/emd/c128-*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/c128-*.joy
%attr(644,root,root) 		/usr/lib/cc65/mou/c128-*.mou
%attr(644,root,root) 		/usr/lib/cc65/ser/c128-*.ser
%attr(644,root,root) 		/usr/lib/cc65/tgi/c128-*.tgi



###############################################################################
#      	       	       	       	 C16 subpackage		   		      #
###############################################################################

%package c16
Requires: %{name} = %{version}
License: Freeware
Summary: C16/116 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description c16
This package contains the header files and libraries needed to write
programs for the Commodore C16/116 using the cc65 crosscompiler.

%files c16
%attr(644,root,root) 		/usr/lib/cc65/lib/c16.lib
%attr(644,root,root) 		/usr/lib/cc65/emd/c16-*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/c16-*.joy



###############################################################################
#		     		C64 subpackage	 	   		      #
###############################################################################

%package c64
Requires: %{name} = %{version}
License: Freeware
Summary: C64 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description c64
This package contains the header files and libraries needed to write
programs for the Commodore C64 using the cc65 crosscompiler.

%files c64
%attr(644,root,root) 		/usr/lib/cc65/lib/c64.lib
%attr(644,root,root) 		/usr/lib/cc65/emd/dtv-*.emd
%attr(644,root,root) 		/usr/lib/cc65/emd/c64-*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/c64-*.joy
%attr(644,root,root) 		/usr/lib/cc65/mou/c64-*.mou
%attr(644,root,root) 		/usr/lib/cc65/ser/c64-*.ser
%attr(644,root,root) 		/usr/lib/cc65/tgi/c64-*.tgi



###############################################################################
#		       	       CBM510 subpackage       			      #
###############################################################################

%package cbm510
Requires: %{name} = %{version}
License: Freeware
Summary: CBM 510 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description cbm510
This package contains the header files and libraries needed to write
programs for the Commodore CBM 510 (aka P500) using the cc65
crosscompiler.

%files cbm510
%attr(644,root,root)   	       	/usr/lib/cc65/lib/cbm510.lib
%attr(644,root,root) 	       	/usr/lib/cc65/emd/cbm510-*.emd
%attr(644,root,root) 	       	/usr/lib/cc65/joy/cbm510-*.joy
%attr(644,root,root) 	       	/usr/lib/cc65/ser/cbm510-*.ser



###############################################################################
#		       	       CBM610 subpackage       			      #
###############################################################################

%package cbm610
Requires: %{name} = %{version}
License: Freeware
Summary: CBM 610 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description cbm610
This package contains the header files and libraries needed to write
programs for the Commodore PET-II (CBM600/700) family of computers using
the cc65 crosscompiler.

%files cbm610
%attr(644,root,root)   	       	/usr/lib/cc65/lib/cbm610.lib
%attr(644,root,root) 	       	/usr/lib/cc65/emd/cbm610-*.emd
%attr(644,root,root) 	       	/usr/lib/cc65/ser/cbm610-*.ser



###############################################################################
#		 		GEOS subpackage	 			      #
###############################################################################

%package geos
Requires: %{name} = %{version}
License: Freeware
Summary: GEOS specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description geos
This package contains the header files and libraries needed to write
GEOS programs for the C64/C128 using the cc65 crosscompiler.

%files geos
%attr(755,root,root)   		/usr/bin/grc
%attr(644,root,root)		/usr/lib/cc65/lib/geos.lib
%attr(755,root,root) %dir	/usr/lib/cc65/include/geos
%attr(644,root,root)   		/usr/lib/cc65/include/geos/*.h
%attr(644,root,root) 		/usr/lib/cc65/emd/geos-*.emd
%attr(644,root,root) 		/usr/lib/cc65/joy/geos-*.joy
%attr(644,root,root) 		/usr/lib/cc65/tgi/geos-*.tgi



###############################################################################
#                               Lynx subpackage                               #
###############################################################################

%package lynx
Requires: %{name} = %{version}
License: Freeware
Summary: Lynx specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description lynx
This package contains the header files and libraries needed to write
programs for the Lynx Game Console using the cc65 crosscompiler.

%files lynx
%attr(644,root,root)		/usr/lib/cc65/lib/lynx.lib
%attr(644,root,root) 		/usr/lib/cc65/joy/lynx-*.joy
%attr(644,root,root) 		/usr/lib/cc65/ser/lynx-*.ser
%attr(644,root,root) 		/usr/lib/cc65/tgi/lynx-*.tgi



###############################################################################
#		       		NES subpackage	       		       	      #
###############################################################################

%package nes
Requires: %{name} = %{version}
License: Freeware
Summary: NES specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description nes
This package contains the header files and libraries needed to write
programs for the Nintendo Entertainment System using the cc65
crosscompiler.

%files nes
%attr(644,root,root)		/usr/lib/cc65/lib/nes.lib
%attr(644,root,root) 		/usr/lib/cc65/joy/nes-*.joy



###############################################################################
#		       		PET subpackage	       		       	      #
###############################################################################

%package pet
Requires: %{name} = %{version}
License: Freeware
Summary: PET specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description pet
This package contains the header files and libraries needed to write
programs for the Commodore PET family of computers using the cc65
crosscompiler.

%files pet
%attr(644,root,root)		/usr/lib/cc65/lib/pet.lib
%attr(644,root,root) 		/usr/lib/cc65/joy/pet-*.joy



###############################################################################
#		       	       Plus/4 subpackage 	    		      #
###############################################################################

%package plus4
Requires: %{name} = %{version}
License: Freeware
Summary: Plus/4 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description plus4
This package contains the header files and libraries needed to write
programs for the Commodore Plus/4 and C16/116 using the cc65
crosscompiler.

%files plus4
%attr(644,root,root)   		/usr/lib/cc65/lib/plus4.lib
%attr(644,root,root) 		/usr/lib/cc65/joy/plus4-*.joy
%attr(644,root,root) 		/usr/lib/cc65/ser/plus4-*.ser



###############################################################################
#                           Supervision subpackage                            #
###############################################################################

%package supervision
Requires: %{name} = %{version}
License: Freeware
Summary: Supervision specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description supervision
This package contains the header files and libraries needed to write
programs for the Supervision Game console using the cc65 crosscompiler.

%files supervision
%attr(644,root,root)		/usr/lib/cc65/lib/supervision.lib
%attr(644,root,root)		/usr/lib/cc65/cfg/supervision-*.cfg



###############################################################################
#		     	       VIC20 subpackage	  			      #
###############################################################################

%package vic20
Requires: %{name} = %{version}
License: Freeware
Summary: VIC20 specific libraries and headers for the cc65 compiler.
Group: Development/Languages

%description vic20
This package contains the header files and libraries needed to write
programs for the Commodore VIC20 using the cc65 crosscompiler.

%files vic20
%attr(644,root,root) 		/usr/lib/cc65/lib/vic20.lib
%attr(644,root,root) 		/usr/lib/cc65/joy/vic20-*.joy
%attr(644,root,root)   	       	/usr/lib/cc65/cfg/vic20-*.cfg



###############################################################################
#		      		     Build	 	    		      #
###############################################################################



%prep
%setup -q


%build
# Binaries
make -C src -f make/gcc.mak
# Libraries
make -C libsrc -s zap all
# Documentation
make -C doc html


%install
mkdir -p $RPM_BUILD_ROOT/usr/{bin,lib/cc65/{asminc,cfg,emd,include/{em,geos,joystick,mouse,sys,tgi},joy,lib,mou,obj,ser,tgi}}

# Binaries
install -s -m 755 src/ar65/ar65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/ca65/ca65 $RPM_BUILD_ROOT/usr/bin
install    -m 755 src/ca65html/ca65html $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/cc65/cc65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/cl65/cl65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/co65/co65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/da65/da65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/grc/grc $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/ld65/ld65 $RPM_BUILD_ROOT/usr/bin
install -s -m 755 src/od65/od65 $RPM_BUILD_ROOT/usr/bin

# Libraries and includes
install -m 644 libsrc/*.lib $RPM_BUILD_ROOT/usr/lib/cc65/lib
install -m 644 include/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include
install -m 644 include/geos/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/geos
install -m 644 include/em/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/em
install -m 644 include/joystick/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/joystick
install -m 644 include/mouse/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/mouse
install -m 644 include/sys/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/sys
install -m 644 include/tgi/*.h $RPM_BUILD_ROOT/usr/lib/cc65/include/tgi
install -m 644 asminc/*.inc $RPM_BUILD_ROOT/usr/lib/cc65/asminc

# Extra object files
for extra in libsrc/*-*.o; do
    install -m 644 ${extra} $RPM_BUILD_ROOT/usr/lib/cc65/obj
done

# Additional linker configurations
for config in src/ld65/cfg/*-*.cfg; do
    install -m 644 ${config} $RPM_BUILD_ROOT/usr/lib/cc65/cfg
done

# Drivers
install -m 644 libsrc/*.emd $RPM_BUILD_ROOT/usr/lib/cc65/emd
install -m 644 libsrc/*.joy $RPM_BUILD_ROOT/usr/lib/cc65/joy
install -m 644 libsrc/*.mou $RPM_BUILD_ROOT/usr/lib/cc65/mou
install -m 644 libsrc/*.ser $RPM_BUILD_ROOT/usr/lib/cc65/ser
install -m 644 libsrc/*.tgi $RPM_BUILD_ROOT/usr/lib/cc65/tgi


%clean
rm -rf $RPM_BUILD_ROOT



%changelog
* Thu Dec 23 2003 Ullrich von Bassewitz <uz@cc65.org>
- Changelog is now in CVS
                      
