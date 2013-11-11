// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// lbspositioningstatusprops.cpp
// A class for the defining the Lbs Positioning Status Publish and Subscribe properties 
// 
//

#include <e32cmn.h>
#include <e32property.h>
#include <centralrepository.h>
#include "lbsdevloggermacros.h"

#include "lbspositioningstatusprops.h"
#include "lbsrootcenrepdefs.h"


/**
Defines the property used by the Positioning Status API implementation for
receiving updates regarding MT-LR positioning sessions in progress

Note that this may be created by a different process to the MO property
due to different processes being persistant in different LBS configurations.

This key is created by LbsRoot.

@leave If a error happens, it leaves with the correspondent error code.
*/
EXPORT_C void LbsPositioningStatusProps::InitializeNiPropertyL()
	{ 
	LBSLOG(ELogP1, "LbsPositioningStatusProps::InitializeNiPropertyL() Begin\n");
	LBSLOG(ELogP9, "->S LbsPositioningStatusProps::InitializeNiPropertyL() InitializeNiPropertyL \n");
	const TSecurityPolicy KPositioningStatusPolicy(ECapabilityReadDeviceData, ECapabilityLocation);
	
	// Define the NI Positioning Status property
	TInt err = RProperty::Define(KLbsNiPositioningStatusKey, RProperty::EInt, KPositioningStatusPolicy, KPositioningStatusPolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	if (err != KErrAlreadyExists)
		{
		// store the Uid of the process that defined the property
		CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
		User::LeaveIfError(rep->Set(KNiPositioningStatusAPIKey, TInt(RProcess().SecureId())));
		CleanupStack::PopAndDestroy(rep);
		}
	
	LBSLOG(ELogP1, "LbsPositioningStatusProps::InitializeNiPropertyL() End\n");
	}

/**
Defines the property used by the Positioning Status API implementation for
receiving updates regarding MO-LR positioning sessions in progress

Note that this may be created by a different process to the MO property
due to different processes being persistant in different LBS configurations.

This key is created by the Location Server by default.

@leave If a error happens, it leaves with the correspondent error code.
*/
EXPORT_C void LbsPositioningStatusProps::InitializeMoPropertyL()
	{ 
	LBSLOG(ELogP1, "LbsPositioningStatusProps::InitializeMoPropertyL() Begin\n");
	LBSLOG(ELogP9, "->S LbsPositioningStatusProps::InitializeMoPropertyL() InitializeMoPropertyL \n");
	const TSecurityPolicy KPositioningStatusPolicy(ECapabilityReadDeviceData, ECapabilityLocation);
	
	// Define the NI Positioning Status property
	TInt err = RProperty::Define(KLbsMoPositioningStatusKey, RProperty::EInt, KPositioningStatusPolicy, KPositioningStatusPolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	if (err != KErrAlreadyExists)
		{
		// store the Uid of the process that defined the property
		CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
		User::LeaveIfError(rep->Set(KMoPositioningStatusAPIKey, TInt(RProcess().SecureId())));
		CleanupStack::PopAndDestroy(rep);
		}
	
	LBSLOG(ELogP1, "LbsPositioningStatusProps::InitializeMoPropertyL() End\n");
	}

