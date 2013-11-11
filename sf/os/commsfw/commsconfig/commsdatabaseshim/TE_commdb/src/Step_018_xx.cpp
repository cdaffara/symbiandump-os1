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
// This contains CommDb Unit Test Cases 018.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_018_xx.h"



/////////////////////
//	Test step 018.01
/////////////////////

// constructor
CCommDbTest018_01::CCommDbTest018_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_018_01"));
	}

// destructor
CCommDbTest018_01::~CCommDbTest018_01()
	{
	}


TVerdict CCommDbTest018_01::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest018_01::executeStepL()
	{
	TInt ret=KErrNone;

	//Verify DeleteRecord() by deleting the records in the Dial out ISP
	//table and then checking for their lack of existance

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );

	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );

	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );
	//The table will now be empty

	delete iTheView;
	iTheView = 0;
	iTheView = iTheDb->OpenTableLC(TPtrC(DIAL_OUT_ISP ) );
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNotFound );
	
	return ret;
	}



TVerdict CCommDbTest018_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 018.01 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 018.02
/////////////////////

// constructor
CCommDbTest018_02::CCommDbTest018_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_018_02"));
	}

// destructor
CCommDbTest018_02::~CCommDbTest018_02()
	{
	}


TVerdict CCommDbTest018_02::doTestStepPreambleL()
	{	
	copyTestDatabase( EDatabase_1 );
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest018_02::executeStepL()
	{
	TInt ret=KErrNone;

	//Try to delete the last record in a view and then 
	//verify its deletion

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	//The DAIL_OUT_ISP table only has 3 records, so we know
	//we are at the end.

	//Delete the last record
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );

	//The table should now only have 2 records
	
	delete iTheView;
	iTheView = 0;
	iTheView = iTheDb->OpenTableLC(TPtrC(DIAL_OUT_ISP ) );
	CleanupStack::Pop();
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound ); //no more records
	
	return ret;
	}



TVerdict CCommDbTest018_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 018.02 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 018.03
/////////////////////

// constructor
CCommDbTest018_03::CCommDbTest018_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_018_03"));
	}

// destructor
CCommDbTest018_03::~CCommDbTest018_03()
	{
	}


TVerdict CCommDbTest018_03::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(IAP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest018_03::executeStepL()
	{
	TInt ret=KErrNone;

	//Delete record in empty view
	CDBLEAVE( iTheView->DeleteRecord(), KErrNotFound );	

	return ret;
	}



TVerdict CCommDbTest018_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 018.03 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 018.04
/////////////////////

// constructor
CCommDbTest018_04::CCommDbTest018_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_018_04"));
	}

// destructor
CCommDbTest018_04::~CCommDbTest018_04()
	{
	}


TVerdict CCommDbTest018_04::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_IN_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest018_04::executeStepL()
	{
	TInt ret=KErrNone;

	//Delete a read only record 

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );

	//Delete the read only record #5
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );

	//Open a new view and check that we've only 4 records left
	delete iTheView;
	iTheView = 0;
	iTheView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP ) );
	CleanupStack::Pop();
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound ); //no more records
	

	return ret;
	}



TVerdict CCommDbTest018_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 018.04 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 018.05
/////////////////////

// constructor
CCommDbTest018_05::CCommDbTest018_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_018_05"));
	}

// destructor
CCommDbTest018_05::~CCommDbTest018_05()
	{
	}

TVerdict CCommDbTest018_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 018.05 called "));

	SetTestStepResult(EPass);
		// Heap test for 018.01

	CCommDbTest018_01* step018_01 = new(ELeave) CCommDbTest018_01;
	CleanupStack::PushL(step018_01);
	if ( doTestStepWithHeapFailureL( *step018_01, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step018_01);


	return TestStepResult();	
}


/////////////////////
//	Test step 018.06
/////////////////////

// constructor
CCommDbTest018_06::CCommDbTest018_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_018_06"));
	}

// destructor
CCommDbTest018_06::~CCommDbTest018_06()
	{
	}

TVerdict CCommDbTest018_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 018.06 called "));

	SetTestStepResult(EPass);	
			// Heap test for 018.02

	CCommDbTest018_02* step018_02 = new CCommDbTest018_02;
	CleanupStack::PushL(step018_02);
	if ( doTestStepWithHeapFailureL( *step018_02, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step018_02);
	
	return TestStepResult();		
	}
	
/////////////////////
//	Test step 018.07
/////////////////////

// constructor
CCommDbTest018_07::CCommDbTest018_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_018_07"));
	}

// destructor
CCommDbTest018_07::~CCommDbTest018_07()
	{
	}

TVerdict CCommDbTest018_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 018.07 called "));

	SetTestStepResult(EPass);	
		// Heap test for 018.03

	CCommDbTest018_03* step018_03 = new CCommDbTest018_03;
	CleanupStack::PushL(step018_03);
	if ( doTestStepWithHeapFailureL( *step018_03, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step018_03);
	
	return TestStepResult();		
	}
	
/////////////////////
//	Test step 018.08
/////////////////////

// constructor
CCommDbTest018_08::CCommDbTest018_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_018_08"));
	}

// destructor
CCommDbTest018_08::~CCommDbTest018_08()
	{
	}

TVerdict CCommDbTest018_08::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 018.08 called "));

	SetTestStepResult(EPass);	
		// Heap test for 018.04

	CCommDbTest018_04* step018_04 = new CCommDbTest018_04;
	CleanupStack::PushL(step018_04);
	if ( doTestStepWithHeapFailureL( *step018_04, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step018_04);
	
	return TestStepResult();		
	}
	

//EOF
