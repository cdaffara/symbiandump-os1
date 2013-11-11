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
// This contains CommDb Unit Test Cases 017.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <e32property.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_017_xx.h"


 	#include <commsdattypesv1_1.h>
	using namespace CommsDat;


/////////////////////
//	Test step 017.01
/////////////////////

// constructor
CCommDbTest017_01::CCommDbTest017_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_01"));
	}

// destructor
CCommDbTest017_01::~CCommDbTest017_01()
	{
	}


TVerdict CCommDbTest017_01::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

#define TEST_VALUE	42

TInt CCommDbTest017_01::GlobalSettingTestL( const TDesC& aName )
	{
	TUint32 newValue = 0;
	TInt ret = KErrGeneral;

	//Perform a read/write test on the global
	iTheDb->SetGlobalSettingL( aName, TEST_VALUE );
	iTheDb->GetGlobalSettingL( aName, newValue );
	iTheDb->ClearGlobalSettingL( aName );

	//Should leave with KErrNotFound
	TRAPD(r, iTheDb->GetGlobalSettingL( aName, newValue ) );
	if ( r!= KErrNotFound )
		User::Leave(r);

	if ( newValue == TEST_VALUE  )
	{
		ret = KErrNone;
	}
	return ret;

}

TInt CCommDbTest017_01::GlobalSettingClearL( const TDesC& aName )
	{
	TUint32 newValue = 0;

	// Check for Global Setting existing. If exists then clear it
	TRAPD(ret,iTheDb->GetGlobalSettingL( aName, newValue ));
	if(ret==KErrNone)
		{
		iTheDb->ClearGlobalSettingL( aName );
		}
	else if(ret==KErrNotFound)
		{
		ret=KErrNone;
		}
	return ret;
	}		

TVerdict CCommDbTest017_01::doTestStepPostambleL()
	{
	TInt ret=KErrNone;
	
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(REDIAL_ATTEMPTS) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(SMS_BEARER) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(SMS_RECEIVE_MODE) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(GPRS_ATTACH_MODE) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(ACCEPT_INCOMING_GPRS) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(GPRS_CLASS_C_BEARER) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(CONNECTION_ATTEMPTS) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(MODEM_DATA_FAX) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(MODEM_PHONE_SERVICES_SMS) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(LOCATION_DATA_FAX) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(LOCATION_PHONE_SERVICES_SMS) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(MAXMBUF_HEAP) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(DEFAULT_AGENT) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingClearL(  TPtrC(DEFAULT_NETWORK) ), KErrNone );
	
	// Cleanup after test run
	CTestStepCommDb::doTestStepPostambleL();	
	if(ret == KErrNone)
		{
         SetTestStepResult(EPass);		return TestStepResult();
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TInt CCommDbTest017_01::executeStepL()
	{
	TInt ret=KErrNone;

	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(REDIAL_ATTEMPTS) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(SMS_BEARER) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(SMS_RECEIVE_MODE) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(GPRS_ATTACH_MODE) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(ACCEPT_INCOMING_GPRS) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(GPRS_CLASS_C_BEARER) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(CONNECTION_ATTEMPTS) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(MODEM_DATA_FAX) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(MODEM_PHONE_SERVICES_SMS) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(LOCATION_DATA_FAX ) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(LOCATION_PHONE_SERVICES_SMS)	 ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(MAXMBUF_HEAP) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(DEFAULT_AGENT) ), KErrNone );
	CDBLEAVE( ret = GlobalSettingTestL( TPtrC(DEFAULT_NETWORK) ), KErrNone );

	return ret;
	}



TVerdict CCommDbTest017_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.01 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 017.02
/////////////////////

// constructor
CCommDbTest017_02::CCommDbTest017_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_02"));
	}

// destructor
CCommDbTest017_02::~CCommDbTest017_02()
	{
	}


TVerdict CCommDbTest017_02::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}



TInt CCommDbTest017_02::executeStepL()
	{
	TInt ret=KErrNone;
	TUint32 id;

	//Checks for notifications when deleting global modem settings

	CMDBSession* ptrSession = CMDBSession::NewL(CMDBSession::LatestVersion());
	ptrSession->PublishProperties();
	delete ptrSession;
	ptrSession = NULL;

 	RProperty propCdbMdmRecordChange;
    RProperty propCdbMdmDataFaxChange;
    RProperty propCdbMdmNameChange;
    RProperty propCdbPhoneSMSChange;
 	
    TRequestStatus rqstatMdmRecordChange;
    TRequestStatus rqstatMdmDatafaxChange;
    TRequestStatus rqstatMdmNameChange;
    TRequestStatus rqstatPhoneSMSChange;
 	
    CDBLEAVE(propCdbMdmRecordChange.Attach(KUidSystemCategory,KUidCommDbModemRecordChange.iUid), KErrNone);
    CleanupClosePushL(propCdbMdmRecordChange);
 	CDBLEAVE(propCdbMdmDataFaxChange.Attach(KUidSystemCategory,KUidCommDbModemDataAndFaxChange.iUid), KErrNone);	
    CleanupClosePushL(propCdbMdmDataFaxChange);
 	CDBLEAVE(propCdbMdmNameChange.Attach(KUidSystemCategory,KUidCommDbModemTsyNameChange.iUid), KErrNone);
    CleanupClosePushL(propCdbMdmNameChange);
 	CDBLEAVE(propCdbPhoneSMSChange.Attach(KUidSystemCategory,KUidCommDbModemPhoneServicesAndSMSChange.iUid), KErrNone);
    CleanupClosePushL(propCdbPhoneSMSChange);
  

	//create two modem table entries
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );
	
	_LIT(KModem,"US Robotics Sportster");
	_LIT(KTsy,"HAYES");
	_LIT8(KModemInitString,"AT&F1");
	_LIT8(KModemFaxInitString,"AT&d2");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KModem);
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

	iTheDb->SetGlobalSettingL( TPtrC(MODEM_DATA_FAX), id );

	//Open a new view up
	delete iTheView;
	iTheView =0;
	iTheView = iTheDb->OpenTableLC( TPtrC(MODEM_BEARER) );
	CleanupStack::Pop();
	
	
	//Request the notifications
	
 	propCdbMdmRecordChange.Subscribe(rqstatMdmRecordChange);
 	__ASSERT_ALWAYS(rqstatMdmRecordChange == KRequestPending, User::Invariant());
 	propCdbMdmDataFaxChange.Subscribe(rqstatMdmDatafaxChange);
 	__ASSERT_ALWAYS(rqstatMdmDatafaxChange == KRequestPending, User::Invariant());
 	propCdbMdmNameChange.Subscribe(rqstatMdmNameChange);
 	__ASSERT_ALWAYS(rqstatMdmNameChange == KRequestPending, User::Invariant());

	//Delete the modem record and then wait for the notifications
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
 	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );

 	User::WaitForRequest(rqstatMdmRecordChange); 
 	if (rqstatMdmRecordChange != KErrNone) 
 		{
 		ret = KErrNotFound;
 		}
 	User::WaitForRequest(rqstatMdmDatafaxChange); 
 	if (rqstatMdmDatafaxChange != KErrNone) 
 		{
 		ret = KErrNotFound;
 		}
 	User::WaitForRequest(rqstatMdmNameChange); 
 	if (rqstatMdmNameChange != KErrNone) 
 		{
 		ret = KErrNotFound;
 		}
	
	//Now try the same thing with the MODEM_PHONE_SERVICES_SMS global setting
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );

	_LIT(KModem2,"Another US Robotics Sportster");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KModem2);
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
	
	iTheDb->SetGlobalSettingL(  TPtrC(MODEM_PHONE_SERVICES_SMS), id );


	//Open a new view up
	delete iTheView;
	iTheView =0;
	iTheView = iTheDb->OpenTableLC( TPtrC(MODEM_BEARER) );
	CleanupStack::Pop();
	
	//Reset notifications
 	propCdbMdmRecordChange.Subscribe(rqstatMdmRecordChange);
 	__ASSERT_ALWAYS(rqstatMdmRecordChange == KRequestPending, User::Invariant());
 	propCdbMdmNameChange.Subscribe(rqstatMdmNameChange);
 	__ASSERT_ALWAYS(rqstatMdmNameChange == KRequestPending, User::Invariant());
 	propCdbPhoneSMSChange.Subscribe(rqstatPhoneSMSChange);
 	__ASSERT_ALWAYS(rqstatPhoneSMSChange == KRequestPending, User::Invariant());

	//Delete the record and wait for the notifications
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );

 	User::WaitForRequest(rqstatMdmRecordChange); 
 	if (rqstatMdmRecordChange != KErrNone) 
 		{
 		ret = rqstatMdmRecordChange.Int();
 		}
 	User::WaitForRequest(rqstatPhoneSMSChange); 
 	if (rqstatPhoneSMSChange != KErrNone) 
 		{
 		ret = rqstatMdmRecordChange.Int();
 		}
  	User::WaitForRequest(rqstatMdmNameChange); 
 	if (rqstatMdmNameChange != KErrNone) 
 		{
 		ret = rqstatMdmRecordChange.Int();
 		}
 		
 	CleanupStack::PopAndDestroy(); // propCdbPhoneSMSChange
 	CleanupStack::PopAndDestroy(); // propCdbMdmNameChange
 	CleanupStack::PopAndDestroy(); // propCdbMdmDataFaxChange
 	CleanupStack::PopAndDestroy(); // propCdbMdmRecordChange
 		
	
	return ret;
	}



TVerdict CCommDbTest017_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.02 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 017.03
/////////////////////

// constructor
CCommDbTest017_03::CCommDbTest017_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_03"));
	}

// destructor
CCommDbTest017_03::~CCommDbTest017_03()
	{
	}

TVerdict CCommDbTest017_03::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest017_03::executeStepL()
	{
	TInt ret=KErrNone;
	TUint32 id;

	//Checks for notifications when deleting global modem settings

 	RProperty propCdbMdmRecordChange;
    RProperty propCdbMdmDataFaxChange;
    RProperty propCdbMdmNameChange;
    RProperty propCdbPhoneSMSChange;
 	
    TRequestStatus rqstatMdmRecordChange;
    TRequestStatus rqstatMdmDatafaxChange;
    TRequestStatus rqstatMdmNameChange;
    TRequestStatus rqstatPhoneSMSChange;
 	
    CDBLEAVE(propCdbMdmRecordChange.Attach(KUidSystemCategory, KUidCommDbModemRecordChange.iUid), KErrNone);
    CleanupClosePushL(propCdbMdmRecordChange);
 	CDBLEAVE(propCdbMdmDataFaxChange.Attach(KUidSystemCategory, KUidCommDbModemDataAndFaxChange.iUid), KErrNone);	//this KUid has been deprecated
    CleanupClosePushL(propCdbMdmDataFaxChange);
 	CDBLEAVE(propCdbMdmNameChange.Attach(KUidSystemCategory, KUidCommDbModemTsyNameChange.iUid), KErrNone);
    CleanupClosePushL(propCdbMdmNameChange);
 	CDBLEAVE(propCdbPhoneSMSChange.Attach(KUidSystemCategory, KUidCommDbModemPhoneServicesAndSMSChange.iUid), KErrNone);	//this KUid has been deprecated
    CleanupClosePushL(propCdbPhoneSMSChange);
 

	//create two modem table entries
	CDBLEAVE( iTheView->InsertRecord( id ), KErrNone );
	
	_LIT(KModem,"US Robotics Sportster");
	_LIT(KTsy,"HAYES");
	_LIT8(KModemInitString,"AT&F1");
	_LIT8(KModemFaxInitString,"AT&d2");
	//Write some sample data into the table
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),KModem);
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

	//Set both modem gloabls to the same record
	iTheDb->SetGlobalSettingL( TPtrC(MODEM_DATA_FAX), id );
	iTheDb->SetGlobalSettingL(  TPtrC(MODEM_PHONE_SERVICES_SMS), id );

	//Open a new view up
	delete iTheView;
	iTheView =0;
	iTheView = iTheDb->OpenTableLC( TPtrC(MODEM_BEARER) );
	CleanupStack::Pop();
	
	
	//Request the notifications
 	propCdbMdmRecordChange.Subscribe(rqstatMdmRecordChange);
 	__ASSERT_ALWAYS(rqstatMdmRecordChange == KRequestPending, User::Invariant());
 	propCdbMdmDataFaxChange.Subscribe(rqstatMdmDatafaxChange);
 	__ASSERT_ALWAYS(rqstatMdmDatafaxChange == KRequestPending, User::Invariant());
 	propCdbMdmNameChange.Subscribe(rqstatMdmNameChange);
 	__ASSERT_ALWAYS(rqstatMdmNameChange == KRequestPending, User::Invariant());
 	propCdbPhoneSMSChange.Subscribe(rqstatPhoneSMSChange);
 	__ASSERT_ALWAYS(rqstatPhoneSMSChange == KRequestPending, User::Invariant());

	//Delete the modem record and then wait for the four notifications
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );

 	User::WaitForRequest(rqstatMdmRecordChange); 
 	if (rqstatMdmRecordChange != KErrNone)
 		{
 		ret = KErrNotFound;
 		}
 	User::WaitForRequest(rqstatMdmDatafaxChange); 
 	if (rqstatMdmDatafaxChange != KErrNone) 
 		{
 		ret = KErrNotFound;
 		}
 	User::WaitForRequest(rqstatPhoneSMSChange); 
 	if (rqstatPhoneSMSChange != KErrNone) 
 		{
 		ret = KErrNotFound;
 		}
 		
 	User::WaitForRequest(rqstatMdmNameChange); 
 	if (rqstatMdmNameChange != KErrNone) 
 		{
 		ret = KErrNotFound;
 		}
 		
 	CleanupStack::PopAndDestroy(); // propCdbPhoneSMSChange
 	CleanupStack::PopAndDestroy(); // propCdbMdmNameChange
 	CleanupStack::PopAndDestroy(); // propCdbMdmDataFaxChange
 	CleanupStack::PopAndDestroy(); // propCdbMdmRecordChange
	
	return ret;
	}



TVerdict CCommDbTest017_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.03 called "));
	
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}



/////////////////////
//	Test step 017.04
/////////////////////

// constructor
CCommDbTest017_04::CCommDbTest017_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_04"));
	}

// destructor
CCommDbTest017_04::~CCommDbTest017_04()
	{
	}

TVerdict CCommDbTest017_04::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define INVALID_GLOBAL_NAME	  _L("Invalid")

TInt CCommDbTest017_04::executeStepL()
	{
	TRAPD( r, iTheDb->SetGlobalSettingL( TPtrC(INVALID_GLOBAL_NAME), 0x55 ) );
	if ( r!=KErrNotSupported )
		return r;

	TUint32 dummInt;
	TRAP( r, iTheDb->GetGlobalSettingL( TPtrC(INVALID_GLOBAL_NAME), dummInt ) );
	return r;
	}



TVerdict CCommDbTest017_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.04 called "));

	
	if ( executeStepL() == KErrNotSupported)
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 017.05
/////////////////////

// constructor
CCommDbTest017_05::CCommDbTest017_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_05"));
	}

// destructor
CCommDbTest017_05::~CCommDbTest017_05()
	{
	}

TVerdict CCommDbTest017_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.05 called "));

	SetTestStepResult(EPass);
	// Have backup of Database in c:\default1.dat
//	copyDatabaseToDefault (EDatabase_1);
	
		// Heap test for 017.01
		
//	copyTestDatabase( EDatabase_1 );
	CCommDbTest017_01* step017_01 = new(ELeave) CCommDbTest017_01;
	CleanupStack::PushL(step017_01);
	if ( doTestStepWithHeapFailureL( *step017_01, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step017_01);

		// Heap test for 017.02

//	copyTestDatabase( EDatabase_1 );
	CCommDbTest017_02* step017_02 = new CCommDbTest017_02;
	CleanupStack::PushL(step017_02);
	if ( doTestStepWithHeapFailureL( *step017_02, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step017_02);

		// Heap test for 017.03

//	copyTestDatabase( EDatabase_1 );
	CCommDbTest017_03* step017_03 = new CCommDbTest017_03;
	CleanupStack::PushL(step017_03);
	if ( doTestStepWithHeapFailureL( *step017_03, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step017_03);

		// Heap test for 017.04

//	copyTestDatabase( EDatabase_1 );
	CCommDbTest017_04* step017_04 = new CCommDbTest017_04;
	CleanupStack::PushL(step017_04);
	if ( doTestStepWithHeapFailureL( *step017_04, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step017_04);


	return TestStepResult();	

}

/////////////////////
//	Test step 017.06
/////////////////////

// constructor
CCommDbTest017_06::CCommDbTest017_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_06"));
	}

// destructor
CCommDbTest017_06::~CCommDbTest017_06()
	{
	}

TVerdict CCommDbTest017_06::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


#define DEFAULT_TSY	  _L("DEFAULT_TSY")

TInt CCommDbTest017_06::executeStepL()
	{
	TRAPD(r, iTheDb->SetGlobalSettingL(TPtrC(BEARER_AVAILABILITY_CHECK_TSY), TPtrC(DEFAULT_TSY)) );

	if (r != KErrNone)
		return r;

	TBuf<KCommsDbSvrMaxFieldLength> defaultTsy;
	TRAP(r, iTheDb->GetGlobalSettingL( TPtrC(BEARER_AVAILABILITY_CHECK_TSY), defaultTsy ) );

	if (r != KErrNone)
		{
		return r;
		}

	if (defaultTsy.Compare(TPtrC(DEFAULT_TSY)) != 0)
		{
		return KErrArgument;
		}

	defaultTsy.Zero();
	TRAP(r, iTheDb->GetDefaultTsyL(defaultTsy));

	if (r != KErrNone)
		{
		return r;
		}

	if (defaultTsy.Compare(TPtrC(DEFAULT_TSY)) != 0)
		{
		return KErrArgument;
		}
	return KErrNone;
	}


TVerdict CCommDbTest017_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.06 called "));

	
	if ( executeStepL() == KErrNone)
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 017.07
/////////////////////

// constructor
CCommDbTest017_07::CCommDbTest017_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_07"));
	}

// destructor
CCommDbTest017_07::~CCommDbTest017_07()
	{
	}

TVerdict CCommDbTest017_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.07 called "));

	SetTestStepResult(EPass);
		// Heap test for 017.06
		
//	copyTestDatabase( EDatabase_1 );
	CCommDbTest017_06* step017_06 = new(ELeave) CCommDbTest017_06;
	CleanupStack::PushL(step017_06);
	if ( doTestStepWithHeapFailureL( *step017_06, KErrNone) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step017_06);

	return TestStepResult();	

}

