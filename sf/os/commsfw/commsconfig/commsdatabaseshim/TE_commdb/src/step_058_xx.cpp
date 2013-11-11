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

#include "step_058_xx.h"

/////////////////////
//	Test step 058.01
/////////////////////

//TestProtectRecords
CCommDbTest058_01::CCommDbTest058_01()
{
	// store the name of this test case
	SetTestStepName(_L("step_058_01"));
}

CCommDbTest058_01::~CCommDbTest058_01()
{
	delete iTheDb;
}

TVerdict CCommDbTest058_01::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}


enum TVerdict CCommDbTest058_01::doTestStepL( void )
	{
	TUint32 Ranking;
	INFO_PRINTF1(_L("Test Protect Connection Prefs Records "));
	SetTestStepResult(EPass);
	CCommsDbProtectConnectPrefTableView* table = (CCommsDbProtectConnectPrefTableView *)iTheDb->OpenConnectionPrefTableLC();

	TESTL(iTheDb->BeginTransaction()==KErrNone);
	
	TESTL(table->GotoFirstRecord()==KErrNone);

	// Check record security level is writable
	TInt recordAccess = RDbRowSet::EReadOnly;
	TESTL(table->GetRecordAccess(recordAccess) == KErrNone);
	TESTL(recordAccess == RDbRowSet::EUpdatable);

	// Modify and test the first record
	TESTL(ChangeCPRankingL(table, Ranking) == KErrNone);

	TRAPD(ErrVal,ProtectRecordL(table,KErrNone));		
	TESTL(ErrVal == KErrNone);

	// Check record security level is read only
	TESTL(table->GetRecordAccess(recordAccess) == KErrNone);
	TESTL(recordAccess == RDbRowSet::EReadOnly);

	// Test that protection is effective - i.e we can't change the record
	ErrVal = ChangeCPRankingL(table, Ranking); // Try modifying a field.
	TESTL(ErrVal == KErrAccessDenied);

	// Check record security level is still read only
	TESTL(table->GetRecordAccess(recordAccess) == KErrNone);
	TESTL(recordAccess == RDbRowSet::EReadOnly);

	table->GotoFirstRecord();

	TESTL(iTheDb->CommitTransaction()==KErrNone);

	SetTestStepResult(EPass);	INFO_PRINTF1(_L("\t\tSUCCESS"));

	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}


/////////////////////
//	Test step 058.02
/////////////////////

//TestUnprotectRecords

CCommDbTest058_02::CCommDbTest058_02()
{
	// store the name of this test case
	SetTestStepName(_L("step_058_02"));
}

CCommDbTest058_02::~CCommDbTest058_02()
{
	delete iTheDb;
}

TVerdict CCommDbTest058_02::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}


enum TVerdict CCommDbTest058_02::doTestStepL( void )
	{

	INFO_PRINTF1(_L("Test UnProtect Connection Prefs Records .."));
	SetTestStepResult(EPass);
	CCommsDbProtectConnectPrefTableView* table = (CCommsDbProtectConnectPrefTableView *)iTheDb->OpenConnectionPrefTableLC();

	TESTL(iTheDb->BeginTransaction()==KErrNone);

	if(table->GotoFirstRecord()==KErrNone)
		{
		// Check record state is read only
		TInt recordAccess = RDbRowSet::EUpdatable;
		TESTL(table->GetRecordAccess(recordAccess) == KErrNone);
		TESTL(recordAccess == RDbRowSet::EReadOnly);

		// Check record cannot be updated
		TRAPD(ret, UpdateRecordL(table,KErrAccessDenied));
		TESTL(ret == KErrNone);

		// Unprotect record
		table->GotoFirstRecord();
		TRAP(ret,UnprotectRecordL(table,KErrNone));
		TESTL(ret == KErrNone);

		// Check record state is now writable
		TESTL(table->GetRecordAccess(recordAccess) == KErrNone);
		TESTL(recordAccess == RDbRowSet::EUpdatable);

		// Test record can now be updated
		TRAP(ret,UpdateRecordL(table,KErrNone));
		TESTL(ret == KErrNone);
		}

	TESTL(iTheDb->CommitTransaction()==KErrNone);
	
	// Do separate transaction to check deletion is 
	// allowed as want to be able to roll this back
	TESTL(iTheDb->BeginTransaction()==KErrNone);
	
	if(table->GotoFirstRecord()==KErrNone)
		{
		// Check can delete record
		TRAPD(ret,DeleteRecordL(table,KErrNone));
		TESTL(ret == KErrNone);
		} 
	
	//don't commit as don't want to risk making db empty.
	iTheDb->RollbackTransaction();

	SetTestStepResult(EPass);	INFO_PRINTF1(_L("\t\tSUCCESS"));
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

