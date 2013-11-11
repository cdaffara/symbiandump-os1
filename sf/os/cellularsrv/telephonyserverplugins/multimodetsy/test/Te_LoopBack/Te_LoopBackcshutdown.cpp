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
// This file implements several shutdown tests for the MM TSY.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32base.h>
#include <mmretrieve.h>
#include "Te_LoopBackcshutdown.h"
#include "Te_LoopbackCWrapGetDetNet.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveShutdown* CTestDriveShutdown::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveShutdown* aA=new(ELeave) CTestDriveShutdown(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveShutdown::CTestDriveShutdown(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveShutdown::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveShutdown::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	RMobileSmsMessaging sms;
	TRequestStatus stat0,reqStatus;

	INFO_PRINTF1(_L("Opening Sms Messaging\n"));
	TESTL(sms.Open(iPhone)==KErrNone);
	CleanupClosePushL(sms);

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	sms.SetReceiveMode (stat0, RMobileSmsMessaging::EReceiveStored);
	User::WaitForRequest(stat0);
	TESTL(stat0 == KErrNone); 

	// which mode - gsm or cdma pdu
	RMobileSmsMessaging::TMobileSmsCapsV1 caps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg capsPkg(caps);
	sms.GetCaps (capsPkg);
	caps = capsPkg();
	if (caps.iSmsMode & RMobileSmsMessaging::KCapsGsmSms)
		{
		INFO_PRINTF1(_L("Phone supports GSM/WCDMA mode.\n"));
		}
	/*else if (caps.iSmsMode & RMobileSmsMessaging::KCapsCdmaSms)
		{
		INFO_PRINTF1(_L("Phone supports CDMA mode.\n"));
		}*/
	RMobileSmsMessaging::TMobileSmsGsmTpdu pduGsm;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attrib;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg attribPkg(attrib);
	
	sms.ReceiveMessage(stat0, pduGsm, attribPkg);
	User::WaitForRequest(stat0);   
	TESTL(stat0.Int()==KErrNone);

	CleanupStack::PopAndDestroy(&sms);

	User::After(2000000L);

	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestShutdown* CTestShutdown::NewL(const TScript* aScript)
	{
	CTestShutdown* aA=new(ELeave) CTestShutdown(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestShutdown::CTestShutdown(const TScript* aScript) : iScript(aScript)
	{}

void CTestShutdown::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestShutdown::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestShutdown::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestShutdown::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveShutdownA* CTestDriveShutdownA::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveShutdownA* aA=new(ELeave) CTestDriveShutdownA(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveShutdownA::CTestDriveShutdownA(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveShutdownA::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveShutdownA::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	TRequestStatus reqStatus;
	RMobileSmsMessaging sms;
	INFO_PRINTF1(_L("Opening Sms Messaging\n"));
	TESTL(sms.Open(iPhone)==KErrNone);
	CleanupClosePushL(sms);

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	CWrapperGetDetectedNetworks* pWrapper=NULL;
	pWrapper = CWrapperGetDetectedNetworks::NewL(iPhone);
	CleanupStack::PushL(pWrapper);

	pWrapper->Start();
	CActiveScheduler::Start();

	User::After(1000000L);

	pWrapper->Cancel();
	CleanupStack::PopAndDestroy(pWrapper);

	iPhone.Close();

	CleanupStack::PopAndDestroy(&sms);

	User::After(2000000L);

	iPhone.Open(iServer,PHONE_NAME);

	return KErrNone;
	}
