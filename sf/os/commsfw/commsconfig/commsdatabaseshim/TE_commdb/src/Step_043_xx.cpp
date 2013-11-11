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
// This contains CommDb Unit Test Cases 043.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_043_xx.h"


CCommDbTest043_01::CCommDbTest043_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_043_01"));
	}

CCommDbTest043_01::~CCommDbTest043_01()
	{}

TVerdict CCommDbTest043_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest043_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();
	
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest043_01::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, ETrue);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 2;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, ETrue);
	
	CCommDbOverrideSettings* overSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overPref;
	overPref.iRanking = 1;
	overPref.iDirection = ECommDbConnectionDirectionOutgoing;
	overPref.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	overPref.iBearer.iBearerSet = KCommDbBearerCSD;
	overPref.iBearer.iIapId = 2;

	User::LeaveIfError(overSet->SetConnectionPreferenceOverride(overPref));

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overPref2;
	overPref2.iRanking = 1;
	overPref2.iDirection = ECommDbConnectionDirectionOutgoing;
	overPref2.iDialogPref = ECommDbDialogPrefUnknown;
	overPref2.iBearer.iBearerSet = 0;
	overPref2.iBearer.iIapId = 0;

	User::LeaveIfError(overSet->GetConnectionPreferenceOverride(overPref2));
	CleanupStack::PopAndDestroy(overSet);

	if(!(overPref==overPref2))
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest043_01::doTestStepPostambleL()
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

///////////////////////////////////////////////////////////////////////////

CCommDbTest043_02::CCommDbTest043_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_043_02"));
	}

CCommDbTest043_02::~CCommDbTest043_02()
	{}

TVerdict CCommDbTest043_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrAlreadyExists)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest043_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();
	
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest043_02::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, ETrue);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 2;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, ETrue);
	
	CCommDbOverrideSettings* overSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overPref;
	overPref.iRanking = 1;
	overPref.iDirection = ECommDbConnectionDirectionOutgoing;
	overPref.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	overPref.iBearer.iBearerSet = KCommDbBearerCSD;
	overPref.iBearer.iIapId = 2;

	User::LeaveIfError(overSet->SetConnectionPreferenceOverride(overPref));

	overPref.iDialogPref = ECommDbDialogPrefPrompt;
	overPref.iBearer.iBearerSet = KCommDbBearerCSD;
	overPref.iBearer.iIapId = 2;

	TInt err = overSet->SetConnectionPreferenceOverride(overPref);
	CleanupStack::PopAndDestroy(overSet);

	return err;
	}
	
TVerdict CCommDbTest043_02::doTestStepPostambleL()
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

///////////////////////////////////////////////////////////////////////////

CCommDbTest043_03::CCommDbTest043_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_043_03"));
	}

CCommDbTest043_03::~CCommDbTest043_03()
	{}

TVerdict CCommDbTest043_03::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest043_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();
	
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest043_03::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, ETrue);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 2;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, ETrue);
	
	CCommDbOverrideSettings* overSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overPref;
	overPref.iRanking = 1;
	overPref.iDirection = ECommDbConnectionDirectionOutgoing;
	overPref.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	overPref.iBearer.iBearerSet = KCommDbBearerCSD;
	overPref.iBearer.iIapId = 2;

	User::LeaveIfError(overSet->SetConnectionPreferenceOverride(overPref));

	overPref.iRanking = 0;
	overPref.iDirection = ECommDbConnectionDirectionUnknown;
	overPref.iDialogPref = ECommDbDialogPrefUnknown;
	overPref.iBearer.iBearerSet = 0;
	overPref.iBearer.iIapId = 0;

	TInt err = overSet->GetConnectionPreferenceOverride(overPref);

	CleanupStack::PopAndDestroy(overSet);

	if(err==KErrArgument)	//Have to map errors to different ones so we can do heap failure tests
		return KErrNone;	//Nasty, but not a lot else we can do.

	if(err==KErrNone)
		return KErrGeneral;

	return err;
	}
	
TVerdict CCommDbTest043_03::doTestStepPostambleL()
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

///////////////////////////////////////////////////////////////////////////

CCommDbTest043_04::CCommDbTest043_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_043_04"));
	}

CCommDbTest043_04::~CCommDbTest043_04()
	{}

TVerdict CCommDbTest043_04::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest043_04::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();
	
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest043_04::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, ETrue);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 2;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, ETrue);
	
	CCommDbOverrideSettings* overSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overPref;
	overPref.iRanking = 1;
	overPref.iDirection = ECommDbConnectionDirectionOutgoing;
	overPref.iDialogPref = ECommDbDialogPrefUnknown;
	overPref.iBearer.iBearerSet = 0;
	overPref.iBearer.iIapId = 0;

	TInt err = overSet->GetConnectionPreferenceOverride(overPref);

	CleanupStack::PopAndDestroy(overSet);

	return err;
	}
	
TVerdict CCommDbTest043_04::doTestStepPostambleL()
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

CCommDbTest043_05::CCommDbTest043_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_043_05"));
	}

CCommDbTest043_05::~CCommDbTest043_05()
	{}

TVerdict CCommDbTest043_05::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 043.05 called "));

	SetTestStepResult(EPass);
	CCommDbTest043_01* step043_01 = new(ELeave) CCommDbTest043_01;
	CleanupStack::PushL(step043_01);
	doTestStepWithHeapFailureL( *step043_01, KErrNone);
	CleanupStack::PopAndDestroy(step043_01);

	CCommDbTest043_02* step043_02 = new(ELeave) CCommDbTest043_02;
	CleanupStack::PushL(step043_02);
	doTestStepWithHeapFailureL( *step043_02, KErrAlreadyExists);
	CleanupStack::PopAndDestroy(step043_02);

	CCommDbTest043_03* step043_03 = new(ELeave) CCommDbTest043_03;
	CleanupStack::PushL(step043_03);
	doTestStepWithHeapFailureL( *step043_03, KErrNone);
	CleanupStack::PopAndDestroy(step043_03);

	CCommDbTest043_04* step043_04 = new(ELeave) CCommDbTest043_04;
	CleanupStack::PushL(step043_04);
	doTestStepWithHeapFailureL( *step043_04, KErrNotFound);
	CleanupStack::PopAndDestroy(step043_04);

	return TestStepResult();
	}
