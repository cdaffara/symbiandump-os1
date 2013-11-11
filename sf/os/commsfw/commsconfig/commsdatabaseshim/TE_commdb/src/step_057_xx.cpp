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

#include "step_057_xx.h"

/////////////////////
//	Test step 057.01
/////////////////////

//TestProtectRecords
CCommDbTest057_01::CCommDbTest057_01()
{
	// store the name of this test case
	SetTestStepName(_L("step_057_01"));
}

CCommDbTest057_01::~CCommDbTest057_01()
{
	delete iTheDb;
}

TVerdict CCommDbTest057_01::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

enum TVerdict CCommDbTest057_01::doTestStepL( void )
	{

	INFO_PRINTF1(_L("Test Protect Connection Prefs Table "));
	SetTestStepResult(EPass);
    CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref  RecordData1, RecordData2;

	// protect the table and do some checking
	// database is modified
	//

	CCommsDbProtectConnectPrefTableView* table = (CCommsDbProtectConnectPrefTableView *)iTheDb->OpenConnectionPrefTableLC();
	CheckTableIsNotProtectedL(table);

	table->ReadConnectionPreferenceL(RecordData1);

	TESTL(table->ProtectTable()==KErrNone);
	
	CheckTableIsProtectedL(table);
	table->ReadConnectionPreferenceL(RecordData2);
	TESTL(RecordData1 == RecordData2);

	SetTestStepResult(EPass);	INFO_PRINTF1(_L("\t\tSUCCESS"));

	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}



/////////////////////
//	Test step 057.02
/////////////////////

//TestUnprotectRecords
CCommDbTest057_02::CCommDbTest057_02()
{
	// store the name of this test case
	SetTestStepName(_L("step_057_02"));
}

CCommDbTest057_02::~CCommDbTest057_02()
{
	delete iTheDb;
}

TVerdict CCommDbTest057_02::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

enum TVerdict CCommDbTest057_02::doTestStepL( void )
	{

	INFO_PRINTF1(_L("Test UnProtect Connection Prefs Table .."));
	SetTestStepResult(EPass);
	// unprotect the table and do some checking
	// database is modified
	//
	CCommsDbProtectConnectPrefTableView* table = (CCommsDbProtectConnectPrefTableView *)iTheDb->OpenConnectionPrefTableLC();
	CheckTableIsProtectedL(table);

	TESTL(table->UnprotectTable()==KErrNone);
	
	CheckTableIsNotProtectedL(table);
	
	SetTestStepResult(EPass);	INFO_PRINTF1(_L("\t\tSUCCESS"));
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

