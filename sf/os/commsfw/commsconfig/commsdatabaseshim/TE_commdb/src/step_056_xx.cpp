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

#include "step_056_xx.h"

/////////////////////
//	Test step 056.01
/////////////////////

//TestProtectRecords
CCommDbTest056_01::CCommDbTest056_01()
{
	// store the name of this test case
	SetTestStepName(_L("step_056_01"));
}

CCommDbTest056_01::~CCommDbTest056_01()
{
	delete iTheDb;
}

TVerdict CCommDbTest056_01::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

enum TVerdict CCommDbTest056_01::doTestStepL( void )
	{

	INFO_PRINTF1(_L("Test Normal Records : Protection .."));
	SetTestStepResult(EPass);
	CCommsDbProtectTableView* table = (CCommsDbProtectTableView *)iTheDb->OpenTableLC(TPtrC(DIAL_OUT_ISP));
	TESTL(iTheDb->BeginTransaction()==KErrNone);
	
	TESTL(table->GotoFirstRecord()==KErrNone);
	
	// Check record security level is writable
	TInt recordAccess = 1;
	TESTL(table->GetRecordAccess(recordAccess) == KErrNone);
	TESTL(recordAccess == 0);

	// Modify and test the first record
	TRAPD(ret,UpdateRecordL(table,KErrNone));
	TESTL(ret == KErrNone);
	
	TRAP(ret,ProtectRecordL(table,KErrNone));		
	TESTL(ret == KErrNone);

		// Test that protection is effective
	TRAP(ret,UpdateRecordL(table,KErrAccessDenied));
	TESTL(ret == KErrNone);
	TRAP(ret,DeleteRecordL(table,KErrAccessDenied));
	TESTL(ret == KErrNone);

		// Check record security level is read only
	TESTL(table->GetRecordAccess(recordAccess) == KErrNone);
	TESTL(recordAccess == 1);

	table->GotoFirstRecord();

	TESTL(iTheDb->CommitTransaction()==KErrNone);


	SetTestStepResult(EPass);	INFO_PRINTF1(_L("\t\tSUCCESS"));

	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}


/////////////////////
//	Test step 056.02
/////////////////////

//TestUnprotectRecords
CCommDbTest056_02::CCommDbTest056_02()
{
	// store the name of this test case
	SetTestStepName(_L("step_056_02"));
}

CCommDbTest056_02::~CCommDbTest056_02()
{
	delete iTheDb;
}

TVerdict CCommDbTest056_02::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

enum TVerdict CCommDbTest056_02::doTestStepL( void )
	{

	INFO_PRINTF1(_L("Test Normal Records : unprotection .."));
	SetTestStepResult(EPass);
	CCommsDbProtectTableView* table = (CCommsDbProtectTableView *)iTheDb->OpenTableLC(TPtrC(DIAL_OUT_ISP));
	TESTL(iTheDb->BeginTransaction()==KErrNone);

	if(table->GotoFirstRecord()==KErrNone)
		{
		// Check record state is read only
		TInt recordAccess = 0;
		TESTL(table->GetRecordAccess(recordAccess) == KErrNone);
		TESTL(recordAccess == 1);

		// Check record cannot be updated
		TRAPD(ret, UpdateRecordL(table,KErrAccessDenied));
		TESTL(ret == KErrNone);

		// Unprotect record
		table->GotoFirstRecord();
		TRAP(ret,UnprotectRecordL(table,KErrNone));
		TESTL(ret == KErrNone);

		// Check record state is now writable
		TESTL(table->GetRecordAccess(recordAccess) == KErrNone);
		TESTL(recordAccess == 0);

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

