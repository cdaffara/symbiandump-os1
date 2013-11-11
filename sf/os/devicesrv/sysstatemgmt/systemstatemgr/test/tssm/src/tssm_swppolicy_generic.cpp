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
#include <ssm/ssmcommandlistresourcereader.h>

#include <e32debug.h>

#include "ssmdebug.h"
#include "tssm_swppolicy_generic.h"
#include "tssm_swppolicy_generic.hrh"

_LIT(KCommandListPath, "z:\\resource\\ssmatest\\swp_generic\\");


EXPORT_C MSsmSwpPolicy* CSsmSwpPolicyGeneric::NewL()
	{
	CSsmSwpPolicyGeneric* self = new (ELeave) CSsmSwpPolicyGeneric;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CSsmSwpPolicyGeneric::CSsmSwpPolicyGeneric()
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyGeneric::CSsmSwpPolicyGeneric() called"));
	}

CSsmSwpPolicyGeneric::~CSsmSwpPolicyGeneric()
	{
	DEBUGPRINT1(_L("CSsmSwpPolicyGeneric::~CSsmSwpPolicyGeneric() called"));
	iFs.Close();
	delete iCommandListResourceReader;
	}

void CSsmSwpPolicyGeneric::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	// create resource reader
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath(), *this);
	}

void CSsmSwpPolicyGeneric::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpGenericTestPanicCategory, ESwpInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmSwpPolicyGeneric::Initialize() called"));
		
	// initialise command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

void CSsmSwpPolicyGeneric::InitializeCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpGenericTestPanicCategory, ESwpInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmSwpPolicyGeneric::InitializeCancel() called"));
		
	iCommandListResourceReader->InitialiseCancel();
	}

void CSsmSwpPolicyGeneric::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CSsmSwpPolicyGeneric::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aMessage; //Security checks on originator omitted for now
	TResponse response = EAllowed;
	if (ESwpGenericBadValueNotAllowed == aSwp.Value()) // bad
		{
		response = ENotAllowed;
		}
	DEBUGPRINT1(_L("CSsmSwpPolicyGeneric::TransitionAllowed() called"));
	return response;
	}

void CSsmSwpPolicyGeneric::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpGenericTestPanicCategory, ESwpInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmSwpPolicyGeneric::PrepareCommandList() called"));
	
	//Build the commandlist from resource, it is always the command list for this value
	iCommandListResourceReader->PrepareCommandList(aSwp.Value(), aSwp, aStatus);
	}

void CSsmSwpPolicyGeneric::PrepareCommandListCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpGenericTestPanicCategory, ESwpInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmSwpPolicyGeneric::PrepareCommandListCancel() called"));
	
	iCommandListResourceReader->PrepareCommandListCancel();
	}

CSsmCommandList* CSsmSwpPolicyGeneric::CommandList()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpGenericTestPanicCategory, ESwpInvalidResourceReader));
	DEBUGPRINT1(_L("CSsmSwpPolicyGeneric::CommandList() called"));
		
	return iCommandListResourceReader->GetCommandList();
	}

void CSsmSwpPolicyGeneric::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;
	(void)aError;
	DEBUGPRINT1(_L("CSsmSwpPolicyGeneric::HandleCleReturnValue() called"));
	// Always complete with success
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CSsmSwpPolicyGeneric::HandleCleReturnValueCancel()
	{
	// Nothing to do as HandleCleReturnValue always completes immediately
	}

TBool CSsmSwpPolicyGeneric::ConditionalCommandAllowedL(CResourceFile& /*aResourceFile*/, TInt __DEBUG_ONLY(aResourceId))
	{
	DEBUGPRINT2(_L("CSsmSwpPolicyGeneric::ConditionalCommandAllowed called with resource id: %d"), aResourceId);
	User::Panic(KSwpGenericTestPanicCategory, ESwpConditionalCommand);
	return EFalse;
	}
