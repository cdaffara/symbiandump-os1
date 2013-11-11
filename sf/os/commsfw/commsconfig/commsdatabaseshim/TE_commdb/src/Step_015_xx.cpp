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
// This contains CommDb Unit Test Cases 015.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>


// Test system includes
#include "Teststepcommdb.h"
#include "Step_015_xx.h"



/////////////////////
//	Test step 015.01
/////////////////////

// constructor
CCommDbTest015_01::CCommDbTest015_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_01"));
	}

// destructor
CCommDbTest015_01::~CCommDbTest015_01()
	{
	}


TVerdict CCommDbTest015_01::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest015_01::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Check the inTransaction flag

	TRAPD(r, iTheDb->BeginTransaction() );

	ret  = !(iTheDb->InTransaction() );

	if ( r != KErrNone )
		{
		ret = r;
		}


	return ret;
	}



TVerdict CCommDbTest015_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.01 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 015.02
/////////////////////

// constructor
CCommDbTest015_02::CCommDbTest015_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_02"));
	}

// destructor
CCommDbTest015_02::~CCommDbTest015_02()
	{
	}

TVerdict CCommDbTest015_02::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}
	


TInt CCommDbTest015_02::executeStepL()
	{
	TInt ret=KErrGeneral;

	TUint32 id;
	TBool beforeValue=EFalse, afterValue=EFalse;

	
	//Check that the write lock flag is set when the
	//database is written to.
	//Take a copy of the flag before we insert a record
	TRAPD(r, beforeValue = iTheDb->IsDatabaseWriteLockedL() );

	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
	
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	//Take a copy after the insert
	afterValue = iTheDb->IsDatabaseWriteLockedL();

	iTheView->CancelRecordChanges();

	//Now get the flag value after the record has been written

	if ( r == KErrNone )
		{
		if ( !beforeValue  && afterValue  )
			ret = KErrNone;
		}
		else
		{	
			//return the trap value
			ret = r;
		}

	return ret;
	}



TVerdict CCommDbTest015_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.02 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 015.03
/////////////////////

// constructor
CCommDbTest015_03::CCommDbTest015_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_03"));
	// default to no local objects allocated
	}

// destructor
CCommDbTest015_03::~CCommDbTest015_03()
	{
	}

TVerdict CCommDbTest015_03::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest015_03::executeStepL()
	{
	TUint32 id;

	//Commit a record and check for correct state of flags

	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );

	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	_LIT(KName,"CCommDbTest015_03");
	_LIT(KTsy,"HAYES");
	_LIT8(KModemInitString,"AT&F1");
	_LIT8(KModemFaxInitString,"AT&d2");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KName);
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

	CDBLEAVE ( iTheView->PutRecordChanges(), KErrNone );
	User::LeaveIfError(iTheDb->CommitTransaction());

	delete iTheView;
	iTheView=NULL;

	//Create a new view that should have the record in it
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), TPtrC(COMMDB_NAME), KName);
	CleanupStack::Pop();

	User::LeaveIfError(iTheView->GotoFirstRecord());	//Will leave if first record isn't there

	if (! iTheDb->IsDatabaseWriteLockedL() && ! iTheDb->InTransaction() )
			return KErrNone;

	return KErrGeneral;
	}



TVerdict CCommDbTest015_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.03 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}





/////////////////////
//	Test step 015.04
/////////////////////

// constructor
CCommDbTest015_04::CCommDbTest015_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_04"));
	// default to no local objects allocated
	}

// destructor
CCommDbTest015_04::~CCommDbTest015_04()
	{
	}

TVerdict CCommDbTest015_04::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest015_04::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	
	//Check that we can rollback a transaction
	//NB Test assumes that the table is empty


	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	_LIT(KName,"CCommDbTest015_04");
	_LIT(KTsy,"HAYES");
	_LIT8(KModemInitString,"AT&F1");
	_LIT8(KModemFaxInitString,"AT&d2");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KName);
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
	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );

	//Rollback the record we've just written
	TRAPD(r, iTheDb->RollbackTransaction() );

	//Get rid of our view
	delete iTheView;
	iTheView=0;

	//Create a new view that should have the record in it
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), TPtrC(COMMDB_NAME), KName);

	CleanupStack::Pop();

	ret = iTheView->GotoFirstRecord();

	if ( r == KErrNone )
		{
		if ( ret == KErrNotFound )
			ret = KErrNone;
		else
			ret = KErrGeneral;
		}
	else
		{	
		//Trap occoured, so return that value instead
		ret = r;
		}

	
	return ret;
	}

TVerdict CCommDbTest015_04::doTestStepPostambleL()
	{
	
	//Get rid of our view
	if(iTheView)
		{
		delete iTheView;
		}
	iTheView=0;

	_LIT(KName,"CCommDbTest015_04");
	//Create a new view that should have the record that was added in executeStepL and
	// may have been left in by a leave condition
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), TPtrC(COMMDB_NAME), KName);
	CleanupStack::Pop();

	if(iTheView->GotoFirstRecord() == KErrNone)
		{
		iTheView->DeleteRecord();
		}

	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest015_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.04 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 015.05
/////////////////////

// constructor
CCommDbTest015_05::CCommDbTest015_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_05"));
	}

// destructor
CCommDbTest015_05::~CCommDbTest015_05()
	{
	}


TVerdict CCommDbTest015_05::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest015_05::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//Try to rollback without a prior call to BeginTransaction()

	//
	// NB. This test will cause the DBMS server to Panic!
	//


	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	_LIT(KName,"CCommDbTest015_05");
	_LIT(KTsy,"HAYES");
	_LIT8(KModemInitString,"AT&F1");
	_LIT8(KModemFaxInitString,"AT&d2");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KName);
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

	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
	//This will cause a panic
	TRAPD(r, iTheDb->RollbackTransaction() );

	delete iTheView ;
	iTheView=0;

	//Create a new view that would have the record in it
	iTheView = iTheDb->OpenViewLC(TPtrC(MODEM_BEARER), _L("Select * from Modem where Name='CCommDbTest015_05'") );
	CleanupStack::Pop();


	ret = iTheView->GotoFirstRecord();

	if ( r == KErrNone )
		{
		if ( ret == KErrNotFound  )
			ret = KErrNone;
		else
			ret = KErrGeneral;
		}
	else
		{	
		//Trap occoured, so return that value instead
		ret = r;
		}


	return ret;
	}



TVerdict CCommDbTest015_05::doTestStepPostambleL()
	{
	//Get rid of our view
	if(iTheView)
		{
		delete iTheView;
		}
	iTheView=0;

	_LIT(KName,"CCommDbTest015_05");
	//Create a new view that should have the record that was added in executeStepL and
	// may have been left in by a leave condition
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), TPtrC(COMMDB_NAME), KName);
	CleanupStack::Pop();

	if(iTheView->GotoFirstRecord() == KErrNone)
		{
		iTheView->DeleteRecord();
		}

	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}


TVerdict CCommDbTest015_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.05 called "));

	TRAPD(leaveCode, executeStepL());
	if (leaveCode != KErrNotSupported) // SQL views no longer supported
		{
		SetTestStepResult(EFail);		}

	return TestStepResult();
	}


/////////////////////
//	Test step 015.06
/////////////////////

// constructor
CCommDbTest015_06::CCommDbTest015_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_06"));
	}

// destructor
CCommDbTest015_06::~CCommDbTest015_06()
	{
	}

TVerdict CCommDbTest015_06::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest015_06::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//Try to commit without a prior call to BeginTransaction()

	//
	// NB. This test will cause the DBMS server to Panic!
	//


	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	_LIT(KName,"CCommDbTest015_06");
	_LIT(KTsy,"HAYES");
	_LIT8(KModemInitString,"AT&F1");
	_LIT8(KModemFaxInitString,"AT&d2");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KName);
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

	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
	//This will cause a panic
	TRAPD(r, iTheDb->CommitTransaction() );

	delete  iTheView ;
	iTheView=0;

	//Create a new view that would have the record in it
	iTheView = iTheDb->OpenViewLC(TPtrC(MODEM_BEARER), _L("Select * from Modem where Name='CCommDbTest015_06'") );
	CleanupStack::Pop();

	ret = iTheView->GotoFirstRecord();

	if ( r == KErrNone )
		{
		if ( ret == KErrNotFound  )
			ret = KErrNone;
		else
			ret = KErrGeneral;
		}
	else
		{	
		//Trap occoured, so return that value instead
		ret = r;
		}


	return ret;
	}



TVerdict CCommDbTest015_06::doTestStepPostambleL()
	{
	//Get rid of our view
	if(iTheView)
		{
		delete iTheView;
		}
	iTheView=0;

	_LIT(KName,"CCommDbTest015_06");
	//Create a new view that should have the record that was added in executeStepL and
	// may have been left in by a leave condition
	iTheView = iTheDb->OpenViewMatchingTextLC( TPtrC(MODEM_BEARER), TPtrC(COMMDB_NAME), KName);
	CleanupStack::Pop();

	if(iTheView->GotoFirstRecord() == KErrNone)
		{
		iTheView->DeleteRecord();
		}

	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
        SetTestStepResult(EPass);	return TestStepResult();
	}


TVerdict CCommDbTest015_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.06 called "));

	TRAPD(leaveCode, executeStepL());
	if (leaveCode != KErrNotSupported) // SQL views no longer supported
		{
		SetTestStepResult(EFail);		}

	return TestStepResult();
	}



/////////////////////
//	Test step 015.07
/////////////////////

// constructor
CCommDbTest015_07::CCommDbTest015_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_07"));
	}

// destructor
CCommDbTest015_07::~CCommDbTest015_07()
	{
	}


TVerdict CCommDbTest015_07::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest015_07::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;


	//Close the database while inside a transaction
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );

	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	_LIT(KName,"CCommDbTest015_07");
	_LIT(KTsy,"HAYES");
	_LIT8(KModemInitString,"AT&F1");
	_LIT8(KModemFaxInitString,"AT&d2");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KName);
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

	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
	
	delete  iTheView ;
	iTheView=0;

	//Close the DB
	delete iTheDb;
	iTheDb = 0;

	//NB Only get here if we didnt trap, so it must have passed
	ret = KErrNone;


	return ret;
	}



TVerdict CCommDbTest015_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.07 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 015.08
/////////////////////

// constructor
CCommDbTest015_08::CCommDbTest015_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_08"));
	}

// destructor
CCommDbTest015_08::~CCommDbTest015_08()
	{
	}


TVerdict CCommDbTest015_08::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest015_08::tryDatabaseWriteL(void)
{
	TInt ret;

	//Open the database
	CCommsDatabase* theDb;
	theDb=CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(theDb);

	//Open a view
	CCommsDbTableView* theView;
	theView = theDb->OpenTableLC( TPtrC(DIAL_OUT_ISP) );

	//Try to write
	TUint32 id;
	ret = theView->InsertRecord( id );

	CleanupStack::PopAndDestroy(2);

	return ret;
}

//
//	This function is executed as a separate thread from Step 015_08.
//  It will try to write to the database, and should fail, as the database
//  will be write locked by CCommDbTest015_08::executeStepL()
//
TInt CCommDbTest015_08::step015_08Thread ( void * /*ptr*/ )
	{
	TInt ret=KErrGeneral;

	// get clean-up stack
	CTrapCleanup* cleanup=CTrapCleanup::New(); 

	if(!cleanup)
		return KErrNoMemory;
	//try to write to the database
	TRAPD(r, ret = tryDatabaseWriteL() );

	if ( r!=KErrNone )
		ret = r;

	// done with the clean up stack
	delete cleanup; 

	
	return ret;		
	}

TInt CCommDbTest015_08::executeStepL()
	{
	TInt ret=KErrGeneral;


	//Lock the database, then request another thread to attempt a write

	//Start an insert to lock the database
	TUint32 id;
	
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	TEST( iTheDb->IsDatabaseWriteLockedL() );
	
	//Set up a new thread
	RThread t;

	TInt res=t.Create(_L("Step015_08Thread"),step015_08Thread, KDefaultStackSize,&User::Heap(),0 );

	if ( res != KErrNone )
		User::Leave( res );

	// start the thread and request the status
	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);

	// get the test result
	ret =  stat.Int();

	TExitType exitVal = t.ExitType();	
	TEST( exitVal == EExitKill );
		
	// done with the test thread
	t.Close();

	return ret;
	}



TVerdict CCommDbTest015_08::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.08 called "));

	if ( executeStepL() == KErrLocked )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 015.09
/////////////////////

// constructor
CCommDbTest015_09::CCommDbTest015_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_09"));
	}

// destructor
CCommDbTest015_09::~CCommDbTest015_09()
	{
	}

TInt CCommDbTest015_09::executeStepL()
	{
	TInt ret=KErrGeneral;


	TPtrC tableName(MODEM_BEARER);

	iTheDb=CCommsDatabase::NewL();


	TUint32 recordToInsert;
	TUint32 recordToUpdate;
	TUint32 recordToDelete;
	TBuf8<128> fieldValue1, fieldValue2;
	_LIT8(KModemInitString, "ABCDE");

	////////////////////////////////////////////////////////////////////////////////
	// rollback transaction test

	// open view, insert record, delete view
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenTableLC(tableName);
		CleanupStack::Pop(iTheView);

		CDBLEAVE( iTheView->InsertRecord(recordToInsert), KErrNone );
		CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->RollbackTransaction();

	// the inserted record must be rolled back
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenViewMatchingUintLC(tableName, TPtrC(COMMDB_ID), recordToInsert);
		CleanupStack::Pop(iTheView);

		// there must be no records found
		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNotFound );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->RollbackTransaction();

	// open view, update record, delete view
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenTableLC(tableName);
		CleanupStack::Pop(iTheView);

		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
		iTheView->ReadUintL(TPtrC(COMMDB_ID), recordToUpdate);
		iTheView->ReadTextL(TPtrC(MODEM_MODEM_INIT_STRING), fieldValue1);
		CDBLEAVE( iTheView->UpdateRecord(), KErrNone );
		iTheView->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING), KModemInitString);
		CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->RollbackTransaction();

	// the updated record must be rolled back
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenViewMatchingUintLC(tableName, TPtrC(COMMDB_ID), recordToUpdate);
		CleanupStack::Pop(iTheView);

		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
		iTheView->ReadTextL(TPtrC(MODEM_MODEM_INIT_STRING), fieldValue2);
		
		if (fieldValue1 != fieldValue2)
			{
			User::Leave(KErrGeneral);
			}
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->RollbackTransaction();

	// open view, delete record, delete view
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenTableLC(tableName);
		CleanupStack::Pop(iTheView);

		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
		iTheView->ReadUintL(TPtrC(COMMDB_ID), recordToDelete);
		CDBLEAVE( iTheView->DeleteRecord(), KErrNone );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->RollbackTransaction();

	// the deleted record must be rolled back
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenViewMatchingUintLC(tableName, TPtrC(COMMDB_ID), recordToDelete);
		CleanupStack::Pop(iTheView);

		// the record must still be there
		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->RollbackTransaction();



	////////////////////////////////////////////////////////////////////////////////
	// commit transaction test

	// open view, insert record, delete view
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenTableLC(tableName);
		CleanupStack::Pop(iTheView);

		CDBLEAVE( iTheView->InsertRecord(recordToInsert), KErrNone );
		CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->CommitTransaction();

	// the inserted records must be commited
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenViewMatchingUintLC(tableName, TPtrC(COMMDB_ID), recordToInsert);
		CleanupStack::Pop(iTheView);

		// the record must have been inserted
		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
		CDBLEAVE( iTheView->GotoNextRecord(), KErrNotFound );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->RollbackTransaction();


	// open view, update record, delete view
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenTableLC(tableName);
		CleanupStack::Pop(iTheView);

		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
		iTheView->ReadUintL(TPtrC(COMMDB_ID), recordToUpdate);
		iTheView->ReadTextL(TPtrC(MODEM_MODEM_INIT_STRING), fieldValue1);
		CDBLEAVE( iTheView->UpdateRecord(), KErrNone );
		iTheView->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING), KModemInitString);
		CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->CommitTransaction();

	// the updated record must be rolled back
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenViewMatchingUintLC(tableName, TPtrC(COMMDB_ID), recordToUpdate);
		CleanupStack::Pop(iTheView);

		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
		iTheView->ReadTextL(TPtrC(MODEM_MODEM_INIT_STRING), fieldValue2);
		
		if (fieldValue2 != KModemInitString)
			{
			User::Leave(KErrGeneral);
			}
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->RollbackTransaction();


	// open view, delete record, delete view
	// unfortunately we cannot do heap marking here because of commsdtat notifications
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenTableLC(tableName);
		CleanupStack::Pop(iTheView);

		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
		iTheView->ReadUintL(TPtrC(COMMDB_ID), recordToDelete);
		CDBLEAVE( iTheView->DeleteRecord(), KErrNone );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->CommitTransaction();

	// the deleted records must be commited
	CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		{
		iTheView=iTheDb->OpenViewMatchingUintLC(tableName, TPtrC(COMMDB_ID), recordToDelete);
		CleanupStack::Pop(iTheView);

		// the record must have been deleted
		CDBLEAVE( iTheView->GotoFirstRecord(), KErrNotFound );
		
		delete  iTheView ;
		iTheView=0;
		}
	iTheDb->RollbackTransaction();

	//Close the DB
	delete iTheDb;
	iTheDb = 0;

	//NB Only get here if we didnt trap, so it must have passed
	ret = KErrNone;
	return ret;
	}



TVerdict CCommDbTest015_09::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.09 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}





/////////////////////
//	Test step 015.10
/////////////////////

// constructor
CCommDbTest015_10::CCommDbTest015_10()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_10"));
	}

// destructor
CCommDbTest015_10::~CCommDbTest015_10()
	{
	}

TVerdict CCommDbTest015_10::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.10 called "));

	SetTestStepResult(EPass);	
	// Have backup of Database in c:\default1.dat

		// Heap test for 015.01

	CCommDbTest015_01* step015_01 = new(ELeave) CCommDbTest015_01;
	CleanupStack::PushL(step015_01);
	doTestStepWithHeapFailureL( *step015_01, KErrNone);
	if ( step015_01->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step015_01);



	return TestStepResult();	

}

/////////////////////
//	Test step 015.11
/////////////////////

// constructor
CCommDbTest015_11::CCommDbTest015_11()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_11"));
	}

// destructor
CCommDbTest015_11::~CCommDbTest015_11()
	{
	}

TVerdict CCommDbTest015_11::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.11 called "));

	SetTestStepResult(EPass);	
			// Heap test for 015.02
	CCommDbTest015_02* step015_02 = new CCommDbTest015_02;
	CleanupStack::PushL(step015_02);
	doTestStepWithHeapFailureL( *step015_02, KErrNone);
	if ( step015_02->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step015_02);
	
	return TestStepResult();
	}
	
/////////////////////
//	Test step 015.12
/////////////////////

// constructor
CCommDbTest015_12::CCommDbTest015_12()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_12"));
	}

// destructor
CCommDbTest015_12::~CCommDbTest015_12()
	{
	}

TVerdict CCommDbTest015_12::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.12 called "));

	SetTestStepResult(EPass);	
		// Heap test for 015.03

	CCommDbTest015_03* step015_03 = new CCommDbTest015_03;
	CleanupStack::PushL(step015_03);
	doTestStepWithHeapFailureL( *step015_03, KErrNone);
	if ( step015_03->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step015_03);
	
	return TestStepResult();
	}	
	
/////////////////////
//	Test step 015.13
/////////////////////

// constructor
CCommDbTest015_13::CCommDbTest015_13()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_13"));
	}

// destructor
CCommDbTest015_13::~CCommDbTest015_13()
	{
	}

TVerdict CCommDbTest015_13::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.13 called "));

	SetTestStepResult(EPass);	
		// Heap test for 015.04
	copyTestDatabase( EDatabase_1 );
	CCommDbTest015_04* step015_04 = new CCommDbTest015_04;
	CleanupStack::PushL(step015_04);
	doTestStepWithHeapFailureL( *step015_04, KErrNone);
	if ( step015_04->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step015_04);
	
	return TestStepResult();
	}	
	
/////////////////////
//	Test step 015.14
/////////////////////

// constructor
CCommDbTest015_14::CCommDbTest015_14()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_14"));
	}

// destructor
CCommDbTest015_14::~CCommDbTest015_14()
	{
	}

TVerdict CCommDbTest015_14::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.14 called "));

	SetTestStepResult(EPass);	
		// Heap test for 015.07

	CCommDbTest015_07* step015_07 = new CCommDbTest015_07;
	CleanupStack::PushL(step015_07);
	doTestStepWithHeapFailureL( *step015_07, KErrNone);
	if ( step015_07->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step015_07);
	
	return TestStepResult();
	}	
	
/////////////////////
//	Test step 015.15
/////////////////////

// constructor
CCommDbTest015_15::CCommDbTest015_15()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_15"));
	}

// destructor
CCommDbTest015_15::~CCommDbTest015_15()
	{
	}

TVerdict CCommDbTest015_15::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.15 called "));

	SetTestStepResult(EPass);	
		// Heap test for 015.08

	CCommDbTest015_08* step015_08 = new CCommDbTest015_08;
	CleanupStack::PushL(step015_08);
	doTestStepWithHeapFailureL( *step015_08, KErrLocked);
	if ( step015_08->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step015_08);
	
	return TestStepResult();
	}	



/////////////////////
//	Test step 015.16
/////////////////////

// constructor
CCommDbTest015_16::CCommDbTest015_16()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_16"));
	}

// destructor
CCommDbTest015_16::~CCommDbTest015_16()
	{
	}

TVerdict CCommDbTest015_16::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.16 called "));

	SetTestStepResult(EPass);	
		// Heap test for 015.09
		
	CCommDbTest015_09* step015_09 = new CCommDbTest015_09;
	CleanupStack::PushL(step015_09);
	doTestStepWithHeapFailureL( *step015_09, KErrNone);
	if ( step015_09->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step015_09);
	
	return TestStepResult();
	}	
