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
// This file implements various transmit tests.  The scripts driven include new and old
// pdu formats and existence and non-existence of prefixes and sca information.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <mmretrieve.h>
#include <etelmm.h>
#include "Te_LoopBackctxmess.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint


//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveTxMess* CTestDriveTxMess::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveTxMess* aA = new (ELeave) CTestDriveTxMess(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveTxMess::CTestDriveTxMess(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

CTestDriveTxMess::~CTestDriveTxMess()
	{}

TInt CTestDriveTxMess::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return (StartEmulatorL());
	}

TInt CTestDriveTxMess::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	// Sending SMS in Multimode ETel API is not vastly different to the 
	// GSM API version. There are slight differences because the TSms class
	// no longer exists. The TPDU data is split from the message attributes
	// and clients will have to create a descriptor to hold the TPDU in 8-bit
	// data format. The client will also create and populate an instance of a
	// "message attribute" class. There is only one method SendMessage in the
	// Multimode ETel API and this is the equivalent of RSmsMessaging::SendMessage
	// and RAdvGsmSmsMessaging:: SendAdvSmsMessage. The difference between the
	// two GSM versions is that the latter also returned the submit report for
	// the sent SMS. In Multimode ETel API, the client can ask for the submit
	// report by using the TMobileSmsSendAttributesV1 class within the 
	// RMobileSmsMessaging::SendMessage.

	TRequestStatus reqStatus1;

	INFO_PRINTF1(_L("Opening SMS Messaging...\n"));
	RMobileSmsMessaging sms;
	TESTL(sms.Open(iPhone) == KErrNone);

	INFO_PRINTF1(_L("Initialising the Phone...\n"));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus1, tableSettingsPckg); 	

	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1 == KErrNone);

// Successful send...
	TPtrC8 pduA(KTestTxPduA, sizeof(KTestTxPduA));  // has "SCA Prepended" set by default
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsSendAttributes;

	//  set the flags according to TMobileSmsAttributeFlags
	smsSendAttributes.iFlags =
		(RMobileSmsMessaging::KSmsDataFormat		||
		 RMobileSmsMessaging::KGsmServiceCentre		||
		 RMobileSmsMessaging::KRemotePartyInfo		||
		 RMobileSmsMessaging::KStorageLocation		||
		 RMobileSmsMessaging::KIncomingStatus		||
		 RMobileSmsMessaging::KMessageReference		||
		 RMobileSmsMessaging::KGsmSubmitReport		||
		 RMobileSmsMessaging::KMoreToSend);

	smsSendAttributes.iMore = EFalse;

	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsSendAttributesPckg(smsSendAttributes);
    sms.SendMessage(reqStatus1, pduA, smsSendAttributesPckg);
	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1 == KErrNone);
	smsSendAttributes = smsSendAttributesPckg();
	TESTL(smsSendAttributes.iMsgRef == KMsgRefA);
	INFO_PRINTF1(_L("Closing SMS Messaging...\n"));
	sms.Close();
	User::After(1000000L);
	return (KErrNone);
	}

//
// Test New Standard PDU messages
//
CTestDriveTxNewStdMess* CTestDriveTxNewStdMess::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveTxNewStdMess* aA = new (ELeave) CTestDriveTxNewStdMess(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveTxNewStdMess::CTestDriveTxNewStdMess(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

CTestDriveTxNewStdMess::~CTestDriveTxNewStdMess()
	{}

TInt CTestDriveTxNewStdMess::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return (StartEmulatorL());
	}

TInt CTestDriveTxNewStdMess::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	TRequestStatus reqStatus;

	INFO_PRINTF1(_L("Initialising the Phone...\n"));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	INFO_PRINTF1(_L("Opening SMS Messaging...\n"));
	RMobileSmsMessaging sms;
	TESTL(sms.Open(iPhone) == KErrNone);

	TPtrC8 pduA(KTestTxPduA, sizeof(KTestTxPduA));  // has "SCA Prepended" set by default


	RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsSendAttributes;

	//  set the flags according to TMobileSmsAttributeFlags
	smsSendAttributes.iFlags =
		(RMobileSmsMessaging::KSmsDataFormat		||
		 RMobileSmsMessaging::KGsmServiceCentre		||
		 RMobileSmsMessaging::KRemotePartyInfo		||
		 RMobileSmsMessaging::KStorageLocation		||
		 RMobileSmsMessaging::KIncomingStatus		||
		 RMobileSmsMessaging::KMessageReference		||
		 RMobileSmsMessaging::KGsmSubmitReport 		||
		 RMobileSmsMessaging::KMoreToSend);

	smsSendAttributes.iMore = EFalse;

	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsSendAttributesPckg(smsSendAttributes);
    sms.SendMessage(reqStatus, pduA, smsSendAttributesPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);
	smsSendAttributes = smsSendAttributesPckg();
	TESTL(smsSendAttributes.iMsgRef == KMsgRefA);


	INFO_PRINTF1(_L("Closing SMS Messaging...\n"));
	sms.Close();
	return (KErrNone);
	}


//
// Test transmit with combination of SCA prefix on PDU, and setting of default SCA
//
CTestDriveTxWithScaCombo* CTestDriveTxWithScaCombo::NewL(const TScriptList aScriptListEntry,TBool aSendScaPrefix,TBool aSetDefaultSca,TBool aNewPduFormat)
	{
	CTestDriveTxWithScaCombo* aA = new (ELeave) CTestDriveTxWithScaCombo(aScriptListEntry,aSendScaPrefix,aSetDefaultSca,aNewPduFormat);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveTxWithScaCombo::CTestDriveTxWithScaCombo(const TScriptList aScriptListEntry,TBool aSendScaPrefix,TBool aSetDefaultSca,TBool aNewPduFormat)
	: iScriptListEntry(aScriptListEntry),
	  iSendScaPrefix(aSendScaPrefix),
	  iSetDefaultSca(aSetDefaultSca),
	  iNewPduFormat(aNewPduFormat)
	{}

CTestDriveTxWithScaCombo::~CTestDriveTxWithScaCombo()
	{}

TInt CTestDriveTxWithScaCombo::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return (StartEmulatorL());
	}

TInt CTestDriveTxWithScaCombo::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	TRequestStatus reqStatus;

	INFO_PRINTF1(_L("Initialising the Phone...\n"));
	
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	INFO_PRINTF1(_L("Opening SMS Messaging...\n"));
	RMobileSmsMessaging sms;
	TESTL(sms.Open(iPhone) == KErrNone);

	TPtrC8 pduA(KTestTxPduA, sizeof(KTestTxPduA));  // has "SCA Prepended" set by default

	TPtrC8 pduB(KTestTxPduB, sizeof(KTestTxPduB));  


	RMobileSmsMessaging::TMobileSmsSendAttributesV1 smsSendAttributes;
 
	//  set the flags according to TMobileSmsAttributeFlags
	smsSendAttributes.iFlags =
		(RMobileSmsMessaging::KSmsDataFormat		||
		 RMobileSmsMessaging::KGsmServiceCentre		||
		 RMobileSmsMessaging::KRemotePartyInfo		||
		 RMobileSmsMessaging::KStorageLocation		||
		 RMobileSmsMessaging::KIncomingStatus		||
		 RMobileSmsMessaging::KMessageReference		||
		 RMobileSmsMessaging::KGsmSubmitReport		||
		 RMobileSmsMessaging::KMoreToSend);

	
	smsSendAttributes.iMore = EFalse;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg smsSendAttributesPckg(smsSendAttributes);

 	TRequestStatus  reqStatus1;
 	sms.SendMessage(reqStatus1, pduA, smsSendAttributesPckg);
 	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1 == KErrNone);
	smsSendAttributes = smsSendAttributesPckg();
 	TESTL(smsSendAttributes.iMsgRef == KMsgRefA);

// Another success...
    sms.SendMessage(reqStatus1, pduB, smsSendAttributesPckg);
	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1 == KErrNone);
	smsSendAttributes = smsSendAttributesPckg();
 	TESTL(smsSendAttributes.iMsgRef == KMsgRefB);


	INFO_PRINTF1(_L("Closing SMS Messaging...\n"));
	sms.Close();
    return (KErrNone);
	}

//
// Test transmit with combination of SCA prefix on PDU, and setting of default SCA
//
CTestDriveTxRx* CTestDriveTxRx::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveTxRx* aA = new (ELeave) CTestDriveTxRx(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveTxRx::CTestDriveTxRx(const TScriptList aScriptListEntry)
	: iScriptListEntry(aScriptListEntry)
	{}

CTestDriveTxRx::~CTestDriveTxRx()
	{}

TInt CTestDriveTxRx::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return (StartEmulatorL());
	}

TInt CTestDriveTxRx::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{

	TRequestStatus reqStatus1;//, reqStatus2;

	INFO_PRINTF1(_L("Initialising the Phone...\n"));
	
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus1 , tableSettingsPckg); 	

	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1 == KErrNone);

	INFO_PRINTF1(_L("Opening SMS Messaging...\n"));
	RMobileSmsMessaging sms;
	TESTL(sms.Open(iPhone) == KErrNone);

	INFO_PRINTF1(_L("Closing SMS Messaging...\n"));
	sms.Close();
	return (KErrNone);
	}


//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestTxMess* CTestTxMess::NewL(const TScript* aScript)
	{
	CTestTxMess* aA = new (ELeave) CTestTxMess(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestTxMess* CTestTxMess::NewL(const TScript* aScript, const TInt aVarDelay)
	{
	CTestTxMess* aA = new(ELeave) CTestTxMess(aScript, aVarDelay);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestTxMess::CTestTxMess(const TScript* aScript) :
	CATScriptEng(), iScript(aScript)
	{}

CTestTxMess::CTestTxMess(const TScript* aScript, const TInt aVarDelay) :
	CATScriptEng(aVarDelay), iScript(aScript)
	{}

void CTestTxMess::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

CTestTxMess::~CTestTxMess()
	{}

TInt CTestTxMess::Start()
	{
	StartScript(iScript);
	return (KErrNone);
	}

void CTestTxMess::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestTxMess::Complete(TInt aError)
	{
	iReturnValue = aError;
	CActiveScheduler::Stop();
	}
