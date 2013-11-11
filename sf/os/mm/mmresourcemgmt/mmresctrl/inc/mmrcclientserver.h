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
 @internalTechnology
*/

#ifndef MMRCCLIENTSERVER_H
#define MMRCCLIENTSERVER_H

#include <e32base.h>

enum TReason
	{
	EUninitialize,
	EInitialize,
	EUnload,
	ELoad,
	EStop,
	EPrimeReason,
	EActivate,
	EReasonError
	};

// Function opcodes for client to server messaging
enum TMMRCClientToServerMessages
	{
	EMMRCClientToServerSetClientThreadId = 0,
	EMMRCClientToServerSetMsgQueueHandle,
	EMMRCClientToServerSetAdaptationProxy,
	EMMRCClientToServerSetCallback,
	EMMRCClientToServerRequestContextId,
	EMMRCClientToServerCloseSession,
	EMMRCClientToServerRequestResource,
	EMMRCClientToServerRegisterAsClient,
	EMMRCClientToServerCancelRegisterAsClient,
	EMMRCClientToServerWillResumePlay
	};

// Function opcodes for server to client messaging
enum TMMRCServerToClientMessageCallTypes
	{
	};

// Function opcodes for server to client messaging
enum TMMRCServerToClientMessageResults
	{
	EMMRCServerToClientResourceAgreed = 0,
	EMMRCServerToClientResourcePreempt,
	EMMRCServerToClientResourceDenied,	
	EMMRCServerToClientCommitUpdate,
	EMMRCServerToClientRequestError
	};

// Function opcodes for server to client messaging
enum TMMRCAdaptationToServerRequestResults
	{
	EMMRCAdaptationToServerResourceComplete = 0,
	EMMRCAdaptationToServerResourceIncomplete,
	EMMRCAdaptationToServerResourceTimeout,
	EMMRCAdaptationToServerShutdown
	};
	
enum TMMRCServerState
	{
	EMMRCServerStateIdle = 0,
	EMMRCServerStateWaitResourceResponseFromAdaptation,
	EMMRCServerStateWaitPreemptionResponse
	};
	
enum TMMRCRule
	{
	EAlwaysProcess = 0,
	EProcessOnIdle
	};
	
	
	
enum TMMRCRequestType
	{
	EMMRCRequestCommitUpdate = 0,
	EMMRCRequestCommitResponse,
	EMMRCRequestResourceUnallocated
	};	


class MLogicalChain;

class TMMRCQueueItem
	{
public:
	MLogicalChain*						iData;
	TMMRCRequestType					iRequestType;
	TMMRCServerToClientMessageResults 	iResult;
	TInt 								iErrorCode;
	TUint 								iMessageSize;
	};

	IMPORT_C TInt StartMMRCServer(RThread& aServerThread);

#endif //__MMRCCLIENTSERVER_H

