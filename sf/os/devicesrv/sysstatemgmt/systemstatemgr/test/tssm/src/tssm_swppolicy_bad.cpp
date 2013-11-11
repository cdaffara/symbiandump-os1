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


#include <ssm/ssmswppolicy.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcommandlist.h>

#include "tssm_swppolicy_bad.h"


EXPORT_C MSsmSwpPolicy* CSsmSwpPolicyBad::NewL()
	{
	CSsmSwpPolicyBad* self = new (ELeave) CSsmSwpPolicyBad;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmSwpPolicyBad::CSsmSwpPolicyBad()
	{
	}

CSsmSwpPolicyBad::~CSsmSwpPolicyBad()
	{
	}

void CSsmSwpPolicyBad::ConstructL()
	{
	}

void CSsmSwpPolicyBad::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicyBad::InitializeCancel()
	{
	}

void CSsmSwpPolicyBad::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicyBad::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aSwp;
	(void)aMessage; //Security checks on originator omitted for now
	return EAllowed;
	}

void CSsmSwpPolicyBad::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	(void)aSwp;

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicyBad::PrepareCommandListCancel()
	{
	}

CSsmCommandList* CSsmSwpPolicyBad::CommandList()
	{
	return NULL;
	}

void CSsmSwpPolicyBad::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, aError);
	}

void CSsmSwpPolicyBad::HandleCleReturnValueCancel()
	{
	}
