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

#include "tssm_statepolicy_paniccle.h"

EXPORT_C MSsmStatePolicy* CSsmStatePolicyPanicCle::NewL()
	{
	CSsmStatePolicyPanicCle* self = new (ELeave) CSsmStatePolicyPanicCle;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicyPanicCle::CSsmStatePolicyPanicCle()
	{
	}

CSsmStatePolicyPanicCle::~CSsmStatePolicyPanicCle()
	{
 	delete iCommandList;
	}

void CSsmStatePolicyPanicCle::ConstructL()
	{
	}

void CSsmStatePolicyPanicCle::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmStatePolicyPanicCle::InitializeCancel()
	{
	}

void CSsmStatePolicyPanicCle::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyPanicCle::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void) aCurrent;
	(void) aQueued;
	(void)aMessage; //Security checks on originator omitted for now
	
	// This state should never be 'active' (unless we know exactly what we're doing), so the only valid condition
	//  is when it is used as per the following way (as in tssm_step_paniccle.cpp)
	if (aRequest.Reason() == ESsmTestPanicCleGoodReason)
		return EDefinitelyAllowed;
	else
		return ENotAllowed; 
	}

void CSsmStatePolicyPanicCle::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	RDebug::Printf ("--- CSsmSwpPolicyPanicCle::PrepareCommandList for state %04x.%04x", aState.MainState(), aState.SubState());
	TRAPD(err, doPrepareCommandListL(aState, aReason));
	RDebug::Printf ("--- CSsmSwpPolicyPanicCle::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference


void CSsmStatePolicyPanicCle::doPrepareCommandListL(TSsmState aState, TInt aReason)
	{
	(void)aReason;

	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

	// Create a simple (valid) command that with an invalid command that panics the Cle
	_LIT(KDllName, "tssm_customcmd.dll");
	TPtrC filename = KDllName();
	TInt32 ordinal = 1;				// ordinal 1 of this command panics
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
	MSsmCommand* cmd2 = SsmCommandFactory::ConstructCustomCommandCommandLC(ECmdIgnoreFailure, ESsmWaitForSignal, *customInfo);
	iCommandList->AppendL(cmd1);
	iCommandList->AppendL(cmd2);
	CleanupStack::Pop(2);

	// clean-up
	CleanupStack::PopAndDestroy(customInfo);
	} //lint !e1746 suppress param 'aState' could be made const ref. Because aState has the same size as a reference

void CSsmStatePolicyPanicCle::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmStatePolicyPanicCle::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

TBool CSsmStatePolicyPanicCle::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	(void)aCurrentTransition;
	(void)aReason;
	(void)aSeverity;
	(void)aNextState;

   	RDebug::Printf("--- CSsmStatePolicyPanicCle::GetNextState received value %d from Cle", aError);

	// We've executed a command list that will panic the Cle --> we're expecting a KErrServerTerminated
	if (aError != KErrServerTerminated)
		{
		// That's a genuine, not expected error ==> panic!
		User::Panic(KStatePanicCleTestPanicCategory, EStatePanicCleTestFailed2);	
		}
	
	return EFalse;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference
