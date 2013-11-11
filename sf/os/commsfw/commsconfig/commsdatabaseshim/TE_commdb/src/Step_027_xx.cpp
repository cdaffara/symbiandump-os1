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
// This contains CommDb Unit Test Cases 027.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_027_xx.h"


CCommDbTest027_01::CCommDbTest027_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_027_01"));
	}

CCommDbTest027_01::~CCommDbTest027_01()
	{}

TVerdict CCommDbTest027_01::doTestStepL( void )
	{
	executeStepL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest027_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest027_01::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
	User::LeaveIfError(iPrefView->GotoFirstRecord());
	iPrefView->ReadConnectionPreferenceL(pref);
	return KErrNone;
	}
	
TVerdict CCommDbTest027_01::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest027_02::CCommDbTest027_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_027_02"));
	}

CCommDbTest027_02::~CCommDbTest027_02()
	{}

TVerdict CCommDbTest027_02::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 027.02 called "));

	SetTestStepResult(EPass);
	CCommDbTest027_01* step027_01 = new(ELeave) CCommDbTest027_01;
	CleanupStack::PushL(step027_01);
	doTestStepWithHeapFailureL( *step027_01, KErrNone);
	CleanupStack::PopAndDestroy(step027_01);

	return TestStepResult();
	}

