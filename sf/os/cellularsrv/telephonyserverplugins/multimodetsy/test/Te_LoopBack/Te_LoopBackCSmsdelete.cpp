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
// This file implements simple SMS Storage Delete tests.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <mmretrieve.h>
#include <etelmm.h>
#include "Te_LoopBackCSmsdelete.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint


#define DMMTSY_SMS_MESSAGE_PDU2		_L8("22221234567899876543212222")
#define DMMTSY_SMS_STORE_INDEX2		(45)
//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveSmsDelete* CTestDriveSmsDelete::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveSmsDelete* aA = new (ELeave) CTestDriveSmsDelete(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveSmsDelete::CTestDriveSmsDelete(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

CTestDriveSmsDelete::~CTestDriveSmsDelete()
	{}

TInt CTestDriveSmsDelete::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveSmsDelete::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	RMobileSmsMessaging sms;
	RMobileSmsStore smsSimStore;

	INFO_PRINTF1(_L("Initialising the Phone...\n"));
	TRequestStatus reqStatus;

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus, tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	INFO_PRINTF1(_L("Opening SMS Messaging...\n"));
	TESTL(sms.Open(iPhone) == KErrNone);

 	INFO_PRINTF1(_L("Opening SMS SIM Storage...\n"));
	TESTL(smsSimStore.Open(sms, KETelCombinedSmsStore) == KErrNone); 
	
	TestSmsDeleteL(smsSimStore);

	INFO_PRINTF1(_L("End of Sms Delete tests\n"));

	smsSimStore.Close();
	sms.Close();

	return (KErrNone);
	}

 
void CTestDriveSmsDelete::TestSmsDeleteL(RMobileSmsStore& aSmsSimStore)
	{

 //	Get SMS store information
	TRequestStatus   Status1,Status2;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 asyncInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg asyncInfoPckg(asyncInfo);
	
	// asynchronous
	INFO_PRINTF1(_L("Mobile Phone SMS Store Info\n"));
	aSmsSimStore.GetInfo(Status1, asyncInfoPckg); 
	User::WaitForRequest(Status1);
	INFO_PRINTF2(_L("SMS GetInfo Status before Write = %d\n "),Status1.Int());

	if(Status1==KErrNone)
		{
		INFO_PRINTF1(_L("Mobile Phone SMS Store Info Before Write\n"));
		asyncInfo=asyncInfoPckg();
		DisplayPhoneStoreInfo(asyncInfo);
		}

	INFO_PRINTF1(_L("Mobile Phone SMS Store Write\n"));

	RMobileSmsStore::TMobileGsmSmsEntryV1 entry1;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg entry1Pckg(entry1);
	entry1.iMsgData = DMMTSY_SMS_MESSAGE_PDU2;
	
	aSmsSimStore.Write(Status2, entry1Pckg);
	
	User::WaitForRequest(Status2);
	TESTL(Status2==KErrNone); 
	INFO_PRINTF2(_L("SMS Write Status = %d\n "),Status2.Int());

	aSmsSimStore.GetInfo(Status1, asyncInfoPckg); 
	User::WaitForRequest(Status1); 

	
	INFO_PRINTF2(_L("SMS GetInfo Status After Write = %d\n "),Status1.Int());

	if(Status1==KErrNone)
		{
		INFO_PRINTF1(_L("Mobile Phone SMS Store Info After Write\n"));
		asyncInfo=asyncInfoPckg();
		DisplayPhoneStoreInfo(asyncInfo);
		}
	
	INFO_PRINTF1(_L("SMS Delete - \n "));
	aSmsSimStore.Delete(Status2,1);
 	User::WaitForRequest(Status2);
    TESTL(Status2==KErrNone);   
	INFO_PRINTF2(_L("SMS Delete Status = %d\n "),Status2.Int());

	INFO_PRINTF1(_L("Mobile Phone SMS Store Info After Delete\n"));
	
	aSmsSimStore.GetInfo(Status1, asyncInfoPckg); 
	User::WaitForRequest(Status1);
	INFO_PRINTF2(_L("SMS GetInfo Status After delete = %d\n "),Status1.Int());

	if(Status1==KErrNone)
		{
		INFO_PRINTF1(_L("Mobile Phone SMS Store Info After Write\n"));
		asyncInfo=asyncInfoPckg();
		DisplayPhoneStoreInfo(asyncInfo);
		}


	}


void CTestDriveSmsDelete::DisplayPhoneStoreInfo(RMobilePhoneStore::TMobilePhoneStoreInfoV1& aStoreInfo)
	{
	INFO_PRINTF1(_L("Mobile Phone Store Info:\n"));
	INFO_PRINTF2(_L("  Name = %S\n"), &aStoreInfo.iName);\

	switch (aStoreInfo.iType)
		{
		case RMobilePhoneStore::EPhoneStoreTypeUnknown:
			INFO_PRINTF1(_L("  Type = Unknown\n"));
			break;
		case RMobilePhoneStore::EShortMessageStore:
			INFO_PRINTF1(_L("  Type = Short Message\n"));
			break;
		case RMobilePhoneStore::ENamStore:
			INFO_PRINTF1(_L("  Type = NAM\n"));
			break;
		case RMobilePhoneStore::EPhoneBookStore:
			INFO_PRINTF1(_L("  Type = Phonebook\n"));
			break;
		case RMobilePhoneStore::EEmergencyNumberStore:
			INFO_PRINTF1(_L("  Type = Emergency Number\n"));
			break;
		case RMobilePhoneStore::EOwnNumberStore:
			INFO_PRINTF1(_L("  Type = Own Number\n"));
			break;
		default:
			INFO_PRINTF2(_L("  Type = %d\n"), aStoreInfo.iType);
			break;
		}

	INFO_PRINTF2(_L("  Total Entries = %d\n"), aStoreInfo.iTotalEntries);
	INFO_PRINTF2(_L("  Used Entries  = %d\n"), aStoreInfo.iUsedEntries);


	// display the caps for the base class RMobilePhoneStore
	INFO_PRINTF2(_L("  SMS Store Capabilities = 0x%x\n"), aStoreInfo.iCaps);
	INFO_PRINTF1(_L("  Phone Store Capabilities:\n"));
	if (aStoreInfo.iCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore))
		INFO_PRINTF1(_L("  Whole Store Supported\n"));
	if (aStoreInfo.iCaps & RMobilePhoneStore::KCapsIndividualEntry)
		INFO_PRINTF1(_L("  Individual Entry Supported\n"));
	if (aStoreInfo.iCaps & RMobilePhoneStore::KCapsReadAccess)
		INFO_PRINTF1(_L("  Read Access Supported\n"));
	if (aStoreInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess)
		INFO_PRINTF1(_L("  Write Access Supported\n"));
	if (aStoreInfo.iCaps & RMobilePhoneStore::KCapsDeleteAll)
		INFO_PRINTF1(_L("  Delete All Supported\n"));
	if (aStoreInfo.iCaps & RMobilePhoneStore::KCapsNotifyEvent)
		INFO_PRINTF1(_L("  Notify Event Supported\n"));

	// display the caps for the derived class RMobileSmsStore
	// these caps use the same variable as the base class
	INFO_PRINTF1(_L("  SMS Store Capabilities:\n"));
	if (aStoreInfo.iCaps & RMobileSmsStore::KCapsUnreadMessages)
		INFO_PRINTF1(_L("  Unread Messages Supported\n"));
	if (aStoreInfo.iCaps & RMobileSmsStore::KCapsReadMessages)
		INFO_PRINTF1(_L("  Read Messages Supported\n"));
	if (aStoreInfo.iCaps & RMobileSmsStore::KCapsSentMessages)
		INFO_PRINTF1(_L("  Sent Messages Supported\n"));
	if (aStoreInfo.iCaps & RMobileSmsStore::KCapsUnsentMessages)
		INFO_PRINTF1(_L("  Unsent Messages Supported\n"));
	if (aStoreInfo.iCaps & RMobileSmsStore::KCapsGsmMessages)
		INFO_PRINTF1(_L("  GSM Messages Supported\n"));
	
	}
 

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestSmsDelMess* CTestSmsDelMess::NewL(const TScript* aScript)
	{
	CTestSmsDelMess* aA = new (ELeave) CTestSmsDelMess(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestSmsDelMess::CTestSmsDelMess(const TScript* aScript) :
	iScript(aScript)
	{}

void CTestSmsDelMess::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

CTestSmsDelMess::~CTestSmsDelMess()
	{}

TInt CTestSmsDelMess::Start()
	{
	StartScript(iScript);
	return (KErrNone);
	}

void CTestSmsDelMess::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestSmsDelMess::Complete(TInt aError)
	{
	iReturnValue = aError;
	CActiveScheduler::Stop();
	}
