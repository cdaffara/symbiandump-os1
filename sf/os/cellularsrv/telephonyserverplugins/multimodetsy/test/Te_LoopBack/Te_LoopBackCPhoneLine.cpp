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
//

#include <e32test.h>
#include <etelmm.h>
#include <mmretrieve.h>
#include "Te_LoopBackCPhoneLine.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

/**
@file
*/

// publics
const TInt Pause=5000000;		// 5 second pause

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDrivePhoneLine* CTestDrivePhoneLine::NewL(const TScriptList aScriptListEntry)
	{
	CTestDrivePhoneLine* phoneLine = new (ELeave) CTestDrivePhoneLine(aScriptListEntry);
	CleanupStack::PushL(phoneLine);
	phoneLine->ConstructL();
	CleanupStack::Pop();
	return phoneLine;
	}

CTestDrivePhoneLine::CTestDrivePhoneLine(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

CTestDrivePhoneLine::~CTestDrivePhoneLine()
	{}

TInt CTestDrivePhoneLine::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDrivePhoneLine::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	// Open the iPhone object
	_LIT(KPhoneName, "GsmPhone1");
	TESTL(iVlPhone.Open(iServer, KPhoneName) == KErrNone);
	
	// Initialization Cancel Request
	INFO_PRINTF1(_L("Phone Initialization Cancel Request"));
	TRequestStatus initStat;
	INFO_PRINTF1(_L("Initialize the iPhone..."));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iVlPhone.InitialiseMM(initStat , tableSettingsPckg); 	

	INFO_PRINTF1(_L("Cancel the iPhone initialization..."));
	iVlPhone.InitialiseCancel();
	User::WaitForRequest(initStat);
	INFO_PRINTF2(_L("Cancel status = %d"), initStat.Int());
	TESTL((initStat==KErrCancel)||(initStat==KErrNone));

	// Initialise the iPhone
	INFO_PRINTF1(_L("Initialize the iPhone..."));

	iVlPhone.InitialiseMM(initStat , tableSettingsPckg); 	
	User::WaitForRequest(initStat);
	TESTL(initStat == KErrNone);	

	ERR_PRINTF1(_L("Phone Information"));
	testPhoneL();
	User::After(Pause);		// Give the user time to see the test results

	ERR_PRINTF1(_L("Line Information"));
	testLineL();

	// Close the iPhone
	INFO_PRINTF1(_L("Close the iPhone..."));
	iVlPhone.Close();		
	return (KErrNone);
	}


void CTestDrivePhoneLine::testPhoneL()

/**
 * This function retrieves the:
 *		Phone's capabilities,
 *		A snapshot of the iPhone's GSM information,
 *		the Number of lines associated with the iPhone and
 *		performs some Cancel Requests.
 */
	{
	// get all of the iPhone's capabilities
	INFO_PRINTF1(_L("Get the Phone's Capabilities...\n"));

	// API Version
	TInt mmApiVersion;
 	TESTL(iVlPhone.GetMultimodeAPIVersion(mmApiVersion)== KErrNone);
 	INFO_PRINTF2(_L("Multimode API version = %d\n"), mmApiVersion);

	// MultiMode Caps
    TUint32 mmCaps =0;
	TESTL(iVlPhone.GetMultimodeCaps(mmCaps)==KErrNone);
	INFO_PRINTF2(_L("Phone Multimode Capabilities = 0x%x"), mmCaps);
	INFO_PRINTF2(_L("Phone Multimode Capabilities = %d"), mmCaps);
	if (mmCaps & RMobilePhone::KCapsGsmSupported)
		INFO_PRINTF1(_L("  Phone Caps Support GSM\n"));
	if (mmCaps & RMobilePhone::KCapsGprsSupported)
		INFO_PRINTF1(_L("  Phone Caps Support GPRS\n"));
	if (mmCaps & RMobilePhone::KCapsAmpsSupported)
		INFO_PRINTF1(_L("  Phone Caps Support AMPS\n"));
	if (mmCaps & RMobilePhone::KCapsWcdmaSupported)
		INFO_PRINTF1(_L("  Phone Caps Support W-CDMA\n"));

	// Get Current Network
	RMobilePhone::TMobilePhoneNetworkMode mmNetworkMode;
	iVlPhone.GetCurrentMode(mmNetworkMode);
	INFO_PRINTF2(_L("Phone Network Mode = 0x%x\n"), mmNetworkMode);
	if (mmNetworkMode & RMobilePhone::ENetworkModeUnknown)
		INFO_PRINTF1(_L("  Phone Network Mode = Unknown\n"));
	if (mmNetworkMode & RMobilePhone::ENetworkModeUnregistered)
		INFO_PRINTF1(_L("  Phone Network Mode = Unregistered\n"));
	if (mmNetworkMode & RMobilePhone::ENetworkModeGsm)
		INFO_PRINTF1(_L("  Phone Network Mode = GSM\n"));
	if (mmNetworkMode & RMobilePhone::ENetworkModeAmps)
		INFO_PRINTF1(_L("  Phone Network Mode = AMPS\n"));
	if (mmNetworkMode & RMobilePhone::ENetworkModeWcdma)
		INFO_PRINTF1(_L("  Phone Network Mode   W-CDMA\n"));

	// ICC (Integrated Circuit Card) Caps
    TUint32 mmIccAccessCaps = 0;
	TESTL(iVlPhone.GetIccAccessCaps(mmIccAccessCaps));
	INFO_PRINTF2(_L("Phone ICC Access Caps = 0x%x\n"), mmIccAccessCaps);
	if (mmIccAccessCaps & RMobilePhone::KCapsSimAccessSupported)
		INFO_PRINTF1(_L("  SIM Access Supported\n"));
	if (mmIccAccessCaps & RMobilePhone::KCapsRUimAccessSupported)
		INFO_PRINTF1(_L("  RUIM Access Supported\n"));
	if (mmIccAccessCaps & RMobilePhone::KCapsUSimAccessSupported)
		INFO_PRINTF1(_L("  USIM Access Supported\n"));

	// Battery Caps
    TUint32 mmBatteryCaps =0;
	TESTL(iVlPhone.GetBatteryCaps(mmBatteryCaps));
	INFO_PRINTF2(_L("Phone Battery Caps = 0x%x\n"), mmBatteryCaps);
	if (mmBatteryCaps & RMobilePhone::KCapsGetBatteryInfo)
		INFO_PRINTF1(_L("  Get Battery Info Supported\n"));
	if (mmBatteryCaps & RMobilePhone::KCapsNotifyBatteryInfoChange)
		INFO_PRINTF1(_L("  Notify Battery Info Supported\n"));

	// Signal Caps
    TUint32 mmSignalCaps =0;
	TESTL(iVlPhone.GetSignalCaps(mmSignalCaps)==KErrNone);
	INFO_PRINTF2(_L("Phone Signal Strength Caps = 0x%x\n"), mmSignalCaps);
	if (mmSignalCaps & RMobilePhone::KCapsGetSignalStrength)
		INFO_PRINTF1(_L("  Get Signal Strength Supported\n"));
	if (mmSignalCaps & RMobilePhone::KCapsNotifySignalStrengthChange)
		INFO_PRINTF1(_L("  Notify Signal Strength Supported\n"));

	// Identity Caps
    TUint32 mmIdentityCaps =0;
	TESTL(iVlPhone.GetIdentityCaps(mmIdentityCaps)==KErrNone);
	INFO_PRINTF2(_L("Phone Identity Caps = 0x%x\n"), mmIdentityCaps);
	if (mmIdentityCaps & RMobilePhone::KCapsGetManufacturer)
		INFO_PRINTF1(_L("  Get Manufacturer Supported\n"));
	if (mmIdentityCaps & RMobilePhone::KCapsGetModel)
		INFO_PRINTF1(_L("  Get Model Supported\n"));
	if (mmIdentityCaps & RMobilePhone::KCapsGetRevision)
		INFO_PRINTF1(_L("  Get Revision Supported\n"));
	if (mmIdentityCaps & RMobilePhone::KCapsGetSerialNumber)
		INFO_PRINTF1(_L("  Get Serial Number Supported\n"));
	if (mmIdentityCaps & RMobilePhone::KCapsGetSubscriberId)
		INFO_PRINTF1(_L("  Get Subscriber Id Supported\n"));

	// DTMF Caps
    TUint32 mmDtmfCaps = 0;
	TESTL(iVlPhone.GetDTMFCaps(mmDtmfCaps));
	INFO_PRINTF2(_L("Phone DTMF Caps = 0x%x\n"), mmDtmfCaps);
	if (mmDtmfCaps & RMobilePhone::KCapsSendDTMFString)
		INFO_PRINTF1(_L("  Send DTMF String Supported\n"));
	if (mmDtmfCaps & RMobilePhone::KCapsSendDTMFSingleTone)
		INFO_PRINTF1(_L("  Send DTMF Single Tone Supported\n"));

	// Network Caps
    TUint32 mmNetworkCaps = 0;
 	TESTL(iVlPhone.GetNetworkCaps(mmNetworkCaps)==KErrNone);
 	INFO_PRINTF2(_L("Phone Network Caps = 0x%x\n"), mmNetworkCaps);
	if (mmNetworkCaps & RMobilePhone::KCapsGetRegistrationStatus)
		INFO_PRINTF1(_L("  Get Registration Status Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsNotifyRegistrationStatus)
		INFO_PRINTF1(_L("  Notify Registration Status Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsGetCurrentMode)
		INFO_PRINTF1(_L("  Get Current Mode Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsNotifyMode)
		INFO_PRINTF1(_L("  Notify Mode Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsGetCurrentNetwork)
		INFO_PRINTF1(_L("  Get Current Network Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsNotifyCurrentNetwork)
		INFO_PRINTF1(_L("  Notify Current Network Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsGetHomeNetwork)
		INFO_PRINTF1(_L("  Get Home Network Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsGetDetectedNetworks)
		INFO_PRINTF1(_L("  Get Detected Networks Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsManualNetworkSelection)
		INFO_PRINTF1(_L("  Manual Network Selection Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsGetNITZInfo)
		INFO_PRINTF1(_L("  Get NITZ Info Supported\n"));
	if (mmNetworkCaps & RMobilePhone::KCapsNotifyNITZInfo)
		INFO_PRINTF1(_L("  Notify NITZ Info Supported\n"));

	// Supplementary Call Service Caps
    TUint32 mmCallServiceCaps = 0;
	TESTL(iVlPhone.GetCallServiceCaps(mmCallServiceCaps));
	INFO_PRINTF2(_L("Supplementary Call Service Caps = 0x%x\n"), mmCallServiceCaps);
	if (mmCallServiceCaps & RMobilePhone::KCapsGetCFStatusCache)
		INFO_PRINTF1(_L("  Get CF Status Cache Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetCFStatusNetwork)
		INFO_PRINTF1(_L("  Get CF Status Network Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsSetCFStatus)
		INFO_PRINTF1(_L("  Set CF Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsNotifyCFStatus)
		INFO_PRINTF1(_L("  Notify CF Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetClipStatus)
		INFO_PRINTF1(_L("  Get Clip Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetClirStatus)
		INFO_PRINTF1(_L("  Get Clir Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetColpStatus)
		INFO_PRINTF1(_L("  Get Colp Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetColrStatus)
		INFO_PRINTF1(_L("  Get Colr Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetCnapStatus)
		INFO_PRINTF1(_L("  Get Cnap Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetCBStatusCache)
		INFO_PRINTF1(_L("  Get CB Status Cache Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetCBStatusNetwork)
		INFO_PRINTF1(_L("  Get CB Status Network Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsSetCBStatus)
		INFO_PRINTF1(_L("  Set CB Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsNotifyCBStatus)
		INFO_PRINTF1(_L("  Notify CB Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsChangeCBPassword)
		INFO_PRINTF1(_L("  Change CB Password Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsBarAllIncoming)
		INFO_PRINTF1(_L("  Bar All Incoming Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsBarIncomingRoaming)
		INFO_PRINTF1(_L("  Bar Incoming Roaming Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsBarAllOutgoing)
		INFO_PRINTF1(_L("  Bar All Outgoing Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsBarOutgoingInternational)
		INFO_PRINTF1(_L("  Bar Outgoing International Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsBarOutgoingInternationalExHC)
		INFO_PRINTF1(_L("  Bar Outgoing International ExHC Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsBarAllCases)
		INFO_PRINTF1(_L("  Bar All Cases Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetCWStatusCache)
		INFO_PRINTF1(_L("  Get CW Status Cache Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetCWStatusNetwork)
		INFO_PRINTF1(_L("  Get CW Status Network Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsSetCWStatus)
		INFO_PRINTF1(_L("  Set CW Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsNotifyCWStatus)
		INFO_PRINTF1(_L("  Notify CW Status Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetCCBSStatusCache)
		INFO_PRINTF1(_L("  Get CCBS Status Cache Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsGetCCBSStatusNetwork)
		INFO_PRINTF1(_L("  Get CCBS Status Network Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsDeactivateAllCCBS)
		INFO_PRINTF1(_L("  Deactivate All CCBS Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsRetrieveActiveCCBS)
		INFO_PRINTF1(_L("  Retrieve Active CCBS Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsFeatureCode)
		INFO_PRINTF1(_L("  Feature Code Supported\n"));
	if (mmCallServiceCaps & RMobilePhone::KCapsNetworkServiceRequest)
		INFO_PRINTF1(_L("  Network Service Request Supported\n"));

	// Alternating Calls Caps
    TUint32 mmAlternatingCallCaps = 0;
	TESTL(iVlPhone.GetAlternatingCallCaps(mmAlternatingCallCaps));
	INFO_PRINTF2(_L("Phone Alternating Call Caps = 0x%x\n"), mmAlternatingCallCaps);
	if (mmAlternatingCallCaps & RMobilePhone::KCapsMOVoiceData)
		INFO_PRINTF1(_L("  MO Voice Call Supported\n"));
	if (mmAlternatingCallCaps & RMobilePhone::KCapsMOVoiceThenData)
		INFO_PRINTF1(_L("  MO Voice Then Data Supported\n"));
	if (mmAlternatingCallCaps & RMobilePhone::KCapsMOVoiceFax)
		INFO_PRINTF1(_L("  MO Voice Fax Supported\n"));
	if (mmAlternatingCallCaps & RMobilePhone::KCapsMTVoiceData)
		INFO_PRINTF1(_L("  MT Voice Data Supported\n"));
	if (mmAlternatingCallCaps & RMobilePhone::KCapsMTVoiceThenData)
		INFO_PRINTF1(_L("  MT Voice Then Data Supported\n"));
	if (mmAlternatingCallCaps & RMobilePhone::KCapsMTVoiceFax)
		INFO_PRINTF1(_L("  MT Voice Fax Supported\n"));

	// Cost Caps
    TUint32 mmCostCaps = 0;
	TESTL(iVlPhone.GetCostCaps(mmCostCaps));
	INFO_PRINTF2(_L("Phone Cost Caps = 0x%x\n"), mmCostCaps);
	if (mmCostCaps & RMobilePhone::KCapsCostInformation)
		INFO_PRINTF1(_L("  Cost Information Supported\n"));
	if (mmCostCaps & RMobilePhone::KCapsCostCharging)
		INFO_PRINTF1(_L("  Cost Charging Supported\n"));
	if (mmCostCaps & RMobilePhone::KCapsClearCost)
		INFO_PRINTF1(_L("  Clear Cost Supported\n"));
	if (mmCostCaps & RMobilePhone::KCapsSetMaxCost)
		INFO_PRINTF1(_L("  Set Max Cost Supported\n"));
	if (mmCostCaps & RMobilePhone::KCapsSetPuct)
		INFO_PRINTF1(_L("  Set Puct Supported\n"));
	if (mmCostCaps & RMobilePhone::KCapsGetCost)
		INFO_PRINTF1(_L("  Get Cost Supported\n"));
	if (mmCostCaps & RMobilePhone::KCapsNotifyCostChange)
		INFO_PRINTF1(_L("  Notify Cost Change Supported\n"));

	// Security Caps
    TUint32 mmSecurityCaps =0;
	TESTL(iVlPhone.GetSecurityCaps(mmSecurityCaps));
	INFO_PRINTF2(_L("Phone Security Caps = 0x%x\n"), mmSecurityCaps);
	if (mmSecurityCaps & RMobilePhone::KCapsLockPhone)
		INFO_PRINTF1(_L("  Lock Phone Supported\n"));
	if (mmSecurityCaps & RMobilePhone::KCapsLockICC)
		INFO_PRINTF1(_L("  Lock ICC Supported\n"));
	if (mmSecurityCaps & RMobilePhone::KCapsLockPhoneToICC)
		INFO_PRINTF1(_L("  Lock Phone To ICC Supported\n"));
	if (mmSecurityCaps & RMobilePhone::KCapsLockPhoneToFirstICC)
		INFO_PRINTF1(_L("  Lock Phone To First ICC Supported\n"));
	if (mmSecurityCaps & RMobilePhone::KCapsLockOTA)
		INFO_PRINTF1(_L("  Lock OTA Supported\n"));
	if (mmSecurityCaps & RMobilePhone::KCapsAccessPin1)
		INFO_PRINTF1(_L("  Access Pin1 Supported\n"));
	if (mmSecurityCaps & RMobilePhone::KCapsAccessPin2)
		INFO_PRINTF1(_L("  Access Pin2 Supported\n"));
	if (mmSecurityCaps & RMobilePhone::KCapsAccessPhonePassword)
		INFO_PRINTF1(_L("  Access Phone Password Supported\n"));
	if (mmSecurityCaps & RMobilePhone::KCapsAccessSPC)
		INFO_PRINTF1(_L("  Access SPC Supported\n"));


	// base class iPhone caps
	RPhone::TCaps phoneCaps;
	TESTL(iVlPhone.GetCaps(phoneCaps) == KErrNone);
	if (!(phoneCaps.iFlags & RPhone::KCapsVoice))
		{
		INFO_PRINTF2(_L("This iPhone does not support voice calls (caps=0x%x)\n"), phoneCaps.iFlags);
		iVlPhone.Close();
		return;
		}
	INFO_PRINTF2(_L("Base Class RPhone Caps = 0x%x\n"), phoneCaps.iFlags);
	if (phoneCaps.iFlags & RPhone::KCapsUnknown)
		INFO_PRINTF1(_L("  Caps Unknown\n"));
	if (phoneCaps.iFlags & RPhone::KCapsData)
		INFO_PRINTF1(_L("  Data Supported\n"));
	if (phoneCaps.iFlags & RPhone::KCapsFaxClassOne)
		INFO_PRINTF1(_L("  Fax Class 1 Supported\n"));
	if (phoneCaps.iFlags & RPhone::KCapsFaxClassOnePointZero)
		INFO_PRINTF1(_L("  Fax Class 2 Point Zero Supported\n"));
	if (phoneCaps.iFlags & RPhone::KCapsFaxClassTwo)
		INFO_PRINTF1(_L("  Fax Class 2 Supported\n"));
	if (phoneCaps.iFlags & RPhone::KCapsFaxClassTwoPointZero)
		INFO_PRINTF1(_L("  Fax Class 2 Point 0 Supported\n"));
	if (phoneCaps.iFlags & RPhone::KCapsFaxClassTwoPointOne)
		INFO_PRINTF1(_L("  Fax Class 2 Point 1 Supported\n"));
	if (phoneCaps.iFlags & RPhone::KCapsVoice)
		INFO_PRINTF1(_L("  Voice Supported\n"));
	if (phoneCaps.iFlags & RPhone::KCapsEventModemDetection)
		INFO_PRINTF1(_L("  Event Modem Detection Supported\n"));
	if (phoneCaps.iFlags & RPhone::KCapsStealCommPort)
		INFO_PRINTF1(_L("  Steal Comm Port Supported\n"));

	
	// Get the iPhone info
	// Unlike GSM, the MM API does not include this method. it uses several other methods
	INFO_PRINTF1(_L("Get Phone Info...\n"));
	RPhone::TPhoneInfo phoneInfo;
	TInt ret = iVlPhone.GetInfo(phoneInfo);
	if (ret == KErrNotSupported)
	{
		INFO_PRINTF1(_L("Get Phone info is NOT Supported\n\n"));
	}
	else if (ret == KErrNone)
		{
		INFO_PRINTF1(_L("Get Phone Info is successful.\n"));
		TESTL(phoneInfo.iDetection == RPhone::EDetectedPresent);				
		INFO_PRINTF1(_L("Local Modem has been detected.\n\n"));
		}
	else
	{
		INFO_PRINTF1(_L("Error getting Phone Info.\n\n"));
	}


	// Number of lines associated with the iPhone
	INFO_PRINTF1(_L("Getting the number of lines associated with the iPhone\n"));
	TInt lineCount = 0;
	TESTL(iVlPhone.EnumerateLines(lineCount) == KErrNone);
	TESTL(lineCount > 0);
	INFO_PRINTF2(_L("Number of associated lines: %d\n"), lineCount);

	// find the iPhone's voice line and get the line info for it
	RPhone::TLineInfo lineInfo;
	for (;lineCount>0; lineCount--)
		{
		TESTL(iVlPhone.GetLineInfo(lineCount-1, lineInfo) == KErrNone);
		INFO_PRINTF4(_L("Line %d (%S) cap flags = 0x%x\n"),
			lineCount-1, &lineInfo.iName, lineInfo.iLineCapsFlags);
		if (lineInfo.iLineCapsFlags & RLine::KCapsVoice)
			{
			INFO_PRINTF3(_L("Line %d (%S) has voice capability\n\n"), lineCount-1, &lineInfo.iName);
			break;
			}
		}
	TESTL(lineCount > 0);

	// get the iPhone status
	INFO_PRINTF1(_L("Get the Phone's Status..."));
	RPhone::TStatus phoneStatus;
	ret = iVlPhone.GetStatus(phoneStatus);
	if (ret == KErrNotSupported)
		INFO_PRINTF1(_L("Get Phone Status is NOT Supported"));
	if (ret == KErrNone)
		{
		INFO_PRINTF1(_L("Get Phone Status is successful."));
		TESTL(phoneStatus.iMode == RPhone::EModeIdle);
		INFO_PRINTF1(_L("The iPhone is currently Idle"));
		}

	// Notification Cancel Requests : Modem Detected
	INFO_PRINTF1(_L("Notify Modem Detected Cancel Request..."));
	RPhone::TModemDetection detected;
	TRequestStatus cancelStat;
	iVlPhone.NotifyModemDetected(cancelStat, detected);
	iVlPhone.NotifyModemDetectedCancel();
	User::WaitForRequest(cancelStat);
	INFO_PRINTF2(_L("Notify Modem Detected Cancel status: %d"), cancelStat.Int());

	// Notification Cancel Requests : Caps Change
	INFO_PRINTF1(_L("Notify Caps Change Cancel Request..."));
	iVlPhone.NotifyCapsChange(cancelStat, phoneCaps);
	iVlPhone.NotifyCapsChangeCancel();
	User::WaitForRequest(cancelStat);
	INFO_PRINTF2(_L("Notify Caps Change Cancel status: %d"), cancelStat.Int());
	}


void CTestDrivePhoneLine::testLineL()

/**
 * This function gets the Line capabilities, line information, Number of calls 
 * on the line and call information, Hook status and changes in hook status. 
 * It also tests Line Notifications and Notification Cancel Requests.
 */
	{
	_LIT(KVoiceLineName, "Voice");
	
	RLine voiceLine;
	INFO_PRINTF1(_L("Opening Voice Line\n"));
	TESTL(voiceLine.Open(iVlPhone, KVoiceLineName) == KErrNone);

	// Line Capabilities
	INFO_PRINTF1(_L("Retrieving Line Capabilities\n"));
	RLine::TCaps lineCaps;
	TInt ret = voiceLine.GetCaps(lineCaps);
	if (ret != KErrNone)
	{
		INFO_PRINTF1(_L("Get Line Capabilities is NOT supported\n"));
	}
	else
		{
		if ((lineCaps.iFlags & RLine::KCapsData) != 0)
			INFO_PRINTF1(_L("This line supports Data connections\n"));
		if ((lineCaps.iFlags & RLine::KCapsFax) != 0)
			INFO_PRINTF1(_L("This line supports Fax connections\n"));
		if ((lineCaps.iFlags & RLine::KCapsVoice) != 0)
			INFO_PRINTF1(_L("This line supports Voice connections\n"));
		if ((lineCaps.iFlags & RLine::KCapsEventIncomingCall) != 0)
			INFO_PRINTF1(_L("This line supports Incoming Call Notification requests\n"));
		}

	// Line Information
	INFO_PRINTF1(_L("Retrieving Line Information\n"));
	RLine::TLineInfo lineInfo;
	ret = voiceLine.GetInfo(lineInfo);
	if (ret != KErrNone)
	{
		INFO_PRINTF1(_L("Get Line Info is NOT supported\n"));
	}
	else
	{
		INFO_PRINTF1(_L("Get Line Info is successful\n"));
	}

	// Line Status Information
	INFO_PRINTF1(_L("\nRetrieving Line Status...\n"));
	RCall::TStatus lineStatus;
	ret = voiceLine.GetStatus(lineStatus);
	if (ret == KErrNone)
		{
		TESTL(lineStatus == RCall::EStatusIdle);
		INFO_PRINTF1(_L("The line is IDLE \n\n"));
		}
	else
		INFO_PRINTF1(_L("Error retrieving the Line status\n"));
	
	// Hook status information
	INFO_PRINTF1(_L("\nRetrieving Hook Status...\n"));
	RCall::THookStatus hookStatus;
	ret = voiceLine.GetHookStatus(hookStatus);
	if (ret != KErrNone)
		INFO_PRINTF1(_L("Get Hook status is NOT supported\n"));
	else
		{
		TESTL(hookStatus == RCall::EHookStatusOn);
		INFO_PRINTF1(_L("The line is currently ON Hook\n"));
		}

	// Count the no. of calls on the line & Get call info.
	INFO_PRINTF1(_L("Retrieving no. of calls opened from the line..\n"));
	TInt numCalls = 0;
	voiceLine.EnumerateCall(numCalls);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("No of calls opened = %d\n")), numCalls);

	INFO_PRINTF1(_L("Getting Call info.."));
	RLine::TCallInfo callInfo;
	TInt firstCall = 0;
	ret = voiceLine.GetCallInfo(firstCall, callInfo);
	if (ret == KErrNotSupported)
		INFO_PRINTF1(_L("Get Call information is NOT supported"));
	if (ret == KErrNone)
		INFO_PRINTF1(_L("Get Call information is successful"));

	User::After(Pause);		// Give the user time to see the test results

	// Notification Cancel Requests
	TRequestStatus cancelStatus;

	// Hook Change Notification Cancel
	INFO_PRINTF1(_L("Hook change Notification Cancel")); 
	voiceLine.NotifyHookChange(cancelStatus, hookStatus);
	voiceLine.NotifyHookChangeCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("Hook notification Cancel status = %d"), cancelStatus.Int());

	// Call Added Notification Cancel
	INFO_PRINTF1(_L("Call Added Notification Cancel")); 
	TName tCallName;
	voiceLine.NotifyCallAdded(cancelStatus, tCallName);
	voiceLine.NotifyCallAddedCancel();
	User::WaitForRequest(cancelStatus); 
	INFO_PRINTF2(_L("Notify Call added Cancel status = %d"), cancelStatus.Int());

	// Caps Change Notification Cancel
	INFO_PRINTF1(_L("Caps change Notification Cancel\n")); 
	voiceLine.NotifyCapsChange(cancelStatus, lineCaps);
	voiceLine.NotifyCapsChangeCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("Caps Change Cancel status = %d"), cancelStatus.Int());

	// Incoming Call Notification Cancel
	INFO_PRINTF1(_L("Incoming call Notification Cancel")); 
	TName callName;
	voiceLine.NotifyIncomingCall(cancelStatus, callName);
	voiceLine.NotifyIncomingCallCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("Incoming call Notification Cancel status = %d"), cancelStatus.Int());

	// Status change Notification Cancel
	INFO_PRINTF1(_L("Status change Notification Cancel\n")); 
	voiceLine.NotifyStatusChange(cancelStatus, lineStatus);
	voiceLine.NotifyStatusChangeCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("Status change Notification Cancel = %d"), cancelStatus.Int());

	// Close the Voice line
	voiceLine.Close();
	}


//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestPhoneLine* CTestPhoneLine::NewL(const TScript* aScript)
	{
	CTestPhoneLine* phoneLine = new (ELeave) CTestPhoneLine(aScript);
	CleanupStack::PushL(phoneLine);
	phoneLine->ConstructL();
	CleanupStack::Pop();
	return (phoneLine);
	}

CTestPhoneLine::CTestPhoneLine(const TScript* aScript) :
	iScript(aScript)
	{}

void CTestPhoneLine::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

CTestPhoneLine::~CTestPhoneLine()
	{}

TInt CTestPhoneLine::Start()
	{
	StartScript(iScript);
	return (KErrNone);
	}

void CTestPhoneLine::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestPhoneLine::Complete(TInt aError)
	{
	iReturnValue = aError;
	CActiveScheduler::Stop();
	}
