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
// This contains CommDb Unit Test Cases 032.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_032_xx.h"


CCommDbTest032_01::CCommDbTest032_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_032_01"));
	}

CCommDbTest032_01::~CCommDbTest032_01()
	{}

TVerdict CCommDbTest032_01::doTestStepL( void )
	{
	executeStepL();
        SetTestStepResult(EPass);	return TestStepResult();	//If we've got this far we've passed
	}

TVerdict CCommDbTest032_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest032_01::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	User::LeaveIfError(iPrefView->GotoFirstRecord());

	iPrefView->UpdateDialogPrefL(ECommDbDialogPrefDoNotPrompt);

	return KErrNone;
	}

TVerdict CCommDbTest032_01::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

///////////////////////////////////////////////////////////////////////////////////

CCommDbTest032_02::CCommDbTest032_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_032_02"));
	}

CCommDbTest032_02::~CCommDbTest032_02()
	{}

TVerdict CCommDbTest032_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest032_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest032_02::executeStepL()
	{
	TRAPD(err, iPrefView->UpdateDialogPrefL(ECommDbDialogPrefDoNotPrompt));

	return err;
	}

TVerdict CCommDbTest032_02::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

///////////////////////////////////////////////////////////////////////////////////

CCommDbTest032_03::CCommDbTest032_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_032_03"));
	}

CCommDbTest032_03::~CCommDbTest032_03()
	{}

TVerdict CCommDbTest032_03::doTestStepL( void )
	{
	if(executeStepL()!=KErrAccessDenied)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest032_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest032_03::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, ETrue);

	User::LeaveIfError(iPrefView->GotoFirstRecord());

	TRAPD(err, iPrefView->UpdateDialogPrefL(ECommDbDialogPrefDoNotPrompt));

	return err;
	}

TVerdict CCommDbTest032_03::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest032_04::CCommDbTest032_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_032_04"));
	}

CCommDbTest032_04::~CCommDbTest032_04()
	{}

TVerdict CCommDbTest032_04::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 032.04 called "));

	SetTestStepResult(EPass);
	CCommDbTest032_01* step032_01 = new(ELeave) CCommDbTest032_01;
	CleanupStack::PushL(step032_01);
	doTestStepWithHeapFailureL( *step032_01, KErrNone);
	CleanupStack::PopAndDestroy(step032_01);


	return TestStepResult();
	}


//////////////////////////////////////////////////////////////////////////

CCommDbTest032_05::CCommDbTest032_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_032_05"));
	}

CCommDbTest032_05::~CCommDbTest032_05()
	{}

TVerdict CCommDbTest032_05::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 032.04 called "));

	SetTestStepResult(EPass);
	CCommDbTest032_02* step032_02 = new(ELeave) CCommDbTest032_02;
	CleanupStack::PushL(step032_02);
	doTestStepWithHeapFailureL( *step032_02, KErrNotFound);
	CleanupStack::PopAndDestroy(step032_02);

	CCommDbTest032_03* step032_03 = new(ELeave) CCommDbTest032_03;
	CleanupStack::PushL(step032_03);
	doTestStepWithHeapFailureL( *step032_03, KErrAccessDenied);
	CleanupStack::PopAndDestroy(step032_03);

	return TestStepResult();
	}

