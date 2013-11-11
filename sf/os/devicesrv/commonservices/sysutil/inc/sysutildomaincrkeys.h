/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef SYSUTILDOMAINCRKEYS_H
#define SYSUTILDOMAINCRKEYS_H

// INCLUDES
#include <centralrepository.h>

// =============================================================================
// Defines critical level values for memory and disk space
// =============================================================================

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
const TUid KCRUidDiskLevel = {0x10205056};

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
const TUint32 KDiskCriticalThreshold = 0x00000001;

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
const TUint32 KRamDiskCriticalLevel = 0x00000002;

#endif // SYSUTILDOMAINCRKEYS_H
