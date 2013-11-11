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


#include <ssm/ssmcommandfactory.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstartupproperties.h>
#include <ssm/ssmpublishsystemstateinfo.h>

#include "../../../ssm/inc/ssmserverpanic.h"
#include "tssm_swppolicy_invalidlist.h"


EXPORT_C MSsmSwpPolicy* CSsmSwpPolicyInvalidList::NewL()
	{
	CSsmSwpPolicyInvalidList* self = new (ELeave) CSsmSwpPolicyInvalidList;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSsmSwpPolicyInvalidList::CSsmSwpPolicyInvalidList()
	{
	}

CSsmSwpPolicyInvalidList::~CSsmSwpPolicyInvalidList()
	{
	delete iCommandList;
	}

void CSsmSwpPolicyInvalidList::ConstructL()
	{
	}

void CSsmSwpPolicyInvalidList::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicyInvalidList::InitializeCancel()
	{
	}

void CSsmSwpPolicyInvalidList::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicyInvalidList::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aSwp;
	(void)aMessage; //Security checks on originator omitted for now
	return EAllowed;
	}

void CSsmSwpPolicyInvalidList::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	RDebug::Printf("--- CSsmSwpPolicyInvalidList::PrepareCommandList for swp with key %x and value %d.", aSwp.Key(), aSwp.Value());
	TRAPD(err, doPrepareCommandListL(aSwp));
	RDebug::Printf("--- CSsmSwpPolicyInvalidList::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	}

void CSsmSwpPolicyInvalidList::doPrepareCommandListL(TSsmSwp aSwp)
	{
	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

	// Use the value to create different types of invalid swp cmd lists
	// Please note that when the list is invalid, the cmd severity is irrellevant, as it never gets executed.
	switch(aSwp.Value())
		{
		case ESwpCmdWithoutPublishSwp:
			{
			// construct some random command list, but without a Publish System-Wide Property
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructPersistHalAttributesCommandLC(ECmdIgnoreFailure);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			CleanupStack::Pop(2);
			}
			break;
		case ESwpCmdTwoPublishSwp:
			{
			// construct some random command list, but with 2 Publish System-Wide Property
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdIgnoreFailure, ESsmWaitForSignal, aSwp);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructPersistHalAttributesCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd4 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdIgnoreFailure, ESsmWaitForSignal, aSwp);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			iCommandList->AppendL(cmd3);
			iCommandList->AppendL(cmd4);
			CleanupStack::Pop(4);
			}
			break;
		case ESwpCmdPublishState:
			{
			TSsmState state(ESsmNormal, KSsmAnySubState); // any state will do..
			TSsmPublishSystemStateInfo info;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(state, ETraverseDefault, 0);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(state, ESsmTraverseDefault, 0);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
			
			

			// construct some random command list, but with a Publish System State
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdIgnoreFailure, ESsmFireAndForget, info, KDefaultCommandPriority);
#else
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdIgnoreFailure, ESsmFireAndForget, info);
#endif
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructPersistHalAttributesCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd4 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdIgnoreFailure, ESsmWaitForSignal, aSwp);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			iCommandList->AppendL(cmd3);
			iCommandList->AppendL(cmd4);
			CleanupStack::Pop(4);
			}
			break;
		case ESwpCmdNoMultipleWait:
			{
			CSsmStartupProperties* prop = CSsmStartupProperties::NewL(KTestAppGood, KNullDesC);
			prop->SetExecutionBehaviour(ESsmDeferredWaitForSignal);

			// construct some random command list, but with deferred commands and no Multiple Wait
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdIgnoreFailure, ESsmWaitForSignal, aSwp);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructStartApplicationCommandLC(ECmdIgnoreFailure, prop);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdIgnoreFailure);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			iCommandList->AppendL(cmd3);
			CleanupStack::Pop(3);
			}
			break;
		case ESwpCmdTooManyMultipleWaitErr1:
			{
			CSsmStartupProperties* prop = CSsmStartupProperties::NewL(KTestAppRvError, KNullDesC);
			prop->SetExecutionBehaviour(ESsmWaitForSignal);

			const TInt timeout = 1000; //some random timeout for the multiple_wait cmd

			// construct a command list with Multiple Wait commands but no deferred commands (this is valid)
			//    Added one command that will fail (KTestAppRvError) with a severity --> the policy should get that error back!
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, aSwp);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructStartApplicationCommandLC(ECmdLowSeverity, prop);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			CleanupStack::Pop(2);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructMultipleWaitCommandLC(timeout);
			iCommandList->AppendL(cmd3);
			CleanupStack::Pop(1);
			}
			break;
		case ESwpCmdTooManyMultipleWaitErr2:
			{
			CSsmStartupProperties* prop = CSsmStartupProperties::NewL(KTestAppPanicZero, KNullDesC);
			prop->SetExecutionBehaviour(ESsmWaitForSignal);

			const TInt timeout = 1000; //some random timeout for the multiple_wait cmd

			// construct a command list with Multiple Wait commands but no deferred commands (this is valid)
			//    Added one command that will panic --> the policy should get that error back!
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, aSwp);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructStartApplicationCommandLC(ECmdMediumSeverity, prop);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			CleanupStack::Pop(2);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructMultipleWaitCommandLC(timeout);
			iCommandList->AppendL(cmd3);
			CleanupStack::Pop(1);
			}
			break;
		case ESwpCmdPrepareCmdListFailed:
			{
			// We just want to leave to provide an aStatus with an error, to see that the engine will panic accordingly
			//  Note: Any negative value should do (we get the panic value, the leave is only recorded in debug)
			User::Leave(-654321);
			}
		default:
			{
			//That's a genuine, not expected error ==> panic!
			User::Panic(KSwpInvalidListTestPanicCategory, ESwpInvalidListTestFailed1);
			}
			break;
		}
	}

void CSsmSwpPolicyInvalidList::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmSwpPolicyInvalidList::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

void CSsmSwpPolicyInvalidList::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;

	RDebug::Printf("--- CSsmSwpPolicyInvalidList::HandleCleReturnValue received value %d from Cle", aError);

	// Both ESwpCmdTooManyMultipleWaitErr1 & ESwpCmdTooManyMultipleWaitErr2 should reach this point,
	//	as both lists are valid, although they contain erroneous individual commands which will complete
	//	with error.
	//
	// Deal with these errors here
	//    ESwpCmdTooManyMultipleWaitErr1   ---  Controlled test panic
	//	  ESwpCmdTooManyMultipleWaitErr2   ---  Leting policy frame deal with the error --> will panic too

	if (aError == KSsmTestAppRvError)
		{
		// ESwpCmdTooManyMultipleWaitErr1: This is the rendezvous value from SsmTestAppRvError
		//   Panic using the sysstatemgr panic string (avoids changing test code expecting this string)
		User::Panic(KPanicSysStateMgr, KSsmTestAppRvError);
		}
	else if (aError == KErrGeneral)
		{
		// ESwpCmdTooManyMultipleWaitErr2: This is the error returned when an app panics with error==0
		//  Note: We're completing the request with an error --> This will panic the engine at some point.
		TRequestStatus* rs = &aStatus;
		User::RequestComplete(rs, aError);
		}
	else
		{
		//That's a genuine, not expected error ==> panic!
		User::Panic(KSwpInvalidListTestPanicCategory, ESwpInvalidListTestFailed2);
		}
	}

void CSsmSwpPolicyInvalidList::HandleCleReturnValueCancel()
	{
	}
