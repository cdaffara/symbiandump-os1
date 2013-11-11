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
// Location Based Services Definition of process UIDs
// Defines the UIDs for each of the component processes of the LBS system
// 
//

/**
 @file
 @internalTechnology
 @release
*/

#ifndef __LBSPROCESSUIDDEFS_H__
#define __LBSPROCESSUIDDEFS_H__

#include <e32std.h>

/**
UID and value of Location Server
@internalcomponent
@released
*/

const TUint KLbsLocServerUidValue			= 0x10281D42;
const TUid  KLbsLocServerUid				= {KLbsLocServerUidValue};

/**
UID and value of Network location manager
@internalcomponent
@released
*/

const TUint KLbsNetLocManagerUidValue		= 0x10281D43;
const TUid  KLbsNetLocManagerUid			= {KLbsNetLocManagerUidValue};

/**
UID and value of SUPL PSY
@internalcomponent
@released
*/
const TUint KLbsSuplPsyUidValue             = 0x20026FB7;
const TUid  KLbsSuplPsyUid                  = {KLbsSuplPsyUidValue};


/**
UID and value of GPS location manager
@internalcomponent
@released
*/

const TUint KLbsGpsLocManagerUidValue		= 0x10281D44;
const TUid  KLbsGpsLocManagerUid			= {KLbsGpsLocManagerUidValue};

/**
UID and value of Network request handler
@internalcomponent
@released
*/

const TUint KLbsNetRequestHandlerUidValue	= 0x10281D45;
const TUid  KLbsNetRequestHandlerUid		= {KLbsNetRequestHandlerUidValue};

/**
UID and value of Network gateway
@internalcomponent
@released
*/

const TUint KLbsNetworkGatewayUidValue   	= 0x10281D46;
const TUid  KLbsNetworkGatewayUid    		= {KLbsNetworkGatewayUidValue};

/**
UID and value of Root process 
@internalcomponent
@released
*/

const TUint KLbsRootUidValue                = 0x10282253;
const TUid  KLbsRootUid			            = {KLbsRootUidValue};

/**
UID and value of Root API
@internalcomponent
@released
*/

const TUint KLbsRootApiUidValue             = 0x10282254;
const TUid  KLbsRootApiUid			        = {KLbsRootApiUidValue};

/**
The root process now defines everything
defauly initialization process is the root process
@internalcomponent
@released
*/

const TUint KLbsDefaultInitializationProcessValue = KLbsRootUidValue;
const TUid KLbsDefaultInitializationProcessUid = {KLbsDefaultInitializationProcessValue};

/**
UID and value of Location Monitor
@internalcomponent
@released
*/

const TUint KLbsLocMonitorUidValue   	= 0x102869DF;
const TUid  KLbsLocMonitorUid    		= {KLbsLocMonitorUidValue};


#endif // __LBSPROCESSUIDDEFS_H__
