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
// ssmclisrv.cpp
// 
//

#ifndef __SSMCLISRV_H__
#define __SSMCLISRV_H__

/**
@internalComponent
@released
*/
_LIT(KSsmServerName, "!SysStateMgr");

/**
A version must be specifyed when creating a session with the server
@internalComponent
@released
*/
const TUint KSsmServMajorVersionNumber=0;
/**
@internalComponent
@released
*/
const TUint KSsmServMinorVersionNumber=1;
/**
@internalComponent
@released
*/
const TUint KSsmServBuildVersionNumber=1;
/**
@internalComponent
@released
*/
const TInt  KSsmFirstUsedOpCode = 1;

/**
Op-codes used in message passing between client and server.
@internalComponent
@released
*/
enum TSsmServerRequests
	{
	ERequestStateTransition = KSsmFirstUsedOpCode,
	ERequestStateTransitionCancel,
	ERequestSwpChange,
	ERequestSwpChangeCancel,
	ERequestRegisterSwpMapping,
	//Only for testing purpose
	ERequestMarkHeap,
	ERequestMarkHeapEnd,
	ERequestDeRegisterSwpMapping,
	//End of supported opcodes
	EEndOfSsmOpCodes
	};

//Used only for testing purpose
enum TSsmHeapMarkEndParameters
	{
	ENoCleanup,
	EDoCleanup,
	ECleanupStateTransitionEngineOnly,
	ECleanupSwpRequestHandlerOnly
	};
#endif
