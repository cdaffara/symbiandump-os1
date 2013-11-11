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
// This contains CommDb Unit Test Cases 025.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_025_xx.h"


CCommDbTest025_01::CCommDbTest025_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_025_01"));
	}

CCommDbTest025_01::~CCommDbTest025_01()
	{}


TVerdict CCommDbTest025_01::doTestStepL( void )
	{
	executeStepL();	//Always returns KErrNone or leaves.
        SetTestStepResult(EPass);	return TestStepResult();	//So if we got to here we must have passed.
	}

TInt CCommDbTest025_01::executeStepL()
	{
	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();
	return KErrNone;
	}

TVerdict CCommDbTest025_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest025_01::doTestStepPostambleL()
	{
	delete iPrefView;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////
// Test 25.02
//////////////////////////////////////////////////////////////////////////

CCommDbTest025_02::CCommDbTest025_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_025_02"));
	}

CCommDbTest025_02::~CCommDbTest025_02()
	{}

TVerdict CCommDbTest025_02::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 025.02 called "));

	SetTestStepResult(EPass);
	CCommDbTest025_01* step025_01 = new(ELeave) CCommDbTest025_01;
	CleanupStack::PushL(step025_01);
	doTestStepWithHeapFailureL( *step025_01, KErrNone);
	CleanupStack::PopAndDestroy(step025_01);

	return TestStepResult();
	}

