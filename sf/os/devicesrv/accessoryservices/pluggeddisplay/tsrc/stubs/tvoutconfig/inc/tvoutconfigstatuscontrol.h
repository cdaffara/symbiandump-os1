/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 */

#ifndef TVOUTCONFIGSTATUSCONTROL_H
#define TVOUTCONFIGSTATUSCONTROL_H

// SYSTEM INCLUDE FILES
#include <e32std.h>

// USER INCLUDE FILES

// DATA TYPES

const TUid KTVOutStatusControlCategory = {0x0BC00001};
const TUid KTVOutControlCategory       = {0x0BC00002};

const TUint32 KTVOutStatusControlEnable       = 0x00000001; // Set to KErrNone for normal operation, any other to induce failure with KErrServerBusy code
const TUint32 KTVOutStatusControlDisable      = 0x00000002; // Set to KErrNone for normal operation, any other to induce failure with KErrServerBusy code
const TUint32 KTVOutStatusControlSetConfig    = 0x00000003; // Set to KErrNone for normal operation, KErrNotSupported, KErrNotFound or KErrNotReady to induce failure
const TUint32 KTVOutStatusControlCableConnect = 0x00000004; // Change this value to toggle HDMI cable connect status -> initially "not connected"
const TUint32 KTVOutStatusControlCopyProtect  = 0x00000005; // Set to 0 for "Copy Ptotection off", 1 for "Copy Ptotection on"
const TUint32 KTVOutStatusControlConfigChange = 0x00000006; // Change this value to indicate that TV Out stting has changed
const TUint32 KTVOutStatusControlConfigChangeImmediateError = 0x00000007; // CTvOutConfig::SettingsChangedListener() should fail immediately with this code.
const TUint32 KTVOutStatusControlAnalogCableConnect = 0x00000008;

// Set to update the Supported CEA mode
// 0 - All CEA modes defined in TFixedModeCea are supported
// Non-Zero - Bit mask represents the indices of TFixedModeCea
const TUint32 KTVOutControlCEAMode  = 0x00000001;
// Set to update the Supported DMT mode
// 0 - All DMT modes defined in TFixedModeDmt are supported
// Non-Zero - Bit mask represents the indices of TFixedModeDmt
const TUint32 KTVOutControlDMTMode  = 0x00000002;

// KTVOutControlHdcpSupport

#endif // TVOUTCONFIGSTATUSCONTROL_H
