REM t_graphics_config_inifiles.bat
REM Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
REM All rights reserved.
REM This component and the accompanying materials are made available
REM under the terms of "Eclipse Public License v1.0"
REM which accompanies this distribution, and is available
REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
REM 
REM Initial Contributors:
REM Nokia Corporation - initial contribution.
REM 
REM Contributors:
REM 
REM Description:
REM 
@echo off
setlocal


if /i "%1"=="uninstall" goto :uninstall
if /i "%1"=="install" goto :install

:help
	echo.
	echo Usage: t_graphics_config_inifiles.bat install/uninstall [multiple_screen] [xga/surfwin/csc_classic/cscnga] [changetracking]
	echo Where:
	echo multiple_screen: modifies epoc.ini to provide a second screen
	echo changetracking:  use changetracking mode in wserv, the default mode is non changetracking  
	echo xga              configure as an xga display
	echo csc_classic      enable screen capture without nga
	echo csc_nga          enable screen capture with nga
	echo.
	goto done


rem *** START STANDARD FILE-REPLACEMENT FUNCTIONS ***
:replace
	set DST=%1
	set SRC=%2
	echo INFO: Replacing %DST% with %SRC%
	copy /y %DST% %DST%.original >NUL
	if defined SRC (
		copy /y %SRC% %DST% >NUL
		if not exist %DST% echo ERROR: Copy failed!
	)
	goto :EOF
:restore
	echo INFO: Restoring %1 
	if not exist %1.original echo ERROR: %1.original does not exist
	copy /y %1.original %1 >NUL
	del %1.original >NUL
	goto :EOF
:replace_em
	if defined EMULATOR_DATA_DIR (
		call :replace %EMULATOR_DATA_DIR%\%1 %2
	) else (
		call :replace %EPOCROOT%epoc32\release\winscw\udeb\%1 %2
		call :replace %EPOCROOT%epoc32\release\winscw\urel\%1 %2
	)
	goto :EOF
:replace_em2
	if defined EMULATOR_DATA_DIR (
		call :replace %EMULATOR_DATA_DIR%\%1 %EMULATOR_DATA_DIR%\%2
	) else (
		call :replace %EPOCROOT%epoc32\release\winscw\udeb\%1 %EPOCROOT%epoc32\release\winscw\udeb\%2
		call :replace %EPOCROOT%epoc32\release\winscw\urel\%1 %EPOCROOT%epoc32\release\winscw\urel\%2
	)
	goto :EOF
:restore_em
	if defined EMULATOR_DATA_DIR (
		call :restore %EMULATOR_DATA_DIR%\%1
	) else (
		call :restore %EPOCROOT%epoc32\release\winscw\udeb\%1
		call :restore %EPOCROOT%epoc32\release\winscw\urel\%1
	)
	goto :EOF
rem *** END STANDARD FILE-REPLACEMENT FUNCTIONS ***

:install
rem *** Installation: first work out which options have been given ***

set OPT1=%2
set OPT2=%3
set OPT3=%4

set OPT_CHANGETRACKING=
if /i "%OPT1%"=="changetracking" set OPT_CHANGETRACKING=1
if /i "%OPT2%"=="changetracking" set OPT_CHANGETRACKING=1
if /i "%OPT3%"=="changetracking" set OPT_CHANGETRACKING=1

set OPT_MULTIPLE_SCREEN=
if /i "%OPT1%"=="multiple_screen" set OPT_MULTIPLE_SCREEN=1
if /i "%OPT2%"=="multiple_screen" set OPT_MULTIPLE_SCREEN=1
if /i "%OPT3%"=="multiple_screen" set OPT_MULTIPLE_SCREEN=1

set OPT_XGA=
if /i "%OPT1%"=="xga" set OPT_XGA=1
if /i "%OPT2%"=="xga" set OPT_XGA=1
if /i "%OPT3%"=="xga" set OPT_XGA=1

set OPT_EGLREF=
if /i "%OPT1%"=="egl_ref" set OPT_EGLREF=1
if /i "%OPT2%"=="egl_ref" set OPT_EGLREF=1
if /i "%OPT3%"=="egl_ref" set OPT_EGLREF=1

set OPT_SURFWIN=
set OPT_SURFWIN_64K=
if /i "%OPT1%"=="surfwin_color64k"  set OPT_SURFWIN=1 & set OPT_SURFWIN_64K=1
if /i "%OPT2%"=="surfwin_color64k"  set OPT_SURFWIN=1 & set OPT_SURFWIN_64K=1
if /i "%OPT3%"=="surfwin_color64k"  set OPT_SURFWIN=1 & set OPT_SURFWIN_64K=1
if /i "%OPT1%"=="surfwin_color16ma" set OPT_SURFWIN=1
if /i "%OPT2%"=="surfwin_color16ma" set OPT_SURFWIN=1
if /i "%OPT3%"=="surfwin_color16ma" set OPT_SURFWIN=1
if /i "%OPT1%"=="surfwin"           set OPT_SURFWIN=1
if /i "%OPT2%"=="surfwin"           set OPT_SURFWIN=1
if /i "%OPT3%"=="surfwin"           set OPT_SURFWIN=1

set OPT_FASTPATH=
set OPT_FASTPATH_ALPHA=
set OPT_OPTDISABLED=
set OPT_OPTFASTPATH=
set OPT_OPTMAX=
if /i "%OPT1%"=="alpha_fastpath"	set OPT_FASTPATH=1 & set OPT_FASTPATH_ALPHA=1
if /i "%OPT2%"=="alpha_fastpath"	set OPT_FASTPATH=1 & set OPT_FASTPATH_ALPHA=1
if /i "%OPT3%"=="alpha_fastpath"	set OPT_FASTPATH=1 & set OPT_FASTPATH_ALPHA=1
if /i "%OPT1%"=="nonalpha_fastpath" set OPT_FASTPATH=1
if /i "%OPT2%"=="nonalpha_fastpath" set OPT_FASTPATH=1
if /i "%OPT3%"=="nonalpha_fastpath" set OPT_FASTPATH=1
if /i "%OPT1%"=="optdisabled"		set OPT_OPTDISABLED=1
if /i "%OPT2%"=="optdisabled"		set OPT_OPTDISABLED=1
if /i "%OPT3%"=="optdisabled"		set OPT_OPTDISABLED=1
if /i "%OPT1%"=="optfastpath"		set OPT_OPTFASTPATH=1
if /i "%OPT2%"=="optfastpath"		set OPT_OPTFASTPATH=1
if /i "%OPT3%"=="optfastpath"		set OPT_OPTFASTPATH=1
if /i "%OPT1%"=="optmax"			set OPT_OPTMAX=1
if /i "%OPT2%"=="optmax"			set OPT_OPTMAX=1
if /i "%OPT3%"=="optmax"			set OPT_OPTMAX=1
 
set OPT_CSC=
set OPT_CSC_NGA=
if /i "%OPT1%"=="csc_classic"		set OPT_CSC=1
if /i "%OPT2%"=="csc_classic"		set OPT_CSC=1
if /i "%OPT3%"=="csc_classic"		set OPT_CSC=1
if /i "%OPT1%"=="csc_nga"			set OPT_CSC=1 & set OPT_CSC_NGA=1
if /i "%OPT2%"=="csc_nga"			set OPT_CSC=1 & set OPT_CSC_NGA=1
if /i "%OPT3%"=="csc_nga"			set OPT_CSC=1 & set OPT_CSC_NGA=1

set OPT_COLOR_64K=
set OPT_COLOR_16MA=
if /i "%OPT1%"=="color64k"			set OPT_COLOR_64K=1
if /i "%OPT2%"=="color64k"			set OPT_COLOR_64K=1
if /i "%OPT3%"=="color64k"			set OPT_COLOR_64K=1
if /i "%OPT1%"=="color16ma"			set OPT_COLOR_16MA=1
if /i "%OPT2%"=="color16ma"			set OPT_COLOR_16MA=1
if /i "%OPT3%"=="color16ma"			set OPT_COLOR_16MA=1


rem *** Installation part 2 - copy the chosen wsini.ini into place and update epoc.ini, making backups ***

set CT=
if defined OPT_CHANGETRACKING set CT=_changetracking


if defined OPT_XGA (
	call :replace %EPOC_INI% %EPOCROOT%epoc32\data\epoc_xga.ini
	if defined EPOC_HOST_CPU echo HostCPU %EPOC_HOST_CPU% >> %EPOC_INI%
	set WSINI=Z\graphics\wsini_integ_xga%CT%.ini
) else (
	call :replace %EPOC_INI% 
)

if not defined OPT_FASTPATH goto skip_fastpath
	if defined OPT_FASTPATH_ALPHA (
		if defined OPT_OPTDISABLED	set WSINI=Z\wstest\fastpath\wins\wsini_alphamode_optdisabled%CT%.ini
		if defined OPT_OPTFASTPATH	set WSINI=Z\wstest\fastpath\wins\wsini_alphamode_optfastpath%CT%.ini
		if defined OPT_OPTMAX		set WSINI=Z\wstest\fastpath\wins\wsini_alphamode_optmax%CT%.ini
	) else (
		if defined OPT_OPTDISABLED	set WSINI=Z\wstest\fastpath\wins\wsini_nonalphamode_optdisabled%CT%.ini
		if defined OPT_OPTFASTPATH	set WSINI=Z\wstest\fastpath\wins\wsini_nonalphamode_optfastpath%CT%.ini
		if defined OPT_OPTMAX		set WSINI=Z\wstest\fastpath\wins\wsini_nonalphamode_optmax%CT%.ini
	)
	echo INFO: Modifying %EPOC_INI% for fastpath tests
	echo. >> %EPOC_INI%
	echo SYMBIAN_GRAPHICS_USE_GCE >> %EPOC_INI%
	echo SYMBIAN_BASE_USE_GCE >> %EPOC_INI%
	echo STARTUPMODE 7 >> %EPOC_INI%
	echo _NewScreen_ >> %EPOC_INI%
	echo ScreenWidth 640 >> %EPOC_INI%
	echo ScreenHeight 240 >> %EPOC_INI%
	echo _NewScreen_ >> %EPOC_INI%
	echo ScreenWidth 640 >> %EPOC_INI%
	echo ScreenHeight 240 >> %EPOC_INI%
:skip_fastpath




if defined OPT_SURFWIN (
	set WSINI=Z\graphics\wsini_wsgcesurfwin_emulator_color16ma%CT%.ini
	if defined OPT_SURFWIN_64K	set WSINI=Z\graphics\wsini_wsgcesurfwin_emulator_color64k%CT%.ini
) 
if defined OPT_CSC (
	set WSINI=Z\wstest\wsini_csc_classic.ini
	if defined OPT_CSC_NGA	set WSINI=Z\wstest\wsini_csc_nga.ini
) 

if not defined WSINI (
	set WSINI=Z\graphics\wsini_integ%CT%.ini
	if defined OPT_COLOR_16MA	set WSINI=Z\graphics\wsini_integ_color16ma%CT%.ini
	if defined OPT_COLOR_64K	set WSINI=Z\graphics\wsini_integ_color64k%CT%.ini
)

call :replace_em2 Z\system\data\wsini.ini %WSINI%


echo. >> %EPOC_INI%
if defined OPT_EGLREF echo symbian_graphics_use_egl_ref on >> %EPOC_INI%
if not defined OPT_MULTIPLE_SCREEN goto skip_multiple_screen
if defined OPT_SURFWIN echo StartupMode 7 >> %EPOC_INI%
echo _NewScreen_ >> %EPOC_INI%
if defined OPT_XGA (
	echo ScreenWidth 1024 >> %EPOC_INI% 
	echo ScreenHeight 768 >> %EPOC_INI%
	echo ScreenOffsetX 143 >> %EPOC_INI%
	echo ScreenOffsetY 163 >> %EPOC_INI%
	echo fasciabitmap  epocbig.bmp >> %EPOC_INI%
	goto skip_multiple_screen
) 
echo ScreenWidth 640 >> %EPOC_INI%
echo ScreenHeight 240 >> %EPOC_INI%
:skip_multiple_screen



echo =======================================
echo wsini.ini:
echo =======================================
type %WSINI%
echo =======================================
echo epoc.ini:	%EPOC_INI%
echo =======================================
type %EPOC_INI%
echo =======================================

goto done



:uninstall
	call :restore_em Z\system\data\wsini.ini
	call :restore    %EPOC_INI%
	goto done

:done

endlocal
