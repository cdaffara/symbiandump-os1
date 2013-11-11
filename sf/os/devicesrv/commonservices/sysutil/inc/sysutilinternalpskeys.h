/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef SYSUTILINTERNALPSKEYS_H
#define SYSUTILINTERNALPSKEYS_H

#include <e32property.h>

/**
UID for Disk level. This is not the preferred method for configuring the
critical disk level, but has been left in for backwards compatibility.
The preferred method is described here:
@see SysUtil::DiskSpaceBelowCriticalLevelL

If the preferred method is not used, that is the patchable data has not been set 
then the Publish & Subscribe keys will be tried, or if that fails,
from Central Repository. An error code will be returned if all fail.
@publishedPartner
@deprecated
**/
const TUid KPSUidDiskLevel = { 0x10205065 };

// =============================================================================
// Critical Disk Level Configuration API
//
// This API will be removed in the future, when Disk and Memory Level Keys API
// can be used also during back-up and restore.
// =============================================================================

/**
Threshold for RAM disk space critical level (bytes). This is not the preferred method
for configuring the critical disk level, but has been left in for backwards compatibility.
The preferred method is described here:
@see SysUtil::DiskSpaceBelowCriticalLevelL

If the preferred method is not used, that is the patchable data has not been set 
then the Publish & Subscribe keys will be tried, or if that fails,
from Central Repository. An error code will be returned if all fail.
@publishedPartner
@deprecated
**/
const TUint32 KRamDiskCriticalThreshold = 0x00000001;

/**
Threshold for disk space critical level (bytes). This is not the preferred method
for configuring the critical disk level, but has been left in for backwards compatibility.
The preferred method is described here:
@see SysUtil::DiskSpaceBelowCriticalLevelL

If the preferred method is not used, that is the patchable data has not been set 
then the Publish & Subscribe keys will be tried, or if that fails,
from Central Repository. An error code will be returned if all fail.
@publishedPartner
@deprecated
**/
const TUint32 KOtherDiskCriticalThreshold = 0x00000002;

#endif      // SYSUTILINTERNALPSKEYS_H

// End of File

