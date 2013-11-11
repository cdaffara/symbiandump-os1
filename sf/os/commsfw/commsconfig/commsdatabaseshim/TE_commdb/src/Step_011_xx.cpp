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
// This contains CommDb Unit Test Cases 011.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>


// Test system includes
#include "Teststepcommdb.h"
#include "Step_011_xx.h"



/////////////////////
//	Test step 011.001
/////////////////////

// constructor
CCommDbTest011_01::CCommDbTest011_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_011_01"));
	}

// destructor
CCommDbTest011_01::~CCommDbTest011_01()
	{
	}


TVerdict CCommDbTest011_01::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define HIDDEN_RECORD_TEST_VALUE	_L("This is a hidden record")

TInt CCommDbTest011_01::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//Create a hidden record in the database
	//NB. This step will panic if the bale used is empty, so use a
	//populated table to simplify things.
	
	//Try to insert a record
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	//Insert some text
	iTheView->WriteTextL( TPtrC(COMMDB_NAME), HIDDEN_RECORD_TEST_VALUE ) ;

	//Must write to these columns as they cannot hold null values
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME), _L("PPP") );
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);

	//Store the record as hidden
	CDBLEAVE( iTheView->PutRecordChanges(ETrue), KErrNone );

	//Now search through the table looking for this record
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	TBuf<KCommsDbSvrMaxFieldLength> value;
	do {

		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
		if ( !value.Compare(HIDDEN_RECORD_TEST_VALUE) )
			{
				//Found the record, even though it is hidden
				ret = KErrGeneral;
				break;
			}
		//Move on to the next record
		TRAPD(r, ret = iTheView->GotoNextRecord() );
		if( r!= KErrNone )
			User::Leave( r );
		} while (ret == KErrNone ) ;

	return ret;
	}



TVerdict CCommDbTest011_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 011.01 called "));

		
	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}

 /////////////////////
 //	Test step 011.02
 /////////////////////
 
 // constructor
CCommDbTest011_02::CCommDbTest011_02()
 	{
 	// store the name of this test case
	SetTestStepName(_L("step_011_02"));
 	}
 
 // destructor
CCommDbTest011_02::~CCommDbTest011_02()
 	{
 	}
 
TVerdict CCommDbTest011_02::doTestStepPreambleL()
 	{	
 	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
         SetTestStepResult(EPass); 	return TestStepResult();
 	}
 
 
TInt CCommDbTest011_02::executeStepL()
 	{
 	TInt ret=KErrGeneral;
 	TUint32 id;

	//Create a hidden record in the database, try to find it,
	//make a call to ShowHiddenRecords the try to find the record again
	
	//Try to insert a test record
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	//Insert some text
	iTheView->WriteTextL( TPtrC(COMMDB_NAME), HIDDEN_RECORD_TEST_VALUE );

	//Must write to these columns as they cannot hold null values
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME), _L("PPP") );
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);

	//Store the record as hidden
	CDBLEAVE( iTheView->PutRecordChanges(ETrue), KErrNone );

	//Now search through the table looking for this record
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//This first search through the table should not find the hidden record
	TBuf<KCommsDbSvrMaxFieldLength> value;
	do {

		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
		if ( !value.Compare(HIDDEN_RECORD_TEST_VALUE) )
			{
				//Found the record
				ret = KErrGeneral;
				break;
			}
		//Move on to the next record
		TRAPD(r, ret= iTheView->GotoNextRecord() );
		if ( r!=KErrNone )
			User::Leave( r );

		}while(ret == KErrNone );
	
	if ( ret == KErrNotFound )
		{

		//Search again, but this this time we'll show the hidden records
		delete iTheView;
		iTheView = 0;

		iTheDb->ShowHiddenRecords();
		iTheView=iTheDb->OpenTableLC( TPtrC(DIAL_OUT_ISP) );
		CleanupStack::Pop();

		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
		TBuf<KCommsDbSvrMaxFieldLength> value;

		do {
			iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
			if ( !value.Compare(HIDDEN_RECORD_TEST_VALUE) )
				{
					//Found the record
					ret = KErrNone;

					//Change the record not to be hidden for the test 11_03
					CDBLEAVE( iTheView->UpdateRecord(), KErrNone );
					CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
					break;
				}
			//Move on to the next record
			TRAPD(r, ret = iTheView->GotoNextRecord() )
			if ( r!=KErrNone )
				User::Leave( r );

			}while( ret == KErrNone );
		}

	return ret;
	}

TVerdict CCommDbTest011_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 011.02 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}


/////////////////////
//	Test step 011.03
/////////////////////

// constructor
CCommDbTest011_03::CCommDbTest011_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_011_03"));
	}

// destructor
CCommDbTest011_03::~CCommDbTest011_03()
	{
	}

TVerdict CCommDbTest011_03::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest011_03::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Check if a record from test 11_02 is readable again
	
	//Now search through the table looking for this record
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//This first search through the table should not find the hidden record
	TBuf<KCommsDbSvrMaxFieldLength> value;
	do {

		iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
		if ( !value.Compare(HIDDEN_RECORD_TEST_VALUE) )
			{
				//Found the record
				ret = KErrNone;
				break;
			}
		//Move on to the next record
		TRAPD(r, ret= iTheView->GotoNextRecord() );
		if ( r!=KErrNone )
			User::Leave( r );

		}while(ret == KErrNone );

	return ret;
	}


TVerdict CCommDbTest011_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 011.03 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}


/////////////////////
//	Test step 011.04
/////////////////////

// constructor
CCommDbTest011_04::CCommDbTest011_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_011_04"));
	}

// destructor
CCommDbTest011_04::~CCommDbTest011_04()
	{
	}

TVerdict CCommDbTest011_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 011.04 called "));

	SetTestStepResult(EPass);
	// Heap test for 011.01

	CCommDbTest011_01* step011_01 = new CCommDbTest011_01;
	CleanupStack::PushL(step011_01);
	if ( doTestStepWithHeapFailureL( *step011_01, KErrNotFound) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step011_01);


	// Heap test for 011.02
	CCommDbTest011_02* step011_02 = new CCommDbTest011_02;
	CleanupStack::PushL(step011_02);
	if ( doTestStepWithHeapFailureL( *step011_02, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step011_02);

	// Heap test for 011.03
	CCommDbTest011_03* step011_03 = new CCommDbTest011_03;
	CleanupStack::PushL(step011_03);
	if ( doTestStepWithHeapFailureL( *step011_03, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step011_03);

	return TestStepResult();	

}
