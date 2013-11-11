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
// This contains CommDb Unit Test Cases 016.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <e32property.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_016_xx.h"

/////////////////////
//	Test step 016.01
/////////////////////

// constructor
CCommDbTest016_01::CCommDbTest016_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_01"));
	}

// destructor
CCommDbTest016_01::~CCommDbTest016_01()
	{
	}


TVerdict CCommDbTest016_01::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest016_01::executeStepL()
	{

	//Request a notification and then cancel it

	TRequestStatus status;

	User::LeaveIfError(iTheDb->RequestNotification( status ));
	iTheDb->CancelRequestNotification();
	User::WaitForRequest(status);



	if (KErrCancel == status.Int())
		{
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	}



TVerdict CCommDbTest016_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.01 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 016.02
/////////////////////

// constructor
CCommDbTest016_02::CCommDbTest016_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_02"));
	}

// destructor
CCommDbTest016_02::~CCommDbTest016_02()
	{
	}

TVerdict CCommDbTest016_02::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest016_02::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Generate an ECommit notification

	//Request notifications
	TRequestStatus event;
	iTheDb->RequestNotification(event);
	
	CDBLEAVE(iTheDb->BeginTransaction(), KErrNone);

	//Insert a record
	TUint32 id;
	CDBLEAVE(iTheView->InsertRecord( id ), KErrNone);
	//Write the record and commit it
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),_L("My Service"));
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	CDBLEAVE(iTheView->PutRecordChanges(), KErrNone);
	CDBLEAVE(iTheDb->CommitTransaction(), KErrNone);

	//Now wait for the notification event
	User::WaitForRequest(event);

	if ( event.Int() == RDbNotifier::ECommit )
		ret = KErrNone;
	return ret;
	}



TVerdict CCommDbTest016_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.02 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 016.04
/////////////////////

// constructor
CCommDbTest016_04::CCommDbTest016_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_04"));
	}

// destructor
CCommDbTest016_04::~CCommDbTest016_04()
	{
	}

TVerdict CCommDbTest016_04::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest016_04::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Generate an EUnlock notification	
	
	TRequestStatus event(KRequestPending);
	iTheDb->RequestNotification(event);
	if(event!=KRequestPending)
		User::LeaveIfError(event.Int());
	
	iTheDb->SetGlobalSettingL(TPtrC(REDIAL_ATTEMPTS), 5 );
	
	
	//EUnlock is returned as a result of a read transaction...Calling CommitTransaction without changing any value will also trigger EUnLock
	TUint32 newvalue = 0;
	iTheDb->GetGlobalSettingL(TPtrC(REDIAL_ATTEMPTS), newvalue);

	User::WaitForRequest(event);
	
	INFO_PRINTF2(_L("Request for notification returned %d"),event.Int());
	
	if ( event.Int() == RDbNotifier::EUnlock )
		ret = KErrNone;
	
    //CleanupStack::PopAndDestroy(sched);
    
	return ret;
	}



TVerdict CCommDbTest016_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.04 called "));

	
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 016.05
/////////////////////

// constructor
CCommDbTest016_05::CCommDbTest016_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_05"));
	// default to no local objects allocated
	iLocalObjects = EFalse;
	}

// destructor
CCommDbTest016_05::~CCommDbTest016_05()
	{
	}


TVerdict CCommDbTest016_05::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest016_05::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Generate an ERollback notification


	TRequestStatus event;
	iTheDb->RequestNotification(event);

	iTheDb->BeginTransaction();
	//Insert a record
	TUint32 id;
	CDBLEAVE(iTheView->InsertRecord( id ), KErrNone);
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),_L("CCommDbTest016_05 Test record"));
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	CDBLEAVE(iTheView->PutRecordChanges(), KErrNone);
	
	iTheDb->RollbackTransaction();

	//Wait for the notify event
	User::WaitForRequest(event);
	
	if ( event.Int() == RDbNotifier::ERollback )
		ret = KErrNone;
	return ret;
	}



TVerdict CCommDbTest016_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.05 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 016.07
/////////////////////

// constructor
CCommDbTest016_07::CCommDbTest016_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_07"));
	}

// destructor
CCommDbTest016_07::~CCommDbTest016_07()
	{
	}


TVerdict CCommDbTest016_07::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest016_07::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Cancel an active notification request
	
	TRequestStatus notifyStatus;
	iTheDb->RequestNotification(notifyStatus);

	iTheDb->BeginTransaction();
	//Insert a record
	TUint32 id;
	CDBLEAVE(iTheView->InsertRecord( id ), KErrNone);
	iTheView->WriteTextL(TPtrC(COMMDB_NAME),_L("CCommDbTest016_05 Test record"));
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);

	CDBLEAVE(iTheView->PutRecordChanges(), KErrNone);

	iTheDb->CancelRequestNotification();

	iTheDb->CommitTransaction();

	//Wait for the notify
	User::WaitForRequest( notifyStatus );
	
	if ( notifyStatus.Int() == KErrCancel )
		{
		//Notify request has been cancelled
		ret = KErrNone;
		}

	return ret;
	}



TVerdict CCommDbTest016_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.07 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 016.08
/////////////////////

// constructor
CCommDbTest016_08::CCommDbTest016_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_08"));
	}

// destructor
CCommDbTest016_08::~CCommDbTest016_08()
	{
	}


TVerdict CCommDbTest016_08::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(MODEM_BEARER));
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest016_08::executeStepL()
	{
	TInt ret=KErrGeneral;
	TUint32 id;

	//Check that notifications are being sent to the Publish&Subscribe
	//when the Modem table is modified

 	//set up the TSY name change event
 	RProperty propCdbModemNameChange;
 	TRequestStatus rqstatMdmNameChange;
 	CDBLEAVE(propCdbModemNameChange.Attach(KUidSystemCategory,KUidCommDbModemTsyNameChange.iUid), KErrNone);
 	CleanupClosePushL(propCdbModemNameChange);
 	propCdbModemNameChange.Subscribe(rqstatMdmNameChange);
 	__ASSERT_ALWAYS(rqstatMdmNameChange == KRequestPending, User::Invariant());
 	
 	//set up the Modem table change event
 	RProperty propCdbModemRecordChange;
 	TRequestStatus rqstatMdmRecordChange;
 	CDBLEAVE(propCdbModemRecordChange.Attach(KUidSystemCategory,KUidCommDbModemRecordChange.iUid), KErrNone);
 	CleanupClosePushL(propCdbModemRecordChange);
 	propCdbModemRecordChange.Subscribe(rqstatMdmRecordChange);
 	__ASSERT_ALWAYS(rqstatMdmRecordChange == KRequestPending, User::Invariant()); 	
 	
 	

	//Make sure there is a record in the modem table	
	CDBLEAVE( iTheView->InsertRecord( id ),  KErrNone );

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

	CDBLEAVE( iTheView->PutRecordChanges(),  KErrNone );

	delete iTheView;
	iTheView = 0;

	
	iTheView = iTheDb->OpenTableLC( TPtrC(MODEM_BEARER));
	CleanupStack::Pop();


	//Now delete the modem record and check for the notifications
	CDBLEAVE( iTheView->GotoFirstRecord(),  KErrNone );
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );
	
 	User::WaitForRequest(rqstatMdmNameChange); 
 	User::WaitForRequest(rqstatMdmRecordChange); 
 	if (rqstatMdmNameChange == KErrNone &&
 	  rqstatMdmRecordChange == KErrNone )
 		{
        ret = KErrNone;
 		}
	CleanupStack::PopAndDestroy(); // rqstatMdmRecordChange
	CleanupStack::PopAndDestroy(); // rqstatMdmNameChange

	return ret;
	}



TVerdict CCommDbTest016_08::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.08 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 016.09
/////////////////////

// constructor
CCommDbTest016_09::CCommDbTest016_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_09"));
	}

// destructor
CCommDbTest016_09::~CCommDbTest016_09()
	{
	}

TVerdict CCommDbTest016_09::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.09 called "));

	SetTestStepResult(EPass);
		// Heap test for 016.01
    //Commented out because it causes a DBMS server crash
	//CCommDbTest016_01* step016_01 = new(ELeave) CCommDbTest016_01;
	//CleanupStack::PushL(step016_01);
	//doTestStepWithHeapFailureL( *step016_01, KErrNone);
	//if ( step016_01->TestStepResult() == EFail )
				//SetTestStepResult(EFail);	//CleanupStack::PopAndDestroy(step016_01);

		// Heap test for 016.02

	//Commented out because it causes a DBMS server crash
	//CCommDbTest016_02* step016_02 = new CCommDbTest016_02;
	//CleanupStack::PushL(step016_02);
	//doTestStepWithHeapFailureL( *step016_02, KErrNone);
	//if ( step016_02->TestStepResult() == EFail )
				//SetTestStepResult(EFail);	//CleanupStack::PopAndDestroy(step016_02);


		// Heap test for 016.04
    //Commented out because it causes a DBMS server crash
	//CCommDbTest016_04* step016_04 = new(ELeave) CCommDbTest016_04;
	//CleanupStack::PushL(step016_04);
	//doTestStepWithHeapFailureL( *step016_04, KErrNone);
	//if ( step016_04->TestStepResult() == EFail )
				//SetTestStepResult(EFail);	//CleanupStack::PopAndDestroy(step016_04);


		// Heap test for 016.05

	//Commented out because it causes a DBMS server crash
	//CCommDbTest016_05* step016_05 = new(ELeave) CCommDbTest016_05;
	//CleanupStack::PushL(step016_05);
	//doTestStepWithHeapFailureL( *step016_05, KErrNone);
	//if ( step016_05->TestStepResult() == EFail )
				//SetTestStepResult(EFail);	//CleanupStack::PopAndDestroy(step016_05);

		// Heap test for 016.07

	//Commented out because it causes a DBMS server crash
	//CCommDbTest016_07* step016_07 = new(ELeave) CCommDbTest016_07;
	//CleanupStack::PushL(step016_07);
	//doTestStepWithHeapFailureL( *step016_07, KErrGeneral);
	//if ( step016_07->TestStepResult() == EFail )
				//SetTestStepResult(EFail);	//CleanupStack::PopAndDestroy(step016_07);

	// Heap test for 016.08

	//Commented out as it causes a 'Hang'
	//CCommDbTest016_08* step016_08 = new(ELeave) CCommDbTest016_08;
	//CleanupStack::PushL(step016_08);
	//doTestStepWithHeapFailureL( *step016_08, KErrNone);
	//if ( step016_08->TestStepResult() == EFail )
				//SetTestStepResult(EFail);	//CleanupStack::PopAndDestroy(step016_08);

	// These OOM tests have a bad habit of generating stray signals as they are intended to fail in the middle.
	// Tidy everything up or scheduletest will set our status to "fail" :-(
	while (RThread().RequestCount() > 0)
		User::WaitForAnyRequest();

	return TestStepResult();	

}

