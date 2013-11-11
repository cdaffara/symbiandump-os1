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
 
// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbstore.h>

// Test system includes
#include "Teststepcommdb.h"
#include "comms-infras/commdb/protection/protectdb.h"
#include "comms-infras/commdb/protection/protectcpdb.h"
#include <cdbpreftable.h>

#include "step_059_xx.h"

#define ACCESS_TYPE_TABLE		_S("AccessTypeTable")
#define TABLE_ACC_NAME          _S("TableName")
#define TABLE_ACC_TYPE          _S("AccessType")

/////////////////////
//	Test step 059.01
/////////////////////

//TestProtectRecords
CCommDbTest059_01::CCommDbTest059_01()
{
	// store the name of this test case
	SetTestStepName(_L("step_059_01"));
}

CCommDbTest059_01::~CCommDbTest059_01()
{
	delete iTheDb;
}

TVerdict CCommDbTest059_01::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

enum TVerdict CCommDbTest059_01::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Combinational test (Only for Conn Pref)"));
	SetTestStepResult(EPass);

// Try a combination of operations on a table, as follows:
// 1) protect record
// 2) attempt update to protected record
// 3) attempt update to unprotected record
// 4) attempt delete of unprotected record
// 5) protect table
// 6) attempt record insertion
// 7) attempt protected record deletion & update
// 8) attempt unprotected record deletion and update
// 9) unprotect table
// 10) attempt record insertion
// 11) attempt record deletion
// 12) attempt update to protected record
// 13) attempt update to unprotected record
// 15) unprotect record
// 15) attempt update to formerly unprotected record
// 16) attempt update to unprotected record
//
	CCommsDbProtectConnectPrefTableView* table = (CCommsDbProtectConnectPrefTableView *)iTheDb->OpenConnectionPrefTableLC();
			
	TESTL(iTheDb->BeginTransaction()==KErrNone);

	if(table->GotoFirstRecord()==KErrNone)
		{
		// Add another record to the table (so there should be 3)
		InsertRecordL(table,KErrNone);

		TESTL(table->GotoFirstRecord()==KErrNone);
		TESTL(table->GotoNextRecord()==KErrNone);

		// 1) Protect the second record
		CheckRecordAccessL(table,RDbRowSet::EUpdatable); 
		ProtectRecordL(table,KErrNone);
		CheckRecordAccessL(table,RDbRowSet::EReadOnly); 
		
		// 2) Check record cannot be updated
		UpdateRecordL(table,KErrAccessDenied);

		// Go to the first record (unprotected)

		TESTL(table->GotoFirstRecord()==KErrNone);
		CheckRecordAccessL(table,RDbRowSet::EUpdatable); 

		// 3) Check it can be updated
		UpdateRecordL(table,KErrNone);

		// 4) Check it can be deleted
		DeleteRecordL(table,KErrNone);

		//(which means that the new first record should be protected.....)
		TESTL(table->GotoFirstRecord()==KErrNone);
		CheckRecordAccessL(table,RDbRowSet::EReadOnly); 

		// 5) Unprotect the first record which should be protected
		CheckRecordAccessL(table,RDbRowSet::EReadOnly); 
		UnprotectRecordL(table,KErrNone);
		CheckRecordAccessL(table,RDbRowSet::EUpdatable); 
		
		// 6) Attempt update to formerly protected record (should succeed)
		UpdateRecordL(table,KErrNone);
		
		// End the transaction, as a protect table is a separate one.
		TESTL(iTheDb->CommitTransaction()==KErrNone);

		// 7) Protect the table
		TESTL(table->ProtectTable()==KErrNone);

		TESTL(iTheDb->BeginTransaction()==KErrNone);

		// 8) Check a record can't be inserted
		InsertRecordL(table,KErrAccessDenied);

		// 9) Check a record can't be deleted or updated
		DeleteRecordL(table,KErrAccessDenied);
		UpdateRecordL(table,KErrAccessDenied);
		
		// End the transaction, as unprotect table is a separate one.
		TESTL(iTheDb->CommitTransaction()==KErrNone);

		// 10) Unprotect the table
		TESTL(table->UnprotectTable()==KErrNone);
	
		table->GotoFirstRecord();
		TESTL(iTheDb->BeginTransaction()==KErrNone);
		// 11) Check a record can be inserted
		InsertRecordL(table,KErrNone);

		// 12) Check an unprotected record can be deleted, but a protected one can't
		// Again, yes it can so don't test this here.
		DeleteRecordL(table,KErrNone);

		// 13) Check the unprotected record can be updated
		// Delete a record so there's only one left. Otherwise an update now will
		// fail validation due to duplicate records.
		TESTL(table->GotoFirstRecord()==KErrNone);
		DeleteRecordL(table,KErrNone);
		TESTL(table->GotoFirstRecord()==KErrNone);
		UpdateRecordL(table,KErrNone);
		
		// 14) Finally check that last record can be deleted
		TESTL(table->GotoFirstRecord()==KErrNone);
		DeleteRecordL(table,KErrNone);
		}
	TESTL(iTheDb->CommitTransaction()==KErrNone);

	SetTestStepResult(EPass);	INFO_PRINTF1(_L("\t\tSUCCESS"));

	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}



/////////////////////
//	Test step 060.01
/////////////////////

///Panic Test
CCommDbTest060_01::CCommDbTest060_01()
{
	// store the name of this test case
	SetTestStepName(_L("step_060_01"));

}

CCommDbTest060_01::~CCommDbTest060_01()
	{
	delete iTheDb;
	}

TVerdict CCommDbTest060_01::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

enum TVerdict CCommDbTest060_01::doTestStepL( void )
	{
	SetTestStepResult(EFail);
	//turn-off JIT debugging...
//	User::SetJustInTime(EFalse);

	// Use the same table as step_055_01
	CCommsDbProtectTableView* table = NULL;
	table = (CCommsDbProtectTableView *)iTheDb->OpenTableLC(TPtrC(DIAL_OUT_ISP));

	CheckTableIsNotProtectedL(table);

	// Protection uses transaction, transaction within transaction is not allowed
	// this test ensures that the ProtectTable code handles this correctly
	if (table->GotoFirstRecord() == KErrNone)
		{
	    if (iTheDb->BeginTransaction() == KErrNone)
			{
			// Set protection from within a transaction
			TESTL(table->ProtectTable()==KErrNone);
	
			CheckTableIsProtectedL(table);
			SetTestStepResult(EPass);			INFO_PRINTF1(_L("\t\tSUCCESS"));
				
			iTheDb->RollbackTransaction();
			}
		}
	CleanupStack::PopAndDestroy(table);
	
	return TestStepResult();
	}

///////////////////////////////////////////
//	Test step 060.02
//This test case is for testing the fix for 
//DEF035522: CommDb protection is easily circumvented using public APIs
////////////////////////////////////////////

///Panic Test
CCommDbTest060_02::CCommDbTest060_02()
{
	// store the name of this test case
	SetTestStepName(_L("step_060_02"));

}

CCommDbTest060_02::~CCommDbTest060_02()
{

}

enum TVerdict CCommDbTest060_02::doTestStepL( void )
	{

	SetTestStepResult(EPass);	CCommsDatabase* db = CCommsDatabase::NewL();
	CleanupStack::PushL(db);
	//Protect ISP the table
	INFO_PRINTF1(_L("Protect ISP table"));
	TRAPD(protect, ProtectISPTableL(db));
	if( protect != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to protect ISP table"));
         SetTestStepResult(EFail);		return TestStepResult();
		}
	//try to remove the protection using public APIs
	INFO_PRINTF1(_L("Try to remove the protection using public APIs"));
	TRAPD(unprotect, RemoveProtectionL(db));
	//Should return KErrNotFound
	if (unprotect != KErrNotFound)
		{
		INFO_PRINTF1(_L("Protection settings broken using public APIs"));
         SetTestStepResult(EFail);		return TestStepResult();
		}
	INFO_PRINTF1(_L("Table is well protected!! "));
	CleanupStack::PopAndDestroy(db);
	return TestStepResult();
	}

void CCommDbTest060_02::ProtectISPTableL(CCommsDatabase* aDb)
//
//  Protect the DialOutISP table using the hidden APIs
//
	{

	CCommsDbProtectTableView* view = (CCommsDbProtectTableView*) aDb->OpenTableLC(TPtrC(DIAL_OUT_ISP));
    if(view->GotoFirstRecord() == KErrNone)
		{
		User::LeaveIfError(view->ProtectTable());
		}

	CleanupStack::PopAndDestroy(view);
	}


void CCommDbTest060_02::RemoveProtectionL(CCommsDatabase* aDb)
//
//  Use the public CommDb APIs to modify the AccessTypeTable
//
	{
	
	aDb->ShowHiddenRecords();
	//should fail here as its trying to open ACCESS_TYPE_TABLE
	//leaves with KErrNotFound
	CCommsDbTableView* view = aDb->OpenTableLC(TPtrC(ACCESS_TYPE_TABLE));
	
	//Shouldn't get here..
	INFO_PRINTF1(_L("Contents of Access Type table:"));
	while (view->GotoNextRecord() == KErrNone)
		{
		// display the current access settings
		TBuf<KCommsDbSvrMaxFieldLength> name;
		view->ReadTextL(TPtrC(TABLE_ACC_NAME), name);

		TUint32 accessType;
		view->ReadUintL(TPtrC(TABLE_ACC_TYPE), accessType);

		RDebug::Print(_L("Table: '%S'\tAccess type: %d"), &name, accessType);

		// now set the access type to 'updateable'
		view->UpdateRecord();
		view->WriteUintL(TPtrC(TABLE_ACC_TYPE), (TUint32)RDbRowSet::EUpdatable);
		view->PutRecordChanges();

		// display the new access settings
		view->ReadUintL(TPtrC(TABLE_ACC_TYPE), accessType);
		INFO_PRINTF3(_L("Table: '%S'\tNew access type: %d"), &name, accessType);
		}

	CleanupStack::PopAndDestroy(view);
	}
