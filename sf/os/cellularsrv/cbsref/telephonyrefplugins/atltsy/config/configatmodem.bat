@echo off
REM Copyright (c) 2010 Symbian Foundation and/or its subsidiary(-ies).
REM All rights reserved.
REM This component and the accompanying materials are made available
REM under the terms of Eclipse Public License v1.0
REM which accompanies this distribution, and is available
REM at the URL http://www.eclipse.org/legal/epl-v10.html.
REM
REM Initial Contributors:
REM Symbian Foundation - initial contribution.
REM
REM Contributors:
REM
REM Description:
REM

echo Backup hctl_base.dll:
move /-Y \epoc32\release\winscw\udeb\hctl_base.dll \epoc32\release\winscw\udeb\hctl_base.bak

echo Copying UREL version of hctl_base.dll:
copy /Y \epoc32\release\winscw\urel\hctl_base.dll \epoc32\release\winscw\udeb\hctl_base.dll

echo Backup phonetsy.tsy:
move /-Y \epoc32\release\winscw\udeb\phonetsy.tsy \epoc32\release\winscw\udeb\phonetsy.bak

echo Copying phonetsywithdispatcher.tsy to phonetsy:
copy /Y \epoc32\release\winscw\udeb\phonetsywithdispatcher.tsy \epoc32\release\winscw\udeb\phonetsy.tsy

echo Setup the feature manager to include GSM and WCDMA features:
@perl -S -I/epoc32/tools/featmgr ./modifyfeaturedb.pl

echo Setup the Converged Call Engine plugin to use CS rather than VCC:
@perl -S ./modifycenrep.pl

echo Setup CommDB:
REM copy /Y \sf\os\cellularsrv\telephonyserverplugins\attestltsy\config\AT-LTSY(default).cfg \epoc32\winscw\c\AT-LTSY(default).cfg
cd \sf\os\cellularsrv\telephonyserverplugins\attestltsy\config
@perl -S ./modifycommdb.pl
\epoc32\release\winscw\udeb\ced.exe -Dtextshell -- c:\AT-LTSY.cfg

REM M:\sf\os\devicesrv\sysstatemgmt\group>sbs -b bld.inf -c winscw_udeb RESOURCE
