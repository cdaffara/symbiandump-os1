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
//

/**
 @file
 @internalComponent
*/

#ifndef HCIIPC_H
#define HCIIPC_H

#include <e32base.h>

_LIT(KHCIServerName, "!HCISrv");

/** Version numbers. */
const TInt8 KHCISrvMajorVersionNumber = 1;
const TInt8 KHCISrvMinorVersionNumber = 1;
const TInt16 KHCISrvBuildNumber = 0;

//Any server that implements KHCISrvVersion2MajorVersionNumber must also be responsible
//for ensuring that it does not get unloaded while it has any open sessions. To
//do this the server should make use of the client usage APIs provided by the Bluetooth
//stack, see MHCIClientUsageCallback and MHCIClientUsage.
const TInt8 KHCISrvVersion2MajorVersionNumber = 2;
const TInt8 KHCISrvVersion2MinorVersionNumber = 0;
const TInt16 KHCISrvVersion2BuildNumber = 0;

//This constant is used in CHCIDirectAccessSAPsManager::CancelIoctl to cancel a HCI
//Direct Access operation using the new client server architecture. Licensees will
//provide the enums for the operations but they must keep the value of Cancel as 0 
//if they still want to maintain the Ioctl interface.
const TInt KHCIDirectAccessCancel = 0;
const TInt KHCIDirectAccessVendorCommand = 1;

const TInt KHCIPowerControlManagerUid = 0x102736DF;
const TInt KHCIDirectAccessManagerUid = 0x102736E0;
const TInt KHCIA2dpOptimserManagerUid = 0x102872AF;
const TInt KHCIBluetoothDutModeManagerUid = 0x102872B0;

enum THciServerFunctions
	{
	EConnectToService = 0,
	EServiceSpecificRequest = 1,
	/* Add new server functions before this */
	ENumOfHciServerFunctions // Never use as an actual function number
	};

_LIT(KHCIServerClientPanicCat, "HCIServerClient");

enum THciServerClientPanics
	{
	EBadServiceConnectRequest = 0,
	EServiceAlreadyConnected = 1,
	EServiceNotConnectedYet = 2,
	EUnknownHciServerFunction = 3,
	EUnknownDirectAccessFunction = 4,
	EUnknownPowerControlFunction = 5,
	EInvalidVendorCommand = 6,
	EUnknownBluetoothDutModeFunction = 7,
	EUnknownA2dpOptimiserFunction = 8,
	EInvalidArgument = 9,
	};

#endif // HCIIPC_H
