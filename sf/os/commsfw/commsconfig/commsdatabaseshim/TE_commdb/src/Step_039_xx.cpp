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
// This contains CommDb Unit Test Cases 039.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbtemp.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_039_xx.h"


CCommDbTest039_01::CCommDbTest039_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_039_01"));
	}

CCommDbTest039_01::~CCommDbTest039_01()
	{}

TVerdict CCommDbTest039_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest039_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest039_01::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(templateRecord);
	
	TInt ret(KErrNone);
	User::LeaveIfError(templateRecord->Modify());
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("DummyName"));
	User::LeaveIfError(templateRecord->StoreModifications());

	if(!templateRecord->TemplateRecordExists())
		ret = KErrGeneral;

	//delete the template
	User::LeaveIfError(templateRecord->Delete());

	if(templateRecord->TemplateRecordExists())
		ret = KErrGeneral;

	CleanupStack::PopAndDestroy(templateRecord);

	return ret;
	}
	
TVerdict CCommDbTest039_01::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest039_02::CCommDbTest039_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_039_02"));
	}

CCommDbTest039_02::~CCommDbTest039_02()
	{}

TVerdict CCommDbTest039_02::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 039.02 called "));

	SetTestStepResult(EPass);
	CCommDbTest039_01* step039_01 = new(ELeave) CCommDbTest039_01;
	CleanupStack::PushL(step039_01);
	doTestStepWithHeapFailureL( *step039_01, KErrNone);
	CleanupStack::PopAndDestroy(step039_01);

	return TestStepResult();
	}
