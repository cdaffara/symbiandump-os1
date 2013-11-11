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
// This contains CommDb Unit Test Cases 006.01 - 006.05

// EPOC includes
#include <e32base.h>
#include <commdb.h>


// Test system includes
#include "Teststepcommdb.h"
#include "step_006_xx.h"



/////////////////////
//	Test step 006.001
/////////////////////

// constructor
CCommDbTest006_01::CCommDbTest006_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_006_01"));
	}

// destructor
CCommDbTest006_01::~CCommDbTest006_01()
	{
	}

TVerdict CCommDbTest006_01::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest006_01::executeStepL()
	{
	TInt ret = iTheView->GotoFirstRecord(); 	
	return ret;
	}

TVerdict CCommDbTest006_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 006.01 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);	
	return TestStepResult();
	}


/////////////////////
//	Test step 006.002
/////////////////////

// constructor
CCommDbTest006_02::CCommDbTest006_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_006_02"));
	// default to no local objects allocated
	iLocalObjects = EFalse;
	}

// destructor
CCommDbTest006_02::~CCommDbTest006_02()
	{
	}

TVerdict CCommDbTest006_02::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(IAP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest006_02::executeStepL()
	{
	TInt ret = iTheView->GotoFirstRecord(); 	
	return ret;
	}

TVerdict CCommDbTest006_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 006.02 called "));
	
	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);	
	return TestStepResult();
	}

/////////////////////
//	Test step 006.003
/////////////////////

// constructor
CCommDbTest006_03::CCommDbTest006_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_006_03"));
	}

// destructor
CCommDbTest006_03::~CCommDbTest006_03()
	{
	}

TVerdict CCommDbTest006_03::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_IN_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest006_03::executeStepL()
	{
	TInt ret = iTheView->GotoFirstRecord(); 	
	return ret;
	}

TVerdict CCommDbTest006_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 006.03 called "));

	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);	
		
	return TestStepResult();
	}


/////////////////////
//	Test step 006.004
/////////////////////

// constructor
CCommDbTest006_04::CCommDbTest006_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_006_04"));
	// default to no local objects allocated
	iLocalObjects = EFalse;
	}

// destructor
CCommDbTest006_04::~CCommDbTest006_04()
	{
	}


TVerdict CCommDbTest006_04::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(IAP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest006_04::executeStepL()
	{
	TInt ret = iTheView->GotoFirstRecord(); 	
	return ret;
	}

TVerdict CCommDbTest006_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 006.04 called "));
	
	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);	
	return TestStepResult();
	}


/////////////////////
//	Test step 006.005
/////////////////////

// constructor
CCommDbTest006_05::CCommDbTest006_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_006_05"));
	}

// destructor
CCommDbTest006_05::~CCommDbTest006_05()
	{
	}


TVerdict CCommDbTest006_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 006.04 called "));
	
	SetTestStepResult(EPass);
	//Heap test for 006.01
	CCommDbTest006_01* step006_01 = new CCommDbTest006_01;
	CleanupStack::PushL(step006_01);
	doTestStepWithHeapFailureL( *step006_01, KErrNone);
	if( step006_01->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step006_01);
		
	//Heap test for 006.02
	
	CCommDbTest006_02* step006_02 = new CCommDbTest006_02;
	CleanupStack::PushL(step006_02);
	doTestStepWithHeapFailureL( *step006_02, KErrNotFound);
	if( step006_02->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step006_02);


	
	//Heap test for 006.03
// it is not possible to copy database files, so this tests couldn't run 
// with proper conditions(database)
	//Heap test for 006.04
	CCommDbTest006_04* step006_04 = new CCommDbTest006_04;
	CleanupStack::PushL(step006_04);
	doTestStepWithHeapFailureL( *step006_04, KErrNotFound);
	if( step006_04->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step006_04);


	return TestStepResult();
	}
