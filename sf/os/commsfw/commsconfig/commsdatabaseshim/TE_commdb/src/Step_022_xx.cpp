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
// This contains CommDb Unit Test Cases 022.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_022_xx.h"


/////////////////////
//	Test step 022.01
/////////////////////

// constructor
CCommDbTest022_01::CCommDbTest022_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_01"));
	}

// destructor
CCommDbTest022_01::~CCommDbTest022_01()
	{
	}


TVerdict CCommDbTest022_01::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_01::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Open a view on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerCSD, ECommDbConnectionDirectionOutgoing);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #1")) )
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		if ( !value.Compare(_L("Record #2")) )
		{
			//There should be no more records
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
			ret = KErrNone;
		}
	}
	
	return ret;
	}


TVerdict CCommDbTest022_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.01 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}




/////////////////////
//	Test step 022.02
/////////////////////

// constructor
CCommDbTest022_02::CCommDbTest022_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_02"));
	}

// destructor
CCommDbTest022_02::~CCommDbTest022_02()
	{
	}


TVerdict CCommDbTest022_02::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_02::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Open a view on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerCSD, ECommDbConnectionDirectionIncoming);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #3")) )
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		if ( !value.Compare(_L("Record #4")) )
			{
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
			iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
			if ( !value.Compare(_L("Record #6")) )
				{
				//There should be no more records
				CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
				ret = KErrNone;
				}
			}
	}
	
	return ret;
	}


TVerdict CCommDbTest022_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.02 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 022.03
/////////////////////

// constructor
CCommDbTest022_03::CCommDbTest022_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_03"));
	}

// destructor
CCommDbTest022_03::~CCommDbTest022_03()
	{
	}


TVerdict CCommDbTest022_03::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_03::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Open a view on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerWcdma, ECommDbConnectionDirectionOutgoing);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #5")) )
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		if ( !value.Compare(_L("Record #7")) )
		{
			//There should be no more records
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
			ret = KErrNone;
		}
	}
	
	return ret;
	}


TVerdict CCommDbTest022_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.03 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 022.04
/////////////////////

// constructor
CCommDbTest022_04::CCommDbTest022_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_04"));
	}

// destructor
CCommDbTest022_04::~CCommDbTest022_04()
	{
	}


TVerdict CCommDbTest022_04::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_04::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Open a view on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerCSD, ECommDbConnectionDirectionOutgoing);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #1")) )
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		if ( !value.Compare(_L("Record #2")) )
		{
			//There should be no more records
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
			ret = KErrNone;
		}
	}
	
	return ret;
	}


TVerdict CCommDbTest022_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.04 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}




/////////////////////
//	Test step 022.05
/////////////////////

// constructor
CCommDbTest022_05::CCommDbTest022_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_05"));
	}

// destructor
CCommDbTest022_05::~CCommDbTest022_05()
	{
	}


TVerdict CCommDbTest022_05::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_05::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Open a view on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerWcdma, ECommDbConnectionDirectionIncoming);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #9")) )
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		if ( !value.Compare(_L("Record #10")) )
		{
			//There should be no more records
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
			ret = KErrNone;
		}
	}
	
	return ret;
	}


TVerdict CCommDbTest022_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.05 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 022.06
/////////////////////

// constructor
CCommDbTest022_06::CCommDbTest022_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_06"));
	}

// destructor
CCommDbTest022_06::~CCommDbTest022_06()
	{
	}


TVerdict CCommDbTest022_06::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_06::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Open a view on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerWcdma, ECommDbConnectionDirectionIncoming);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #9")) )
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		if ( !value.Compare(_L("Record #10")) )
		{
			//There should be no more records
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
			ret = KErrNone;
		}
	}
	
	return ret;
	}


TVerdict CCommDbTest022_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.06 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 022.07
/////////////////////

// constructor
CCommDbTest022_07::CCommDbTest022_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_07"));
	}

// destructor
CCommDbTest022_07::~CCommDbTest022_07()
	{
	}


TVerdict CCommDbTest022_07::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_07::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Open a view on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerCSD, ECommDbConnectionDirectionIncoming);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #3")) )
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		if ( !value.Compare(_L("Record #4")) )
			{
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
			iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
			if ( !value.Compare(_L("Record #6")) )
				{
				//There should be no more records
				CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
				ret = KErrNone;
				}
			}
	}
	
	return ret;
	}


TVerdict CCommDbTest022_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.07 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 022.08
/////////////////////

// constructor
CCommDbTest022_08::CCommDbTest022_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_08"));
	}

// destructor
CCommDbTest022_08::~CCommDbTest022_08()
	{
	}


TVerdict CCommDbTest022_08::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_08::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Open a view on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerLAN, ECommDbConnectionDirectionIncoming);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #11")) )
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		if ( !value.Compare(_L("Record #12")) )
		{
			//There should be no more records
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
			ret = KErrNone;
		}
	}
	
	return ret;
	}


TVerdict CCommDbTest022_08::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.08 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 022.09
/////////////////////

// constructor
CCommDbTest022_09::CCommDbTest022_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_09"));
	}

// destructor
CCommDbTest022_09::~CCommDbTest022_09()
	{
	}


TVerdict CCommDbTest022_09::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_09::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Open a view on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerLAN, ECommDbConnectionDirectionOutgoing);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( !value.Compare(_L("Record #11")) )
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		if ( !value.Compare(_L("Record #12")) )
		{
			//There should be no more records
			CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
			ret = KErrNone;
		}
	}
	
	return ret;
	}


TVerdict CCommDbTest022_09::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.09 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 022.10
/////////////////////

// constructor
CCommDbTest022_10::CCommDbTest022_10()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_10"));
	}

// destructor
CCommDbTest022_10::~CCommDbTest022_10()
	{
	}


TVerdict CCommDbTest022_10::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest022_10::executeStepL()
	{
	TInt ret=KErrNone;

	//Open a view with multiple bearers on the IAP table, then check the records returned
	iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerCSD | KCommDbBearerLAN, ECommDbConnectionDirectionOutgoing);
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #1")) && ret == KErrNone)
		{
		ret = KErrGeneral;
		}
	else
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		}

	if ( value.Compare(_L("Record #2"))  && ret == KErrNone )
		{
		ret = KErrGeneral;
		}
	else
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		}

	if ( value.Compare(_L("Record #11"))  && ret == KErrNone )
		{
		ret = KErrGeneral;
		}
	else
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value);
		}
		
	if ( value.Compare(_L("Record #12"))  && ret == KErrNone )
		{
		ret = KErrGeneral;
		}
	else
		{
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
		}
		

	return ret;
	}


TVerdict CCommDbTest022_10::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.10 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 022.11
/////////////////////

// constructor
CCommDbTest022_11::CCommDbTest022_11()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_11"));
	}

// destructor
CCommDbTest022_11::~CCommDbTest022_11()
	{
	}


TVerdict CCommDbTest022_11::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define INVALID_BEARER 0x200

TInt CCommDbTest022_11::executeStepL()
	{
	TInt ret=KErrGeneral;


	TRAPD( r, iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( INVALID_BEARER , ECommDbConnectionDirectionOutgoing) );
	if ( r!=KErrNone )
		{
		ret = r;
		}
	else
		{
		//Shouldnt get here, but just in case ....
		CleanupStack::Pop();
		}

	return ret;
	}


TVerdict CCommDbTest022_11::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.11 called "));


	if ( executeStepL() == KErrArgument )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}




/////////////////////
//	Test step 022.12
/////////////////////

// constructor
CCommDbTest022_12::CCommDbTest022_12()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_12"));
	}

// destructor
CCommDbTest022_12::~CCommDbTest022_12()
	{
	}


TVerdict CCommDbTest022_12::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define INVALID_DIRECTION 0xF

TInt CCommDbTest022_12::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Due to the strong type checking in C++, this test step is not particularly valid
	TRAPD( r, iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerCSD , (TCommDbConnectionDirection)INVALID_DIRECTION ) );
	if ( r!=KErrNone )
		{
		ret = r;
		}
	else
		{
		//Shouldnt get here, but just in case ....
		CleanupStack::Pop();
		}

	return ret;
	}


TVerdict CCommDbTest022_12::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.12 called "));


	if ( executeStepL() == KErrArgument )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 022.13
/////////////////////

// constructor
CCommDbTest022_13::CCommDbTest022_13()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_13"));
	}

// destructor
CCommDbTest022_13::~CCommDbTest022_13()
	{
	}


TVerdict CCommDbTest022_13::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest022_13::executeStepL()
	{
	TInt ret=KErrGeneral;
	
	//Due to the strong type checking in C++, this test step is not particularly valid
	TRAPD( r, iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerCSD , (TCommDbConnectionDirection)5 ) );
	if ( r!=KErrNone )
		{
		ret = r;
		}
	else
		{
		//Shouldnt get here, but just in case ....
		CleanupStack::Pop();
		}

	return ret;
	}


TVerdict CCommDbTest022_13::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.13 called "));


	if ( executeStepL() == KErrArgument )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 022.14
/////////////////////

// constructor
CCommDbTest022_14::CCommDbTest022_14()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_14"));
	}

// destructor
CCommDbTest022_14::~CCommDbTest022_14()
	{
	}


TVerdict CCommDbTest022_14::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define INVALID_DIRECTION2 0xffffffff

TInt CCommDbTest022_14::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Due to the strong type checking in C++, this test step is not particularly valid
	TRAPD( r, iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( KCommDbBearerCSD , (TCommDbConnectionDirection)INVALID_DIRECTION2 ) );
	if ( r!=KErrNone )
		{
		ret = r;
		}
	else
		{
		//Shouldnt get here, but just in case ....
		CleanupStack::Pop();
		}

	return ret;
	}


TVerdict CCommDbTest022_14::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.14 called "));


	if ( executeStepL() == KErrArgument )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 022.15
/////////////////////

// constructor
CCommDbTest022_15::CCommDbTest022_15()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_15"));
	}

// destructor
CCommDbTest022_15::~CCommDbTest022_15()
	{
	}


TVerdict CCommDbTest022_15::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define UNSUPPORTED_BEARER 0x30

TInt CCommDbTest022_15::executeStepL()
	{
	TInt ret=KErrGeneral;
	// Similar to test 022_11, but with direction incoming...

	TRAPD( r, iTheView = iTheDb->OpenIAPTableViewMatchingBearerSetLC( UNSUPPORTED_BEARER , ECommDbConnectionDirectionIncoming) );
	if ( r!=KErrNone )
		{
		ret = r;
		}
	else
		{
		//Shouldnt get here, but just in case ....
		CleanupStack::Pop();
		}

	return ret;
	}


TVerdict CCommDbTest022_15::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.15 called "));


	if ( executeStepL() == KErrArgument )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 022.16
/////////////////////

// constructor
CCommDbTest022_16::CCommDbTest022_16()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_16"));
	}

// destructor
CCommDbTest022_16::~CCommDbTest022_16()
	{
	}

TVerdict CCommDbTest022_16::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.16 called "));

	SetTestStepResult(EPass);
		// Heap test for 022.01

	CCommDbTest022_01* step022_01 = new(ELeave) CCommDbTest022_01;
	CleanupStack::PushL(step022_01);
	if ( doTestStepWithHeapFailureL( *step022_01, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_01);



	return TestStepResult();	

}


/////////////////////
//	Test step 022.17
/////////////////////

// constructor
CCommDbTest022_17::CCommDbTest022_17()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_17"));
	}

// destructor
CCommDbTest022_17::~CCommDbTest022_17()
	{
	}

TVerdict CCommDbTest022_17::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.17 called "));

	SetTestStepResult(EPass);	
			// Heap test for 022.02

	CCommDbTest022_02* step022_02 = new CCommDbTest022_02;
	CleanupStack::PushL(step022_02);
	if ( doTestStepWithHeapFailureL( *step022_02, KErrNone) == EFail ) 
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_02);
	
	return TestStepResult();
	
	}

/////////////////////
//	Test step 022.18
/////////////////////

// constructor
CCommDbTest022_18::CCommDbTest022_18()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_18"));
	}

// destructor
CCommDbTest022_18::~CCommDbTest022_18()
	{
	}

TVerdict CCommDbTest022_18::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.18 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.03

	CCommDbTest022_03* step022_03 = new CCommDbTest022_03;
	CleanupStack::PushL(step022_03);
	if ( doTestStepWithHeapFailureL( *step022_03, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_03);
	
	return TestStepResult();
	
	}

/////////////////////
//	Test step 022.19
/////////////////////

// constructor
CCommDbTest022_19::CCommDbTest022_19()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_19"));
	}

// destructor
CCommDbTest022_19::~CCommDbTest022_19()
	{
	}

TVerdict CCommDbTest022_19::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.19 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.04

	CCommDbTest022_04* step022_04 = new CCommDbTest022_04;
	CleanupStack::PushL(step022_04);
	if ( doTestStepWithHeapFailureL( *step022_04, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_04);
	
	return TestStepResult();
	
	}
	
/////////////////////
//	Test step 022.20
/////////////////////

// constructor
CCommDbTest022_20::CCommDbTest022_20()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_20"));
	}

// destructor
CCommDbTest022_20::~CCommDbTest022_20()
	{
	}

TVerdict CCommDbTest022_20::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.20 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.05

	CCommDbTest022_05* step022_05 = new CCommDbTest022_05;
	CleanupStack::PushL(step022_05);
	if ( doTestStepWithHeapFailureL( *step022_05, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_05);
	
	return TestStepResult();
	
	}
	
/////////////////////
//	Test step 022.21
/////////////////////

// constructor
CCommDbTest022_21::CCommDbTest022_21()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_21"));
	}

// destructor
CCommDbTest022_21::~CCommDbTest022_21()
	{
	}

TVerdict CCommDbTest022_21::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.21 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.06

	CCommDbTest022_06* step022_06 = new CCommDbTest022_06;
	CleanupStack::PushL(step022_06);
	if ( doTestStepWithHeapFailureL( *step022_06, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_06);
	
	return TestStepResult();
	
	}
	
/////////////////////
//	Test step 022.22
/////////////////////

// constructor
CCommDbTest022_22::CCommDbTest022_22()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_22"));
	}

// destructor
CCommDbTest022_22::~CCommDbTest022_22()
	{
	}

TVerdict CCommDbTest022_22::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.22 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.07

	CCommDbTest022_07* step022_07 = new CCommDbTest022_07;
	CleanupStack::PushL(step022_07);
	if ( doTestStepWithHeapFailureL( *step022_07, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_07);
	
	return TestStepResult();
	
	}
	
/////////////////////
//	Test step 022.23
/////////////////////

// constructor
CCommDbTest022_23::CCommDbTest022_23()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_23"));
	}

// destructor
CCommDbTest022_23::~CCommDbTest022_23()
	{
	}

TVerdict CCommDbTest022_23::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.23 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.08

	CCommDbTest022_08* step022_08 = new CCommDbTest022_08;
	CleanupStack::PushL(step022_08);
	if ( doTestStepWithHeapFailureL( *step022_08, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_08);
	
	return TestStepResult();
	
	}	
	
/////////////////////
//	Test step 022.24
/////////////////////

// constructor
CCommDbTest022_24::CCommDbTest022_24()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_24"));
	}

// destructor
CCommDbTest022_24::~CCommDbTest022_24()
	{
	}

TVerdict CCommDbTest022_24::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.24 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.09

	CCommDbTest022_09* step022_09 = new CCommDbTest022_09;
	CleanupStack::PushL(step022_09);
	if ( doTestStepWithHeapFailureL( *step022_09, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_09);
	
	return TestStepResult();
	
	}		
	
/////////////////////
//	Test step 022.25
/////////////////////

// constructor
CCommDbTest022_25::CCommDbTest022_25()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_25"));
	}

// destructor
CCommDbTest022_25::~CCommDbTest022_25()
	{
	}

TVerdict CCommDbTest022_25::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.25 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.10

	CCommDbTest022_10* step022_10 = new CCommDbTest022_10;
	CleanupStack::PushL(step022_10);
	if ( doTestStepWithHeapFailureL( *step022_10, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_10);
	
	return TestStepResult();
	
	}		
	
/////////////////////
//	Test step 022.26
/////////////////////

// constructor
CCommDbTest022_26::CCommDbTest022_26()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_26"));
	}

// destructor
CCommDbTest022_26::~CCommDbTest022_26()
	{
	}

TVerdict CCommDbTest022_26::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.26 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.11

	CCommDbTest022_11* step022_11 = new CCommDbTest022_11;
	CleanupStack::PushL(step022_11);
	if ( doTestStepWithHeapFailureL( *step022_11, KErrArgument) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_11);
	
	return TestStepResult();
	
	}
	
/////////////////////
//	Test step 022.27
/////////////////////

// constructor
CCommDbTest022_27::CCommDbTest022_27()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_27"));
	}

// destructor
CCommDbTest022_27::~CCommDbTest022_27()
	{
	}

TVerdict CCommDbTest022_27::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.27 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.12

	CCommDbTest022_12* step022_12 = new CCommDbTest022_12;
	CleanupStack::PushL(step022_12);
	if ( doTestStepWithHeapFailureL( *step022_12, KErrArgument) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_12);
	
	return TestStepResult();
	
	}	
	
/////////////////////
//	Test step 022.28
/////////////////////

// constructor
CCommDbTest022_28::CCommDbTest022_28()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_28"));
	}

// destructor
CCommDbTest022_28::~CCommDbTest022_28()
	{
	}

TVerdict CCommDbTest022_28::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.28 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.13

	CCommDbTest022_13* step022_13 = new CCommDbTest022_13;
	CleanupStack::PushL(step022_13);
	if ( doTestStepWithHeapFailureL( *step022_13, KErrArgument) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_13);
	
	return TestStepResult();
	
	}		
	
/////////////////////
//	Test step 022.29
/////////////////////

// constructor
CCommDbTest022_29::CCommDbTest022_29()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_29"));
	}

// destructor
CCommDbTest022_29::~CCommDbTest022_29()
	{
	}

TVerdict CCommDbTest022_29::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.29 called "));

	SetTestStepResult(EPass);	
		// Heap test for 022.14

	CCommDbTest022_14* step022_14 = new CCommDbTest022_14;
	CleanupStack::PushL(step022_14);
	if ( doTestStepWithHeapFailureL( *step022_14, KErrArgument) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step022_14);
	
	return TestStepResult();
	
	}			


/////////////////////
//	Test step 022.30
/////////////////////

// constructor
CCommDbTest022_30::CCommDbTest022_30()
	{
	// store the name of this test case
	SetTestStepName(_L("step_022_30"));
	}

// destructor
CCommDbTest022_30::~CCommDbTest022_30()
	{
	}

TInt CCommDbTest022_30::executeStepL()
	{
	TInt ret = KErrGeneral;

	CCommsDatabase* db = CCommsDatabase::NewL();
	CleanupStack::PushL(db);

	CCommsDbTableView* view1 = db->OpenTableLC( TPtrC(IAP) );
	TUint32 a = 0;
	User::LeaveIfError(view1->InsertRecord(a));
	// expected commdb currently only has 1 IAP, hence 2 here
	if (a != 2)
		{
		User::Leave(KErrGeneral);
		}
	CleanupStack::Pop(view1);

	CCommsDbTableView* view2 = db->OpenIAPTableViewMatchingBearerSetLC(KCommDbBearerWcdma, ECommDbConnectionDirectionOutgoing);
	a = 0;
	User::LeaveIfError(view2->InsertRecord(a));
	if (a != 3)
		{
		User::Leave(KErrGeneral);
		}
	CleanupStack::Pop(view2);

	CleanupStack::Pop(db);

	ret = KErrNone;
	return ret;
	}


TVerdict CCommDbTest022_30::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 022.30 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

//EOF
