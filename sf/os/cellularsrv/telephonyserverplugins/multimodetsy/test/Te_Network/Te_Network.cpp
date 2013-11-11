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
// This file contains the main methods that test the Network
// Functionality of the MMTSY
// 
//


#include "Te_Network.h"

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
	TInt ret=iTelServer.GetPhoneInfo(numOfPhones-1, phoneInfo);

	if (ret==KErrNone)
		{
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
		}

	// Query TSY about supported functionality
	TBool result;
	TInt mixin=1; // same value used by etel\tetel\t_hyres1.cpp

	TBuf<10> tsyName;
	tsyName = KMmtsyName;
	ret=iTelServer.IsSupportedByModule(tsyName, mixin, result);

	if(ret==KErrNone)
		{
		INFO_PRINTF1(_L("Querying TSY about supported functionality is Successful"));
		}

	return TestStepResult();
	}




	
/*-- Get Network Caps -----------------------------------------------------------------*/

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
3.10.1 Get Network Caps
TInt RMobilePhone::GetNetworkCaps(TUint32& aCaps) const
This method returns the static network capabilities of the phone.
The aCaps parameter returned is the sum of TNetworkCaps constants detailed in the table 
above. If the TSY/ME does not support any client access to network functionality then 
the request should be completed with KErrNotSupported.
*/
	{
	INFO_PRINTF1(_L("Get Network Caps"));

	TInt ret=iPhone.GetNetworkCaps(iCaps);

	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Network Capabilities NOT Supported"));
		}
	
	else if (ret==KErrNone)
		{
		if(iCaps & RMobilePhone::KCapsGetRegistrationStatus)
			INFO_PRINTF1(_L("The phone supports retrieval of current registration status"));

		if(iCaps & RMobilePhone::KCapsNotifyRegistrationStatus)
			INFO_PRINTF1(_L("The phone supports notifications of change in registration status"));

		if(iCaps & RMobilePhone::KCapsGetCurrentMode)
			INFO_PRINTF1(_L("The phone supports retrieval of current mode"));

		if(iCaps & RMobilePhone::KCapsNotifyMode)
			INFO_PRINTF1(_L("The phone supports notification of change in mode"));

		if(iCaps & RMobilePhone::KCapsGetCurrentNetwork)
			INFO_PRINTF1(_L("The phone supports retrieval of current network information"));

		if(iCaps & RMobilePhone::KCapsNotifyCurrentNetwork)
			INFO_PRINTF1(_L("The phone supports notification of change of current network"));

		if(iCaps & RMobilePhone::KCapsGetHomeNetwork)
			INFO_PRINTF1(_L("The phone supports retrieval of home network information"));

		if(iCaps & RMobilePhone::KCapsGetDetectedNetworks)
			INFO_PRINTF1(_L("The phone supports retrieval of a list of detected networks"));

		if(iCaps & RMobilePhone::KCapsManualNetworkSelection)
			INFO_PRINTF1(_L("The phone supports manual network selection mode"));
		}
	else
		{
			SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}



/*-- Disconnected Network Test --------------------------------------------------------*/

CDisconnectedNetworkTest::CDisconnectedNetworkTest()
/** Each test step initialises it's own name
*/
	{

	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("DisconnectedNetworkTest"));

	}		

TVerdict CDisconnectedNetworkTest::doTestStepL( void )
/**
 * This function gets the Network Status while Disconnected (optional test).
 * @test GT83-TNETWORK-004.00 Test retrieval of the phone's current mode (Generic)
 * @test GT83-TNETWORK-006.01 Test retrieving information about the phone's current 
 network and print the information to screen
*/
	{
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneRegistrationStatus localRegStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV1  localNetworkInfo;
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg localNetworkInfoBuf(localNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer


	INFO_PRINTF1(_L("Test Disconnected Network"));

	/*
	// Test previously prompted tester to run this test or not This is no longer
	// possible as there is no way to prompt user in scheduletest
  
	Log(_L("\nIf you can cause the phone to disconnect from the network,"));
	Log(_L("\nplease do so now and then press Y.")); 
	Log(_L("\n*** Press any other key to skip this test ***\n"));
		
	TKeyCode key=gTest.Getch();
	if (key!='y' && key!='Y')
		{
		INFO_PRINTF1(_L("Skipping disconnected network test\n"));
		return EAbort;
		}
	*/

	
	// Prompt user to somehow make phone disconnect from network
	// A good way to do this may be to remove the aerial or use some form of
	// Faraday cage
	INFO_PRINTF1(_L("**************************************************************"));
	INFO_PRINTF1(_L("* If you can cause the phone to disconnect from the network, *"));
	INFO_PRINTF1(_L("* please do so now...                                        *"));
	INFO_PRINTF1(_L("**************************************************************"));

	// wait for 40 seconds for user to get phone out of reception and for phone to
	// realise it is no longer in reception and drop its network registration settings
	User::After(40000000);

	iPhone.GetNetworkRegistrationStatus(iStatus, localRegStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-004.00")) ;

	PrintRegistration(localRegStatus);

 	// TESTL(localRegStatus!=RMobilePhone::ERegisteredOnHomeNetwork);
	// TESTL(localRegStatus!=RMobilePhone::ERegisteredRoaming);

	TBool stillRegistered = (localRegStatus==RMobilePhone::ERegisteredOnHomeNetwork||
			localRegStatus==RMobilePhone::ERegisteredRoaming);


	iPhone.GetCurrentNetwork(iStatus, localNetworkInfoBuf, aArea);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-006.01")) ;

	if (iStatus!=KErrNone)
		{
		INFO_PRINTF1(_L("This phone does not support all forms of the Operator Command"));
		INFO_PRINTF1(_L("The information that is supported is listed below"));
		}

	localNetworkInfo = localNetworkInfoBuf();

	// Prints the status part ...
	INFO_PRINTF1(_L("The Status of the Network : "));

	switch(localNetworkInfo.iStatus)
	{
		case RMobilePhone::ENetworkStatusUnknown:
			INFO_PRINTF1(_L("Status is unknown."));
		break;

		case RMobilePhone::ENetworkStatusAvailable:
			INFO_PRINTF1(_L("A network that the Mobile Equipment is allowed to register to."));
		break;

		case RMobilePhone::ENetworkStatusCurrent:
			INFO_PRINTF1(_L("The currently registered network"));
		break;

		case RMobilePhone::ENetworkStatusForbidden:
			INFO_PRINTF1(_L("A network to which the Mobile Equipment is not allowed to register to."));
		break;
	}

	// Prints long and short names of the network ... (e.g VODAPHONE and VODA ...)

	INFO_PRINTF2(_L("\tNetwork Long name  = >%S<"), &localNetworkInfo.iLongName);
	INFO_PRINTF2(_L("\tNetwork Short name = >%S<"), &localNetworkInfo.iShortName);

	// Prints the MCC (Mobile Country Code) and the MNC (Mobile Network Code)
	//???? INFO_PRINTF1(_L("\tNetwork Numbers    = country >%S< network >%S<"), &localNetworkInfo.iCountryCode, &localNetworkInfo.iNetworkId);

	// TMobilePhoneLocationAreaV1

	if(aArea.iAreaKnown)
		{
		// Print the Location area code and the Cell ID
		INFO_PRINTF3(_L("\tCurrent Location   = area %4x cell %4x"), aArea.iLocationAreaCode, aArea.iCellId);
		}
	else
		{
		INFO_PRINTF1(_L("Location area is unknown"));
		}

	validateNetworkInfo(localNetworkInfo, localRegStatus);

	
	INFO_PRINTF1(_L("**************************************************************"));
	INFO_PRINTF1(_L("* Test Complete! Please ensure the phone is connected to the *"));
	INFO_PRINTF1(_L("* network.                                                   *"));
	INFO_PRINTF1(_L("**************************************************************"));


	// wait for 60 seconds for user to get phone back in reception and for phone to
	// realise it is back in reception and reconnect to the network
	User::After(60000000);

	if(stillRegistered)
		{
		INFO_PRINTF1(_L("Phone was not disconnected from the network"));
		return EInconclusive;
		}
	
	return TestStepResult();
	}
	


/*-- Network Registration tests -------------------------------------------------------*/

CGetNetworkRegistrationStatus::CGetNetworkRegistrationStatus()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetNetworkRegistrationStatus"));
	}

TVerdict CGetNetworkRegistrationStatus::doTestStepL( void )
/**
3.10.3 Get Network Registration Status
1. RMobilePhone::GetNetworkRegistrationStatus(TRequestStatus& aReqStatus,
TMobilePhoneRegistrationStatus& aStatus) const

This method returns information about the current network registration status.
The aRegistrationStatus parameter returned is one of the enumerated
TMobilePhoneRegistrationStatus values.
* @test GT83-TNETWORK-002.00 Test retrieval of the phone's network registration status (Generic)
*/
	{
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneNetworkInfoV1  bNetworkInfo	= aNetworkInfoBuf();
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus ;

	INFO_PRINTF1(_L("Test GetNetworkRegistrationStatus"));

	//4.4 Get Network Registration Status
	iPhone.GetNetworkRegistrationStatus(iStatus, regStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-002.00"));

	PrintRegistration(regStatus);

	validateNetworkInfo(bNetworkInfo, regStatus);

	if((regStatus == RMobilePhone::ERegisteredOnHomeNetwork) || (regStatus == RMobilePhone::ERegisteredRoaming))
		{
		// Phone seems to provide operator information
		iOperatorInfoSupported = ETrue;
		}
	
	return TestStepResult();
	}



CGetNetworkRegistrationStatusCancel::CGetNetworkRegistrationStatusCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetNetworkRegistrationStatusCancel"));
	}	

TVerdict CGetNetworkRegistrationStatusCancel::doTestStepL( void )
/**
3.10.3.1 Cancel Get Network Registration Status
void RMobilePhone::GetNetworkRegistrationStatusCancel() const
	
This method cancels an outstanding asynchronous GetNetworkRegistrationStatus request.
* @test GT83-TNETWORK-002.00 Test retrieval of the phone's network registration 
status (Generic)
* @test GT83-TNETWORK-002.08 Test that the Get Network Registration status 
Request can be cancelled
*/
	{

	RMobilePhone::TMobilePhoneRegistrationStatus  regStatus;
	INFO_PRINTF1(_L("Test GetNetworkRegistrationStatusCancel"));
	iPhone.GetNetworkRegistrationStatus(iStatus, regStatus);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-002.00"));

	iPhone.CancelAsyncRequest(EMobilePhoneGetNetworkRegistrationStatus) ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone), 1, _L("GT83-TNETWORK-002.08"));

	return TestStepResult();
	}




CNotifyNetworkRegistrationStatusChange::CNotifyNetworkRegistrationStatusChange()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyNetworkRegistrationStatusChange"));
	}	

TVerdict CNotifyNetworkRegistrationStatusChange::doTestStepL( void )
/**
3.10.4 Notify Network Registration Status Change
void RMobilePhone::NotifyNetworkRegistrationStatusChange(TRequestStatus& aReqStatus,
TMobilePhoneRegistrationStatus& aRegistrationStatus) const
 
This method allows the client to be notified of changes to the current network
registration status. When the registration status changes, the TSY will trigger t
his notification and return the new registration status within the aRegistrationStatus
parameter. This will take one of the enumerated TMobilePhoneRegistrationStatus values
detailed in the table in section 3.10.3.
* @test GT83-TNETWORK-003.01 Test setting a request for notification when there is a 
change in the phone's network registration status
* @test GT83-TNETWORK-003.02 Test for a notification when there is a change in the
phone's network registration status
*/
	{
	RMobilePhone::TMobilePhoneRegistrationStatus localRegStatus;

	INFO_PRINTF1(_L("Test NotifyNetworkRegistrationStatusChange"));

	iPhone.NotifyNetworkRegistrationStatusChange(iStatus, localRegStatus);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-003.01")) ;
	//@note we need to change it some how

	//User::WaitForRequest(iStatus);
	//TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-003.02")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange) ;
	User::WaitForRequest(iStatus);
	TESTL(iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported);

	return TestStepResult();
	}



CNotifyNetworkRegistrationStatusChangeCancel::CNotifyNetworkRegistrationStatusChangeCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyNetworkRegistrationStatusChangeCancel"));
	}

TVerdict CNotifyNetworkRegistrationStatusChangeCancel::doTestStepL( void )
/**
3.10.4.1 Cancel Notify Network Registration Status Change
void RMobilePhone::NotifyNetworkRegistrationStatusChangeCancel() const

This method cancels an outstanding request to be notified about changes to the current
network registration status, placed with the NotifyNetworkRegistrationStatusChange method.
* @test GT83-TNETWORK-003.01 Test setting a request for notification when there is a change 
in the phone's network registration status
* @test GT83-TNETWORK-003.03 Test cancelling a request for notification when there is a 
change in phone's network registration status
*/
	{
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneRegistrationStatus localRegStatus;

	INFO_PRINTF1(_L("Test NotifyNetworkRegistrationStatusChangeCancel"));

	iPhone.NotifyNetworkRegistrationStatusChange(iStatus, localRegStatus);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-003.01"));

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-003.03"));
	
	return TestStepResult();
	}



/*-- Network Mode Tests ---------------------------------------------------------------*/

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
3.10.5 Get Current Network Mode
TInt RMobilePhone::GetCurrentMode(TMobilePhoneNetworkMode& aNetworkMode) const

This method returns information about the current network mode. The aNetworkMode
parameter returned is one of the enumerated TMobilePhoneNetworkMode values detailed .
* @test GT83-TNETWORK-004.00 Test retrieval of the phone's current mode (Generic)
*/
	{
	INFO_PRINTF1(_L("Test GetCurrentMode"));
	RMobilePhone::TMobilePhoneNetworkMode aNetworkMode ;
	iStatus = iPhone.GetCurrentMode(aNetworkMode) ;
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-004.00"));
	PrintNetworkMode(aNetworkMode) ;
	return TestStepResult();

	}



CNotifyModeChange::CNotifyModeChange()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyModeChange"));
	}

TVerdict CNotifyModeChange::doTestStepL( void )
/**
3.10.6 Notify Network Mode Change
void RMobilePhone::NotifyModeChange(TRequestStatus& aReqStatus,TMobilePhoneNetworkMode& aNetworkMode)

This method allows the client to be notified of changes to the current network mode.
When the mode changes, the TSY will trigger this notification and return the new mode
within the aNetworkMode parameter. This will take one of the enumerated 
TMobilePhoneNetworkMode values.
* @test GT83-TNETWORK-005.01 Test setting a request for notification when there is a 
change in the phone's network mode
* @test GT83-TNETWORK-005.02 Test for a notification when there is a change in the 
phone's network mode
*/
	{
	INFO_PRINTF1(_L("Test NotifyModeChange"));

	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNetworkMode aNetworkMode ;

	iPhone.NotifyModeChange(iStatus, aNetworkMode);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-005.01"));

	//we don't want to wait forever, we have to invent something better
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyModeChange)  ;

	User::WaitForRequest(iStatus);
	// 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TESTL(iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported);


	//User::WaitForRequest(iStatus);
	//TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-005.02"))

	return TestStepResult();

	}



CNotifyModeChangeCancel::CNotifyModeChangeCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyModeChangeCancel"));
	}

TVerdict CNotifyModeChangeCancel::doTestStepL( void )
/**
3.10.6.1 Cancel Notify Network Mode Change
void RMobilePhone::NotifyModeChangeCancel() const

This method cancels an outstanding asynchronous NotifyModeChange request.
* @test GT83-TNETWORK-005.01 Test setting a request for notification when 
there is a change in the phone's network mode
* @test GT83-TNETWORK-005.03 Test cancelling a request for notification 
when there is a change in phone's network mode
*/
	{
	INFO_PRINTF1(_L("Test NotifyModeChangeCancel"));

	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNetworkMode aNetworkMode ;

	iPhone.NotifyModeChange(iStatus, aNetworkMode);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-005.01"));

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyModeChange)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-005.03"));
	
	return TestStepResult();
	}



/*-- Current Network Information Tests ------------------------------------------------*/

CGetCurrentNetwork::CGetCurrentNetwork()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetCurrentNetwork"));
	}

TVerdict CGetCurrentNetwork::doTestStepL( void )
/**
3.10.7 Get Current Network
Structured Types:
void RMobilePhone::GetCurrentNetwork(TRequestStatus& aReqStatus, TDes8& aNetworkInfo, 
TMobilePhoneLocationAreaV1& aArea) const

This method returns information about the network that the phone is currently
registered to. This current network information is returned in the aNetworkInfo parameter.
The packaged version of TMobilePhoneNetworkInfoV1 will be returned. The elements
of this structure are described in section 3.10.2.
In GSM/ WCDMA mode, it may also be possible to return information on the current
location of the phone. This information is returned within the aArea parameter.
If the location area code and cell identifier are unavailable the iAreaKnown boolean
will indicate EFalse.
* @test GT83-TNETWORK-006.01 Test retrieving information about the phone's current network 
and print the information to screen
*/
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test GetCurrentNetwork()"));

	iPhone.GetCurrentNetwork(iStatus, aNetworkInfoBuf, aArea);
	User::WaitForRequest(iStatus);
	
	if (iStatus.Int()==KErrNotSupported)
		{	
		INFO_PRINTF1(_L("This phone does not support the GetCurrentNetwork command"));
		return EInconclusive;
		}

	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-006.01")) ;

	RMobilePhone::TMobilePhoneNetworkInfoV1  bNetworkInfo	= aNetworkInfoBuf();
	PrintNetworkStatus(bNetworkInfo, aArea) ;

	return TestStepResult();
	}



CGetCurrentNetworkCancel::CGetCurrentNetworkCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetCurrentNetworkCancel"));
	}

TVerdict CGetCurrentNetworkCancel::doTestStepL( void )
/**
3.10.7.1 Cancel Get Current Network
void RMobilePhone::GetCurrentNetworkCancel() const

This method cancels an outstanding asynchronous GetCurrentNetwork request.
* @test GT83-TNETWORK-006.01 Test retrieving information about the phone's 
current network and print the information to screen
* @test GT83-TNETWORK-006.02 Test cancelling a request to retrieve information
about the phones current network
*/
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test GetCurrentNetworkCancel()"));

	iPhone.GetCurrentNetwork(iStatus, aNetworkInfoBuf, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-006.01")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork)  ;
	User::WaitForRequest(iStatus);

	if (iStatus.Int()==KErrNotSupported)
		{	
		INFO_PRINTF1(_L("This phone does not support the GetCurrentNetworkCancel command"));
		return EInconclusive;
		}

	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone), 1, _L("GT83-TNETWORK-006.02")) ;

	return TestStepResult();
	}



CNotifyCurrentNetworkChange::CNotifyCurrentNetworkChange()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyCurrentNetworkChange"));
	}

TVerdict CNotifyCurrentNetworkChange::doTestStepL( void )

/**
3.10.8 Notify Current Network Change
void RMobilePhone::NotifyCurrentNetworkChange(TRequestStatus& aReqStatus, 
TDes8& aNetworkInfo, TMobilePhoneLocationArea& aArea)

This method allows the client to be notified of changes to the current network. Upon 
completion the new network information will be returned in a packaged version in the 
aNetworkInfo parameter. If the current location of the phone is known then this 
information can be returned within the aArea parameter whose structure is defined in 
section 3.10.7
* @test GT83-TNETWORK-007.01 Test setting a request for notification when there is a 
change in the phone's current network
* @test GT83-TNETWORK-007.02 Test for a notification when there is a change in the 
phone's current network
*/
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test NotifyCurrentNetworkChange()"));

	iPhone.NotifyCurrentNetworkChange(iStatus, aNetworkInfoBuf, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-007.01"));

	// Difficult to get the phone to change "live" networks
	// So just cancel request if not completed after 10 seconds
	User::After(10000000);
	if (iStatus==KRequestPending)
		{
		iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange)  ;
		User::WaitForRequest(iStatus);
		}
	else if (iStatus.Int()==KErrNotSupported)
		{	
		User::WaitForRequest(iStatus);
		INFO_PRINTF1(_L("This phone does not support the NotifyCurrentNetworkChange command"));
		return EInconclusive;
		}
	else
		{
		User::WaitForRequest(iStatus);
		TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-006.01")) ;
		RMobilePhone::TMobilePhoneNetworkInfoV1  bNetworkInfo	= aNetworkInfoBuf();
		PrintNetworkStatus(bNetworkInfo, aArea) ;
		}

	return TestStepResult();
	}



CNotifyCurrentNetworkChangeCancel::CNotifyCurrentNetworkChangeCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyCurrentNetworkChangeCancel"));
	}

TVerdict CNotifyCurrentNetworkChangeCancel::doTestStepL( void )

/**
3.10.8.1 Cancel Notify Current Network Change
void RMobilePhone::NotifyCurrentNetworkChangeCancel() const

This method cancels an outstanding asynchronous NotifyCurrentNetworkChange request.
* @test GT83-TNETWORK-007.01 Test setting a request for notification when there is a 
change in the phone's current network
* @test GT83-TNETWORK-007.03 Test cancelling a request for notification when there is 
a change in phone's current network
*/
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test NotifyCurrentNetworkChangeCancel()"));

	iPhone.NotifyCurrentNetworkChange(iStatus, aNetworkInfoBuf, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-007.01")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange)  ;
	User::WaitForRequest(iStatus);

	if (iStatus.Int()==KErrNotSupported)
		{	
		INFO_PRINTF1(_L("This phone does not support the GetCurrentNetworkCancel command"));
		return EInconclusive;
		}
	
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone), 1, _L("GT83-TNETWORK-007.03"));

	return TestStepResult();
	}



/*-- Home Network Tests ---------------------------------------------------------------*/

CGetHomeNetwork::CGetHomeNetwork()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetHomeNetwork"));
	}
	
TVerdict CGetHomeNetwork::doTestStepL( void )
/**
3.10.9 Get Home Network
void RMobilePhone::GetHomeNetwork(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const

This method provides the client with information on the home network of this subscriber. 
This allows the client to compare detected and/or current network information with the 
home network information in order to determine if the current or detected networks are 
also the home network. This home network information is returned in the aNetworkInfo 
parameter. The packaged version of TMobilePhoneNetworkInfoV1 will be returned. The 
elements of this structure are described in section 3.10.2.
*/
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test GetHomeNetwork()"));

	iPhone.GetHomeNetwork(iStatus, aNetworkInfoBuf) ;

	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNotSupported, _L("GT83-TNETWORK-000.00"));

	RMobilePhone::TMobilePhoneNetworkInfoV1  bNetworkInfo	= aNetworkInfoBuf();
	PrintNetworkStatus(bNetworkInfo, aArea) ;
	return TestStepResult();
	}



CGetHomeNetworkCancel::CGetHomeNetworkCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetHomeNetworkCancel"));
	}

TVerdict CGetHomeNetworkCancel::doTestStepL( void )
/**
3.10.9.1 Cancel Get Home Network
void RMobilePhone::GetHomeNetworkCancel() const

This method cancels a previously placed asynchronous GetHomeNetwork request.
*/
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer

	INFO_PRINTF1(_L("Test GetHomeNetworkCancel()"));

	iPhone.GetHomeNetwork(iStatus, aNetworkInfoBuf) ;
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-000.00")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneGetHomeNetwork)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00"));

	return TestStepResult();
	}


/*-- Detected Networks Test -----------------------------------------------------------*/

CDetectedNetworkTest::CGetDetectedNetworks* CDetectedNetworkTest::CGetDetectedNetworks::NewLC(RMobilePhone& aPhone)
	{
	CGetDetectedNetworks* r = new(ELeave) CGetDetectedNetworks(aPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
	}

CDetectedNetworkTest::CGetDetectedNetworks::CGetDetectedNetworks(RMobilePhone& aPhone)
    : CActive(EPriorityNormal), iPhone(aPhone)
	{
	}

CDetectedNetworkTest::CGetDetectedNetworks::~CGetDetectedNetworks()
    {
	delete iRetrieve;
	}



void CDetectedNetworkTest::CGetDetectedNetworks::Start()
	{
    iRetrieve->Start(iStatus);
    SetActive();	
	}

CMobilePhoneNetworkList* CDetectedNetworkTest::CGetDetectedNetworks::RetrieveList()
	{
	CMobilePhoneNetworkList *list = NULL ;
	TRAP_IGNORE(list=iRetrieve->RetrieveListL()) ;
	return (list);
	}

TInt CDetectedNetworkTest::CGetDetectedNetworks::CompleteCode()
	{
	return iCompleteCode;
	}

void CDetectedNetworkTest::CGetDetectedNetworks::ConstructL()
	{
	CActiveScheduler::Add(this);
	iRetrieve = CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
	}

void CDetectedNetworkTest::CGetDetectedNetworks::RunL()
	{
	iCompleteCode = iStatus.Int();
    CActiveScheduler::Stop();
	}


void CDetectedNetworkTest::CGetDetectedNetworks::DoCancel()
    {
    iRetrieve->Cancel();
    }


CDetectedNetworkTest::CDetectedNetworkTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("DetectedNetworkTest"));
	}

TVerdict CDetectedNetworkTest::doTestStepL( void )
/**
 * Detected Network test. This function gets the number of available networks
 * and information on these networks. Also tests some Cancel Requests.
 * This test is run only if Operator Information is supported by the phone.
 3.10.10 Get Detected Networks
	 Clients can retrieve a list of all networks that are currently detected
	by the phone. Each detected network will be represented by an entry in
	the list class (CMobilePhoneNetworkList). See section 9.3  for details of
	this list class. The client is able to use the methods of the list class
	to enumerate and read the detected network entries.
	Due to the probable time taken to retrieve a list, clients must use an asynchronous
	approach based around the use of an active object, CRetrieveMobilePhoneDetectedNetworks.
 * @test GT83-TNETWORK-002.00 Test retrieval of the phone's network registration status (Generic)

*/
	{
	INFO_PRINTF1(_L("Test Detected Networks"));
	
	INFO_PRINTF1(_L("This test can take up to a minute to complete..."));

	iTestScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);

	CGetDetectedNetworks* retrieveMobileNets;
	CMobilePhoneNetworkList *networkList=NULL;
	retrieveMobileNets = CGetDetectedNetworks::NewLC(iPhone);

	// Does phone provide operator information?
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus ;
	iPhone.GetNetworkRegistrationStatus(iStatus, regStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-002.00"));

	if((regStatus == RMobilePhone::ERegisteredOnHomeNetwork) || (regStatus == RMobilePhone::ERegisteredRoaming))
		{
		// Phone seems to provide operator information
		iOperatorInfoSupported = ETrue;
		}

	if (!iOperatorInfoSupported)
		{
		INFO_PRINTF1(_L("Network Operator information not supported - skipping this test"));
		CleanupStack::PopAndDestroy(); // retrieveMobileNets;
		return EInconclusive;
		}
	
	retrieveMobileNets->Start();
	CActiveScheduler::Start();

	TInt completeCode=retrieveMobileNets->CompleteCode();
	if(completeCode==KErrNotSupported)		// Do not consider KErrNotSupported a test failure
		{
		INFO_PRINTF1(_L("KErrNotSupported returned"));
		}
	else if(completeCode!=KErrNone)
		{
		// Log that and error occurred
		INFO_PRINTF1(_L("Network Detection Error"));
		}

	if(completeCode==KErrNone)
		{
		networkList = retrieveMobileNets->RetrieveList();

		const TInt count = networkList->Enumerate();

		INFO_PRINTF2(_L("CRetrieveMobilePhoneDetectedNetworks::Start() reports %d networks"), count);

		RMobilePhone::TMobilePhoneNetworkInfoV1 network;
		for (TInt i=0; i<count; i++)
			{
			network = networkList->GetEntryL(i);

			INFO_PRINTF6(_L("%d = >%S< >%S< country >%S< network >%S< status : "), i,
				&network.iLongName, &network.iShortName,
				&network.iCountryCode, &network.iNetworkId);


			switch(network.iStatus)
				{
				case RMobilePhone::ENetworkStatusUnknown:
					INFO_PRINTF1(_L("Status is unknown"));
				break;

				case RMobilePhone::ENetworkStatusAvailable:
					INFO_PRINTF1(_L("A network that the Mobile Equipment is allowed to register to"));
				break;

				case RMobilePhone::ENetworkStatusCurrent:
					INFO_PRINTF1(_L("The currently registered network"));
				break;

				case RMobilePhone::ENetworkStatusForbidden:
					INFO_PRINTF1(_L("A network that the Mobile Equipment is not allowed to register to"));
				break;
				}
			}
		}

	CleanupStack::PopAndDestroy(); // retrieveMobileNets;
	delete networkList;
	delete iTestScheduler;
	User::After(5*KOneSecond);		// Give user time to see the test results

	return TestStepResult();
	}

/*-- Network Selection Settings tests -------------------------------------------------*/

CGetNetworkSelectionSetting::CGetNetworkSelectionSetting()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetNetworkSelectionSetting"));
	}

TVerdict CGetNetworkSelectionSetting::doTestStepL( void )
/**
*	3.10.11 Get Network Selection Setting
*	TInt RMobilePhone::GetNetworkSelectionSetting(TDes8& aSetting) const
*	This method returns the current value of the network selection setting.
*	The aSetting parameter returned is a packaged version of the
*	TMobilePhoneNetworkSelectionV1V1 class above.
*	It is a synchronous method because it is assumed that the TSY will
*	have previously retrieved and locally stored this system setting.
*	@note 11.09.2001 TMobilePhoneNetworkSelectionV1V1 has not been implemended yet, 
*	so I have to use TMobilePhoneNetworkSelectionV1
*/
	{

	INFO_PRINTF1(_L("Test GetNetworkSelectionSetting()"));

	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;
	TInt ret = iPhone.GetNetworkSelectionSetting(localNetworkSelBuf) ;
	TEST_CHECKL(ret, KErrNotSupported, _L("GT83-TNETWORK-000.00")) ;

	PrintSelectionSettings(localNetworkSel) ;
	
	return TestStepResult();
	}
	



CSetNetworkSelectionSetting::CSetNetworkSelectionSetting()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SetNetworkSelectionSetting"));
	}

TVerdict CSetNetworkSelectionSetting::doTestStepL( void )
/**
*	3.10.12 Set Network Selection Setting
*	void RMobilePhone::SetNetworkSelectionSetting(TRequestStatus& aReqStatus,
*		const TDes8& aSetting) const
*	This method is used by client application to set the way a phone will select a
*	network. The new setting is supplied within the aSetting parameter, which is a
*	packaged version of the TMobilePhoneNetworkSelectionV1V1 class above.
*	In manual network selection, the user is able to select a network to camp on to
*	from a list of detected networks. The selected network has to be accessible by the
*	user, i.e. it is not a forbidden network.
*	When a phone is using manual network selection, at power-up or after signal loss,
*	it will try to register to the network that the user last selected. If this network
*	is not available then it is up to the phone whether it tries to fall-back to
*	automatic selection or leaves the phone in no service.
*
*/
	{
	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;


	INFO_PRINTF1(_L("Test SetNetworkSelectionSetting()"));

	iPhone.GetNetworkSelectionSetting(localNetworkSelBuf) ;

	iPhone.SetNetworkSelectionSetting(iStatus, localNetworkSelBuf) ;
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-000.00")) ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus == KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;

	return TestStepResult();
	}



CSetNetworkSelectionSettingCancel::CSetNetworkSelectionSettingCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SetNetworkSelectionSettingCancel"));
	}

TVerdict CSetNetworkSelectionSettingCancel::doTestStepL( void )
/**
3.10.12.1 Cancel Set Network Selection Setting
void RMobilePhone::SetNetworkSelectionSettingCancel() const

This method cancels an outstanding asynchronous SetNetworkSelectionSetting request

@note 11.09.2001 TMobilePhoneNetworkSelectionV1V1 has not been implemended yet, so I have to use
TMobilePhoneNetworkSelectionV1
*/
	{
	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;

	INFO_PRINTF1(_L("Test SetNetworkSelectionSettingCancel()"));

	iPhone.GetNetworkSelectionSetting(localNetworkSelBuf) ;

	iPhone.SetNetworkSelectionSetting(iStatus, localNetworkSelBuf) ;
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-000.00")) ;
	iPhone.CancelAsyncRequest(EMobilePhoneSetNetworkSelectionSetting)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;

	return TestStepResult();
	}




CNotifyNetworkSelectionSettingChange::CNotifyNetworkSelectionSettingChange()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyNetworkSelectionSettingChange"));
	}

TVerdict CNotifyNetworkSelectionSettingChange::doTestStepL( void )
/**
3.10.13 Notify Network Selection Setting Change
void RMobilePhone::NotifyNetworkSelectionSettingChange(TRequestStatus& aReqStatus,
	TDes8& aSetting) const
This method allows the client to be notified of changes to the network selection
setting.  Upon completion the new network selection setting will be returned in the
aSetting parameter, which is a packaged version of the TMobilePhoneNetworkSelectionV1V1
class above.
*/
	{
	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;


	INFO_PRINTF1(_L("Test NotifyNetworkSelectionSettingChange()"));

	iPhone.NotifyNetworkSelectionSettingChange(iStatus, localNetworkSelBuf) ;
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-000.00"));
	//!!!!!!!!!!!!!!!!!!!!!!!!! Stub only
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSelectionSettingChange)  ;
	User::WaitForRequest(iStatus);
	//TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-000.00"));
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TESTL(iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported);

	return TestStepResult();
	}



CNotifyNetworkSelectionSettingChangeCancel::CNotifyNetworkSelectionSettingChangeCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyNetworkSelectionSettingChangeCancel"));
	}

TVerdict CNotifyNetworkSelectionSettingChangeCancel::doTestStepL( void )
/**
3.10.13.1 Notify Network Selection Setting Change Cancel
void RMobilePhone::NotifyNetworkSelectionSettingChangeCancel() const
	
This method cancels an outstanding asynchronous
NotifyNetworkSelectionSettingChange request.
*/

	{
	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;

	iPhone.NotifyNetworkSelectionSettingChange(iStatus, localNetworkSelBuf) ;
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-000.00")) ;
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSelectionSettingChange)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;

	return TestStepResult();
	}


/*-- Date & Tim tests -----------------------------------------------------------------*/

CGetNITZInfo::CGetNITZInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetNITZInfo"));
	}

TVerdict CGetNITZInfo::doTestStepL( void )
/**
3.10.15.2 Get NITZ Information
TInt RMobilePhone::GetNITZInfo(TMobilePhoneNITZ& aNITZInfo) const

This method returns the current time information received from the network
in the TMobilePhoneNITZ parameter.  It is a synchronous method because it is
assumed that the TSY has previously retrieved and locally stored this information.
If the TSY/ME does not support access to network time and date information
then the request should be completed with KErrNotSupported.
If the TSY has not received any network time and date information then the
request should be completed with KErrNotFound.
*/
	{
	RMobilePhone::TMobilePhoneNITZ aNITZInfo ;
	INFO_PRINTF1(_L("Test GetNITZInfo()"));

	TInt ret = iPhone.GetNITZInfo(aNITZInfo) ;
	TESTL(ret == KErrNone || ret == KErrNotSupported || ret == KErrNotFound) ;

	return TestStepResult();
	}



CNotifyNITZInfoChange::CNotifyNITZInfoChange()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyNITZInfoChange"));
	}

TVerdict CNotifyNITZInfoChange::doTestStepL( void )
/**
3.10.15.3 Notify NITZ Information Change
void RMobilePhone::NotifyNITZInfoChange(TRequestStatus& aReqStatus,
TMobilePhoneNITZ& aNITZInfo) const

This method allows the client to be notified of changes to the network time
and date info. Upon completion the new network time information will be returned
in the aNITZInfo parameter.
*/
	{
	RMobilePhone::TMobilePhoneNITZ aNITZInfo ;


	INFO_PRINTF1(_L("Test NotifyNITZInfoChange()"));

	iPhone.NotifyNITZInfoChange(iStatus, aNITZInfo) ;
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-000.00")) ;

	//!!!!!!!!!!!!!!!!!!!!!!!!! Stub only
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNITZInfoChange)  ;
	User::WaitForRequest(iStatus);
	TESTL(iStatus == KErrNone || iStatus == KErrCancel || iStatus==KErrNotSupported);

	return TestStepResult();
	}



CNotifyNITZInfoChangeCancel::CNotifyNITZInfoChangeCancel()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NotifyNITZInfoChangeCancel"));
	}

TVerdict CNotifyNITZInfoChangeCancel::doTestStepL( void )
/**
3.10.15.3.1 Notify NITZ Information Change Cancel
void RMobilePhone::NotifyNITZInfoChangeCancel() const

This method cancels an outstanding asynchronous NotifyNITZInfoChange request.
*/
	{
	RMobilePhone::TMobilePhoneNITZ aNITZInfo ;

	INFO_PRINTF1(_L("Test NotifyNITZInfoChangeCancel()"));

	iPhone.NotifyNITZInfoChange(iStatus, aNITZInfo) ;
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-000.00")) ;

	//!!!!!!!!!!!!!!!!!!!!!!!!! Stub only
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNITZInfoChange)  ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus == KErrNone || iStatus == KErrCancel || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;

	return TestStepResult();
	}

//------------------------------------------------------------------


