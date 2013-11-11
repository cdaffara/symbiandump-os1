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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <ssm/ssmstatetransition.h>
#include <ssm/ssmcommandfactory.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmswp.h>

#include "tcmd_statepolicy_createswp.h"

EXPORT_C MSsmStatePolicy* CSsmStatePolicyTestCreateSwp::NewL()
	{
	CSsmStatePolicyTestCreateSwp* self = new (ELeave) CSsmStatePolicyTestCreateSwp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicyTestCreateSwp::CSsmStatePolicyTestCreateSwp()
	{
	}

CSsmStatePolicyTestCreateSwp::~CSsmStatePolicyTestCreateSwp()
	{
 	delete iCommandList;
	}

void CSsmStatePolicyTestCreateSwp::ConstructL()
	{
	}

void CSsmStatePolicyTestCreateSwp::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmStatePolicyTestCreateSwp::InitializeCancel()
	{
	}

void CSsmStatePolicyTestCreateSwp::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyTestCreateSwp::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void)aMessage; //Security checks on originator omitted for now
	if(!aCurrent && !aQueued)
		{
		return EDefinitelyAllowed;
		}
	else if(aRequest.State().MainState() == ESsmFailTest)
		{
		return EReplaceCurrentClearQueue;
		}
	return ECurrentRemainReplaceQueued;
	}

void CSsmStatePolicyTestCreateSwp::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	(void)aState;
	(void)aReason;
	TRAPD(err, doPrepareCommandListL());
  		
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

void CSsmStatePolicyTestCreateSwp::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmStatePolicyTestCreateSwp::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

TBool CSsmStatePolicyTestCreateSwp::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	(void)aCurrentTransition;
	(void)aReason;
	(void)aError;
	(void)aSeverity;
	(void)aNextState;
	return EFalse;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference

void CSsmStatePolicyTestCreateSwp::doPrepareCommandListL()
	{
	iCommandList = CSsmCommandList::NewL();
	
	_LIT(KPolicyFilename, "ssm.swp.policy.test.cmdreqswp");
	TCmdErrorSeverity severity = ECmdHighSeverity;
	const TUint KSwpKey1 = 885;
	const TUint KSwpKey2 = 886;
	const TUint KSwpKey3 = 887;
	const TUint KSwpKey4 = 888;
	const TUint KSwpKey5 = 889;
  	TPtrC filename = KPolicyFilename();
  
	TSsmSwp aSwpInfo(KSwpKey1, 1);
  	MSsmCommand* cmd = SsmCommandFactory::ConstructCreateSwpCommandLC(severity, aSwpInfo, filename);
  	iCommandList->AppendL(cmd);
  	CleanupStack::Pop();

	aSwpInfo.Set(KSwpKey2, 1);
	cmd = SsmCommandFactory::ConstructCreateSwpCommandLC(severity, aSwpInfo, filename);
	iCommandList->AppendL(cmd);
	CleanupStack::Pop();

	aSwpInfo.Set(KSwpKey3, 1);
	cmd = SsmCommandFactory::ConstructCreateSwpCommandLC(severity, aSwpInfo, filename);
	iCommandList->AppendL(cmd);
	CleanupStack::Pop();

	aSwpInfo.Set(KSwpKey4, 1);
	cmd = SsmCommandFactory::ConstructCreateSwpCommandLC(severity, aSwpInfo, filename);
	iCommandList->AppendL(cmd);
	CleanupStack::Pop();

	aSwpInfo.Set(KSwpKey5, 1);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	cmd = SsmCommandFactory::ConstructCreateSwpCommandLC(severity, aSwpInfo, filename, KDefaultCommandPriority);
#else
	cmd = SsmCommandFactory::ConstructCreateSwpCommandLC(severity, aSwpInfo, filename);
#endif
	iCommandList->AppendL(cmd);
	CleanupStack::Pop();
	}

