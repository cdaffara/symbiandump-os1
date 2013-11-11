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
// lbssuplpushimplprops.cpp
// A class for the defining the Lbs Supl Push Publish and Subscribe properties 
// 
//

#include <e32cmn.h>
#include <e32property.h>
#include <centralrepository.h>
#include "lbsdevloggermacros.h"

#include "lbssuplpushprops.h"
#include "lbsrootcenrepdefs.h"


/**
Defines the RProperties used by the SUPL Push API implementation.

@leave If a error happens, it leaves with the correspondent error code.

@see CLbsSuplPushImpl
@see CLbsSuplPushRecChannel
*/
EXPORT_C void LbsSuplPushProps::InitializeL()
	{ 
	LBSLOG(ELogP1, "LbsSuplPushProps::InitializeL() Begin\n");
	LBSLOG(ELogP9, "->S LbsSuplPushProps::InitializeL() SuplPush\n");
	const TSecurityPolicy KBusyPolicy(ECapabilityReadDeviceData, ECapabilityNetworkServices);
	const TSecurityPolicy KInitReadPolicy(ECapabilityReadDeviceData, ECapabilityNetworkServices);
	const TSecurityPolicy KInitWritePolicy(ECapabilityReadDeviceData, ECapabilityNetworkServices);
	const TSecurityPolicy KAckReadPolicy(ECapabilityReadDeviceData, ECapabilityNetworkServices);
	const TSecurityPolicy KAckWritePolicy(ECapabilityReadDeviceData, ECapabilityNetworkServices);
	
	// Define the SMS BUSY property and set the initial data
	TInt err = RProperty::Define(KLbsSuplPushSmsBusyKey, RProperty::EInt, KBusyPolicy, KBusyPolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	// Define the WAP BUSY property
	err = RProperty::Define(KLbsSuplPushWapBusyKey, RProperty::EInt, KBusyPolicy, KBusyPolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	// Define the SMS INIT property
	err = RProperty::Define(KLbsSuplPushSmsInitKey, RProperty::ELargeByteArray, KInitReadPolicy, KInitWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	// Define the WAP INIT property
	err = RProperty::Define(KLbsSuplPushWapInitKey, RProperty::ELargeByteArray, KInitReadPolicy, KInitWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	// Define the SMS ACK property
	err = RProperty::Define(KLbsSuplPushSmsAckKey, RProperty::EInt, KAckReadPolicy, KAckWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	// Define the WAP ACK property
	err = RProperty::Define(KLbsSuplPushWapAckKey, RProperty::EInt, KAckReadPolicy, KAckWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	// Save the Uid of the process which defined the properties
	// in the cenrep file owned by LbsRoot.
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Set(KSuplPushAPIKey, TInt(RProcess().SecureId())));
	CleanupStack::PopAndDestroy(rep);
	LBSLOG(ELogP1, "LbsSuplPushProps::InitializeL() End\n");
	}
