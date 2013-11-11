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
// This contains CommDb Unit Test Cases 044.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbstore.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_044_xx.h"


CCommDbTest044_01::CCommDbTest044_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_044_01"));
	}

CCommDbTest044_01::~CCommDbTest044_01()
	{}

TVerdict CCommDbTest044_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest044_01::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest044_01::executeStepL()
	{
	CStoreableOverrideSettings* overSet=CStoreableOverrideSettings::NewL(CStoreableOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet);

	TUint32 tempUint = 5000;
	User::LeaveIfError(overSet->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), tempUint));

	TBool tempBool = ETrue;
	User::LeaveIfError(overSet->SetBoolOverride(TPtrC(DIAL_OUT_ISP), TPtrC(SERVICE_IF_CALLBACK_ENABLED), tempBool));

	TBuf8<KCommsDbSvrMaxFieldLength> temp8String(_L8("Hi There!!"));
	User::LeaveIfError(overSet->SetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_CALLBACK_INFO), temp8String));
	
	TBuf<KCommsDbSvrMaxFieldLength> temp16String(_L("Hi There!!"));
	User::LeaveIfError(overSet->SetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_DEFAULT_TEL_NUM), temp16String));

	TBuf<KCommsDbSvrMaxFieldLength> tempLongString(_L("SEND \"hello\"\nWAIT 20\n{\n\"hello back\"\n"));	//Lots of stuff, nothing important
	User::LeaveIfError(overSet->SetLongDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_LOGIN_SCRIPT), tempLongString));

	CleanupStack::PopAndDestroy(overSet);

	return KErrNone;	//We've not left, so we pass
	}
	
TVerdict CCommDbTest044_01::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

///////////////////////////////////////////////////////////////////////////////////////

CCommDbTest044_02::CCommDbTest044_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_044_02"));
	}

CCommDbTest044_02::~CCommDbTest044_02()
	{}

TVerdict CCommDbTest044_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest044_02::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest044_02::executeStepL()
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

	//We've made an override object, now store it...

	CBufFlat* store = overSet->StoreLC();
	TPtr8 ptrToStore=store->Ptr(0);

	//Now restore into a new CStoreableOverrideSettings object
	CStoreableOverrideSettings* restoreOverSet=CStoreableOverrideSettings::NewL(CStoreableOverrideSettings::EParamListPartial);
	CleanupStack::PushL(restoreOverSet);

	HBufC8* restoredBuf=HBufC8::NewLC(ptrToStore.Length());
	TPtr8 ptrToBuf(restoredBuf->Des());
	ptrToBuf.Copy(ptrToStore);
	restoreOverSet->RestoreL(restoredBuf);
	
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

	CleanupStack::PopAndDestroy(restoredBuf);
	CleanupStack::PopAndDestroy(restoreOverSet);
	CleanupStack::PopAndDestroy(store);
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
	
TVerdict CCommDbTest044_02::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest044_03::CCommDbTest044_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_044_03"));
	}

CCommDbTest044_03::~CCommDbTest044_03()
	{}

TVerdict CCommDbTest044_03::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 044.03 called "));

	SetTestStepResult(EPass);
	CCommDbTest044_01* step044_01 = new(ELeave) CCommDbTest044_01;
	CleanupStack::PushL(step044_01);
	doTestStepWithHeapFailureL( *step044_01, KErrNone);
	CleanupStack::PopAndDestroy(step044_01);

	CCommDbTest044_02* step044_02 = new(ELeave) CCommDbTest044_02;
	CleanupStack::PushL(step044_02);
	doTestStepWithHeapFailureL( *step044_02, KErrNone);
	CleanupStack::PopAndDestroy(step044_02);

	return TestStepResult();
	}
