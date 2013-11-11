// ssmswppolicyconst.h

// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SSMSWPPOLICYCONST_H_
#define SSMSWPPOLICYCONST_H_

const TUint KFirstUsedOpCode = 1;

enum TSsmSwpPolicySrvCommand
	{
	ESsmPolicySrvSetSwpPolicyDllHandleAndInitialize = KFirstUsedOpCode,
	ESsmPolicySrvInitializeCancel,
	ESsmPolicySrvPrepareCommandList,
	ESsmPolicySrvPrepareCommandListCancel,
	ESsmPolicySrvCommandList,
	ESsmPolicySrvHandleCleReturnValue,
	ESsmPolicySrvHandleCleReturnValueCancel,

	//Only for testing purpose and used in debug builds only
	EDebugMarkHeap,
	EDeleteSSwpPolicySessionInfo,
	EDebugMarkHeapEnd,
	EDebugSetHeapFailure,
	EDebugHeapReset,
	
	EEndOfSsmSwpPolicySrvOpCodes
	};

const TInt KSsmSwpPolicySrvVersionMajor = 0; // 0-127
const TInt KSsmSwpPolicySrvVersionMinor = 0; // 0-99
const TInt KSsmSwpPolicySrvVersionBuild = 0; // 0-32767
	
_LIT(KSsmSwpPolicyServer,"SsmSwpPolicyServer");

#define IN_RANGE( x, y ) ( (x >= 0) && (x < y) )

_LIT( KSsmSwpPolicyBadIdx,"SsmSwpPolicyBadIdx" );
const TInt KSsmSwpPolicySrvArrayIndexInvalid = -555;	//random no.
const TInt KSsmSwpPolicyStreamBufMaxSize = 6000;
	
#endif //SSMSWPPOLICYCONST_H_