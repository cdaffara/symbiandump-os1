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
// This contains CommDb Unit Test Cases 028.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_028_xx.h"

CCommDbTest028_01::CCommDbTest028_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_028_01"));
	}

CCommDbTest028_01::~CCommDbTest028_01()
	{}

TVerdict CCommDbTest028_01::doTestStepL( void )
	{
	if(executeStepL()==KErrNone)
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest028_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest028_01::executeStepL()
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

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	User::LeaveIfError(iPrefView->GotoFirstRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);
	
	if(!(retrievedPref==pref1))	//Check to see if correct preferance has been fetched
		return KErrGeneral;

	User::LeaveIfError(iPrefView->GotoNextRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	if(!(retrievedPref==pref2))	//Check to see if correct preferance has been fetched
		return KErrGeneral;

	if((iPrefView->GotoNextRecord())!=KErrNotFound)	//Make sure there are no more records.
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest028_01::doTestStepPostambleL()
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

///////////////////////////////////////////////////////////////////////////////

CCommDbTest028_02::CCommDbTest028_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_028_02"));
	}

CCommDbTest028_02::~CCommDbTest028_02()
	{}

TVerdict CCommDbTest028_02::doTestStepL( void )
	{
	if(executeStepL()==KErrNone)
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest028_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest028_02::executeStepL()
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

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	User::LeaveIfError(iPrefView->GotoFirstRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);
	
	while (iPrefView->GotoNextRecord()!=KErrNotFound)	//Move through to last record
		{}
	
	User::LeaveIfError(iPrefView->GotoPreviousRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	if(!(retrievedPref==pref2))	//Check to see if correct preferance has been fetched
		return KErrGeneral;

	User::LeaveIfError(iPrefView->GotoPreviousRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	if(!(retrievedPref==pref1))	//Check to see if correct preferance has been fetched
		return KErrGeneral;

	if(iPrefView->GotoPreviousRecord()!=KErrNotFound)	//Check that there are no more records
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest028_02::doTestStepPostambleL()
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

///////////////////////////////////////////////////////////////////////////////

CCommDbTest028_03::CCommDbTest028_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_028_03"));
	}

CCommDbTest028_03::~CCommDbTest028_03()
	{}

TVerdict CCommDbTest028_03::doTestStepL( void )
	{
	if(executeStepL()==KErrNone)
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest028_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest028_03::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionIncoming;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 3;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 2;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	delete iPrefView;	//Create a new view to look only at outgoing records.
	iPrefView=NULL;
	iPrefView=iTheDb->OpenConnectionPrefTableLC(ECommDbConnectionDirectionOutgoing);
	CleanupStack::Pop(iPrefView);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	User::LeaveIfError(iPrefView->GotoFirstRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);
		
	if(!(retrievedPref==pref2))	//First record in this table should be pref2
		return KErrGeneral;

	if(iPrefView->GotoNextRecord()!=KErrNotFound)	//There should be no more records in this view.
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest028_03::doTestStepPostambleL()
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

///////////////////////////////////////////////////////////////////////////////

CCommDbTest028_04::CCommDbTest028_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_028_04"));
	}

CCommDbTest028_04::~CCommDbTest028_04()
	{}

TVerdict CCommDbTest028_04::doTestStepL( void )
	{
	if(executeStepL()==KErrNone)
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest028_04::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest028_04::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionIncoming;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 3;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 2;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	delete iPrefView;	//Create a new view to look only at incoming records.
	iPrefView=NULL;
	iPrefView=iTheDb->OpenConnectionPrefTableLC(ECommDbConnectionDirectionIncoming);
	CleanupStack::Pop(iPrefView);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	User::LeaveIfError(iPrefView->GotoFirstRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);
		
	if(!(retrievedPref==pref1))	//First record in this table should be pref1
		return KErrGeneral;

	if(iPrefView->GotoNextRecord()!=KErrNotFound)	//There should be no more records in this view.
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest028_04::doTestStepPostambleL()
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

///////////////////////////////////////////////////////////////////////////////

CCommDbTest028_05::CCommDbTest028_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_028_05"));
	}

CCommDbTest028_05::~CCommDbTest028_05()
	{}

TVerdict CCommDbTest028_05::doTestStepL( void )
	{
	if(executeStepL()==KErrNone)
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest028_05::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest028_05::executeStepL()
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

	delete iPrefView;	//Create a new view to look only at incoming records.
	iPrefView=NULL;
	iPrefView=iTheDb->OpenConnectionPrefTableLC(ECommDbConnectionDirectionIncoming);
	CleanupStack::Pop(iPrefView);

	if(iPrefView->GotoPreviousRecord()!=KErrNotFound)	//There should be no records in this table.
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest028_05::doTestStepPostambleL()
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

///////////////////////////////////////////////////////////////////////////////

CCommDbTest028_06::CCommDbTest028_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_028_06"));
	}

CCommDbTest028_06::~CCommDbTest028_06()
	{}

TVerdict CCommDbTest028_06::doTestStepL( void )
	{
	if(executeStepL()==KErrNone)
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest028_06::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest028_06::executeStepL()
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

	delete iPrefView;	//Create a new view to look only at incoming records.
	iPrefView=NULL;
	iPrefView=iTheDb->OpenConnectionPrefTableLC(ECommDbConnectionDirectionIncoming);
	CleanupStack::Pop(iPrefView);

	if(iPrefView->GotoNextRecord()!=KErrNotFound)	//There should be no records in this table.
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest028_06::doTestStepPostambleL()
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

CCommDbTest028_07::CCommDbTest028_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_028_07"));
	}

CCommDbTest028_07::~CCommDbTest028_07()
	{}

TVerdict CCommDbTest028_07::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 028.07 called "));

	SetTestStepResult(EPass);
	CCommDbTest028_01* step028_01 = new(ELeave) CCommDbTest028_01;
	CleanupStack::PushL(step028_01);
	doTestStepWithHeapFailureL( *step028_01, KErrNone);
	CleanupStack::PopAndDestroy(step028_01);

	CCommDbTest028_02* step028_02 = new(ELeave) CCommDbTest028_02;
	CleanupStack::PushL(step028_02);
	doTestStepWithHeapFailureL( *step028_02, KErrNone);
	CleanupStack::PopAndDestroy(step028_02);

	CCommDbTest028_03* step028_03 = new(ELeave) CCommDbTest028_03;
	CleanupStack::PushL(step028_03);
	doTestStepWithHeapFailureL( *step028_03, KErrNone);
	CleanupStack::PopAndDestroy(step028_03);

	CCommDbTest028_04* step028_04 = new(ELeave) CCommDbTest028_04;
	CleanupStack::PushL(step028_04);
	doTestStepWithHeapFailureL( *step028_04, KErrNone);
	CleanupStack::PopAndDestroy(step028_04);

	CCommDbTest028_05* step028_05 = new(ELeave) CCommDbTest028_05;
	CleanupStack::PushL(step028_05);
	doTestStepWithHeapFailureL( *step028_05, KErrNone);
	CleanupStack::PopAndDestroy(step028_05);

	CCommDbTest028_06* step028_06 = new(ELeave) CCommDbTest028_06;
	CleanupStack::PushL(step028_06);
	doTestStepWithHeapFailureL( *step028_06, KErrNone);
	CleanupStack::PopAndDestroy(step028_06);

	return TestStepResult();
	}
