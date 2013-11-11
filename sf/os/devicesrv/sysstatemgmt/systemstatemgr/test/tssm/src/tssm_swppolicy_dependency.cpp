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
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcommandlist.h>
#include <e32debug.h>

#include "tssm_swppolicy_dependency.h"


EXPORT_C MSsmSwpPolicy* CSsmSwpPolicyDependency::NewL()
	{
	CSsmSwpPolicyDependency* self = new (ELeave) CSsmSwpPolicyDependency;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmSwpPolicyDependency::CSsmSwpPolicyDependency()
	{
	}

CSsmSwpPolicyDependency::~CSsmSwpPolicyDependency()
	{
	delete iCommandList;
	}

void CSsmSwpPolicyDependency::ConstructL()
	{
	iCommandList = CSsmCommandList::NewL();
	}

void CSsmSwpPolicyDependency::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicyDependency::InitializeCancel()
	{
	}

void CSsmSwpPolicyDependency::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicyDependency::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aSwp;
	(void)aMessage; //Security checks on originator omitted for now
	return EAllowed; 
	}

void CSsmSwpPolicyDependency::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	RDebug::Printf ("--- CSsmSwpPolicyDependency::PrepareCommandList for swp with key %d", aSwp.Key());
	TRAPD(err, doPrepareCommandListL(aSwp));
	RDebug::Printf ("--- CSsmSwpPolicyDependency::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	}

void CSsmSwpPolicyDependency::doPrepareCommandListL(TSsmSwp aSwp)
	{
	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

	// Prepare the dependant swp's
	TSsmSwp swp1(KTestDependentSwp1, aSwp.Value() + 1);
	TSsmSwp swp2(KTestDependentSwp2, aSwp.Value() + 2);

	// Prepare a valid command list with several requests to changes of dependent Swp's
	MSsmCommand* cmd1 = SsmCommandFactory::ConstructReqSwpChangeCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, swp1);
	MSsmCommand* cmd2 = SsmCommandFactory::ConstructReqSwpChangeCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, swp2);
	MSsmCommand* cmd3 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, aSwp);
	iCommandList->AppendL(cmd1);
	iCommandList->AppendL(cmd2);
	iCommandList->AppendL(cmd3);
	CleanupStack::Pop(3);
	}

void CSsmSwpPolicyDependency::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmSwpPolicyDependency::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

void CSsmSwpPolicyDependency::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;
	
   	RDebug::Printf("--- CSsmSwpPolicyDependency::HandleCleReturnValue received value %d from Cle", aError);

	// We've executed a valid command list --> we're expecting a KErrNone
	if (aError != KErrNone)
		{
		// That's a genuine, not expected error ==> panic!
		User::Panic(KSwpDependencyTestPanicCategory, ESwpDependencyTestFailed2);	
		}

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, aError);
	}

void CSsmSwpPolicyDependency::HandleCleReturnValueCancel()
	{
	}
