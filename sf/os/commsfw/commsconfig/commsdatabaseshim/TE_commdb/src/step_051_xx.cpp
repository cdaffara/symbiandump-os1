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

#include "step_051_xx.h"


/////////////////////
//	Test step 051.01
/////////////////////

CCommDbTest051_01::CCommDbTest051_01()
{
	// store the name of this test case
	SetTestStepName(_L("step_051_01"));

}

CCommDbTest051_01::~CCommDbTest051_01()
{
	delete iTheDb;
}


TVerdict CCommDbTest051_01::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

//protect normal table
enum TVerdict CCommDbTest051_01::doTestStepL( void )
	{

	INFO_PRINTF1(_L("Normal Table : Protection.. "));
	SetTestStepResult(EPass);	CCommsDbProtectTableView* table = (CCommsDbProtectTableView *)iTheDb->OpenTableLC(TPtrC(DIAL_OUT_ISP));

	if(table->GotoFirstRecord()==KErrNone)
		{
		TESTL(table->ProtectTable()==KErrNone);
		SetTestStepResult(EPass);		INFO_PRINTF1(_L("\t\tSUCCESS"));
		}
	else
		{
		SetTestStepResult(EFail);		INFO_PRINTF1(_L("FAIL"));
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}

/////////////////////
//	Test step 051.02
/////////////////////

CCommDbTest051_02::CCommDbTest051_02()
{
	// store the name of this test case
	SetTestStepName(_L("step_051_02"));

}

CCommDbTest051_02::~CCommDbTest051_02()
{
	delete iTheDb;
}

TVerdict CCommDbTest051_02::doTestStepPreambleL()
	{	
	iTheDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        SetTestStepResult(EPass);	return TestStepResult();
	}

//Unprotect Normal Table
enum TVerdict CCommDbTest051_02::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Normal Table : Unprotection .."));
	SetTestStepResult(EPass);	CCommsDbProtectTableView* table = (CCommsDbProtectTableView *)iTheDb->OpenTableLC(TPtrC(DIAL_OUT_ISP));
	if(table->GotoFirstRecord()==KErrNone)
		{
		TESTL(table->UnprotectTable()==KErrNone);
		SetTestStepResult(EPass);		INFO_PRINTF1(_L("\t\tSUCCESS"));
		}
	else
		{
		SetTestStepResult(EFail);		INFO_PRINTF1(_L("FAIL"));
		}
	CleanupStack::PopAndDestroy(); 
	return TestStepResult();
	}
