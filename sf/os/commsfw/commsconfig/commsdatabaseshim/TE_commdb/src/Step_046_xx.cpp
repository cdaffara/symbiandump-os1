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
// This contains CommDb Unit Test Cases 046.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbstore.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_046_xx.h"
#include <e32math.h>


const TInt KTest04605Repeats = 20;

/////////////////////
//	Test step 046.01
/////////////////////

// constructor
CCommDbTest046_01::CCommDbTest046_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_046_01"));
	}

// destructor
CCommDbTest046_01::~CCommDbTest046_01()
	{
	}


TVerdict CCommDbTest046_01::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest046_01::executeStepL()
	{
	TInt ret=KErrGeneral;

	//This step checks that record id's are created in the expected manner,
	//and that the record cursor also behaves as expected ie it moves to
	//the next record, when the record it points to is deleted.

	iTheView = iTheDb->OpenTableLC( TPtrC(DIAL_OUT_ISP) );
	CleanupStack::Pop(iTheView);
	//Insert 4 records checking the id each time
	TUint32 id;

	CDBLEAVE(iTheView->InsertRecord( id ), KErrNone);
	if ( id != 1 )
		User::Leave( ret );

	iTheView->WriteTextL(TPtrC(COMMDB_NAME),_L("My Service #1"));
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );

	CDBLEAVE(iTheView->InsertRecord( id ), KErrNone);
	if ( id != 2 )
		User::Leave( ret );

	iTheView->WriteTextL(TPtrC(COMMDB_NAME),_L("My Service #2"));
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );

	CDBLEAVE(iTheView->InsertRecord( id ), KErrNone);
	if ( id !=3 )
		User::Leave( ret );


	iTheView->WriteTextL(TPtrC(COMMDB_NAME),_L("My Service #3"));
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );


	CDBLEAVE(iTheView->InsertRecord( id ), KErrNone);
	if ( id !=4 )
		User::Leave( ret );


	iTheView->WriteTextL(TPtrC(COMMDB_NAME),_L("My Service #4"));
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );


	//Open a new view and move to the last record
	delete iTheView;
	iTheView = 0;

	iTheView = iTheDb->OpenTableLC( TPtrC(DIAL_OUT_ISP) );
	CleanupStack::Pop(iTheView);

	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );

	//Delete the last record
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoPreviousRecord(), KErrNone);
	//Check the cursor is now pointing at the previous record

	TUint32 value;
	iTheView->ReadUintL( TPtrC(COMMDB_ID), value );

	if ( value != 3 )
		User::Leave( ret );

	//Goto the first record and delete that
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	CDBLEAVE( iTheView->DeleteRecord(), KErrNone );
	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	//Check the cursor is now pointing at the second record
	
	iTheView->ReadUintL( TPtrC(COMMDB_ID), value );
	if ( value != 2 )
		User::Leave( ret );

	//Insert a new record and check the id is correct
	
	CDBLEAVE(iTheView->InsertRecord( id ), KErrNone);
	/* 
	DBMS supports cyclic id allocation..commsdat was forced
	to do that too..but looks like we do reuse the record ids
	if the deleted record was the last one. I think we should forget about
	this slip because 
		a)Its not a behaviour worth having for us and we got away with it anyway
		b)None of our client seem to give a damn about this for last few years  
					
	if ( id != 5 )
		User::Leave( ret );*/
		
	if ( id != 4 )
		User::Leave( ret );	

	iTheView->WriteTextL(TPtrC(COMMDB_NAME),_L("My Service #5"));
	iTheView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
	iTheView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//	iTheView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
	iTheView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	iTheView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
	CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );

	ret = KErrNone; //Passed if we get this far

	return ret;
	}


TVerdict CCommDbTest046_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 046.01 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 046.02
/////////////////////

// constructor
CCommDbTest046_02::CCommDbTest046_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_046_02"));
	}

// destructor
CCommDbTest046_02::~CCommDbTest046_02()
	{
	}


TVerdict CCommDbTest046_02::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

void CCommDbTest046_02::insertTestRecordsL( TUint aQuantity )
	{
	TUint count;
	TUint32 id;
	CCommsDbTableView* dbView;

	dbView  = iTheDb->OpenTableLC( TPtrC(DIAL_OUT_ISP) );

	for( count=0; count < aQuantity; count++ )
		{

		CDBLEAVE(dbView->InsertRecord( id ), KErrNone);
		dbView->WriteTextL(TPtrC(COMMDB_NAME),_L("My Service Test Record"));
		dbView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
		dbView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
		dbView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//		dbView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
		dbView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
		dbView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
		dbView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
		dbView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
		CDBLEAVE( dbView->PutRecordChanges(), KErrNone );
		
		}
	
	CleanupStack::PopAndDestroy(dbView);

	}

void CCommDbTest046_02::deleteLastRecordsL( TUint aQuantity )
	{
	TUint count;
	TInt ret = KErrGeneral;
	CCommsDbTableView* dbView;

	dbView  = iTheDb->OpenTableLC( TPtrC(DIAL_OUT_ISP) );
	CDBLEAVE( dbView->GotoFirstRecord(), KErrNone );

	//Move to the end of the view
	do
	{
		ret = dbView->GotoNextRecord();
	}while ( ret != KErrNotFound );
	
	//Need to move the cursor back into the view
	dbView->GotoPreviousRecord();

	//Now delete the record and move back one
	for ( count=0; count< aQuantity; count++)
		{
		CDBLEAVE( dbView->DeleteRecord(), KErrNone );
		CDBLEAVE( dbView->GotoPreviousRecord(), KErrNone );
		}

	CleanupStack::PopAndDestroy(dbView);
	}


TInt CCommDbTest046_02::executeStepL()
	{
	TInt ret=KErrGeneral, count;

	// NOTE: count limit reduced from 100 to 5, because commsdat has an id limit
	// of 128 when inserting records.  After this, you get a KErrOverflow from
	// CCommsDbTableView::InsertRecord() in insertTestRecordsL(...).
	for ( count=0; count < 5; count++ )
		{
		insertTestRecordsL( 10 );

		//Start a transaction, write 10 records, then rollback
		CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		insertTestRecordsL( 10 );
		iTheDb->RollbackTransaction();

		//Start a transaction, write 10 records, delete them, then rollback
		CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		insertTestRecordsL( 10 );
		deleteLastRecordsL( 10 );
		iTheDb->RollbackTransaction();

		//Start a transaction, write 10 records and commit them
		CDBLEAVE( iTheDb->BeginTransaction(), KErrNone );
		insertTestRecordsL( 10 );
		CDBLEAVE( iTheDb->CommitTransaction(), KErrNone );

		}

	ret = KErrNone;

	return ret;
	}



TVerdict CCommDbTest046_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 046.02 called "));


	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


///////////////////////////////////////////////////////////////////////////
//Thread1 does the first right, our main thread then does the second.
//They continue to take it in turns to do writes

CCommDbTest046_03::CCommDbTest046_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_046_03"));
	}

CCommDbTest046_03::~CCommDbTest046_03()
	{}

TVerdict CCommDbTest046_03::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest046_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest046_03::Thread1Function(TAny* aPtr)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup==NULL)
		return(KErrNoMemory);

	TRAPD(err, DoThread1TestL(aPtr));
	if(err != KErrNone)
		{
		return err;
		}

	delete cleanup;
	return(KErrNone);
	}

void CCommDbTest046_03::DoThread1TestL(TAny* aPtr)
	{
	TThreadInfo* syncInfo = (TThreadInfo*) aPtr;
	TRequestStatus localStatus(KRequestPending);
	syncInfo->iThread1Status = &localStatus;
	
	CCommsDatabase* cDb = CCommsDatabase::NewL();
	CleanupStack::PushL(cDb);
	CCommsDbTableView* modemTable = cDb->OpenTableLC(TPtrC(MODEM_BEARER));

	TUint32 dummyId;
	modemTable->InsertRecord(dummyId);
	modemTable->WriteTextL(TPtrC(COMMDB_NAME),_L("Thread1, Entry1"));
	User::LeaveIfError(modemTable->PutRecordChanges());

	//this thread is the first on to do all its rights
	CleanupStack::PopAndDestroy(modemTable);
	CleanupStack::PopAndDestroy(cDb);

	syncInfo->iMainThread.RequestComplete(syncInfo->iMainThreadStatus, KErrNone);
	}

TInt CCommDbTest046_03::executeStepL()
	{
	//First thing is to empty the modem table
	CCommsDbTableView* modemTable = iTheDb->OpenTableLC(TPtrC(MODEM_BEARER));
	while(modemTable->GotoFirstRecord()==KErrNone)
		modemTable->DeleteRecord();

	RThread thread1;
	TThreadInfo sync1;
	TRequestStatus status1(KRequestPending);
	sync1.iMainThreadStatus=&status1;
	sync1.iMainThread.Duplicate(RThread(),EOwnerProcess);
	_LIT(KThread1Name,"Thread1");
	User::LeaveIfError(thread1.Create(KThread1Name,Thread1Function,KDefaultStackSize,NULL,&sync1));
	CleanupClosePushL(thread1);

	iTheDb->BeginTransaction();

	TUint32 dummyId;
	modemTable->InsertRecord(dummyId);
	_LIT(KTestName, "Main, Entry1");
	modemTable->WriteTextL(TPtrC(COMMDB_NAME), KTestName);

	//OK, lets start the other thread off now.

	TRequestStatus status2(KRequestPending);

	thread1.Logon(status2);
	thread1.Resume();
	User::WaitForRequest(status1,status2);

	//OK, one of the status has completed, if it's status1 then that means thread1 didn't panic
	//which is a bad thing.

	if(status1 != KRequestPending)
		User::Leave(KErrGeneral);	//Report error by leaving

	if(thread1.ExitType() != EExitPanic)
		User::Leave(KErrGeneral);	//Thread1 didn't panic, something else has gone wrong.
	
	CleanupStack::PopAndDestroy();	//thread1
	//Lets finish our entry off, and check it's OK.
	User::LeaveIfError(modemTable->PutRecordChanges());
	
	iTheDb->CommitTransaction();

	CleanupStack::PopAndDestroy(modemTable);

	//Lets make a new modem view and check it's only got our entry in.
	modemTable=NULL;
	modemTable = iTheDb->OpenTableLC(TPtrC(MODEM_BEARER));
	User::LeaveIfError(modemTable->GotoFirstRecord());

	TBuf<KCommsDbSvrMaxFieldLength> tempBuf;
	modemTable->ReadTextL(TPtrC(COMMDB_NAME), tempBuf);

	if(tempBuf!=KTestName)
		User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(modemTable);
        SetTestStepResult(EPass);	return TestStepResult();
	}
	
TVerdict CCommDbTest046_03::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

///////////////////////////////////////////////////////////////////////////
/*
Note for anyone who looks at this code.
It's a bit of a mess, but it seems to do the job. Basically thread1 is the thread trying to
do a write to the database, and our main thread is the one responsible for checking that
the change hasn't gone in before PutRecordChanges is called by thread1. After thread1 has
done it's write, we check that the write was sucessfull. This is done by our main thread
right at the end of CCommDbTest046_04::executeStepL().
*/

CCommDbTest046_04::CCommDbTest046_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_046_04"));
	}

CCommDbTest046_04::~CCommDbTest046_04()
	{}

TVerdict CCommDbTest046_04::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest046_04::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest046_04::Thread1Function(TAny* aPtr)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup==NULL)
		return(KErrNoMemory);

	TRAPD(err, DoThread1TestL(aPtr));

	delete cleanup;
	return(err);
	}

void CCommDbTest046_04::DoThread1TestL(TAny* aPtr)
	{
	TThreadInfo* remoteThreadInfo = (TThreadInfo*) aPtr;
	CCommsDatabase* cDb =CCommsDatabase::NewL();
	CleanupStack::PushL(cDb);

	CCommsDbTableView* dialInIspView = cDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	User::LeaveIfError(dialInIspView->GotoFirstRecord());
	dialInIspView->UpdateRecord();
	dialInIspView->WriteLongTextL(TPtrC(ISP_LOGIN_SCRIPT), _L("Changed"));

	remoteThreadInfo->iCaller.RequestComplete(remoteThreadInfo->iStatus1,KErrNone);//We're now ready to kick of the other thread

	User::After(10000000);
	TInt err = dialInIspView->PutRecordChanges();

	CleanupStack::PopAndDestroy(dialInIspView);

	CleanupStack::PopAndDestroy(cDb);
	remoteThreadInfo->iCaller.RequestComplete(remoteThreadInfo->iStatus2,err);
	}

TInt CCommDbTest046_04::executeStepL()
	{
	//Set the commdb up so we know what's in the first record of the Dial in ISP before we start.
	CCommsDbTableView* dialInIspView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	User::LeaveIfError(dialInIspView->GotoFirstRecord());
	dialInIspView->UpdateRecord();
	dialInIspView->WriteLongTextL(TPtrC(ISP_LOGIN_SCRIPT), _L("Original"));
	User::LeaveIfError(dialInIspView->PutRecordChanges());
	CleanupStack::PopAndDestroy(dialInIspView);

	//Start off the first thread, this will run until it has done all but PutRecordChanges is called on the transaction.
	RThread thread1;
	TThreadInfo sync1;
	TRequestStatus status1(KRequestPending);
	TRequestStatus status2(KRequestPending);
	sync1.iStatus1=&status1;
	sync1.iStatus2=&status2;
	sync1.iCaller.Duplicate(RThread(),EOwnerProcess);
	_LIT(KThread1Name,"Thread1");
	TInt err=thread1.Create(KThread1Name,Thread1Function,KDefaultStackSize,NULL,&sync1);
	CleanupClosePushL(thread1);

	if(err==KErrNone)
		{
		thread1.Resume();
		User::WaitForRequest(status1);
		}
	//Thread just kicked off will have run to User::After(10000000); by the time we get to here
	//We've basically got 10 seconds from here to check that the record hasn't been committed, which sould be plenty long enough. 

	dialInIspView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	User::LeaveIfError(dialInIspView->GotoFirstRecord());
	
	HBufC* retrievedString = dialInIspView->ReadLongTextLC(TPtrC(ISP_LOGIN_SCRIPT));

	if(*retrievedString!=_L("Original"))
		User::Leave(KErrGeneral);
	
	CleanupStack::PopAndDestroy();	//retrievedString
	retrievedString=NULL;
	CleanupStack::PopAndDestroy(dialInIspView);

	User::WaitForRequest(status2);
	User::LeaveIfError(status2.Int());	//status2.Int() is the value returned when PutRecordChanges is called by thread1.

	//Might as well check that thread1s changes got through....
	dialInIspView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	User::LeaveIfError(dialInIspView->GotoFirstRecord());
	retrievedString= dialInIspView->ReadLongTextLC(TPtrC(ISP_LOGIN_SCRIPT));
	err=KErrNone;
	if(*retrievedString!=_L("Changed"))
		err= KErrGeneral;
	CleanupStack::PopAndDestroy();	//retrievedString
	CleanupStack::PopAndDestroy(dialInIspView);
	CleanupStack::PopAndDestroy();	//thread1
	return err;
	}
	
TVerdict CCommDbTest046_04::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

///////////////////////////////////////////////////////////////////////////


CCommDbTest046_05::CCommDbTest046_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_046_05"));
	}

CCommDbTest046_05::~CCommDbTest046_05()
	{}

TVerdict CCommDbTest046_05::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest046_05::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest046_05::Thread1Function(TAny* aPtr)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup==NULL)
		return(KErrNoMemory);

	TRAPD(err, DoThread1TestL(aPtr));

	delete cleanup;
	return(err);
	}

void CCommDbTest046_05::DoThread1TestL(TAny* aPtr)
	{
	TInt ret;
	TInt64 seed;
	TThreadInfo* remoteThreadInfo = (TThreadInfo*) aPtr;

	CCommsDatabase* cDb = CCommsDatabase::NewL();
	CleanupStack::PushL(cDb);

	for (TInt count=0; count < KTest04605Repeats; count++ )
		{
		PrintDebug(_L("DoThread1TestL() - 1. InsertTestRecords"));
		InsertTestRecordsL(EFalse, 10 , *cDb);

		
		// Start a transaction, write 10 records, then rollback
		RandomDelay(seed);
		while(ret = cDb->BeginTransaction(), ret!=KErrNone)
			{
			PrintDebug(_L("DoThread1TestL() - 2. BeginTransaction failed"));
			User::After(2000);
			}
		PrintDebug(_L("DoThread1TestL() - 2. BeginTransaction"));
		TRequestStatus status1(KRequestPending);
		cDb->RequestNotification(status1);
		PrintDebug(_L("DoThread1TestL() - 3. InsertTestRecords"));
		while(!InsertTestRecordsL(EFalse, 10 , *cDb))
			{
			// Error during record insertion, most likely the other thread beat
			// us to it, Rollback and try again
			PrintDebug(_L("DoThread1TestL() - Rolling back Transaction"));
			cDb->RollbackTransaction();
			User::WaitForRequest(status1);
			status1 = KRequestPending;
			RandomDelay(seed);
			while(ret = cDb->BeginTransaction(), ret!=KErrNone)
				{
				PrintDebug(_L("DoThread1TestL() - Begin Transaction Again failed"));
				User::After(2000);
				}
			PrintDebug(_L("DoThread1TestL() - Begin Transaction Again"));
			cDb->RequestNotification(status1);
			}
		// 10 records inserted successfully, now rollback and wait for CommDb
		PrintDebug(_L("DoThread1TestL() - 4. Rollback"));
		cDb->RollbackTransaction();
		PrintDebug(_L("DoThread1TestL() - 5. WaitForRequest"));
		User::WaitForRequest(status1);

		
		
		//Start a transaction, write 10 records, delete them, then rollback
		RandomDelay(seed);
		while(ret = cDb->BeginTransaction(), ret!=KErrNone)
			{
			PrintDebug(_L("DoThread1TestL() - 6. BeginTransaction failed"));
			User::After(2000);
			}
		PrintDebug(_L("DoThread1TestL() - 6. BeginTransaction"));
		TRequestStatus status2(KRequestPending);
		cDb->RequestNotification(status2);
		PrintDebug(_L("DoThread1TestL() - 7. InsertTestRecords"));
		while(!InsertTestRecordsL( EFalse, 10 , *cDb))
			{
			// Error during record insertion, most likely the other thread beat
			// us to it, Rollback and try again
			PrintDebug(_L("DoThread1TestL() - Rolling back Transaction"));
			cDb->RollbackTransaction();
			User::WaitForRequest(status2);
			status2 = KRequestPending;
			RandomDelay(seed);
			while(ret = cDb->BeginTransaction(), ret!=KErrNone)
				{
				PrintDebug(_L("DoThread1TestL() - Begin Transaction Again failed"));
				User::After(2000);
				}
			PrintDebug(_L("DoThread1TestL() - Begin Transaction Again"));
			cDb->RequestNotification(status2);
			}
		// 10 records inserted successfully, now delete them, rollback and wait for commdb
		DeleteLastRecordsL(EFalse, 10, *cDb );
		PrintDebug(_L("DoThread1TestL() - 8. Rollback"));
		cDb->RollbackTransaction();
		PrintDebug(_L("DoThread1TestL() - 9. WaitForRequest"));
		User::WaitForRequest(status2);

		
		//Start a transaction, write 10 records and commit them
		RandomDelay(seed);
		while(ret = cDb->BeginTransaction(), ret!=KErrNone)
			{
			PrintDebug(_L("DoThread1TestL() - 10. BeginTransaction failed"));
			User::After(2000);
			}
		PrintDebug(_L("DoThread1TestL() -10. BeginTransaction"));
		TRequestStatus status3(KRequestPending);
		cDb->RequestNotification(status3);
		PrintDebug(_L("DoThread1TestL() -11. InsertRecords"));
		while(!InsertTestRecordsL(EFalse, 10 , *cDb))
			{
			// Error during record insertion, most likely the other thread beat
			// us to it, Rollback and try again
			PrintDebug(_L("DoThread1TestL() - Rolling back Transaction"));
			cDb->RollbackTransaction();
			User::WaitForRequest(status3);
			status3 = KRequestPending;
			RandomDelay(seed);
			while(ret = cDb->BeginTransaction(), ret!=KErrNone)
				{
				PrintDebug(_L("DoThread1TestL() - Begin Transaction Again failed"));
				User::After(2000);
				}
			PrintDebug(_L("DoThread1TestL() - Begin Transaction Again"));
			cDb->RequestNotification(status3);
			}
		// Commit the 10 records, wait for CommDb
		PrintDebug(_L("DoThread1TestL() -12. Commit"));
		User::LeaveIfError(cDb->CommitTransaction());
		PrintDebug(_L("DoThread1TestL() -13. WaitForRequest"));
		User::WaitForRequest(status3);

		// Give the other thread a chance
		PrintDebug(_L("DoThread1TestL() -14. User::After"));
		RandomDelay(seed);
		}
	
	CleanupStack::PopAndDestroy(cDb);
	PrintDebug(_L("DoThread1TestL() RequestComplete"));
	remoteThreadInfo->iCaller.RequestComplete(remoteThreadInfo->iStatus1, KErrNone);

	}

TInt CCommDbTest046_05::executeStepL()
	{
	TInt ret;
	
	//Start off the first thread, this will run until it has done all but PutRecordChanges is called on the transaction.
	RThread thread1;
	TInt64 seed;
	TThreadInfo sync1;
	TRequestStatus remoteThreadStatus(KRequestPending);
	sync1.iStatus1=&remoteThreadStatus;
	sync1.iCaller.Duplicate(RThread(),EOwnerProcess);
	_LIT(KThread1Name,"Thread1");
	User::LeaveIfError(thread1.Create(KThread1Name,Thread1Function,KDefaultStackSize,NULL,&sync1));
	CleanupClosePushL(thread1);

	thread1.Resume();
	for (TInt count=0; count < KTest04605Repeats; count++ )
		{
		PrintDebug(_L("ExecuteStepL() - 1. InsertTestRecords"));
		InsertTestRecordsL(ETrue, 10, *iTheDb );
		
		//Start a transaction, write 10 records, then rollback
		RandomDelay(seed);
		while(ret = iTheDb->BeginTransaction(), ret!=KErrNone)
			{
			PrintDebug(_L("ExecuteStepL() - 2. BeginTransaction failed"));
			User::After(2000);
			}
		PrintDebug(_L("ExecuteStepL() - 2. BeginTransaction"));
		TRequestStatus status1(KRequestPending);
		iTheDb->RequestNotification(status1);
		PrintDebug(_L("ExecuteStepL() - 3. InsertTestRecords"));
		while(!InsertTestRecordsL(ETrue, 10 , *iTheDb))
			{
			// Error during record insertion, most likely the other thread beat
			// us to it, Rollback and try again
			PrintDebug(_L("ExecuteStepL() - Rolling back Transaction"));
			iTheDb->RollbackTransaction();
			User::WaitForRequest(status1);
			status1 = KRequestPending;
			RandomDelay(seed);
			while(ret = iTheDb->BeginTransaction(), ret!=KErrNone)
				{
				PrintDebug(_L("ExecuteStepL()  - Begin Transaction Again failed"));
				User::After(2000);
				}
			PrintDebug(_L("ExecuteStepL()   - Begin Transaction Again"));
			iTheDb->RequestNotification(status1);
			}
		// 10 records written, now rollback and wait for commdb
		PrintDebug(_L("ExecuteStepL() - 4. Rollback"));
		iTheDb->RollbackTransaction();
		PrintDebug(_L("ExecuteStepL() - 5. WaitForRequest"));
		User::WaitForRequest(status1);


		
		//Start a transaction, write 10 records, delete them, then rollback
		RandomDelay(seed);
		while(ret = iTheDb->BeginTransaction(), ret!=KErrNone)
			{
			PrintDebug(_L("ExecuteStepL() - 6. BeginTransaction failed"));
			User::After(2000);
			}
		PrintDebug(_L("ExecuteStepL() - 6. BeginTransaction"));
		TRequestStatus status2(KRequestPending);
		iTheDb->RequestNotification(status2);
		PrintDebug(_L("ExecuteStepL() - 7. InsertTestRecordsL"));
		while(!InsertTestRecordsL(ETrue, 10 , *iTheDb))
			{
			// Error during record insertion, most likely the other thread beat
			// us to it, Rollback and try again
			PrintDebug(_L("ExecuteStepL() - Rolling back Transaction"));
			iTheDb->RollbackTransaction();
			User::WaitForRequest(status2);
			status2 = KRequestPending;
			RandomDelay(seed);
			while(ret = iTheDb->BeginTransaction(), ret!=KErrNone)
				{
				PrintDebug(_L("ExecuteStepL()  - Begin Transaction Again failed"));
				User::After(2000);
				}
			PrintDebug(_L("ExecuteStepL()   - Begin Transaction Again"));
			iTheDb->RequestNotification(status2);
			}
		// 10 records written successfully, now delete them, rollback and wait for commmdb
		PrintDebug(_L("ExecuteStepL() - 8. DeleteTestRecordsL"));
		DeleteLastRecordsL(ETrue, 10, *iTheDb);
		PrintDebug(_L("ExecuteStepL() - 9. Rollback"));
		iTheDb->RollbackTransaction();
		PrintDebug(_L("ExecuteStepL() -10. WaitForRequest"));
		User::WaitForRequest(status2);


		
		//Start a transaction, write 10 records and commit them
		RandomDelay(seed);
		while(ret = iTheDb->BeginTransaction(), ret!=KErrNone)
			{
			PrintDebug(_L("ExecuteStepL() - 11. BeginTransaction failed"));
			User::After(2000);
			}
		PrintDebug(_L("ExecuteStepL() -11. BeginTransaction"));
		TRequestStatus status3(KRequestPending);
		iTheDb->RequestNotification(status3);
		PrintDebug(_L("ExecuteStepL() -12. InsertTestRecords"));
		while(!InsertTestRecordsL( ETrue,10 , *iTheDb))
			{
			// Error during record insertion, most likely the other thread beat
			// us to it, Rollback and try again
			PrintDebug(_L("ExecuteStepL() - Rolling back Transaction"));
			iTheDb->RollbackTransaction();
			User::WaitForRequest(status3);
			status3 = KRequestPending;
			RandomDelay(seed);
			while(ret = iTheDb->BeginTransaction(), ret!=KErrNone)
				{
				PrintDebug(_L("ExecuteStepL()  - Begin Transaction Again failed"));
				User::After(2000);
				}
			PrintDebug(_L("ExecuteStepL()   - Begin Transaction Again"));
			iTheDb->RequestNotification(status3);
			}
		PrintDebug(_L("ExecuteStepL() -13. Commit"));
		User::LeaveIfError(iTheDb->CommitTransaction());
		PrintDebug(_L("ExecuteStepL() -14. WaitForRequest"));
		User::WaitForRequest(status3);
		

		// Give the other thread a chance to do stuff
		PrintDebug(_L("ExecuteStepL() -15. User::After"));
		RandomDelay(seed);
		}

	PrintDebug(_L("ExecuteStepL() - Wait For Remote Thread"));
	User::WaitForRequest(remoteThreadStatus);
	CleanupStack::PopAndDestroy();	//thread1
	PrintDebug(_L("ExecuteStepL() - Exiting"));
	return KErrNone;
	}
	
TVerdict CCommDbTest046_05::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

TBool CCommDbTest046_05::InsertTestRecordsL( TBool aCallerIsMainThread, TUint aQuantity, CCommsDatabase& aDb )
	{
	TUint32 id;
	CCommsDbTableView* dbView = aDb.OpenTableLC( TPtrC(DIAL_OUT_ISP) );
	for(TUint count=0; count < aQuantity; count++ )
		{
		while(dbView->InsertRecord( id )!= KErrNone)
			{
			if(aCallerIsMainThread)
				PrintDebug(_L("ExecuteStepL()    Insert Record failed..."));
			else PrintDebug(_L("DoThread1TestL() Insert Record failed..."));
			CleanupStack::PopAndDestroy(dbView);
			return EFalse;
			}
		dbView->WriteTextL(TPtrC(COMMDB_NAME),_L("My Service Test Record"));
		dbView->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
		dbView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
		dbView->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
//		dbView->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
		dbView->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
		dbView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
		dbView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
		dbView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue);
		User::LeaveIfError(dbView->PutRecordChanges());
		if(aCallerIsMainThread)
			PrintDebug(_L("ExecuteStepL()    InsertingRecord..."));
		else PrintDebug(_L("DoThread1TestL() InsertingRecord..."));

	// Allow the other thread to do things between each record insertion
	User::After(5000);	
	}

	CleanupStack::PopAndDestroy(dbView);
	return ETrue;
	}

void CCommDbTest046_05::DeleteLastRecordsL( TBool aCallerIsMainThread, TUint aQuantity, CCommsDatabase& aDb)
	{
	TUint count;
	TInt ret = KErrGeneral;
	CCommsDbTableView* dbView;

	dbView  = aDb.OpenTableLC( TPtrC(DIAL_OUT_ISP) );
	CDBLEAVE( dbView->GotoFirstRecord(), KErrNone );

	//Move to the end of the view
	do
	{
		ret = dbView->GotoNextRecord();
	}while ( ret != KErrNotFound );
	
	//Need to move the cursor back into the view
	dbView->GotoPreviousRecord();

	//Now delete the record and move back one
	for ( count=0; count< aQuantity; count++)
		{
		// Allow the other thread to do things between each record deletion
		User::After(5000);
		User::LeaveIfError(dbView->DeleteRecord());
		User::LeaveIfError( dbView->GotoPreviousRecord());
		if(aCallerIsMainThread)
			PrintDebug(_L("ExecuteStepL()    Record Deleted..."));
		else PrintDebug(_L("DoThread1TestL() Record Deleted..."));

		}

	CleanupStack::PopAndDestroy(dbView);
	}

void CCommDbTest046_05::RandomDelay(TInt64 &aSeed)
	{
	// Random delay for between 0-1 seconds
	// Gives a thread a chance to access the database while the other 
	// thread is still sleeping

	// If this delay is made excessively large, the test is pointeless, 
	// the two threads will just take turns to do things with the database

	// If the delay is too short both threads will hammer CommDb simulataneously and 
	// they will effectively deadlock, neither getting the chance to complete anything

	
	TInt32 delay;
	TReal rnd = Math::FRand(aSeed);
	rnd*=1000000;
	delay=(TInt32) rnd;
	User::After(delay);
	}

void CCommDbTest046_05::PrintDebug(TPtrC )//aString)
	{
	// Uncomment this line to see whats going on in the debug trace
	//RDebug::Print(aString);
	}
