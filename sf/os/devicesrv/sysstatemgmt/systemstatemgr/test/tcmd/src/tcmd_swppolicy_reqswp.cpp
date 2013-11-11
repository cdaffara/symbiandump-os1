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
// tcmd_swppolicy_reqswp.h
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <ssm/ssmcommandfactory.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmswp.h>
#include "e32debug.h"
#include "tcmd_swppolicy_reqswp.h"

EXPORT_C MSsmSwpPolicy* CSsmSwpPolicyTestReqSwp::NewL()
	{
	CSsmSwpPolicyTestReqSwp* self = new (ELeave) CSsmSwpPolicyTestReqSwp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmSwpPolicyTestReqSwp::CSsmSwpPolicyTestReqSwp()
	{
	}

CSsmSwpPolicyTestReqSwp::~CSsmSwpPolicyTestReqSwp()
	{
	delete iCommandList;
	}

void CSsmSwpPolicyTestReqSwp::Release()
	{
	delete this;
	}

void CSsmSwpPolicyTestReqSwp::ConstructL()
	{
	}

void CSsmSwpPolicyTestReqSwp::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicyTestReqSwp::InitializeCancel()
	{
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicyTestReqSwp::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aSwp;
	(void)aMessage;
	return EAllowed;
	}

void CSsmSwpPolicyTestReqSwp::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	TInt err = KErrNone;
	if(aSwp.Key())
		{
		TRAP(err, doPrepareCommandListL(aSwp));
		}
	else
		{
		User::Panic(_L("SwpCmdListTest"), KSwpPolicyReqSwpTest);
		}
		
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
		
	}

void CSsmSwpPolicyTestReqSwp::PrepareCommandListCancel()
	{
	delete iCommandList;
	iCommandList = NULL;
	}

CSsmCommandList* CSsmSwpPolicyTestReqSwp::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

void CSsmSwpPolicyTestReqSwp::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;
	RDebug::Printf("--- CSsmSwpPolicyTestReqSwp::HandleCleReturnValue received value %d from Cle", aError);
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, aError);
	}

void CSsmSwpPolicyTestReqSwp::HandleCleReturnValueCancel()
	{
	}

void CSsmSwpPolicyTestReqSwp::doPrepareCommandListL(const TSsmSwp& aSwp)
	{
	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

	MSsmCommand* cmd = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, aSwp);
	iCommandList->AppendL(cmd);
	CleanupStack::Pop();
	}
