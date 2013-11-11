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

#include "tssm_statepolicy_bad.h"

EXPORT_C MSsmStatePolicy* CSsmStatePolicyBad::NewL()
	{
	CSsmStatePolicyBad* self = new (ELeave) CSsmStatePolicyBad;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicyBad::CSsmStatePolicyBad()
	{
	}

CSsmStatePolicyBad::~CSsmStatePolicyBad()
	{
	}

void CSsmStatePolicyBad::ConstructL()
	{
	}

void CSsmStatePolicyBad::Initialize(TRequestStatus& aStatus)
	{
	(void)aStatus;
	}

void CSsmStatePolicyBad::InitializeCancel()
	{
	}

void CSsmStatePolicyBad::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyBad::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void)aRequest;
	(void)aCurrent;
	(void)aQueued;
	(void)aMessage; //Security checks on originator omitted for now
	return ENotAllowed;
	}

void CSsmStatePolicyBad::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	(void)aState;
	(void)aReason;
	(void)aStatus;
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

void CSsmStatePolicyBad::PrepareCommandListCancel()
	{
	}

CSsmCommandList* CSsmStatePolicyBad::CommandList()
	{
	return NULL;
	}

TBool CSsmStatePolicyBad::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	(void)aCurrentTransition;
	(void)aReason;
	(void)aError;
	(void)aSeverity;
	(void)aNextState;
	return EFalse;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference
