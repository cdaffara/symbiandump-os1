// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// API for the Remote Control server side.
// Used by the Remote Control client side library.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef REMCONSERVER_H
#define REMCONSERVER_H

#include <remcon/remconserverid.h>
#include <remconaddress.h>
#include "operationinformation.h"

_LIT(KRemConServerImg, "z:\\sys\\bin\\remconserver.exe");

_LIT(KRemConServerName, "!RemConSrv");

/** Version numbers for the RemCon server. */
const TInt8 KRemConSrvMajorVersionNumber = 2;
const TInt8 KRemConSrvMinorVersionNumber = 0;
const TInt16 KRemConSrvControllerSession = 1;
const TInt16 KRemConSrvTargetSession = 2;

/** IPC messages supported by the RemCon server. */
enum TRemConIpc
	{
	ERemConDbgMarkHeap						= 0, // the 1st message must be 0 for CPolicyServer to start up
	ERemConDbgCheckHeap 					= 1,
	ERemConDbgMarkEnd						= 2,
	ERemConDbgFailNext						= 3,
	ERemConSetPlayerType					= 4,
	ERemConGoConnectionOriented 			= 5,
	ERemConGoConnectionless 				= 6,
	ERemConConnectBearer					= 7,
	ERemConConnectBearerCancel				= 8,
	ERemConDisconnectBearer 				= 9,
	ERemConDisconnectBearerCancel			= 10,
	ERemConSend 							= 11,
	ERemConSendCancel						= 12,
	ERemConReceive							= 13,
	ERemConReceiveCancel					= 14,
	ERemConGetConnectionCount				= 15,
	ERemConGetConnections					= 16,
	ERemConNotifyConnectionsChange			= 17,
	ERemConNotifyConnectionsChangeCancel	= 18,
	ERemConRegisterInterestedAPIs			= 19,
	ERemConSendUnreliable					= 20,
	ERemConSendNotify						= 21,
	//
	// Next entry should always come last. 
	//
	ERemConNumberOfServerFunctions
	};

_LIT(KRemConBulkServerName, "!RemConBulkSrv");

/** Version numbers for RemCon bulk server. */
const TInt8 KRemConBulkSrvMajorVersionNumber = 1;
const TInt8 KRemConBulkSrvMinorVersionNumber = 1;
const TInt16 KRemConBulkSrvBuildNumber = 0;

/** IPC messages supported by the RemCon bulk server. */
enum TRemConBulkIpc
	{
	ERemConBulkDbgMarkHeap						= 0, // the 1st message must be 0 for CPolicyServer to start up
	ERemConBulkDbgCheckHeap 					= 1,
	ERemConBulkDbgMarkEnd						= 2,
	ERemConBulkDbgFailNext						= 3,
	ERemConBulkSend 							= 4,
	ERemConBulkSendCancel						= 5,
	ERemConBulkReceive							= 6,
	ERemConBulkReceiveCancel					= 7,
	ERemConBulkRegisterInterestedAPIs			= 8,
	ERemConBulkSendUnreliable					= 9,
	//
	// Next entry should always come last.
	//
	ERemConBulkNumberOfServerFunctions
	};

/**
This struct wraps up the information passed from the server to the client in 
an RRemCon::Receive operation.
It does not however include the arbitrary-length narrow descriptor 'associated 
data' as this cannot be wrapped in a struct.
We do it this way because TIpcArgs only has four slots and we've used all of 
them. 
*/
NONSHARABLE_STRUCT(TRemConClientReceivePackage)
	{
	TUid iInterfaceUid;
	TUint iOperationId;
	TRemConMessageSubType iMessageSubType;
	TRemConAddress iRemoteAddress;
	};

#endif // REMCONSERVER_H
