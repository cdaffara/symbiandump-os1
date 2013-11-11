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



#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcommandlist.h>

#include "tsus_swppolicy_sustest.h"


EXPORT_C MSsmSwpPolicy* CSsmSwpPolicyTest::NewL()
	{
	CSsmSwpPolicyTest* self = new (ELeave) CSsmSwpPolicyTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmSwpPolicyTest::CSsmSwpPolicyTest()
	{
	}

CSsmSwpPolicyTest::~CSsmSwpPolicyTest()
	{
	delete iCommandList;
	delete iCommandListResourceReader;
	iFs.Close();
	}

void CSsmSwpPolicyTest::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());

	_LIT(KCommandListPath, "z:\\private\\2000d75b\\policy.00000\\");
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath, *this);
	}
	
TBool CSsmSwpPolicyTest::ConditionalCommandAllowedL(CResourceFile& /*aResourceFile*/, TInt /*aResourceId*/)
	{
	return ETrue;
	}

void CSsmSwpPolicyTest::Initialize(TRequestStatus& aStatus)
	{
	iCommandListResourceReader->Initialise(aStatus);
	}

void CSsmSwpPolicyTest::InitializeCancel()
	{
	iCommandListResourceReader->InitialiseCancel();
	}

void CSsmSwpPolicyTest::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicyTest::TransitionAllowed(const TSsmSwp& /*aSwp*/, const RMessagePtr2& /*aMessage*/)
	{
	return EAllowed;
	}

void CSsmSwpPolicyTest::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	TInt commandListId=0;	
	iCommandListResourceReader->PrepareCommandList(commandListId, aSwp, aStatus);	
	} 

void CSsmSwpPolicyTest::PrepareCommandListCancel()
	{
	iCommandListResourceReader->PrepareCommandListCancel();
	}

CSsmCommandList* CSsmSwpPolicyTest::CommandList()
	{
	return iCommandListResourceReader->GetCommandList();
	}

void CSsmSwpPolicyTest::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;
	
	// If aError contains any error, this will panic in the RunError of the swp transition engine
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, aError);
	}
	
void CSsmSwpPolicyTest::HandleCleReturnValueCancel()
	{
	}
