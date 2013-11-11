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
//

/**
 @file
*/

#include "Te_SimMisc.h"

#include "../../inc/simtsyglobalproperties.h"

const TInt KOneSecond=1000000;	// Used in a time out function, 1 second (in microSeconds)
 

//------------------------------------------------------------------
//			** TEST SimMisc STEPS ***

//------------------------------------------------------------------

CSignalAndBatteryStrengthTest::CSignalAndBatteryStrengthTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SignalAndBatteryStrengthTest"));
	}
//------------------------------------------------------------------

TVerdict CSignalAndBatteryStrengthTest::doTestStepL( void )
/** 
 * This method retrieves a phone's Signal and Battery Strength Information.
 */
	{
    INFO_PRINTF1(_L("Get Signal Strength Information"));

	// Get Signal caps

	INFO_PRINTF1(_L("Calling RMobilePhone::GetSignalCaps()"));
	TUint32 capabilities;
	TInt ret = iPhone.GetSignalCaps(capabilities);
	if(ret) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetSignalCaps() returned %d"), ret);
		return EFail;
		}

	if(capabilities & RMobilePhone::KCapsGetSignalStrength)
		{
		INFO_PRINTF1(_L("Phone supports requests to get the current signal strength"));
		}
	if(capabilities & RMobilePhone::KCapsNotifySignalStrengthChange)
		{
		INFO_PRINTF1(_L("Phone supports requests for notification of change in signal strength"));
		}

	TInt32 signalStrength;
	TInt8 bars;
	RMobilePhone::TMobilePhoneBatteryInfoV1 battery;
	// Get Battery Caps

	INFO_PRINTF1(_L("Calling RMobilePhone::GetBatteryCaps()"));
	ret = iPhone.GetBatteryCaps(capabilities);
	if (ret)
		{
		INFO_PRINTF2(_L("RMobilePhone::GetBatteryCaps() returned: %d"), ret);
		return EFail;
		}

	if(capabilities & RMobilePhone::KCapsGetBatteryInfo)
		INFO_PRINTF1(_L("Phone supports requests to get the current battery info"));
	if(capabilities & RMobilePhone::KCapsNotifyBatteryInfoChange)
		INFO_PRINTF1(_L("Phone supports requests for notification of change in battery information."));

	INFO_PRINTF1(_L("Calling RMobilePhone::GetSignalStrength()"));
	iPhone.GetSignalStrength(iStatus, signalStrength, bars);
	User::WaitForRequest(iStatus);
	if (iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Signal strength (in dBm) : %d")), signalStrength);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Signal strength (in display bars) : %d")), bars);
		}
	else{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Signal strength, Error %d returned.")), ret);
		return EFail;
		}


	INFO_PRINTF1(_L("Calling RMobilePhone::GetBatteryInfo()"));
	iPhone.GetBatteryInfo(iStatus, battery);
	User::WaitForRequest(iStatus);
	if (iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Battery status : %d")), battery.iStatus);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Battery charge level : %d")), battery.iChargeLevel);
		}
	else{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Battery charge, Error %d returned.")), ret);
		return EFail;
	}


	// Cancel Requests
	INFO_PRINTF1(_L("Performing Cancel Requests for the above functions"));

	iPhone.GetSignalStrength(iStatus, signalStrength, bars);
	iPhone.CancelAsyncRequest(EMobilePhoneGetSignalStrength);
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetSignalStrength Cancel status = %d")), iStatus.Int());

	return TestStepResult();
	}
//------------------------------------------------------------------

CSubscriberIdTest::CSubscriberIdTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SubscriberIdTest"));
	}
//------------------------------------------------------------------

TVerdict CSubscriberIdTest::doTestStepL( void )
	{
    INFO_PRINTF1(_L("Get Subscriber ID"));

	RMobilePhone::TMobilePhoneSubscriberId subId;
	iPhone.GetSubscriberId(iStatus, subId);
	User::WaitForRequest(iStatus);
	if (iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("SubscriberId: %S")), &subId);
		}
	else
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetSubscriberId status %d")), iStatus.Int());
		return EFail;
		}
	
	iPhone.GetSubscriberId(iStatus, subId);
	iPhone.CancelAsyncRequest(EMobilePhoneGetSubscriberId);
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("RMobilePhone::GetSubscriberIdCancel status = %d")), iStatus.Int());
	return TestStepResult();
	}
//------------------------------------------------------------------

CPhoneStoreTest::CPhoneStoreTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("PhoneStoreTest"));
	}
//------------------------------------------------------------------

TVerdict CPhoneStoreTest::doTestStepL( void )
	{
	// Get phone store info for various phonebooks
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg(info);

    INFO_PRINTF1(_L("Test the phone stores"));

	iPhone.GetPhoneStoreInfo(iStatus, infoPckg, KETelIccAdnPhoneBook);
	User::WaitForRequest(iStatus);
	if(iStatus.Int())
		{
		return EFail;
		}
	

	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Store Name: %S")), &info.iName);
	TESTL(info.iType==RMobilePhoneStore::EPhoneBookStore);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of used entries: %d")), info.iUsedEntries);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of total entries: %d")), info.iTotalEntries);

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phbkInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phbkInfoPckg(phbkInfo);


	iPhone.GetPhoneStoreInfo(iStatus, phbkInfoPckg, KETelIccAdnPhoneBook);
	User::WaitForRequest(iStatus);
	if(iStatus.Int())
		{
		return EFail;
		}

	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Store Name: %S")), &phbkInfo.iName);
	TESTL(phbkInfo.iType==RMobilePhoneStore::EPhoneBookStore);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of used entries: %d")), phbkInfo.iUsedEntries);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Number of total entries: %d")), phbkInfo.iTotalEntries);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Max text length: %d")), phbkInfo.iMaxTextLength);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Max number length: %d")), phbkInfo.iMaxNumLength);
	return TestStepResult();
	}
//------------------------------------------------------------------

CGetCaps::CGetCaps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetCaps"));
	}
//------------------------------------------------------------------

TVerdict CGetCaps::doTestStepL( void )
	{
	RPhone::TCaps capabilities;
	RPhone::TStatus phoneStatus;
	RPhone::TPhoneInfo phoneInfo;
	TInt ret;
    INFO_PRINTF1(_L("Get Phone Capabilities"));

// Get Caps and print them
	INFO_PRINTF1(_L("Calling RMobilePhone::GetCaps()"));
	ret = iPhone.GetCaps(capabilities);
	if(ret) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetCaps() returned: %d"), ret);
		return EFail;
		}

	if(capabilities.iFlags & RPhone::KCapsUnknown)
		{
		INFO_PRINTF1(_L("The capabilities of the phone are not currently known"));
		}
	if(capabilities.iFlags & RPhone::KCapsData)
		{
		INFO_PRINTF1(_L("The phone supports data calls"));
		}
	if(capabilities.iFlags & RPhone::KCapsFaxClassOne)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 1 interface"));
		}
	if(capabilities.iFlags &	RPhone::KCapsFaxClassOnePointZero)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 1.0 interface"));
		}
	if(capabilities.iFlags & RPhone::KCapsFaxClassTwo)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 2 interface"));
		}
	if(capabilities.iFlags & RPhone::KCapsFaxClassTwoPointZero)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 2.0 interface"));
		}
	if(capabilities.iFlags & RPhone::KCapsFaxClassTwoPointOne)
		{
		INFO_PRINTF1(_L("The phone supports the fax class 2.1 interface"));
		}
	if(capabilities.iFlags & RPhone::KCapsVoice)
		{
		INFO_PRINTF1(_L("The phone supports voice calls"));
		}
	if(capabilities.iFlags & RPhone::KCapsEventModemDetection)
		{
		INFO_PRINTF1(_L("The phone supports modem detection events"));
		}
	if(capabilities.iFlags & RPhone::KCapsStealCommPort)
		{
		INFO_PRINTF1(_L("The phone supports the ability to take the comms port "));
		}
// Gets Status and prints info
	
	INFO_PRINTF1(_L("Calling RMobilePhone::GetStatus()"));	
	ret = iPhone.GetStatus(phoneStatus);
	if(ret) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetStatus() returned %d"), ret);
		return EFail;
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

	// Gives time to read the results
	INFO_PRINTF1(_L("Calling RMobilePhone::GetInfo()"));
	ret = iPhone.GetInfo(phoneInfo);
	if(ret) {
		INFO_PRINTF2(_L("RMobilePhone::GetInfo() returned %d"), ret);
		return EFail;
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
//------------------------------------------------------------------

CGetLinesStatus::CGetLinesStatus()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetLinesStatus"));
	}
//------------------------------------------------------------------

TVerdict CGetLinesStatus::doTestStepL( void )
	{
	TInt aCount;
	TInt i;					// Loop counter
	RPhone::TLineInfo	lineInfo;

    INFO_PRINTF1(_L("Testing the line(s) status"));

	TInt ret = iPhone.EnumerateLines(aCount);
	INFO_PRINTF2(_L("There are %d lines on this phone"), aCount);
	if(ret)
		{
		return EFail;
		}

	for(i=0;i<aCount;i++)
		{
		iPhone.GetLineInfo(i+1, lineInfo);
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
//------------------------------------------------------------------

CGetMultimodeCaps::CGetMultimodeCaps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetMultimodeCaps"));
	}
//------------------------------------------------------------------

TVerdict CGetMultimodeCaps::doTestStepL( void )
{
	TUint32 capabilities;

    INFO_PRINTF1(_L("Testing the Multimode caps"));
	
	INFO_PRINTF1(_L("Calling RMobilePhone::GetMultimodeCaps()"));
	TInt ret = iPhone.GetMultimodeCaps(capabilities);
	if(ret) 
	{
		INFO_PRINTF2(_L("RMobilePhone::GetMultimodeCaps returned %d"), ret);
		return EFail;
	}

	if(capabilities & RMobilePhone::KCapsGsmSupported)	
		{
		INFO_PRINTF1(_L("Phone can operate in GSM 900/1800/1900 mode"));
		}
	if(capabilities & RMobilePhone::KCapsGprsSupported)
		{
		INFO_PRINTF1(_L("Phone can operate in GPRS mode"));
		}
	if(capabilities & RMobilePhone::KCapsAmpsSupported)
		{
		INFO_PRINTF1(_L("Phone can operate in AMPS 800 mode"));
		}
	if(capabilities & RMobilePhone::KCapsWcdmaSupported)
		{
		INFO_PRINTF1(_L("Phone can operate in W-CDMA mode"));
		}

	return TestStepResult();
	}
//------------------------------------------------------------------

CGetNetworkCaps::CGetNetworkCaps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetNetworkCaps"));
	}
//------------------------------------------------------------------

TVerdict CGetNetworkCaps::doTestStepL( void )
{
	TUint32 aCaps;

    INFO_PRINTF1(_L("Getting the phone network caps"));

	INFO_PRINTF1(_L("Calling RMobilePhone::GetNetworkCaps()"));
	TInt ret = iPhone.GetNetworkCaps(aCaps);
	if(ret) 
	{
		INFO_PRINTF2(_L("RMobilePhone::GetNetworkCaps() returned %d"), ret);
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

	return TestStepResult();
	}
//------------------------------------------------------------------

CGetCurrentMode::CGetCurrentMode()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetCurrentMode"));
	}
//------------------------------------------------------------------

TVerdict CGetCurrentMode::doTestStepL( void )
	{
	RMobilePhone::TMobilePhoneNetworkMode netMode;
    INFO_PRINTF1(_L("Testing the current mode and notification"));

	INFO_PRINTF1(_L("Calling RMobilePhone::GetCurrentMode()"));
	TInt ret = iPhone.GetCurrentMode(netMode);
	if(ret)
		{
		INFO_PRINTF2(_L("RMobilePhone::GetCurrentMode() returned %d"), ret);
		return EFail;
		}

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
			INFO_PRINTF1(_L("CDMA network."));
			break;

		case RMobilePhone::ENetworkModeCdma2000:
			INFO_PRINTF1(_L("CDMA network."));
			break;

		case RMobilePhone::ENetworkModeWcdma:
			INFO_PRINTF1(_L("WCDMA network"));
			break;
			
		case RMobilePhone::ENetworkModeTdcdma:
			INFO_PRINTF1(_L("TD-CDMA network"));
			break;
			
		case RMobilePhone::ENetworkModeUnknown:
		default:
			INFO_PRINTF1(_L("Network mode is unknown"));
			break;
		}
	
	return TestStepResult();
	}


CGetPhoneId::CGetPhoneId()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetPhoneIdAndCaps"));
	}

TVerdict CGetPhoneId::doTestStepL( void )
	{
	TUint32 aCaps;

    INFO_PRINTF1(_L("Getting the phone identity caps"));

	INFO_PRINTF1(_L("Calling RMobilePhone::GetIdentityCaps()"));
	TInt ret = iPhone.GetIdentityCaps(aCaps);
	if(ret) 
		{
		INFO_PRINTF2(_L("RMobilePhone::GetIdentityCaps() returned %d"), ret);
		return EFail;
		}

	if(aCaps & RMobilePhone::  KCapsGetManufacturer)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of manufacturer identity."));
		}
		
	if(aCaps & RMobilePhone:: KCapsGetModel)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of model identity."));
		}
		
	if(aCaps & RMobilePhone:: KCapsGetRevision)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of revision identity."));
		}
		
	if(aCaps & RMobilePhone:: KCapsGetSerialNumber)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of serial number identity."));
		}
		
	if(aCaps & RMobilePhone:: KCapsGetSubscriberId)
		{
		INFO_PRINTF1(_L("The phone supports retrieval of subscriber identity."));
		} 
		
	//Test GePhoneId()
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(iStatus, phoneId);
	User::WaitForRequest(iStatus);
	if(iStatus.Int())
		{
		return EFail;
		}
	
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Phone manufacturer: %S")), &(phoneId.iManufacturer));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Phone model: %S")), &(phoneId. iModel));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Phone revision: %S")), &(phoneId. iRevision));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Phone serial number: %S")), &(phoneId. iSerialNumber));	
	
	//Test cancel
	iPhone.GetPhoneId(iStatus, phoneId);
	iPhone.CancelAsyncRequest(EMobilePhoneGetPhoneId);
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("GetPhoneId Cancel status = %d")), iStatus.Int());
		  
	return TestStepResult();	
	}
	
CPubSubMode::CPubSubMode()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("PubSubMode"));
	}

TVerdict CPubSubMode::doTestStepL( void )
	{
    INFO_PRINTF1(_L("Test changing test number P&S methods"));

	DisconnectFromEtel();
	
	TInt testnumber = 5;
	TInt ret;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	TBuf<25> manufacturerid;
	TBuf<25> modelid;
	TBuf<25> serialid;
	TBuf<25> revision;
		
	//Test 1: Set the test number using P&S	
	INFO_PRINTF1(_L("Set the test number using P&S."));
	testnumber = 6;
	DisconnectFromEtel();
	
	ret = RProperty::Set(KUidPSSimTsyCategory,	KPSSimTsyTestNumber, testnumber);
	TEST(ret == KErrNone);
	
	TInt value;
	ret = RProperty::Get(KUidPSSimTsyCategory,	KPSSimTsyTestNumber, value);
	TEST(value == testnumber);
	
	if ( (ret = ConnectToEtelL()) !=KErrNone)
		{
		ERR_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		SetTestStepResult(EFail);
		}
	
	iPhone.GetPhoneId(iStatus, phoneId);
	User::WaitForRequest(iStatus);
	if(iStatus.Int())
		{
		SetTestStepResult(EFail);
		}
	
	manufacturerid=_L("yPSIONy");
	modelid=_L("y666y");
	serialid=_L("y1234567890y");
	revision=_L("y1.1y");
	
	TEST(phoneId.iManufacturer == manufacturerid);
	TEST(phoneId.iModel == modelid);
	TEST(phoneId.iRevision == revision);
	TEST(phoneId.iSerialNumber == serialid);
	
	//Test 2: Test phone power uid with P&S
	INFO_PRINTF1(_L("Test phone power uid with and P&S."));
	testnumber = 7;
	DisconnectFromEtel();	
	
	ret = RProperty::Set(KUidPSSimTsyCategory,	KPSSimTsyTestNumber, testnumber);
	TEST(ret == KErrNone);
	
	if ( (ret = ConnectToEtelL()) !=KErrNone)
		{
		ERR_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		SetTestStepResult(EFail);
		}

	testnumber = 0;
	DisconnectFromEtel();
	
	ret = RProperty::Set(KUidPSSimTsyCategory,	KPSSimTsyTestNumber, testnumber);
	TEST(ret == KErrNone);
	
	if ( (ret = ConnectToEtelL()) !=KErrNone)
		{
		ERR_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		SetTestStepResult(EFail);
		}
	
	//Test 3: Initiate voice call using P&S
	INFO_PRINTF1(_L("Initiate voice call using P&S."));		

	TestCallInitiationL(KVoiceLineName, KUidPSSimTsyCategory, KPSSimTsyIncomingVoiceCall);
	
	//Test 4: Initiate data call using P&S
	INFO_PRINTF1(_L("Initiate data call using P&S."));		

	TestCallInitiationL(KDataLineName, KUidPSSimTsyCategory, KPSSimTsyIncomingDataCall);

	//Test 5: Test reg status using old P&S uid (KUidSimtsyCategory/KSimtsyRegStatChange) and new P&S uid (KUidPSSimTsyCategory/KPSSimTsyRegStatChange)
	INFO_PRINTF1(_L("Test reg status."));	
	
	//KUidSimtsyCategory/KSimtsyRegStatChange (Regression)
	
	TInt dummy;	
	ret = RProperty::Get(KUidSimtsyCategoryLocal, KSimtsyRegStatChange, dummy);
	if(ret == KErrNotFound)
		{
		RProperty::Define(KUidSimtsyCategoryLocal, KSimtsyRegStatChange, KSimtsyRegStatChangeType);
		}

	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
	iPhone.NotifyNetworkRegistrationStatusChange(iStatus,regStatus);
	
	//change it using KUidSimtsyCategory/KSimtsySignalStrengthChange - this only changes the bar level
	RMobilePhone::TMobilePhoneRegistrationStatus expectedRegStatus = RMobilePhone::ENotRegisteredEmergencyOnly;
	ret = RProperty::Set(KUidSimtsyCategoryLocal, KSimtsyRegStatChange, expectedRegStatus);
	TEST(ret == KErrNone);	
	
	User::WaitForRequest(iStatus);
	TEST(iStatus==KErrNone);
	TEST(regStatus == expectedRegStatus);
	
	//KUidPSSimTsyCategory/KPSSimTsyRegStatChange
	iPhone.NotifyNetworkRegistrationStatusChange(iStatus,regStatus);
	
	//change it using KUidSimtsyCategory/KSimtsySignalStrengthChange - this only changes the bar level
	expectedRegStatus = RMobilePhone::ERegisteredRoaming;
	ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyRegStatChange, expectedRegStatus);
	TEST(ret == KErrNone);	
	
	User::WaitForRequest(iStatus);
	TEST(iStatus==KErrNone);
	TEST(regStatus == expectedRegStatus);	

	//Test 6:  Test signal strength status using old P&S uid (KUidSimtsyCategory/KSimtsySignalStrengthChange) and new P&S uid (KUidPSSimTsyCategory/KPSSimTsySignalStrengthChange)
	INFO_PRINTF1(_L("Test signal strength status."));
	//KUidSimtsyCategory/KSimtsySignalStrengthChange    (Regression)

	ret = RProperty::Get(KUidSimtsyCategoryLocal, KSimtsySignalStrengthChange, dummy);
	if(ret == KErrNotFound)
		{
		RProperty::Define(KUidSimtsyCategoryLocal, KSimtsySignalStrengthChange, KSimtsySignalStrengthChangeType);
		}

	TInt32 signalStrength;
	TInt8 barLevel;
	iPhone.NotifySignalStrengthChange(iStatus,signalStrength,barLevel);
	
	//change it using KUidSimtsyCategory/KSimtsySignalStrengthChange - this only changes the bar level
	TInt8 expectedBarLevel = 11;
	ret = RProperty::Set(KUidSimtsyCategoryLocal, KSimtsySignalStrengthChange, expectedBarLevel);
	TEST(ret == KErrNone);	
	
	User::WaitForRequest(iStatus);
	TEST(iStatus.Int()==KErrNone);
	TEST(barLevel == expectedBarLevel);
	
	//KUidPSSimTsyCategory/KPSSimTsySignalStrengthChange
	
	//do not have to define this property as defined by simtsy
	iPhone.NotifySignalStrengthChange(iStatus,signalStrength,barLevel);
	
	//change it using KUidSimtsyCategory/KSimtsySignalStrengthChange - this only changes the bar level
	expectedBarLevel = 4;
	ret = RProperty::Set(KUidSimtsyCategoryLocal, KSimtsySignalStrengthChange, expectedBarLevel);
	TEST(ret == KErrNone);	
	
	User::WaitForRequest(iStatus);
	TEST(iStatus.Int()==KErrNone);
	TEST(barLevel == expectedBarLevel);	
	

	return TestStepResult();
	}
	
void CPubSubMode::DisconnectFromEtel()
	{	
	//disconnect phone session
	iPhone.Close();
	}

TInt CPubSubMode::ConnectToEtelL()
	{
	User::LeaveIfError(iPhone.Open(iTelServer,KPhoneName)) ;
	// Initialize the Phone
	return iPhone.Initialise();
	}
	
void CPubSubMode::TestCallInitiationL(const TDesC& aLineName, TUid aPSCallInitCategory, const TInt aPSCallInitKey)
	{		
	RLine line;
	CleanupClosePushL(line); 
	
	TName callName;
	
	INFO_PRINTF2(_L("Opening %S Line"), &aLineName);

	TInt ret = line.Open(iPhone,aLineName);
	TEST(ret==KErrNone);

	TInt callID = 0;
	RCall call;

	//Test  Initiate call using P&S
	
	line.NotifyIncomingCall(iStatus, callName);	// wait for a call
	
	ret = RProperty::Set(aPSCallInitCategory, aPSCallInitKey, callID);
	TEST(ret == KErrNone);
	callID++;
	
	User::WaitForRequest(iStatus);
	TEST(iStatus == KErrNone);	
	
	//answer the call then hangup.
	ret = call.OpenExistingCall(line, callName);
	TEST(ret == KErrNone);
	call.AnswerIncomingCall(iStatus);
	User::WaitForRequest(iStatus);	
	call.HangUp();
	call.Close();
	
	CleanupStack::PopAndDestroy(); //call	
	}
