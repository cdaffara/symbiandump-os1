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
// NOTE:
//

// ### NextStateA in conjunction with NextStateB plugin dlls are used as part of an integration test.
// The idea is that the test will request the following transitions:
// InitialState 
// |	(request transition to state NextStateA)
// NextStateA (with sub states A1 and A2)
// |	(::GetNextState() returns NextStateB)
// NextStateB (with sub states B1 and B2)
// |	(::GetNextState() returns NextStateC)
// NextStateC ----> There are no further transitions in this test state, so the loop terminates!
// ### To verify that each state is activated, the cmd list will inclue a CreateSwp command,
// so the test needs to verify that the 4 unique swp keys exist.
// swp	UID = substate
// swp	Value = mainstate + substate
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include <ssm/ssmcommandfactory.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmstates.hrh>
#include <ssm/ssmstatemanager.h>
#include <domainmanager.h>
#include <ssm/ssmstate.h>
#include <ssm/ssmstartupproperties.h>
#include <ssm/ssmpublishsystemstateinfo.h>
#include <e32debug.h>

#include "tssm_statepolicy_nextstatea.h"
#include "tssm_statepolicy_nextstateb.h"
#include "tssm_statepolicy_nextstatec.h"

#include "tssm_swppolicy_common.h"

EXPORT_C MSsmStatePolicy* CSsmStatePolicyNextStateC::NewL()
	{
	CSsmStatePolicyNextStateC* self = new (ELeave) CSsmStatePolicyNextStateC;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicyNextStateC::CSsmStatePolicyNextStateC()
	{
	}

CSsmStatePolicyNextStateC::~CSsmStatePolicyNextStateC()
	{
 	delete iCommandList;
	}

void CSsmStatePolicyNextStateC::ConstructL()
	{
	}

void CSsmStatePolicyNextStateC::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmStatePolicyNextStateC::InitializeCancel()
	{
	}

void CSsmStatePolicyNextStateC::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyNextStateC::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void)aMessage;
	(void)aCurrent;
	(void)aQueued;

	
	// While transitioning by means of GetNextState, the framework doesn't ask the policy if it is allowed
	//	as it is supposed that the policy knows what it is doing
	// Only don't panic if one case, used to verify that this policy dll is properly loaded.
	if (aRequest.Reason() != ESsmTestNextStateCNoPanicReason)
		{
		RDebug::Printf ("--- CSsmStatePolicyNextStateC::TransitionAllowed called....this should never happen! Panic will come soon...");
		User::Panic(KStateNextStateCTestPanicCategory, EStateNextStateCTestFailed3);
		}
	
	return ENotAllowed;
	}


void CSsmStatePolicyNextStateC::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	RDebug::Printf ("--- CSsmStatePolicyNextStateC::PrepareCommandList for Main state 0x%04x", aState.MainState());
	TRAPD(err, doPrepareCommandListL(aState, aReason));
	RDebug::Printf ("--- CSsmStatePolicyNextStateC::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference


void CSsmStatePolicyNextStateC::doPrepareCommandListL(TSsmState aState, TInt aReason)
	{
	(void)aReason;

	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

	// Prepare the swp to be created
	const TUint key={ESsmTestNextStateC1Substate};
	const TInt value=aState.MainState() + ESsmTestNextStateC1Substate;
	TSsmSwp swp(key,value);
	TPtrC filename = KTestSwpPolicyCommonFile();

	// Prepare state info
	TSsmPublishSystemStateInfo info;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
	info.Set(aState, ETraverseDefault, 0);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
	info.Set(aState, ESsmTraverseDefault, 0);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

	// Create a valid command that with just 2 commands (createSwp & publishSystemState)
	MSsmCommand* cmd1 = SsmCommandFactory::ConstructCreateSwpCommandLC(ECmdCriticalSeverity, swp, filename);
	MSsmCommand* cmd2 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, info);
	iCommandList->AppendL(cmd1);
	iCommandList->AppendL(cmd2);
	CleanupStack::Pop(2);
	} //lint !e1746 suppress param 'aState' could be made const ref. Because aState has the same size as a reference


void CSsmStatePolicyNextStateC::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmStatePolicyNextStateC::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

TBool CSsmStatePolicyNextStateC::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	(void)aReason;
   	(void)aSeverity;
   	(void)aNextState;

   	RDebug::Printf("--- CSsmStatePolicyNextStateC::GetNextState received value %d from Cle", aError);

	// We should be in NextStateC always at this point
	__ASSERT_ALWAYS(aCurrentTransition.MainState() == ESsmTestNextStateCState, User::Panic(KStateNextStateCTestPanicCategory, EStateNextStateCTestFailed1));

	// We've executed a valid command list --> we're expecting a KErrNone
	__ASSERT_ALWAYS(aError == KErrNone, User::Panic(KStateNextStateCTestPanicCategory, EStateNextStateCTestFailed2));
	
	//Signal the state transition completion
	RSemaphore sem;
	TInt err = sem.OpenGlobal(KStateTransitionComplete);
	RDebug::Printf("--- CSsmStatePolicyNextStateC::GetNextState RSemaphore OpenGlobal returned error %d", err);
	sem.Signal();
	sem.Close();
	
	return EFalse;
	}
