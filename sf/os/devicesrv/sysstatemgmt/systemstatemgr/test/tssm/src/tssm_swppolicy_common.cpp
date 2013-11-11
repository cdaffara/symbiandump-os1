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

#include "tssm_swppolicy_common.h"


EXPORT_C MSsmSwpPolicy* CSsmSwpPolicyCommon::NewL()
	{
	CSsmSwpPolicyCommon* self = new (ELeave) CSsmSwpPolicyCommon;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmSwpPolicyCommon::CSsmSwpPolicyCommon()
	{
	}

CSsmSwpPolicyCommon::~CSsmSwpPolicyCommon()
	{
	delete iCommandList;
	}

void CSsmSwpPolicyCommon::ConstructL()
	{
	iCommandList = CSsmCommandList::NewL();
	}

void CSsmSwpPolicyCommon::Initialize(TRequestStatus& aStatus)
	{
	// Artificial delay, only used to postpone completition of a request so we can cancel it while still running.
	User::After(10000);
	
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicyCommon::InitializeCancel()
	{
	}

void CSsmSwpPolicyCommon::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicyCommon::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aSwp;
	(void)aMessage; //Security checks on originator omitted for now
	return EAllowed; 
	}

void CSsmSwpPolicyCommon::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	RDebug::Printf ("--- CSsmSwpPolicyCommon::PrepareCommandList for swp with key %d", aSwp.Key());
	TRAPD(err, doPrepareCommandListL(aSwp));
	RDebug::Printf ("--- CSsmSwpPolicyCommon::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	}

void CSsmSwpPolicyCommon::doPrepareCommandListL(TSsmSwp aSwp)
	{
	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();
	
	RDebug::Printf ("--- CSsmSwpPolicyCommon::doPrepareCommandListL aSwp Value %d", aSwp.Value());
	if (aSwp.Value() == 100)
		{
		// We are panicking here, in order to panic the ssmswppolicyserver.
		// when ssmswppolicyserver is panicked the remaining swp's will not the changed and published
		RDebug::Printf ("--- Panic in CSsmSwpPolicyCommon::doPrepareCommandListL");
		User::Panic(KSwpCommonTestPanicCategory, ESwpCommonTestFailed3);
		}

	// Use the value to create a simple valid swp cmd list with just a publish swp command. 
	MSsmCommand* cmd = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, aSwp);
	iCommandList->AppendL(cmd);
	CleanupStack::Pop(cmd);
	}

void CSsmSwpPolicyCommon::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmSwpPolicyCommon::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

void CSsmSwpPolicyCommon::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;
	
	RDebug::Printf ("--- CSsmSwpPolicyCommon::HandleCleReturnValue gets aError '%d'", aError);

	// We've executed a valid command list --> we're expecting a KErrNone
	// In case of testing the RSsmSwpPolicySession class methods, we are intentionally
	// passing an KTestRSsmSwpPolicySessionErr even after successful execution of the commandlist
	// inorder to test errorneous behavior of HandleCleReturnValue
	if(aError == KErrNone || aError == KTestRSsmSwpPolicySessionErr)
		{
		TRequestStatus* rs = &aStatus;
		User::RequestComplete(rs, aError);
		}
	else
		{
		// That's a genuine, not expected error ==> panic!
		User::Panic(KSwpCommonTestPanicCategory, ESwpCommonTestFailed2);	
		}
	}

void CSsmSwpPolicyCommon::HandleCleReturnValueCancel()
	{
	}
