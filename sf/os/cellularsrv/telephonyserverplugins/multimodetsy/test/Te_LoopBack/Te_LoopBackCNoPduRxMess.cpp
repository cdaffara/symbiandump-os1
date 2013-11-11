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
// Test Rx SMS Request Failure when PDU Mode is not supported by the mobile
// This file implements Receive SMS Request failure tests when PDU mode is not 
// supported by the mobile iPhone.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <mmretrieve.h>
#include <etelmm.h>
#include "Te_LoopBackCNoPduRxMess.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint


//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveNoPduRxMess* CTestDriveNoPduRxMess::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveNoPduRxMess* aA = new (ELeave) CTestDriveNoPduRxMess(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveNoPduRxMess::CTestDriveNoPduRxMess(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

CTestDriveNoPduRxMess::~CTestDriveNoPduRxMess()
	{}

TInt CTestDriveNoPduRxMess::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveNoPduRxMess::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	INFO_PRINTF1(_L("Test for a Receive SMS Request Failure"));
	INFO_PRINTF1(_L("when PDU mode is not supported by the"));
	INFO_PRINTF1(_L("mobile iPhone\n"));

	INFO_PRINTF1(_L("Initializing the Phone...\n"));
	TRequestStatus reqStatus;

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	INFO_PRINTF1(_L("Opening Mobile SMS Messaging..."));
	RMobileSmsMessaging sms;
	TESTL(sms.Open(iPhone) == KErrNone);

	INFO_PRINTF1(_L("Reading SMS Message..."));
	
	sms.SetReceiveMode (reqStatus, RMobileSmsMessaging::EReceiveModeUnspecified);

	INFO_PRINTF1(_L("Verifying the Expected SMS Failure..."));
	User::WaitForRequest(reqStatus);
	INFO_PRINTF2(_L("  reqStatus = %d"), reqStatus.Int());
	TESTL(reqStatus == KErrNotSupported);

	INFO_PRINTF1(_L("Closing SMS Messaging..."));
	sms.Close();
	return (KErrNone);
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestNoPduRxMess* CTestNoPduRxMess::NewL(const TScript* aScript)
	{
	CTestNoPduRxMess* aA = new (ELeave) CTestNoPduRxMess(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestNoPduRxMess::CTestNoPduRxMess(const TScript* aScript) :
	iScript(aScript)
	{}

void CTestNoPduRxMess::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

CTestNoPduRxMess::~CTestNoPduRxMess()
	{}

TInt CTestNoPduRxMess::Start()
	{
	StartScript(iScript);
	return (KErrNone);
	}

void CTestNoPduRxMess::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestNoPduRxMess::Complete(TInt aError)
	{
	iReturnValue = aError;
	CActiveScheduler::Stop();
	}
