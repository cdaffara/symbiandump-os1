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
#include <ssm/conditionevaluate.h>
#include <e32debug.h>
#include <e32property.h> 

#include "tconditionevaluator_commandlist.hrh"
#include "tconditionevaluator_swppolicy_evaluate.h"

_LIT(KCommandListPath, "z:\\resource\\sspluginstest\\conditionevaluator\\");


EXPORT_C MSsmSwpPolicy* CTestSsmSwpCondCmd::NewL()
	{
	CTestSsmSwpCondCmd* self = new (ELeave) CTestSsmSwpCondCmd;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
CTestSsmSwpCondCmd::CTestSsmSwpCondCmd()
	{
	RDebug::Printf("CTestSsmSwpCondCmd::CTestSsmSwpCondCmd() called");
	}

CTestSsmSwpCondCmd::~CTestSsmSwpCondCmd()
	{
	RDebug::Printf("CTestSsmSwpCondCmd::~CTestSsmSwpCondCmd() called");
	iFs.Close();
	delete iCommandListResourceReader;
	}

void CTestSsmSwpCondCmd::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	// create resource reader
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath(), *this);
	iEvaluateRes = 0;
	}

void CTestSsmSwpCondCmd::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KTestSwpCondCmdPolicyFile, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpCondCmd::Initialize() called");
		
	// initialise command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

void CTestSsmSwpCondCmd::InitializeCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpCondCmdestCategory, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpCondCmd::InitializeCancel() called");
		
	iCommandListResourceReader->InitialiseCancel();
	}

void CTestSsmSwpCondCmd::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CTestSsmSwpCondCmd::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aSwp;
	(void)aMessage; 
	RDebug::Printf("CTestSsmSwpCondCmd::TransitionAllowed() called");
	return EAllowed; 
	}

void CTestSsmSwpCondCmd::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpCondCmdestCategory, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpCondCmd::PrepareCommandList() called");
	
	//Build the commandlist from resource, it is always the command list for this value
	iCommandListResourceReader->PrepareCommandList(aSwp.Value(), aSwp, aStatus);
	}

void CTestSsmSwpCondCmd::PrepareCommandListCancel()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpCondCmdestCategory, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpCondCmd::PrepareCommandListCancel() called");
	
	iCommandListResourceReader->PrepareCommandListCancel();
	}

CSsmCommandList* CTestSsmSwpCondCmd::CommandList()
	{
	__ASSERT_DEBUG(iCommandListResourceReader, User::Panic(KSwpCondCmdestCategory, ESwpInvalidResourceReader));
	RDebug::Printf("CTestSsmSwpCondCmd::CommandList() called");
		
	return iCommandListResourceReader->GetCommandList();
	}

void CTestSsmSwpCondCmd::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;
	(void)aError;
	RDebug::Printf("CTestSsmSwpCondCmd::HandleCleReturnValue() called with error %d",aError);
	// Always complete with success
	// To test that conditional evaluator will not support recursive conditions
	//ERecuesiveConditionalCommandsList has a recursive condition
	if(aSwp.Value() == ERecuesiveConditionalCommandsList && iEvaluateRes == KErrCorrupt)
		{
		const TUid KPropertyCategory={0x2000D75B};
		const TUid KServerSID = KPropertyCategory;
		RProperty::Set(KServerSID, EPropertyCondition45, 1);
		}
		
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CTestSsmSwpCondCmd::HandleCleReturnValueCancel()
	{
	// Nothing to do as HandleCleReturnValue always completes immediately
	}

TBool CTestSsmSwpCondCmd::ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId)
	{
	RDebug::Printf("CTestSsmSwpCondCmd::ConditionalCommandAllowedL() called");
	CConditionEvaluate* evaluate = CConditionEvaluate::NewLC();
	TBool value = 0;
	TRAPD(err,value = evaluate->EvaluateL(aResourceFile,aResourceId));	
	iEvaluateRes = err;
	TBool ret = value == 1 ? ETrue : EFalse;
	CleanupStack::PopAndDestroy(evaluate);
	return ret;
	}
