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
// This contains CommDb Unit Test Cases 029.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_029_xx.h"


CCommDbTest029_01::CCommDbTest029_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_029_01"));
	}

CCommDbTest029_01::~CCommDbTest029_01()
	{}

TVerdict CCommDbTest029_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest029_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest029_01::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref1;
	pref1.iRanking = 1;
	pref1.iDirection = ECommDbConnectionDirectionOutgoing;
	pref1.iDialogPref = ECommDbDialogPrefPrompt;
	pref1.iBearer.iBearerSet = KCommDbBearerCSD;
	pref1.iBearer.iIapId = 1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);

	User::LeaveIfError(iPrefView->GotoFirstRecord());

	iPrefView->DeleteConnectionPreferenceL();

	return iPrefView->GotoFirstRecord();	//There should now be no records in the connection pref table, so this should be KErrNotFound
	}

TVerdict CCommDbTest029_01::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

///////////////////////////////////////////////////////////////////////////////////

CCommDbTest029_02::CCommDbTest029_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_029_02"));
	}

CCommDbTest029_02::~CCommDbTest029_02()
	{}

TVerdict CCommDbTest029_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrAccessDenied)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest029_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest029_02::executeStepL()
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

	User::LeaveIfError(iPrefView->GotoFirstRecord());

	TRAPD(err, iPrefView->UpdateDialogPrefL(ECommDbDialogPrefDoNotPrompt));	//Trying to edit a read only entry

	return err;
	}

TVerdict CCommDbTest029_02::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

///////////////////////////////////////////////////////////////////////////////////

CCommDbTest029_03::CCommDbTest029_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_029_03"));
	}

CCommDbTest029_03::~CCommDbTest029_03()
	{}

TVerdict CCommDbTest029_03::doTestStepL( void )
	{
	if (executeStepL() != KErrNotFound)
		{
		SetTestStepResult(EFail);		}

	return TestStepResult();
	}

TVerdict CCommDbTest029_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest029_03::executeStepL()
	{
	TRAPD(err, iPrefView->DeleteConnectionPreferenceL());	//This should leave with KErrNotFound

	return err;
	}

TVerdict CCommDbTest029_03::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest029_04::CCommDbTest029_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_029_04"));
	}

CCommDbTest029_04::~CCommDbTest029_04()
	{}

TVerdict CCommDbTest029_04::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 029.04 called "));

	SetTestStepResult(EPass);
	CCommDbTest029_01* step029_01 = new(ELeave) CCommDbTest029_01;
	CleanupStack::PushL(step029_01);
	doTestStepWithHeapFailureL( *step029_01, KErrNotFound);
	CleanupStack::PopAndDestroy(step029_01);

	CCommDbTest029_02* step029_02 = new(ELeave) CCommDbTest029_02;
	CleanupStack::PushL(step029_02);
	doTestStepWithHeapFailureL( *step029_02, KErrAccessDenied);
	CleanupStack::PopAndDestroy(step029_02);

	//Don't bother with step 3, since it's a panic test.

	return TestStepResult();
	}
