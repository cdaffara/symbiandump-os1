// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements OOM voice call scenarios.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <mmretrieve.h>
#include <etelmm.h>
#include "Te_Loopbackcoomvoicecall.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

const TInt KETelServerShutdownGuardPeriod=500000;

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveOOMVoiceCall* CTestDriveOOMVoiceCall::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveOOMVoiceCall* aA = new(ELeave) CTestDriveOOMVoiceCall(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveOOMVoiceCall::CTestDriveOOMVoiceCall(const TScriptList aScriptListEntry) : 
	iScriptListEntry(aScriptListEntry)	
	{}

TInt CTestDriveOOMVoiceCall::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return StartEmulatorL();
	}

void CTestDriveOOMVoiceCall::CloseDown()
	{
	iPhone2.Close();
	iPhone.Close();
	iServer2.Close();
	iServer.Close();
	}

void CTestDriveOOMVoiceCall::OpenUpL()
	{
	TESTL(iServer.Connect()==KErrNone);
	TESTL(iServer.LoadPhoneModule(MODULE_NAME)==KErrNone);
	TESTL(iServer2.Connect()==KErrNone);
	TESTL(iPhone.Open(iServer, PHONE_NAME)==KErrNone);
	TESTL(iPhone2.Open(iServer, PHONE_NAME)==KErrNone);
	}

TInt CTestDriveOOMVoiceCall::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	CloseDown();
	User::After(KETelServerShutdownGuardPeriod);

	TInt n=1;
	for(;;n++)
		{
		INFO_PRINTF2(_L("Fail After %d allocs... "),n);
		TInt res=DoTestL(n);
		INFO_PRINTF2(_L("Result=%d"),res);
		if(res==KErrNone)
			break;
		RestartScript();
		User::After(KETelServerShutdownGuardPeriod);
		}

	OpenUpL();
	TerminateScript();
	return KErrNone;
	}

TInt CTestDriveOOMVoiceCall::DoTestL(TInt aAllocFailNumber)
	{
	_LIT(KPhoneModule,"MM");
	_LIT(KPhoneName,  "GsmPhone1");
	_LIT(KLineName,   "Voice");

	RTelServer telServer;
	TInt ret=telServer.Connect();
	if(ret!=KErrNone)
		return ret;

	telServer.__DbgFailNext(aAllocFailNumber);

	ret=telServer.LoadPhoneModule(KPhoneModule);
	if(ret!=KErrNone)
		{
		telServer.Close();
		return ret;
		}

	RMobilePhone mobilePhone;
	ret=mobilePhone.Open(telServer, KPhoneName);
	if(ret!=KErrNone)
		{
		mobilePhone.Close();
		telServer.Close();
		return ret;
		}

	RLine line;
	INFO_PRINTF1(_L("Opening Voice Line"));
	ret=line.Open(mobilePhone,KLineName);
	if(ret!=KErrNone)
		{
		line.Close();
		mobilePhone.Close();
		telServer.Close();
		return ret;
		}

	INFO_PRINTF1(_L("Opening New Voice Call"));
	RCall call;
	ret=call.OpenNewCall(line);
	if(ret!=KErrNone)
		{
		call.Close();
		line.Close();
		mobilePhone.Close();
		telServer.Close();
		return ret;
		}

	INFO_PRINTF1(_L("Initialise the Phone..."));
	
	TRequestStatus reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	mobilePhone.InitialiseMM(reqStatus, tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	if(reqStatus!=KErrNone)
		{
		call.Close();
		line.Close();
		mobilePhone.Close();
		telServer.Close();
		return ret;
		}

	INFO_PRINTF1(_L("Dial a number..."));
	_LIT(KDialString, "+1234");
	ret=call.Dial(KDialString);
	if(ret!=KErrNone)
		{
		call.Close();
		line.Close();
		mobilePhone.Close();
		telServer.Close();
		return ret;
		}

	INFO_PRINTF1(_L("Hangup a call..."));
	ret=call.HangUp();
	if(ret!=KErrNone)
		{
		call.Close();
		line.Close();
		mobilePhone.Close();
		telServer.Close();
		return ret;
		}

	INFO_PRINTF1(_L("Close the call, line & iPhone..."));
	call.Close();
	line.Close();
	mobilePhone.Close();

	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestOOMVoiceCall* CTestOOMVoiceCall::NewL(const TScript* aScript)
	{
	CTestOOMVoiceCall* aA = new(ELeave) CTestOOMVoiceCall(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestOOMVoiceCall::CTestOOMVoiceCall(const TScript* aScript) : iScript(aScript)
	{}

void CTestOOMVoiceCall::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestOOMVoiceCall::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestOOMVoiceCall::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestOOMVoiceCall::Complete(TInt aError)
	{
	iReturnValue = aError;
	CActiveScheduler::Stop();
	}
