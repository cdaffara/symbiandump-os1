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
// This contains CommDb Unit Test Cases 040.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_041_xx.h"


CCommDbTest041_01::CCommDbTest041_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_01"));
	}

CCommDbTest041_01::~CCommDbTest041_01()
	{}

TVerdict CCommDbTest041_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_01::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_01::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TUint32 temp = 5000;
	User::LeaveIfError(iOverSet->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), temp));
	temp=22;	//Set temp to something different
	User::LeaveIfError(iOverSet->GetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), temp));

	if(temp!=5000)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest041_01::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_02::CCommDbTest041_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_02"));
	}

CCommDbTest041_02::~CCommDbTest041_02()
	{}

TVerdict CCommDbTest041_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_02::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_02::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TBool temp = ETrue;
	User::LeaveIfError(iOverSet->SetBoolOverride(TPtrC(DIAL_OUT_ISP), TPtrC(SERVICE_IF_CALLBACK_ENABLED), temp));
	temp=EFalse;
	User::LeaveIfError(iOverSet->GetBoolOverride(TPtrC(DIAL_OUT_ISP), TPtrC(SERVICE_IF_CALLBACK_ENABLED), temp));

	if(temp==EFalse)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest041_02::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_03::CCommDbTest041_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_03"));
	}

CCommDbTest041_03::~CCommDbTest041_03()
	{}

TVerdict CCommDbTest041_03::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_03::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_03::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TBuf8<KCommsDbSvrMaxFieldLength> temp(_L8("Hi There!!"));
	User::LeaveIfError(iOverSet->SetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_CALLBACK_INFO), temp));
	temp = _L8("Go Away!!");
	User::LeaveIfError(iOverSet->GetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_CALLBACK_INFO), temp));

	if(temp!=_L8("Hi There!!"))
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest041_03::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_04::CCommDbTest041_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_04"));
	}

CCommDbTest041_04::~CCommDbTest041_04()
	{}

TVerdict CCommDbTest041_04::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_04::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_04::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TBuf<KCommsDbSvrMaxFieldLength> temp(_L("Hi There!!"));
	User::LeaveIfError(iOverSet->SetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_DEFAULT_TEL_NUM), temp));
	temp = _L("Go Away!!");
	User::LeaveIfError(iOverSet->GetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_DEFAULT_TEL_NUM), temp));

	if(temp!=_L("Hi There!!"))
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest041_04::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_05::CCommDbTest041_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_05"));
	}

CCommDbTest041_05::~CCommDbTest041_05()
	{}

TVerdict CCommDbTest041_05::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_05::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_05::executeStepL()
	{
	_LIT(KLongString, "SEND \"hello\"\nWAIT 20\n{\n\"hello back\"\n");
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TBuf<KCommsDbSvrMaxFieldLength> temp(KLongString);	//Lots of stuff, nothing important
	User::LeaveIfError(iOverSet->SetLongDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_LOGIN_SCRIPT), temp));
	temp = _L("Go Away!!");
	User::LeaveIfError(iOverSet->GetLongDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_LOGIN_SCRIPT), temp));

	if(temp!=KLongString)
		return KErrGeneral;

	TInt stringLength;

	User::LeaveIfError(iOverSet->GetLongDesOverrideLength(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_LOGIN_SCRIPT), stringLength));

	if(stringLength!=TPtrC(KLongString).Length())
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest041_05::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_06::CCommDbTest041_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_06"));
	}

CCommDbTest041_06::~CCommDbTest041_06()
	{}

TVerdict CCommDbTest041_06::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_06::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_06::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);

	TUint32 tempUint = 5000;
	User::LeaveIfError(iOverSet->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), tempUint));
	if(!iOverSet->IsOverridden(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), CCommDbOverrideSettings::EIntValue))
		return KErrGeneral;

	TBool tempBool = ETrue;
	User::LeaveIfError(iOverSet->SetBoolOverride(TPtrC(DIAL_OUT_ISP), TPtrC(SERVICE_IF_CALLBACK_ENABLED), tempBool));
	if(!iOverSet->IsOverridden(TPtrC(DIAL_OUT_ISP), TPtrC(SERVICE_IF_CALLBACK_ENABLED), CCommDbOverrideSettings::EBoolValue))
		return KErrGeneral;

	TBuf8<KCommsDbSvrMaxFieldLength> temp8String(_L8("Hi There!!"));
	User::LeaveIfError(iOverSet->SetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_CALLBACK_INFO), temp8String));
	if(!iOverSet->IsOverridden(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IF_CALLBACK_INFO), CCommDbOverrideSettings::EDes8Value))
		return KErrGeneral;
	
	TBuf<KCommsDbSvrMaxFieldLength> temp16String(_L("Hi There!!"));
	User::LeaveIfError(iOverSet->SetDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_DEFAULT_TEL_NUM), temp16String));
	if(!iOverSet->IsOverridden(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_DEFAULT_TEL_NUM), CCommDbOverrideSettings::EDes16Value))
		return KErrGeneral;

	TBuf<KCommsDbSvrMaxFieldLength> tempLongString(_L("SEND \"hello\"\nWAIT 20\n{\n\"hello back\"\n"));	//Lots of stuff, nothing important
	User::LeaveIfError(iOverSet->SetLongDesOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_LOGIN_SCRIPT), tempLongString));
	if(!iOverSet->IsOverridden(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_LOGIN_SCRIPT), CCommDbOverrideSettings::ELongDesValue))
		return KErrGeneral;

	return KErrNone;
	}

TVerdict CCommDbTest041_06::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_07::CCommDbTest041_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_07"));
	}

CCommDbTest041_07::~CCommDbTest041_07()
	{}

TVerdict CCommDbTest041_07::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_07::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_07::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);

	return iOverSet->SetIntOverride(TPtrC(DIAL_OUT_ISP), _L("Where is the Column"), 55);
	}

TVerdict CCommDbTest041_07::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_08::CCommDbTest041_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_08"));
	}

CCommDbTest041_08::~CCommDbTest041_08()
	{}

TVerdict CCommDbTest041_08::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_08::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_08::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);

	return iOverSet->SetIntOverride(_L("WhatTable"), TPtrC(ISP_IWF_TO_MS), 55);
	}

TVerdict CCommDbTest041_08::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_09::CCommDbTest041_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_09"));
	}

CCommDbTest041_09::~CCommDbTest041_09()
	{}

TVerdict CCommDbTest041_09::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotSupported)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_09::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_09::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TBuf<KCommsDbSvrMaxFieldLength> temp(_L("Hi There!!"));
	return iOverSet->SetDesOverride(TPtrC(PROXIES), TPtrC(PROXY_SERVICE_TYPE), temp);
	}

TVerdict CCommDbTest041_09::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_10::CCommDbTest041_10()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_10"));
	}

CCommDbTest041_10::~CCommDbTest041_10()
	{}

TVerdict CCommDbTest041_10::doTestStepL( void )
	{
	if(executeStepL()!=KErrAlreadyExists)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_10::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_10::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	User::LeaveIfError(iOverSet->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), 5000));
	return iOverSet->SetIntOverride(TPtrC(DIAL_OUT_ISP), TPtrC(ISP_IWF_TO_MS), 5000);
	}

TVerdict CCommDbTest041_10::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_11::CCommDbTest041_11()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_11"));
	}

CCommDbTest041_11::~CCommDbTest041_11()
	{}

TVerdict CCommDbTest041_11::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_11::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_11::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TBuf<KCommsDbSvrMaxFieldLength> temp;
	return iOverSet->GetDesOverride(TPtrC(PROXIES), TPtrC(PROXY_SERVICE_TYPE), temp);
	}

TVerdict CCommDbTest041_11::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_12::CCommDbTest041_12()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_12"));
	}

CCommDbTest041_12::~CCommDbTest041_12()
	{}

TVerdict CCommDbTest041_12::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_12::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_12::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListFull);
	if (iOverSet->PartialFull() != CCommDbOverrideSettings::EParamListFull)
		return KErrGeneral;

	delete iOverSet;
	iOverSet = NULL;

	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	if (iOverSet->PartialFull() != CCommDbOverrideSettings::EParamListPartial)
		return KErrGeneral;

	return KErrNone;
	}

TVerdict CCommDbTest041_12::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_13::CCommDbTest041_13()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_13"));
	}

CCommDbTest041_13::~CCommDbTest041_13()
	{}

TVerdict CCommDbTest041_13::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_13::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_13::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TBuf<KCommsDbSvrMaxFieldLength> temp(_L("There's no place like home"));
	return iOverSet->SetDesOverride(TPtrC(MODEM_BEARER), TPtrC(MODEM_TSY_NAME), temp);
	}

TVerdict CCommDbTest041_13::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_14::CCommDbTest041_14()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_14"));
	}

CCommDbTest041_14::~CCommDbTest041_14()
	{}

TVerdict CCommDbTest041_14::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotSupported)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_14::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_14::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	
	return iOverSet->SetIntOverride(TPtrC(MODEM_BEARER), TPtrC(MODEM_STOP_BITS), 10);
	}

TVerdict CCommDbTest041_14::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_15::CCommDbTest041_15()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_15"));
	}

CCommDbTest041_15::~CCommDbTest041_15()
	{}

TVerdict CCommDbTest041_15::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_15::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_15::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TBuf<KCommsDbSvrMaxFieldLength> temp(_L("There's no place like home"));
	if(KErrNone==iOverSet->SetIntOverride(TPtrC(PROXIES), TPtrC(PROXY_ISP), 0))
		return KErrGeneral;
	if(KErrNone==iOverSet->SetDesOverride(TPtrC(PROXIES), TPtrC(PROXY_SERVICE_TYPE), temp))
		return KErrGeneral;
	if(KErrNone==iOverSet->SetBoolOverride(TPtrC(PROXIES), TPtrC(PROXY_SERVICE_TYPE), ETrue))
		return KErrGeneral;
	if(KErrNone==iOverSet->SetLongDesOverride(TPtrC(PROXIES), TPtrC(PROXY_SERVER_NAME), temp))
		return KErrGeneral;
	if(KErrNone==iOverSet->SetDesOverride(TPtrC(PROXIES), TPtrC(PROXY_PROTOCOL_NAME), temp))
		return KErrGeneral;
	if(KErrNone==iOverSet->SetIntOverride(TPtrC(PROXIES), TPtrC(PROXY_PORT_NUMBER), 22))
		return KErrGeneral;
	if(KErrNone==iOverSet->SetLongDesOverride(TPtrC(PROXIES), TPtrC(PROXY_EXCEPTIONS), temp))
		return KErrGeneral;
	return KErrNone;
	}

TVerdict CCommDbTest041_15::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_16::CCommDbTest041_16()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_16"));
	}

CCommDbTest041_16::~CCommDbTest041_16()
	{}

TVerdict CCommDbTest041_16::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotSupported)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_16::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_16::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TInt err;
	TBuf<KCommsDbSvrMaxFieldLength> temp(_L("There's no place like home"));
	if(KErrNotSupported!=(err=iOverSet->SetDesOverride(TPtrC(WAP_ACCESS_POINT), TPtrC(WAP_CURRENT_BEARER), temp)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetLongDesOverride(TPtrC(WAP_ACCESS_POINT), TPtrC(WAP_START_PAGE), temp)))
		return err;
	return KErrNotSupported;
	}

TVerdict CCommDbTest041_16::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////////////////

CCommDbTest041_17::CCommDbTest041_17()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_17"));
	}

CCommDbTest041_17::~CCommDbTest041_17()
	{}

TVerdict CCommDbTest041_17::doTestStepL( void )
	{
	if(executeStepL()!=KErrNotSupported)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest041_17::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest041_17::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	TInt err;
	TBuf<KCommsDbSvrMaxFieldLength> temp(_L("There's no place like home"));
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_USAGE), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetDesOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_APN), temp)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_PDP_TYPE), 0)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetDesOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_PDP_ADDRESS), temp)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_PRECEDENCE), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_DELAY), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_RELIABILITY), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_PEAK_THROUGHPUT), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_MEAN_THROUGHPUT), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_MIN_PRECEDENCE), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_MIN_DELAY), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_MIN_RELIABILITY), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetIntOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_MIN_PEAK_THROUGHPUT), 1)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetBoolOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_DATA_COMPRESSION), ETrue)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetBoolOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_HEADER_COMPRESSION), ETrue)))
		return err;
	if(KErrNotSupported!=(err=iOverSet->SetBoolOverride(TPtrC(DEFAULT_GPRS), TPtrC(DEFAULT_GPRS_ANONYMOUS_ACCESS), ETrue)))
		return err;

	return KErrNotSupported;
	}

TVerdict CCommDbTest041_17::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest041_18::CCommDbTest041_18()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_18"));
	}

CCommDbTest041_18::~CCommDbTest041_18()
	{}

TVerdict CCommDbTest041_18::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.18 called "));

	SetTestStepResult(EPass);
	CCommDbTest041_01* step041_01 = new(ELeave) CCommDbTest041_01;
	CleanupStack::PushL(step041_01);
	doTestStepWithHeapFailureL( *step041_01, KErrNone);
	CleanupStack::PopAndDestroy(step041_01);

	return TestStepResult();
	}



//////////////////////////////////////////////////////////////////////////

CCommDbTest041_19::CCommDbTest041_19()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_19"));
	}

CCommDbTest041_19::~CCommDbTest041_19()
	{}

TVerdict CCommDbTest041_19::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.19 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_02* step041_02 = new(ELeave) CCommDbTest041_02;
	CleanupStack::PushL(step041_02);
	doTestStepWithHeapFailureL( *step041_02, KErrNone);
	CleanupStack::PopAndDestroy(step041_02);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_20::CCommDbTest041_20()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_20"));
	}

CCommDbTest041_20::~CCommDbTest041_20()
	{}

TVerdict CCommDbTest041_20::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.20 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_03* step041_03 = new(ELeave) CCommDbTest041_03;
	CleanupStack::PushL(step041_03);
	doTestStepWithHeapFailureL( *step041_03, KErrNone);
	CleanupStack::PopAndDestroy(step041_03);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_21::CCommDbTest041_21()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_21"));
	}

CCommDbTest041_21::~CCommDbTest041_21()
	{}

TVerdict CCommDbTest041_21::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.21 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_04* step041_04 = new(ELeave) CCommDbTest041_04;
	CleanupStack::PushL(step041_04);
	doTestStepWithHeapFailureL( *step041_04, KErrNone);
	CleanupStack::PopAndDestroy(step041_04);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_22::CCommDbTest041_22()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_22"));
	}

CCommDbTest041_22::~CCommDbTest041_22()
	{}

TVerdict CCommDbTest041_22::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.22 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_05* step041_05 = new(ELeave) CCommDbTest041_05;
	CleanupStack::PushL(step041_05);
	doTestStepWithHeapFailureL( *step041_05, KErrNone);
	CleanupStack::PopAndDestroy(step041_05);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_23::CCommDbTest041_23()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_23"));
	}

CCommDbTest041_23::~CCommDbTest041_23()
	{}

TVerdict CCommDbTest041_23::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.23 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_06* step041_06 = new(ELeave) CCommDbTest041_06;
	CleanupStack::PushL(step041_06);
	doTestStepWithHeapFailureL( *step041_06, KErrNone);
	CleanupStack::PopAndDestroy(step041_06);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_24::CCommDbTest041_24()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_24"));
	}

CCommDbTest041_24::~CCommDbTest041_24()
	{}

TVerdict CCommDbTest041_24::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.24 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_07* step041_07 = new(ELeave) CCommDbTest041_07;
	CleanupStack::PushL(step041_07);
	doTestStepWithHeapFailureL( *step041_07, KErrNotFound);
	CleanupStack::PopAndDestroy(step041_07);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_25::CCommDbTest041_25()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_25"));
	}

CCommDbTest041_25::~CCommDbTest041_25()
	{}

TVerdict CCommDbTest041_25::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.25 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_08* step041_08 = new(ELeave) CCommDbTest041_08;
	CleanupStack::PushL(step041_08);
	doTestStepWithHeapFailureL( *step041_08, KErrNotFound);
	CleanupStack::PopAndDestroy(step041_08);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_26::CCommDbTest041_26()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_26"));
	}

CCommDbTest041_26::~CCommDbTest041_26()
	{}

TVerdict CCommDbTest041_26::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.26 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_09* step041_09 = new(ELeave) CCommDbTest041_09;
	CleanupStack::PushL(step041_09);
	doTestStepWithHeapFailureL( *step041_09, KErrNotSupported);
	CleanupStack::PopAndDestroy(step041_09);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_27::CCommDbTest041_27()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_27"));
	}

CCommDbTest041_27::~CCommDbTest041_27()
	{}

TVerdict CCommDbTest041_27::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.27 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_10* step041_10 = new(ELeave) CCommDbTest041_10;
	CleanupStack::PushL(step041_10);
	doTestStepWithHeapFailureL( *step041_10, KErrAlreadyExists);
	CleanupStack::PopAndDestroy(step041_10);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_28::CCommDbTest041_28()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_28"));
	}

CCommDbTest041_28::~CCommDbTest041_28()
	{}

TVerdict CCommDbTest041_28::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.28 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_11* step041_11 = new(ELeave) CCommDbTest041_11;
	CleanupStack::PushL(step041_11);
	doTestStepWithHeapFailureL( *step041_11, KErrNotFound);
	CleanupStack::PopAndDestroy(step041_11);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_29::CCommDbTest041_29()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_29"));
	}

CCommDbTest041_29::~CCommDbTest041_29()
	{}

TVerdict CCommDbTest041_29::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.29 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_12* step041_12 = new(ELeave) CCommDbTest041_12;
	CleanupStack::PushL(step041_12);
	doTestStepWithHeapFailureL( *step041_12, KErrNone);
	CleanupStack::PopAndDestroy(step041_12);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_30::CCommDbTest041_30()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_30"));
	}

CCommDbTest041_30::~CCommDbTest041_30()
	{}

TVerdict CCommDbTest041_30::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.30 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_13* step041_13 = new(ELeave) CCommDbTest041_13;
	CleanupStack::PushL(step041_13);
	doTestStepWithHeapFailureL( *step041_13, KErrNone);
	CleanupStack::PopAndDestroy(step041_13);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_31::CCommDbTest041_31()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_31"));
	}

CCommDbTest041_31::~CCommDbTest041_31()
	{}

TVerdict CCommDbTest041_31::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.31 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_14* step041_14 = new(ELeave) CCommDbTest041_14;
	CleanupStack::PushL(step041_14);
	doTestStepWithHeapFailureL( *step041_14, KErrNotSupported);
	CleanupStack::PopAndDestroy(step041_14);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_32::CCommDbTest041_32()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_32"));
	}

CCommDbTest041_32::~CCommDbTest041_32()
	{}

TVerdict CCommDbTest041_32::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.32 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_15* step041_15 = new(ELeave) CCommDbTest041_15;
	CleanupStack::PushL(step041_15);
	doTestStepWithHeapFailureL( *step041_15, KErrNone);
	CleanupStack::PopAndDestroy(step041_15);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_33::CCommDbTest041_33()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_33"));
	}

CCommDbTest041_33::~CCommDbTest041_33()
	{}

TVerdict CCommDbTest041_33::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.33 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_16* step041_16 = new(ELeave) CCommDbTest041_16;
	CleanupStack::PushL(step041_16);
	doTestStepWithHeapFailureL( *step041_16, KErrNotSupported);
	CleanupStack::PopAndDestroy(step041_16);
	
	return TestStepResult();
	}
	
//////////////////////////////////////////////////////////////////////////

CCommDbTest041_34::CCommDbTest041_34()
	{
	// store the name of this test case
	SetTestStepName(_L("step_041_34"));
	}

CCommDbTest041_34::~CCommDbTest041_34()
	{}

TVerdict CCommDbTest041_34::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 041.34 called "));

	SetTestStepResult(EPass);	
	CCommDbTest041_17* step041_17 = new(ELeave) CCommDbTest041_17;
	CleanupStack::PushL(step041_17);
	doTestStepWithHeapFailureL( *step041_17, KErrNotSupported);
	CleanupStack::PopAndDestroy(step041_17);
	
	return TestStepResult();
	}

//EOF
