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
// This contains CommDb Unit Test Cases 033.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_033_xx.h"


CCommDbTest033_01::CCommDbTest033_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_033_01"));
	}

CCommDbTest033_01::~CCommDbTest033_01()
	{}

TVerdict CCommDbTest033_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();	//If we've got this far we've passed
	}

TVerdict CCommDbTest033_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest033_01::executeStepL()
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

	iPrefView->SwapConnectionPreferencesL(ECommDbConnectionDirectionOutgoing, 1, 2);

	User::LeaveIfError(iPrefView->GotoFirstRecord());

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	TBool rank1Found=EFalse;
	if(retrievedPref.iRanking==1)	//should be pref2 with new ranking
		{
		rank1Found=ETrue;
		if(pref2.iBearer.iIapId!=retrievedPref.iBearer.iIapId)
			return KErrGeneral;		//something has gone wrong with the swap
		}
	else if(retrievedPref.iRanking==2)	//should be pref1 with new ranking
		{
		if(pref1.iBearer.iIapId!=retrievedPref.iBearer.iIapId)
			return KErrGeneral;		//something has gone wrong with the swap
		}
	else
		return KErrGeneral;			//We've got a rank other that 1&2 which is wrong.

	User::LeaveIfError(iPrefView->GotoNextRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	if(retrievedPref.iRanking==1)
		{
		if(rank1Found)
			return KErrGeneral;		//we've found 2 rank 1 records which is wrong.
		if(pref2.iBearer.iIapId!=retrievedPref.iBearer.iIapId)
			return KErrGeneral;		//something has gone wrong with the swap
		}
	else if (retrievedPref.iRanking==2)
		{
		if(!rank1Found)
			return KErrGeneral;		//we've found 2 rank 2 records which is wrong.
		if(pref1.iBearer.iIapId!=retrievedPref.iBearer.iIapId)
			return KErrGeneral;		//something has gone wrong with the swap
		}
	else 
		return KErrGeneral;
	
	return KErrNone;
	}
	
TVerdict CCommDbTest033_01::doTestStepPostambleL()
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

CCommDbTest033_02::CCommDbTest033_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_033_02"));
	}

CCommDbTest033_02::~CCommDbTest033_02()
	{}

TVerdict CCommDbTest033_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();	//If we've got this far we've passed
	}

TVerdict CCommDbTest033_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest033_02::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 0;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	iPrefView->SwapConnectionPreferencesL(ECommDbConnectionDirectionOutgoing, 1, 0);

	User::LeaveIfError(iPrefView->GotoFirstRecord());

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref retrievedPref;
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	TBool rank1Found=EFalse;
	if(retrievedPref.iRanking==1)	//should be pref2 with new ranking
		{
		rank1Found=ETrue;
		if(pref2.iBearer.iIapId!=retrievedPref.iBearer.iIapId)
			return KErrGeneral;		//something has gone wrong with the swap
		}
	else if(retrievedPref.iRanking==0)	//should be pref1 with new ranking
		{
		if(pref1.iBearer.iIapId!=retrievedPref.iBearer.iIapId)
			return KErrGeneral;		//something has gone wrong with the swap
		}
	else
		return KErrGeneral;			//We've got a rank other that 1&0 which is wrong.

	User::LeaveIfError(iPrefView->GotoNextRecord());
	iPrefView->ReadConnectionPreferenceL(retrievedPref);

	if(retrievedPref.iRanking==1)
		{
		if(rank1Found)
			return KErrGeneral;		//we've found 2 rank 1 records which is wrong.
		if(pref2.iBearer.iIapId!=retrievedPref.iBearer.iIapId)
			return KErrGeneral;		//something has gone wrong with the swap
		}
	else if (retrievedPref.iRanking==0)
		{
		if(!rank1Found)
			return KErrGeneral;		//we've found 2 rank 0 records which is wrong.
		if(pref1.iBearer.iIapId!=retrievedPref.iBearer.iIapId)
			return KErrGeneral;		//something has gone wrong with the swap
		}
	else 
		return KErrGeneral;
	
	return KErrNone;
	}
	
TVerdict CCommDbTest033_02::doTestStepPostambleL()
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

CCommDbTest033_03::CCommDbTest033_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_033_03"));
	}

CCommDbTest033_03::~CCommDbTest033_03()
	{}

TVerdict CCommDbTest033_03::doTestStepL( void )
	{
	if (executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();	//If we've got this far we've passed
	}

TVerdict CCommDbTest033_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest033_03::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	TRAPD(err, iPrefView->SwapConnectionPreferencesL(ECommDbConnectionDirectionOutgoing, 1, 2));

	return err;
	}
	
TVerdict CCommDbTest033_03::doTestStepPostambleL()
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

CCommDbTest033_04::CCommDbTest033_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_033_04"));
	}

CCommDbTest033_04::~CCommDbTest033_04()
	{}

TVerdict CCommDbTest033_04::doTestStepL( void )
	{
	if (executeStepL()!=KErrAccessDenied)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();	//If we've got this far we've passed
	}

TVerdict CCommDbTest033_04::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest033_04::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, ETrue);	//Make this entry read only

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 2;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	TRAPD(err, iPrefView->SwapConnectionPreferencesL(ECommDbConnectionDirectionOutgoing, 1, 2));

	return err;
	}
	
TVerdict CCommDbTest033_04::doTestStepPostambleL()
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

CCommDbTest033_05::CCommDbTest033_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_033_05"));
	}

CCommDbTest033_05::~CCommDbTest033_05()
	{}

TVerdict CCommDbTest033_05::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 033.05 called "));

	SetTestStepResult(EPass);
	CCommDbTest033_01* step033_01 = new(ELeave) CCommDbTest033_01;
	CleanupStack::PushL(step033_01);
	doTestStepWithHeapFailureL( *step033_01, KErrNone);
	CleanupStack::PopAndDestroy(step033_01);

	
	return TestStepResult();
	}


//////////////////////////////////////////////////////////////////////////

CCommDbTest033_06::CCommDbTest033_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_033_06"));
	}

CCommDbTest033_06::~CCommDbTest033_06()
	{}

TVerdict CCommDbTest033_06::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 033.06 called "));

	SetTestStepResult(EPass);	
	CCommDbTest033_02* step033_02 = new(ELeave) CCommDbTest033_02;
	CleanupStack::PushL(step033_02);
	doTestStepWithHeapFailureL( *step033_02, KErrNone);
	CleanupStack::PopAndDestroy(step033_02);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest033_07::CCommDbTest033_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_033_07"));
	}

CCommDbTest033_07::~CCommDbTest033_07()
	{}

TVerdict CCommDbTest033_07::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 033.07 called "));

	SetTestStepResult(EPass);	
	CCommDbTest033_03* step033_03 = new(ELeave) CCommDbTest033_03;
	CleanupStack::PushL(step033_03);
	doTestStepWithHeapFailureL( *step033_03, KErrNotFound);
	CleanupStack::PopAndDestroy(step033_03);

	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest033_08::CCommDbTest033_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_033_08"));
	}

CCommDbTest033_08::~CCommDbTest033_08()
	{}

TVerdict CCommDbTest033_08::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 033.08 called "));

	SetTestStepResult(EPass);	
	CCommDbTest033_04* step033_04 = new(ELeave) CCommDbTest033_04;
	CleanupStack::PushL(step033_04);
	doTestStepWithHeapFailureL( *step033_04, KErrAccessDenied);
	CleanupStack::PopAndDestroy(step033_04);
		
	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest033_09::CCommDbTest033_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_033_09"));
	}

TVerdict CCommDbTest033_09::doTestStepL()
	{
	if (executeStepL() != KErrNone)
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	
	return TestStepResult();
	}

TVerdict CCommDbTest033_09::doTestStepPreambleL()
	{
	iTheDb = CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

	return TestStepResult();
	}

TInt CCommDbTest033_09::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 0;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref2;
	pref2.iRanking = 0;
	pref2.iDirection = ECommDbConnectionDirectionOutgoing;
	pref2.iDialogPref = ECommDbDialogPrefPrompt;
	pref2.iBearer.iBearerSet = KCommDbBearerCSD;
	pref2.iBearer.iIapId = 2;
	
	iPrefView->InsertConnectionPreferenceL(pref2, EFalse);

	TRAPD(leaveCode, iPrefView->SwapConnectionPreferencesL(ECommDbConnectionDirectionOutgoing, 1, 0));
	if (leaveCode != KErrArgument)
		{
		return KErrGeneral;
		}

	return KErrNone;
	}
	
TVerdict CCommDbTest033_09::doTestStepPostambleL()
	{
	//
	// Get rid of our view
	//
	if(iPrefView)
		{
		delete iPrefView;
		iPrefView = NULL;
		}

	//
	// Create a new view that should have the record that was added in executeStepL and
	// may have been left in by a leave condition
	//
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

	return TestStepResult();
	}

//EOF
