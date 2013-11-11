// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the main methods that test the Misc
// Functionality of the MMTSY
// 
//

/**
 @file
*/

#include "Te_Misc.h"

const TInt KOneSecond=1000000;		// Used in a time out function, 1 second (in microSeconds)


/*-- Server info Test -----------------------------------------------------------------*/

CServerInfo::CServerInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("ServerInfo"));
	}

TVerdict CServerInfo::doTestStepL( void )
/**
 * This function retrieves the number of phones supported by the loaded ETel Tsy
 * Modules. Also gets information associated with the phone and queries the Tsy
 * about its supported functionality.
 */
	{
	TInt numOfPhones;
	TESTL(iTelServer.EnumeratePhones(numOfPhones)==KErrNone);
	INFO_PRINTF2(_L("No. of phones supported by loaded ETel modules = %d"), numOfPhones);

	// Phone information
	RTelServer::TPhoneInfo phoneInfo;
	TEST_CHECKL(iTelServer.GetPhoneInfo(numOfPhones-1, phoneInfo),KErrNone, _L("Failed GetPhoneInfo()"));

	// Get the Network Type
	const TText* type=_S("Unknown");
	switch(phoneInfo.iNetworkType)
		{
		case RTelServer::ENetworkTypeWiredAnalog:				
			type=_S("Wired Analog Network"); break;
		case RTelServer::ENetworkTypeWiredDigital:
			type=_S("Wired Digital Network"); break;
		case RTelServer::ENetworkTypeMobileAnalog:
			type=_S("Mobile Analog Network"); break;
		case RTelServer::ENetworkTypeMobileDigital:
			type=_S("Mobile Digital Network"); break;
		default:
			type=_S("Unknown Network Type"); break;
		}

	INFO_PRINTF2(_L("Network Type : %s"), type);
	INFO_PRINTF2(_L("Phone's name : %S"), &phoneInfo.iName);
	INFO_PRINTF2(_L("No. of lines supported by the phone : %d"), phoneInfo.iNumberOfLines);
	INFO_PRINTF2(_L("Extensions supported by Tsy Module : %d"), phoneInfo.iExtensions);
	

	// Query TSY about supported functionality
	TBool result;
	TInt mixin=1; // same value used by etel\tetel\t_hyres1.cpp

	TBuf<10> tsyName;
	tsyName = KMmtsyName;

	TESTL(iTelServer.IsSupportedByModule(tsyName, mixin, result)==KErrNone);
	INFO_PRINTF1(_L("Querying TSY about supported functionality is Successful"));

	return TestStepResult();
	}




/*-- Signal And Battery Strength Test -------------------------------------------------*/

CSignalAndBatteryStrengthTest::CSignalAndBatteryStrengthTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SignalAndBatteryStrengthTest"));
	}

TVerdict CSignalAndBatteryStrengthTest::doTestStepL( void )	
/** 
 * This method retrieves a phone's Signal and Battery Strength Information.
 */
	{
	INFO_PRINTF1(_L("Get Signal And Battery Strength Information"));

	// Test Getting Battery caps
	INFO_PRINTF1(_L("Calling RMobilePhone::GetBatteryCaps()"));
	TUint32 aCaps;
	TEST_CHECKL(iPhone.GetBatteryCaps(aCaps), KErrNone, _L("Failed RMobilePhone::GetBatteryCaps()")) ;

	//Print Capabilities
	if(aCaps & RMobilePhone::KCapsGetBatteryInfo)
		{
		INFO_PRINTF1(_L("Phone supports requests to get the current battery info"));
		}
	if(aCaps & RMobilePhone::KCapsNotifyBatteryInfoChange)
		{
		INFO_PRINTF1(_L("Phone supports requests for notification of change in battery information."));
		}


	//Test Getting Signal caps
	INFO_PRINTF1(_L("Calling RMobilePhone::GetSignalCaps()"));
	TEST_CHECKL(iPhone.GetSignalCaps(aCaps), KErrNone, _L("Failed RMobilePhone::GetSignalCaps()"));

	//Print Capabilities
	if(aCaps & RMobilePhone::KCapsGetSignalStrength)
		{
		INFO_PRINTF1(_L("Phone supports requests to get the current signal strength"));
		}
	if(aCaps & RMobilePhone::KCapsNotifySignalStrengthChange)
		{
		INFO_PRINTF1(_L("Phone supports requests for notification of change in signal strength"));
		}


	//Test Getting Battery Info
	INFO_PRINTF1(_L("Calling RMobilePhone::GetBatteryInfo()"));
	RMobilePhone::TMobilePhoneBatteryInfoV1 battery;
	iPhone.GetBatteryInfo(iStatus, battery);
	User::WaitForRequest(iStatus);
	if (iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Battery status : %d")), battery.iStatus);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Battery charge level : %d")), battery.iChargeLevel);
		}
	else
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Battery charge, Error %d returned.")), iStatus.Int());
		SetTestStepResult(EFail);
		}


	//Get Signal Strength
	TInt32 signalStrength;
	TInt8 bars;

	INFO_PRINTF1(_L("Calling RMobilePhone::GetSignalStrength()"));
	iPhone.GetSignalStrength(iStatus, signalStrength, bars);
	User::WaitForRequest(iStatus);
	if (iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Signal strength (in dBm) : %d")), signalStrength);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Signal strength (in display bars) : %d")), bars);
		}

	else
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Signal strength, Error %d returned.")), iStatus.Int());
		SetTestStepResult(EFail);
		}


	//Test Cancelling requests
	INFO_PRINTF1(_L("Performing Cancel Requests for the above functions"));

	iPhone.GetBatteryInfo(iStatus, battery);
	iPhone.CancelAsyncRequest(EMobilePhoneGetBatteryInfo);
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetBatteryInfo Cancel status = %d")), iStatus.Int());
	User::After(1000000); //Wait for phone to produce response and for TSY to reject it

	iPhone.GetSignalStrength(iStatus, signalStrength, bars);
	iPhone.CancelAsyncRequest(EMobilePhoneGetSignalStrength);
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetSignalStrength Cancel status = %d")), iStatus.Int());
	User::After(1000000); //Wait for phone to produce response and for TSY to reject it

	return TestStepResult();
	}


/*-- Manufacturer Info Test -----------------------------------------------------------*/
CManufacturerInfoTest::CManufacturerInfoTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("ManufacturerInfoTest"));
	}

TVerdict CManufacturerInfoTest::doTestStepL( void )	
/** 
 * This method retrieves a phone's Signal and Battery Strength Information.
 */
	{
	INFO_PRINTF1(_L("Get Manufacturer ID, Model ID, Revision ID and Serial Number"));

	//Test Getting Phone Identity Caps
	INFO_PRINTF1(_L("Calling RMobilePhone::GetIdentityCaps()"));
	TUint32 aCaps;
	TInt ret = iPhone.GetIdentityCaps(aCaps);
	if(ret) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetIdentityCaps() returned %d"), ret);
		SetTestStepResult(EFail);
		}

	if(aCaps & RMobilePhone::KCapsGetManufacturer)
		{
		INFO_PRINTF1(_L("Phone can return the identity of its manufacturer"));
		}
	if(aCaps & RMobilePhone::KCapsGetModel)
		{
		INFO_PRINTF1(_L("Phone can return the identity of its model"));
		}
	if(aCaps & RMobilePhone::KCapsGetRevision)
		{
		INFO_PRINTF1(_L("Phone can return the identity of its revision"));
		}
	if(aCaps & RMobilePhone::KCapsGetSerialNumber)
		{
		INFO_PRINTF1(_L("Phone can return the identity of its serial number"));
		}
	if(aCaps & RMobilePhone::KCapsGetSubscriberId)
		{	
		INFO_PRINTF1(_L("Phone can return the identity of its subscriber"));
		}


	INFO_PRINTF1(_L("Calling RMobilePhone::GetPhoneId()"));
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(iStatus, phoneId);
	User::WaitForRequest(iStatus);

	if (iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Manufacturer ID : %S")), &phoneId.iManufacturer);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Model ID : %S")), &phoneId.iModel);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Revision ID : %S")), &phoneId.iRevision);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Serial number : %S")), &phoneId.iSerialNumber);
		}
	else
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Error RMobilePhone::GetPhoneId() returned %d")), iStatus.Int());
		SetTestStepResult(EFail);
		}
		
	return TestStepResult();	
	}

/*-- Subscriber ID Test -----------------------------------------------------------*/
CSubscriberIdTest::CSubscriberIdTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SubscriberIdTest"));
	}

TVerdict CSubscriberIdTest::doTestStepL( void )	
/** 
 * This method retrieves a phone's subscriber ID
 */
	{
	INFO_PRINTF1(_L("Get Subscriber ID"));

	// Get subscriber ID test
	INFO_PRINTF1(_L("Calling RMobilePhone::GetSubscriberId()"));
	RMobilePhone::TMobilePhoneSubscriberId subId;
	iPhone.GetSubscriberId(iStatus, subId);
	User::WaitForRequest(iStatus);
	
	// Do not consider KErrNotSupported return code as a test failure
	if(iStatus==KErrNotSupported)
		{	
		INFO_PRINTF1(_L("***Phone returned KErrNotSupported***"));
		}
	else if(iStatus!=KErrNone)
		{
		// Log that and error occurred
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetSubscriberId failed with status %d")), iStatus.Int());
		SetTestStepResult(EFail);
		}

	if(iStatus==KErrNone)
		{
		// Log the Subscriber ID returned
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("SubscriberId: %S")), &subId);
		}
							
	
	// Get subscriber ID and then cancel test
	iPhone.GetSubscriberId(iStatus, subId);
	iPhone.CancelAsyncRequest(EMobilePhoneGetSubscriberId);
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("RMobilePhone::GetSubscriberIdCancel status = %d")), iStatus.Int());

	return TestStepResult();
	}



/*-- Phone Store Test -----------------------------------------------------------------*/

CPhoneStoreTest::CPhoneStoreTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("PhoneStoreTest"));
	}

TVerdict CPhoneStoreTest::doTestStepL( void )	
/** 
 * This method retrieves various phone store information
 */	

	{
	// Get phone store info for various phonebook
	INFO_PRINTF1(_L("Test the Phone Stores"));

	//Get phone store info for IccAdnPhoneBook
	INFO_PRINTF1(_L("Calling RMobilePhone::GetPhoneStoreInfo() for KETelIccAdnPhoneBook"));
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg(info);
	iPhone.GetPhoneStoreInfo(iStatus, infoPckg, KETelIccAdnPhoneBook);
	User::WaitForRequest(iStatus);

	if(iStatus==KErrNone)
		{	
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Store Name: %S")), &info.iName);
		TESTL(info.iType==RMobilePhoneStore::EPhoneBookStore);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of used entries: %d")), info.iUsedEntries);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of total entries: %d")), info.iTotalEntries);
		}
	else if (iStatus==KErrNotSupported)
		{
		INFO_PRINTF1(_L("***Phone returned KErrNotSupported***"));
		}
	else 
		{
		// Log that and error occurred
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetPhoneStoreInfo failed with status %d")), iStatus.Int());
		SetTestStepResult(EFail);
		}

	//Get phonebook store info for MeAdnPhoneBook
	INFO_PRINTF1(_L("Calling RMobilePhone::GetPhoneStoreInfo() for KETelMeAdnPhoneBook"));
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phbkInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phbkInfoPckg(phbkInfo);
	iPhone.GetPhoneStoreInfo(iStatus, phbkInfoPckg, KETelMeAdnPhoneBook);
	User::WaitForRequest(iStatus);

	if(iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Store Name: %S")), &phbkInfo.iName);
		TESTL(phbkInfo.iType==RMobilePhoneStore::EPhoneBookStore);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of used entries: %d")), phbkInfo.iUsedEntries);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of total entries: %d")), phbkInfo.iTotalEntries);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Max text length: %d")), phbkInfo.iMaxTextLength);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Max number length: %d")), phbkInfo.iMaxNumLength);
		}
	else if(iStatus==KErrNotSupported)
		{
		INFO_PRINTF1(_L("***Phone returned KErrNotSupported***"));
		}
	else
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetPhoneStoreInfo failed with status %d")), iStatus.Int());
		SetTestStepResult(EFail);
		}

	// Get phone store info for KETelOwnNumberStore
	INFO_PRINTF1(_L("Calling RMobilePhone::GetPhoneStoreInfo() for KETelOwnNumberStore"));
	iPhone.GetPhoneStoreInfo(iStatus, infoPckg, KETelOwnNumberStore);
	User::WaitForRequest(iStatus);

	if(iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Store Name: %S")), &info.iName);
		TESTL(info.iType==RMobilePhoneStore::EOwnNumberStore);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of used entries: %d")), info.iUsedEntries);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of total entries: %d")), info.iTotalEntries);
		}
	else if(iStatus==KErrNotSupported)
		{
		INFO_PRINTF1(_L("***Phone returned KErrNotSupported***"));
		}
	else
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetPhoneStoreInfo failed with status %d")), iStatus.Int());
		SetTestStepResult(EFail);
		}

	//Get own number store information
	INFO_PRINTF1(_L("Calling RMobilePhone::GetPhoneStoreInfo() for KETelOwnNumberStore..."));
	INFO_PRINTF1(_L("...but this time passing in a TMobileONStoreInfoV1Pckg"));
	RMobileONStore::TMobileONStoreInfoV1 onInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg onInfoPckg(onInfo);
	iPhone.GetPhoneStoreInfo(iStatus, onInfoPckg, KETelOwnNumberStore);
	User::WaitForRequest(iStatus);	

	if(iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Store Name: %S")), &onInfo.iName);
		TESTL(onInfo.iType==RMobilePhoneStore::EOwnNumberStore);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of used entries: %d")), onInfo.iUsedEntries);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of total entries: %d")), onInfo.iTotalEntries);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Max text length: %d")), onInfo.iTextLen);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Max number length: %d")), onInfo.iNumberLen);
		}
	else if(iStatus==KErrNotSupported)
		{
		INFO_PRINTF1(_L("***Phone returned KErrNotSupported***"));
		}
	else
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetPhoneStoreInfo failed with status %d")), iStatus.Int());
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}


/*-- Own Number Test ------------------------------------------------------------------*/

COwnNumberTest::COwnNumberTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("OwnNumberTest"));
	}

TVerdict COwnNumberTest::doTestStepL( void )	
/** 
 * This method test own number storage
 */	
	{
	INFO_PRINTF1(_L("Own number tests"));
	
	RMobileONStore iOwnNumberStore;

	// Check that TSY supports Own Number Store
	TBool supported;
	TInt ret;
	
	ret = iTelServer.IsSupportedByModule(KMmtsyName, KETelFuncMobileOwnNumberStore, supported);

	if ((ret==KErrNone) && (supported))
		{
		// Open RMobileONStore
		ret=iOwnNumberStore.Open(iPhone);

		if (ret!=KErrNone)
			{
			INFO_PRINTF2(_L("Failed to open Own Number store (%d)"), ret);
			return EFail;
			}

		// Get Own Number Information
		RMobileONStore::TMobileONStoreInfoV1 ownNumberInfo;
		RMobileONStore::TMobileONStoreInfoV1Pckg ownNumberInfoPckg(ownNumberInfo);
	
		iOwnNumberStore.GetInfo(iStatus, ownNumberInfoPckg);
		User::WaitForRequest(iStatus);
	
		if ((iStatus==KErrNone) && (ownNumberInfo.iType==RMobilePhoneStore::EOwnNumberStore))
			{
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("Store Name: %S")), &ownNumberInfo.iName);
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("Store Caps: %d")), ownNumberInfo.iCaps);
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of Entries: %d")), ownNumberInfo.iUsedEntries);
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("Maximum no. of entries: %d")), ownNumberInfo.iTotalEntries);
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("Max. length of tel.no: %d")), ownNumberInfo.iNumberLen);
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("Max. length of text: %d")), ownNumberInfo.iTextLen);
			}
		else if (iStatus==KErrNotSupported)
			{
			INFO_PRINTF1(TRefByValue<const TDesC>(_L("GetOwnNumberInfo not supported by this phone")));
			}
		else
			{
			INFO_PRINTF3(TRefByValue<const TDesC>(_L("GetOwnNumberInfo failed - status: %d, store type: %d")), iStatus.Int(), ownNumberInfo.iType);
			SetTestStepResult(EFail);
			}

		// Own Number Entry
		RMobileONStore::TMobileONEntryV1 ownNumberEntry;
		RMobileONStore::TMobileONEntryV1Pckg ownNumberEntryPckg(ownNumberEntry);

		for (TInt i=1; i<=ownNumberInfo.iTotalEntries; i++)
			{
			ownNumberEntry.iIndex = i;
	
			iOwnNumberStore.Read(iStatus, ownNumberEntryPckg);
			User::WaitForRequest(iStatus);
		
			if (iStatus==KErrNone)
				{	
				INFO_PRINTF2(_L("GetOwnNumberEntry (%d) was successful"),i);
				INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number: %S")), &ownNumberEntry.iNumber.iTelNumber);
				}
			else if (iStatus==KErrNotFound)
				{
				INFO_PRINTF2(_L("No Own Number Entry (%d) was found."),i);
				}
			else if (iStatus==KErrNotSupported)
				{
				INFO_PRINTF1(TRefByValue<const TDesC>(_L("GetOwnNumberEntry not supported by this phone")));
				}
			else
				{	
				INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetOwnNumberEntry status %d")), iStatus.Int());
				}
			}

		// Close and reopen store to test read before getinfo request
		iOwnNumberStore.Close();
		ret=iOwnNumberStore.Open(iPhone);
		if (ret!=KErrNone)
			{
			INFO_PRINTF2(_L("Failed to open Own Number store (%d)"), ret);
			SetTestStepResult(EFail);
			}

		// Own Number Entry
		RMobileONStore::TMobileONEntryV1 ownNumberEntry2;
		ownNumberEntry2.iIndex = 2;
	
		RMobileONStore::TMobileONEntryV1Pckg ownNumberEntryPckg2(ownNumberEntry2);
	
		iOwnNumberStore.Read(iStatus, ownNumberEntryPckg2);
		User::WaitForRequest(iStatus);
		
		if (iStatus==KErrNone)
			{	
			INFO_PRINTF1(_L("GetOwnNumberEntry (2) was successful"));
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number: %S")), &ownNumberEntry2.iNumber.iTelNumber);
			}
		else if (iStatus==KErrNotFound)
			{
			INFO_PRINTF1(_L("No Own Number Entry (2) was found."));
			}
		else if (iStatus==KErrNotSupported)
			{
			INFO_PRINTF1(TRefByValue<const TDesC>(_L("GetOwnNumberEntry not supported by this phone")));
			}
		else
			{
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetOwnNumberEntry (index 2) status %d")), iStatus.Int());			
			}


		//Test Cancel Requests
		INFO_PRINTF1(_L("Performing Cancel Requests for the above functions"));
		iOwnNumberStore.GetInfo(iStatus, ownNumberInfoPckg);
		iOwnNumberStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
		User::WaitForRequest(iStatus);
		if (iStatus!=KErrNotSupported)
			{
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("RMobileONStore::GetInfoCancel status = %d")), iStatus.Int());
			}

		ownNumberEntry2.iIndex = 1;	
		iOwnNumberStore.Read(iStatus, ownNumberEntryPckg2);
		iOwnNumberStore.CancelAsyncRequest(EMobilePhoneStoreRead);
		User::WaitForRequest(iStatus);
		if (iStatus!=KErrNotSupported)
			{
			INFO_PRINTF2(TRefByValue<const TDesC>(_L("RMobileONStore::ReadCancel status = %d")), iStatus.Int());
			}

		iOwnNumberStore.Close();
		}
	else
		{
		INFO_PRINTF1(_L("TSY does not support Own Number store"));
		}

	return TestStepResult();
	}


/*-- Get Caps Test --------------------------------------------------------------------*/

CGetCaps::CGetCaps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetCaps"));
	}

TVerdict CGetCaps::doTestStepL( void )	
/** 
 * This method test own number storage
 */	
	{
	INFO_PRINTF1(_L("Get Phone Capabilities"));
	
	TInt ret;

	// Get Caps and print them
	INFO_PRINTF1(_L("Calling RMobilePhone::GetCaps()"));
	RPhone::TCaps aCaps;
	ret = iPhone.GetCaps(aCaps);
	if(ret) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetCaps() returned: %d"), ret);
		SetTestStepResult(EFail);
		}
	if(aCaps.iFlags & RPhone::KCapsUnknown)
		{
		INFO_PRINTF1(_L("The capabilities of the phone are not currently known"));
		}
	if(aCaps.iFlags & RPhone::KCapsData)
		{
		INFO_PRINTF1(_L("The phone supports data calls"));
		}
	if(aCaps.iFlags & RPhone::KCapsFaxClassOne)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 1 interface"));
		}
	if(aCaps.iFlags &	RPhone::KCapsFaxClassOnePointZero)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 1.0 interface"));
		}
	if(aCaps.iFlags & RPhone::KCapsFaxClassTwo)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 2 interface"));
		}
	if(aCaps.iFlags & RPhone::KCapsFaxClassTwoPointZero)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 2.0 interface"));
		}
	if(aCaps.iFlags & RPhone::KCapsFaxClassTwoPointOne)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 2.1 interface"));
		}
	if(aCaps.iFlags & RPhone::KCapsVoice)
		{
		INFO_PRINTF1(_L("The phone supports voice calls"));
		}
	if(aCaps.iFlags & RPhone::KCapsEventModemDetection)
		{
		INFO_PRINTF1(_L("The phone supports modem detection events"));
		}
	if(aCaps.iFlags & RPhone::KCapsStealCommPort)
		{
		INFO_PRINTF1(_L("The phone supports the ability to take the comms port"));
		}


	// Gets Status and prints info
	INFO_PRINTF1(_L("Calling RMobilePhone::GetStatus()"));	
	RPhone::TStatus phoneStatus;
	ret = iPhone.GetStatus(phoneStatus);
	if(ret) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetStatus() returned %d"), ret);
		SetTestStepResult(EFail);
		}

	switch(phoneStatus.iMode)
		{
		case RPhone::EModeUnknown:
			INFO_PRINTF1(_L("The phone is currently in an unknown state"));
			break;

		case RPhone::EModeIdle:
			INFO_PRINTF1(_L("The phone is currently idle"));
			break;

		case RPhone::EModeEstablishingLink:
			INFO_PRINTF1(_L("The phone is currently trying to establish a call"));
			break;

		case RPhone::EModeOnlineData:
			INFO_PRINTF1(_L("The phone is in the connected and \"on-line data\" state"));
			break;

		case RPhone::EModeOnlineCommand:
			INFO_PRINTF1(_L("The phone is in the connected and \"on-line command\" state"));
			break;

		}

	//Get and print info
	INFO_PRINTF1(_L("Calling RMobilePhone::GetInfo()"));
	RPhone::TPhoneInfo phoneInfo;
	ret = iPhone.GetInfo(phoneInfo);
	if(ret) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetInfo() returned %d"), ret);
		SetTestStepResult(EFail);
		}

	switch(phoneInfo.iDetection)
		{
		case RPhone::EDetectedPresent:
			INFO_PRINTF1(_L("The local modem is present"));
			break;

		case RPhone::EDetectedNotPresent:
			INFO_PRINTF1(_L("The local modem is not present"));
			break;

		case RPhone::EDetectedUnknown:
			INFO_PRINTF1(_L("Status of the local modem not detected"));
			break;
		}

	return TestStepResult();
	}


/*-- Get Lines Status Test ------------------------------------------------------------*/

CGetLinesStatus::CGetLinesStatus()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetLinesStatus"));
	}

TVerdict CGetLinesStatus::doTestStepL( void )	
/** 
 * This method tests getting the line status
 */	
	{
	INFO_PRINTF1(_L("Get Lines Status"));

	TInt aCount;
	TInt i;					// Loop counter
	RPhone::TLineInfo	lineInfo;

	//Test Enumeratting the lines
	INFO_PRINTF1(_L("Calling RPhone::EnumerateLines()"));
	TInt ret = iPhone.EnumerateLines(aCount);
	if(ret) 
		{
		INFO_PRINTF2(_L("RPhone::EnumerateLines() returned %d"), ret);
		SetTestStepResult(EFail);
		}
	
	INFO_PRINTF2(_L("There are %d lines on this phone"), aCount);

	INFO_PRINTF1(_L("Calling RPhone::GetLineInfo()"));
	for(i=0;i<aCount;i++)
		{
		ret = iPhone.GetLineInfo(i, lineInfo);
		if(ret) 
			{
			INFO_PRINTF2(_L("RPhone::GetLineInfo() returned %d"), ret);
			SetTestStepResult(EFail);
			}

		switch(lineInfo.iStatus)
			{
			case RCall::EStatusUnknown:
				INFO_PRINTF3(_L("Line index %d: %S, Status: The call status is not currently known"), i+1, &lineInfo.iName);
				break;

			case RCall::EStatusIdle:
				INFO_PRINTF3(_L("Line index %d: %S, Status: The call is idle"), i+1, &lineInfo.iName);
				break;
			
			case RCall::EStatusDialling:   
				INFO_PRINTF3(_L("Line index %d: %S, Status: The call is dialling."), i+1, &lineInfo.iName);
				break;

			case RCall::EStatusRinging:
				INFO_PRINTF3(_L("Line index %d: %S, Status: The call is ringing "), i+1, &lineInfo.iName);
				break;

			case RCall::EStatusAnswering:
				INFO_PRINTF3(_L("Line index %d: %S, Status: The call is being answered"), i+1, &lineInfo.iName);
				break;

			case RCall::EStatusConnecting:
				INFO_PRINTF3(_L("Line index %d: %S, Status: The call is connecting"), i+1, &lineInfo.iName);
				break;

			case RCall::EStatusConnected:
				INFO_PRINTF3(_L("Line index %d: %S, Status: The call is connected and active"), i+1, &lineInfo.iName);
				break;

			case RCall::EStatusHangingUp:
				INFO_PRINTF3(_L("Line index %d: %S, Status: The call is being terminated"), i+1, &lineInfo.iName);
				break;

			}

		// Waits a bit
		User::After( 3 * KOneSecond);

		if(lineInfo.iLineCapsFlags & RLine::KCapsData)
			{
			INFO_PRINTF1(_L("The line supports data connections"));
			}	
		if(lineInfo.iLineCapsFlags & RLine::KCapsFax)
			{
			INFO_PRINTF1(_L("The line supports fax connections"));
			}
		if(lineInfo.iLineCapsFlags & RLine::KCapsVoice)
			{
			INFO_PRINTF1(_L("The line supports voice connections"));
			}
		if(lineInfo.iLineCapsFlags & RLine::KCapsEventIncomingCall)
			{
			INFO_PRINTF1(_L("The line supports incoming call notification requests"));
			}
		}

	return TestStepResult();
	}

/*-- Get Multimode Caps Test ----------------------------------------------------------*/

CGetMultimodeCaps::CGetMultimodeCaps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetMultimodeCaps"));
	}

TVerdict CGetMultimodeCaps::doTestStepL( void )	
/** 
 * This method tests getting the phone's multimode capabilities
 */	
	{
	INFO_PRINTF1(_L("Testing the phone's Multimode caps"));
	
	INFO_PRINTF1(_L("Calling RMobilePhone::GetMultimodeCaps()"));
	TUint32 aCaps;
	TInt ret = iPhone.GetMultimodeCaps(aCaps);
	if(ret) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetMultimodeCaps returned %d"), ret);
		return EFail;
		}

	if(aCaps & RMobilePhone::KCapsGsmSupported)
		{
		INFO_PRINTF1(_L("Phone can operate in GSM 900/1800/1900 mode"));
		}
	if(aCaps & RMobilePhone::KCapsGprsSupported)
		{
		INFO_PRINTF1(_L("Phone can operate in GPRS mode"));
		}
	if(aCaps & RMobilePhone::KCapsAmpsSupported)
		{
		INFO_PRINTF1(_L("Phone can operate in AMPS 800 mode"));
		}
	if(aCaps & RMobilePhone::KCapsCdma95Supported)
		{
		INFO_PRINTF1(_L("Phone can operate in CDMA95 800/1900 mode"));
		}
	if(aCaps & RMobilePhone::KCapsCdma2000Supported)
		{
		INFO_PRINTF1(_L("Phone can operate in CDMA2000 800/1900 mode"));
		}
	if(aCaps & RMobilePhone::KCapsWcdmaSupported)
		{
		INFO_PRINTF1(_L("Phone can operate in W-CDMA mode"));
		}

	return EPass;
	}


/*-- Get Network Caps Test ------------------------------------------------------------*/

CGetNetworkCaps::CGetNetworkCaps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetNetworkCaps"));
	}

TVerdict CGetNetworkCaps::doTestStepL( void )	
/** 
 * This method tests getting Network Capabilities
 */	
	{
	INFO_PRINTF1(_L("Get Network Caps"));

	INFO_PRINTF1(_L("Calling RMobilePhone::GetNetworkCaps()"));
	TUint32 aCaps;
	TInt ret = iPhone.GetNetworkCaps(aCaps);
	if(ret==KErrNotSupported)	// Do not consider KErrNotSupported as a test failure
		{
		INFO_PRINTF1(_L("***Phone returned KErrNotSupported***"));
		}
	else if(ret!=KErrNone) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetNetworkCaps() returned %d/n"), ret);
		return EFail;
		}

	if(aCaps & RMobilePhone::KCapsGetRegistrationStatus)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of current registration status"));
		}
	if(aCaps & RMobilePhone::KCapsNotifyRegistrationStatus)
		{
		INFO_PRINTF1(_L("The phone supports notifications of change in registration status"));
		}
	if(aCaps & RMobilePhone::KCapsGetCurrentMode)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of current mode"));
		}
	if(aCaps & RMobilePhone::KCapsNotifyMode)
		{
		INFO_PRINTF1(_L("The phone supports notification of change in mode"));
		}
	if(aCaps & RMobilePhone::KCapsGetCurrentNetwork)
		{
		INFO_PRINTF1(_L("phone supports retrieval of current network information"));
		}
	if(aCaps & RMobilePhone::KCapsNotifyCurrentNetwork)
		{
		INFO_PRINTF1(_L("The phone supports notification of change of current network"));
		}
	if(aCaps & RMobilePhone::KCapsGetHomeNetwork)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of home network information"));
		}
	if(aCaps & RMobilePhone::KCapsGetDetectedNetworks)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of a list of detected networks"));
		}
	if(aCaps & RMobilePhone::KCapsManualNetworkSelection)
		{
		INFO_PRINTF1(_L("The phone supports manual network selection mode"));
		}
	if(aCaps & RMobilePhone::KCapsGetNITZInfo)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of time/date from the network"));
		}
	if(aCaps & RMobilePhone::KCapsNotifyNITZInfo)
		{	
		INFO_PRINTF1(_L("The phone supports notification of new updates of time/date from the network"));
		}


	return EPass;

	}


/*-- Get Current Mode Test ------------------------------------------------------------*/
CGetCurrentMode::CGetCurrentMode()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetCurrentMode"));
	}

TVerdict CGetCurrentMode::doTestStepL( void )	
/** 
 * This method tests getting the current mode that the phone is in
 */	
	{
	INFO_PRINTF1(_L("Testing the current mode and notification"));
	
	INFO_PRINTF1(_L("Calling RMobilePhone::GetCurrentMode()"));
	RMobilePhone::TMobilePhoneNetworkMode netMode;
	TInt ret = iPhone.GetCurrentMode(netMode);
	if(ret==KErrNotSupported)		// Do not consider KErrNotSupported as a test failure
		{
		INFO_PRINTF1(_L("***Phone returned KErrNotSupported***"));
		}
	else if(ret!=KErrNone)
		{	
		// Log error code and return with error code so that AutoTest logs this as a test failure
		INFO_PRINTF2(_L("RMobilePhone::GetCurrentMode() returned %d"), ret);
		return EFail;
		}

	if(ret==KErrNone)
		{
		switch(netMode)
			{
		case RMobilePhone::	ENetworkModeUnregistered:
			INFO_PRINTF1(_L("ME is not registered."));
			break;
		case RMobilePhone::ENetworkModeGsm:
			INFO_PRINTF1(_L("GSM or DCS1800 network."));
			break;
		case RMobilePhone::ENetworkModeAmps:
			INFO_PRINTF1(_L("AMPS network"));
			break;
		case RMobilePhone::ENetworkModeCdma95:
			INFO_PRINTF1(_L("CDMA95 network."));
			break;
		case RMobilePhone::ENetworkModeCdma2000:
			INFO_PRINTF1(_L("CDMA2000 network."));
			break;
		case RMobilePhone::ENetworkModeWcdma:
			INFO_PRINTF1(_L("WCDMA network"));
		case RMobilePhone::ENetworkModeTdcdma:
			INFO_PRINTF1(_L("TD-CDMA network"));
		case RMobilePhone::ENetworkModeUnknown:
		default:
			INFO_PRINTF1(_L("Network mode is unknown"));
			break;
			}
		}
	
	return EPass;
	}


/*-- Async Network List Retrieval Test ------------------------------------------------*/

CGetAsyncNetList::CTestAsyncGetNetList* CGetAsyncNetList::CTestAsyncGetNetList::NewLC(RMobilePhone& aPhone)
	{
    CTestAsyncGetNetList* r=new(ELeave) CTestAsyncGetNetList(aPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CGetAsyncNetList::CTestAsyncGetNetList::CTestAsyncGetNetList(RMobilePhone& aPhone)
    : CActive(EPriorityNormal), iPhone(aPhone), iCompleteCode(KErrNone)
    {
	}
      
void CGetAsyncNetList::CTestAsyncGetNetList::ConstructL()
	{
	CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
    }
         
CGetAsyncNetList::CTestAsyncGetNetList::~CTestAsyncGetNetList()
    {
	delete iRetrieve;
	}
      
void CGetAsyncNetList::CTestAsyncGetNetList::Start()
    {
    iRetrieve->Start(iStatus);
    SetActive();
    }       

CMobilePhoneNetworkList* CGetAsyncNetList::CTestAsyncGetNetList::RetrieveList()
	{
	CMobilePhoneNetworkList* ptr=NULL;
	TRAP_IGNORE(ptr=iRetrieve->RetrieveListL()); // trap and ignore err
	return ptr;
	}
          
void CGetAsyncNetList::CTestAsyncGetNetList::RunL()
	{
	iCompleteCode=iStatus.Int();
	CActiveScheduler::Stop();
	}	


TInt CGetAsyncNetList::CTestAsyncGetNetList::CompleteCode()
	{
	return iCompleteCode;
	}

void CGetAsyncNetList::CTestAsyncGetNetList::DoCancel()
    {
    iRetrieve->Cancel();
    }

CGetAsyncNetList::CGetAsyncNetList()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetAsyncNetList"));
	}

TVerdict CGetAsyncNetList::doTestStepL()
/** 
 * This method tests asynchronously getting a list of detected networks
 */	
	{
	INFO_PRINTF1(_L("Testing Async retrieval of Network list"));

	iTestScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);

	CTestAsyncGetNetList* netRetrieveList = CTestAsyncGetNetList::NewLC(iPhone);	
	
	INFO_PRINTF1(_L("Starting ActiveObject CRetrieveMobilePhoneDetectedNetworks"));
	netRetrieveList->Start();
	CActiveScheduler::Start();


	// Get completion code from netRetrieveList
	const TInt completeCode = netRetrieveList->CompleteCode();
	if(completeCode==KErrNotSupported)		// Do not consider KErrNotSupported a test failure
		{
		INFO_PRINTF1(_L("***Phone returned KErrNotSupported***"));
		}
	else if(completeCode!=KErrNone)
		{
		// Log that and error occurred
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Failed with error code %d")),completeCode);
		return EFail;
		}


	TInt i;			// Loop counter
	CMobilePhoneNetworkList* retList=NULL;
	RMobilePhone::TMobilePhoneNetworkInfoV1 anEntry;

	if(completeCode==KErrNone)
		{
		retList = netRetrieveList->RetrieveList();
		const TInt count = retList->Enumerate();
		INFO_PRINTF2(_L("%d networks detected"), count);

		for(i=0; i<count; i++)
			{
			anEntry = retList->GetEntryL(i);	
			INFO_PRINTF3(_L("Network %d: %S"), i+1, &anEntry.iLongName);
			INFO_PRINTF1(_L("Mode of the network: "));
		
			switch(anEntry.iMode)
				{
			case RMobilePhone::ENetworkModeUnregistered:
				INFO_PRINTF1(_L("ME is not registered."));
				break;
			case RMobilePhone::ENetworkModeGsm:
				INFO_PRINTF1(_L("GSM or DCS1800 network."));
				break;
			case RMobilePhone::ENetworkModeAmps:
				INFO_PRINTF1(_L("	AMPS network"));
				break;
			case RMobilePhone::ENetworkModeCdma95:
				INFO_PRINTF1(_L("CDMA95 network"));
				break;
			case RMobilePhone::ENetworkModeCdma2000:
				INFO_PRINTF1(_L("CDMA2000 network"));
				break;
			case RMobilePhone::ENetworkModeWcdma:
				INFO_PRINTF1(_L("WCDMA network."));
				break;
			case RMobilePhone::ENetworkModeTdcdma:
				INFO_PRINTF1(_L("TD-CDMA network."));
				break;
			case RMobilePhone::ENetworkModeUnknown:
			default:
				INFO_PRINTF1(_L("Network mode is unknown."));
				break;
				}
	
			INFO_PRINTF1(_L("Status of the network: "));
			switch(anEntry.iStatus)
				{
			case RMobilePhone::ENetworkStatusUnknown:
				INFO_PRINTF1(_L("Status is unknown"));
				break;
			case RMobilePhone::ENetworkStatusAvailable:
				INFO_PRINTF1(_L("A network that the ME is allowed to register to"));
				break;
			case RMobilePhone::ENetworkStatusCurrent:
				INFO_PRINTF1(_L("The currently registered network."));
				break;
			case RMobilePhone::ENetworkStatusForbidden:
				INFO_PRINTF1(_L("A network that the ME is not allowed to register to"));
				break;
				}

			INFO_PRINTF2(_L("Country code: %S"), &anEntry.iCountryCode);
			INFO_PRINTF2(_L("The system identity (SID) of the CDMA network: %S"), &anEntry.iCdmaSID);
			INFO_PRINTF2(_L("The system identity (SID) of the AMPS network: %S"), &anEntry.iAnalogSID);
			INFO_PRINTF2(_L("The network identity (NID in CDMA and MNC in GSM): %S"), &anEntry.iNetworkId);
			INFO_PRINTF2(_L("The alpha-tag displayed when this is the serving network: %S"), &anEntry.iDisplayTag);
			INFO_PRINTF2(_L("The short name (up to 8 characters) of the network operator: %S"), &anEntry.iShortName);
			INFO_PRINTF2(_L("The long name (up to 16 characters) of the network operator.: %S"), &anEntry.iLongName);
			}

		}
	CleanupStack::PopAndDestroy(); // netRetrieveList;
	delete retList;
	delete iTestScheduler;
	return EPass;
	}


/*-- Functional Unit Support Test -----------------------------------------------------*/
CIsSupportedByModuleTest::CIsSupportedByModuleTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("IsSupportedByModuleTest"));
	}


TBool CIsSupportedByModuleTest::doIsSupportedByModuleTest(TInt aMixin)
	{
	TBool isSupported(EFalse);
	TInt ret=iTelServer.IsSupportedByModule(KMmtsyName,aMixin,isSupported);
	if(ret==KErrNone)
		{
		INFO_PRINTF2(_L("%d functional unit is supported!"), aMixin);
		return ETrue;		
		}		
	else
		{
		INFO_PRINTF2(_L("%d functional unit is NOT supported"), aMixin);
		return EFalse;
		}
	}

TVerdict CIsSupportedByModuleTest::doTestStepL( void )	
/** 
 * This method tests is certain Functional Units are supported
 */	
	{
	TBool isSupported(EFalse);
	
	isSupported = doIsSupportedByModuleTest(KETelFuncMobileNetwork);
	if(!isSupported)
		{
		SetTestStepResult(EFail);
		}

	isSupported = doIsSupportedByModuleTest(KETelFuncMobileIdentity);
	if(!isSupported)
		{
		SetTestStepResult(EFail);
		}

	isSupported = doIsSupportedByModuleTest(KETelFuncMobilePower);
	if(!isSupported)
		{
		SetTestStepResult(EFail);
		}

	isSupported = doIsSupportedByModuleTest(KETelFuncMobileSignal);
	if(!isSupported)
		{
		SetTestStepResult(EFail);
		}

	isSupported = doIsSupportedByModuleTest(KETelFuncMobileDataCall);
	if(!isSupported)
		{
		SetTestStepResult(EFail);
		}

	isSupported = doIsSupportedByModuleTest(KETelFuncMobileSmsMessaging);
	if(!isSupported)
		{
		SetTestStepResult(EFail);
		}

	isSupported = doIsSupportedByModuleTest(KETelFuncMobilePhonebook);
	if(!isSupported)
		{
		SetTestStepResult(EFail);
		}
	isSupported = doIsSupportedByModuleTest(KETelFuncMobileSmsStore);
	if(!isSupported)
		{
		SetTestStepResult(EFail);
		}
	isSupported = doIsSupportedByModuleTest(KETelFuncMobileOwnNumberStore);
	if(!isSupported)
		{
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}
