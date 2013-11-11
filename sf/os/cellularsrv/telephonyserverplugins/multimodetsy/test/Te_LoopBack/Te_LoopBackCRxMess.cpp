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
// This file implements various receive message tests.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <etelmm.h>
#include "Te_LoopBackcrxmess.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveRxMess* CTestDriveRxMess::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveRxMess* aA=new(ELeave) CTestDriveRxMess(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveRxMess::CTestDriveRxMess(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{
	// NOP
	}

TInt CTestDriveRxMess::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveRxMess::DriveETelApiL() //1
//
// This function contains the real meat of the Client-side test code
//
	{
	TRequestStatus	reqStatus;
	INFO_PRINTF1(_L("Initialising the Phone..."));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg);

	User::WaitForRequest(reqStatus);

	TESTL(reqStatus == KErrNone);

	RMobileSmsMessaging sms;

	INFO_PRINTF1(_L("Opening Sms Messaging"));

	TESTL(sms.Open(iPhone)==KErrNone);

	RMobileSmsMessaging::TMobileSmsCapsV1     smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(smsCaps);

	sms.GetCaps(smsCapsPckg);
	smsCaps = smsCapsPckg();
	DispCaps(smsCaps);

	TRequestStatus	stat1, stat2;
	sms.SetReceiveMode (stat1, RMobileSmsMessaging::EReceiveStored);
	User::WaitForRequest(stat1);
	TESTL(stat1 == KErrNone);

	if(stat1 == KErrNone)
		{
		RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attrib;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg attribPkg(attrib);
		sms.ReceiveMessage(stat2, pdu, attribPkg);
 		User::WaitForRequest(stat2);
		TESTL(stat2 == KErrNone);
 		attrib=attribPkg();
 		DispSmsAttributes(attrib);
		}

	INFO_PRINTF1(_L("Closing SMS Messaging..."));
	sms.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestRxMess* CTestRxMess::NewL(const TScript* aScript)
	{
	CTestRxMess* aA=new(ELeave) CTestRxMess(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestRxMess::CTestRxMess(const TScript* aScript) : iScript(aScript)
	{
	// NOP
	}

void CTestRxMess::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestRxMess::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestRxMess::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestRxMess::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}

//
// Test the SMS Notification Request
//
CTestDriveNotMess* CTestDriveNotMess::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveNotMess* aA=new(ELeave) CTestDriveNotMess(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveNotMess::CTestDriveNotMess(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{
	// NOP
	}

TInt CTestDriveNotMess::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveNotMess::DriveETelApiL() //2
//
// This function contains the real meat of the Client-side test code
//
	{
	// Since NotifySmsMessageArrived is no more supported, we just have to
	// test with 1 script. We don't have to do post notify, read sim. and
	// then post notify, wait for complete, and post read.
	// Also SetReceiveMode requires a completed iPhone initialization.
	RMobileSmsMessaging sms;

	TRequestStatus	reqStatus, stat1, stat2, stat3;
	INFO_PRINTF1(_L("Initialising the Phone..."));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus, tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	INFO_PRINTF1(_L("Opening Sms Messaging"));
	TESTL(sms.Open(iPhone) == KErrNone);

	RMobileSmsMessaging::TMobileSmsReceiveMode aReceiveMode;
	// KErrNotSupported is normal response as there can be only
	// one sms client who knows if the mode is changed.
	sms.NotifyReceiveModeChange(stat3, aReceiveMode); 
	
	// set to CMTI mode (CNMI=0,1)
	sms.SetReceiveMode (stat1, RMobileSmsMessaging::EReceiveStored);
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);

	User::WaitForRequest(stat3);
	INFO_PRINTF2(_L("Sms Messaging: NotifyReceiveModeChange Not Supported = %d"), stat3.Int());

	if(stat1==KErrNone)
		{
		RMobileSmsMessaging::TMobileSmsGsmTpdu rxPdu;			
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attrib;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg attribPkg(attrib);
		sms.ReceiveMessage(stat2, rxPdu, attribPkg);
		User::WaitForRequest(stat2);
		TESTL(stat2==KErrNone);
		attrib=attribPkg();
 		//TestDriveRxMess->
		DispSmsAttributes(attrib);
		}

	INFO_PRINTF1(_L("Closing SMS Messaging..."));
	sms.Close();
	return KErrNone;
	}

//
// Test-side class
// Test CMT based incoming SMS messages
//
CTestDriveRxMessCmt* CTestDriveRxMessCmt::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveRxMessCmt* aA=new(ELeave) CTestDriveRxMessCmt(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveRxMessCmt::CTestDriveRxMessCmt(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{
	// NOP
	}

TInt CTestDriveRxMessCmt::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveRxMessCmt::DriveETelApiL() //3
//
// This function contains the real meat of the Client-side test code
//
	{
	TRequestStatus reqStatus;

	INFO_PRINTF1(_L("Initialising the Phone..."));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus, tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	RMobileSmsMessaging sms;
	INFO_PRINTF1(_L("Opening Sms Messaging"));
	TESTL(sms.Open(iPhone)==KErrNone);

    TRequestStatus	stat1,stat2;
	sms.SetReceiveMode(stat1, RMobileSmsMessaging::EReceiveUnstoredPhoneAck);
	User::WaitForRequest(stat1);
	TESTL(stat1 == KErrNone);  

	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;		
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attrib;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg attribPkg(attrib);
	INFO_PRINTF1(_L("Closing SMS Messaging..."));
	sms.ReceiveMessage(stat2, pdu, attribPkg);
	User::WaitForRequest(stat2);
	TESTL(stat2 == KErrNone); 
	attrib=attribPkg();
	//aTestDriveRxMess->
	DispSmsAttributes(attrib);

	INFO_PRINTF1(_L("Rx CMT type message"));
	sms.ReceiveMessage(stat2, pdu, attribPkg);
	User::WaitForRequest(stat2);
	TESTL(stat2 == KErrNone); 
	attrib=attribPkg();
	DispSmsAttributes(attrib);
 
	INFO_PRINTF1(_L("Closing SMS Messaging..."));
	sms.Close();
	return KErrNone;
	}

//
// Test-side class
// Test CMT based incoming SMS Notifications
//
CTestDriveNotMessCmt* CTestDriveNotMessCmt::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveNotMessCmt* aA=new(ELeave) CTestDriveNotMessCmt(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveNotMessCmt::CTestDriveNotMessCmt(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{
	// NOP
	}

TInt CTestDriveNotMessCmt::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveNotMessCmt::DriveETelApiL() //4
//
// This function contains the real meat of the Client-side test code
//
	{
	RMobileSmsMessaging sms;

	TRequestStatus	reqStatus, stat1, stat2, stat3;
	INFO_PRINTF1(_L("Initialising the Phone..."));
	
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus, tableSettingsPckg);

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	INFO_PRINTF1(_L("Opening Sms Messaging"));
	TESTL(sms.Open(iPhone) == KErrNone);

	RMobileSmsMessaging::TMobileSmsReceiveMode aReceiveMode;
	sms.NotifyReceiveModeChange(stat3, aReceiveMode);

	sms.SetReceiveMode (stat1, RMobileSmsMessaging::EReceiveStored);
	User::WaitForRequest(stat1);
	TESTL(stat1 == KErrNone);

	User::WaitForRequest(stat3);
	INFO_PRINTF2(_L("Sms Messaging: NotifyReceiveModeChange Not Supported = %d"), stat3.Int());

	if (stat1 == KErrNone)
		{
		RMobileSmsMessaging::TMobileSmsGsmTpdu rxPdu;		
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attrib;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg attribPkg(attrib);
		sms.ReceiveMessage(stat2, rxPdu, attribPkg);
 		User::WaitForRequest(stat2);  
		TESTL(stat2 == KErrNone);  
		}

	INFO_PRINTF1(_L("Closing SMS Messaging..."));

	sms.Close();
	return KErrNone;
	}


void CTestDriveRxMess::DispSmsAttributes(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aMobSmsAttr)
	{
	switch (aMobSmsAttr.iStatus)
		{
		case RMobileSmsMessaging::EMtMessageUnknownStatus:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageUnknownStatus."));
			break;
		case RMobileSmsMessaging::EMtMessageUnstoredPhoneAck:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageUnstoredPhoneAck."));
			break;
		case RMobileSmsMessaging::EMtMessageUnstoredClientAck:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageUnstoredClientAck."));
			break;
		case RMobileSmsMessaging::EMtMessageStored:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageStored."));
			break;
		default:
			break;
		}

	INFO_PRINTF2(_L(" RMobileSmsMessaging Store Index = %d"), aMobSmsAttr.iStoreIndex);
	INFO_PRINTF2(_L(" RMobileSmsMessaging Store Name = %S"), &aMobSmsAttr.iStore);
	INFO_PRINTF2(_L(" RMobileSmsMessaging  iOriginator = %S"), &aMobSmsAttr.iOriginator.iTelNumber);
	}	

//it's bad, but I don't  have any idea to redesing these tests
void CTestDriveNotMess::DispSmsAttributes(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aMobSmsAttr)
	{
	switch (aMobSmsAttr.iStatus)
		{
		case RMobileSmsMessaging::EMtMessageUnknownStatus:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageUnknownStatus."));
			break;
		case RMobileSmsMessaging::EMtMessageUnstoredPhoneAck:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageUnstoredPhoneAck."));
			break;
		case RMobileSmsMessaging::EMtMessageUnstoredClientAck:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageUnstoredClientAck."));
			break;
		case RMobileSmsMessaging::EMtMessageStored:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageStored."));
			break;
		default:
			break;
		}

	INFO_PRINTF2(_L(" RMobileSmsMessaging Store Index = %d"), aMobSmsAttr.iStoreIndex);
	INFO_PRINTF2(_L(" RMobileSmsMessaging Store Name = %S"), &aMobSmsAttr.iStore);
	INFO_PRINTF2(_L(" RMobileSmsMessaging  iOriginator = %S"), &aMobSmsAttr.iOriginator.iTelNumber);
	}


void CTestDriveRxMess::DispCaps(RMobileSmsMessaging::TMobileSmsCapsV1& aSmsCaps)
	{
	if ( aSmsCaps.iSmsMode & RMobileSmsMessaging::KCapsGsmSms)
		{
		INFO_PRINTF1(_L("    GSM SMS Supported"));
		}
   	if (!( aSmsCaps.iSmsMode & RMobileSmsMessaging::KCapsGsmSms))
		{
		INFO_PRINTF2(_L("    SMS Mode = 0x%x"), aSmsCaps.iSmsMode);
		}

	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveStored)
		{
		INFO_PRINTF1(_L("   ReceiveStored Caps Supported"));
		}
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveUnstoredPhoneAck)
		{
		INFO_PRINTF1(_L("   ReceiveUnstoredPhoneAck Caps Supported"));
		}
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveEither)
		{
		INFO_PRINTF1(_L("   ReceiveAny Caps Supported"));
		}
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsCreateAck)
		{
		INFO_PRINTF1(_L("   CreateAck Caps Supported"));
		}
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSendNoAck)
		{
		INFO_PRINTF1(_L("   SendNoAck Caps Supported"));
		}
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSendWithAck)
		{
		INFO_PRINTF1(_L("   SendWithAck Caps Supported"));
		}
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsGetSmspList)
		{
		INFO_PRINTF1(_L("   GetSmspList Caps Supported"));
		}
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSetSmspList)
		{
		INFO_PRINTF1(_L("   SetSmspList Caps Supported"));
		}
	}

void CTestDriveRxMessCmt::DispSmsAttributes(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aMobSmsAttr)
	{
	switch (aMobSmsAttr.iStatus)
		{
		case RMobileSmsMessaging::EMtMessageUnknownStatus:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageUnknownStatus."));
			break;
		case RMobileSmsMessaging::EMtMessageUnstoredPhoneAck:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageUnstoredPhoneAck."));
			break;
		case RMobileSmsMessaging::EMtMessageUnstoredClientAck:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageUnstoredClientAck."));
			break;
		case RMobileSmsMessaging::EMtMessageStored:
			INFO_PRINTF1(_L(" RMobileSmsMessaging::EMtMessageStored."));
			break;
		default:
			break;
		}

	INFO_PRINTF2(_L(" RMobileSmsMessaging Store Index = %d"), aMobSmsAttr.iStoreIndex);
	INFO_PRINTF2(_L(" RMobileSmsMessaging Store Name = %S"), &aMobSmsAttr.iStore);
	INFO_PRINTF2(_L(" RMobileSmsMessaging  iOriginator = %S"), &aMobSmsAttr.iOriginator.iTelNumber);
	}
