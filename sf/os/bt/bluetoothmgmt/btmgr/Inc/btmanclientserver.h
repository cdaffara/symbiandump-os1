// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTMANCLIENTSERVER_H
#define BTMANCLIENTSERVER_H

/*
* BT Manager Client and Server Utilities
*/

// server name
_LIT(KBTManServerName,"!BTManServer");


#ifdef __BTMANSERVER_NO_PROCESSES__
	_LIT(KBTManServerImg,"BTManServer");	//<DLL/EXE name
	const TInt KBTManServerStackSize=0x2000;			//  8KB
	const TInt KBTManServerInitHeapSize=0x1000;		//  4KB
	const TInt KBTManServerMaxHeapSize=0x1000000;		// 16MB
#else
	_LIT(KBTManServerImg,"BTManServer");	//<DLL/EXE name
#endif




const TUid KBTManServerUid3={0x100069cc};

// A version must be specified when creating a session with the server
const TUint KBTManServerMajorVersionNumber=2;
const TUint KBTManServerMinorVersionNumber=0;
const TUint KBTManServerBuildVersionNumber=0;

// opcodes used in message passing between client and server
enum TBTManServerRequest
	{
//	EBTManCreateRegistrySession,	//can't do this...no named sessions :-(
	EBTManCreateHostResolverSubSession,
	EBTManCreateRegistrySubSession,
	EBTManCreateCommPortSettingsSubSession,
	EBTManCreateLocalDeviceSubSession,
	EBTManRegistrySearch,
	EBTManExtractRegistryDataIntoServer,
	EBTManRetrieveRegistryData,
	EBTManCloseSubSession,
	EBTManCancelRequest,
	EBTRegistryRetrieveDevice,
	EBTRegistryAddDevice,
	EBTRegistryGetNamelessDevice,
	EBTRegistryModifyBluetoothName,
	EBTRegistryModifyFriendlyName,
	EBTRegistryModifyNamelessDevice,
	EBTRegistryDeleteDevices,
	EBTRegistryDeleteLinkKey,
	EBTRegistryUnpairView,
	EBTRegistryCloseView,
	EBTRegistryGetLocalDevice,
	EBTRegistryUpdateLocalDevice,
	EBTRegistryGetCommPortSettings,
	EBTRegistryUpdateCommPortSettings,
	EBTRegistryDeleteCommPortSettings,
//	EBTRegistryNotifyLocalDeviceChange,
//	EBTRegistryNotifyRegistryChange,
	EBTHostResolverDeviceRequest,
	EBTHostResolverGetNextDeviceRequest,
	EBTHostResolverDeviceModifyDevice,
	EBTHostResolverNotifyRequest,
	EBTManSetHeapFailure,
	EBTManSubSessionCount,
	EBTRegistryNotifyViewChange,
	//	** Important note **
	//	If new function enums are added, the security policy
	//	in BtManSec.h *will* need to be amended.
	//	********************
	};

_LIT(KBTManClientPanic, "BTManClient");

enum TBTManClientPanics
	{
	EBTManClientBadResultRetrieveState,
	EBTManClientResultRetrieveAlreadyActive,
	};

#endif
