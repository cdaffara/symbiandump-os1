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
// This contains CommDb Unit Test Cases 026.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_026_xx.h"

//_LIT(KCommDefault3, "C:\\default3.dat");

CCommDbTest026_01::CCommDbTest026_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_01"));
	}

CCommDbTest026_01::~CCommDbTest026_01()
	{}

TVerdict CCommDbTest026_01::doTestStepL( void )
	{
	executeStepL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest026_01::doTestStepPreambleL()
	{
//	copyFileL(KCommDbName, KCommDefault3);
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();


        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest026_01::executeStepL()
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

	return KErrNone;
	}
	
TVerdict CCommDbTest026_01::doTestStepPostambleL()
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

////////////////////////////////////////////////////////////////////////////////////

CCommDbTest026_02::CCommDbTest026_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_02"));
	}

CCommDbTest026_02::~CCommDbTest026_02()
	{}

TVerdict CCommDbTest026_02::doTestStepL( void )
	{
	if (KErrAlreadyExists == executeStepL())
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest026_02::doTestStepPreambleL()
	{
//	copyFileL(KCommDefault3, KCommDbName);
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	iTheDb->SetGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS),2);
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest026_02::executeStepL()
	{

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 1;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	TRAPD(err, iPrefView->InsertConnectionPreferenceL(pref2, EFalse));
	
	return err;
	}
	
TVerdict CCommDbTest026_02::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////////

CCommDbTest026_03::CCommDbTest026_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_03"));
	}

CCommDbTest026_03::~CCommDbTest026_03()
	{}

TVerdict CCommDbTest026_03::doTestStepL( void )
	{
	if (KErrAlreadyExists == executeStepL())
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest026_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	iTheDb->SetGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS),2);
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest026_03::executeStepL()
	{

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionIncoming;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 3;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 1;
	pref2.iDirection = ECommDbConnectionDirectionIncoming;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 4;
	
	TRAPD(err, iPrefView->InsertConnectionPreferenceL(pref2, EFalse));
	
	return err;
	}
	
TVerdict CCommDbTest026_03::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////////

CCommDbTest026_04::CCommDbTest026_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_04"));
	}

CCommDbTest026_04::~CCommDbTest026_04()
	{}

TVerdict CCommDbTest026_04::doTestStepL( void )
	{
	if (KErrOverflow == executeStepL())
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest026_04::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	iTheDb->SetGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS),2);
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest026_04::executeStepL()
	{

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 85;
	pref1.iDirection = ECommDbConnectionDirectionIncoming;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 3;

	TRAPD(err, iPrefView->InsertConnectionPreferenceL(pref1, EFalse));
	
	return err;
	}
	
TVerdict CCommDbTest026_04::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////////

CCommDbTest026_05::CCommDbTest026_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_05"));
	}

CCommDbTest026_05::~CCommDbTest026_05()
	{}

TVerdict CCommDbTest026_05::doTestStepL( void )
	{
	if (KErrNotFound == executeStepL())
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest026_05::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	iTheDb->SetGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS),2);
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest026_05::executeStepL()
	{

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionIncoming;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 0xffffffff;

	TRAPD(err, iPrefView->InsertConnectionPreferenceL(pref1, EFalse));

	return err;
	}
	
TVerdict CCommDbTest026_05::doTestStepPostambleL()
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
	
	iTheDb->ClearGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS));
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////////

CCommDbTest026_06::CCommDbTest026_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_06"));
	}

CCommDbTest026_06::~CCommDbTest026_06()
	{}

TVerdict CCommDbTest026_06::doTestStepL( void )
	{
	if (KErrArgument == executeStepL())
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest026_06::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	iTheDb->SetGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS),2);
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest026_06::executeStepL()
	{

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionIncoming;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = 0x80000000;
	pref1.iBearer.iIapId = 3;

	TRAPD(err, iPrefView->InsertConnectionPreferenceL(pref1, EFalse));

	return err;
	}
	
TVerdict CCommDbTest026_06::doTestStepPostambleL()
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

	iTheDb->ClearGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS));
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////////

CCommDbTest026_07::CCommDbTest026_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_07"));
	}

CCommDbTest026_07::~CCommDbTest026_07()
	{}

TVerdict CCommDbTest026_07::doTestStepL( void )
	{
	if (KErrArgument == executeStepL())
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest026_07::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	iTheDb->SetGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS),2);
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest026_07::executeStepL()
	{

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPromptIfWrongMode;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	TRAPD(err, iPrefView->InsertConnectionPreferenceL(pref1, EFalse));

	return err;
	}
	
TVerdict CCommDbTest026_07::doTestStepPostambleL()
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

	iTheDb->ClearGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS));
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////////

CCommDbTest026_08::CCommDbTest026_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_08"));
	}

CCommDbTest026_08::~CCommDbTest026_08()
	{}

TVerdict CCommDbTest026_08::doTestStepL( void )
	{
	if (KErrArgument == executeStepL())
         SetTestStepResult(EPass);        SetTestStepResult(EFail);	return TestStepResult();
	}

TVerdict CCommDbTest026_08::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	iTheDb->SetGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS),2);
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest026_08::executeStepL()
	{

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionIncoming;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 3;

	TRAPD(err, iPrefView->InsertConnectionPreferenceL(pref1, EFalse));

	return err;
	}
	
TVerdict CCommDbTest026_08::doTestStepPostambleL()
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

	iTheDb->ClearGlobalSettingL(TPtrC(CONNECTION_ATTEMPTS));
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest026_09::CCommDbTest026_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_09"));
	}

CCommDbTest026_09::~CCommDbTest026_09()
	{}

TVerdict CCommDbTest026_09::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 026.09 called "));

	SetTestStepResult(EPass);
	CCommDbTest026_01* step026_01 = new(ELeave) CCommDbTest026_01;
	CleanupStack::PushL(step026_01);
	doTestStepWithHeapFailureL( *step026_01, KErrNone);
	CleanupStack::PopAndDestroy(step026_01);


	return TestStepResult();
	}


CCommDbTest026_10::CCommDbTest026_10()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_10"));
	}

CCommDbTest026_10::~CCommDbTest026_10()
	{}

TVerdict CCommDbTest026_10::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 026.10 called "));

	SetTestStepResult(EPass);	
	CCommDbTest026_02* step026_02 = new(ELeave) CCommDbTest026_02;
	CleanupStack::PushL(step026_02);
	doTestStepWithHeapFailureL( *step026_02, KErrAlreadyExists);
	CleanupStack::PopAndDestroy(step026_02);
	
	return TestStepResult();
	}
	
	
CCommDbTest026_11::CCommDbTest026_11()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_11"));
	}

CCommDbTest026_11::~CCommDbTest026_11()
	{}

TVerdict CCommDbTest026_11::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 026.11 called "));

	SetTestStepResult(EPass);	
		CCommDbTest026_03* step026_03 = new(ELeave) CCommDbTest026_03;
	CleanupStack::PushL(step026_03);
	doTestStepWithHeapFailureL( *step026_03, KErrAlreadyExists);
	CleanupStack::PopAndDestroy(step026_03);
	
	return TestStepResult();
	}
	
CCommDbTest026_12::CCommDbTest026_12()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_12"));
	}

CCommDbTest026_12::~CCommDbTest026_12()
	{}

TVerdict CCommDbTest026_12::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 026.12 called "));

	SetTestStepResult(EPass);	
	CCommDbTest026_04* step026_04 = new(ELeave) CCommDbTest026_04;
	CleanupStack::PushL(step026_04);
	doTestStepWithHeapFailureL( *step026_04, KErrOverflow);
	CleanupStack::PopAndDestroy(step026_04);
	
	return TestStepResult();
	}	
	
CCommDbTest026_13::CCommDbTest026_13()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_13"));
	}

CCommDbTest026_13::~CCommDbTest026_13()
	{}

TVerdict CCommDbTest026_13::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 026.13 called "));

	SetTestStepResult(EPass);	
	CCommDbTest026_05* step026_05 = new(ELeave) CCommDbTest026_05;
	CleanupStack::PushL(step026_05);
	doTestStepWithHeapFailureL( *step026_05, KErrNotFound);
	CleanupStack::PopAndDestroy(step026_05);
	
	return TestStepResult();
	}	
		
CCommDbTest026_14::CCommDbTest026_14()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_14"));
	}

CCommDbTest026_14::~CCommDbTest026_14()
	{}

TVerdict CCommDbTest026_14::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 026.14 called "));

	SetTestStepResult(EPass);	
	CCommDbTest026_06* step026_06 = new(ELeave) CCommDbTest026_06;
	CleanupStack::PushL(step026_06);
	doTestStepWithHeapFailureL( *step026_06, KErrArgument);
	CleanupStack::PopAndDestroy(step026_06);
	
	return TestStepResult();
	}	
	
CCommDbTest026_15::CCommDbTest026_15()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_15"));
	}

CCommDbTest026_15::~CCommDbTest026_15()
	{}

TVerdict CCommDbTest026_15::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 026.15 called "));

	SetTestStepResult(EPass);	
	CCommDbTest026_07* step026_07 = new(ELeave) CCommDbTest026_07;
	CleanupStack::PushL(step026_07);
	doTestStepWithHeapFailureL( *step026_07, KErrArgument);
	CleanupStack::PopAndDestroy(step026_07);
	
	return TestStepResult();
	}	
	
CCommDbTest026_16::CCommDbTest026_16()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_16"));
	}

CCommDbTest026_16::~CCommDbTest026_16()
	{}

TVerdict CCommDbTest026_16::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 026.16 called "));

	SetTestStepResult(EPass);	
	CCommDbTest026_08* step026_08 = new(ELeave) CCommDbTest026_08;
	CleanupStack::PushL(step026_08);
	doTestStepWithHeapFailureL( *step026_08, KErrArgument);
	CleanupStack::PopAndDestroy(step026_08);
	
	return TestStepResult();
	}	

//EOF
