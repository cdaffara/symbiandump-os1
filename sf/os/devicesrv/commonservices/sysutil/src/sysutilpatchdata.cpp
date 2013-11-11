// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <e32base.h>

//all the constant value needs to be in a separate file
//Care should be taken not to define the exported data in the same source file 
//in which they are referred, lest the compiler inline's it. If inlined, the 
//data cannot be patched. Hence, the definition of data must be put in a 
//separate source file and they must be referred in other source files by 
//using 'extern' qualifier

/**
Patchable data used in the Sysutil framework. This patchable data should be
set to the drive letter of the MMC drive, in the range as specified by 
TDriveNumber enumeration. See sysutil.cpp for further details of the scheme 
used to configure this constant.
@see TDriveNumber
SysUtil::MMCSpaceBelowCriticalLevelL will use this specified drive to report 
on the disk levels.
@internalComponent
*/
EXPORT_C extern const TInt KSysUtilDefaultMMCDrive = -1;

/**
Patchable data used in the Sysutil framework. Threshold for
disk space critical level for a RAM drive. 
SysUtil::DiskSpaceBelowCriticalLevelL, SysUtil::FFSSpaceBelowCriticalLevelL or 
SysUtil::MMCSpaceBelowCriticalLevelL will use this level to determine if the 
disk level has reached the critical threshold. See sysutil.cpp for further 
details of the scheme used to configure this constant.
@internalComponent
*/
#ifdef __WINSCW__ // Set to valid value for WINSCW as no patching supported.
EXPORT_C extern const TInt KSysUtilRamDiskCriticalThreshold  = 65536;
#else
EXPORT_C extern const TInt KSysUtilRamDiskCriticalThreshold  = -1;
#endif

/**
Patchable data used in the Sysutil framework. Thres
SysUtil::DiskSpaceBelowCriticalLevelL, SysUtil::FFSSpaceBelowCriticalLevelL or 
SysUtil::MMCSpaceBelowCriticalLevelL will use this level to determine if the 
disk level has reached the critical threshold.. See sysutil.cpp for
further details of the scheme used to configure this constant.
@internalComponent
*/
#ifdef __WINSCW__ // Set to valid value for WINSCW as no patching supported.
EXPORT_C extern const TInt KSysUtilOtherDiskCriticalThreshold = 262144;
#else
EXPORT_C extern const TInt KSysUtilOtherDiskCriticalThreshold = -1;
#endif

/**
Patchable data used in the Sysutil framework. This patchable data should be
set to the drive letter of the FFS drive, in the range as specified by 
TDriveNumber enumeration. See sysutil.cpp for further details of the scheme 
used to configure this constant.
@see TDriveNumber
SysUtil::FFSSpaceBelowCriticalLevelL will use this specified drive to report on 
the disk levels.
@internalComponent
*/
EXPORT_C extern const TInt KSysUtilDefaultFFSDrive = -1;

/**
Patchable data used to turn off sysutilsetup.exe for the version APIs. By 
doing this caching of the version information will be turned off. This means 
the configuration files will be read each time the SysUtil version information 
APIs are called. Off/disabled is the default platform value to ensure backwards 
compatibility with earlier SysUtil implementations.
@internalComponent
*/
EXPORT_C extern const TInt KSysUtilDisableVersionSetupExe = ETrue;


/**
Patchable data used to turn off sysutilsetup.exe for the device type 
information API. By doing this two things will happen. Firstly, caching of 
the device type information will be turned off. This means the configuration 
files will be read each time the SysUtil device type information API is called. 
Secondly, the ability to use a plug-in will be disabled. This will limit the 
configuration of the device type information API to only the configuration 
files.
@internalComponent
*/
EXPORT_C extern const TInt KSysUtilDisableDeviceTypeInfoSetupExe = EFalse;

/**
Patchable data used to enable the ability of SysUtil to find it's text files in
the 'C:\versions\' folder rather than 'Z:\resource\versions\'. 
Once enabled the test paths can be switched on by setting a P&S property to 
ETrue in the system category (KUidSystemCategory) with a key of 0x10285B40. 
If the test paths are switched on then any of the TXT files, INI files and 
plug-in can be placed on the system drive and will be used instead of the 
equivalent files on the Z drive.

This constant also turns on the ability to use the -delete flag with 
SysUtilSetup.exe. This flag tells SysUtilSetup.exe to delete the Version 
and Device Type Information Publish and Subscribe properties.
@internalComponent
*/
#ifdef __WINSCW__ 
// Set to valid value for WINSCW since cannot patch WINSCW builds
// as there is no security threat posed by enabling this under the emulator.
EXPORT_C extern const TInt KSysUtilTestModeEnabled = ETrue;
#else
EXPORT_C extern const TInt KSysUtilTestModeEnabled = EFalse;
#endif
