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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <ssm/ssmcommandfactory.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmcustomcommandinfo.h>
#include <ssm/ssmpublishsystemstateinfo.h>
#include <e32debug.h>

#include <e32capability.h>

#include "tssm_statepolicy_capability.h"

EXPORT_C MSsmStatePolicy* CSsmStatePolicyCapability::NewL()
	{
	CSsmStatePolicyCapability* self = new (ELeave) CSsmStatePolicyCapability;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicyCapability::CSsmStatePolicyCapability()
	{
	}

CSsmStatePolicyCapability::~CSsmStatePolicyCapability()
	{
 	delete iCommandList;
	}

void CSsmStatePolicyCapability::ConstructL()
	{
	}

void CSsmStatePolicyCapability::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmStatePolicyCapability::InitializeCancel()
	{
	}

void CSsmStatePolicyCapability::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyCapability::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void)aCurrent;
	(void)aQueued;

	if (aRequest.Reason() == ESsmTestCapabilityReasonPowerMgmt)
		{
		return EDefinitelyAllowed;
		}
		
	if (aRequest.Reason() == ESsmTestCapabilityReasonLeave)	
		{
		// That's an intentionate programming error, as TransitionAllowed must not leave. We're testing what if.
		aMessage.HasCapabilityL(ECapabilityPowerMgmt, __PLATSEC_DIAGNOSTIC_STRING("--- CSsmStatePolicyCapability::TransitionAllowed thats a known negative test..."));
		}
	
	return ENotAllowed;
	}

void CSsmStatePolicyCapability::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	RDebug::Printf ("--- CSsmStatePolicyCapability::PrepareCommandList for state %04x.%04x", aState.MainState(), aState.SubState());
	TRAPD(err, doPrepareCommandListL(aState, aReason));
	RDebug::Printf ("--- CSsmStatePolicyCapability::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference


void CSsmStatePolicyCapability::doPrepareCommandListL(TSsmState aState, TInt aReason)
	{
	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

	if (aReason == ESsmTestCapabilityReasonCustomCmd)
		{
		// Create a simple (valid) command that with an custom command that requires certain capability
		_LIT(KDllName, "tssm_customcmd.dll");
		TPtrC filename = KDllName();
		TInt32 ordinal = 2;				// ordinal 2 of this command returns KErrPermissionDenied
		TPtrC8 params = KNullDesC8();
		TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
		TInt retries = 3;

		CSsmCustomCommandInfo* customInfo = new (ELeave) CSsmCustomCommandInfo();
		CleanupStack::PushL(customInfo);
		customInfo->SetL(filename, ordinal, unloading, retries, params);

		TSsmPublishSystemStateInfo info;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
		info.Set(aState, ETraverseDefault, 0);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
		info.Set(aState, ESsmTraverseDefault, 0);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
		
		// Make sure that the publish system state command is the first one in the command list (Symbian's recomendation, but not important here)
		MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, info);
		MSsmCommand* cmd2 = SsmCommandFactory::ConstructCustomCommandCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, *customInfo);
		iCommandList->AppendL(cmd1);
		iCommandList->AppendL(cmd2);
		CleanupStack::Pop(2);
		
		// clean-up
		CleanupStack::PopAndDestroy(customInfo);
		}
	else if (aReason == ESsmTestCapabilityReasonPowerMgmt)
		{
		// do nothing --> Empty command list
		}
	else
		{
		// That's a genuine, not expected error, that policy should not be used in that way ==> panic!
		User::Panic(KStateCapabilityTestPanicCategory, EStateCapabilityTestFailed1);	
		}
	
	} //lint !e1746 suppress param 'aState' could be made const ref. Because aState has the same size as a reference


void CSsmStatePolicyCapability::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmStatePolicyCapability::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

TBool CSsmStatePolicyCapability::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	(void)aCurrentTransition;
	(void)aSeverity;
	(void)aNextState;

   	RDebug::Printf("--- CSsmStatePolicyCapability::GetNextState received value %d from Cle", aError);

	if (aReason == ESsmTestCapabilityReasonCustomCmd && aError == KErrPermissionDenied)
		{
		// We've executed a cmd which required a capability we don't have --> we're expecting a KErrPermissionDenied
		return EFalse;
		}
	else if (aReason == ESsmTestCapabilityReasonPowerMgmt && aError == KErrArgument)
		{
		// We've executed an empty cmd list --> we're expecting a KErrArgument
		return EFalse;
		}
	
	// That's a genuine, not expected error, we should never reach this point ==> panic!
	User::Panic(KStateCapabilityTestPanicCategory, EStateCapabilityTestFailed2);	

	return EFalse;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference
