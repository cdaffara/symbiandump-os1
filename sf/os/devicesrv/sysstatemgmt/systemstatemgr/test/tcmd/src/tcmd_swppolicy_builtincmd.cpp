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

#include "tcmd_swppolicy_builtincmd.h"

_LIT(KCommandListPath, "z:\\resource\\ssmatest\\bic\\");


EXPORT_C MSsmSwpPolicy* CTestSsmSwpBuiltInCmd::NewL()
	{
	CTestSsmSwpBuiltInCmd* self = new (ELeave) CTestSsmSwpBuiltInCmd;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CTestSsmSwpBuiltInCmd::CTestSsmSwpBuiltInCmd()
	{
	RDebug::Printf("CTestSsmSwpBuiltInCmd::CTestSsmSwpBuiltInCmd() called");
	}

CTestSsmSwpBuiltInCmd::~CTestSsmSwpBuiltInCmd()
	{
	RDebug::Printf("CTestSsmSwpBuiltInCmd::~CTestSsmSwpBuiltInCmd() called");
	iFs.Close();
	delete iCommandListResourceReader;
	}

void CTestSsmSwpBuiltInCmd::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	// create resource reader
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath(), *this);
	}

void CTestSsmSwpBuiltInCmd::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KTestSwpBuiltInCmdPolicyFile, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpBuiltInCmd::Initialize() called");
		
	// initialise command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

void CTestSsmSwpBuiltInCmd::InitializeCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpBuiltInCmdestCategory, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpBuiltInCmd::InitializeCancel() called");
		
	iCommandListResourceReader->InitialiseCancel();
	}

void CTestSsmSwpBuiltInCmd::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CTestSsmSwpBuiltInCmd::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aSwp;
	(void)aMessage; //Security checks on originator omitted for now
	RDebug::Printf("CTestSsmSwpBuiltInCmd::TransitionAllowed() called");
	return EAllowed; 
	}

void CTestSsmSwpBuiltInCmd::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpBuiltInCmdestCategory, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpBuiltInCmd::PrepareCommandList() called");
	
	//Build the commandlist from resource, it is always the command list for this value
	iCommandListResourceReader->PrepareCommandList(aSwp.Value(), aSwp, aStatus);
	}

void CTestSsmSwpBuiltInCmd::PrepareCommandListCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpBuiltInCmdestCategory, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpBuiltInCmd::PrepareCommandListCancel() called");
	
	iCommandListResourceReader->PrepareCommandListCancel();
	}

CSsmCommandList* CTestSsmSwpBuiltInCmd::CommandList()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpBuiltInCmdestCategory, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpBuiltInCmd::CommandList() called");
		
	return iCommandListResourceReader->GetCommandList();
	}

void CTestSsmSwpBuiltInCmd::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;
	(void)aError;
	RDebug::Printf("CTestSsmSwpBuiltInCmd::HandleCleReturnValue() called with error %d",aError);
	// Always complete with success
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CTestSsmSwpBuiltInCmd::HandleCleReturnValueCancel()
	{
	// Nothing to do as HandleCleReturnValue always completes immediately
	}

TBool CTestSsmSwpBuiltInCmd::ConditionalCommandAllowedL(CResourceFile& /*aResourceFile*/, TInt /*__DEBUG_ONLY(aResourceId)*/)
	{
	User::Panic(KSwpBuiltInCmdestCategory, ESwpConditionalCommand);
	return EFalse;
	}
