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
// This contains CommDb Unit Test Cases 007.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>


// Test system includes
#include "Teststepcommdb.h"
#include "Step_007_xx.h"

//#ifndef SYMBIAN_COMMS_REPOSITORY
//	_LIT(KTempDb3, "c:\\system\\data\\default3.dat");
//#endif

/////////////////////
//	Test step 007.001
/////////////////////

// constructor
CCommDbTest007_01::CCommDbTest007_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_01"));
	}

// destructor
CCommDbTest007_01::~CCommDbTest007_01()
	{
	}


TVerdict CCommDbTest007_01::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

#define EXPECTED_FIELD_VALUE _L8("Call me back")

TInt CCommDbTest007_01::executeStepL()
	{
	TInt ret = EFail;


	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone );
	//Read the value from the field and compare it to the expected value
	TBuf8<KCommsDbSvrMaxFieldLength> value;

	iTheView->ReadTextL( TPtrC(ISP_IF_CALLBACK_INFO), value );

	if( !value.Compare( TPtrC8(EXPECTED_FIELD_VALUE) ) )
		{
		ret = EPass;
		}

	return ret;
	}

TVerdict CCommDbTest007_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.01 called "));

	SetTestStepResult(EFail);

	if ( executeStepL() == EPass )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 007.002
/////////////////////

// constructor
CCommDbTest007_02::CCommDbTest007_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_02"));
	}

// destructor
CCommDbTest007_02::~CCommDbTest007_02()
	{
	}


TVerdict CCommDbTest007_02::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

#define EXPECTED_ISP_DESCRIPTION _S("Description")

TInt CCommDbTest007_02::executeStepL()
	{
	TInt32 ret = EFail;


	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone );
	//Read the value from the field and compare it to the expected value
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(ISP_DESCRIPTION), value );

	if( !value.Compare( TPtrC(EXPECTED_ISP_DESCRIPTION) ) )
		{
		ret = EPass;
		}

	return ret;
	}

TVerdict CCommDbTest007_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.02 called "));
	SetTestStepResult(EFail);
	if ( executeStepL() == EPass  )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 007.003
/////////////////////

// constructor
CCommDbTest007_03::CCommDbTest007_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_03"));
	}

// destructor
CCommDbTest007_03::~CCommDbTest007_03()
	{
	}

TVerdict CCommDbTest007_03::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

#define EXPECTED_ISP_LOGIN_SCRIPT _S("wobble")

TInt CCommDbTest007_03::executeStepL()
	{
	TInt32 ret = EFail;

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone );

	//Read the value from the LongText field and compare it to the expected value
	HBufC* value = iTheView->ReadLongTextLC( TPtrC(ISP_LOGIN_SCRIPT) );

	if ( value )
		{
		if( !value->Compare( TPtrC(EXPECTED_ISP_LOGIN_SCRIPT) ) )
			{
			ret = EPass;
			}
		CleanupStack::PopAndDestroy();
		}


	return ret;
	}

TVerdict CCommDbTest007_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.03 called "));

	if ( executeStepL() == EPass  )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 007.004
/////////////////////

// constructor
CCommDbTest007_04::CCommDbTest007_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_04"));
	}

// destructor
CCommDbTest007_04::~CCommDbTest007_04()
	{
	}

TVerdict CCommDbTest007_04::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

#define EXPECTED_ISP_TYPE  2

TInt CCommDbTest007_04::executeStepL()
	{
	TBool ret = EFail;


	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone);
	//Read the value from the Uint field and compare it to the expected value
	TUint32 value;
	iTheView->ReadUintL( TPtrC(ISP_TYPE), value );

	if( value == EXPECTED_ISP_TYPE )
		{
		ret = EPass;
		}

	return ret;
}

TVerdict CCommDbTest007_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.04 called "));


	if ( executeStepL() == EPass  )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}


/////////////////////
//	Test step 007.005
/////////////////////

// constructor
CCommDbTest007_05::CCommDbTest007_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_05"));
	// default to no local objects allocated
	iLocalObjects = EFalse;
	}

// destructor
CCommDbTest007_05::~CCommDbTest007_05()
	{
	}

TVerdict CCommDbTest007_05::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest007_05::executeStepL()
	{
	TBool ret = EFail;

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone);
	//Read the value from the Uint field and compare it to the expected value
	TBool expectedValue;
	iTheView->ReadBoolL( TPtrC(ISP_DISPLAY_PCT), expectedValue );

	if( expectedValue )
		{
		SetTestStepResult(EPass);		}

	return ret;
	}

TVerdict CCommDbTest007_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.05 called "));

	if ( executeStepL() == EPass  )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 007.006
/////////////////////

// constructor
CCommDbTest007_06::CCommDbTest007_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_06"));
	}

// destructor
CCommDbTest007_06::~CCommDbTest007_06()
	{
	}

TVerdict CCommDbTest007_06::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

#define EXPECTED_COLUMN_LENGTH	10		//Expected length of ISP_IP_ADDR column

TInt CCommDbTest007_06::executeStepL()
	{
	TBool ret = EFail;

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone );

	//Read the length of a column
	TInt value;
	iTheView->ReadColumnLengthL( TPtrC(ISP_IP_ADDR), value );

	if( value == EXPECTED_COLUMN_LENGTH )
		SetTestStepResult(EPass);
	return ret;
	}

TVerdict CCommDbTest007_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.06 called "));

	if ( executeStepL() == EPass  )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 007.007
/////////////////////

// constructor
CCommDbTest007_07::CCommDbTest007_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_07"));
	}

// destructor
CCommDbTest007_07::~CCommDbTest007_07()
	{
	}



TVerdict CCommDbTest007_07::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest007_07::executeStepL()
	{
	TInt ret = KErrGeneral;

	ret = iTheView->GotoFirstRecord();
	if ( ret == KErrNotFound )
		return ret;

	//Try to read a column value from the empty view
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(ISP_DESCRIPTION), value );
	return ret;
	}

TVerdict CCommDbTest007_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.07 called "));

	//NB. create a view on an empty table
	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}

/////////////////////
//	Test step 007.08
/////////////////////

// constructor
CCommDbTest007_08::CCommDbTest007_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_08"));
	// default to no local objects allocated
	iLocalObjects = EFalse;
	}

// destructor
CCommDbTest007_08::~CCommDbTest007_08()
	{
	}



TVerdict CCommDbTest007_08::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest007_08::executeStepL()
	{
	TBool ret = EFail;

	ret = iTheView->GotoFirstRecord();
	if ( ret == KErrNotFound )
		{
         	SetTestStepResult(EPass);	
		return TestStepResult();
		}

	//Read the length of a column
	TInt value;
	iTheView->ReadColumnLengthL( TPtrC(ISP_IF_PARAMS), value );
	if ( value == 0 )
		ret = EPass;

	return ret;
	}

TVerdict CCommDbTest007_08::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.08 called "));

	//NB. create a view on an empty table

	if ( executeStepL() == EPass )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 007.09
/////////////////////

// constructor
CCommDbTest007_09::CCommDbTest007_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_09"));
	}

// destructor
CCommDbTest007_09::~CCommDbTest007_09()
	{
	}


TVerdict CCommDbTest007_09::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest007_09::executeStepL()
	{
	TInt ret = KErrGeneral;

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone )

	//Try to read from an invalid column
	TBuf<KCommsDbSvrMaxFieldLength> value;
	TRAPD(r, iTheView->ReadTextL( KInvalidColumnName, value ) );

	ret = r;

	return ret;
	}

TVerdict CCommDbTest007_09::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.09 called "));

	//NB. create a view on an empty table

	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}


/////////////////////
//	Test step 007.10
/////////////////////

// constructor
CCommDbTest007_10::CCommDbTest007_10()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_10"));
	}

// destructor
CCommDbTest007_10::~CCommDbTest007_10()
	{
	}


TVerdict CCommDbTest007_10::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest007_10::executeStepL()
	{
	TInt ret = KErrGeneral;

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone );

	//Try to read length of an invalid column
	TInt value;
	TRAPD(r,iTheView->ReadColumnLengthL( KInvalidColumnName, value ));
	ret = r;

	return ret;
	}

TVerdict CCommDbTest007_10::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.10 called "));

	//NB. create a view on an empty table

	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 007.11
/////////////////////

// constructor
CCommDbTest007_11::CCommDbTest007_11()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_11"));
	}

// destructor
CCommDbTest007_11::~CCommDbTest007_11()
	{
	}


TVerdict CCommDbTest007_11::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest007_11::executeStepL()
	{
	TInt ret = KErrGeneral;

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone);

	//Read a column that will have a NULL value
	TBuf<KCommsDbSvrMaxFieldLength> value;
	TRAPD( r, iTheView->ReadTextL( TPtrC( ISP_DESCRIPTION ), value ) );
	if ( r==KErrNone && value.Size() == 0 )
		{
			//test passed
			ret = KErrNone;
		}
	else if ( r != KErrNone )
		{
		//return the leave reason
		ret = r;
		}


	return ret;
	}

TVerdict CCommDbTest007_11::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.11 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}



/////////////////////
//	Test step 007.12
/////////////////////

// constructor
CCommDbTest007_12::CCommDbTest007_12()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_12"));
	}

// destructor
CCommDbTest007_12::~CCommDbTest007_12()
	{
	}


TVerdict CCommDbTest007_12::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest007_12::executeStepL()
	{
	TInt ret = KErrGeneral;

	CDBLEAVE( ret =  iTheView->GotoFirstRecord(), KErrNone )

	//Try to read length of column containing a NULL value
	TInt value = -1;
	TRAPD(r, iTheView->ReadColumnLengthL( TPtrC(ISP_DESCRIPTION), value ) );
	if ( r==KErrNone && value == 0 )
		{
			//test passed
			ret = KErrNone;
		}
	else if ( r != KErrNone )
		{
		//return the leave reason
		ret = r;
		}

	return ret;
	}

TVerdict CCommDbTest007_12::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.12 called "));

	//NB. create a view on an empty table
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}




/////////////////////
//	Test step 007.13
/////////////////////
/*
//Removed since this test will not compile on Code warrior due to having an array of zero length
// constructor
CCommDbTest007_13::CCommDbTest007_13()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_13"));
	}

// destructor
CCommDbTest007_13::~CCommDbTest007_13()
	{
	}


TVerdict CCommDbTest007_13::doTestStepPreambleL()
	{
	copyTestDatabase( EDatabase_1 );
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest007_13::executeStepL()
	{
	TInt ret = KErrGeneral;


	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//Read a column and pass a zero length buffer

	//This next line causes a warning
	TBuf<0> value;


	TRAPD( r, iTheView->ReadTextL( TPtrC( ISP_DESCRIPTION ), value ) );
	if ( r==KErrNone  )
		{
			//test passed
			ret = KErrNone;
		}
	else if ( r != KErrNone )
		{
		//return the leave reason
		ret = r;
		}

	return ret;
	}

TVerdict CCommDbTest007_13::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.13 called."));

	//NB. create a view on an empty table

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}


*/


/////////////////////
//	Test step 007.14
/////////////////////

// constructor
CCommDbTest007_14::CCommDbTest007_14()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_14"));
	}

// destructor
CCommDbTest007_14::~CCommDbTest007_14()
	{
	}


TVerdict CCommDbTest007_14::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest007_14::executeStepL()
	{
	TInt ret = KErrGeneral;

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//Read a column and pass a small length buffer

	TBuf<5> value;

	TRAPD( r, iTheView->ReadTextL( TPtrC( ISP_DESCRIPTION ), value ) );
	if ( r==KErrNone  )
		{
			//test passed
			ret = KErrNone;
		}
	else if ( r != KErrNone )
		{
		//return the leave reason
		ret = r;
		}

	return ret;
	}

TVerdict CCommDbTest007_14::doTestStepL( )
	{
	//NB. create a view on an empty table

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}



/////////////////////
//	Test step 007.15
/////////////////////

// constructor
CCommDbTest007_15::CCommDbTest007_15()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_15"));
	}

// destructor
CCommDbTest007_15::~CCommDbTest007_15()
	{
	}


TVerdict CCommDbTest007_15::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define ISP_DESCRIPTION_VALUE	_S("Isp #1")

TInt CCommDbTest007_15::executeStepL()
	{
	TInt ret = KErrGeneral;

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//Read a read only column
	TBuf<KCommsDbSvrMaxFieldLength> value;
	TRAPD( r, iTheView->ReadTextL( TPtrC( ISP_DESCRIPTION ), value ) );
	if ( r==KErrNone && value.Compare(TPtrC(ISP_DESCRIPTION_VALUE) ) == 0 )
		{
			//test passed
			ret = KErrNone;
		}
	else if ( r != KErrNone )
		{
		//return the leave reason
		ret = r;
		}

	return ret;
	}

TVerdict CCommDbTest007_15::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.15 called"));

	//NB. create a view on an empty table

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}


/////////////////////
//	Test step 007.16
// Check different paths in ReadTextL
/////////////////////

// constructor
CCommDbTest007_16::CCommDbTest007_16()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_16"));
	}

// destructor
CCommDbTest007_16::~CCommDbTest007_16()
	{
	}


TVerdict CCommDbTest007_16::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(OUTGOING_GPRS));
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define SHORT_APN_VALUE	_S("Test")

// this is KCommsDbSvrRealMaxFieldLength chars long (=255)
#define LONG_APN_VALUE	_S("TheQuickBrownFoxJumpedOverTheLazyDogTheQuickBrownFoxJumpedOverTheLazyDogTheQuickBrownFoxJumpedOverTheLazyDogTheQuickBrownFoxJumpedOverTheLazyDogTheQuickBrownFoxJumpedOverTheLazyDogTheQuickBrownFoxJumpedOverTheLazyDogTheQuickBrownFoxJumpedOverTheLazyDog123")

TInt CCommDbTest007_16::executeStepL()
	{
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	// Read a short APN value
	TBuf<KCommsDbSvrRealMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC( GPRS_APN ), value );

	if (value.Compare(TPtrC(SHORT_APN_VALUE) ) != 0 )
		{
		User::Leave(KErrGeneral);
		}

	// check that the long desc test is valid - ie KCommsDbSvrRealMaxFieldLength
	// hasn't changed since we wrote this
	TPtrC longDesc(LONG_APN_VALUE);
	if (longDesc.Length() != KCommsDbSvrRealMaxFieldLength)
		{
		User::Leave(KErrGeneral);
		}

	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	iTheView->ReadTextL( TPtrC( GPRS_APN ), value );

	if (value.Compare(TPtrC(LONG_APN_VALUE) ) != 0 )
		{
		User::Leave(KErrGeneral);
		}

	return KErrNone;
	}

TVerdict CCommDbTest007_16::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.16 called"));

	//NB. create a view on an empty table

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}


/////////////////////
//	Test step 007.17
// Check different paths in ReadTextL
/////////////////////

// constructor
CCommDbTest007_17::CCommDbTest007_17()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_17"));
	}

// destructor
CCommDbTest007_17::~CCommDbTest007_17()
	{
	}

TVerdict CCommDbTest007_17::doTestStepL()
	{
	INFO_PRINTF1(_L("Step 007.17 called"));

	SetTestStepResult(EPass);
	//Heap test for 007.01 - uses default1.xml 
	CCommDbTest007_01* step007_01 = new CCommDbTest007_01;
	CleanupStack::PushL(step007_01);
	doTestStepWithHeapFailureL( *step007_01, KErrNone );
	if ( step007_01->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_01);

	//Heap test for 007.02 - uses default1.xml 
	CCommDbTest007_02* step007_02 = new CCommDbTest007_02;
	CleanupStack::PushL(step007_02);
	doTestStepWithHeapFailureL( *step007_02, KErrNone );
	if ( step007_02->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_02);

	//Heap test for 007.03 - uses default1.xml 
	CCommDbTest007_03* step007_03 = new CCommDbTest007_03;
	CleanupStack::PushL(step007_03);
	doTestStepWithHeapFailureL( *step007_03, KErrNone );
	if ( step007_03->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_03);

	//Heap test for 007.04 - uses default1.xml 
	CCommDbTest007_04* step007_04 = new CCommDbTest007_04;
	CleanupStack::PushL(step007_04);
	doTestStepWithHeapFailureL( *step007_04, KErrNone );
	if ( step007_04->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_04);

	//Heap test for 007.05 - uses default1.xml 
	CCommDbTest007_05* step007_05 = new CCommDbTest007_05;
	CleanupStack::PushL(step007_05);
	doTestStepWithHeapFailureL( *step007_05, KErrNone );
	if ( step007_05->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_05);

	//Heap test for 007.06 - uses default1.xml 
	CCommDbTest007_06* step007_06 = new CCommDbTest007_06;
	CleanupStack::PushL(step007_06);
	doTestStepWithHeapFailureL( *step007_06, KErrNone );
	if ( step007_06->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_06);

	//Heap test for 007.07 - uses default7.xml 
	//Heap test for 007.08 - uses default7.xml 

	//Heap test for 007.09 - uses default1.xml 
	CCommDbTest007_09* step007_09 = new CCommDbTest007_09;
	CleanupStack::PushL(step007_09);
	doTestStepWithHeapFailureL( *step007_09, KErrNotFound );
	if ( step007_09->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_09);

	//Heap test for 007.10 - uses default1.xml 
	CCommDbTest007_10* step007_10 = new CCommDbTest007_10;
	CleanupStack::PushL(step007_10);
	doTestStepWithHeapFailureL( *step007_10, KErrNotFound );
	if ( step007_10->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_10);

	//Heap test for 007.11 - uses default1.xml 
	CCommDbTest007_11* step007_11 = new CCommDbTest007_11;
	CleanupStack::PushL(step007_11);
	doTestStepWithHeapFailureL( *step007_11, KErrNone );
	if ( step007_11->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_11);

	//Heap test for 007.12 - uses default1.xml 
	CCommDbTest007_12* step007_12 = new CCommDbTest007_12;
	CleanupStack::PushL(step007_12);
	doTestStepWithHeapFailureL( *step007_12, KErrNone );
	if ( step007_12->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_12);

	//Heap test for 007.14 - uses default1.xml - panic test see 7.20

	//Heap test for 007.15 - uses default1.xml 
	CCommDbTest007_15* step007_15 = new CCommDbTest007_15;
	CleanupStack::PushL(step007_15);
	doTestStepWithHeapFailureL( *step007_15, KErrNone );
	if ( step007_15->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_15);
	
	return TestStepResult();
	}

/////////////////////
//	Test step 007.18
// Check different paths in ReadTextL
/////////////////////

// constructor
CCommDbTest007_18::CCommDbTest007_18()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_18"));
	}

// destructor
CCommDbTest007_18::~CCommDbTest007_18()
	{
	}

TVerdict CCommDbTest007_18::doTestStepL()
	{
	INFO_PRINTF1(_L("Step 007.18 called"));

	SetTestStepResult(EPass);
	//Heap test for 007.07 - uses default7.xml 
	copyTestDatabase( EDatabase_7 );
	CCommDbTest007_07* step007_07 = new CCommDbTest007_07;
	CleanupStack::PushL(step007_07);
	doTestStepWithHeapFailureL( *step007_07, KErrNotFound );
	if ( step007_07->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_07);


	//Heap test for 007.08 - uses default7.xml
	CCommDbTest007_08* step007_08 = new CCommDbTest007_08;
	CleanupStack::PushL(step007_08);
	doTestStepWithHeapFailureL( *step007_08, KErrNone );
	if ( step007_08->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_08);

	return TestStepResult();
	}
	
/////////////////////
//	Test step 007.19
// Check different paths in ReadTextL
/////////////////////

// constructor
CCommDbTest007_19::CCommDbTest007_19()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_19"));
	}

// destructor
CCommDbTest007_19::~CCommDbTest007_19()
	{
	}

TVerdict CCommDbTest007_19::doTestStepL()
	{
	INFO_PRINTF1(_L("Step 007.19 called"));

	SetTestStepResult(EPass);
	//Heap test for 007.16 - uses default3.xml
	CCommDbTest007_16* step007_16 = new CCommDbTest007_16;
	CleanupStack::PushL(step007_16);
	doTestStepWithHeapFailureL( *step007_16, KErrNone );
	if ( step007_16->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_16);

	return TestStepResult();
	}
	
/////////////////////
//	Test step 007.20
// Check different paths in ReadTextL
/////////////////////

// constructor
CCommDbTest007_20::CCommDbTest007_20()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_20"));
	}

// destructor
CCommDbTest007_20::~CCommDbTest007_20()
	{
	}

TVerdict CCommDbTest007_20::doTestStepL()
	{

	SetTestStepResult(EPass);
	//Heap test for 007.14 - uses default1.xml
	CCommDbTest007_14* step007_14 = new CCommDbTest007_14;
	CleanupStack::PushL(step007_14);
	doTestStepWithHeapFailureL( *step007_14, KErrNone );
	if ( step007_14->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step007_14);

	return TestStepResult();
	}
