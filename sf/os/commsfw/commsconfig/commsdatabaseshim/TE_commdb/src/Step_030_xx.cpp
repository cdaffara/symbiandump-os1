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
// This contains CommDb Unit Test Cases 030.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_030_xx.h"


CCommDbTest030_01::CCommDbTest030_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_030_01"));
	}

CCommDbTest030_01::~CCommDbTest030_01()
	{}

TVerdict CCommDbTest030_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest030_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest030_01::executeStepL()
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
	pref2.iDirection = ECommDbConnectionDirectionIncoming;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 3;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	User::LeaveIfError(iPrefView->GotoFirstRecord());
	
	CCommsDbConnectionPrefTableView::TCommDbIapBearer newBearer;	//Set new bearer to point at other outgoing Iap in commdb
	newBearer.iBearerSet=KCommDbBearerLAN|KCommDbBearerCSD;
	newBearer.iIapId=2;

	iPrefView->UpdateBearerL(newBearer);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;

	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	if(	  (retrievedPref.iBearer.iBearerSet!=newBearer.iBearerSet)	//Check that retrieved pref is what we would expect
		||(retrievedPref.iBearer.iIapId!=newBearer.iIapId)
		||(retrievedPref.iRanking!=pref1.iRanking)
		||(retrievedPref.iDirection!=pref1.iDirection)
		||(retrievedPref.iDialogPref!=pref1.iDialogPref))
			return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest030_01::doTestStepPostambleL()
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

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest030_02::CCommDbTest030_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_030_02"));
	}

CCommDbTest030_02::~CCommDbTest030_02()
	{}

TVerdict CCommDbTest030_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrAccessDenied)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest030_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest030_02::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, ETrue);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 1;
	pref2.iDirection = ECommDbConnectionDirectionIncoming;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 3;
	
	iPrefView->InsertConnectionPreferenceL(pref2, ETrue);

	User::LeaveIfError(iPrefView->GotoFirstRecord());
	
	CCommsDbConnectionPrefTableView::TCommDbIapBearer newBearer;	//Set new bearer to point at other outgoing Iap in commdb
	newBearer.iBearerSet=KCommDbBearerLAN|KCommDbBearerCSD;
	newBearer.iIapId=2;

	TRAPD(err, iPrefView->UpdateBearerL(newBearer));
	return err;
	}
	
TVerdict CCommDbTest030_02::doTestStepPostambleL()
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

CCommDbTest030_03::CCommDbTest030_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_030_03"));
	}

CCommDbTest030_03::~CCommDbTest030_03()
	{}

TVerdict CCommDbTest030_03::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 030.03 called "));

	SetTestStepResult(EPass);
	CCommDbTest030_01* step030_01 = new(ELeave) CCommDbTest030_01;
	CleanupStack::PushL(step030_01);
	doTestStepWithHeapFailureL( *step030_01, KErrNone);
	CleanupStack::PopAndDestroy(step030_01);

	CCommDbTest030_02* step030_02 = new(ELeave) CCommDbTest030_02;
	CleanupStack::PushL(step030_02);
	doTestStepWithHeapFailureL( *step030_02, KErrAccessDenied);
	CleanupStack::PopAndDestroy(step030_02);

	return TestStepResult();
	}

