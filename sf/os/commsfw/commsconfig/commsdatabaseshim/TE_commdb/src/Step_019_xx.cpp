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
// This contains CommDb Unit Test Cases 019.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_019_xx.h"



/////////////////////
//	Test step 019.01
/////////////////////

// constructor
CCommDbTest019_01::CCommDbTest019_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_01"));
	}

// destructor
CCommDbTest019_01::~CCommDbTest019_01()
	{
	}


TVerdict CCommDbTest019_01::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest019_01::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Select a view and verify the records in it

	//This should just select 2 records
	iTheView = iTheDb->OpenViewMatchingUintLC( TPtrC(DIAL_OUT_ISP), TPtrC(ISP_TYPE), EIspTypeInternetAndWAP );
	CleanupStack::Pop();
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//Check its the record we expect
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #1")) )
		{
		//There should be no more records in the view
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
		ret = KErrNone;
		}
	
	return ret;
	}



TVerdict CCommDbTest019_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.01 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 019.02
/////////////////////

// constructor
CCommDbTest019_02::CCommDbTest019_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_02"));
	}

// destructor
CCommDbTest019_02::~CCommDbTest019_02()
	{
	}


TVerdict CCommDbTest019_02::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest019_02::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Select a view and verify the records in it

	//This should just select 2 records
	iTheView = iTheDb->OpenViewMatchingBoolLC( TPtrC(DIAL_OUT_ISP), TPtrC(ISP_PROMPT_FOR_LOGIN), EFalse );
	CleanupStack::Pop();
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//Check its the record we expect
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #1")) )
		{
			// Move onto the second record and check that
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
			iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
			if ( !value.Compare(_L("Record #2")) )
				{
				//There should be no more records in the view
				CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
				ret = KErrNone;
				}
		}

	
	return ret;
	}



TVerdict CCommDbTest019_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.02 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 019.03
/////////////////////

// constructor
CCommDbTest019_03::CCommDbTest019_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_03"));
	}

// destructor
CCommDbTest019_03::~CCommDbTest019_03()
	{
	}


TVerdict CCommDbTest019_03::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest019_03::executeStepL()
	{
	TInt ret=KErrNone;
	//Select a view and verify the records in it

	//This should just select 2 records
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_CALLBACK_INFO), _L8("Call me back") );
	CleanupStack::Pop();
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//Check its the record we expect
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #1")) )
		{
			// Move onto the second record and check that
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
			iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
			if ( !value.Compare(_L("Record #3")) )
				{
				//There should be no more records in the view
				CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
				ret = KErrNone;
				}
		}

	return ret;
	}



TVerdict CCommDbTest019_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.03 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 019.04
/////////////////////

// constructor
CCommDbTest019_04::CCommDbTest019_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_04"));
	}

// destructor
CCommDbTest019_04::~CCommDbTest019_04()
	{
	}


TVerdict CCommDbTest019_04::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest019_04::executeStepL()
	{
	TInt ret=KErrNone;
	//Select a view and verify the records in it

	//This should just select 2 records
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_CALLBACK_INFO), _L("Call me back") );
	CleanupStack::Pop();
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//Check its the record we expect
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #1")) )
		{
			// Move onto the second record and check that
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
			iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
			if ( !value.Compare(_L("Record #3")) )
				{
				//There should be no more records in the view
				CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
				ret = KErrNone;
				}
		}

	return ret;
	}



TVerdict CCommDbTest019_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.04 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 019.05
/////////////////////

// constructor
CCommDbTest019_05::CCommDbTest019_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_05"));
	}

// destructor
CCommDbTest019_05::~CCommDbTest019_05()
	{
	}


TVerdict CCommDbTest019_05::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest019_05::executeStepL()
	{
	TInt ret=KErrGeneral;
	//Select a view on an invalid table

	TRAPD(r, iTheView = iTheDb->OpenViewMatchingTextLC( _L("Invalid"), TPtrC(ISP_IF_CALLBACK_INFO), _L8("Call me back") ) );
	if ( r== KErrNone )
		{
		//The test has failed as the open view did not leave
		CleanupStack::Pop();
		}
		else
		{
			//The call did leave
			ret = r;
		}
	
	return ret;
	}



TVerdict CCommDbTest019_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.05 called "));


	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 019.06
/////////////////////

// constructor
CCommDbTest019_06::CCommDbTest019_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_06"));
	}

// destructor
CCommDbTest019_06::~CCommDbTest019_06()
	{
	}


TVerdict CCommDbTest019_06::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest019_06::executeStepL()
	{
	TInt ret=KErrGeneral;
	//Select a view on an invalid table

	TRAPD(r, iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(DIAL_OUT_ISP), _L("Invalid"), _L8("Call me back") ) );
	if ( r== KErrNone )
		{
		//The test has failed as the open view did not leave
		CleanupStack::Pop();
		}
		else
		{
			//The call did leave
			ret = r;
		}
	
	return ret;
	}



TVerdict CCommDbTest019_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.06 called "));


	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 019.07
/////////////////////

// constructor
CCommDbTest019_07::CCommDbTest019_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_07"));
	}

// destructor
CCommDbTest019_07::~CCommDbTest019_07()
	{
	}


TVerdict CCommDbTest019_07::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest019_07::executeStepL()
	{
	TInt ret=KErrNone;
	//Select a view and verify the records in it

	//This should just select 3 records
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(DIAL_OUT_ISP), TPtrC(ISP_LOGIN_NAME), _L8("") );
	CleanupStack::Pop();
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//Check its the record we expect
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #1")) )
		{
			// Move onto the second record and check that
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
			iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
			if ( !value.Compare(_L("Record #2")) )
				{
				//There should be no more records in the view
				CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
				iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
				if ( !value.Compare(_L("Record #3")) )
					{
					CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
					ret = KErrNone;
					}
				}
		}

	return ret;
	}



TVerdict CCommDbTest019_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.07 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 019.08
/////////////////////

// constructor
CCommDbTest019_08::CCommDbTest019_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_08"));
	}

// destructor
CCommDbTest019_08::~CCommDbTest019_08()
	{
	}

TVerdict CCommDbTest019_08::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.08 called "));

	SetTestStepResult(EPass);
		// Heap test for 019.01

	CCommDbTest019_01* step019_01 = new(ELeave) CCommDbTest019_01;
	CleanupStack::PushL(step019_01);
	if ( doTestStepWithHeapFailureL( *step019_01, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step019_01);


	return TestStepResult();	

}


/////////////////////
//	Test step 019.09
/////////////////////

// constructor
CCommDbTest019_09::CCommDbTest019_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_09"));
	}

// destructor
CCommDbTest019_09::~CCommDbTest019_09()
	{
	}

TVerdict CCommDbTest019_09::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.09 called "));

	SetTestStepResult(EPass);	
			// Heap test for 019.02

	CCommDbTest019_02* step019_02 = new CCommDbTest019_02;
	CleanupStack::PushL(step019_02);
	if ( doTestStepWithHeapFailureL( *step019_02, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step019_02);
	
	return TestStepResult();
	
	}
	
/////////////////////
//	Test step 019.10
/////////////////////

// constructor
CCommDbTest019_10::CCommDbTest019_10()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_10"));
	}

// destructor
CCommDbTest019_10::~CCommDbTest019_10()
	{
	}

TVerdict CCommDbTest019_10::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.10 called "));

	SetTestStepResult(EPass);	
		// Heap test for 019.03

	CCommDbTest019_03* step019_03 = new CCommDbTest019_03;
	CleanupStack::PushL(step019_03);
	if ( doTestStepWithHeapFailureL( *step019_03, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step019_03);
	
	return TestStepResult();
	
	}
	
/////////////////////
//	Test step 019.11
/////////////////////

// constructor
CCommDbTest019_11::CCommDbTest019_11()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_11"));
	}

// destructor
CCommDbTest019_11::~CCommDbTest019_11()
	{
	}

TVerdict CCommDbTest019_11::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.11 called "));

	SetTestStepResult(EPass);	
		// Heap test for 019.04

	CCommDbTest019_04* step019_04 = new CCommDbTest019_04;
	CleanupStack::PushL(step019_04);
	if ( doTestStepWithHeapFailureL( *step019_04, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step019_04);
	
	return TestStepResult();
	
	}

/////////////////////
//	Test step 019.12
/////////////////////

// constructor
CCommDbTest019_12::CCommDbTest019_12()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_12"));
	}

// destructor
CCommDbTest019_12::~CCommDbTest019_12()
	{
	}

TVerdict CCommDbTest019_12::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.12 called "));

	SetTestStepResult(EPass);	
		// Heap test for 019.05

	CCommDbTest019_05* step019_05 = new CCommDbTest019_05;
	CleanupStack::PushL(step019_05);
	if ( doTestStepWithHeapFailureL( *step019_05, KErrNotFound) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step019_05);
	
	
	return TestStepResult();
	
	}
	
/////////////////////
//	Test step 019.13
/////////////////////

// constructor
CCommDbTest019_13::CCommDbTest019_13()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_13"));
	}

// destructor
CCommDbTest019_13::~CCommDbTest019_13()
	{
	}

TVerdict CCommDbTest019_13::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.13 called "));

	SetTestStepResult(EPass);	
		// Heap test for 019.06

	CCommDbTest019_06* step019_06 = new CCommDbTest019_06;
	CleanupStack::PushL(step019_06);
	if ( doTestStepWithHeapFailureL( *step019_06, KErrNotFound) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step019_06);
	
	
	return TestStepResult();
	
	}
	
/////////////////////
//	Test step 019.14
/////////////////////

// constructor
CCommDbTest019_14::CCommDbTest019_14()
	{
	// store the name of this test case
	SetTestStepName(_L("step_019_14"));
	}

// destructor
CCommDbTest019_14::~CCommDbTest019_14()
	{
	}

TVerdict CCommDbTest019_14::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 019.14 called "));

	SetTestStepResult(EPass);	
		// Heap test for 019.07

	CCommDbTest019_07* step019_07 = new CCommDbTest019_07;
	CleanupStack::PushL(step019_07);
	if ( doTestStepWithHeapFailureL( *step019_07, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step019_07);
	
	return TestStepResult();
	
	}

//EOF
