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
// Shared client/server definitions
// 
//

/**
 @file
 @internalComponent
*/

#ifndef _EIRMANSHARED_H
#define _EIRMANSHARED_H

#include <e32std.h>

const TUid KEirManSvrUid = {0x102835A6};

const TInt KEirManSrvMajorVersionNumber = 1;
const TInt KEirManSrvMinorVersionNumber = 0;
const TInt KEirManSrvBuildVersionNumber = 0;

// Packet size
const TInt KEirDM1PacketSize = 17;				// DM1
const TInt KEirDM3PacketSize = 123;				// DM3
const TInt KEirDM5PacketSize = 226;				// DM5
const TInt KEirPacketSize = KEirDM5PacketSize;	// The maximum eir packet size with FEC
const TInt KEirDM1PayloadHeaderSize = 1;		// Payload Header length for DM1
const TInt KEirNoneDM1PayloadHeaderSize = 2;	// Payload Header length for DM3 and DM5

const TInt KEirLengthTagLength = 2;

_LIT(KEirManServerName, "!eirman");

enum TEirManMessages
	{
	EEirManRegisterTag,
	EEirManSpaceAvailableNotification,
	EEirManCancelSpaceAvailableNotification,
	EEirManSetData,
	EEirManNewData,
	/* Add new server functions before this */
	EEirManServerMessageCount
	};

_LIT(KEirManCliPncCat, "EirMan-Client");

enum TEirManPanicClient
	{
	EEirManPanicInvalidIPC,
	EEirManPanicInvalidTag,
	EEirManPanicSetDataPending,
	EEirManPanicSetDataUninvited,
	};

enum TEirTag
	{
	EEirTagName,
	EEirTagSdpUuid16,
	EEirTagSdpUuid32,
	EEirTagSdpUuid128,
	EEirTagManufacturerSpecific,
	EEirTagFlags,
	EEirTagTxPowerLevel,
	EEirTagRESERVED
	};


enum TEirDataMode
	{
	EEirDataPartial = 0,
	EEirDataComplete,
	};
	
#endif //_EIRMANSHARED_H

