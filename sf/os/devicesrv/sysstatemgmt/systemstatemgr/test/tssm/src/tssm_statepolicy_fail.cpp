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


#include <ssm/ssmstatetransition.h>
#include <ssm/ssmcommandlist.h>
#include <e32debug.h>

#include "tssm_statepolicy_fail.h"

EXPORT_C MSsmStatePolicy* CSsmStatePolicyFail::NewL()
	{
	CSsmStatePolicyFail* self = new (ELeave) CSsmStatePolicyFail;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicyFail::CSsmStatePolicyFail()
	{
	}

CSsmStatePolicyFail::~CSsmStatePolicyFail()
	{
 	delete iCommandList;
	}

void CSsmStatePolicyFail::ConstructL()
	{
	}

void CSsmStatePolicyFail::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmStatePolicyFail::InitializeCancel()
	{
	}

void CSsmStatePolicyFail::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyFail::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void)aRequest;
	(void)aCurrent;
	(void)aQueued;
	(void)aMessage;

	// We're supposed to be shutting down, so we won't have a test asking for further transitions, will we?
	return ENotAllowed;
	}

void CSsmStatePolicyFail::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	(void)aState;
	(void)aReason;
	TRAPD(err, iCommandList = CSsmCommandList::NewL());
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

void CSsmStatePolicyFail::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmStatePolicyFail::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

TBool CSsmStatePolicyFail::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	(void)aCurrentTransition;
	(void)aSeverity;
	(void)aReason;
	(void)aNextState;

   	RDebug::Printf("--- CSsmStatePolicyFail::GetNextState received value %d from Cle", aError);

	// We've executed an empty command list --> we're expecting a KErrArgument
	if (aError != KErrArgument)
		{
		// That's a genuine, not expected error ==> panic!
		User::Panic(KStateFailTestPanicCategory, EStateFailTestFailed2);	
		}
	
	return EFalse;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference
