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
// This contains CommDb Unit Test Cases 013.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>


// Test system includes
#include "Teststepcommdb.h"
#include "Step_013_xx.h"



/////////////////////
//	Test step 013.001
/////////////////////

// constructor
CCommDbTest013_01::CCommDbTest013_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_013_01"));
	}

// destructor
CCommDbTest013_01::~CCommDbTest013_01()
	{
	}


TVerdict CCommDbTest013_01::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_IN_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest013_01::executeStepL()
	{
	TInt ret=KErrGeneral;


	//Update an existing record

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	
	//Check the value is what we expect 
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	TEST( !value.Compare(_L("Record #1")) );

	//Update the value in the field
	TRAPD(r, iTheView->UpdateRecord() );
	iTheView->WriteTextL( TPtrC(COMMDB_NAME), _L("Updated Record #1") );
	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );

	delete iTheView;
	iTheView = 0;

	//Open the view again and check for the updated value
	iTheView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP) );
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	
	if ( !value.Compare(_L("Updated Record #1")) )
		{
		//Found the record, even though it is hidden
		ret = KErrNone;
		}

	if ( r!= KErrNone )
		{
		ret = r;
		}


	if ( iLocalObjects )
		popAndDestroyDatabaseObjects();

	return ret;
	}



TVerdict CCommDbTest013_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 013.01 called "));

	
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 013.02
/////////////////////

// constructor
CCommDbTest013_02::CCommDbTest013_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_013_02"));
	}

// destructor
CCommDbTest013_02::~CCommDbTest013_02()
	{
	}


TVerdict CCommDbTest013_02::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest013_02::executeStepL()
	{
	TInt ret=KErrGeneral;
	//This view must contain zero records
	//NB. This test will crash the server

	iTheView = iTheDb->OpenViewLC(TPtrC(DIAL_IN_ISP), _L("Select * from DialInISP where IfNetworks='xxx'") );
	CleanupStack::Pop();

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNotFound );
	
	TRAPD(r, ret = iTheView->UpdateRecord() );

	if ( r!= KErrNone )
		{
		ret = r;
		}

	return ret;
	}



TVerdict CCommDbTest013_02::doTestStepL( )
	{
	TRAPD(ret, executeStepL());
	if( ret == KErrNotSupported )
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 013.03
/////////////////////

// constructor
CCommDbTest013_03::CCommDbTest013_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_013_03"));
	}

// destructor
CCommDbTest013_03::~CCommDbTest013_03()
	{
	}

TVerdict CCommDbTest013_03::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_IN_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest013_03::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//Try to insert a read only record
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	//Insert some text
	iTheView->WriteTextL( TPtrC(COMMDB_NAME), _L("Example record name") ) ;

	//Must write to these columns as they cannot hold null values
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME), _L("PPP") );
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);

	//Store the record as hidden
	CDBLEAVE( iTheView->PutRecordChanges(EFalse, ETrue), KErrNone );

	delete iTheView;
	iTheView = 0;

    iTheView = iTheDb->OpenViewMatchingTextLC(TPtrC(DIAL_IN_ISP), TPtrC(COMMDB_NAME),  _L("Example record name") );
	CleanupStack::Pop();
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TRAPD(r, ret = iTheView->UpdateRecord() );

	delete iTheView;
	iTheView = 0;


	if ( r!= KErrNone )
		{
		ret = r;
		}

	return ret;
	}



TVerdict CCommDbTest013_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 013.03 called "));

	if ( executeStepL() == KErrAccessDenied )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}






	
/////////////////////
//	Test step 013.04
/////////////////////

// constructor
CCommDbTest013_04::CCommDbTest013_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_013_04"));
	}

// destructor
CCommDbTest013_04::~CCommDbTest013_04()
	{
	}

TVerdict CCommDbTest013_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 013.04 called "));

	SetTestStepResult(EPass);
	// Heap test for 013.01

	CCommDbTest013_01* step013_01 = new CCommDbTest013_01;
	CleanupStack::PushL(step013_01);
	if ( doTestStepWithHeapFailureL( *step013_01, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step013_01);

	// Heap test for 013.02
	// SQl interface is deprecated from 9.1

	CCommDbTest013_02* step013_02 = new CCommDbTest013_02;
	CleanupStack::PushL(step013_02);
	if ( doTestStepWithHeapFailureL( *step013_02, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step013_02);

	// Heap test for 013.03

	CCommDbTest013_03* step013_03 = new CCommDbTest013_03;
	CleanupStack::PushL(step013_03);
	doTestStepWithHeapFailureL( *step013_03, KErrAccessDenied);
	if ( step013_03->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step013_03);

	return TestStepResult();	

}
