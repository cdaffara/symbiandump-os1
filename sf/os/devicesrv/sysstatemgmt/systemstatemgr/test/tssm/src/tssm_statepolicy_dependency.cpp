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

#include "tssm_statepolicy_dependency.h"

EXPORT_C MSsmStatePolicy* CSsmStatePolicyDependency::NewL()
	{
	CSsmStatePolicyDependency* self = new (ELeave) CSsmStatePolicyDependency;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicyDependency::CSsmStatePolicyDependency()
	{
	}

CSsmStatePolicyDependency::~CSsmStatePolicyDependency()
	{
 	delete iCommandList;
	}

void CSsmStatePolicyDependency::ConstructL()
	{
	}

void CSsmStatePolicyDependency::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmStatePolicyDependency::InitializeCancel()
	{
	}

void CSsmStatePolicyDependency::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyDependency::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void) aCurrent;
	(void) aQueued;
	(void) aRequest;
	(void) aMessage; //Security checks on originator omitted for now
	
	return ENotAllowed; 
	}

void CSsmStatePolicyDependency::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	RDebug::Printf ("--- CSsmSwpPolicyDependency::PrepareCommandList for state %04x.%04x", aState.MainState(), aState.SubState());
	TRAPD(err, doPrepareCommandListL(aState, aReason));
	RDebug::Printf ("--- CSsmSwpPolicyDependency::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference


void CSsmStatePolicyDependency::doPrepareCommandListL(TSsmState aState, TInt aReason)
	{
	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

	// Prepare the dependant swp's
	TSsmSwp swp1(KTestDependentSwp3, aReason + aState.SubState());
	TSsmSwp swp2(KTestDependentSwp4, aReason - aState.SubState());

	TSsmPublishSystemStateInfo info;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
	info.Set(aState, ETraverseDefault, 0);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
	info.Set(aState, ESsmTraverseDefault, 0);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
	

	// Prepare a valid command list with several requests to changes of dependent Swp's
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	MSsmCommand* cmd1 = SsmCommandFactory::ConstructReqSwpChangeCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, swp1, KDefaultCommandPriority);
#else
	MSsmCommand* cmd1 = SsmCommandFactory::ConstructReqSwpChangeCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, swp1);
#endif
	MSsmCommand* cmd2 = SsmCommandFactory::ConstructReqSwpChangeCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, swp2);
	MSsmCommand* cmd3 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, info);
	iCommandList->AppendL(cmd1);
	iCommandList->AppendL(cmd2);
	iCommandList->AppendL(cmd3);
	CleanupStack::Pop(3);
	} //lint !e1746 suppress param 'aState' could be made const ref. Because aState has the same size as a reference

void CSsmStatePolicyDependency::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmStatePolicyDependency::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

TBool CSsmStatePolicyDependency::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	(void)aCurrentTransition;
	(void)aSeverity;
	(void)aReason;
	(void)aNextState;

   	RDebug::Printf("--- CSsmStatePolicyDependency::GetNextState received value %d from Cle", aError);

	// We've executed a valid command list --> we're expecting a KErrNone
	if (aError != KErrNone)
		{
		// That's a genuine, not expected error ==> panic!
		User::Panic(KStateDependencyTestPanicCategory, EStateDependencyTestFailed2);	
		}
	
	return EFalse;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference
