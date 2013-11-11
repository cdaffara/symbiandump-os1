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
#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/conditionevaluate.h>

#include <e32property.h>
#include <s32mem.h> 

#include "ssmdebug.h"
#include "tssm_statepolicy_conditionalcommands.h"
#include "tssm_statepolicy_conditionalcommands.hrh"

_LIT(KLocalPanicCategory, "Test CC Policy");

enum TLocalPanicCodes
{
	EInvalidResourceReader,
	ECleReturnedError,
};

_LIT(KCommandListPath, "z:\\resource\\ssmatest\\conditionalcommands\\");

EXPORT_C MSsmStatePolicy* CSsmStatePolicyConditionalCommands::NewL()
	{
	CSsmStatePolicyConditionalCommands* self = new (ELeave) CSsmStatePolicyConditionalCommands;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmStatePolicyConditionalCommands::CSsmStatePolicyConditionalCommands()
	{
	DEBUGPRINT1(_L("CSsmStatePolicyConditionalCommands::CSsmStatePolicyConditionalCommands() called"));
	}

CSsmStatePolicyConditionalCommands::~CSsmStatePolicyConditionalCommands()
	{
	DEBUGPRINT1(_L("CSsmStatePolicyConditionalCommands::~CSsmStatePolicyConditionalCommands() called"));
	iFs.Close();
	delete iCommandListResourceReader;
	}

void CSsmStatePolicyConditionalCommands::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	// create resource reader
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath(), *this);
	}

void CSsmStatePolicyConditionalCommands::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KLocalPanicCategory, EInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyConditionalCommands::Initialize() called"));
	
	// initialise command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

void CSsmStatePolicyConditionalCommands::InitializeCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KLocalPanicCategory, EInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyConditionalCommands::InitializeCancel() called"));
	
	iCommandListResourceReader->InitialiseCancel();
	}

void CSsmStatePolicyConditionalCommands::Release()
	{
	delete this;
	}

MSsmStatePolicy::TResponse CSsmStatePolicyConditionalCommands::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	(void)aRequest;
	(void)aCurrent;
	(void)aQueued;
	(void)aMessage; //Security checks on originator omitted for now
	DEBUGPRINT1(_L("CSsmStatePolicyConditionalCommands::TransitionAllowed() called"));
	return EDefinitelyAllowed;
	}

void CSsmStatePolicyConditionalCommands::PrepareCommandList(TSsmState aState, TInt /*aReason*/, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KLocalPanicCategory, EInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyConditionalCommands::PrepareCommandList() called"));
	
	//Build the commandlist from resource, it is always the same command list
	iCommandListResourceReader->PrepareCommandList(EConditionalCommandsList1, aState, aStatus);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

void CSsmStatePolicyConditionalCommands::PrepareCommandListCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KLocalPanicCategory, EInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyConditionalCommands::PrepareCommandListCancel() called"));
	
	iCommandListResourceReader->PrepareCommandListCancel();
	}

CSsmCommandList* CSsmStatePolicyConditionalCommands::CommandList()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KLocalPanicCategory, EInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmStatePolicyConditionalCommands::CommandList() called"));
	
	return iCommandListResourceReader->GetCommandList();
	}

TBool CSsmStatePolicyConditionalCommands::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	DEBUGPRINT2(_L("CSsmStatePolicyConditionalCommands::GetNextState called with error: %d"), aError);
	__ASSERT_ALWAYS(aError != KErrNone,	User::Panic(KLocalPanicCategory, ECleReturnedError));
	(void)aCurrentTransition;
	(void)aReason;
	(void)aSeverity;
	// Always transition back to normal
	aNextState.Set(ESsmNormal, KSsmAnySubState);
	return ETrue;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference

TBool CSsmStatePolicyConditionalCommands::ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId)
	{
	DEBUGPRINT2(_L("CSsmStatePolicyConditionalCommands::ConditionalCommandAllowedL called with resource id: %d"), aResourceId);
	RDebug::Printf("CSsmStatePolicyConditionalCommands::ConditionalCommandAllowedL called");
	CConditionEvaluate* evaluate = CConditionEvaluate::NewLC();
	TInt value = evaluate->EvaluateL(aResourceFile,aResourceId);	
	TBool ret = value == 1 ? ETrue : EFalse;
	CleanupStack::PopAndDestroy(evaluate);
	return ret;
	}
