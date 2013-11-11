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

#include <e32capability.h>
#include <ssm/ssmswp.h>

#include "ssmdebug.h"
#include "ssmswppolicysrv.h"
#include "ssmswppolicysess.h"
#include "ssmswppolicyconst.h"
#include "ssmcommandutilprovider.h"

void CSsmSwpPolicySession::ServiceL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
		case ESsmPolicySrvSetSwpPolicyDllHandleAndInitialize:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicySession: ESsmPolicySrvSetSwpPolicyDllHandleAndInitialize"));
			iSsmSwpPolicySvr.CallSetDllHandleAndInitializeL(aMessage, iSessionIndex);
			break;
			}
			
		case ESsmPolicySrvInitializeCancel:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicySession: ESsmPolicySrvInitializeCancel"));
			iSsmSwpPolicySvr.CallInitializeCancel(iSessionIndex);
			aMessage.Complete(KErrNone);
			break;
			}
			
		case ESsmPolicySrvPrepareCommandList:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicySession: ESsmPolicySrvPrepareCommandList"));
			iSsmSwpPolicySvr.CallPrepareCommandListL(aMessage, iSessionIndex);
			break;
			}

		case ESsmPolicySrvPrepareCommandListCancel:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicySession: ESsmPolicySrvPrepareCommandListCancel"));
			iSsmSwpPolicySvr.CallPrepareCommandListCancel(iSessionIndex);
			aMessage.Complete(KErrNone);
			break;
			}

		case ESsmPolicySrvCommandList:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicySession: ESsmPolicySrvCommandList"));
			iSsmSwpPolicySvr.CallCommandListL(aMessage, iSessionIndex);
			break;
			}

		case ESsmPolicySrvHandleCleReturnValue:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicySession: ESsmPolicySrvHandleCleReturnValue"));
			iSsmSwpPolicySvr.CallHandleCleReturnValueL(aMessage, iSessionIndex);
			break;
			}

		case ESsmPolicySrvHandleCleReturnValueCancel:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicySession: ESsmPolicySrvHandleCleReturnValueCancel"));
			iSsmSwpPolicySvr.CallHandleCleReturnValueCancel(iSessionIndex);
			aMessage.Complete(KErrNone);
			break;
			}

		case EDebugMarkHeap:
			{
			#ifdef _DEBUG
			__UHEAP_MARK;
			DEBUGPRINT2(_L("CSsmSwpPolicySession - EDebugMarkHeap: Alloc Cells: %d."), User::CountAllocCells());
			#endif
			aMessage.Complete(KErrNone);
			break;
			}

		case EDeleteSSwpPolicySessionInfo:
			{
			#ifdef TEST_SSM_SERVER
			iSsmSwpPolicySvr.DeleteSSwpPolicySessionInfo(iSessionIndex);
			DEBUGPRINT1(_L("CSsmSwpPolicySession - EDeleteSSwpPolicySessionInfo"));
			#endif	//TEST_SSM_SERVER
			aMessage.Complete(KErrNone);
			break;
			}

		case EDebugMarkHeapEnd:
			{
			#ifdef _DEBUG
			__UHEAP_MARKEND;
			DEBUGPRINT2(_L("CSsmSwpPolicySession - EDebugMarkHeapEnd: Alloc Cells: %d."), User::CountAllocCells());
			#endif
			aMessage.Complete(KErrNone);
			break;
			}
			
		case EDebugSetHeapFailure:
			{
			#ifdef TEST_SSM_SERVER
			//User::__DbgSetAllocFail(RHeap::EUser,(RHeap::TAllocFail)aMessage.Int0(),aMessage.Int1());
			__UHEAP_SETFAIL(RAllocator::EFailNext,aMessage.Int0());
			DEBUGPRINT2(_L("CSsmSwpPolicySession - EDebugSetHeapFailure: Value of Failure Rate: %d."), aMessage.Int0());
			#endif
			aMessage.Complete(KErrNone);
			break;
			}
		case EDebugHeapReset:
			{
			#ifdef _DEBUG
			__UHEAP_RESET;
			DEBUGPRINT1(_L("CSsmSwpPolicySession - EDebugHeapReset"));
			#endif
			aMessage.Complete(KErrNone);
			break;
			}	
		default:
			{
			DEBUGPRINT1(_L("CSsmSwpPolicySession: Opcode not supported"));
			aMessage.Complete(KErrNotSupported);
			break;
			}
		}
	}

/**
Constructs and returns a pointer to CSsmSwpPolicySession.
*/
CSsmSwpPolicySession* CSsmSwpPolicySession::NewL(CSsmSwpPolicyServer& aSsmPolicyServer)
	{
	DEBUGPRINT1(_L("CSsmSwpPolicySession: Constructs Server side session for RSsmSwpPolicySession"));
	CSsmSwpPolicySession* self = new(ELeave) CSsmSwpPolicySession(aSsmPolicyServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSsmSwpPolicySession::CSsmSwpPolicySession(CSsmSwpPolicyServer& aSsmPolicyServer)
	: iSsmSwpPolicySvr(aSsmPolicyServer)
	{
	}

/**
Registers the session with SsmSwpPolicyServer
*/
void CSsmSwpPolicySession::ConstructL()
	{
	DEBUGPRINT1(_L("CSsmSwpPolicySession: Registers the session with SsmSwpPolicyServer"));
	iSsmSwpPolicySvr.RegisterSessionL(iSessionIndex);
	}
	
/**
Deregisters the session from SsmSwpPolicyServer
*/
CSsmSwpPolicySession::~CSsmSwpPolicySession()
	{
	DEBUGPRINT1(_L("CSsmSwpPolicySession: Deregisters the session from SsmSwpPolicyServer"));
	iSsmSwpPolicySvr.DeregisterSession(iSessionIndex);
	}

