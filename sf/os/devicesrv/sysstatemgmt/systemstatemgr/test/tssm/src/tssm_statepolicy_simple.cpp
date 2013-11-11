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
#include <ssm/ssmcommandlist.h>
#include <e32debug.h>

#include "tssm_statepolicy_fail.h"
#include "tssm_statepolicy_simple.h"

EXPORT_C MSsmStatePolicy* CSsmStatePolicySimple::NewL()
	{
	CSsmStatePolicySimple* self = new (ELeave) CSsmStatePolicySimple;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicySimple::CSsmStatePolicySimple()
	{
	}

CSsmStatePolicySimple::~CSsmStatePolicySimple()
	{
 	delete iCommandList;
	}

void CSsmStatePolicySimple::ConstructL()
	{
	}

void CSsmStatePolicySimple::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmStatePolicySimple::InitializeCancel()
	{
	}

void CSsmStatePolicySimple::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicySimple::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void)aMessage; //Security checks on originator omitted for now
	if(!aCurrent && !aQueued)
		{
		return EDefinitelyAllowed;
		}
	else if(aRequest.State().MainState() == ESsmTestFailState)
		{
		return EReplaceCurrentClearQueue;
		}
	else if (aRequest.Reason() == KTestNotAllowedReason)
		{
		return ENotAllowed;
		}
	
	return ECurrentRemainReplaceQueued;
	}

void CSsmStatePolicySimple::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	(void)aState;
	(void)aReason;
	TRAPD(err, iCommandList = CSsmCommandList::NewL());
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

void CSsmStatePolicySimple::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmStatePolicySimple::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

TBool CSsmStatePolicySimple::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	(void)aCurrentTransition;
	(void)aReason;
	(void)aSeverity;
	(void)aNextState;

   	RDebug::Printf("--- CSsmStatePolicySimple::GetNextState received value %d from Cle", aError);

	// We've executed an empty command list --> we're expecting a KErrArgument
	if (aError != KErrArgument)
		{
		// That's a genuine, not expected error ==> panic!
		User::Panic(KStateSimpleTestPanicCategory, EStateSimpleTestFailed2);	
		}
	
	return EFalse;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference
