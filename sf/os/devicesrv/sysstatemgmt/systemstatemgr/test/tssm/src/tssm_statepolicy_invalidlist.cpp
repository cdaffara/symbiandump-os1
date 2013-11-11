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
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmstates.hrh>
#include <ssm/ssmstatemanager.h>
#include <domainmanager.h>
#include <ssm/ssmstate.h>
#include <ssm/ssmstartupproperties.h>
#include <ssm/ssmpublishsystemstateinfo.h>
#include <ssm/ssmsupinfo.h>

#include "../../../ssm/inc/ssmserverpanic.h"
#include "tssm_statepolicy_invalidlist.h"


EXPORT_C MSsmStatePolicy* CSsmStatePolicyInvalidListTest::NewL()
	{
	CSsmStatePolicyInvalidListTest* self = new (ELeave) CSsmStatePolicyInvalidListTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSsmStatePolicyInvalidListTest::CSsmStatePolicyInvalidListTest()
	{
	}

CSsmStatePolicyInvalidListTest::~CSsmStatePolicyInvalidListTest()
	{
 	delete iCommandList;
	}

void CSsmStatePolicyInvalidListTest::ConstructL()
	{
	}

void CSsmStatePolicyInvalidListTest::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmStatePolicyInvalidListTest::InitializeCancel()
	{
	}

void CSsmStatePolicyInvalidListTest::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyInvalidListTest::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void)aRequest;
	(void)aCurrent;
	(void)aQueued;
	(void)aMessage; //Security checks on originator omitted for now

	//Don't bother much, we're testing invalid cmd lists anyway ==> Always allow transitions
	return EDefinitelyAllowed;
	}

void CSsmStatePolicyInvalidListTest::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	RDebug::Printf("--- CSsmStatePolicyInvalidListTest::PrepareCommandList for reason %d", aReason);
	TRAPD(err, doPrepareCommandListL(aState, aReason));
	RDebug::Printf("--- CSsmStatePolicyInvalidListTest::PrepareCommandList completes with code '%d'", err);

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	} //lint !e1746 suppress param 'aState' could be made const ref. Because aState has the same size as a reference

void CSsmStatePolicyInvalidListTest::doPrepareCommandListL(TSsmState aState, TInt aReason)
	{
	// reset the command list
	delete iCommandList;
	iCommandList = NULL;
	iCommandList = CSsmCommandList::NewL();

	// Use the reason to create different types of invalid states cmd lists
	// Please note that when the list is invalid, the cmd severity is irrellevant, as it never gets executed.
	switch(aReason)
		{
		case EStateCmdWithoutPublishState:
			{
			// construct some random command list, but without a Publish System State
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructPersistHalAttributesCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructFinaliseDrivesCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd4 = SsmCommandFactory::ConstructPowerOffCommandLC(ECmdIgnoreFailure, EPwActive);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			iCommandList->AppendL(cmd3);
			iCommandList->AppendL(cmd4);
			CleanupStack::Pop(4);
			}
			break;
		case EStateCmdTwoPublishState:
			{
			TSsmPublishSystemStateInfo info;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ETraverseDefault, 0);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ESsmTraverseDefault, 0);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
			
			
			TInt16 retries=5;
			_LIT(KEmptyFileName, "tempfile.txt");
			TInt32 ordinal=1;
			TUid uid;
			uid.iUid = 1;
			TSsmSupInfo supInfo;
			supInfo.SetL(KEmptyFileName,ordinal,uid);

			// construct some random command list, but with 2 Publish System State
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdIgnoreFailure, ESsmFireAndForget, info);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructPersistHalAttributesCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd4 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdIgnoreFailure, ESsmFireAndForget, info);
			MSsmCommand* cmd5 = SsmCommandFactory::ConstructLoadSupCommandLC(ECmdIgnoreFailure, ESsmFireAndForget, retries, supInfo);
			MSsmCommand* cmd6 = SsmCommandFactory::ConstructAMAStarterCommandLC(ECmdIgnoreFailure, ESsmFireAndForget,uid);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			iCommandList->AppendL(cmd3);
			iCommandList->AppendL(cmd4);
			iCommandList->AppendL(cmd5);
			iCommandList->AppendL(cmd6);
			CleanupStack::Pop(6);
			}
			break;
		case EStateCmdPublishSwp:
			{
			TSsmPublishSystemStateInfo info;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ETraverseDefault, 0);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ESsmTraverseDefault, 0);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
			
			const TUint key = {123};
			TSsmSwp swp(key, 456);

			// construct some random command list, but with a Publish System-Wide Property
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdIgnoreFailure, ESsmFireAndForget, info);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructPersistHalAttributesCommandLC(ECmdIgnoreFailure);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
			MSsmCommand* cmd4 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdIgnoreFailure, ESsmWaitForSignal, swp, KDefaultCommandPriority);
			MSsmCommand* cmd5 = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdIgnoreFailure, KDefaultCommandPriority);
#else			
			MSsmCommand* cmd4 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdIgnoreFailure, ESsmWaitForSignal, swp);
#endif			
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			iCommandList->AppendL(cmd3);
			iCommandList->AppendL(cmd4);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
			iCommandList->AppendL(cmd5);
			CleanupStack::Pop(5);
#else
			CleanupStack::Pop(4);
#endif
			}
			break;
		case EStateCmdNoMultipleWait:
			{
			TSsmPublishSystemStateInfo info;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ETraverseDefault, 0);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ESsmTraverseDefault, 0);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

			CSsmStartupProperties* prop = CSsmStartupProperties::NewL(KTestAppGood, KNullDesC);
			prop->SetExecutionBehaviour(ESsmDeferredWaitForSignal);

			// construct some random command list, but with deferred commands and no Multiple Wait
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdIgnoreFailure, ESsmWaitForSignal, info);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructStartApplicationCommandLC(ECmdIgnoreFailure, prop);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdIgnoreFailure);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
			MSsmCommand* cmd4 = SsmCommandFactory::ConstructStartApplicationCommandLC(ECmdIgnoreFailure, prop);
			MSsmCommand* cmd5 = SsmCommandFactory::ConstructStartProcessCommandLC(ECmdIgnoreFailure, prop);
			MSsmCommand* cmd6 = SsmCommandFactory::ConstructStartProcessCommandLC(ECmdIgnoreFailure, prop, KDefaultCommandPriority);
			MSsmCommand* cmd7 = SsmCommandFactory::ConstructStartApplicationCommandLC(ECmdIgnoreFailure, prop, KDefaultCommandPriority);
			MSsmCommand* cmd8 = SsmCommandFactory::ConstructFinaliseDrivesCommandLC(ECmdIgnoreFailure);
			MSsmCommand* cmd9 = SsmCommandFactory::ConstructPowerOffCommandLC(ECmdIgnoreFailure, EPwActive);
#endif
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			iCommandList->AppendL(cmd3);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
			iCommandList->AppendL(cmd4);
			iCommandList->AppendL(cmd5);
			iCommandList->AppendL(cmd6);
			iCommandList->AppendL(cmd7);
			iCommandList->AppendL(cmd8);
			iCommandList->AppendL(cmd9);
			CleanupStack::Pop(9);
#else
			CleanupStack::Pop(3);
#endif
			}
			break;
		case EStateCmdTooManyMultipleWaitErr1:
			{
			TSsmPublishSystemStateInfo info;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ETraverseDefault, 0);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ESsmTraverseDefault, 0);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
			
			CSsmStartupProperties* prop = CSsmStartupProperties::NewL(KTestAppRvError, KNullDesC);
			prop->SetExecutionBehaviour(ESsmWaitForSignal);

			const TInt timeout = 1000; //some random timeout for the multiple_wait cmd

			// construct a command list with Multiple Wait commands but no deferred commands (this is valid)
			//    Added one command that will fail (KTestAppRvError) with a severity --> the policy should get that error back!
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, info);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructStartApplicationCommandLC(ECmdLowSeverity, prop);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			CleanupStack::Pop(2);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructMultipleWaitCommandLC(timeout);
			iCommandList->AppendL(cmd3);
			CleanupStack::Pop(1);
			}
			break;
		case EStateCmdTooManyMultipleWaitErr2:
			{
			TSsmPublishSystemStateInfo info;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ETraverseDefault, 0);
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
			info.Set(aState, ESsmTraverseDefault, 0);
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
			
			CSsmStartupProperties* prop = CSsmStartupProperties::NewL(KTestAppPanicZero, KNullDesC);
			prop->SetExecutionBehaviour(ESsmWaitForSignal);

			const TInt timeout = 1000; //some random timeout for the multiple_wait cmd

			// construct a command list with Multiple Wait commands but no deferred commands (this is valid)
			//    Added one command that will panic --> the policy should get that error back!
			MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSystemStateCommandLC(ECmdCriticalSeverity, ESsmWaitForSignal, info);
			MSsmCommand* cmd2 = SsmCommandFactory::ConstructStartApplicationCommandLC(ECmdMediumSeverity, prop);
			iCommandList->AppendL(cmd1);
			iCommandList->AppendL(cmd2);
			CleanupStack::Pop(2);
			MSsmCommand* cmd3 = SsmCommandFactory::ConstructMultipleWaitCommandLC(timeout);
			iCommandList->AppendL(cmd3);
			CleanupStack::Pop(1);
			}
			break;
		case EStateCmdPrepareCmdListFailed:
			{
			// We just want to leave to provide an aStatus with an error, to see that the engine will panic accordingly
			//  Note: Any negative value should do (we get the panic value, the leave is only recorded in debug)
			User::Leave(-123456);
			}
			break;
		default:
			{
			//That's a genuine, not expected error ==> panic!
			User::Panic(KStateInvalidListTestPanicCategory, EStateInvalidListTestFailed1);
			}
			break;
		}
	} //lint !e1746 suppress param 'aState' could be made const ref. Because aState has the same size as a reference

void CSsmStatePolicyInvalidListTest::PrepareCommandListCancel()
	{
 	delete iCommandList;
 	iCommandList = NULL;
	}

CSsmCommandList* CSsmStatePolicyInvalidListTest::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

TBool CSsmStatePolicyInvalidListTest::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
   	(void)aCurrentTransition;
   	(void)aReason;
   	(void)aSeverity;

   	RDebug::Printf("--- CSsmStatePolicyInvalidListTest::GetNextState received value %d from Cle", aError);

	// Both EStateCmdTooManyMultipleWaitErr1 & EStateCmdTooManyMultipleWaitErr2 should reach this point,
	//	as both lists are valid, although they contain erroneous individual commands which will complete
	//	with error.
	//
	// Deal with these errors here
	//    EStateCmdTooManyMultipleWaitErr1   ---  Controlled test panic
	//	  EStateCmdTooManyMultipleWaitErr2   ---  Leting policy frame deal with the error --> will panic too

	if (aError == KSsmTestAppRvError)
		{
		// EStateCmdTooManyMultipleWaitErr1: This is the rendezvous value from SsmTestAppRvError
		//   Panic using the sysstatemgr panic string (avoids changing test code expecting this string)
		User::Panic(KPanicSysStateMgr, KSsmTestAppRvError);
		}
	else if (aError == KErrGeneral)
		{
		// EStateCmdTooManyMultipleWaitErr2: This is the error returned when an app panics with error==0
		//  Note: We're returning an unkonwn state --> This will panic the engine at some point.
		aNextState.Set(0x00FF, 0xFFFF);
		return ETrue;
		}
	else
		{
		//That's a genuine, not expected error ==> panic!
		User::Panic(KStateInvalidListTestPanicCategory, EStateInvalidListTestFailed2);
		}

	return EFalse;
	}

