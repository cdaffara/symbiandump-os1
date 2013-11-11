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
// This contains CommDb Unit Test Cases 037.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbtemp.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_037_xx.h"


CCommDbTest037_01::CCommDbTest037_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_037_01"));
	}

CCommDbTest037_01::~CCommDbTest037_01()
	{}

TVerdict CCommDbTest037_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest037_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest037_01::executeStepL()
	{
	//alter something in the template
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("DummyName"));
	TBufC8<KCommsDbSvrMaxFieldLength> firstInputString = _L8("first string");

	templateRecord->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING), firstInputString);
	User::LeaveIfError(templateRecord->StoreModifications());
	//Now start to overwrite initial change, but don't commit, cancel instead.
	User::LeaveIfError(templateRecord->Modify());

	TBufC8<KCommsDbSvrMaxFieldLength> secondInputString = _L8("second string");
	templateRecord->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING), secondInputString);
	templateRecord->CancelModifications();

	TBuf8<KCommsDbSvrMaxFieldLength> retrievedString;
	templateRecord->ReadTextL(TPtrC(MODEM_FAX_INIT_STRING), retrievedString);
	CleanupStack::PopAndDestroy(templateRecord);

	if(retrievedString!=firstInputString)
		return KErrGeneral;
	
	return KErrNone;
	}
	
TVerdict CCommDbTest037_01::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest037_02::CCommDbTest037_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_037_02"));
	}

CCommDbTest037_02::~CCommDbTest037_02()
	{}

TVerdict CCommDbTest037_02::doTestStepL( void )
	{
	executeStepL();
        SetTestStepResult(EPass);	return TestStepResult();	//If we get here we've passed
	}

TVerdict CCommDbTest037_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest037_02::executeStepL()
	{
	//alter something in the template
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(templateRecord);

	templateRecord->CancelModifications();
	
	CleanupStack::PopAndDestroy(templateRecord);

	return KErrNone;
	}
	
TVerdict CCommDbTest037_02::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest037_03::CCommDbTest037_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_037_03"));
	}

CCommDbTest037_03::~CCommDbTest037_03()
	{}

TVerdict CCommDbTest037_03::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 037.03 called "));

	SetTestStepResult(EPass);
	CCommDbTest037_01* step037_01 = new(ELeave) CCommDbTest037_01;
	CleanupStack::PushL(step037_01);
	doTestStepWithHeapFailureL( *step037_01, KErrNone);
	CleanupStack::PopAndDestroy(step037_01);

	CCommDbTest037_02* step037_02 = new(ELeave) CCommDbTest037_02;
	CleanupStack::PushL(step037_02);
	doTestStepWithHeapFailureL( *step037_02, KErrNone);
	CleanupStack::PopAndDestroy(step037_02);

	return TestStepResult();
	}
