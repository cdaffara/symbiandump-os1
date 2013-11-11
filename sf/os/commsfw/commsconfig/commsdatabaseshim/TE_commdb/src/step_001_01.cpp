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
// This contains CommDb Unit Test Case 001.01

// EPOC includes
#include <e32base.h>

// Test system includes
#include "Teststepcommdb.h"
#include "step_001_01.h"

#include <commdb.h>

// constructor
CCommDbTest001_01::CCommDbTest001_01()
{
	// store the name of this test case
	SetTestStepName(_L("step_001_01"));
}

// destructor
CCommDbTest001_01::~CCommDbTest001_01()
{
}

TVerdict CCommDbTest001_01::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest001_01::executeStepL()
	{
	CCommsDatabase* TheDb = NULL;
	TRAPD( err, TheDb=CCommsDatabase::NewL() );
	delete TheDb;
	return err;
	}

// do Test step 001.01
TVerdict CCommDbTest001_01::doTestStepL( )
{
	TInt res;
	INFO_PRINTF1(_L("Step 001.01 called "));
	res = executeStepL();
	if (res == KErrNone)
		{
		SetTestStepResult(EPass);		}
	else
		{
		INFO_PRINTF2(_L("Step 001.01 failed with error: %d"),res);
		SetTestStepResult(EFail);		}

	return TestStepResult();
}

// constructor
CCommDbTest001_03::CCommDbTest001_03()
{
	// store the name of this test case
	SetTestStepName(_L("step_001_03"));
}

// destructor
CCommDbTest001_03::~CCommDbTest001_03()
{
}

TInt CCommDbTest001_03::executeStepL()
{
	TInt ret = KErrNone;

	CCommsDatabase* TheDb = NULL;
	TRAPD( err, TheDb=CCommsDatabase::NewL() );
	if (err != KErrNone)
		ret = err;

	delete TheDb;
	return ret;
}

// do Test step 001.03
TVerdict CCommDbTest001_03::doTestStepL( )
{

	INFO_PRINTF1(_L("Step 001.03 called "));

	if (executeStepL() == KErrNone)
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
}

// constructor
CCommDbTest001_04::CCommDbTest001_04()
{
	// store the name of this test case
	SetTestStepName(_L("step_001_04"));
}

// destructor
CCommDbTest001_04::~CCommDbTest001_04()
{
}

TVerdict CCommDbTest001_04::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest001_04::executeStepL()
{
	CCommsDatabase* TheDb = NULL;
	TRAPD( err, TheDb=CCommsDatabase::NewL() );

	delete TheDb;

	return err;
}

// do Test step 001.04
TVerdict CCommDbTest001_04::doTestStepL( )
{

	INFO_PRINTF1(_L("Step 001.04 called "));
    if (executeStepL() == KErrNone)
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
}

// constructor
CCommDbTest001_05::CCommDbTest001_05()
{
	// store the name of this test case
	SetTestStepName(_L("step_001_05"));
}

// destructor
CCommDbTest001_05::~CCommDbTest001_05()
{
}

TVerdict CCommDbTest001_05::doTestStepPreambleL()
	{
	// delete database file before running this test
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest001_05::executeStepL()
{
	TInt ret = KErrGeneral;

	CCommsDatabase* TheDb;
	TCommDbOpeningMethod openingMethod;
	TheDb=CCommsDatabase::NewL(openingMethod);

	if ( openingMethod == ECommDbOpened )
		ret = KErrNone;

	delete TheDb;

	return ret;
}

// do Test step 001.05
TVerdict CCommDbTest001_05::doTestStepL( )
{

	INFO_PRINTF1(_L("Step 001.05 called "));

	if (executeStepL() == KErrNone)
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
}

// constructor
CCommDbTest001_07::CCommDbTest001_07()
{
	// store the name of this test case
	SetTestStepName(_L("step_001_07"));
}

// destructor
CCommDbTest001_07::~CCommDbTest001_07()
{
}

TInt CCommDbTest001_07::executeStepL()
{
	// make sure database exists before running this test
	TInt ret = KErrGeneral;

	copyTestDatabase( EDatabase_1 );

	CCommsDatabase* TheDb;
	TCommDbOpeningMethod openingMethod;
	TheDb=CCommsDatabase::NewL(openingMethod);

	if ( openingMethod == ECommDbOpened )
		ret = KErrNone;

	delete TheDb;

	return ret;
}

// do Test step 001.07
TVerdict CCommDbTest001_07::doTestStepL( )
{

	INFO_PRINTF1(_L("Step 001.07 called "));

	if (executeStepL() == KErrNone)
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
}

// constructor
CCommDbTest001_08::CCommDbTest001_08()
{
	// store the name of this test case
	SetTestStepName(_L("step_001_08"));
}

// destructor
CCommDbTest001_08::~CCommDbTest001_08()
{
}

TInt CCommDbTest001_08::executeStepL()
{
	TInt ret = KErrNone;

	CCommsDatabase* TheDb = NULL;
	TRAPD( err, TheDb=CCommsDatabase::NewL(EFalse) );
	if (err != KErrNone)
		ret = err;

	// if we get here, test was successful
	delete TheDb;

	return ret;
}

// do Test step 001.08
TVerdict CCommDbTest001_08::doTestStepL( )
{

	INFO_PRINTF1(_L("Step 001.08 called "));

	if (executeStepL() == KErrNone)
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
}

// constructor
CCommDbTest001_09::CCommDbTest001_09()
{
	// store the name of this test case
	SetTestStepName(_L("step_001_09"));
}

// destructor
CCommDbTest001_09::~CCommDbTest001_09()
{
}

TInt CCommDbTest001_09::executeStepL()
{
	TInt ret = KErrNone;

	CCommsDatabase* TheDb = NULL;
	TRAPD( err, TheDb=CCommsDatabase::NewL(ETrue) );
	if (err != KErrNone)
		ret = err;

	// if we get here, test was successful
	delete TheDb;

	return ret;
}

// do Test step 001.09
TVerdict CCommDbTest001_09::doTestStepL( )
{

	INFO_PRINTF1(_L("Step 001.09 called "));

	if (executeStepL() == KErrNone)
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
}

// constructor
CCommDbTest001_10::CCommDbTest001_10()
{
	// store the name of this test case
	SetTestStepName(_L("step_001_10"));
}

// destructor
CCommDbTest001_10::~CCommDbTest001_10()
{
}

// do Test step 001.10
TVerdict CCommDbTest001_10::doTestStepL( )
{

	INFO_PRINTF1(_L("Step 001.10 called "));

	SetTestStepResult(EPass);
	//Heap test for 001.01

	CCommDbTest001_01* step001_01 = new CCommDbTest001_01;
	CleanupStack::PushL(step001_01);
	doTestStepWithHeapFailureL( *step001_01, KErrNone);
	if( step001_01->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step001_01);

	//Heap test for 001.03

	CCommDbTest001_03* step001_03 = new CCommDbTest001_03;
	CleanupStack::PushL(step001_03);
	doTestStepWithHeapFailureL( *step001_03, KErrNone);
	if( step001_03->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step001_03);

	//Heap test for 001.04

	CCommDbTest001_04* step001_04 = new CCommDbTest001_04;
	CleanupStack::PushL(step001_04);
	doTestStepWithHeapFailureL( *step001_04, KErrNone);
	if( step001_04->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step001_04);

	//Heap test for 001.05

	CCommDbTest001_05* step001_05 = new CCommDbTest001_05;
	CleanupStack::PushL(step001_05);
	doTestStepWithHeapFailureL( *step001_05, KErrNone);
	if( step001_05->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step001_05);

	//Heap test for 001.07

	CCommDbTest001_07* step001_07 = new CCommDbTest001_07;
	CleanupStack::PushL(step001_07);
	doTestStepWithHeapFailureL( *step001_07, KErrNone);
	if( step001_07->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step001_07);

	//Heap test for 001.08

	CCommDbTest001_08* step001_08 = new CCommDbTest001_08;
	CleanupStack::PushL(step001_08);
	doTestStepWithHeapFailureL( *step001_08, KErrNone);
	if( step001_08->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step001_08);

	//Heap test for 001.09

	CCommDbTest001_09* step001_09 = new CCommDbTest001_09;
	CleanupStack::PushL(step001_09);
	doTestStepWithHeapFailureL( *step001_09, KErrNone);
	if( step001_09->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step001_09);

	return TestStepResult();
}
