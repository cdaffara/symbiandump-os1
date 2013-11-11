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
// This file implements test scenarios for message storage with both old and 
// new format PDU's.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/



#include "Te_LoopBackcmessstor.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveMessStor* CTestDriveMessStor::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveMessStor* aA = new (ELeave) CTestDriveMessStor(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveMessStor::CTestDriveMessStor(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

CTestDriveMessStor::~CTestDriveMessStor()
	{}

TInt CTestDriveMessStor::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return StartEmulatorL();
	}



TInt CTestDriveMessStor::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	
// create the SMS objects
	RMobileSmsMessaging sms;
	RMobileSmsStore smsSimStore;    // SIM

	TRequestStatus reqStatus;

	INFO_PRINTF1(_L("Opening SMS Messaging...\n"));

	TESTL(sms.Open(iPhone) == KErrNone);

	INFO_PRINTF1(_L("Initialising the Phone...\n"));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus, tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);


// get the SMS Messaging capabilities
	TestSmsMessagingCapablitiesL(sms);

	INFO_PRINTF1(_L("Opening SMS SIM Storage...\n"));
	TInt ret = smsSimStore.Open(sms, KETelCombinedSmsStore); // OK
	TESTL(ret == KErrNone);

// display the SMS SIM  Combined Storage capabilities
	TestSmsSimStorageCapablitiesL(smsSimStore);

	sms.NotifySmspListChange(reqStatus);  
// Test the SMS SIM Storage  by storing SMSpLIST 
	TestSmsStorage(sms);
	User::WaitForRequest(reqStatus); 
	INFO_PRINTF2(_L("\nNotifySmspListChange returns = %d\n"),reqStatus.Int());

	
// Display SMS SIM Storage  - Requires AO for getting the Smsplist

 	RMobileSmsMessaging::TMobileSmspEntryV1 aSmspEntry;
	TRAP_IGNORE(GetSmspListL(aSmspEntry,sms));		// trap and ignore error
 	
 	DisplaySmspEntry(aSmspEntry);

	smsSimStore.Close();
	sms.Close();

	return (KErrNone);
	}




void CTestDriveMessStor::DisplaySmsCaps(RMobileSmsMessaging::TMobileSmsCapsV1& aSmsCaps)
	{
	INFO_PRINTF1(_L("  SMS Supported Modes:\n"));

	if (aSmsCaps.iSmsMode & RMobileSmsMessaging::KCapsGsmSms)
		INFO_PRINTF1(_L("    GSM SMS Supported\n"));
	if (!(aSmsCaps.iSmsMode & RMobileSmsMessaging::KCapsGsmSms))
		INFO_PRINTF2(_L("    SMS Mode = 0x%x\n"), aSmsCaps.iSmsMode);
	INFO_PRINTF1(_L("  SMS Supported Controls:\n"));
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveStored)
		INFO_PRINTF1(_L("    Receive Stored Supported\n"));
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveUnstoredPhoneAck)
		INFO_PRINTF1(_L("    Receive Unstored Phone Ack Supported\n"));
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveUnstoredClientAck)
		INFO_PRINTF1(_L("    Receive Unstored Client Ack Supported\n"));
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveEither)
		INFO_PRINTF1(_L("    Receive Any Supported\n"));
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSendNoAck)
		INFO_PRINTF1(_L("    Send No Ack Supported\n"));
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSendWithAck)
		INFO_PRINTF1(_L("    Send With Ack Supported\n"));
 	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsCreateAck)
 		INFO_PRINTF1(_L("     CapsCreate with Ack Supported\n"));
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsGetSmspList)
		INFO_PRINTF1(_L("    Get Smsp List Supported\n"));
	if (aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSetSmspList)
		INFO_PRINTF1(_L("    Set Smsp List Supported\n"));

	if (!(aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveStored)            &&
		!(aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveUnstoredPhoneAck)  &&
		!(aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveUnstoredClientAck) &&
		!(aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveEither)            &&
		!(aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSendNoAck)                &&
		!(aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSendWithAck)              &&
 		!(aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsCreateAck)                &&
		!(aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsGetSmspList)              &&
		!(aSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSetSmspList))
		INFO_PRINTF2(_L("    SMS Controls = 0x%x\n"), aSmsCaps.iSmsControl);

	INFO_PRINTF1(_L("\n"));
}


void CTestDriveMessStor::DisplayPhoneStoreInfo(RMobilePhoneStore::TMobilePhoneStoreInfoV1& aStoreInfo)
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


void CTestDriveMessStor::DisplaySmspEntry(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry)
	{
	INFO_PRINTF1(_L("SMS Param Entry:\n"));
	INFO_PRINTF2(_L("  Index = %d:\n"), aSmspEntry.iIndex);
	INFO_PRINTF2(_L("  Text = %S:\n"), &aSmspEntry.iText);
	INFO_PRINTF2(_L("  Valid Params = %d\n"), aSmspEntry.iValidityPeriod);

	INFO_PRINTF1(_L("  Destination:\n"));
	DisplayMobileAddr(aSmspEntry.iDestination);

	INFO_PRINTF1(_L("  SCA:\n"));
	DisplayMobileAddr(aSmspEntry.iServiceCentre);

	INFO_PRINTF2(_L("  Protocol Id     = %d\n"), aSmspEntry.iProtocolId);
	INFO_PRINTF2(_L("  DCS             = %d\n"), aSmspEntry.iDcs);
	INFO_PRINTF2(_L("  Validity Period = %d\n"), aSmspEntry.iValidityPeriod);
	}


void CTestDriveMessStor::DisplayMobileAddr(RMobilePhone::TMobileAddress aMobileAddr)
	{
	switch (aMobileAddr.iTypeOfNumber)
		{
		case RMobilePhone::EUnknownNumber:
			INFO_PRINTF1(_L("    Type of Number = Unknown\n"));
			break;
		case RMobilePhone::EInternationalNumber:
			INFO_PRINTF1(_L("    Type of Number = International\n"));
			break;
		case RMobilePhone::ENationalNumber:
			INFO_PRINTF1(_L("    Type of Number = National\n"));
			break;
		case RMobilePhone::ENetworkSpecificNumber:
			INFO_PRINTF1(_L("    Type of Number = Network Specific\n"));
			break;
		case RMobilePhone::ESubscriberNumber:
			INFO_PRINTF1(_L("    Type of Number = Subscriber\n"));
			break;
		case RMobilePhone::EAbbreviatedNumber:
			INFO_PRINTF1(_L("    Type of Number = Abbreviated\n"));
			break;
		default:
			INFO_PRINTF2(_L("    Type of Number = %d\n"), aMobileAddr.iTypeOfNumber);
			break;
		}

	switch (aMobileAddr.iNumberPlan)
		{
		case RMobilePhone::EUnknownNumberingPlan:
			INFO_PRINTF1(_L("    Number Plan = Unknown\n"));
			break;
		case RMobilePhone::EIsdnNumberPlan:
			INFO_PRINTF1(_L("    Number Plan = ISDN\n"));
			break;
		case RMobilePhone::EDataNumberPlan:
			INFO_PRINTF1(_L("    Number Plan = Data\n"));
			break;
		case RMobilePhone::ETelexNumberPlan:
			INFO_PRINTF1(_L("    Number Plan = Telex\n"));
			break;
		case RMobilePhone::EPrivateNumberPlan:
			INFO_PRINTF1(_L("    Number Plan = Private\n"));
			break;
		case RMobilePhone::ENationalNumberPlan:
			INFO_PRINTF1(_L("    Number Plan = National\n"));
			break;
		default:
			INFO_PRINTF2(_L("    Number Plan = %d\n"), aMobileAddr.iNumberPlan);
			break;
		}

	INFO_PRINTF2(_L("  Tel # = %S\n"), &aMobileAddr.iTelNumber);

	}


void CTestDriveMessStor::TestSmsMessagingCapablitiesL(RMobileSmsMessaging& aSms)
	{
	INFO_PRINTF1(_L("Getting SMS Capabilities...\n\n"));
	RMobileSmsMessaging::TMobileSmsCapsV1     smsCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(smsCaps);
	
	TInt ret = aSms.GetCaps(smsCapsPckg);
	TESTL(ret == KErrNone);
	smsCaps = smsCapsPckg();
	DisplaySmsCaps(smsCaps);
	}


void CTestDriveMessStor::TestSmsSimStorageCapablitiesL(RMobileSmsStore& aSmsSimStore)
	{
	INFO_PRINTF1(_L("Getting SMS SIM Store Info...\n"));
	RMobilePhoneStore::TMobilePhoneStoreInfoV1     simStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg simStoreInfoPckg(simStoreInfo);

	TRequestStatus reqStatus;
	aSmsSimStore.GetInfo(reqStatus, simStoreInfoPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);
	simStoreInfo = simStoreInfoPckg();
	DisplayPhoneStoreInfo(simStoreInfo);
	}

void CTestDriveMessStor::TestSmsStorage(RMobileSmsMessaging& aSms)
	{
	TRAP_IGNORE(DoTestSmsStorageL(aSms));
	}

void CTestDriveMessStor::DoTestSmsStorageL(RMobileSmsMessaging& aSms)
	{
	INFO_PRINTF1(_L("Store new SMSP list\n"));

	RMobileSmsMessaging::TMobileSmspEntryV1 newSmspEntry;

	newSmspEntry.iValidParams = RMobileSmsMessaging::KSCAIncluded;
	newSmspEntry.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	newSmspEntry.iServiceCentre.iNumberPlan=RMobilePhone:: EIsdnNumberPlan;
	newSmspEntry.iServiceCentre.iTelNumber.Copy(_L("441632960000"));

	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	smspList->AddEntryL(newSmspEntry);

	TRequestStatus reqStatus;
	
	aSms.StoreSmspListL(reqStatus, smspList);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);  

	CleanupStack::PopAndDestroy(); // smspList;
	}


void CTestDriveMessStor::GetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry,RMobileSmsMessaging& aSms)
/**
 * This function tests CRetrieveMobilePhoneSmspList
 */
	{
	CTestGetSmspList* retrieveSmspList;
	CMobilePhoneSmspList* smspList;
// We have 	ActiveScheduler already
//	CActiveScheduler* testActiveScheduler = new(ELeave) CActiveScheduler();
//	TESTL(testActiveScheduler != NULL);
//	CActiveScheduler::Install(testActiveScheduler);

	retrieveSmspList = CTestGetSmspList::NewLC(aSms);
	CleanupStack::PushL(retrieveSmspList);

	retrieveSmspList->Start();
	CActiveScheduler::Start();

	smspList = retrieveSmspList->RetrieveList();

	TInt count = smspList->Enumerate();
	INFO_PRINTF2(_L("SMSP list has %d entries\n"), count);

	TInt i;
	for (i=0; i<count; i++)
		{
		aSmspEntry = smspList->GetEntryL(i);
		INFO_PRINTF3(_L("Entry %d: Service Centre = >%S<\n"),i,&aSmspEntry.iServiceCentre.iTelNumber);
		}

	CleanupStack::PopAndDestroy(); // retrieveSmspList;
	delete smspList;
//	delete testActiveScheduler;
//	User::After(1000000);		// Give user time to see the test results 
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestMessStor* CTestMessStor::NewL(const TScript* aScript)
	{
	CTestMessStor* aA = new (ELeave) CTestMessStor(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestMessStor::CTestMessStor(const TScript* aScript) :
	iScript(aScript)
	{}

void CTestMessStor::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

CTestMessStor::~CTestMessStor()
	{}

TInt CTestMessStor::Start()
	{
	StartScript(iScript);
	return (KErrNone);
	}

void CTestMessStor::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestMessStor::Complete(TInt aError)
	{
	iReturnValue = aError;
	CActiveScheduler::Stop();
	}
