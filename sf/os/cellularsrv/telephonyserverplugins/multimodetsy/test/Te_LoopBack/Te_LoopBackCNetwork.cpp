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

/**
 @file
*/

#include <e32test.h>
#include <mmretrieve.h>
#include <etelmm.h>
#include "Te_LoopBackCNetwork.h"
#include "Te_LoopbackCWrapGetDetNet.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

#define TSY_MODULE_NAME	_L("MM")    // TSY Name


//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveNetworkRegistration* CTestDriveNetworkRegistration::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveNetworkRegistration* aA = new(ELeave) CTestDriveNetworkRegistration(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveNetworkRegistration::CTestDriveNetworkRegistration(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveNetworkRegistration::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return (StartEmulatorL());
	}

TInt CTestDriveNetworkRegistration::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	const TInt pause = 5000000;		// 5 seconds, used to give user time to see the test results
	_LIT(KPhoneName, "GsmPhone1");
	
	// Open and Initialise the iPhone
	TESTL(iTestPhone.Open(iServer, KPhoneName) == KErrNone);

	TRequestStatus reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iTestPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	ERR_PRINTF1(_L("Server and TSY Info"));
	testServerInfoL();
	User::After(pause);

	ERR_PRINTF1(_L("Operator and Network Info"));
	testNetworkInfoL();
	User::After(pause);

	ERR_PRINTF1(_L("Detected Networks test"));
	testDetectedNetwork();
	User::After(pause);

	ERR_PRINTF1(_L("Synchronous Network detection test"));
	testSynchronousDetect();
	User::After(pause);

	ERR_PRINTF1(_L("Disconnected Network test"));
	TInt ret = testDisconnectedNetworkL();
	if (ret == KErrNone)
		{
		ERR_PRINTF1(_L("Notifications test"));
		testNotificationsL();
		}
	User::After(pause);

	ERR_PRINTF1(_L("Immediate Notification reposting test"));
	testMoreNotificationsL();
	User::After(pause);

	iTestPhone.Close();
	iPhone.Close();
	iPhone2.Close();

	User::After(pause);

	// open a new iPhone which only provides partial support of current network 
	// operator information
	INFO_PRINTF1(_L("\nOpening a new iPhone.\n\n"));
	TESTL(iPhone.Open(iServer, PHONE_NAME)==KErrNone);
	TESTL(iPhone2.Open(iServer, PHONE_NAME)==KErrNone);
	TESTL(iTestPhone.Open(iServer, KPhoneName) == KErrNone);

	iTestPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	ERR_PRINTF1(_L("Test Partial support of Current Network operator commands"));
	testMoreNotificationsL();
	
	iTestPhone.Close();
	return (KErrNone);
	}


void CTestDriveNetworkRegistration::testServerInfoL()
/**
 * This function:
 *		Retrieves the number of phones supported by the loaded ETel Tsy Modules;
 *		Gets information associated with the iPhone and; 
 *		Queries the Tsy about its supported functionality.
 */
	{
	TInt numOfPhones;

	TESTL(iServer.EnumeratePhones(numOfPhones) == KErrNone );
	INFO_PRINTF2(_L("No. of phones supported by loaded ETel modules = %d\n"), numOfPhones);

	// Phone information
	RTelServer::TPhoneInfo phoneInfo;
	TInt ret = iServer.GetPhoneInfo(numOfPhones - 1, phoneInfo);

	if (ret == KErrNone)
		{
		// get the Network Type
		const TText* type = _S("Unknown");
		switch (phoneInfo.iNetworkType)
			{
			case RTelServer::ENetworkTypeWiredAnalog:
				type = _S("Wired Analog Network");
				break;
			case RTelServer::ENetworkTypeWiredDigital:
				type = _S("Wired Digital Network");
				break;
			case RTelServer::ENetworkTypeMobileAnalog:
				type = _S("Mobile Analog Network");
				break;
			case RTelServer::ENetworkTypeMobileDigital:
				type = _S("Mobile Digital Network");
				break;
			default:
				type = _S("Unknown Network Type");
				break;
			}

		INFO_PRINTF2(_L("Network Type : %s"), type);
		INFO_PRINTF2(_L("Phone's name : %S"), &phoneInfo.iName);
		INFO_PRINTF2(_L("No. of lines supported by the iPhone : %d"), phoneInfo.iNumberOfLines);
		INFO_PRINTF2(_L("Extensions supported by Tsy Module : %d\n"), phoneInfo.iExtensions);
		}
	
	// get the TSY's name
	TBuf<10> tsyName;
	tsyName = TSY_MODULE_NAME;
	INFO_PRINTF2(_L("TSY Name: %S\n"), &tsyName);  // "MM" vs. "GSMBSC"

	// get the TSY's version
	TVersion tsyVersion;
	INFO_PRINTF1(_L("Querying TSY for version...\n"));
	TESTL(iServer.GetTsyVersionNumber(TSY_MODULE_NAME, tsyVersion) == KErrNone);

	INFO_PRINTF1(_L("TSY Version:"));
	INFO_PRINTF2(_L("  Major = %d"), tsyVersion.iMajor);
	INFO_PRINTF2(_L("  Minor = %d"), tsyVersion.iMinor);
	INFO_PRINTF2(_L("  Build = %d\n"), tsyVersion.iBuild);

	// get TSY's supported functionality
	TBool result;	// Returns TRUE (1), if the Extended functionality is supported
	INFO_PRINTF1(_L("Querying TSY for supported functionality..."));
	TESTL(iServer.IsSupportedByModule(TSY_MODULE_NAME, KETelFuncMobileNetwork, result) == KErrNone);

	if (result == TRUE)
		{
			INFO_PRINTF1(_L("Network functionality is supported by this TSY\n")); 
		}
	else
		{
			INFO_PRINTF1(_L("Network functionality is NOT supported by this TSY\n")); 
		}
	}


void CTestDriveNetworkRegistration::testNetworkInfoL(void)
/**
 * This function retrieves the
 *		Network Capabilities, 
 *		Current Network Information and the 
 *		Phone's Registration Status.
 */
	{
	// get the network capabilities 
	INFO_PRINTF1(_L("Get Network Caps"));

	TUint32 networkCaps = 0;
	TInt ret = iTestPhone.GetNetworkCaps(networkCaps);
	CHECKPOINT_EXPR((ret == KErrNone || KErrNotSupported), 
		_L("*** Failed RMobilePhone::'GetNetworkCaps() ***"));
	
	if (ret == KErrNotSupported)
		{
		INFO_PRINTF1(_L("Network Capabilities NOT Supported\n"));		
		}
	else if (ret == KErrNone)
		{
		INFO_PRINTF2(_L("Network Capabilities = %xH\n"), networkCaps);
		TESTL((networkCaps & RMobilePhone::KCapsGetCurrentNetwork) != 0);

		if (networkCaps & RMobilePhone::KCapsGetRegistrationStatus)
			INFO_PRINTF1(_L("  Get Registration Status Supported\n"));

		if (networkCaps & RMobilePhone::KCapsNotifyRegistrationStatus)
			INFO_PRINTF1(_L("  Notify Registration Status Supported\n"));

		if (networkCaps & RMobilePhone::KCapsGetCurrentMode)
			INFO_PRINTF1(_L("  Get Current Mode Supported\n"));

		if (networkCaps & RMobilePhone::KCapsNotifyMode)
			INFO_PRINTF1(_L("  Notify Mode Supported\n"));

		if (networkCaps & RMobilePhone::KCapsGetCurrentNetwork)
			INFO_PRINTF1(_L("  Get Current Network Supported\n"));

		if (networkCaps & RMobilePhone::KCapsNotifyCurrentNetwork)
			INFO_PRINTF1(_L("  Notify Current Network Supported\n"));

		if (networkCaps & RMobilePhone::KCapsGetHomeNetwork)
			INFO_PRINTF1(_L("  Get Home Network Supported\n"));

		if (networkCaps & RMobilePhone::KCapsGetDetectedNetworks)
			INFO_PRINTF1(_L("  Get Detected Networks Supported\n"));

		if (networkCaps & RMobilePhone::KCapsManualNetworkSelection)
			INFO_PRINTF1(_L("  Manual Network Selection Supported\n"));

		if (networkCaps & RMobilePhone::KCapsGetNITZInfo)
			INFO_PRINTF1(_L("  Get NITZ Info Supported\n"));

		if (networkCaps & RMobilePhone::KCapsNotifyNITZInfo)
			INFO_PRINTF1(_L("  Notify NITZ Info Supported\n"));
		}


	// Get the Current network
	INFO_PRINTF1(_L("Get Current Network Info"));

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV1     phoneNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg phoneNetworkInfoPckg(phoneNetworkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1    locationArea;

	iTestPhone.GetCurrentNetwork(reqStatus, phoneNetworkInfoPckg, locationArea);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);
	phoneNetworkInfo = 	phoneNetworkInfoPckg();

	INFO_PRINTF1(_L("Network Info: "));
	INFO_PRINTF2(_L("  Mode          = %d"), phoneNetworkInfo.iMode);
	INFO_PRINTF2(_L("  Status        = %d"), phoneNetworkInfo.iStatus);
	INFO_PRINTF2(_L("  Country Code  = '%S'"), &phoneNetworkInfo.iCountryCode);
	INFO_PRINTF2(_L("  Analog SID    = '%S'"), &phoneNetworkInfo.iAnalogSID);
	INFO_PRINTF2(_L("  Network Id    = '%S'"), &phoneNetworkInfo.iNetworkId);
	INFO_PRINTF2(_L("  Display Tag   = '%S'"), &phoneNetworkInfo.iDisplayTag);
	INFO_PRINTF2(_L("  Short Name    = '%S'"), &phoneNetworkInfo.iShortName);
	INFO_PRINTF2(_L("  Long Name     = '%S'\n"), &phoneNetworkInfo.iLongName);

	INFO_PRINTF1(_L("Network Location Area: "));
	INFO_PRINTF2(_L("  Area Is Known = %d"), locationArea.iAreaKnown);
	INFO_PRINTF2(_L("  Area Code     = %d"), locationArea.iLocationAreaCode);
	INFO_PRINTF2(_L("  Cell Id       = %d\n"), locationArea.iCellId);


	// Get the Registration status
	INFO_PRINTF1(_L("Get the Registration status"));
	RMobilePhone::TMobilePhoneRegistrationStatus phoneRegistationStatus;
	iTestPhone.GetNetworkRegistrationStatus(reqStatus, phoneRegistationStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	INFO_PRINTF1(_L("Registration Status = "));
	switch (phoneRegistationStatus)
		{
		case RMobilePhone::ERegistrationUnknown:
			INFO_PRINTF1(_L("Registration Unknown\n"));
			break;
		case RMobilePhone::ENotRegisteredNoService:
			INFO_PRINTF1(_L("Not Registered No Service\n"));
			break;
		case RMobilePhone::ENotRegisteredEmergencyOnly:
			INFO_PRINTF1(_L("Not Registered Emergency Only\n"));
			break;
		case RMobilePhone::ENotRegisteredSearching:
			INFO_PRINTF1(_L("Not Registered Searching\n"));
			break;
		case RMobilePhone::ERegisteredBusy:
			INFO_PRINTF1(_L("Registered Busy\n"));
			break;
		case RMobilePhone::ERegisteredOnHomeNetwork:
			INFO_PRINTF1(_L("Registered On Home Network\n"));
			break;
		case RMobilePhone::ERegistrationDenied:
			INFO_PRINTF1(_L("Registration Denied\n"));
			break;
		case RMobilePhone::ERegisteredRoaming:
			INFO_PRINTF1(_L("Registered Roaming\n"));
			break;
		default:
			INFO_PRINTF2(_L("%d Unknown Value\n"), phoneRegistationStatus);
			break;
		}
	}


void CTestDriveNetworkRegistration::testDetectedNetwork(void)
	{
	TRAP_IGNORE(DotestDetectedNetworkL());		// trap but ignore error
	}

void CTestDriveNetworkRegistration::DotestDetectedNetworkL(void)
/**
 * This function retrieves all the available networks and performs some Cancel requests.
 */
	{
	INFO_PRINTF1(_L("Enumerate Detected Networks Request...\n"));

	// create wrapper for list AO
	CWrapperGetDetectedNetworks *pWrapper = CWrapperGetDetectedNetworks::NewL(iPhone);
	CleanupStack::PushL(pWrapper);
	pWrapper->Start();
	CActiveScheduler::Start();

	// get the information on the detected networks
	INFO_PRINTF1(_L("Retrieving information on the Detected networks..\n"));

	CMobilePhoneNetworkList* networkList = pWrapper->RetrieveList();
	CleanupStack::PushL(networkList);

	TInt maxNetworksAllowed = networkList->MaxNumberEntries();
	INFO_PRINTF2(_L("CMobilePhoneNetworkList allows maximum of %d networks"), maxNetworksAllowed);
	TInt numNetworksDetected = networkList->Enumerate();
	INFO_PRINTF2(_L("CMobilePhoneNetworkList reports %d networks\n"), numNetworksDetected);

	for (TInt i=0; i<numNetworksDetected; i++)
		{
		const RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfoV1 = networkList->GetEntryL(i);

		INFO_PRINTF2(_L("Network Info for Detected Network #%d: "), i+1);
		INFO_PRINTF2(_L("  Mode          = %d"), networkInfoV1.iMode);
		INFO_PRINTF2(_L("  Status        = %d"), networkInfoV1.iStatus);
		INFO_PRINTF2(_L("  Country Code  = '%S'"), &networkInfoV1.iCountryCode);
		INFO_PRINTF2(_L("  Analog SID    = '%S'"), &networkInfoV1.iAnalogSID);
		INFO_PRINTF2(_L("  Network Id    = '%S'"), &networkInfoV1.iNetworkId);
		INFO_PRINTF2(_L("  Display Tag   = '%S'"), &networkInfoV1.iDisplayTag);
		INFO_PRINTF2(_L("  Short Name    = '%S'"), &networkInfoV1.iShortName);
		INFO_PRINTF2(_L("  Long Name     = '%S'\n"), &networkInfoV1.iLongName);
		}
		
	CleanupStack::PopAndDestroy(networkList);

	pWrapper->Cancel();
	CleanupStack::PopAndDestroy(pWrapper);
	}


void CTestDriveNetworkRegistration::testSynchronousDetect()
/**
 * 2nd Detected Network test.
 */
	{
	INFO_PRINTF1(_L("\nSynchronous Detect Networks Not Supported by MM API !! \n\n"));
	}


TInt CTestDriveNetworkRegistration::testDisconnectedNetworkL()
/**
 * This method simulates a loss in network coverage. 
 * On a live test this will be done by disconnecting the antenna of the iPhone 
 * (if this is possible).
 */
	{
	INFO_PRINTF1(_L("Simulating a loss of network coverage, this will take a short while...\n"));
	
	// wait 10 seconds, the script will send a registration status of "Not Registered", 
	// which this function should detect.
	const TInt pause = 10000000;		// 10 seconds
	User::After(pause);

	// retrieve the network registration status
	TInt ret = TestAPI_GetNetworkRegistrationStatusL();
	TESTL(ret == KErrNone);

	// retrieve the network information
	ret = TestAPI_GetCurrentNetworkL();
	TESTL(ret == KErrNone);

	return (KErrNone);
	}


void CTestDriveNetworkRegistration::testNotificationsL()
/**
 * This method tests Change of Registration status and Current Network notifications.
 */
	{
	// Post a couple of Notification requests while Network coverage is being resumed.
	INFO_PRINTF1(_L("Posting a NotifyChangeOfCurrentNetwork request...\n"));
	TRequestStatus reqStatus1;
	RMobilePhone::TMobilePhoneNetworkInfoV1     phoneNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg phoneNetworkInfoPckg(phoneNetworkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1    locationArea;
	iTestPhone.NotifyCurrentNetworkChange(reqStatus1, phoneNetworkInfoPckg, locationArea);

	INFO_PRINTF1(_L("Posting a NotifyChangeOfNetworkRegistrationStatus request...\n"));
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
	iTestPhone.NotifyNetworkRegistrationStatusChange(reqStatus2, regStatus);

	User::WaitForRequest(reqStatus2);
	User::WaitForRequest(reqStatus1);

	TESTL(reqStatus1 == KErrNone);
	INFO_PRINTF1(_L("Notification Received for CurrentNetworkChange\n"));

	TESTL(reqStatus2 == KErrNone);
	INFO_PRINTF1(_L("Notification Received for NetworkRegistrationStatusChange \n\n"));

	phoneNetworkInfo = phoneNetworkInfoPckg();

	// NotifyChangeOfNetworkRegistration Cancel Request.
	INFO_PRINTF1(_L("Posting a NotifyChangeOfNetworkRegistration Cancel Request...\n"));
	TRequestStatus cancelReqStatus;
	iTestPhone.NotifyNetworkRegistrationStatusChange(cancelReqStatus, regStatus);

	iTestPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange);
	User::WaitForRequest(cancelReqStatus);
	INFO_PRINTF2(_L("NotifyChangeOfNetworkRegistration Cancel Status = %d"), cancelReqStatus.Int());
	TESTL((cancelReqStatus == KErrCancel) || (cancelReqStatus == KErrNone));

	// NotifyChangeOfCurrentNetworkCancel Cancel Request.
	INFO_PRINTF1(_L("Posting a NotifyChangeOfCurrentNetworkCancel Cancel Request..."));
	iTestPhone.NotifyCurrentNetworkChange(cancelReqStatus, phoneNetworkInfoPckg, locationArea);
	iTestPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
	User::WaitForRequest(cancelReqStatus);
	INFO_PRINTF2(_L("NotifyChangeOfCurrentNetwork Cancel Status = %d"), cancelReqStatus.Int());
	TESTL((cancelReqStatus == KErrCancel) || (cancelReqStatus == KErrNone));
	}


void CTestDriveNetworkRegistration::testMoreNotificationsL()
/**
 * This method tests immediate reposting of notifications. The scenario is as follows:
 *		Retrieve current network operator and registration status (Known state).
 *		Post change of registration status and current network operator notifications.
 *		Simulate loss of network coverage and wait for the notifications to complete (Unknown state)
 *		Immediately repost both notifications.
 *		Recover network (Known state)
 *		Wait for both notifications to complete.
 */
	{
	INFO_PRINTF1(_L("Get Current network Info and Registration status.."));

	// retrieve the registration status
	TInt ret = TestAPI_GetCurrentNetworkL();
	TESTL(ret == KErrNone);

	// retrieve the registration status
	ret = TestAPI_GetNetworkRegistrationStatusL();
	TESTL(ret == KErrNone);

	// Post registration status and current network change notifications
	INFO_PRINTF1(_L("Posting a NotifyChangeOfCurrentNetwork request."));
	INFO_PRINTF1(_L("Posting a NotifyChangeOfNetworkRegistrationStatus request."));

	TRequestStatus reqStatusNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1     phoneNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg phoneNetworkInfoPckg(phoneNetworkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1    locationArea;
	iTestPhone.NotifyCurrentNetworkChange(reqStatusNetworkInfo, phoneNetworkInfoPckg, locationArea);

	TRequestStatus reqStatusRegStatus;
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
	iTestPhone.NotifyNetworkRegistrationStatusChange(reqStatusRegStatus, regStatus);


	// Simulate loss of network coverage. Wait 10 seconds, the script will send a 
	// registration status of "Not Registered", which this function should detect and
	// subsequently complete the notifications.
	INFO_PRINTF1(_L("\nSimulating loss of network coverage....\n"));
	const TInt pause = 10000000;			// 10 seconds
	User::After(pause);

	User::WaitForRequest(reqStatusNetworkInfo);
	User::WaitForRequest(reqStatusRegStatus);
	
	TESTL(reqStatusNetworkInfo == KErrNone);
	INFO_PRINTF1(_L("Notification of Change in Current Network Info is successful"));
	TESTL(reqStatusRegStatus == KErrNone);
	INFO_PRINTF1(_L("Notification of Change in Network Registration Status is successful\n"));

	phoneNetworkInfo = phoneNetworkInfoPckg();

	// Display the current network and registration status information (Unknown state)
	DisplayCurrentNetworkInfo(phoneNetworkInfo, locationArea);
	DisplayNetworkRegistrationStatus(regStatus);


	// Repost the notifications
	TRequestStatus reqStatusNetworkInfo3;
	RMobilePhone::TMobilePhoneNetworkInfoV1     phoneNetworkInfo3;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg phoneNetworkInfoPckg3(phoneNetworkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1    locationArea3;
	iTestPhone.NotifyCurrentNetworkChange(reqStatusNetworkInfo3, phoneNetworkInfoPckg3, locationArea3);

	TRequestStatus reqStatusRegStatus3;
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus3;
	iTestPhone.NotifyNetworkRegistrationStatusChange(reqStatusRegStatus3, regStatus3);


	// Simulate recovery of network coverage. Wait 5 seconds, the script will send a 
	// registration status of "Registered on Home network", which this function should 
	// detect and subsequently complete the notifications.
	INFO_PRINTF1(_L("\nSimulating recovery of network coverage....\n"));
	const TInt Pause5 = 5000000;			// 5 seconds
	User::After(Pause5);
	
	User::WaitForRequest(reqStatusNetworkInfo3);
	User::WaitForRequest(reqStatusRegStatus3);
	
	TESTL(reqStatusNetworkInfo3 == KErrNone);
	INFO_PRINTF1(_L("Notification of Change in Current Network Info is successful\n"));
	TESTL(reqStatusRegStatus3 == KErrNone);
	INFO_PRINTF1(_L("Notification of Change in Network Registration Status is successful\n\n"));

	// Display the current network and registration status information (Unknown state)
	DisplayCurrentNetworkInfo(phoneNetworkInfo3, locationArea3);
	DisplayNetworkRegistrationStatus(regStatus3);
	}


TInt CTestDriveNetworkRegistration::TestAPI_GetNetworkRegistrationStatusL(void)
	{
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;

	iTestPhone.GetNetworkRegistrationStatus(reqStatus, regStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone); 

	DisplayNetworkRegistrationStatus(regStatus);

	return (KErrNone);
	}


TInt CTestDriveNetworkRegistration::TestAPI_GetCurrentNetworkL(void)
	{
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV1     phoneNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg phoneNetworkInfoPckg(phoneNetworkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1    locationArea;

	iTestPhone.GetCurrentNetwork(reqStatus, phoneNetworkInfoPckg, locationArea);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);
	phoneNetworkInfo = phoneNetworkInfoPckg();

	INFO_PRINTF1(_L("Network Info: "));

	INFO_PRINTF2(_L("  Mode          = %d"), phoneNetworkInfo.iMode);

	INFO_PRINTF2(_L("  Status        = %d"), phoneNetworkInfo.iStatus);

	if (phoneNetworkInfo.iCountryCode.Length() == 0)
	{
		INFO_PRINTF1(_L("  Country Code  = ''"));
	}
	else
	{
		INFO_PRINTF2(_L("  Country Code  = '%S'"), &phoneNetworkInfo.iCountryCode);
	}

	if (phoneNetworkInfo.iAnalogSID.Length() == 0)
	{
		INFO_PRINTF1(_L("  Analog SID    = ''"));
	}
	else
	{
		INFO_PRINTF2(_L("  Analog SID    = '%S'"), &phoneNetworkInfo.iAnalogSID);
	}

	if (phoneNetworkInfo.iNetworkId.Length() == 0)
	{
		INFO_PRINTF1(_L("  Network Id    = ''"));
	}
	else
	{
		INFO_PRINTF2(_L("  Network Id    = '%S'"), &phoneNetworkInfo.iNetworkId);
	}

	if (phoneNetworkInfo.iDisplayTag.Length() == 0)
	{
		INFO_PRINTF1(_L("  Display Tag   = ''"));
	}
	else
	{
		INFO_PRINTF2(_L("  Display Tag   = '%S'"), &phoneNetworkInfo.iDisplayTag);
	}

	if (phoneNetworkInfo.iShortName.Length() == 0)
	{
		INFO_PRINTF1(_L("  Short Name    = ''"));
	}
	else
	{
		INFO_PRINTF2(_L("  Short Name    = '%S'"), &phoneNetworkInfo.iShortName);
	}

	if (phoneNetworkInfo.iLongName.Length() == 0)
	{
		INFO_PRINTF1(_L("  Long Name     = ''\n"));
	}
	else
	{
		INFO_PRINTF2(_L("  Long Name     = '%S'\n"), &phoneNetworkInfo.iLongName);
	}

	INFO_PRINTF1(_L("Network Location Area: "));
	INFO_PRINTF2(_L("  Area Is Known = %d"), locationArea.iAreaKnown);
	INFO_PRINTF2(_L("  Area Code     = %d"), locationArea.iLocationAreaCode);
	INFO_PRINTF2(_L("  Cell Id       = %d\n"), locationArea.iCellId);

	return (KErrNone);
	}


void CTestDriveNetworkRegistration::DisplayNetworkRegistrationStatus(
						RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus)
	{
	switch (aRegStatus)
		{
		case RMobilePhone::ERegistrationUnknown:
			INFO_PRINTF1(_L("Network registration status: Unknown\n"));
			break;
		case RMobilePhone::ENotRegisteredNoService:
			INFO_PRINTF1(_L("Network registration status: Not Registered No Service\n"));
			break;
		case RMobilePhone::ENotRegisteredEmergencyOnly:
			INFO_PRINTF1(_L("Network registration status: Not Registered Emergency Only\n"));
			break;
		case RMobilePhone::ENotRegisteredSearching:
			INFO_PRINTF1(_L("Network registration status: Not Registered Searching\n"));
			break;
		case RMobilePhone::ERegisteredBusy:
			INFO_PRINTF1(_L("Network registration status: Registered Busy\n"));
			break;
		case RMobilePhone::ERegisteredOnHomeNetwork:
			INFO_PRINTF1(_L("Network registration status: Registered On Home Network\n"));
			break;
		case RMobilePhone::ERegistrationDenied:
			INFO_PRINTF1(_L("Network registration status: Registration Denied\n"));
			break;
		case RMobilePhone::ERegisteredRoaming:
			INFO_PRINTF1(_L("Network registration status: Registered Roaming\n"));
			break;
		default:
			INFO_PRINTF2(_L("Network registration status: %d\n"), aRegStatus);
			break;
		}
	}

void CTestDriveNetworkRegistration::DisplayCurrentNetworkInfo(
						RMobilePhone::TMobilePhoneNetworkInfoV1& aPhoneNetworkInfo,
						RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
	{
	INFO_PRINTF1(_L("Network Info: "));
	INFO_PRINTF2(_L("  Mode          = %d"), aPhoneNetworkInfo.iMode);
	INFO_PRINTF2(_L("  Status        = %d"), aPhoneNetworkInfo.iStatus);
	INFO_PRINTF2(_L("  Country Code  = '%S'"), &aPhoneNetworkInfo.iCountryCode);
	INFO_PRINTF2(_L("  Analog SID    = '%S'"), &aPhoneNetworkInfo.iAnalogSID);
	INFO_PRINTF2(_L("  Network Id    = '%S'"), &aPhoneNetworkInfo.iNetworkId);
	INFO_PRINTF2(_L("  Display Tag   = '%S'"), &aPhoneNetworkInfo.iDisplayTag);
	INFO_PRINTF2(_L("  Short Name    = '%S'"), &aPhoneNetworkInfo.iShortName);
	INFO_PRINTF2(_L("  Long Name     = '%S'\n"), &aPhoneNetworkInfo.iLongName);

	INFO_PRINTF1(_L("Network Location Area: "));
	INFO_PRINTF2(_L("  Area Is Known = %d"), aLocationArea.iAreaKnown);
	INFO_PRINTF2(_L("  Area Code     = %d"), aLocationArea.iLocationAreaCode);
	INFO_PRINTF2(_L("  Cell Id       = %d\n"), aLocationArea.iCellId);
	}


//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestNetworkRegistration* CTestNetworkRegistration::NewL(const TScript* aScript)
	{
	CTestNetworkRegistration* aA = new (ELeave) CTestNetworkRegistration(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestNetworkRegistration::CTestNetworkRegistration(const TScript* aScript) :
	iScript(aScript)
	{}

void CTestNetworkRegistration::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestNetworkRegistration::Start()
	{
	StartScript(iScript);
	return (KErrNone);
	}

void CTestNetworkRegistration::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestNetworkRegistration::Complete(TInt aError)
	{
	iReturnValue = aError;
	CActiveScheduler::Stop();
	}


