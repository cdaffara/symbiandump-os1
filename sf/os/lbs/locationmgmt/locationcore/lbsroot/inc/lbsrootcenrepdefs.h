// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definitions of various constants that
// are used when accessing the central repository file that 
// belongs to the LbsRoot process. For example, the UID of
// the repository itself, and the value of keys used to
// store information in it.
// The repoistory is mainly used by LbsRoot, but it is also
// used to store the Category UID for each of the internal 
// LBS APIs' that use Publish & Subscribe.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBSROOTCENREPDEFS_H
#define LBSROOTCENREPDEFS_H

#include <e32def.h>


/* Keys of the LBS system's processes within the cenrep initialisation file
 */
const TUint32 KLbsRootProcessNameFirstKey			= 0x00001000;
const TUint32 KLbsRootProcessUidFirstKey			= 0x00002000;
const TUint32 KLbsRootProcessIsServerFirstKey		= 0x00003000;

const TUint32 KLbsSystemTransientKey				= 0x00000001;
const TUint32 KLbsRootProcessTransientKey			= 0x00000002;
//const TUint32 KLbsProcessMaxStartupTries			= 0x00000003;

const TUint KLbsCenRepUidValue						= 0x10282266;
const TUid  KLbsCenRepUid							= {KLbsCenRepUidValue};


/* These keys are used to store the Category UID for each of
   the internal APIs that use Publish & Subsribe (RProperty).
*/

// Interfaces/APIs defined in LbsInternalInterface.h
const TUint32 KModuleInfoCategoryKey				= 0x00005000;
const TUint32 KModuleStatusCategoryKey				= 0x00005001;
const TUint32 KSystemModuleEventCategoryKey			= 0x00005002;
const TUint32 KManagerPowerModeAdviceCategoryKey	= 0x00005003;
const TUint32 KPositionUpdatesCategoryKey			= 0x00005004;
const TUint32 KPositionUpdateRequestsCategoryKey	= 0x00005005;
const TUint32 KQualityProfileCategoryKey			= 0x00005006;
const TUint32 KGpsMeasurementUpdatesCategoryKey		= 0x00005007;

// Interfaces/APIs
const TUint32 KNgAssistanceDataUpdatesCategoryKey	= 0x00006000;

// Interfaces/APIs defined in LbsNetInternalApi.h
const TUint32 KNetGatewayChannelCategoryKey			= 0x00007000;
const TUint32 KNetRequestChannelCategoryKey			= 0x00007001;
const TUint32 KNetworkLocationRequestCategoryKey	= 0x00007002;
const TUint32 KNetworkLocationResponseCategoryKey	= 0x00007003;
const TUint32 KNetworkPositionUpdateCategoryKey		= 0x00007004;
const TUint32 KNetAGPS2NGChannelCategoryKey			= 0x00007005;
const TUint32 KNetNG2AGPSChannelCategoryKey			= 0x00007006;
const TUint32 KNetworkRegistrationStatusCategoryKey	= 0x00007007;
const TUint32 KNetworkLsToNrhCategoryKey			= 0x00007008;
const TUint32 KNetworkNrhToLsCategoryKey			= 0x00007009;
const TUint32 KNetSUPLPSY2NGChannelCategoryKey      = 0x0000700A;
const TUint32 KNetNG2SUPLPSYChannelCategoryKey      = 0x0000700B;

// Interfaces/APIs defined in LbsRootApi
const TUint32 KSystemControllerCategoryKey			= 0x00008000;
const TUint32 KProcessSupervisorCategoryKey			= 0x00008001;

// Interfaces/APIs defined in LbsDebug
const TUint32 KOomTestCategoryKey					= 0x0000A000;
const TUint32 KOomTestKey							= 0x0000A001;

// SUPL Push API 
const TUint32 KSuplPushAPIKey						= 0x0000B000;

// Positioning Status API 
const TUint32 KNiPositioningStatusAPIKey			= 0x0000C000;
const TUint32 KMoPositioningStatusAPIKey			= 0x0000C001;

#endif // LBSROOTCENREPDEFS_H
