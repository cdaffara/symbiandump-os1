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
// This file implements various tests for incoming call handling.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <mmretrieve.h>
#include <etelmm.h>
#include "Te_LoopBackcincall.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveInCall* CTestDriveInCall::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveInCall* aA = new (ELeave) CTestDriveInCall(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveInCall::CTestDriveInCall(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveInCall::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return (StartEmulatorL());
	}

TInt CTestDriveInCall::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	_LIT(KVoiceLineName, "Voice");
	_LIT(KDataLineName,  "Data");
	//_LIT(KFaxLineName,   "Fax");

	RLine voiceLine;
	INFO_PRINTF1(_L("Opening Voice Line\n"));
	TESTL(voiceLine.Open(iPhone,KVoiceLineName) == KErrNone);
	RCall voiceCall;
	INFO_PRINTF1(_L("Opening New Voice Call\n"));
	TESTL(voiceCall.OpenNewCall(voiceLine) == KErrNone);

	RLine dataLine;
	INFO_PRINTF1(_L("Opening Data Line\n"));
	TESTL(dataLine.Open(iPhone,KDataLineName) == KErrNone);
	RCall dataCall;
	INFO_PRINTF1(_L("Opening New Data Call\n"));
	TESTL(dataCall.OpenNewCall(dataLine) == KErrNone);

	TRequestStatus stat1, stat2, stat3, reqStatus;

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

// Now wait for an incoming voice call...
	INFO_PRINTF1(_L("Answering a Voice Call...\n"));
	voiceCall.AnswerIncomingCall(stat1);
	dataCall.AnswerIncomingCall(stat2);
	User::WaitForRequest(stat1);
	TESTL(stat1 == KErrNone);
	dataCall.AnswerIncomingCallCancel();
	User::WaitForRequest(stat2);
	User::After(1000000L);
	INFO_PRINTF1(_L("Hangup a Voice Call...\n"));
	TESTL(voiceCall.HangUp() == KErrNone);

// Now wait for an incoming data call...
	INFO_PRINTF1(_L("Answering a Data Call\n"));
	TInt ret = dataCall.AnswerIncomingCall();
	INFO_PRINTF2(_L("ret=%d "),ret);
	if (ret == KErrNone)
	    {
	    SetTestStepResult(EPass);
	    }
	else
	    SetTestStepResult(EFail);
	    
	 User::After(300000L);
	TESTL(dataCall.HangUp() == KErrNone);

//	Get the FaxSettings before closing the line and call
	
	dataCall.Close();
	dataLine.Close();
	voiceCall.Close();
	voiceLine.Close();

	return (KErrNone);
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestInCall* CTestInCall::NewL(const TScript* aScript)
	{
	CTestInCall* aA = new (ELeave) CTestInCall(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestInCall::CTestInCall(const TScript* aScript) :
	iScript(aScript)
	{}

void CTestInCall::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestInCall::Start()
	{
	StartScript(iScript);
	return (KErrNone);
	}

void CTestInCall::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestInCall::Complete(TInt aError)
	{
	iReturnValue = aError;
	CActiveScheduler::Stop();
	}
