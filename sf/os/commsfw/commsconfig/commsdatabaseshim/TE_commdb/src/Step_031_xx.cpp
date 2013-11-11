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
// This contains CommDb Unit Test Cases 031.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_031_xx.h"


CCommDbTest031_01::CCommDbTest031_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_031_01"));
	}

CCommDbTest031_01::~CCommDbTest031_01()
	{}

TVerdict CCommDbTest031_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest031_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest031_01::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 2;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 2;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 1;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 1;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	delete iPrefView;	//Open a new view in rank order
	iPrefView=NULL;
	iPrefView=iTheDb->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionOutgoing);
	CleanupStack::Pop(iPrefView);

	User::LeaveIfError(iPrefView->GotoFirstRecord());

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	iPrefView->ReadConnectionPreferenceL(retrievedPref);
	if(!(retrievedPref==pref2))
		return KErrGeneral;

	User::LeaveIfError(iPrefView->GotoNextRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);
	if(!(retrievedPref==pref1))
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest031_01::doTestStepPostambleL()
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

/////////////////////////////////////////////////////////////////////////////////

CCommDbTest031_02::CCommDbTest031_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_031_02"));
	}

CCommDbTest031_02::~CCommDbTest031_02()
	{}

TVerdict CCommDbTest031_02::doTestStepL( void )
	{
	executeStepL();
        SetTestStepResult(EPass);	return TestStepResult();	//if we get to here we must have passed.
	}

TVerdict CCommDbTest031_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest031_02::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 2;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 2;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 1;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 1;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	delete iPrefView;	//Open a new view in rank order looking at incoming only
	iPrefView=NULL;
	iPrefView=iTheDb->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionIncoming);
	CleanupStack::Pop(iPrefView);

	return KErrNone;
	}
	
TVerdict CCommDbTest031_02::doTestStepPostambleL()
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

/////////////////////////////////////////////////////////////////////////////////

CCommDbTest031_03::CCommDbTest031_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_031_03"));
	}

CCommDbTest031_03::~CCommDbTest031_03()
	{}

TVerdict CCommDbTest031_03::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();	//if we get to here we must have passed.
	}

TVerdict CCommDbTest031_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest031_03::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 2;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 2;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 1;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 1;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	delete iPrefView;	//Open a new view to look at rank 1 outgoing only
	iPrefView=NULL;
	iPrefView=iTheDb->OpenConnectionPrefTableViewOnRankLC(ECommDbConnectionDirectionOutgoing, 1);
	CleanupStack::Pop(iPrefView);

	User::LeaveIfError(iPrefView->GotoFirstRecord());

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	if(!(retrievedPref==pref2))
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest031_03::doTestStepPostambleL()
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

/////////////////////////////////////////////////////////////////////////////////

CCommDbTest031_04::CCommDbTest031_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_031_04"));
	}

CCommDbTest031_04::~CCommDbTest031_04()
	{}

TVerdict CCommDbTest031_04::doTestStepL( void )
	{
	if(executeStepL()!=KErrOverflow)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();	//if we get to here we must have passed.
	}

TVerdict CCommDbTest031_04::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest031_04::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 2;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 2;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 1;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 1;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	delete iPrefView;	//Open a new view with invalid rank;
	iPrefView=NULL;
	TRAPD(err, iPrefView=iTheDb->OpenConnectionPrefTableViewOnRankLC(ECommDbConnectionDirectionOutgoing, 5));
	if (err==KErrNone)		//This should never happen, but better safe than sorry
		CleanupStack::Pop(iPrefView);

	return err;
	}
	
TVerdict CCommDbTest031_04::doTestStepPostambleL()
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

//////////////////////////////////////////////////////////////////////////

CCommDbTest031_05::CCommDbTest031_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_031_05"));
	}

CCommDbTest031_05::~CCommDbTest031_05()
	{}

TVerdict CCommDbTest031_05::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 031.05 called "));

	SetTestStepResult(EPass);
	CCommDbTest031_01* step031_01 = new(ELeave) CCommDbTest031_01;
	CleanupStack::PushL(step031_01);
	doTestStepWithHeapFailureL( *step031_01, KErrNone);
	CleanupStack::PopAndDestroy(step031_01);

	CCommDbTest031_02* step031_02 = new(ELeave) CCommDbTest031_02;
	CleanupStack::PushL(step031_02);
	doTestStepWithHeapFailureL( *step031_02, KErrNone);
	CleanupStack::PopAndDestroy(step031_02);

	CCommDbTest031_03* step031_03 = new(ELeave) CCommDbTest031_03;
	CleanupStack::PushL(step031_03);
	doTestStepWithHeapFailureL( *step031_03, KErrNone);
	CleanupStack::PopAndDestroy(step031_03);

	CCommDbTest031_04* step031_04 = new(ELeave) CCommDbTest031_04;
	CleanupStack::PushL(step031_04);
	doTestStepWithHeapFailureL( *step031_04, KErrOverflow);
	CleanupStack::PopAndDestroy(step031_04);
	

	return TestStepResult();
	}

