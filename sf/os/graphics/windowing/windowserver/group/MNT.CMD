@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem
@echo off
set _main_ver=00490
set _main_ver_sub=114869
set _ver=151
set _prefix=GT
set _prefixN=%_prefix%_%_main_ver%
set ZipPrefix=%_prefix%
set _ve32toolp=113
set _ve32tools=123
set _ve32utl=035
set _vgditools=052
set _vsdk=014

set _ve32=189
set _vf32=148
set _vstore=062
set _vgdi=071
set _vfntstore=060
set _vfbserv=093
set _vbitgdi=097
if "%1"=="?" goto help
if "%1"=="" goto help
if "%EPOCROOT%"=="" echo error: Variable Epocroot must be defined
if "%EPOCROOT%"=="" goto help
goto %1
goto help

:rombld
rombuild -slog %2.oby
goto end

:bldass
copy ..\rom\wserv.oby %EPOCROOT%epoc32\rom\include\WSERV.OBY
copy ..\rom\wservtest.iby %EPOCROOT%epoc32\rom\include\WSERVTest.IBY
cd %EPOCROOT%epoc32\rom
call tools\buildrom assabet wserv
copy ab_001.engbuild.img h:\sys$rom.bin
goto endgroup

:bldint
call prj group
copy ..\rom\wserv.oby %EPOCROOT%epoc32\rom\include\WSERV.OBY
copy ..\rom\wservtest.iby %EPOCROOT%epoc32\rom\include\WSERVTest.IBY
cd %EPOCROOT%epoc32\rom
call tools\buildrom int wserv
copy v1_001.engbuild.img def.img
dir *.img
goto end


:imgerror1
echo Invalid target machine specified
goto end
:imgerror2
echo invalid build specified
goto end

:ver
echo Current Mainline version number %_main_ver% @%_main_ver_sub%
echo Current release version number %_ver%
echo.
grep VersionNumber= ..\SERVER\w32cmd.h
echo.
grep \? ..\group\rel.txt
goto end

rem ================ Getting source and build files ====================

:getzip
echo About to delete old zips
pause
del \%_prefix%*.log
del \%_prefix%*.zip
rem del \refui_wins.zip
:getzipn
set _dirname=
if "%2"=="P" set _dirname=_PENDING_RELEASE
if "%2"=="p" set _dirname=_PENDING_RELEASE
if "%2"=="R" set _dirname=_roms_patched
if "%2"=="r" set _dirname=_roms_patched
rem cd r:\mainline\%_main_ver%%_dirname%
cd r:\mainline
if not exist r:%_main_ver%%_dirname%\zips\nul goto tryname1
cd r:%_main_ver%%_dirname%
goto foundname
:tryname1
set _dirname=_PENDING_RELEASE
if not exist r:%_main_ver%%_dirname%\zips\nul goto tryname2
cd r:%_main_ver%%_dirname%
goto foundname
:tryname2
set _dirname=
if not exist r:%_main_ver%%_dirname%\zips\nul goto tryname3
cd r:%_main_ver%%_dirname%
goto foundname
:tryname3
echo Cannot find directory for build %_main_ver%
goto end
:foundname
cd r:zips
echo About to get zip files for build %_main_ver%
pause
echo Copying Build Log %2
copy R:..\logs\%_prefixN%_preamble.log + R:..\logs\%_prefixN%_build.log \GT_%_main_ver%_Build.LOG
echo Copying %_prefix%_arm4.zip
copy r:%_prefix%_arm4.zip \
echo Copying %_prefix%_armi.zip
copy r:%_prefix%_armi.zip \
echo Copying %_prefix%_thumb.zip
copy r:%_prefix%_thumb.zip \
rem echo Copying RefUI_wins.zip
rem copy r:RefUI_wins.zip \
rem goto endgroup
echo Copying %_prefix%_libs.zip
copy r:%_prefix%_libs.zip \
echo Copying %_prefix%_wins.zip
copy r:%_prefix%_wins.zip \
rem echo Copying RefUI_wins.zip
rem copy RefUI_wins.zip H:\
goto end

:unzip
:getline
if exist \%_prefix%_libs.zip goto dogetline
echo No baseline present!
goto end
:dogetline
echo About to get SDK
set file=%_prefix%_libs
call prj group
call mnt getsdk
rem set file=RefUI_wins
cd \
unzip %file% epoc32\gcc\*
rem unzip -C baseline epoc32\include\*
:getline1
if "%file%"=="" set file=%_prefix%_libs
cd \
unzip -C %file% epoc32\rom\*
unzip -C %file% epoc32\tools\*
:getcomps
cd \
rem set _build=winc
call gr %_build% e32utils e32 hal f32 loce32 stdlib store bafl dbms
call gr %_build% series5mx1 assabetb integratorap bootstrap c32 esock commdb configdb debugger_stub
call gr %_build% tcpip dial charconv sysagent gdi fntstore fbserv bitgdi screendriver
if not exist  %EPOCROOT%epoc32\wins\nul md  %EPOCROOT%epoc32\WINS
if not exist  %EPOCROOT%epoc32\wins\c\nul md  %EPOCROOT%epoc32\wins\C
set file=
goto endgroup

:getwline
call gr %_build% wserv
goto end

:integ
cd \wserv_dev
p4 integ -b DougF_Dev_Main -s ...
cd \wserv
p4 revert ...\distribution.policy
p4 revert ...\*.uid.cpp
goto endgroup

:oldmakework
if not exist  %EPOCROOT%epoc32\nul md  %EPOCROOT%epoc32
if not exist  %EPOCROOT%epoc32\build\nul md  %EPOCROOT%epoc32\build
if not exist  %EPOCROOT%epoc32\build\wserv\nul md  %EPOCROOT%epoc32\build\wserv
if not exist  %EPOCROOT%epoc32\build\wserv\wins\nul md  %EPOCROOT%epoc32\build\wserv\wins
if not exist  %EPOCROOT%epoc32\build\wserv\wins\udeb\nul md  %EPOCROOT%epoc32\build\wserv\wins\udeb
if not exist  %EPOCROOT%epoc32\build\wserv\wins\urel\nul md  %EPOCROOT%epoc32\build\wserv\wins\urel
if not exist  %EPOCROOT%epoc32\build\wserv\winsd\nul md  %EPOCROOT%epoc32\build\wserv\winsd
if not exist  %EPOCROOT%epoc32\build\wserv\winsd\udeb\nul md  %EPOCROOT%epoc32\build\wserv\winsd\udeb
if not exist  %EPOCROOT%epoc32\build\wserv\winsd\urel\nul md  %EPOCROOT%epoc32\build\wserv\winsd\urel
if not exist  %EPOCROOT%epoc32\build\wserv\marm\nul md  %EPOCROOT%epoc32\build\wserv\marm
if not exist  %EPOCROOT%epoc32\build\wserv\marm\udeb\nul md  %EPOCROOT%epoc32\build\wserv\marm\udeb
if not exist  %EPOCROOT%epoc32\build\wserv\marm\urel\nul md  %EPOCROOT%epoc32\build\wserv\marm\urel
if not exist  %EPOCROOT%epoc32\build\wserv\marmd\nul md  %EPOCROOT%epoc32\build\wserv\marmd
if not exist  %EPOCROOT%epoc32\build\wserv\marmd\udeb\nul md  %EPOCROOT%epoc32\build\wserv\marmd\udeb
if not exist  %EPOCROOT%epoc32\build\wserv\marmd\urel\nul md  %EPOCROOT%epoc32\build\wserv\marmd\urel
goto end

:makework
:makeworkall
call prj bwins
call makmake ..\group\WSERV wins
nmake -f wserv.wins makework
call makmake ..\group\W32 wins
nmake -f w32.wins makework
call makmake ..\group\WSEXE wins
nmake -f wsexe.wins makework
call makmake ..\group\DEBLOG wins
nmake -f deblog.wins makework
call makmake ..\group\DEBLOGFL wins
nmake -f deblogfl.wins makework
call makmake ..\group\DEBLOGWN wins
nmake -f deblogwn.wins makework
call makmake ..\group\DEBLOGSR wins
nmake -f deblogsr.wins makework
call makmake ..\group\TLIB wins
nmake -f tlib.wins makework
call makmake ..\group\SHELL wins
nmake -f shell.wins makework
call makmake ..\group\CONNECT wins
nmake -f connect.wins makework
call makmake ..\group\HANDANIM wins
nmake -f handanim.wins makework
call makmake ..\group\AUTO wins
nmake -f auto.wins makework
call makmake ..\group\AUTODLL wins
nmake -f autodll.wins makework
call makmake ..\group\TMAN wins
nmake -f tman.wins makework
if !%1==!makework goto end
:makeworkother
call makmake ..\group\TTIME wins
nmake -f ttime.wins makework
call makmake ..\group\ANIM wins
nmake -f anim.wins makework
call makmake ..\group\ANIMDLL wins
nmake -f animdll.wins makework
goto end

:getbld
:getcomp
:getcomps
if not exist  %EPOCROOT%epoc32\nul md  %EPOCROOT%epoc32
if not exist  %EPOCROOT%epoc32\tools md  %EPOCROOT%epoc32\tools
if not exist  %EPOCROOT%epoc32\data md  %EPOCROOT%epoc32\data
if not exist  %EPOCROOT%epoc32\wins md  %EPOCROOT%epoc32\wins
if not exist  %EPOCROOT%epoc32\wins\c md  %EPOCROOT%epoc32\wins\c
if not exist  %EPOCROOT%epoc32\wins\c\data\nul md  %EPOCROOT%epoc32\wins\c\data
if not exist  %EPOCROOT%epoc32\include\nul md  %EPOCROOT%epoc32\include
if not exist  %EPOCROOT%epoc32\release\nul md  %EPOCROOT%epoc32\release
if not exist  %EPOCROOT%epoc32\release\wins\nul md  %EPOCROOT%epoc32\release\wins
if not exist  %EPOCROOT%epoc32\release\wins\udeb\nul md  %EPOCROOT%epoc32\release\wins\udeb
if not exist  %EPOCROOT%epoc32\release\wins\urel\nul md  %EPOCROOT%epoc32\release\wins\urel
if not exist  %EPOCROOT%epoc32\release\marm\nul md  %EPOCROOT%epoc32\release\marm
if not exist  %EPOCROOT%epoc32\release\marm\udeb\nul md  %EPOCROOT%epoc32\release\marm\udeb
if not exist  %EPOCROOT%epoc32\release\marm\urel\nul md  %EPOCROOT%epoc32\release\marm\urel
goto end

:getsdk
set _group=sdk
set _grver=%_vsdk%
if not exist  %EPOCROOT%epoc32\data\nul md  %EPOCROOT%epoc32\DATA
cd  %EPOCROOT%epoc32\data
set _drv=C:
call ..\group\getrel %_group% sdmodels %_grver%
set _drv=
goto endgroup

:getsrc
setlocal
set p4client=Lon-DougF_Main_Source
p4 sync %3 //EPOC/Main/generic/.../%2/...@%_main_ver_sub%
endlocal
goto end

:getsrctip
setlocal
set p4client=Lon-DougF_Main_Source
p4 sync %3 //EPOC/Main/generic/%2/...
endlocal
goto end


rem ===============Comparing Freeze Files===================

:def
REM b  %EPOCROOT%epoc32\build\wserv\winsd\deb\tanimd.def \wserv\bwins\wsanim.def
copy  %EPOCROOT%epoc32\build\wserv\winsd\deb\tanimd.def \wserv\bwins
goto end

:difdefs
if exist ..\ws32.lis del ..\ws32.lis
defmake  %EPOCROOT%epoc32\release\wins\rel\ws32.dll  %EPOCROOT%epoc32\release\wins\rel\ws32.def
vdiff ..\bwins\ws32.def  %EPOCROOT%epoc32\release\wins\rel\ws32.def > ..\ws32.lis
defmake  %EPOCROOT%epoc32\release\wins\deb\ws32.dll  %EPOCROOT%epoc32\release\wins\deb\ws32.def
vdiff ..\bwins\ws32d.def  %EPOCROOT%epoc32\release\wins\deb\ws32.def >> ..\ws32.lis
goto end

:bdefs
defmake  %EPOCROOT%epoc32\release\wins\rel\ws32.dll  %EPOCROOT%epoc32\release\wins\rel\ws32.def
b ..\bwins\ws32.def  %EPOCROOT%epoc32\release\wins\rel\ws32.def
goto end
:bdefsm
dlltool --output-def ws32.def  %EPOCROOT%epoc32\build\wserv\marmd\rel\ws32.in
b ..\bmarm\ws32.frz  %EPOCROOT%epoc32\release\marm\rel\ws32.def
goto end


rem ================ Run test code ===============

:run
:runudeb
set _rdir_=udeb
call ..\group\mnt dorun %2 %3
goto end

:runurel
set _rdir_=urel
call ..\group\mnt dorun %2 %3
goto end

:dorun
call prj test
call ..\group\mnt xrun epoc %2 %3
goto end

:xrun
start  %EPOCROOT%epoc32\release\wins\%_rdir_%\%2
set _rdir_=
goto end

rem ================ Quick backup to zipping to the U drive

:backup
if exist u:\netx.bug goto do_it
echo Death! Death! no network to back up to
goto end
:do_it
call prj group
if %2?==? goto update
call kill u:\wserv\bak.zip
pkzip u:\wserv\bak.zip \wserv\*.* -wHS -r -p -x@exclude.prj
attrib \wserv\*.* -A /S
goto end
:update
pkzip u:\wserv\bak.zip \wserv\*.* -i -wHS -r -p -x@exclude.prj
goto end

rem ================ Install ==============

:install
echo Replacing WSINI.INI files with test versions.
call :doinstall wins udeb wservu
call :doinstall wins urel wservu
call :doinstall winscw udeb wservu_multiscreen
call :doinstall winscw urel wservu_multiscreen

rem If EKA1 then finish because EKA1 does not support multiscreen.
if exist %EPOCROOT%epoc32\release\wins\udeb\auto.dll goto :EOF
if exist %EPOCROOT%epoc32\release\wins\urel\auto.dll goto :EOF

echo Backing up EPOC.INI.
if not exist %EPOCROOT%epoc32\data\epoc.bak copy %EPOCROOT%epoc32\data\epoc.ini %EPOCROOT%epoc32\data\epoc.bak >nul

rem add line to epoc.ini so that the system starter will start the emulator in wshell mode
echo. >> %EPOCROOT%epoc32\data\epoc.ini
echo StartupMode 7 >> %EPOCROOT%epoc32\data\epoc.ini

echo Editing EPOC.INI to use multiscreen.
echo _NewScreen_ >> %EPOCROOT%epoc32\data\epoc.ini
echo ScreenWidth 640 >> %EPOCROOT%epoc32\data\epoc.ini
echo ScreenHeight 240 >> %EPOCROOT%epoc32\data\epoc.ini
goto :EOF


:doinstall
rem Back up WSINI.INI
if not exist %EPOCROOT%epoc32\release\%1\%2\z\system\data\wsini.bak if exist %EPOCROOT%epoc32\release\%1\%2\z\system\data\wsini.ini copy %EPOCROOT%epoc32\release\%1\%2\z\system\data\wsini.ini %EPOCROOT%epoc32\release\%1\%2\z\system\data\wsini.bak >nul
if not exist %EPOCROOT%epoc32\release\%1\%2\z\resource\data\wsini.bak if exist %EPOCROOT%epoc32\release\%1\%2\z\resource\data\wsini.ini copy %EPOCROOT%epoc32\release\%1\%2\z\resource\data\wsini.ini %EPOCROOT%epoc32\release\%1\%2\z\resource\data\wsini.bak >nul

rem Back up ECONS.DLL
if exist %EPOCROOT%epoc32\release\%1\%2\econs.dll xcopy /r /i /y %EPOCROOT%epoc32\release\%1\%2\econs.dll %EPOCROOT%epoc32\release\%1\%2\econs-temp.dll

rem Rename WSECONS.DLL to ECONS.DLL
if exist %EPOCROOT%epoc32\release\%1\%2\wsecons.dll xcopy /r /i /y %EPOCROOT%epoc32\release\%1\%2\wsecons.dll %EPOCROOT%epoc32\release\%1\%2\econs.dll

if "%4"=="nonnga" goto :dononngainstall

rem Replace WSINI.INI with test version
if exist %EPOCROOT%epoc32\release\%1\%2\z\system\data\wsini.ini xcopy /r /y %~dp0%3.ini %EPOCROOT%epoc32\release\%1\%2\z\system\data\wsini.ini >nul
if exist %EPOCROOT%epoc32\release\%1\%2\z\resource\data\wsini.ini xcopy /r /y %~dp0%3.ini %EPOCROOT%epoc32\release\%1\%2\z\resource\data\wsini.ini >nul
goto :EOF

rem ================ Uninstall ==============

:uninstall
echo Restoring WSINI.INI files.
call :douninstall wins udeb
call :douninstall wins urel
call :douninstall winscw udeb
call :douninstall winscw urel

rem If EKA1 then finish because EKA1 does not support multiscreen.
if exist %EPOCROOT%epoc32\release\wins\udeb\auto.dll goto :EOF
if exist %EPOCROOT%epoc32\release\wins\urel\auto.dll goto :EOF

echo Restoring EPOC.INI.
if exist %EPOCROOT%epoc32\data\epoc.bak xcopy /r /y %EPOCROOT%epoc32\data\epoc.bak %EPOCROOT%epoc32\data\epoc.ini >nul
goto :EOF

echo Restoring backedup ECONS.DLL
if exist %EPOCROOT%epoc32\release\%1\%2\econs-temp.dll xcopy /r /i /y %EPOCROOT%epoc32\release\%1\%2\econs-temp.dll %EPOCROOT%epoc32\release\%1\%2\econs.dll
goto :EOF

:douninstall
rem Restore WSINI.INI
if exist %EPOCROOT%epoc32\release\%1\%2\z\system\data\wsini.bak   xcopy /r /y %EPOCROOT%epoc32\release\%1\%2\z\system\data\wsini.bak   %EPOCROOT%epoc32\release\%1\%2\z\system\data\wsini.ini >nul
if exist %EPOCROOT%epoc32\release\%1\%2\z\resource\data\wsini.bak xcopy /r /y %EPOCROOT%epoc32\release\%1\%2\z\resource\data\wsini.bak %EPOCROOT%epoc32\release\%1\%2\z\resource\data\wsini.ini >nul
goto :EOF

rem =================================================

:absolutefadingtest
call mnt install
copy ..\group\absolutefading.ini %EPOCROOT%epoc32\release\wins\udeb\z\private\10003b20\wsini.ini >NUL
copy \wserv\group\absolutefading.ini %EPOCROOT%epoc32\release\wins\urel\z\system\data\wsini.ini >NUL
goto end

rem ================ Clean up temp files ==============

:cleanup
xdel ..\*.err /ns
xdel ..\*.lis /ns
xdel ..\*.log /ns
goto end

rem ================ Check for files not in li.prj ==============

:check
@perl -w c:\bin\CHECKADD.PL wserv %2 %3 %4 %5 %6 %7 %8 %9
goto end


rem ================ SWAP! =================

:bsw
:bswap
:swb
:swapb
if not exist %EPOCROOT%epoc32\release\wins\udeb\bak\null goto dobackswap
echo Already done!!!
goto end
:dobackswap
cd %EPOCROOT%epoc32\release\wins
md udeb\BAK
md urel\BAK
rem cd ..\marm
rem md udeb\BAK
rem md urel\BAK
cd %EPOCROOT%epoc32\release\wins
copy udeb\ws*.* udeb\bak
copy urel\ws*.* urel\bak
rem cd ..\marm
rem copy udeb\ws*.* udeb\bak
rem copy urel\ws*.* urel\bak
goto endgroup

:swap
if exist %EPOCROOT%epoc32\release\marm\urel\bak\wserv.exe goto doswap
echo Not done a backup!!
goto end
:doswap
cd %EPOCROOT%epoc32\release\wins
xcopy urel\bak\*.* udeb
xcopy udeb\bak\*.* urel
cd ..\marm
xcopy urel\bak\*.* udeb
xcopy udeb\bak\*.* urel
goto endgroup

:swapres
:unswap
:swapr
if exist %EPOCROOT%epoc32\release\marm\urel\bak\wserv.exe goto doswaprestore
echo Not done a backup!!
goto end
:doswaprestore
cd %EPOCROOT%epoc32\release\wins
xcopy udeb\bak\*.* udeb
xcopy urel\bak\*.* urel
cd ..\marm
xcopy udeb\bak\*.* udeb
xcopy urel\bak\*.* urel
goto endgroup

:reverse
if exist %EPOCROOT%epoc32\release\wins\urel\bak\wserv.dll goto dorev
echo Not done a backup!!
goto end
:dorev
cd %EPOCROOT%epoc32\release\wins
if not exist %EPOCROOT%epoc32\release\wins\urel\bak1\wserv.dll goto dorev2
erase /S /Q udeb\bak1
rmdir udeb\bak1
erase /S /Q  urel\bak1
rmdir urel\bak1
:dorev2
rename udeb\bak Bak1
rename urel\bak Bak1
call mnt bsw
cd %EPOCROOT%epoc32\release\wins
xcopy udeb\bak1\*.* udeb
xcopy urel\bak1\*.* urel
goto endgroup
erase /S /Q udeb\bak1
rmdir udeb\bak1
erase /S /Q  urel\bak1
rmdir urel\bak1
goto endgroup

:export
copy ..\inc\w32*.h %EPOCROOT%epoc32\include
copy ..\bwins\wsanimu.def %EPOCROOT%epoc32\release\wins
copy ..\bmarm\wsanimu.def %EPOCROOT%epoc32\release\marm
goto end

:epoc32a
xcopy /s /v %EPOCROOT%epoc32a\build\wserv %EPOCROOT%epoc32\build\WSERV
xcopy /s /v %EPOCROOT%epoc32a\make\wserv %EPOCROOT%epoc32\make\WSERV
xcopy /s /v %EPOCROOT%epoc32a\release\arm4\udeb\ws*.*  %EPOCROOT%epoc32\release\arm4\udeb
xcopy /s /v %EPOCROOT%epoc32a\release\arm4\urel\ws*.*  %EPOCROOT%epoc32\release\arm4\urel
xcopy /s /v %EPOCROOT%epoc32a\release\armi\udeb\ws*.*  %EPOCROOT%epoc32\release\armi\udeb
xcopy /s /v %EPOCROOT%epoc32a\release\armi\urel\ws*.*  %EPOCROOT%epoc32\release\armi\urel
xcopy /s /v %EPOCROOT%epoc32a\release\thumb\udeb\ws*.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v %EPOCROOT%epoc32a\release\thumb\urel\ws*.*  %EPOCROOT%epoc32\release\thumb\urel

xcopy /s /v %EPOCROOT%epoc32a\release\armi\urel\auto.*  %EPOCROOT%epoc32\release\armi\urel
xcopy /s /v %EPOCROOT%epoc32a\release\armi\urel\conn.*  %EPOCROOT%epoc32\release\armi\urel
xcopy /s /v %EPOCROOT%epoc32a\release\armi\urel\dlog*.*  %EPOCROOT%epoc32\release\armi\urel
xcopy /s /v %EPOCROOT%epoc32a\release\armi\urel\shell.*  %EPOCROOT%epoc32\release\armi\urel
xcopy /s /v %EPOCROOT%epoc32a\release\armi\urel\tlib.*  %EPOCROOT%epoc32\release\armi\urel
xcopy /s /v %EPOCROOT%epoc32a\release\armi\urel\tman.*  %EPOCROOT%epoc32\release\armi\urel

xcopy /s /v %EPOCROOT%epoc32a\release\thumb\udeb\anidll.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\udeb\auto*.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\udeb\conn.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\udeb\dlog*.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\udeb\econs.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\udeb\jotanim.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\udeb\mbmanim.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\udeb\shell.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\udeb\tlib.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\udeb\tman.*  %EPOCROOT%epoc32\release\thumb\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\anidll.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\auto*.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\conn.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\dlog*.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\econs.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\jotanim.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\mbmanim.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\shell.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\tlib.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\thumb\urel\tman.*  %EPOCROOT%epoc32\release\thumb\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\anidll.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\auto*.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\conn.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\dlog*.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\econs.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\epoc.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\mbmanim.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\shell.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\tlib.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\udeb\tman.*  %EPOCROOT%epoc32\release\wins\udeb
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\anidll.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\auto*.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\conn.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\dlog*.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\econs.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\epoc.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\jotanim.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\mbmanim.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\shell.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\tlib.*  %EPOCROOT%epoc32\release\wins\urel
xcopy /s /v  %EPOCROOT%epoc32a\release\wins\urel\tman.*  %EPOCROOT%epoc32\release\wins\urel
goto end

:depoc32a
del /f /s /q  %EPOCROOT%epoc32a\build\wserv
del /f /s /q  %EPOCROOT%epoc32a\make\wserv
del /f /q  %EPOCROOT%epoc32a\release\arm4\udeb\ws*.*
del /f /q  %EPOCROOT%epoc32a\release\arm4\urel\ws*.*
del /f /q  %EPOCROOT%epoc32a\release\armi\udeb\ws*.*
del /f /q  %EPOCROOT%epoc32a\release\armi\urel\ws*.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\ws*.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\ws*.*

del /f /q  %EPOCROOT%epoc32a\release\armi\urel\auto.*
del /f /q  %EPOCROOT%epoc32a\release\armi\urel\conn.*
del /f /q  %EPOCROOT%epoc32a\release\armi\urel\dlog*.*
del /f /q  %EPOCROOT%epoc32a\release\armi\urel\shell.*
del /f /q  %EPOCROOT%epoc32a\release\armi\urel\tlib.*
del /f /q  %EPOCROOT%epoc32a\release\armi\urel\tman.*

del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\anidll.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\auto*.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\conn.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\dlog*.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\econs.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\jotanim.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\mbmanim.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\shell.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\tlib.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\udeb\tman.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\anidll.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\auto*.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\conn.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\dlog*.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\econs.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\jotanim.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\mbmanim.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\shell.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\tlib.*
del /f /q  %EPOCROOT%epoc32a\release\thumb\urel\tman.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\anidll.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\auto*.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\conn.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\dlog*.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\econs.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\epoc.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\mbmanim.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\shell.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\tlib.*
del /f /q  %EPOCROOT%epoc32a\release\wins\udeb\tman.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\anidll.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\auto*.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\conn.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\dlog*.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\econs.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\epoc.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\jotanim.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\mbmanim.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\shell.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\tlib.*
del /f /q  %EPOCROOT%epoc32a\release\wins\urel\tman.*
goto end


rem ================ HELP! =================

:help
echo Parameters are:
echo BACKUP    to backup to U:\wserv
echo GETZIP    to get the zip files for the current baseline
echo GETLINE   to extract all the components we depend on
echo GETSRC {comp}  to get the source of another component
rem echo -----------------------------------------------------------------------
echo DELBLD    to delete all work files for the current build
echo CHECK     to check that all the files in the group are in a .PRJ file
echo ARMUDEB   to build unicode debug code for ARM debug
goto end

:endgroup
call cont wserv group
:end
set _main_ver=
set _main_ver_sub=
set _prefix=
set _prefixN=
set _ver=
set _ve32tools=
set _ve32toolp=
set _ve32utl=
set _vgditools=
set _ve32=
set _vf32=
set _vgdi=
set _vfntstore=
set _vfbserv=
set _vbitgdi=
set _vstore=
set _vsdk=
set _par1=
set _par2=
