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
#include <ssm/ssmcommandlistresourcereader.h>
#include <e32debug.h>

#include "ssmdebug.h"
#include "tssm_statepolicy_generic.h"

_LIT(KCommandListPath, "z:\\resource\\ssmatest\\state_generic\\");

EXPORT_C MSsmStatePolicy* CSsmStatePolicyGeneric::NewL()
	{
	CSsmStatePolicyGeneric* self = new (ELeave) CSsmStatePolicyGeneric;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicyGeneric::CSsmStatePolicyGeneric()
	{
	DEBUGPRINT1(_L("CSsmStatePolicyGeneric::CSsmStatePolicyGeneric() called"));
	}

CSsmStatePolicyGeneric::~CSsmStatePolicyGeneric()
	{
	DEBUGPRINT1(_L("CSsmStatePolicyGeneric::~CSsmStatePolicyGeneric() called"));
	delete iCommandListResourceReader;
	iFs.Close();
	}

void CSsmStatePolicyGeneric::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	// create resource reader
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath(), *this);
	}

void CSsmStatePolicyGeneric::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KStateGenericTestPanicCategory, EStateInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyGeneric::Initialize() called"));
		
	// initialise command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

void CSsmStatePolicyGeneric::InitializeCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KStateGenericTestPanicCategory, EStateInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyGeneric::InitializeCancel() called"));
		
	iCommandListResourceReader->InitialiseCancel();
	}

void CSsmStatePolicyGeneric::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyGeneric::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void)aRequest;
	(void)aCurrent;
	(void)aQueued;
	(void)aMessage; //Security checks on originator omitted for now
	DEBUGPRINT1(_L("CSsmStatePolicyConditionalCommands::TransitionAllowed() called"));
	if(aCurrent)
		{
		return EReplaceCurrentClearQueue;
		}
	return EDefinitelyAllowed;
	}

void CSsmStatePolicyGeneric::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	(void)aReason;
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KStateGenericTestPanicCategory, EStateInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyGeneric::PrepareCommandList() called"));
	
	//Build the commandlist from resource, it is always the command list for this substate
	iCommandListResourceReader->PrepareCommandList(aState.SubState(), aState, aStatus);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

void CSsmStatePolicyGeneric::PrepareCommandListCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KStateGenericTestPanicCategory, EStateInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyGeneric::PrepareCommandListCancel() called"));
		
	iCommandListResourceReader->PrepareCommandListCancel();
	}

CSsmCommandList* CSsmStatePolicyGeneric::CommandList()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KStateGenericTestPanicCategory, EStateInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyGeneric::CommandList() called"));
			
	return iCommandListResourceReader->GetCommandList();
	}

TBool CSsmStatePolicyGeneric::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	DEBUGPRINT2(_L("CSsmStatePolicyGeneric::GetNextState called with error: %d"), aError);
	(void)aCurrentTransition;
	(void)aReason;
	(void)aSeverity;
	(void)aError;
	// Always transition back to normal
	aNextState.Set(ESsmNormal, KSsmAnySubState);
	return ETrue;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference

TBool CSsmStatePolicyGeneric::ConditionalCommandAllowedL(CResourceFile& /*aResourceFile*/, TInt __DEBUG_ONLY(aResourceId))
	{
	DEBUGPRINT2(_L("CSsmStatePolicyGeneric::ConditionalCommandAllowed called with resource id: %d"), aResourceId);
	User::Panic(KStateGenericTestPanicCategory, EStateConditionalCommand);
	return EFalse;
	}
