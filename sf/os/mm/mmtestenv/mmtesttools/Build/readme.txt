Summary of files in this directory
==================================

Commands for developers
-----------------------

(these are perl-based, and include a .bat file to merely call the perl).

mmbuild.bat/.pl: Rebuild Multimedia in the current codeline (see below)
mmbuildrom.bat/.pl: Build a rom, possibly including Multimedia test files (see below)

Commands intended for test builds
---------------------------------

fullMmBuild.bat/.pl: Designed to be called from Tools/auto.bat to do full rebuild, as part of test build
winsMmBuild.bat/.pl: Similar but only build WINS - used for ODC integration
testExtras.bat: Build those things required by test build but not built by mmbuild
testExtras_ccov.bat: Similar to testExtras.bat, but for CCover builds
testExtras_clean.bat: Again similar, but used to remove the associated components

Note that the action carried out is configured by editing the .mbc files of the same name.

Usage of MmBuild and MmBuildRom
-------------------------------

MmBuild is intended to be a central place where any developer can rebuild the whole of the codeline, essentially as the test build - and thus similarly to the overnight build.

A standard sequence would be:

	mmbuild setup 		// work out which things to build and run bldmake
	mmbuild -t clean	// do reallyclean - -t means on test files *too*
	mmbuild build 		// build main code
	mmbuild -t build	// build test code - testframework, unit test etc

[ As an aside, the fullmmbuild command can be used to run the above sequence, as in:
	 fullmmbuild.bat -c > bld.log 2>&1
  where the -c does the clean stage - generally use for the first time you build from that
  area, and leave out subsequently].

By default, build runs through WINS, WINSCW, ARM4 and MCOT - for MCOT only the DevSound and device driver layers are built. You can give the target to build instead, just as you want with "abld build", except that it is not possible to only build udeb or urel. For EKA2, where WINS is not build as part of the DevBuild, the WINS stage is just an "export". 

There are some useful flags to be added to "setup", that change what happens on subsequent build/clean calls:
	-x	include "optional" modules
	-i	Build/Clean ICL only
	-m	Build/Clean MMF only

eg.

	mmbuild -x -m setup

Note that mmbuild works by generating some temporary files, so these options only need be given on setup.

MmBuildRom is similarly designed to build the rom. 

	mmbuildrom -t

will build a rom including all the non-optional multimedia software.

Again there are some useful options:
	-t	include test software 
	-x	include optional modules, and where -t is given, their tests
	-i	if -t given, only include icl-unit tests
	-m	if -t given, only include mmf-unit tests


Configuration files
-------------------

The following .mbc files are used by mmbuild to determine what is built, eg. so it knows what is the main ICL code, what is optional, and what is test.

Misc.mbc	Main "Misc" modules - potentially ECam
icl.mbc		Main ICL modules
mmf.mbc		Main MMF modules

MiscOpt.mbc/ICLOpt.mbc/MMFOpt.mbc: Optional Misc/ICL/MMF Modules respectively

MiscTest.mbc/ICLTest.mbc/MMFTest.mbc: Main Misc/ICL/MMF test code modules respectively

MiscOptTest.mbc/ICLOptTest.mbc/MMFOptTest.mbc: Optional Misc/ICL/MMF test code modules respectively

mmfNotOptTest.mbc: MMF test code for when mmfOptTest.mbc is not employed (currently mmf-only)

mmfPhys.mbc: What to build for MMF with real targets like MCOT
mmfPhysOpt.mbc: Optional things to build for real targets

It should be noted that the definition of "optional" v. "non-optional" is whether it is part of the current "standard" build - rather than whether a module is theorectically optional or not.

These .mbc files are intended to be maintained - particularly if new sub-components are added or when they cease to become "optional", etc.

testExtras.mbc and testExtras_ccov.mbc support the associated .bat files. They build test programs such as tmdavid, tmdaaud and timageviewer. They might also need maintaining if things change.

Temporary files
---------------

Please ignore and don't add to perforce

__main.mbc
__phys.mbc
__test.mbc
$NULL
