/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Central Repository definitions of the SysUtil subsystem
*
*/


#ifndef SYSUTIL_DOMAIN_CR_KEYS_H
#define SYSUTIL_DOMAIN_CR_KEYS_H

// INCLUDES
#include <centralrepository.h>

// =============================================================================
// Defines critical level values for memory and disk space
// =============================================================================

const TUid KCRUidDiskLevel = {0x10205056};

/**
* Threshold for disk space critical level (bytes).
**/
const TUint32 KDiskCriticalThreshold = 0x00000001;

/**
* Threshold for RAM disk space critical level (bytes).
**/
const TUint32 KRamDiskCriticalLevel = 0x00000002;

#endif // SYSUTIL_DOMAIN_CR_KEYS_H
