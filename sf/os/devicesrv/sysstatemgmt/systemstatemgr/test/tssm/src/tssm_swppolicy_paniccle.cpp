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
#include <ssm/ssmcustomcommandinfo.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcommandlist.h>
#include <e32debug.h>

#include "tssm_swppolicy_paniccle.h"


EXPORT_C MSsmSwpPolicy* CSsmSwpPolicyPanicCle::NewL()
	{
	CSsmSwpPolicyPanicCle* self = new (ELeave) CSsmSwpPolicyPanicCle;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmSwpPolicyPanicCle::CSsmSwpPolicyPanicCle()
	{
	}

CSsmSwpPolicyPanicCle::~CSsmSwpPolicyPanicCle()
	{
	delete iCommandList;
	}

void CSsmSwpPolicyPanicCle::ConstructL()
	{
	iCommandList = CSsmCommandList::NewL();
	}

void CSsmSwpPolicyPanicCle::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicyPanicCle::InitializeCancel()
	{
	}

void CSsmSwpPolicyPanicCle::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicyPanicCle::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aSwp;
	(void)aMessage; //Security checks on originator omitted for now
	return EAllowed; 
	}

void CSsmSwpPolicyPanicCle::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	RDebug::Printf ("--- CSsmSwpPolicyPanicCle::PrepareCommandList for swp with key %d", aSwp.Key());
	TRAPD(err, doPrepareCommandListL(aSwp));
	RDebug::Printf ("--- CSsmSwpPolicyPanicCle::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	}

void CSsmSwpPolicyPanicCle::doPrepareCommandListL(TSsmSwp aSwp)
	{
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

	// Make sure that the publish swp command is the first one in the command list (Symbian's recomendation, but not important here)
	MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, aSwp);
	MSsmCommand* cmd2 = SsmCommandFactory::ConstructCustomCommandCommandLC(ECmdIgnoreFailure, ESsmWaitForSignal, *customInfo);
	iCommandList->AppendL(cmd1);
	iCommandList->AppendL(cmd2);
	CleanupStack::Pop(2);
	
	// clean-up
	CleanupStack::PopAndDestroy(customInfo);
	}

void CSsmSwpPolicyPanicCle::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmSwpPolicyPanicCle::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

void CSsmSwpPolicyPanicCle::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;

   	RDebug::Printf("--- CSsmSwpPolicyPanicCle::HandleCleReturnValue received value %d from Cle", aError);

	// We've executed a command list that will panic the Cle --> we're expecting a KErrServerTerminated
	if (aError != KErrServerTerminated)
		{
		// That's a genuine, not expected error ==> panic!
		User::Panic(KSwpPanicCleTestPanicCategory, ESwpPanicCleTestFailed2);	
		}

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicyPanicCle::HandleCleReturnValueCancel()
	{
	}
