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


#include <ssm/ssmswppolicy.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcommandlist.h>
#include <e32debug.h>

#include "tssm_swppolicy_simple.h"


EXPORT_C MSsmSwpPolicy* CSsmSwpPolicySimple::NewL()
	{
	CSsmSwpPolicySimple* self = new (ELeave) CSsmSwpPolicySimple;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmSwpPolicySimple::CSsmSwpPolicySimple()
	{
	}

CSsmSwpPolicySimple::~CSsmSwpPolicySimple()
	{
	delete iCommandList;
	}

void CSsmSwpPolicySimple::ConstructL()
	{
	}

void CSsmSwpPolicySimple::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicySimple::InitializeCancel()
	{
	}

void CSsmSwpPolicySimple::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicySimple::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aMessage; //Security checks on originator omitted for now
	if(aSwp.Value() == KTestNotAllowedValue)
		{
		return ENotAllowed;
		}
	return EAllowed; 
	}

void CSsmSwpPolicySimple::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	RDebug::Printf ("--- CSsmSwpPolicySimple::PrepareCommandList for swp with key %d", aSwp.Key());
	TRAPD(err, iCommandList = CSsmCommandList::NewL());
	RDebug::Printf ("--- CSsmSwpPolicySimple::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
		
	}

void CSsmSwpPolicySimple::PrepareCommandListCancel()
	{
	delete iCommandList;
	iCommandList = NULL;
	}

CSsmCommandList* CSsmSwpPolicySimple::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

void CSsmSwpPolicySimple::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;

	RDebug::Printf ("--- CSsmSwpPolicySimple::HandleCleReturnValue gets aReturnValue '%d'", aError);

	// We've executed an empty command list --> we're expecting a KErrArgument
	if (aError != KErrArgument)
		{
		// That's a genuine, not expected error ==> panic!
		User::Panic(KSwpSimpleTestPanicCategory, ESwpSimpleTestFailed2);	
		}

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);

	// This semphore is used to signal the completion of the swp request change.
	RSemaphore sem;
	TInt err = sem.OpenGlobal(KRequestCompleteSem);
	if(err == KErrNone)
		{
		sem.Signal();
		sem.Close();
		}
	}

void CSsmSwpPolicySimple::HandleCleReturnValueCancel()
	{
	}
