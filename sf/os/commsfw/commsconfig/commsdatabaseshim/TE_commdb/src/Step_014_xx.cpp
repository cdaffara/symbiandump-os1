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
// This contains CommDb Unit Test Cases 014.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>


// Test system includes
#include "Teststepcommdb.h"
#include "Step_014_xx.h"



/////////////////////
//	Test step 014.01
/////////////////////

// constructor
CCommDbTest014_01::CCommDbTest014_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_014_01"));
	}

// destructor
CCommDbTest014_01::~CCommDbTest014_01()
	{
	}

TVerdict CCommDbTest014_01::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest014_01::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//Create a record and cancel it
	//NB. This test assumes the Modem table will be empty

	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );
	
	_LIT(KModem,"US Robotics Sportster");
	_LIT(KTsy,"HAYES");
	_LIT8(KModemInitString,"AT&F1");
	_LIT8(KModemFaxInitString,"AT&d2");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KModem);
	iTheView->WriteTextL(TPtrC(MODEM_NIF_NAME),_L("PPP"));
	iTheView->WriteTextL(TPtrC(MODEM_TSY_NAME),KTsy);
	iTheView->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps57600);
	iTheView->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
	iTheView->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
	iTheView->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
	iTheView->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)(KConfigObeyCTS|KConfigObeyDSR|KConfigFailDSR|KConfigObeyDCD|KConfigFailDCD));	//KConfigObeyCTS;
	iTheView->WriteUintL(TPtrC(MODEM_SPECIAL_RATE),(TUint32)0);
	iTheView->WriteUintL(TPtrC(MODEM_XON_CHAR),(TUint32)0);
	iTheView->WriteUintL(TPtrC(MODEM_XOFF_CHAR),(TUint32)0);
	iTheView->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING),KModemInitString);
	iTheView->WriteTextL(TPtrC(MODEM_DATA_INIT_STRING),KNullDesC8);
	iTheView->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING),KModemFaxInitString);
	iTheView->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingAfterDialUntilAnswer);
	iTheView->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);

	//Now cancel the insert and close the view
	iTheView->CancelRecordChanges();
	
	delete iTheView;
	iTheView = 0;

	//Open a view and check record was not written into the database
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), TPtrC(COMMDB_NAME), KModem);
	CleanupStack::Pop();

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNotFound );

	return ret;
	}



TVerdict CCommDbTest014_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 014.01 called "));

	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 014.02
/////////////////////

// constructor
CCommDbTest014_02::CCommDbTest014_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_014_02"));
	}

// destructor
CCommDbTest014_02::~CCommDbTest014_02()
	{
	}

TVerdict CCommDbTest014_02::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest014_02::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	
	//Insert a new record, try to update it and cancel the update
	//NB. This test assumes the Modem table will be empty

	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );
	
	_LIT(KModem,"US Robotics Sportster");
	_LIT(KTsy,"HAYES");
	_LIT8(KModemInitString,"AT&F1");
	_LIT8(KModemFaxInitString,"AT&d2");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KModem);
	iTheView->WriteTextL(TPtrC(MODEM_NIF_NAME),_L("PPP"));
	iTheView->WriteTextL(TPtrC(MODEM_TSY_NAME),KTsy);
	iTheView->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps57600);
	iTheView->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
	iTheView->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
	iTheView->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
	iTheView->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)(KConfigObeyCTS|KConfigObeyDSR|KConfigFailDSR|KConfigObeyDCD|KConfigFailDCD));	//KConfigObeyCTS;
	iTheView->WriteUintL(TPtrC(MODEM_SPECIAL_RATE),(TUint32)0);
	iTheView->WriteUintL(TPtrC(MODEM_XON_CHAR),(TUint32)0);
	iTheView->WriteUintL(TPtrC(MODEM_XOFF_CHAR),(TUint32)0);
	iTheView->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING),KModemInitString);
	iTheView->WriteTextL(TPtrC(MODEM_DATA_INIT_STRING),KNullDesC8);
	iTheView->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING),KModemFaxInitString);
	iTheView->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingAfterDialUntilAnswer);
	iTheView->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);

	//Insert the record and destroy the view
	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
	
	delete iTheView;
	iTheView = 0;

	//Open a view and make a change to the record just written
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), TPtrC(COMMDB_NAME), KModem);
	CleanupStack::Pop();

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone );
	CDBLEAVE( iTheView->UpdateRecord(), KErrNone );
	_LIT(KModem2,"Another US Robotics Sportster");
	iTheView->WriteTextL(TPtrC(COMMDB_NAME), KModem2 );
	
	//Cancel the changes made and destroy the view
	iTheView->CancelRecordChanges();

	delete iTheView;
	iTheView = 0;

	//Open a view to check that the record was not modified
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), TPtrC(COMMDB_NAME), KModem);
	CleanupStack::Pop();

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone );

	return ret;
	}



TVerdict CCommDbTest014_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 014.02 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);


	return TestStepResult();
	}


/////////////////////
//	Test step 014.03
/////////////////////

// constructor
CCommDbTest014_03::CCommDbTest014_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_014_03"));
	}

// destructor
CCommDbTest014_03::~CCommDbTest014_03()
	{
	}

TVerdict CCommDbTest014_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 014.03 called "));

	SetTestStepResult(EPass);
		// Heap test for 014.01

	CCommDbTest014_01* step014_01 = new CCommDbTest014_01;
	CleanupStack::PushL(step014_01);
	doTestStepWithHeapFailureL( *step014_01, KErrNotFound);
	if ( step014_01->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step014_01);

		// Heap test for 014.02

	CCommDbTest014_02* step014_02 = new CCommDbTest014_02;
	CleanupStack::PushL(step014_02);
	doTestStepWithHeapFailureL( *step014_02, KErrNone);
	if ( step014_02->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step014_02);

	return TestStepResult();	

}

/////////////////////
//	Test step 014.04
/////////////////////

// constructor
CCommDbTest014_04::CCommDbTest014_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_014_04"));
	}

// destructor
CCommDbTest014_04::~CCommDbTest014_04()
	{
	}

TVerdict CCommDbTest014_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 014_04 called "));

	SetTestStepResult(EPass);
__UHEAP_MARK;
 

    CCommsDatabase* db = CCommsDatabase::NewL( );
    CleanupStack::PushL( db );
   
    CCommsDbTableView* v =
        db->OpenTableLC( TPtrC(WAP_ACCESS_POINT ) );

    TUint32 dummyUid( 0 );


    TInt err = v->InsertRecord( dummyUid );
    User::LeaveIfError( err );
    v->CancelRecordChanges();

   
    CleanupStack::PopAndDestroy( v );
    CleanupStack::PopAndDestroy( db );

__UHEAP_MARKEND;

	return TestStepResult();	

}
