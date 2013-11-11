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
// This contains CommDb Unit Test Cases 034.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_034_xx.h"


CCommDbTest034_01::CCommDbTest034_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_034_01"));
	}

CCommDbTest034_01::~CCommDbTest034_01()
	{}

TVerdict CCommDbTest034_01::doTestStepL( void )
	{
	executeStepL();
        SetTestStepResult(EPass);	return TestStepResult();	//If we've got this far we've passed
	}

TVerdict CCommDbTest034_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest034_01::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	User::LeaveIfError(iPrefView->GotoFirstRecord());
	iPrefView->ChangeConnectionPreferenceRankL(2);

	return KErrNone;
	}
	
TVerdict CCommDbTest034_01::doTestStepPostambleL()
	{
		//Get rid of our view
	if(iPrefView)
		{
		delete iPrefView;
		}
	iPrefView=0;

	//Create a new view that should have the record that was added in executeStepL and
	// may have been left in by a leave condition
	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	if(iPrefView->GotoFirstRecord() == KErrNone)
		{
		iPrefView->DeleteConnectionPreferenceL();
		}
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////

CCommDbTest034_02::CCommDbTest034_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_034_02"));
	}

CCommDbTest034_02::~CCommDbTest034_02()
	{}

TVerdict CCommDbTest034_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest034_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest034_02::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 2;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	User::LeaveIfError(iPrefView->GotoFirstRecord());
	iPrefView->ChangeConnectionPreferenceRankL(2);

	User::LeaveIfError(iPrefView->GotoNextRecord());

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	if(retrievedPref.iRanking!=0)
		return KErrGeneral;		//pref2 should have it's ranking displaced to 0 by ChangeConnectionPreferenceRankL

	return KErrNone;
	}
	
TVerdict CCommDbTest034_02::doTestStepPostambleL()
	{
		//Get rid of our view
	if(iPrefView)
		{
		delete iPrefView;
		}
	iPrefView=0;

	//Create a new view that should have the record that was added in executeStepL and
	// may have been left in by a leave condition
	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	if(iPrefView->GotoFirstRecord() == KErrNone)
		{
		iPrefView->DeleteConnectionPreferenceL();
		}
	if(iPrefView->GotoFirstRecord() == KErrNone)
		{
		iPrefView->DeleteConnectionPreferenceL();
		}
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////

CCommDbTest034_03::CCommDbTest034_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_034_03"));
	}

CCommDbTest034_03::~CCommDbTest034_03()
	{}

TVerdict CCommDbTest034_03::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest034_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest034_03::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 2;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	User::LeaveIfError(iPrefView->GotoFirstRecord());
	TRAPD(err, iPrefView->ChangeConnectionPreferenceRankL(5));
	if (err!=KErrOverflow)
		{
		if(err==KErrNone)		//This is horrible, but need to make sure that if this returns with KErrNone we catch it.
			err=KErrGeneral;
		return err;
		}

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	if(retrievedPref.iRanking!=1)
		return KErrGeneral;		//pref1 should still have ranking 1

	return KErrNone;
	}
	
TVerdict CCommDbTest034_03::doTestStepPostambleL()
	{
		//Get rid of our view
	if(iPrefView)
		{
		delete iPrefView;
		}
	iPrefView=0;

	//Create a new view that should have the record that was added in executeStepL and
	// may have been left in by a leave condition
	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	if(iPrefView->GotoFirstRecord() == KErrNone)
		{
		iPrefView->DeleteConnectionPreferenceL();
		}
	if(iPrefView->GotoFirstRecord() == KErrNone)
		{
		iPrefView->DeleteConnectionPreferenceL();
		}
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////

CCommDbTest034_04::CCommDbTest034_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_034_04"));
	}

CCommDbTest034_04::~CCommDbTest034_04()
	{}

TVerdict CCommDbTest034_04::doTestStepL( void )
	{
	if (executeStepL() != KErrNotFound)
		{
		SetTestStepResult(EFail);		}

	return TestStepResult();
	}

TVerdict CCommDbTest034_04::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest034_04::executeStepL()
	{
	//
	// Previously (I guess) CommDB would have been able to create two
	// Conn Pref entries and then the fresh view would not have seen them
	// and it would panic. Now CommsDAT does see them and returns
	// KErrNotFound, so now we no longer inserted in this test allowing
	// the test to actually test the expected scenario.
	//
	delete iPrefView;		//Create a fresh view
	iPrefView=NULL;
	iPrefView=iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();
	TRAPD(err, iPrefView->ChangeConnectionPreferenceRankL(2));

	return err;
	}
	
TVerdict CCommDbTest034_04::doTestStepPostambleL()
	{
		//Get rid of our view
	if(iPrefView)
		{
		delete iPrefView;
		}
	iPrefView=0;

	//Create a new view that should have the record that was added in executeStepL and
	// may have been left in by a leave condition
	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	if(iPrefView->GotoFirstRecord() == KErrNone)
		{
		iPrefView->DeleteConnectionPreferenceL();
		}
	if(iPrefView->GotoFirstRecord() == KErrNone)
		{
		iPrefView->DeleteConnectionPreferenceL();
		}
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////

CCommDbTest034_05::CCommDbTest034_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_034_05"));
	}

CCommDbTest034_05::~CCommDbTest034_05()
	{}

TVerdict CCommDbTest034_05::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest034_05::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest034_05::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, ETrue);

	User::LeaveIfError(iPrefView->GotoFirstRecord());

	TRAPD(err, iPrefView->ChangeConnectionPreferenceRankL(2));
	if(err!=KErrAccessDenied)
		{
		if(err==KErrNone)		//Need to do some maping here otherwise if this returns KErrNone the test will pass!!
			err=KErrGeneral;
		return err;
		}

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	iPrefView->ReadConnectionPreferenceL(retrievedPref);
	
	if(retrievedPref.iRanking!=1)		//The ranking has been changed for a read only entry
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest034_05::doTestStepPostambleL()
	{
		//Get rid of our view
	if(iPrefView)
		{
		delete iPrefView;
		}
	iPrefView=0;

	//Create a new view that should have the record that was added in executeStepL and
	// may have been left in by a leave condition
	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	if(iPrefView->GotoFirstRecord() == KErrNone)
		{
		iPrefView->DeleteConnectionPreferenceL();
		}
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest034_06::CCommDbTest034_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_034_06"));
	}

CCommDbTest034_06::~CCommDbTest034_06()
	{}

TVerdict CCommDbTest034_06::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 034.06 called "));

	SetTestStepResult(EPass);
	CCommDbTest034_01* step034_01 = new(ELeave) CCommDbTest034_01;
	CleanupStack::PushL(step034_01);
	doTestStepWithHeapFailureL( *step034_01, KErrNone);
	CleanupStack::PopAndDestroy(step034_01);

	CCommDbTest034_02* step034_02 = new(ELeave) CCommDbTest034_02;
	CleanupStack::PushL(step034_02);
	doTestStepWithHeapFailureL( *step034_02, KErrNone);
	CleanupStack::PopAndDestroy(step034_02);

	CCommDbTest034_03* step034_03 = new(ELeave) CCommDbTest034_03;
	CleanupStack::PushL(step034_03);
	doTestStepWithHeapFailureL( *step034_03, KErrNone);
	CleanupStack::PopAndDestroy(step034_03);

	CCommDbTest034_04* step034_04 = new(ELeave) CCommDbTest034_04;
	CleanupStack::PushL(step034_04);
	doTestStepWithHeapFailureL( *step034_04, KErrNotFound);
	CleanupStack::PopAndDestroy(step034_04);

	CCommDbTest034_05* step034_05 = new(ELeave) CCommDbTest034_05;
	CleanupStack::PushL(step034_05);
	doTestStepWithHeapFailureL( *step034_05, KErrNone);
	CleanupStack::PopAndDestroy(step034_05);
	
	return TestStepResult();
	}
