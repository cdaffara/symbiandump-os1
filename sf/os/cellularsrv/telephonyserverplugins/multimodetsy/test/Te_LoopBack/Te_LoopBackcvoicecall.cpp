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
// This file implements voice call scenarios.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/


#include <e32test.h>
#include <mmretrieve.h>
#include <etelmm.h>
#include "Te_LoopBackcvoicecall.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint


//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveVoiceCall* CTestDriveVoiceCall::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveVoiceCall* aA = new(ELeave) CTestDriveVoiceCall(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveVoiceCall::CTestDriveVoiceCall(const TScriptList aScriptListEntry) : 
	iScriptListEntry(aScriptListEntry)	
	{}

TInt CTestDriveVoiceCall::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveVoiceCall::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	_LIT(KPhoneName, "GsmPhone1");
	_LIT(KLineName,  "Voice");

	RMobilePhone mobilePhone;
	TESTL(mobilePhone.Open(iServer, KPhoneName) == KErrNone);

	RLine line;
	INFO_PRINTF1(_L("Opening Voice Line\n"));
	TESTL(line.Open(iPhone,KLineName) == KErrNone);

	INFO_PRINTF1(_L("Opening New Voice Call\n"));
	RCall call;
	TESTL(call.OpenNewCall(line) == KErrNone);

	INFO_PRINTF1(_L("Initialise the Phone...\n"));

	TRequestStatus reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	INFO_PRINTF1(_L("Dial a number...\n"));
	_LIT(KDialString, "+1234");
	TESTL(call.Dial(KDialString) == KErrNone);

	INFO_PRINTF1(_L("Hangup a call...\n"));
	TESTL(call.HangUp() == KErrNone);

	// wait for an incoming call...
	INFO_PRINTF1(_L("Answer an Incoming call...\n"));
	TESTL(call.AnswerIncomingCall() == KErrNone);

	INFO_PRINTF1(_L("Hangup a call...\n"));
	TESTL(call.HangUp() == KErrNone);

	// repeat the test asking for the signal strength during each request
	INFO_PRINTF1(_L("Dial a number...\n"));
	TESTL(call.Dial(KDialString) == KErrNone);

	TRequestStatus requestStatus;
	TInt32         signalStrength;
	TInt8          bar;

	INFO_PRINTF1(_L("Retrieve Signal Strength...\n"));
	mobilePhone.GetSignalStrength(requestStatus, signalStrength, bar);
	User::WaitForRequest(requestStatus);
	TESTL(requestStatus == KErrNone);

	INFO_PRINTF1(_L("Hangup a call...\n"));
	TESTL(call.HangUp() == KErrNone);

	INFO_PRINTF1(_L("Retrieve Signal Strength...\n"));
	mobilePhone.GetSignalStrength(requestStatus, signalStrength, bar);
	User::WaitForRequest(requestStatus);
	TESTL(requestStatus == KErrNone);

    // wait for an incoming call...
	INFO_PRINTF1(_L("Answer an Incoming call...\n"));
	TESTL(call.AnswerIncomingCall() == KErrNone);

	INFO_PRINTF1(_L("Retrieve Signal Strength...\n"));
	mobilePhone.GetSignalStrength(requestStatus, signalStrength, bar);
	User::WaitForRequest(requestStatus);
	TESTL(requestStatus == KErrNone);

	INFO_PRINTF1(_L("Hangup a call...\n"));
	TESTL(call.HangUp() == KErrNone);

	INFO_PRINTF1(_L("Retrieve Signal Strength...\n"));
	mobilePhone.GetSignalStrength(requestStatus, signalStrength, bar);
	User::WaitForRequest(requestStatus);
	TESTL(requestStatus == KErrNone);

	INFO_PRINTF1(_L("Close the call, line & iPhone...\n"));
	call.Close();
	line.Close();
	mobilePhone.Close();

	return (KErrNone);
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestVoiceCall* CTestVoiceCall::NewL(const TScript* aScript)
	{
	CTestVoiceCall* aA = new(ELeave) CTestVoiceCall(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestVoiceCall::CTestVoiceCall(const TScript* aScript) : iScript(aScript)
	{}

void CTestVoiceCall::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestVoiceCall::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestVoiceCall::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestVoiceCall::Complete(TInt aError)
	{
	iReturnValue = aError;
	CActiveScheduler::Stop();
	}
