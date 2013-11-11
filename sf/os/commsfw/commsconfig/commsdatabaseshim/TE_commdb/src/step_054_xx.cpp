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

#include "step_054_xx.h"

/////////////////////
//	Test step 054.01
/////////////////////

CCommDbTest054_01::CCommDbTest054_01()
{
	// store the name of this test case
	SetTestStepName(_L("step_054_01"));
}

CCommDbTest054_01::~CCommDbTest054_01()
{
	delete iTheDb;
}

TVerdict CCommDbTest054_01::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

//Protect Connection Preference Record
enum TVerdict CCommDbTest054_01::doTestStepL( void )
	{

	INFO_PRINTF1(_L("Connection Preference Record : Protection .."));
	SetTestStepResult(EPass);
	CCommsDbProtectConnectPrefTableView* table = (CCommsDbProtectConnectPrefTableView *)iTheDb->OpenConnectionPrefTableLC();
	TESTL(iTheDb->BeginTransaction()==KErrNone);
	
	if(table->GotoFirstRecord()==KErrNone)
		{
		// Protect the first record
		ProtectRecordL(table,KErrNone);
		SetTestStepResult(EPass);		INFO_PRINTF1(_L("\t\tSUCCESS"));
		}
	else
		{
		SetTestStepResult(EFail);		INFO_PRINTF1(_L("\t\tFAIL"));
		}

	TESTL(iTheDb->CommitTransaction()==KErrNone);

	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/////////////////////
//	Test step 054.02
/////////////////////

CCommDbTest054_02::CCommDbTest054_02()
{
	// store the name of this test case
	SetTestStepName(_L("step_054_02"));
}

CCommDbTest054_02::~CCommDbTest054_02()
{
	delete iTheDb;
}

TVerdict CCommDbTest054_02::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

//Protect Connection Preference Record
enum TVerdict CCommDbTest054_02::doTestStepL( void )
	{

	INFO_PRINTF1(_L("Connection Preference Record : Unprotection .."));
	SetTestStepResult(EPass);
	CCommsDbProtectConnectPrefTableView* table = (CCommsDbProtectConnectPrefTableView *)iTheDb->OpenConnectionPrefTableLC();
	TESTL(iTheDb->BeginTransaction()==KErrNone);

	if(table->GotoFirstRecord()==KErrNone)
		{
		// UnProtect the first record
		UnprotectRecordL(table,KErrNone);
		SetTestStepResult(EPass);		INFO_PRINTF1(_L("\t\tSUCCESS"));
		}
	else
		{
		SetTestStepResult(EFail);		INFO_PRINTF1(_L("\t\tFAIL"));
		}

	TESTL(iTheDb->CommitTransaction()==KErrNone);

	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

