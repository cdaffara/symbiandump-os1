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
// This file implements cancelling of SMS operations test scenarios.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <etelmm.h>
#include "Te_LoopBackcsmscancel.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint


//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveSmsCancel* CTestDriveSmsCancel::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveSmsCancel* aA=new(ELeave) CTestDriveSmsCancel(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveSmsCancel::CTestDriveSmsCancel(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveSmsCancel::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveSmsCancel::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	RMobileSmsMessaging sms;
	TRequestStatus stat0, stat1;

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(stat0, tableSettingsPckg); 	
	User::WaitForRequest(stat0);
	TESTL(stat0 == KErrNone);

	INFO_PRINTF1(_L("Opening Sms Messaging\n"));
	TESTL(sms.Open(iPhone)==KErrNone);


	if (iScriptListEntry == ESmsCancelScript)  
		{
		INFO_PRINTF1(_L("Set receive mode to EReceiveUnstoredPhoneAck.\n"));
		sms.SetReceiveMode (stat0, RMobileSmsMessaging::EReceiveUnstoredPhoneAck);
		}
	else if (iScriptListEntry == ESmsCancelCmtiScript)  
		{
		INFO_PRINTF1(_L("Set receive mode to EReceiveStored.\n"));
		sms.SetReceiveMode (stat0, RMobileSmsMessaging::EReceiveStored);
		}
	else
		{
			ERR_PRINTF1(_L("Error: Unknown script id.\n"));
			User::Leave(KErrGeneral) ;
		}
	User::WaitForRequest(stat0);
	TESTL(stat0 == KErrNone);

	// Which pdu size to use? Find out from caps of iPhone - GSM/WCDMA 
	RMobileSmsMessaging::TMobileSmsCapsV1 caps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg capsPkg(caps);
	sms.GetCaps (capsPkg);
	caps = capsPkg();
	if (caps.iSmsMode & RMobileSmsMessaging::KCapsGsmSms)
		{
		INFO_PRINTF1(_L("Phone supports GSM/WCDMA mode.\n"));
		}
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;	
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attrib;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg attribPkg(attrib);

	sms.ReceiveMessage(stat1, pdu, attribPkg);

// A request that will fail and force a re-initialisation
	TInt32 signalStrength=0;
	TInt8 bar=0;
	iPhone.GetSignalStrength(stat0,signalStrength,bar);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrTimedOut);

	sms.CancelAsyncRequest(EMobileSmsMessagingReceiveMessage);
	User::WaitForRequest(stat1);
	TESTL((stat1.Int()==KErrCancel)||(stat1.Int()==KErrNone));  // check that ReceiveMessage is cancelled

// Re-post a read request and re-initialise
	sms.ReceiveMessage(stat1, pdu, attribPkg);
	iPhone.InitialiseMM(stat0, tableSettingsPckg); 

	User::After(1000000L);

	sms.CancelAsyncRequest(EMobileSmsMessagingReceiveMessage);
	User::WaitForRequest(stat1);
	TESTL((stat1.Int()==KErrCancel)||(stat1.Int()==KErrNone));  // check that ReceiveMessage is cancelled

	User::WaitForRequest(stat0);
	TESTL(stat0.Int()==KErrNone);  // check that iPhone init ok

	sms.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestSmsCancel* CTestSmsCancel::NewL(const TScript* aScript)
	{
	CTestSmsCancel* aA=new(ELeave) CTestSmsCancel(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestSmsCancel::CTestSmsCancel(const TScript* aScript) : iScript(aScript)
	{}

void CTestSmsCancel::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestSmsCancel::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestSmsCancel::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestSmsCancel::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}

