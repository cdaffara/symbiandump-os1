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
// This contains CommDb Unit Test Cases 009.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>


// Test system includes
#include "Teststepcommdb.h"
#include "Step_009_xx.h"



/////////////////////
//	Test step 009.001
/////////////////////

// constructor
CCommDbTest009_01::CCommDbTest009_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_01"));
	}

// destructor
CCommDbTest009_01::~CCommDbTest009_01()
	{
	}


TVerdict CCommDbTest009_01::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

#define ISP_INIT_STRING_TEST_VALUE	_L8("Test Value")
#define COMMDB_NAME_TEST_VALUE	_L("Test record")
//#define ISP_IF_NAME_TEST_VALUE	_L("Test record")


TInt CCommDbTest009_01::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//NB. The table MUST be empty for this test to work!

	//Try to insert a record
	
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	//Insert some text
	iTheView->WriteTextL( TPtrC(ISP_INIT_STRING), ISP_INIT_STRING_TEST_VALUE );

	//Must write to these columns as they cannot hold null values
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME), ISP_IF_NAME_TEST_VALUE );
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),COMMDB_NAME_TEST_VALUE);

	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );


	//Read the value from the field and compare it to the value we've just written
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	TBuf8<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(ISP_INIT_STRING), value );
 	if( !value.Compare( ISP_INIT_STRING_TEST_VALUE ) )
		{
		ret = KErrNone;
		}


	return ret;
	}


TVerdict CCommDbTest009_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 009.01 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
		
	return TestStepResult();
	}


/////////////////////
//	Test step 009.002
/////////////////////

// constructor
CCommDbTest009_02::CCommDbTest009_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_02"));
	}

// destructor
CCommDbTest009_02::~CCommDbTest009_02()
	{
	}


TVerdict CCommDbTest009_02::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define ISP_DESCRIPTION_TEST_VALUE	_L16("ISP description test value")

TInt CCommDbTest009_02::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//NB. The table MUST be empty for this test to work!

	//Try to insert a record
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	//Insert some text
	iTheView->WriteTextL( TPtrC(ISP_DESCRIPTION), ISP_DESCRIPTION_TEST_VALUE );

	//Must write to these columns as they cannot hold null values
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME), ISP_IF_NAME_TEST_VALUE );
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),COMMDB_NAME_TEST_VALUE);

	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone);
	
	//Read the value from the field and compare it to the value we've just written
	CDBLEAVE( iTheView->GotoFirstRecord(),  KErrNone ) ;

	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(ISP_DESCRIPTION), value );
	if( !value.Compare( ISP_DESCRIPTION_TEST_VALUE ) )
		{
		ret = KErrNone;
		}
	
	return ret;
	}


TVerdict CCommDbTest009_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 009.02 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);	
	return TestStepResult();
	}




/////////////////////
//	Test step 009.003
/////////////////////

// constructor
CCommDbTest009_03::CCommDbTest009_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_03"));
	}

// destructor
CCommDbTest009_03::~CCommDbTest009_03()
	{
	}


TVerdict CCommDbTest009_03::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

#define ISP_BEARER_TYPE_TEST_VALUE		0x1

TInt CCommDbTest009_03::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//NB. The table MUST be empty for this test to work!

	//Try to insert a record
	CDBLEAVE( iTheView->InsertRecord( id ),  KErrNone );

	//Insert a value
	iTheView->WriteUintL( TPtrC(ISP_BEARER_TYPE), ISP_BEARER_TYPE_TEST_VALUE );

	//Must write to these columns as they cannot hold null values
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME), ISP_IF_NAME_TEST_VALUE );
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),COMMDB_NAME_TEST_VALUE);

	CDBLEAVE( iTheView->PutRecordChanges(),  KErrNone );

	//Read the value from the field and compare it to the value we've just written
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	TUint32 value;
	iTheView->ReadUintL( TPtrC(ISP_BEARER_TYPE), value );
	if( value == ISP_BEARER_TYPE_TEST_VALUE )
		{
		ret = KErrNone;
		}
		

	return ret;
	}


TVerdict CCommDbTest009_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 009.03 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}







/////////////////////
//	Test step 009.004
/////////////////////

// constructor
CCommDbTest009_04::CCommDbTest009_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_04"));
	}

// destructor
CCommDbTest009_04::~CCommDbTest009_04()
	{
	}


TVerdict CCommDbTest009_04::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

#define ISP_ENABLE_SW_COMP_TEST_VALUE	ETrue

TInt CCommDbTest009_04::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//NB. The table MUST be empty for this test to work!

	//Try to insert a record
	CDBLEAVE( iTheView->InsertRecord( id ),  KErrNone );
		
	//Insert a value
	iTheView->WriteBoolL( TPtrC(ISP_ENABLE_SW_COMP), ISP_ENABLE_SW_COMP_TEST_VALUE );

	//Must write to these columns as they cannot hold null values
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME), ISP_IF_NAME_TEST_VALUE );
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),COMMDB_NAME_TEST_VALUE);

	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
	
	//Read the value from the field and compare it to the value we've just written
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	
	TBool value;
	iTheView->ReadBoolL( TPtrC(ISP_ENABLE_SW_COMP), value );
	if( value )
		{
		ret = KErrNone;
		}


	return ret;
	}


TVerdict CCommDbTest009_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 009.04 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}





/////////////////////
//	Test step 009.005
/////////////////////

// constructor
CCommDbTest009_05::CCommDbTest009_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_05"));
	}

// destructor
CCommDbTest009_05::~CCommDbTest009_05()
	{
	}

TVerdict CCommDbTest009_05::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define ISP_LOGIN_SCRIPT_TEST_VALUE	 _L("This is a test string that will be used in the test. It must be longer than 50 characters")

TInt CCommDbTest009_05::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//NB. The table MUST be empty for this test to work!

	//Try to insert a record
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	//Insert a value
	iTheView->WriteLongTextL( TPtrC(ISP_LOGIN_SCRIPT), TPtrC(ISP_LOGIN_SCRIPT_TEST_VALUE) );

	//Must write to these columns as they cannot hold null values
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME), ISP_IF_NAME_TEST_VALUE );
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),COMMDB_NAME_TEST_VALUE);

	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );


	//Read the value from the field and compare it to the value we've just written
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	
	HBufC* value = iTheView->ReadLongTextLC( TPtrC(ISP_LOGIN_SCRIPT) );

	if ( value )
		{
		if( !value->Compare( ISP_LOGIN_SCRIPT_TEST_VALUE) )
			{
			ret = KErrNone;
			}
		CleanupStack::PopAndDestroy();
		}

	return ret;
	}


TVerdict CCommDbTest009_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 009.05 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);		
	return TestStepResult();
	}





/////////////////////
//	Test step 009.06
/////////////////////

// constructor
CCommDbTest009_06::CCommDbTest009_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_06"));
	}

// destructor
CCommDbTest009_06::~CCommDbTest009_06()
	{
	}


TVerdict CCommDbTest009_06::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define TEST_DATA		_L("Test data")


TInt CCommDbTest009_06::executeStepL()
	{
	TInt ret=KErrNone;
	TUint32 id;

	//NB. The table MUST be empty for this test to work!

	//NB. This test will cause a PANIC

	//Try to insert a record
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	//Insert a value in an invalid column
	iTheView->WriteTextL( KInvalidColumnName, TPtrC(TEST_DATA) );

	// We should never get here because we should have hit a panic
	// If the Panic doesnt happen, we'll cause a leave

	User::Leave( KErrGeneral );

	return ret;
	}


TVerdict CCommDbTest009_06::doTestStepL( )
	{

	//This test step needs to trap a Panic

	TRAPD( r, executeStepL() );

	if ( r == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
		
	return TestStepResult();
	}



/////////////////////
//	Test step 009.07
/////////////////////

// constructor
CCommDbTest009_07::CCommDbTest009_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_07"));
	}

// destructor
CCommDbTest009_07::~CCommDbTest009_07()
	{
	}


TVerdict CCommDbTest009_07::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest009_07::executeStepL()
	{
	TInt ret=KErrNone;

	//NB. This test will cause a PANIC

	//Try to insert a record without a call to InsertRecord()
	
	//Insert a value in column
	iTheView->WriteBoolL( TPtrC(ISP_ENABLE_SW_COMP), ETrue );

	// We should never get here because we should have hit a panic
	// If the Panic doesnt happen, we'll cause a leave

	User::Leave( KErrGeneral );

	return ret;
	}


TVerdict CCommDbTest009_07::doTestStepL( )
	{
	//This test step needs to trap a Panic

	TRAPD( r, executeStepL() );

	if ( r == KErrNone )
		SetTestStepResult(EFail);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}







/////////////////////
//	Test step 009.08
/////////////////////

// constructor
CCommDbTest009_08::CCommDbTest009_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_08"));

	}

// destructor
CCommDbTest009_08::~CCommDbTest009_08()
	{
	}


TVerdict CCommDbTest009_08::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest009_08::executeStepL()
	{
	TInt ret=KErrNone;

	//NB. This test will cause a PANIC
	//Try to PutRecordChanges() without a call to InsertRecord()

	//Insert a value
	iTheView->WriteUintL( TPtrC(ISP_ENABLE_SW_COMP), ISP_ENABLE_SW_COMP_TEST_VALUE );

	//Must write to these columns as they cannot hold null values
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME), ISP_IF_NAME_TEST_VALUE );
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),COMMDB_NAME_TEST_VALUE);

	iTheView->PutRecordChanges();

	// We should never get here because we should have hit a panic
	// If the Panic doesnt happen, we'll cause a leave

	User::Leave( KErrGeneral );

	return ret;
	}


TVerdict CCommDbTest009_08::doTestStepL( )
	{
	//This test step needs to trap a Panic

	if ( executeStepL() ==  KErrNone )
		SetTestStepResult(EFail);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 009.09
/////////////////////

// constructor
CCommDbTest009_09::CCommDbTest009_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_09"));
	}

// destructor
CCommDbTest009_09::~CCommDbTest009_09()
	{
	}


TVerdict CCommDbTest009_09::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}



TInt CCommDbTest009_09::executeStepL()
	{
	return KErrInUse;
	}


TVerdict CCommDbTest009_09::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 009.09 called"));
			
	//This test step needs to trap a Panic

	if ( executeStepL() == KErrInUse )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);	return TestStepResult();
	}

/////////////////////
//	Test step 009.10
/////////////////////

// constructor
CCommDbTest009_10::CCommDbTest009_10()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_10"));
	}

// destructor
CCommDbTest009_10::~CCommDbTest009_10()
	{
	}


TVerdict CCommDbTest009_10::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define ISP_INIT_STRING_LONG_VALUE	_L8("aVeryVeryLongColumnValueThatIsLongerThanFiftyCharacters")


TInt CCommDbTest009_10::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//Try to insert a record
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	//Write a long string
	TRAP(ret, iTheView->WriteTextL( TPtrC(ISP_INIT_STRING), ISP_INIT_STRING_LONG_VALUE ) );
	
	return ret;
	}


TVerdict CCommDbTest009_10::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 009.10 called"));

		if (  executeStepL() == KErrOverflow )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}




/////////////////////
//	Test step 009.11
/////////////////////

// constructor
CCommDbTest009_11::CCommDbTest009_11()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_11"));
	}

// destructor
CCommDbTest009_11::~CCommDbTest009_11()
	{
	}

TVerdict CCommDbTest009_11::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 009.11 called "));

	SetTestStepResult(EPass);
	// Have backup of Database in c:\default7.dat
	// Heap test for 009.01

	CCommDbTest009_01* step009_01 = new(ELeave) CCommDbTest009_01;
	CleanupStack::PushL(step009_01);
	doTestStepWithHeapFailureL( *step009_01, KErrNone);
	if ( step009_01->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_01);


	return TestStepResult();

	}


// constructor
CCommDbTest009_12::CCommDbTest009_12()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_12"));
	}

// destructor
CCommDbTest009_12::~CCommDbTest009_12()
	{
	}

TVerdict CCommDbTest009_12::doTestStepL( )
	{
		// store the name of this test case
	SetTestStepName(_L("step_009_12"));
	SetTestStepResult(EPass);	
	CCommDbTest009_02* step009_02 = new(ELeave) CCommDbTest009_02;
	CleanupStack::PushL(step009_02);
	doTestStepWithHeapFailureL( *step009_02, KErrNone);
	if ( step009_02->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_02);

	return TestStepResult();
	}
	
// constructor
CCommDbTest009_13::CCommDbTest009_13()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_13"));
	}

// destructor
CCommDbTest009_13::~CCommDbTest009_13()
	{
	}

TVerdict CCommDbTest009_13::doTestStepL( )
	{
		// store the name of this test case
	SetTestStepName(_L("step_009_13"));
	SetTestStepResult(EPass);	
		// Heap test for 009.03
	CCommDbTest009_03* step009_03 = new(ELeave) CCommDbTest009_03;
	CleanupStack::PushL(step009_03);
	doTestStepWithHeapFailureL( *step009_03, KErrNone);
	if ( step009_03->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_03);
	
	return TestStepResult();
	}
	
// constructor
CCommDbTest009_14::CCommDbTest009_14()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_14"));
	}

// destructor
CCommDbTest009_14::~CCommDbTest009_14()
	{
	}

TVerdict CCommDbTest009_14::doTestStepL( )
	{
		// store the name of this test case
	SetTestStepName(_L("step_009_14"));
	SetTestStepResult(EPass);	
		// Heap test for 009.04
	CCommDbTest009_04* step009_04 = new(ELeave) CCommDbTest009_04;
	CleanupStack::PushL(step009_04);
	doTestStepWithHeapFailureL( *step009_04, KErrNone);
	if ( step009_04->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_04);

	return TestStepResult();
	}
	
// constructor
CCommDbTest009_15::CCommDbTest009_15()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_15"));
	}

// destructor
CCommDbTest009_15::~CCommDbTest009_15()
	{
	}

TVerdict CCommDbTest009_15::doTestStepL( )
	{
		// store the name of this test case
	SetTestStepName(_L("step_009_15"));
	SetTestStepResult(EPass);	
		// Heap test for 009.05
	CCommDbTest009_05* step009_05 = new(ELeave) CCommDbTest009_05;
	CleanupStack::PushL(step009_05);
	doTestStepWithHeapFailureL( *step009_05, KErrNone);
	if ( step009_05->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_05);
	
			// Heap test for 009.06
	CCommDbTest009_06* step009_06 = new(ELeave) CCommDbTest009_06;
	CleanupStack::PushL(step009_06);
	doTestStepWithHeapFailureL( *step009_06, KErrNotFound);
	if ( step009_06->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_06);

	return TestStepResult();
	}
	
// constructor
CCommDbTest009_16::CCommDbTest009_16()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_16"));
	}

// destructor
CCommDbTest009_16::~CCommDbTest009_16()
	{
	}

TVerdict CCommDbTest009_16::doTestStepL( )
	{
		// store the name of this test case
	SetTestStepName(_L("step_009_16"));
	SetTestStepResult(EPass);	
		// Heap test for 009.09
	CCommDbTest009_09* step009_09 = new(ELeave) CCommDbTest009_09;
	CleanupStack::PushL(step009_09);
	doTestStepWithHeapFailureL( *step009_09, KErrInUse);
	if ( step009_09->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_09);

		// Heap test for 009.10
	CCommDbTest009_10* step009_10 = new(ELeave) CCommDbTest009_10;
	CleanupStack::PushL(step009_10);
	doTestStepWithHeapFailureL( *step009_10, KErrOverflow);
	if ( step009_10->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_10);

	return TestStepResult();
	}
	

// constructor
CCommDbTest009_17::CCommDbTest009_17()
{
	// store the name of this test case
	SetTestStepName(_L("step_009_17"));
}

// destructor
CCommDbTest009_17::~CCommDbTest009_17()
{
}

TVerdict CCommDbTest009_17::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest009_17::executeStepL()
{
	TBuf<50> buff;

	TInt ret(0);

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone );

	iTheView->UpdateRecord();
	
	iTheView->SetNullL( TPtrC(ISP_IF_CALLBACK_INFO ));
	
	User::LeaveIfError(iTheView->PutRecordChanges());
		
	iTheView->ReadTextL(TPtrC(ISP_IF_CALLBACK_INFO ), buff);

	if(buff.Length() == 0)
		{
		ret = KErrNone;
		}
	else
		{
		ret	= KErrGeneral;
		}

	return ret;
}

// do Test step 009.17
TVerdict CCommDbTest009_17::doTestStepL( )
{

	INFO_PRINTF1(_L("Step 009.17 called "));
    if (executeStepL() == KErrNone)
	SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
}

/////////////////////
//	Test step 009.18
/////////////////////

// constructor
CCommDbTest009_18::CCommDbTest009_18()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_18"));
	}

// destructor
CCommDbTest009_18::~CCommDbTest009_18()
	{
	}

TVerdict CCommDbTest009_18::doTestStepPreambleL()
	{
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest009_18::executeStepL()
	{
	TBuf<KCommsDbSvrMaxColumnNameLength> tableName;
	SetTestStepResult(EFail);	iTheView->GetTableName( tableName ); 
	if ( tableName.Compare( TPtrC(DIAL_OUT_ISP)) != KErrNone )
		{	
		SetTestStepResult(EFail);		}
	else
		{
		SetTestStepResult(EPass);		} 
	return TestStepResult();
	}

// do Test step 009.18
TVerdict CCommDbTest009_18::doTestStepL( )
	{
	SetTestStepResult(EFail);	INFO_PRINTF1(_L("Step 009.18 called "));
	if (executeStepL() != KErrNone)
		{
		SetTestStepResult(EFail);		}
	else
		{
		SetTestStepResult(EPass);		}
	return TestStepResult();
	}

/////////////////////
//	Test step 009.19
/////////////////////

// constructor
CCommDbTest009_19::CCommDbTest009_19()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_19"));
	}

// destructor
CCommDbTest009_19::~CCommDbTest009_19()
	{
	}

TVerdict CCommDbTest009_19::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 009.19 called "));

	SetTestStepResult(EPass);
		// Heap test for 009.17
		
	CCommDbTest009_17* step009_17 = new(ELeave) CCommDbTest009_17;
	CleanupStack::PushL(step009_17);
	if ( doTestStepWithHeapFailureL( *step009_17, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_17);

	return TestStepResult();	

	}

/////////////////////
//	Test step 009.20
/////////////////////

// constructor
CCommDbTest009_20::CCommDbTest009_20()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_20"));
	}

// destructor
CCommDbTest009_20::~CCommDbTest009_20()
	{
	}

TVerdict CCommDbTest009_20::doTestStepL( )
	{

	SetTestStepResult(EPass);
		// Heap test for 009.07
		
	CCommDbTest009_07* step009_07 = new(ELeave) CCommDbTest009_07;
	CleanupStack::PushL(step009_07);
	if ( doTestStepWithHeapFailureL( *step009_07, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_07);

	return TestStepResult();	

	}

/////////////////////
//	Test step 009.21
/////////////////////

// constructor
CCommDbTest009_21::CCommDbTest009_21()
	{
	// store the name of this test case
	SetTestStepName(_L("step_009_21"));
	}

// destructor
CCommDbTest009_21::~CCommDbTest009_21()
	{
	}

TVerdict CCommDbTest009_21::doTestStepL( )
	{

	SetTestStepResult(EPass);
		// Heap test for 009.08
		
	CCommDbTest009_08* step009_08 = new(ELeave) CCommDbTest009_08;
	CleanupStack::PushL(step009_08);
	if ( doTestStepWithHeapFailureL( *step009_08, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step009_08);

	return TestStepResult();	

	}

//EOF
