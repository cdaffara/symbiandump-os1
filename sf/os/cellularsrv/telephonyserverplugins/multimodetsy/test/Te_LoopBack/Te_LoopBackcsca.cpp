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
// This file implements SCA retrieval and setting tests.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/


#include "Te_LoopBackcsca.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveSca* CTestDriveSca::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveSca* aA = new (ELeave) CTestDriveSca(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveSca::CTestDriveSca(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

CTestDriveSca::~CTestDriveSca()
	{}

TInt CTestDriveSca::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return (StartEmulatorL());
	}



TInt CTestDriveSca::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	INFO_PRINTF1(_L("Opening SMS Messaging...\n"));

	RMobileSmsMessaging sms;
	TESTL(sms.Open(iPhone) == KErrNone);

	INFO_PRINTF1(_L("Initialising the Phone...\n"));

	TRequestStatus reqStatus;
	
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus, tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);
	
	RMobileSmsMessaging::TMobileSmspEntryV1 newSmspEntry;
	RMobileSmsMessaging::TMobileSmspEntryV1 checkSmspEntry;

	INFO_PRINTF1(_L("Retrieve default SMSP list\n"));

	INFO_PRINTF1(_L("Store new SMSP list\n"));

	newSmspEntry.iValidParams = RMobileSmsMessaging::KSCAIncluded;
	newSmspEntry.iServiceCentre.iTypeOfNumber=RMobilePhone::EInternationalNumber;
	newSmspEntry.iServiceCentre.iNumberPlan=RMobilePhone:: EIsdnNumberPlan;
	newSmspEntry.iServiceCentre.iTelNumber.Copy(_L("441632960000"));

	TRAP_IGNORE(SetSmspListL(newSmspEntry,sms));

	INFO_PRINTF1(_L("Check SMSP list contains new entry\n"));
 	TRAP_IGNORE(GetSmspListL(checkSmspEntry,sms));

	//Check SMSP list contains new entry 
	TESTL(checkSmspEntry.iValidParams == RMobileSmsMessaging::KSCAIncluded);
	TESTL(checkSmspEntry.iServiceCentre.iTypeOfNumber==RMobilePhone::EInternationalNumber);
	TESTL(checkSmspEntry.iServiceCentre.iNumberPlan==RMobilePhone:: EIsdnNumberPlan);
	TESTL(checkSmspEntry.iServiceCentre.iTelNumber==_L("441632960000"));

	INFO_PRINTF1(_L("Display SMSP list contains new entry\n"));

	DisplaySmspEntry(checkSmspEntry);

	INFO_PRINTF1(_L("Closing SMS Messaging...\n"));
	sms.Close();

	return (KErrNone);
	}


void CTestDriveSca::GetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry,RMobileSmsMessaging& aSms)
/**
 * This function tests CRetrieveMobilePhoneSmspList
 */
	{
	CTestGetSmspList* retrieveSmspList;
	CMobilePhoneSmspList* smspList;

// We have 	ActiveScheduler already.
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

	CleanupStack::PopAndDestroy();
	delete smspList;
//	delete testActiveScheduler;
//	User::After(1000000);		// Give user time to see the test results 
	}


void CTestDriveSca::SetSmspListL(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry,RMobileSmsMessaging& aSms)
/**
 * This function tests writing service centre address
 */
	{
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);

	smspList->AddEntryL(aSmspEntry);

	TRequestStatus reqStatus;

	aSms.StoreSmspListL(reqStatus, smspList);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	CleanupStack::PopAndDestroy(); // smspList;
	}

void CTestDriveSca::DisplaySmspEntry(RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry)
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

void CTestDriveSca::DisplayMobileAddr(RMobilePhone::TMobileAddress aMobileAddr)
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
