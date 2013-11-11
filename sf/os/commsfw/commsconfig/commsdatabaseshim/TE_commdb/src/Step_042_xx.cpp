//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
//
// This contains CommDb Unit Test Cases 042.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_042_xx.h"


CCommDbTest042_01::CCommDbTest042_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_042_01"));
	}

CCommDbTest042_01::~CCommDbTest042_01()
	{}

TVerdict CCommDbTest042_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest042_01::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest042_01::executeStepL()
	{
	TInt err(KErrNone);

	CCommDbOverrideSettings* overSet1=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet1);
	TUint32 temp1 = 5000;
	User::LeaveIfError(overSet1->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), temp1));

	CCommDbOverrideSettings* overSet2=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet2);
	TUint32 temp2=5000;
	User::LeaveIfError(overSet2->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_AUTH_RETRIES), temp2));

	if(overSet1->Compare(overSet2))
		err = KErrGeneral;

	CleanupStack::PopAndDestroy(overSet2);
	CleanupStack::PopAndDestroy(overSet1);
	
	return err;
	}
	
TVerdict CCommDbTest042_01::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

///////////////////////////////////////////////////////////////////////////////////////

CCommDbTest042_02::CCommDbTest042_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_042_02"));
	}

CCommDbTest042_02::~CCommDbTest042_02()
	{}

TVerdict CCommDbTest042_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest042_02::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest042_02::executeStepL()
	{
	TInt err(KErrNone);

	CCommDbOverrideSettings* overSet1=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet1);
	TUint32 temp1 = 5000;
	User::LeaveIfError(overSet1->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), temp1));

	CCommDbOverrideSettings* overSet2=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet2);
	TUint32 temp2=5000;
	User::LeaveIfError(overSet2->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), temp2));

	if(!overSet1->Compare(overSet2))
		err = KErrGeneral;

	CleanupStack::PopAndDestroy(overSet2);
	CleanupStack::PopAndDestroy(overSet1);
	
	return err;
	}
	
TVerdict CCommDbTest042_02::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest042_03::CCommDbTest042_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_042_03"));
	}

CCommDbTest042_03::~CCommDbTest042_03()
	{}

TVerdict CCommDbTest042_03::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 042.03 called "));

	SetTestStepResult(EPass);
	CCommDbTest042_01* step042_01 = new(ELeave) CCommDbTest042_01;
	CleanupStack::PushL(step042_01);
	doTestStepWithHeapFailureL( *step042_01, KErrNone);
	CleanupStack::PopAndDestroy(step042_01);

	CCommDbTest042_02* step042_02 = new(ELeave) CCommDbTest042_02;
	CleanupStack::PushL(step042_02);
	doTestStepWithHeapFailureL( *step042_02, KErrNone);
	CleanupStack::PopAndDestroy(step042_02);

	return TestStepResult();
	}
