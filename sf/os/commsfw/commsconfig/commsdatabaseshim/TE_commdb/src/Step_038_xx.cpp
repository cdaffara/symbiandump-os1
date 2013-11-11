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
// This contains CommDb Unit Test Cases 038.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbtemp.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_038_xx.h"



CCommDbTest038_01::CCommDbTest038_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_038_01"));
	}

CCommDbTest038_01::~CCommDbTest038_01()
	{}

TVerdict CCommDbTest038_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest038_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest038_01::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(templateRecord);
	User::LeaveIfError(	templateRecord->Modify());
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("DummyName"));
	User::LeaveIfError(templateRecord->StoreModifications());

	//delete the template
	User::LeaveIfError(templateRecord->Delete());

	TInt ret(KErrNone);

	if(templateRecord->TemplateRecordExists())
		ret = KErrGeneral;

	CleanupStack::PopAndDestroy(templateRecord);

	return ret;
	}
	
TVerdict CCommDbTest038_01::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

///////////////////////////////////////////////////////////////////////

CCommDbTest038_02::CCommDbTest038_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_038_02"));
	}

CCommDbTest038_02::~CCommDbTest038_02()
	{}

TVerdict CCommDbTest038_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest038_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest038_02::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(templateRecord);

	User::LeaveIfError(	templateRecord->Modify());
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("DummyName"));
	User::LeaveIfError(templateRecord->StoreModifications());

	//delete the template
	User::LeaveIfError(templateRecord->Delete());
	
	TInt ret = templateRecord->Delete();

	CleanupStack::PopAndDestroy(templateRecord);

	return ret;
	}
	
TVerdict CCommDbTest038_02::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest038_03::CCommDbTest038_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_038_03"));
	}

CCommDbTest038_03::~CCommDbTest038_03()
	{}

TVerdict CCommDbTest038_03::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 038.03 called "));

	SetTestStepResult(EPass);
	CCommDbTest038_01* step038_01 = new(ELeave) CCommDbTest038_01;
	CleanupStack::PushL(step038_01);
	doTestStepWithHeapFailureL( *step038_01, KErrNone);
	CleanupStack::PopAndDestroy(step038_01);

	CCommDbTest038_02* step038_02 = new(ELeave) CCommDbTest038_02;
	CleanupStack::PushL(step038_02);
	doTestStepWithHeapFailureL( *step038_02, KErrNotFound);
	CleanupStack::PopAndDestroy(step038_02);

	return TestStepResult();
	}
