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
// This contains CommDb Unit Test Cases 062.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbtemp.h>
#include <cdbover.h>


// Test system includes
#include "Teststepcommdb.h"
#include "Step_062_xx.h"

/////////////////////
//	Test step 062.01
/////////////////////

// constructor
CCommDbTest062_01::CCommDbTest062_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_062_01"));
	}

// destructor
CCommDbTest062_01::~CCommDbTest062_01()
	{
	}

TVerdict CCommDbTest062_01::doTestStepPreambleL()
	{	
	openDbAndViewL(_L("Modem"));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest062_01::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	
	//Insert a new record, 

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
	iTheView = iTheDb->OpenViewMatchingTextLC( _L("Modem"), TPtrC(COMMDB_NAME), KModem);
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
	iTheView = iTheDb->OpenViewMatchingTextLC( _L("Modem"), TPtrC(COMMDB_NAME), KModem);
	CleanupStack::Pop();

	CDBLEAVE( ret = iTheView->GotoFirstRecord(), KErrNone );

	return ret;
	}



TVerdict CCommDbTest062_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 062.01 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 062.02
/////////////////////

// constructor
CCommDbTest062_02::CCommDbTest062_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_062_02"));
	}

// destructor
CCommDbTest062_02::~CCommDbTest062_02()
	{
	}

TVerdict CCommDbTest062_02::doTestStepPreambleL()
	{	
	openDbL();
//	TInt ret = copyTestDatabase( EDatabase_1 );
//	if(ret != KErrNone) // can't check this at the moment because copyTestDatabase returns 
//	   User::Leave(ret);// KErrGeneral when it passes!!
        SetTestStepResult(EPass);	return TestStepResult();
	}



TInt CCommDbTest062_02::executeStepL()
	{
	TInt ret=KErrGeneral;
	CCommsDbTableView* table = NULL;

	// Not much to this - in fact it's the specific case raised in the defect report
	// INC027976 which describes the BC break due to chenging the name of the Modem table 
	// to ModemBearer
	
	__UHEAP_MARK;
	table = iTheDb->OpenViewLC(_L("Modem"),_L("select * from Modem where Hidden=0")); 
	CleanupStack::Pop();

	if(table != NULL)
	{
		ret = KErrNone;
		delete table;
		table = 0;
	}

	__UHEAP_MARKEND;
	return ret;
	}



TVerdict CCommDbTest062_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 062.02 called "));

	TRAPD(ret, executeStepL());
	if( ret == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

// Test step 03 is the same as 37_01, except that the literal string 'Modem'
// is used to verify that template handling is BC.



CCommDbTest062_03::CCommDbTest062_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_062_03"));
	}

CCommDbTest062_03::~CCommDbTest062_03()
	{}

TVerdict CCommDbTest062_03::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest062_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest062_03::executeStepL()
	{
	//alter something in the template
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, _L("Modem"));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("DummyName"));
	TBufC8<KCommsDbSvrMaxFieldLength> firstInputString = _L8("first string");

	templateRecord->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING), firstInputString);
	User::LeaveIfError(templateRecord->StoreModifications());
	//Now start to overwrite initial change, but don't commit, cancel instead.
	User::LeaveIfError(templateRecord->Modify());

	TBufC8<KCommsDbSvrMaxFieldLength> secondInputString = _L8("second string");
	templateRecord->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING), secondInputString);
	templateRecord->CancelModifications();

	TBuf8<KCommsDbSvrMaxFieldLength> retrievedString;
	templateRecord->ReadTextL(TPtrC(MODEM_FAX_INIT_STRING), retrievedString);
	CleanupStack::PopAndDestroy(templateRecord);

	if(retrievedString!=firstInputString)
		return KErrGeneral;
	
	return KErrNone;
	}
	
TVerdict CCommDbTest062_03::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

// Test steps 04 & 05 are the same as 41_13 & 41_14, except that the literal string
// 'Modem' is used to access the table to verify that the override handling is BC.
/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest062_04::CCommDbTest062_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_062_04"));
	}

CCommDbTest062_04::~CCommDbTest062_04()
	{}

TVerdict CCommDbTest062_04::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest062_04::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest062_04::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TBuf<KCommsDbSvrMaxFieldLength> temp(_L("There's no place like home"));
	return iOverSet->SetDesOverride(_L("Modem"), TPtrC(MODEM_TSY_NAME), temp);
	}

TVerdict CCommDbTest062_04::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest062_05::CCommDbTest062_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_062_05"));
	}

CCommDbTest062_05::~CCommDbTest062_05()
	{}

TVerdict CCommDbTest062_05::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotSupported)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest062_05::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest062_05::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	
	return iOverSet->SetIntOverride(_L("Modem"), TPtrC(MODEM_STOP_BITS), 10);
	}

TVerdict CCommDbTest062_05::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}
