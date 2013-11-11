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
// This contains CommDb Unit Test Cases 045.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbstore.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_045_xx.h"


CCommDbTest045_01::CCommDbTest045_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_045_01"));
	}

CCommDbTest045_01::~CCommDbTest045_01()
	{}

TVerdict CCommDbTest045_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest045_01::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest045_01::executeStepL()
	{
	CStoreableOverrideSettings* overSet=CStoreableOverrideSettings::NewL(CStoreableOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet);

	TUint32 setUint = 5000;
	User::LeaveIfError(overSet->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), setUint));

	TBool setBool = ETrue;
	User::LeaveIfError(overSet->SetBoolOverride(TPtrC(DIAL_OUT_ISP), TPtrC(SERVICE_IF_CALLBACK_ENABLED), setBool));

	TBuf8<KCommsDbSvrMaxFieldLength> set8String(_L8("Hi There!!"));
	User::LeaveIfError(overSet->SetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_CALLBACK_INFO), set8String));
	
	TBuf<KCommsDbSvrMaxFieldLength> set16String(_L("Hi There!!"));
	User::LeaveIfError(overSet->SetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_DEFAULT_TEL_NUM), set16String));

	TBuf<KCommsDbSvrMaxFieldLength> setLongString(_L("SEND \"hello\"\nWAIT 20\n{\n\"hello back\"\n"));	//Lots of stuff, nothing important
	User::LeaveIfError(overSet->SetLongDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_LOGIN_SCRIPT), setLongString));

	HBufC8* storeBuf=HBufC8::NewLC(1024);	//Create a buf on the heap to use as the store
	TPtr8 storePtr=storeBuf->Des();

	RDesWriteStream writeStream;
	writeStream.Open(storePtr);
	writeStream.PushL();
	
	overSet->ExternalizeL(writeStream);
	writeStream.Close();
	CleanupStack::Pop();	//writeStream

	//Settings have been stored to stream (storeBuf),
	//now read them into a new settings object and check them

	CStoreableOverrideSettings* restoreOverSet=CStoreableOverrideSettings::NewL(CStoreableOverrideSettings::EParamListPartial);
	CleanupStack::PushL(restoreOverSet);

	RDesReadStream readStream;
	readStream.Open(storePtr);
	readStream.PushL();

	restoreOverSet->InternalizeL(readStream);
	readStream.Close();
	CleanupStack::Pop();	//readStream

	//Check we've got the same stuff in the restored object as in the original
	TUint32 restoredUint;
	TBool restoredBool;
	TBuf8<KCommsDbSvrMaxFieldLength> restored8String;
	TBuf<KCommsDbSvrMaxFieldLength> restored16String;
	TBuf<KCommsDbSvrMaxFieldLength> restoredLongString;

	User::LeaveIfError(restoreOverSet->GetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), restoredUint));
	User::LeaveIfError(restoreOverSet->GetBoolOverride(TPtrC(DIAL_OUT_ISP), TPtrC(SERVICE_IF_CALLBACK_ENABLED), restoredBool));
	User::LeaveIfError(restoreOverSet->GetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_CALLBACK_INFO), restored8String));
	User::LeaveIfError(restoreOverSet->GetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_DEFAULT_TEL_NUM), restored16String));
	User::LeaveIfError(restoreOverSet->GetLongDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_LOGIN_SCRIPT), restoredLongString));

	CleanupStack::PopAndDestroy(restoreOverSet);
	CleanupStack::PopAndDestroy(storeBuf);
	CleanupStack::PopAndDestroy(overSet);

	//Now do a comparison
	if(restoredUint!=setUint
		||restoredBool!=setBool
		||restored8String!=set8String
		||restored16String!=set16String
		||restoredLongString!=setLongString)
		return KErrGeneral;


	return KErrNone;
	}
	
TVerdict CCommDbTest045_01::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////////////////////////////////////////////

CCommDbTest045_02::CCommDbTest045_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_045_02"));
	}

CCommDbTest045_02::~CCommDbTest045_02()
	{}

TVerdict CCommDbTest045_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrEof)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest045_02::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest045_02::executeStepL()
	{
	HBufC8* storeBuf=HBufC8::NewLC(1024);	//Create a buf on the heap to use as the store
	TPtr8 storePtr=storeBuf->Des();

	//Try and restore from a blank stream

	CStoreableOverrideSettings* restoreOverSet=CStoreableOverrideSettings::NewL(CStoreableOverrideSettings::EParamListPartial);
	CleanupStack::PushL(restoreOverSet);

	RDesReadStream readStream;
	storePtr=storeBuf->Des();
	readStream.Open(storePtr);
	readStream.PushL();

	TRAPD(err, restoreOverSet->InternalizeL(readStream));
	readStream.Close();
	CleanupStack::Pop();	//readStream


	CleanupStack::PopAndDestroy(restoreOverSet);
	CleanupStack::PopAndDestroy(storeBuf);

	return err;
	}
	
TVerdict CCommDbTest045_02::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest045_03::CCommDbTest045_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_045_03"));
	}

CCommDbTest045_03::~CCommDbTest045_03()
	{}

TVerdict CCommDbTest045_03::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 045.03 called "));

	SetTestStepResult(EPass);
	CCommDbTest045_01* step045_01 = new(ELeave) CCommDbTest045_01;
	CleanupStack::PushL(step045_01);
	doTestStepWithHeapFailureL( *step045_01, KErrNone);
	CleanupStack::PopAndDestroy(step045_01);

	CCommDbTest045_02* step045_02 = new(ELeave) CCommDbTest045_02;
	CleanupStack::PushL(step045_02);
	doTestStepWithHeapFailureL( *step045_02, KErrEof);
	CleanupStack::PopAndDestroy(step045_02);

	return TestStepResult();
	}
