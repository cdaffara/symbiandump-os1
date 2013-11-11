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
// This contains CTestCase which is the base class for all the TestCase DLLs

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <f32file.h>
// Test system includes
#include "comms-infras/commdb/protection/protectdb.h"
#include "comms-infras/commdb/protection/protectcpdb.h"
#include <cdbpreftable.h>
#include "CDBSTD.H"
#include "Teststepcommdb.h"


// constructor
CTestStepCommDb::CTestStepCommDb()
	{
	iRemoteObjects = EFalse;
	}

// destructor
CTestStepCommDb::~CTestStepCommDb()
	{
	popAndDestroyDatabaseObjects();
	}

// file copy
void CTestStepCommDb::copyFileL(const TDesC& anOld,const TDesC& aNew)
	{
	// create a fileserver
	RFs fileSystem;

	// connect to file server
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);

	// create a file manager
	CFileMan * fileMan = CFileMan::NewL(fileSystem);
	CleanupStack::PushL(fileMan);

	// parse the filenames
	TParse source;
	User::LeaveIfError(source.Set(anOld, NULL, NULL));

	// parse the filenames
	TParse target;
	User::LeaveIfError(target.Set(aNew, NULL, NULL));

	// do the copy
	User::LeaveIfError(fileMan->Copy(source.FullName(), target.FullName(), CFileMan::EOverWrite));

	// make read/write
	User::LeaveIfError(fileSystem.SetAtt(source.FullName(),NULL, KEntryAttReadOnly));

	// clean up
	CleanupStack::PopAndDestroy(fileMan);
	CleanupStack::PopAndDestroy(&fileSystem);
	}

void CTestStepCommDb::deleteFileL(const TDesC& aFileName)
	{
	// create a fileserver
	RFs  fileSystem;

	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);

	INFO_PRINTF1(_L("Set file to read-write in order to delete"));

	// Remove read only flag
	TInt ret = fileSystem.SetAtt(aFileName, 0, KEntryAttReadOnly);
	if (ret == KErrNotFound)
		{
		// If file already removed then no need to delete it
		INFO_PRINTF1(_L("File not found"));
		CleanupStack::PopAndDestroy(&fileSystem);
		return;
		}
	User::LeaveIfError(ret);

	// Delete file
	User::LeaveIfError(fileSystem.Delete(aFileName));
	INFO_PRINTF1(_L("deleted file"));

	// clean up
	CleanupStack::PopAndDestroy(&fileSystem);
	}

void CTestStepCommDb::DeleteCurrentDatabaseL()
	{
	RDbs rDbs;

//	User::LeaveIfError(rDbs.Connect());
	TInt ret(KErrNone);
	ret = rDbs.Connect();
	if(ret!=KErrNone)
		{
		INFO_PRINTF2(_L("CTestStepCommDb::DeleteCurrentDatabaseL : rDbs.Connect FAILED with %d"),ret );
		User::Leave(ret);
		}

	CleanupClosePushL(rDbs);
	TName commDb(KCDrive);
	commDb.Append(KDbBaseName);
	TUid KCommsDbFileUid = { 0x10004e1d };
	// May need to make several
	// attempts if it is transiently locked; report these and if
	// not successful after a number of attempts report this and abort
	TInt remainingTries = 5;
//	TInt ret (KErrNone);
	while(remainingTries-- > 0)
		{
		ret = rDbs.DeleteDatabase(commDb, KCommsDbFileUid);
		if(ret == KErrNone || ret == KErrPathNotFound || ret == KErrNotFound)
			{
			ret = KErrNone;
			break;
			}
		else
			{
			INFO_PRINTF4(_L("CTestStepCommDb::DeleteCurrentDatabaseL : rDbs.DeleteDatabase FAILED to delete %S %S with %d"),&KCDrive,&KDbBaseName,ret);
			}
		}
	User::LeaveIfError(ret);
	CleanupStack::PopAndDestroy(&rDbs);
	}

void CTestStepCommDb::CopyDatabaseL(const TDesC& aSource)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RDbs rDbs;
	User::LeaveIfError(rDbs.Connect());
	CleanupClosePushL(rDbs);
	const TUid KCommsDbFileUid = {0x10004e1d};

	TInt remainingTries = 5;
	TInt ret (KErrNone);
	DeleteCurrentDatabaseL();
	while(remainingTries-- > 0)
		{
		ret = rDbs.CopyDatabase(aSource, KDbFileName, KCommsDbFileUid);
		if(ret == KErrNone || ret == KErrPathNotFound || ret == KErrNotFound)
			{
			break;
			}
		}
	User::LeaveIfError(ret);
	CleanupStack::PopAndDestroy(&rDbs);
	CleanupStack::PopAndDestroy(&fs);
	}


TInt CTestStepCommDb::copyTestDatabase( TDatabaseNumber aDbNumber )
	{
	(void)aDbNumber;
	return KErrNone;
	}

TInt CTestStepCommDb::copyDatabaseToDefault( TDatabaseNumber aDbNumber )
	{
	(void)aDbNumber;
	return KErrNone;
	}

TBool CTestStepCommDb::openDbAndViewL( const TDesC16& aTable )
	{
	TBool created = EFalse;

	//Only create Db and view objects if they're not already created
	if ( ! iTheDb )
		{
		iTheDb=CCommsDatabase::NewL();
		created = ETrue;
		}

	if ( !iTheView )
		{
		iTheView=iTheDb->OpenTableLC( aTable );
		CleanupStack::Pop(); //no need to put it on the cleanup stack
		}

	return created;
	}

TVerdict CTestStepCommDb::doTestStepPostambleL()
	{
	if ( iTheView )
		{
		delete iTheView;
		iTheView = 0;
		}

	if ( iTheDb )
		{
		delete iTheDb;
		iTheDb = 0;
		}

        SetTestStepResult(EPass);	return TestStepResult();
	}



void CTestStepCommDb::popAndDestroyDatabaseObjects()
	{
		// doTestStepPostambleL();
	}

TBool CTestStepCommDb::openDbL()
	{

	TBool created = EFalse;

	if ( !iTheDb )
		{
		//Only open the Db if its not already open
		TRAPD(ret,iTheDb=CCommsDatabase::NewL());
		if(ret!=KErrNone)
			{
			INFO_PRINTF2(_L("CTestStepCommDb::openDbL : CCommsDatabase::NewL LEFT with %d"),ret);
			User::Leave(ret);
			}
		created = ETrue;
		}
	return created;

	}

void CTestStepCommDb::setRemoteDatabaseObjects( CCommsDatabase* aDb, CCommsDbTableView* aDbView )
	{
		if ( !iTheDb )
			{
			//make sure that no-one else has already set the objects
			iTheDb = aDb;
			iTheView = aDbView;
			iRemoteObjects = ETrue;
			}
	}


void CTestStepCommDb::clearRemoteDatabaseObjects()
	{
		if( iRemoteObjects )
			{
			//Only clear objects if they were previously set remotly
			iTheDb = 0;
			iTheView = 0;
			}
	}

TInt CTestStepCommDb::executeStepL()
	{
		return KErrGeneral;
	}


TInt CTestStepCommDb::executeStep(CTestStepCommDb& aTestStep)
	{
	TInt r,ret= KErrNone;

	TRAP( r, ret = aTestStep.executeStepL() );

	if ( r != KErrNone )
		ret = r;

	return ret;
	}

TInt CTestStepCommDb::executeStep( )
	{
	TInt ret=0;

	TRAPD( r, ret = executeStepL() );

	if ( r != KErrNone )
		ret = r;

	return ret;
	}

//
//This heap failure member function is used when a test step needs a Database and a View
//object.
//
TInt CTestStepCommDb::doTestStepWithHeapFailureL( CTestStepCommDb& aTestStep, TInt aReturnValue)
	{
	TInt ret=0;

	for (TInt i=1; i< KHeapFailure ; i++)
		{
		aTestStep.doTestStepPreambleL();

		__UHEAP_FAILNEXT(i);

		ret = aTestStep.executeStep();

		__UHEAP_RESET;

		aTestStep.doTestStepPostambleL();

		if ( ret == KErrNoMemory )
			{
			//The heap failure has been trapped correctly
			continue;
			}
		else if ( ret == aReturnValue )
			{
			//Test step normal behaviour
			INFO_PRINTF4(_L("%S PASSED heap failure test, i = %d return code==%d"),
				&aTestStep.TestStepName(), i, ret );

			break;
			}
		else
			{
			// test step has not returned the exepected error value ( which was either KErrNoMemory or aReturnValue );
			INFO_PRINTF5(_L("%S *FAILED* heap failure test, i=%d return code:%d expected:%d"),
				&aTestStep.TestStepName(), i, ret, aReturnValue );
			SetTestStepResult(EFail);			break;
			}

		}

	return KErrNone;
	}

void CTestStepCommDb::CheckTableIsProtectedL(CCommsDbProtectTableView* aTable)
	{

	TESTL(iTheDb->BeginTransaction()==KErrNone);

	if(aTable->GotoFirstRecord()==KErrNone)
		{
		// Check settings on Record
		RDbRowSet::TAccess tableAccess = RDbRowSet::EUpdatable;
		TInt ret = aTable->GetTableAccess(tableAccess);
		TESTL(ret == KErrNone);
		TESTL(tableAccess == RDbRowSet::EReadOnly);

		// Test that first record cannot be updated
		TRAP(ret,UpdateRecordL(aTable,KErrAccessDenied));
		TESTL(ret == KErrNone);

		// Test that cannot delete first record
		TRAP(ret,DeleteRecordL(aTable,KErrAccessDenied));
		TESTL(ret == KErrNone);

		// Test that cannot insert new record
		TRAP(ret,InsertRecordL(aTable,KErrAccessDenied));
		TESTL(ret == KErrNone);

		aTable->GotoFirstRecord();
		}
	TESTL(iTheDb->CommitTransaction()==KErrNone);
	}


void CTestStepCommDb::CheckTableIsProtectedL(CCommsDbProtectConnectPrefTableView* aTable)
	{

	TESTL(iTheDb->BeginTransaction()==KErrNone);
	if(aTable->GotoFirstRecord()==KErrNone)
		{
		// Check settings on Record
		RDbRowSet::TAccess tableAccess = RDbRowSet::EUpdatable;
		TInt ret = aTable->GetTableAccess(tableAccess);
		TESTL(ret == KErrNone);
		TESTL(tableAccess == RDbRowSet::EReadOnly);

		// Test that first record cannot be updated
		TRAP(ret,UpdateRecordL(aTable,KErrAccessDenied));
		TESTL(ret == KErrNone);

		// Test that cannot delete first record
		TRAP(ret,DeleteRecordL(aTable,KErrAccessDenied));
		TESTL(ret == KErrNone);

		// Test that cannot insert new record
		TRAP(ret,InsertRecordL(aTable,KErrAccessDenied));
		TESTL(ret == KErrNone);

		aTable->GotoFirstRecord();
		}
	TESTL(iTheDb->CommitTransaction()==KErrNone);
	}

void CTestStepCommDb::CheckTableIsNotProtectedL(CCommsDbProtectTableView* aTable)
	{
	TESTL(iTheDb->BeginTransaction()==KErrNone);
	if(aTable->GotoFirstRecord()==KErrNone)
		{
		// Check settings on Table
		RDbRowSet::TAccess tableAccess = RDbRowSet::EReadOnly;
		TInt ret = aTable->GetTableAccess(tableAccess);
		TESTL(ret == KErrNone);
		TESTL(tableAccess == RDbRowSet::EUpdatable);

		// Test can update first record
		TRAP(ret,UpdateRecordL(aTable,KErrNone));
		TESTL(ret == KErrNone);

		// Test that can delete record
		TRAP(ret,DeleteRecordL(aTable,KErrNone));
		TESTL(ret == KErrNone);

		// Test that can insert new record
		TRAP(ret,InsertRecordL(aTable,KErrNone));
		TESTL(ret == KErrNone);

		aTable->GotoFirstRecord();
		}
	TESTL(iTheDb->CommitTransaction()==KErrNone);
	}

void CTestStepCommDb::CheckTableIsNotProtectedL(CCommsDbProtectConnectPrefTableView* aTable)
	{
	TESTL(iTheDb->BeginTransaction()==KErrNone);
	if(aTable->GotoFirstRecord()==KErrNone)
		{
		// Check settings on Table
		RDbRowSet::TAccess tableAccess = RDbRowSet::EReadOnly;
		TInt ret = aTable->GetTableAccess(tableAccess);
		TESTL(ret == KErrNone);
		TESTL(tableAccess == RDbRowSet::EUpdatable);

		// Test can update first record
		TRAP(ret,UpdateRecordL(aTable,KErrNone));
		TESTL(ret == KErrNone);

		// Test that can delete record
		TRAP(ret,DeleteRecordL(aTable,KErrNone));
		TESTL(ret == KErrNone);

		// Test that can insert new record
		TRAP(ret,InsertRecordL(aTable,KErrNone));
		TESTL(ret == KErrNone);

		aTable->GotoFirstRecord();
		}
	TESTL(iTheDb->CommitTransaction()==KErrNone);
	}

void CTestStepCommDb::GoToPositionL(CCommsDbProtectTableView* aTable,TInt aPos)
	{
	TESTL(aTable->GotoFirstRecord()==KErrNone);
	for (TInt i = 0; i < aPos; i++)
		{
		TESTL(aTable->GotoNextRecord()==KErrNone);
		}
	}

void CTestStepCommDb::ProtectRecordL(CCommsDbProtectTableView* aTable, TInt aResult)
	{
	TESTL(aTable->ProtectRecord()==aResult);
	}

void CTestStepCommDb::ProtectRecordL(CCommsDbProtectConnectPrefTableView* aTable, TInt aResult)
	{
	TESTL(aTable->ProtectRecord()==aResult);
	}

void CTestStepCommDb::UnprotectRecordL(CCommsDbProtectTableView* aTable, TInt aResult)
	{
	TESTL(aTable->UnprotectRecord()==aResult);
	}

void CTestStepCommDb::UnprotectRecordL(CCommsDbProtectConnectPrefTableView* aTable, TInt aResult)
	{
	TESTL(aTable->UnprotectRecord()==aResult);
	}

void CTestStepCommDb::UpdateRecordL(CCommsDbProtectTableView* aTable, TInt aResult)
	{

	TESTL(aTable->UpdateRecord()==aResult);
	if (aResult == KErrNone)
		{
		ModifyRecordL(aTable);
		TESTL(aTable->PutRecordChanges()==KErrNone);
		}
	}

void CTestStepCommDb::UpdateRecordL(CCommsDbProtectConnectPrefTableView* aTable, TInt aExpectedResult)
	{

	TUint32 Ranking;
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref RecordData;

	TInt ErrVal = ChangeCPRankingL(aTable, Ranking); // Try modifying a field.

	TESTL(ErrVal == aExpectedResult);

	if (ErrVal == KErrNone)
		{
		// The change was allowed, so check it actually happened.
		aTable->ReadConnectionPreferenceL(RecordData);
		TESTL(Ranking == RecordData.iRanking);
		}
	}

void CTestStepCommDb::DeleteRecordL(CCommsDbProtectTableView* aTable, TInt aResult)
	{
	TESTL(aTable->DeleteRecord()==aResult);
	}

void CTestStepCommDb::DeleteRecordL(CCommsDbProtectConnectPrefTableView* aTable, TInt aExpectedResult)
	{
	TRAPD(ErrVal, aTable->DeleteConnectionPreferenceL());
	TESTL(ErrVal == aExpectedResult);
	}

void CTestStepCommDb::InsertRecordL(CCommsDbProtectTableView* aTable, TInt aExpectedResult)
	{
	TUint32 recordId;
	TInt ret = aTable->InsertRecord(recordId);
	TESTL(ret==aExpectedResult||ret==KErrAlreadyExists); //for some tables insertion fails
											  //because they can only have
											//one record (i.e. DEFAULT_GPRS)
	if (ret == KErrNone)
		{
		PopulateInsertedRecordL(aTable);
		TESTL(aTable->PutRecordChanges()==KErrNone);
		}
	}

void CTestStepCommDb::InsertRecordL(CCommsDbProtectConnectPrefTableView* aTable, TInt aExpectedResult)
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref PrefData1, PrefData2;

	PrefData1.iRanking = 0;
	PrefData1.iDirection = ECommDbConnectionDirectionOutgoing;
	PrefData1.iDialogPref = ECommDbDialogPrefWarn;
	PrefData1.iBearer.iBearerSet = ECommDbBearerCSD;
	PrefData1.iBearer.iIapId = 1;

	TRAPD(ErrVal, aTable->InsertConnectionPreferenceL(PrefData1));
	TESTL(ErrVal==aExpectedResult); //for some tables insertion fails
											  //because they can only have
											//one record (i.e. DEFAULT_GPRS)
	if (ErrVal == KErrNone)
		{
		aTable->ReadConnectionPreferenceL(PrefData2);
		TESTL(PrefData1 == PrefData2);
		}
	}


#define TEST_FCN(_F_)		{TRAPD(ret,_F_); TESTL(ret == KErrNone);}

void CTestStepCommDb::ModifyRecordL(CCommsDbProtectTableView* aTable)
//
// Modify some field in current record
//
	{
	TEST_FCN(aTable->WriteTextL(TPtrC(COMMDB_NAME),_L("test")));
	}
// Extract the protection state for the current record,
// and leave if it's not the state expected
void CTestStepCommDb::CheckRecordAccessL(CCommsDbProtectConnectPrefTableView* aTable, TInt aExpectedAccess)
	{
	TInt FoundAccess;
	// Set the state to one not expected, so it will fail if not explicitly changed
	if (aExpectedAccess == RDbRowSet::EUpdatable)
		FoundAccess = RDbRowSet::EReadOnly;
	else
		FoundAccess = RDbRowSet::EUpdatable;

	TESTL(aTable->GetRecordAccess(FoundAccess) == KErrNone);
	TESTL(aExpectedAccess == FoundAccess);
	}
void CTestStepCommDb::PopulateInsertedRecordL(CCommsDbProtectTableView* aTable)
//
// Populate minimal set of fields in current newly inserted record.
//
	{
	TBuf<50> tableName;
	aTable->GetTableName(tableName);

	if (tableName == TPtrC(DIAL_OUT_ISP))
		{
		TEST_FCN(aTable->WriteTextL(TPtrC(COMMDB_NAME),_L("Inserted Service")));
		TEST_FCN(aTable->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue));
		TEST_FCN(aTable->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse));
		TEST_FCN(aTable->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue));
		TEST_FCN(aTable->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue));
		TEST_FCN(aTable->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue));
		TEST_FCN(aTable->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue));
		TEST_FCN(aTable->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER),ETrue));
		}

	// EXTEND OR MODIFY this function if want to use different table
	}
// Just a utility function - for checking the protection of the Connection Preferences
// table and its records, we just try changing the ranking field of the first one.
TInt CTestStepCommDb::ChangeCPRankingL(CCommsDbProtectConnectPrefTableView* aTable, TUint32& Ranking)
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref  RecordData;

	aTable->ReadConnectionPreferenceL(RecordData);
	if(RecordData.iRanking == 1)
		Ranking = 0;
	else
		Ranking = 1;
	TRAPD(ErrVal, aTable->ChangeConnectionPreferenceRankL(Ranking));
	return(ErrVal);
	}

#undef TEST_FCN
