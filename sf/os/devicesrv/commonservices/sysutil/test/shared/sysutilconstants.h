/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* file sysutilconstants.h
* 
*
*/



#ifndef SYSUTILCONSTANTS_H
#define SYSUTILCONSTANTS_H

#include <e32base.h>

// UID of SysUtilSetup.exe
const TUid KSysUtilSetupUid = {0x10285B3B};

// Device Type Information attribute UIDs.
const TUid KManufacturerNameUid = {0x10286358};
const TUid KModelNameUid = {0x10286359};
const TUid KModelCodeUid = {0x1028635A};
const TUid KRevisionIDUid = {0x1028635B};
const TUid KDefaultDeviceNameUid = {0x1028635C};
const TUid KUIPlatformNameUid = {0x1028635D};
const TUid KUIPlatformVersionMajorUid = {0x1028635E};
const TUid KUIPlatformVersionMinorUid = {0x1028635F};
const TUid KUIPlatformVersionUid = {0x10286360};
const TUid KOSVersionMajorUid = {0x10286361};
const TUid KOSVersionMinorUid = {0x10286362};
const TUid KOSVersionUid = {0x10286363};
const TUid KDeviceCreatorAttributeUid = {0xE1000001};

// Publish and Subscribe property UIDs for the properties which contain 
// the device type and version information.
const TUid KSWVersionUid = {0x10285B3C};
const TUid KLangSWVersionUid = {0x10285B3D};
const TUid KLangVersionUid = {0x10285B3E};
const TUid KDeviceTypeInfoUid = {0x10285B3F};
const TUid KPRInformationUid = {0x20027BC4};

// Publish and Subscribe property used to turn test paths on and off
const TUid KTestFilePathsUid = {0x10285B40};

// File paths
_LIT(KDeviceAttributesIniPath, "Z:\\resource\\versions\\deviceattributes.ini");
_LIT(KDeviceAttributesIniTestPath, "Z:\\versions\\deviceattributes.ini");

#endif
