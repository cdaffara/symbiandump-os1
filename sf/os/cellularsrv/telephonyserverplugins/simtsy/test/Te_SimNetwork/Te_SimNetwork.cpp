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

#ifndef TE_SimNetwork_H__
	#include "Te_SimNetwork.h"
#endif// TE_SimNetwork_H__

const TInt KOneSecond=1000000;	// Used in a time out function, 1 second (in microSeconds)

//------------------------------------------------------------------

CGetNetworkCapsTests::CGetNetworkCapsTests()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("GetNetworkCapsTests"));
	}
//------------------------------------------------------------------

TVerdict CGetNetworkCapsTests::doTestStepL( void )
	{
//void CTestNetwork::GetNetworkCapsL()
/**
 3.10.1 Get Network Caps
 TInt RMobilePhone::GetNetworkCaps(TUint32& aCaps) const
 This method returns the static network capabilities of the phone.
 The aCaps parameter returned is the sum of TNetworkCaps constants detailed in the table above.
 If the TSY/ME does not support any client access to network functionality then the request
 should be completed with KErrNotSupported.
*/
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
	return TestStepResult();
	}
//------------------------------------------------------------------

CNetworkRegistrationStatusTests::CNetworkRegistrationStatusTests()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NetworkRegistrationStatusTests"));
	}
//------------------------------------------------------------------

TVerdict CNetworkRegistrationStatusTests::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test NetworkRegistrationStatusTests()"));
	GetNetworkRegistrationStatusL() ;
	GetNetworkRegistrationStatusCancelL() ;
	NotifyNetworkRegistrationStatusChangeL() ;
	NotifyNetworkRegistrationStatusChangeCancelL() ;
	return TestStepResult();
	}


void CNetworkRegistrationStatusTests::GetNetworkRegistrationStatusL()
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
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneNetworkInfoV1  bNetworkInfo	= aNetworkInfoBuf();
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus ;

	INFO_PRINTF1(_L("Test GetNetworkRegistrationStatus"));

	//4.4 Get Network Registration Status
	iPhone.GetNetworkRegistrationStatus(iStatus, regStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-002.00")) ;

	PrintRegistration(regStatus);

	validateNetworkInfoL(bNetworkInfo, regStatus);

	if((regStatus == RMobilePhone::ERegisteredOnHomeNetwork) || (regStatus == RMobilePhone::ERegisteredRoaming))
		{
		// Phone seems to provide operator information
		iOperatorInfoSupported = ETrue;
		}
	}


void CNetworkRegistrationStatusTests::GetNetworkRegistrationStatusCancelL()
/**
	3.10.3.1 Cancel Get Network Registration Status
	void RMobilePhone::GetNetworkRegistrationStatusCancel() const
	This method cancels an outstanding asynchronous
	GetNetworkRegistrationStatus request.
 * @test GT83-TNETWORK-002.00 Test retrieval of the phone's network registration status (Generic)
 * @test GT83-TNETWORK-002.08 Test that the Get Network Registration status Request can be cancelled
*/
	{

	RMobilePhone::TMobilePhoneRegistrationStatus  regStatus;
	INFO_PRINTF1(_L("Test GetNetworkRegistrationStatusCancel"));

	iPhone.GetNetworkRegistrationStatus(iStatus, regStatus);
	TEST_CHECKL((iStatus==KRequestPending || iStatus==KErrNone), 1, _L("GT83-TNETWORK-002.00")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneGetNetworkRegistrationStatus) ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone), 1, _L("GT83-TNETWORK-002.08"));
	}


void CNetworkRegistrationStatusTests::NotifyNetworkRegistrationStatusChangeL()
/**
 3.10.4 Notify Network Registration Status Change
 void RMobilePhone::NotifyNetworkRegistrationStatusChange(TRequestStatus& aReqStatus,
 TMobilePhoneRegistrationStatus& aRegistrationStatus) const
 This method allows the client to be notified of changes to the current network
 registration status. When the registration status changes, the TSY will trigger t
 his notification and return the new registration status within the aRegistrationStatus
 parameter. This will take one of the enumerated TMobilePhoneRegistrationStatus values
 detailed in the table in section 3.10.3.
 * @test GT83-TNETWORK-003.01 Test setting a request for notification when there is a change in the phone's network registration status
 * @test GT83-TNETWORK-003.02 Test for a notification when there is a change in the phone's network registration status
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
	TESTL(iStatus==KErrCancel || iStatus==KErrNone);
	}

void CNetworkRegistrationStatusTests::NotifyNetworkRegistrationStatusChangeCancelL()
/**
 3.10.4.1 Cancel Notify Network Registration Status Change
 void RMobilePhone::NotifyNetworkRegistrationStatusChangeCancel() const
 This method cancels an outstanding request to be notified about changes to the current
 network registration status, placed with the NotifyNetworkRegistrationStatusChange method.
 * @test GT83-TNETWORK-003.01 Test setting a request for notification when there is a change in the phone's network registration status
 * @test GT83-TNETWORK-003.03 Test cancelling a request for notification when there is a change in phone's network registration status
*/
	{
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneRegistrationStatus localRegStatus;
	//RMobilePhone::TMobilePhoneNetworkInfoV1  localNetworkInfo;

	INFO_PRINTF1(_L("Test NotifyNetworkRegistrationStatusChangeCancel"));

	iPhone.NotifyNetworkRegistrationStatusChange(iStatus, localRegStatus);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-003.01"));

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-003.03"));
	}


//------------------------------------------------------------------

CModeTests::CModeTests()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("ModeTests"));
	}
//------------------------------------------------------------------

TVerdict CModeTests::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test ModeTests()"));
	GetCurrentModeL() ;
	NotifyModeChangeL() ;
	NotifyModeChangeCancelL() ;
	return TestStepResult();
	}

void CModeTests::PrintNetworkMode(RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode)
	{
		switch(aNetworkMode)
			{
			case RMobilePhone::ENetworkModeUnregistered:
				INFO_PRINTF1(_L(" ME is not registered"));
				break;

			case RMobilePhone::ENetworkModeGsm:
				INFO_PRINTF1(_L(" GSM or DCS1800 network."));
				break;

			case RMobilePhone::ENetworkModeAmps:
				INFO_PRINTF1(_L(" AMPS network."));
				break;

			case RMobilePhone::ENetworkModeCdma95:
				INFO_PRINTF1(_L(" CDMA network 95."));
				break;

			case RMobilePhone::ENetworkModeCdma2000:
				INFO_PRINTF1(_L(" CDMA network 2000."));
				break;

			case RMobilePhone::ENetworkModeWcdma:
				INFO_PRINTF1(_L(" WCDMA network."));
				break;

			case RMobilePhone::ENetworkModeUnknown:
			default:
				INFO_PRINTF1(_L("Network mode is unknown"));
				break;
			}
	}

void  CModeTests::GetCurrentModeL()
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

	}

void CModeTests::NotifyModeChangeL()
/**
3.10.6 Notify Network Mode Change
 void RMobilePhone::NotifyModeChange(TRequestStatus& aReqStatus,
 TMobilePhoneNetworkMode& aNetworkMode)
 This method allows the client to be notified of changes to the current network mode.
 When the mode changes, the TSY will trigger this notification and return the new mode
 within the aNetworkMode parameter. This will take one of the enumerated TMobilePhoneNetworkMode
 values.
 * @test GT83-TNETWORK-005.01 Test setting a request for notification when there is a change in the phone's network mode
 * @test GT83-TNETWORK-005.02 Test for a notification when there is a change in the phone's network mode
 */
	{
	INFO_PRINTF1(_L("Test NotifyModeChange"));

	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNetworkMode aNetworkMode ;

	iPhone.NotifyModeChange(iStatus, aNetworkMode);
	TEST_CHECKL((iStatus==KRequestPending || iStatus==KErrNone), 1, _L("GT83-TNETWORK-005.01")) ;

	User::WaitForRequest(iStatus);

	TESTL(iStatus==KErrNone);

	PrintNetworkMode(aNetworkMode) ;
	//User::WaitForRequest(iStatus);
	//TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-005.02"))
	}

void CModeTests::NotifyModeChangeCancelL()
/**
3.10.6.1 Cancel Notify Network Mode Change
 void RMobilePhone::NotifyModeChangeCancel() const
This method cancels an outstanding asynchronous NotifyModeChange request.
 * @test GT83-TNETWORK-005.01 Test setting a request for notification when there is a change in the phone's network mode
 * @test GT83-TNETWORK-005.03 Test cancelling a request for notification when there is a change in phone's network mode
*/
	{
	INFO_PRINTF1(_L("Test NotifyModeChangeCancel"));

	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNetworkMode aNetworkMode ;

	iPhone.NotifyModeChange(iStatus, aNetworkMode);
	TEST_CHECKL((iStatus==KRequestPending || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-005.01")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyModeChange)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-005.03"));
	}
//------------------------------------------------------------------

CCurrentNetworkTests::CCurrentNetworkTests()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("CurrentNetworkTests"));
	}
//------------------------------------------------------------------

TVerdict CCurrentNetworkTests::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test CurrentNetworkTests()"));
	GetCurrentNetworkL();
	GetCurrentNetworkCancelL() ;
	GetServiceProviderL();
	NotifyCurrentNetworkChangeL() ;
	NotifyCurrentNetworkChangeCancelL() ;
	GetCurrentNetworkNameL();
	return TestStepResult();
	}


void CCurrentNetworkTests::GetCurrentNetworkL()
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
 * @test GT83-TNETWORK-006.01 Test retrieving information about the phone's current network and print the information to screen
 * @test TEL-SIMTSY-NA-1071.01 Test retrieving information about the phone's current network and print the information to screen
*/
	{
	//Test RMobilePhone::GetCurrentNetwork
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test GetCurrentNetwork()"));

	iPhone.GetCurrentNetwork(iStatus, aNetworkInfoBuf, aArea);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-006.01")) ;

	if (iStatus!=KErrNone)
		{
		INFO_PRINTF1(_L("This phone does not support all forms of the Operator Command"));
		INFO_PRINTF1(_L("The information that is supported is listed below"));
		}


	RMobilePhone::TMobilePhoneNetworkInfoV1  bNetworkInfo	= aNetworkInfoBuf();
	PrintNetworkStatus(bNetworkInfo, aArea) ;

	//Test RMobilePhone::GetCurrentNetworkISV
 	RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1 aNetworkInfoISV;
 	RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1Pckg aNetworkInfoISVPckg(aNetworkInfoISV);

	INFO_PRINTF1(_L("Test GetCurrentNetwork() using TEtel3rdPartyMobileNetworkInfoV1"));
	
	iPhone.GetCurrentNetwork(iStatus, aNetworkInfoISVPckg, aArea);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-006.01")) ;
	
	if (iStatus!=KErrNone)
		{
		INFO_PRINTF1(_L("This phone does not support all forms of the Operator Command"));
		INFO_PRINTF1(_L("The information that is supported is listed below"));
		}
		
	// extract the returned Current network info from the package
	aNetworkInfoISV = aNetworkInfoISVPckg();
	PrintNetworkStatusISV(aNetworkInfoISV, aArea);
	
	//test GetCurrentNetwork no location
	INFO_PRINTF1(_L("Test GetCurrentNetwork No Location overload using TMobilePhoneNetworkInfoV2"));
	
	RMobilePhone::TMobilePhoneNetworkInfoV2  aNetworkInfoV2;
	RMobilePhone::TMobilePhoneNetworkInfoV2Pckg aNetworkInfoBufV2(aNetworkInfoV2); // package TMobilePhoneNetworkInfoV2 into a buffer

	INFO_PRINTF1(_L("Test GetCurrentNetwork() (No location overload)"));

	iPhone.GetCurrentNetwork(iStatus, aNetworkInfoBufV2);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-006.03")) ;

	if (iStatus!=KErrNone)
		{
		INFO_PRINTF1(_L("This phone does not support all forms of the Operator Command"));
		INFO_PRINTF1(_L("The information that is supported is listed below"));
		}

	PrintNetworkStatus(aNetworkInfoV2, aArea, EFalse) ;
	
	//test GetCurrentNetwork no location using TMobilePhoneNetworkInfoV8
	INFO_PRINTF1(_L("Test GetCurrentNetwork No Location overload using TMobilePhoneNetworkInfoV8"));
	
	RMobilePhone::TMobilePhoneNetworkInfoV8  networkInfoV8;
	RMobilePhone::TMobilePhoneNetworkInfoV8Pckg networkInfoBufV8(networkInfoV8); // package TMobilePhoneNetworkInfoV8 into a buffer

	INFO_PRINTF1(_L("Test GetCurrentNetwork() (No location overload)"));

	iPhone.GetCurrentNetwork(iStatus, networkInfoBufV8);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("TEL-SIMTSY-NA-1071.01")) ;

	if (iStatus!=KErrNone)
		{
		INFO_PRINTF1(_L("This phone does not support all forms of the Operator Command"));
		INFO_PRINTF1(_L("The information that is supported is listed below"));
		}

	PrintNetworkStatus(networkInfoV8, aArea, EFalse) ;

	}


void CCurrentNetworkTests::GetCurrentNetworkCancelL()
/**
3.10.7.1 Cancel Get Current Network
 void RMobilePhone::GetCurrentNetworkCancel() const
 This method cancels an outstanding asynchronous GetCurrentNetwork request.
 * @test GT83-TNETWORK-006.01 Test retrieving information about the phone's current network and print the information to screen
 * @test GT83-TNETWORK-006.02 Test cancelling a request to retrieve information about the phones current network
 * @test TEL-SIMTSY-NA-1071.01 Test retrieving information about the phone's current network and print the information to screen
 * @test TEL-SIMTSY-NA-1071.02 Test cancelling a request to retrieve information about the phones current network
*/
	{
	//Test RMobilePhone::GetCurrentNetworkCancel()
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test GetCurrentNetworkCancel()"));

	iPhone.GetCurrentNetwork(iStatus, aNetworkInfoBuf, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-006.01")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork);
	User::WaitForRequest(iStatus);

	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone), 1, _L("GT83-TNETWORK-006.02")) ;

	INFO_PRINTF1(_L("Test GetCurrentNetworkISVCancel()"));

	iPhone.GetCurrentNetwork(iStatus, aNetworkInfoBuf);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-006.03")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetworkNoLocation);
	User::WaitForRequest(iStatus);

	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone), 1, _L("GT83-TNETWORK-006.04")) ;

	//Test RMobilePhone::GetCurrentNetworkCancel() for TMobilePhoneNetworkInfoV8 
	RMobilePhone::TMobilePhoneNetworkInfoV8  aNetworkInfoV8;
	RMobilePhone::TMobilePhoneNetworkInfoV8Pckg aNetworkInfoBufV8(aNetworkInfoV8); // package TMobilePhoneNetworkInfoV8 into a buffer

	INFO_PRINTF1(_L("Test GetCurrentNetworkCancel()"));

	iPhone.GetCurrentNetwork(iStatus, aNetworkInfoBufV8, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("TEL-SIMTSY-NA-1071.02"));
	
	iPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork);
	User::WaitForRequest(iStatus); 
	}

void CCurrentNetworkTests::NotifyCurrentNetworkChangeL()
/**
3.10.8 Notify Current Network Change
 void RMobilePhone::NotifyCurrentNetworkChange(TRequestStatus& aReqStatus, TDes8& aNetworkInfo, TMobilePhoneLocationArea& aArea)
 This method allows the client to be notified of changes to the current network.  Upon completion the new network information will be returned in a packaged version in the aNetworkInfo parameter. If the current location of the phone is known then this information can be returned within the aArea parameter whose structure is defined in section 3.10.7
 * @test GT83-TNETWORK-007.01 Test setting a request for notification when there is a change in the phone's current network
 * @test GT83-TNETWORK-007.02 Test for a notification when there is a change in the phone's current network
 * @test TEL-SIMTSY-NA-1074.01 Test for a notification when there is a change in the phone's current network
 * @test TEL-SIMTSY-NA-1074.02 Test for cancelling the  notification when there is a change in the phone's current network
 */
	{
	//Test RMobilePhone::NotifyCurrentNetworkChange()
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test NotifyCurrentNetworkChange()"));

	iPhone.NotifyCurrentNetworkChange(iStatus, aNetworkInfoBuf, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-007.01"));

	//we don't want to wait forever,thus we have to invent something better
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange)  ;
	User::WaitForRequest(iStatus);

	//User::WaitForRequest(iStatus);
	//TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-007.02"))

	//Test RMobilePhone::NotifyCurrentNetworkISVChangeL()
 	RMobilePhone::TMobilePhoneNetworkInfoV2  aNetworkInfoV2;
	RMobilePhone::TMobilePhoneNetworkInfoV2Pckg aNetworkInfoBufV2(aNetworkInfoV2); // package TMobilePhoneNetworkInfoV1 into a buffer


	INFO_PRINTF1(_L("Test NotifyCurrentNetworkChange() - No Location"));

	iPhone.NotifyCurrentNetworkChange(iStatus, aNetworkInfoBufV2);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-007.02"));

	//we don't want to wait forever,thus we have to invent something better
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkNoLocationChange)  ;
	User::WaitForRequest(iStatus);	

	//Test RMobilePhone::NotifyCurrentNetworkChange() for TMobilePhoneNetworkInfoV8
	RMobilePhone::TMobilePhoneNetworkInfoV8  networkInfoV8;
	RMobilePhone::TMobilePhoneNetworkInfoV8Pckg networkInfoBufV8(networkInfoV8); // package TMobilePhoneNetworkInfoV8 into a buffer

	INFO_PRINTF1(_L("Test NotifyCurrentNetworkChange()"));

	iPhone.NotifyCurrentNetworkChange(iStatus, networkInfoBufV8, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("TEL-SIMTSY-NA-1074.01"));

	//we don't want to wait forever,thus we have to invent something better
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange)  ;
	User::WaitForRequest(iStatus);
	}

void CCurrentNetworkTests::NotifyCurrentNetworkChangeCancelL()
/**
3.10.8.1 Cancel Notify Current Network Change
 void RMobilePhone::NotifyCurrentNetworkChangeCancel() const
 This method cancels an outstanding asynchronous NotifyCurrentNetworkChange request.
 * @test GT83-TNETWORK-007.01 Test setting a request for notification when there is a change in the phone's current network
 * @test GT83-TNETWORK-007.03 Test cancelling a request for notification when there is a change in phone's current network
 * @test GT349-TNETWORK-002.01 Test setting a request for notification when there is a change in the phone's current network
 * @test GT349-TNETWORK-002.02 Test cancelling a request for notification when there is a change in phone's current network
*/
	{
	//Test RMobilePhone::NotifyCurrentNetworkChangeCancel()
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test NotifyCurrentNetworkChangeCancel()"));

	iPhone.NotifyCurrentNetworkChange(iStatus, aNetworkInfoBuf, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-007.01")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange)  ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone), 1, _L("GT83-TNETWORK-007.07")) ;

	//Test RMobilePhone::NotifyCurrentNetworkISVChangeCancel()
	INFO_PRINTF1(_L("Test NotifyCurrentNetworkChange Cancel No Location"));
	
	iPhone.NotifyCurrentNetworkChange(iStatus, aNetworkInfoBuf, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TNETWORK-007.01")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkNoLocationChange)  ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone), 1, _L("GT83-TNETWORK-007.08")) ;

	//Test RMobilePhone::NotifyCurrentNetworkChangeCancel() for TMobilePhoneNetworkInfoV8
	RMobilePhone::TMobilePhoneNetworkInfoV8  networkInfoV8;
	RMobilePhone::TMobilePhoneNetworkInfoV8Pckg networkInfoBufV8(networkInfoV8); // package TMobilePhoneNetworkInfoV8 into a buffer

	INFO_PRINTF1(_L("Test NotifyCurrentNetworkChangeCancel()"));

	iPhone.NotifyCurrentNetworkChange(iStatus, networkInfoBufV8, aArea);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("TEL-SIMTSY-NA-1074.02")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange)  ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone), 1, _L("TEL-SIMTSY-NA-1074.02")) ;
	}

void CCurrentNetworkTests::GetServiceProviderL()
	{
	RMobilePhone::TMobilePhoneServiceProviderNameV2 currentSP;
	RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg currentSPPckg=(currentSP);
	
	INFO_PRINTF1(_L("Test GetServiceProviderL()"));
	
	iPhone.GetServiceProviderName(iStatus, currentSPPckg);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT166-GetServiceProviderL-Async"));
		 	
 	INFO_PRINTF2(_L("Display Requirements: %d"), &(currentSP.iDisplayReq));
 	INFO_PRINTF2(_L("Service Provider Name: %S"), &(currentSP.iSPName));
 	INFO_PRINTF2(_L("PLMN field (TLV): %S"), &(currentSP.iPLMNField));		
 	
 	//Test cancel
 	iPhone.GetServiceProviderName(iStatus, currentSPPckg);
 	iPhone.CancelAsyncRequest(EMobilePhoneGetServiceProviderName);
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone),1, _L("GT166-GetServiceProviderL-Async & Cancel")); 		
	}
	
void CCurrentNetworkTests::GetCurrentNetworkNameL()
	{
	RMobilePhone::TMobilePhoneNetworkNameV3 networkname;
	RMobilePhone::TMobilePhoneNetworkNameV3Pckg networknamePckg(networkname);
	
	RMobilePhone::TMobilePhoneOPlmnV3 locInfo;
	RMobilePhone::TMobilePhoneOPlmnV3Pckg locInfoPckg(locInfo);
	
	INFO_PRINTF1(_L("Test GetCurrentNetworkNameL()"));
	
	iPhone.GetCurrentNetworkName(iStatus, networknamePckg, locInfoPckg);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT166-GetCurrentNetworkNameL-Async"));
	
	INFO_PRINTF2(_L("Long Name: %S"),&(networkname.iLongName));
	INFO_PRINTF2(_L("Short Name: %S"),&(networkname.iShortName));
	INFO_PRINTF2(_L("Other Name: %S"),&(networkname.iOtherNames));
	
	INFO_PRINTF2(_L("Country Code: %S"), &(locInfo.iCountryCode));
	INFO_PRINTF2(_L("First Location Area Code: %d"), locInfo.iFirstLocationAreaCode);
	INFO_PRINTF2(_L("Last Location Area Code: %d"), locInfo.iLastLocationAreaCode);
	INFO_PRINTF2(_L("Network Id: %S"), &(locInfo.iNetworkId));
	INFO_PRINTF2(_L("PNN id: %d"), locInfo.iPNNid);	
	
	//Test cancel
 	iPhone.GetCurrentNetworkName(iStatus, networknamePckg, locInfoPckg);
 	iPhone.CancelAsyncRequest(EMobilePhoneGetNetworkName);
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone),1, _L("GT166-GetCurrentNetworkNameL-Async & Cancel")); 		
	}

//------------------------------------------------------------------

CHomeNeworkTests::CHomeNeworkTests()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("HomeNeworkTests"));
	}
//------------------------------------------------------------------

TVerdict CHomeNeworkTests::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Start HomeNeworkTests()"));
	GetHomeNetworkL() ;
	GetHomeNetworkCancelL() ;
	return TestStepResult();
	}

void CHomeNeworkTests::GetHomeNetworkL()
/**
3.10.9 Get Home Network
 void RMobilePhone::GetHomeNetwork(TRequestStatus& aReqStatus, TDes8& aNetworkInfo) const
 This method provides the client with information on the home network of this subscriber. This allows the client to compare detected and/or current network information with the home network information in order to determine if the current or detected networks are also the home network.
 This home network information is returned in the aNetworkInfo parameter. The packaged version of TMobilePhoneNetworkInfoV1 will be returned. The elements of this structure are described in section 3.10.2.
* @test TEL-SIMTSY-NA-1072.01 Test retrieving information about the phone's home network and print the information to screen
*/
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer
	RMobilePhone::TMobilePhoneLocationAreaV1 aArea;

	INFO_PRINTF1(_L("Test GetHomeNetwork()"));

	iPhone.GetHomeNetwork(iStatus, aNetworkInfoBuf) ;

	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-000.00"));

	RMobilePhone::TMobilePhoneNetworkInfoV1  bNetworkInfo	= aNetworkInfoBuf();
	PrintNetworkStatus(bNetworkInfo, aArea) ;
	
	//Test RMobilePhone::GetHomeNetwork for TMobilePhoneNetworkInfoV8
	RMobilePhone::TMobilePhoneNetworkInfoV8  networkInfoV8;
	RMobilePhone::TMobilePhoneNetworkInfoV8Pckg networkInfoV8Buf(networkInfoV8); // package TMobilePhoneNetworkInfoV1 into a buffer

	INFO_PRINTF1(_L("Test GetHomeNetwork()"));

	iPhone.GetHomeNetwork(iStatus, networkInfoV8Buf) ;

	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("TEL-SIMTSY-NA-1072.01"));

	RMobilePhone::TMobilePhoneNetworkInfoV8  bNetworkInfoV8	= networkInfoV8Buf();
	PrintNetworkStatus(bNetworkInfoV8, aArea) ;
	}

void CHomeNeworkTests::GetHomeNetworkCancelL()
/**
3.10.9.1 Cancel Get Home Network
  void RMobilePhone::GetHomeNetworkCancel() const
 This method cancels a previously placed asynchronous GetHomeNetwork request.
* @test TEL-SIMTSY-NA-1072.01 Test retrieving information about the phone's home network and print the information to screen
* @test TEL-SIMTSY-NA-1072.02 Test cancelling a request to retrieve information about the phones home network
*/
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1  aNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg aNetworkInfoBuf(aNetworkInfo); // package TMobilePhoneNetworkInfoV1 into a buffer

	INFO_PRINTF1(_L("Test GetHomeNetworkCancel()"));

	iPhone.GetHomeNetwork(iStatus, aNetworkInfoBuf) ;
	TEST_CHECKL((iStatus==KRequestPending || iStatus==KErrNone), 1, _L("GT83-TNETWORK-000.00")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneGetHomeNetwork)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00"));
	
	//Test RMobilePhone::GetHomeNetwork for TMobilePhoneNetworkInfoV8
	RMobilePhone::TMobilePhoneNetworkInfoV8  networkInfoV8;
	RMobilePhone::TMobilePhoneNetworkInfoV8Pckg networkInfoV8Buf(networkInfoV8); // package TMobilePhoneNetworkInfoV1 into a buffer

	INFO_PRINTF1(_L("Test GetHomeNetworkCancel()"));

	iPhone.GetHomeNetwork(iStatus, networkInfoV8Buf) ;
	TEST_CHECKL((iStatus==KRequestPending || iStatus==KErrNone), 1, _L("GT349-TNETWORK-xx3.01")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneGetHomeNetwork)  ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT349-TNETWORK-xx3.02"));
	}
//------------------------------------------------------------------

CNetworkSelectionSettingTests::CNetworkSelectionSettingTests()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NetworkSelectionSettingTests"));
	}
//------------------------------------------------------------------

TVerdict CNetworkSelectionSettingTests::doTestStepL( void )
/** 
*	This area of the API allows clients to specify how the phone will select a
*	network - either automatically based on an algorithm and preferred choices or
*	manually after detection of available networks.
*/
	{
	INFO_PRINTF1(_L("Test Network Selection Settings Tests"));

	GetNetworkSelectionSettingL() ;
	SetNetworkSelectionSettingL() ;
	SetNetworkSelectionSettingCancelL() ;
	NotifyNetworkSelectionSettingChangeL() ;
	NotifyNetworkSelectionSettingChangeCancelL() ;

	return TestStepResult();
	}

void CNetworkSelectionSettingTests::PrintSelectionSettings(RMobilePhone::TMobilePhoneNetworkSelectionV1& aSetting)
/**
*	@note 11.09.2001 TMobilePhoneNetworkSelectionV1V1 has not been implemended yet, so I have to use
*	TMobilePhoneNetworkSelectionV1
*/
	{
		switch(aSetting.iMethod)
			{
			case RMobilePhone::ENetworkSelectionUnknown:
				INFO_PRINTF1(_L("A network selection  method is ENetworkSelectionUnknown."));
				break;
			case RMobilePhone::ENetworkSelectionAutomatic:
				INFO_PRINTF1(_L("A network selection  method is ENetworkSelectionAutomatic."));
				break;
			case RMobilePhone::ENetworkSelectionManual:
				INFO_PRINTF1(_L("A network selection method is ENetworkSelectionManual."));
				break;
			case RMobilePhone::ENetworkSelectionHomeOnly:
				INFO_PRINTF1(_L("A network selection method is ENetworkSelectionHomeOnly."));
				break;
			}


		switch(aSetting.iBandClass)
			{
			case RMobilePhone::ENetworkBandClassUnknown:
				INFO_PRINTF1(_L("A network band class is ENetworkBandClassUnknown."));
				break;
			case RMobilePhone::ENetworkBandClassAOnly:
				INFO_PRINTF1(_L("A network band class is ENetworkBandClassAOnly."));
				break;
			case RMobilePhone::ENetworkBandClassBOnly:
				INFO_PRINTF1(_L("A network band class is ENetworkBandClassBOnly."));
				break;
			case RMobilePhone::ENetworkBandClassAPreferred:
				INFO_PRINTF1(_L("A network band class is ENetworkBandClassAPreferred."));
				break;
			case RMobilePhone::ENetworkBandClassBPreferred:
				INFO_PRINTF1(_L("A network band class is ENetworkBandClassBPreferred."));
				break;
			}

		switch(aSetting.iOperationMode)
			{
			case RMobilePhone::ENetworkOperationUnknown:
				INFO_PRINTF1(_L("A network phone operation is ENetworkOperationUnknown."));
				break;
			case RMobilePhone::ENetworkOperationAnalogOnly:
				INFO_PRINTF1(_L("A network phone operation is ENetworkOperationAnalogOnly."));
				break;
			case RMobilePhone::ENetworkOperationDigitalOnly:
				INFO_PRINTF1(_L("A network phone operation is ENetworkOperationDigitalOnly."));
				break;
			case RMobilePhone::ENetworkOperationAnalogPreferred:
				INFO_PRINTF1(_L("A network phone operation is ENetworkOperationAnalogPreferred."));
				break;
			case RMobilePhone::ENetworkOperationDigitalPreferred:
				INFO_PRINTF1(_L("A network phone operation is ENetworkOperationDigitalPreferred."));
				break;
			}

	}

void CNetworkSelectionSettingTests::GetNetworkSelectionSettingL()
/**
*	3.10.11 Get Network Selection Setting
*	TInt RMobilePhone::GetNetworkSelectionSetting(TDes8& aSetting) const
*	This method returns the current value of the network selection setting.
*	The aSetting parameter returned is a packaged version of the
*	TMobilePhoneNetworkSelectionV1V1 class above.
*	It is a synchronous method because it is assumed that the TSY will
*	have previously retrieved and locally stored this system setting.
*	@note 11.09.2001 TMobilePhoneNetworkSelectionV1V1 has not been implemended yet, so I have to use
*	TMobilePhoneNetworkSelectionV1
*/
	{


	INFO_PRINTF1(_L("Test GetNetworkSelectionSetting()"));

	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;
	TInt ret = iPhone.GetNetworkSelectionSetting(localNetworkSelBuf) ;
	TEST_CHECKL(ret, KErrNotSupported, _L("GT83-TNETWORK-000.00")) ;

	PrintSelectionSettings(localNetworkSel) ;
	}

void CNetworkSelectionSettingTests::SetNetworkSelectionSettingL()
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
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;

	INFO_PRINTF1(_L("Test SetNetworkSelectionSetting()"));

	iPhone.GetNetworkSelectionSetting(localNetworkSelBuf) ;

	iPhone.SetNetworkSelectionSetting(iStatus, localNetworkSelBuf) ;
	TEST_CHECKL((iStatus == KRequestPending || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus == KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;
	}

void CNetworkSelectionSettingTests::SetNetworkSelectionSettingCancelL()
/**
	3.10.12.1 Cancel Set Network Selection Setting
	void RMobilePhone::SetNetworkSelectionSettingCancel() const
	This method cancels an outstanding asynchronous SetNetworkSelectionSetting request

 	@note 11.09.2001 TMobilePhoneNetworkSelectionV1V1 has not been implemended yet, so I have to use
	TMobilePhoneNetworkSelectionV1
 */
	{
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;


	INFO_PRINTF1(_L("Test SetNetworkSelectionSettingCancel()"));

	iPhone.GetNetworkSelectionSetting(localNetworkSelBuf) ;

	iPhone.SetNetworkSelectionSetting(iStatus, localNetworkSelBuf) ;
	TEST_CHECKL((iStatus == KRequestPending || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;
	iPhone.CancelAsyncRequest(EMobilePhoneSetNetworkSelectionSetting)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;
	}

void CNetworkSelectionSettingTests::NotifyNetworkSelectionSettingChangeL()
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
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;

	INFO_PRINTF1(_L("Test NotifyNetworkSelectionSettingChange()"));

	iPhone.NotifyNetworkSelectionSettingChange(iStatus, localNetworkSelBuf) ;
	TEST_CHECKL((iStatus == KRequestPending || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;
	//!!!!!!!!!!!!!!!!!!!!!!!!! Stub only
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSelectionSettingChange)  ;
	User::WaitForRequest(iStatus);
	//TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TNETWORK-000.00"));
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TESTL(iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported);
	}

void CNetworkSelectionSettingTests::NotifyNetworkSelectionSettingChangeCancelL()
/**
	3.10.13.1 Notify Network Selection Setting Change Cancel
	void RMobilePhone::NotifyNetworkSelectionSettingChangeCancel() const
	This method cancels an outstanding asynchronous
	NotifyNetworkSelectionSettingChange request.
*/

	{
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNetworkSelectionV1  localNetworkSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg localNetworkSelBuf(localNetworkSel) ;

	iPhone.NotifyNetworkSelectionSettingChange(iStatus, localNetworkSelBuf) ;
	TEST_CHECKL((iStatus == KRequestPending || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSelectionSettingChange)  ;
	User::WaitForRequest(iStatus);
	//@note 13.09.2001 function is not supported yet  is  the reason of iStatus==KErrNotSupported
	TEST_CHECKL((iStatus==KErrCancel || iStatus==KErrNone || iStatus==KErrNotSupported), 1, _L("GT83-TNETWORK-000.00")) ;
	}
//------------------------------------------------------------------

CNetworkTimeDateTests::CNetworkTimeDateTests()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NetworkTimeDateTests"));
	}
//------------------------------------------------------------------

TVerdict CNetworkTimeDateTests::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test NetworkTimeDateTestsL()"));

	GetNITZInfoL() ;
	NotifyNITZInfoChangeL() ;
	NotifyNITZInfoChangeCancelL() ;

	return TestStepResult();
	}

void  CNetworkTimeDateTests::GetNITZInfoL()
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
	TESTL(!(ret == KErrNotFound));
	TESTL(ret == KErrNone);
// DEF053798 - received values are compared against hard coded expecting values
	TESTL(aNITZInfo.Year() == 2001); // in test_config.txt
	TESTL(aNITZInfo.Month() == EOctober);  // month's range is 0 - 11 
	TESTL(aNITZInfo.Day() == 15);
	TESTL(aNITZInfo.Hour() == 10);
	TESTL(aNITZInfo.Minute() == 15);
	TESTL(aNITZInfo.Second() == 0);
	TESTL(aNITZInfo.MicroSecond() == 0);
	TESTL(aNITZInfo.iTimeZone == 1234);
	TESTL(aNITZInfo.iDST == 5678); 
	}

void CNetworkTimeDateTests::NotifyNITZInfoChangeL()
/**
	3.10.15.3 Notify NITZ Information Change
	void RMobilePhone::NotifyNITZInfoChange(TRequestStatus& aReqStatus,
		TMobilePhoneNITZ& aNITZInfo) const
	This method allows the client to be notified of changes to the network time
	and date info. Upon completion the new network time information will be returned
	in the aNITZInfo parameter.
*/
	{
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNITZ aNITZInfo ;


	INFO_PRINTF1(_L("Test NotifyNITZInfoChange()"));

// DEF053798 
	iPhone.NotifyNITZInfoChange(iStatus, aNITZInfo) ;
	TESTL(iStatus == KRequestPending);	
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus == KErrNone), 1, _L("GT83-TNETWORK-000.00")) ;
// received values are compared against hard coded expecting values
	TESTL(aNITZInfo.Year() == 2002);
	TESTL(aNITZInfo.Month() == ENovember);  
	TESTL(aNITZInfo.Day() == 16);
	TESTL(aNITZInfo.Hour() == 11);
	TESTL(aNITZInfo.Minute() == 16);
	TESTL(aNITZInfo.Second() == 1);
	TESTL(aNITZInfo.MicroSecond() == 1);
	TESTL(aNITZInfo.iTimeZone == 9012);
	TESTL(aNITZInfo.iDST == 3456); 

	}

void CNetworkTimeDateTests::NotifyNITZInfoChangeCancelL()
/**
3.10.15.3.1 Notify NITZ Information Change Cancel
 void RMobilePhone::NotifyNITZInfoChangeCancel() const
This method cancels an outstanding asynchronous NotifyNITZInfoChange request.
*/
	{
	//TRequestStatus aStatus;
	RMobilePhone::TMobilePhoneNITZ aNITZInfo ;


	INFO_PRINTF1(_L("Test NotifyNITZInfoChangeCancel()"));

// DEF053798 
	iPhone.NotifyNITZInfoChange(iStatus, aNITZInfo) ;
	TEST_CHECKL((iStatus == KRequestPending), 1, _L("GT83-TNETWORK-000.00")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNITZInfoChange)  ;
	User::WaitForRequest(iStatus);
	TESTL(iStatus == KErrNone || iStatus == KErrCancel);
	if (iStatus == KErrCancel)
		{
		INFO_PRINTF1(_L("CNetworkTimeDateTests::NotifyNITZInfoChangeCancelL (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("CNetworkTimeDateTests::NotifyNITZInfoChangeCancelL (async & cancel (Request Not Cancelled)) passed"));
		}

	}
//------------------------------------------------------------------

CNetworkCellInfoTests::CNetworkCellInfoTests()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("NetworkCellInfoTests"));
	}
//------------------------------------------------------------------

TVerdict CNetworkCellInfoTests::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test NetworkCellInfoTestsL()"));

	GetCellInfoL() ;
	NotifyCellInfoChangeL() ;
	GetCellInfoCancelL() ;
	NotifyCellInfoChangeCancelL() ;

	return TestStepResult();
	}

void  CNetworkCellInfoTests::GetCellInfoL()
/**
	Get cell Information
	void RMobilePhone::GetCellInfo(TRequestStatus& aReqStatus, TDes8& aCellInfo) const
	This method returns the current cell information
	in the aCellInfo parameter.  It is a asynchronous method.
*/
	{
	RMobilePhone::TMobilePhoneCellInfoV9 aCellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg aCellInfoPckg(aCellInfo);
	INFO_PRINTF1(_L("Test GetCellInfo()"));

	iPhone.GetCellInfo(iStatus, aCellInfoPckg);
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus == KErrNone), 1, _L("PREQ965.1")) ;
	TESTL(aCellInfo.iMode == RMobilePhone::ECellModeGsm);
	TESTL(aCellInfo.iCellId == 1515);
	TESTL(aCellInfo.iTimingAdvance == 111);
	TESTL(aCellInfo.iStatus == RMobilePhone::ECellInfoStatusUnknown);
	}

void CNetworkCellInfoTests::NotifyCellInfoChangeL()
/**
	Notify Cell Information Change
	void RMobilePhone::NotifyCellInfoChange(TRequestStatus& aReqStatus, TDes8& aCellInfo) const;
	This method allows the client to be notified of changes to the cell  info. Upon completion 
	the new cell information will be returned in the aCellInfo parameter.
*/
	{
	RMobilePhone::TMobilePhoneCellInfoV9 aCellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg aCellInfoPckg(aCellInfo);


	INFO_PRINTF1(_L("Test NotifyCellInfoChange()"));

	iPhone.NotifyCellInfoChange(iStatus, aCellInfoPckg) ;
	TESTL(iStatus == KRequestPending);	
	User::WaitForRequest(iStatus);
	TEST_CHECKL((iStatus == KErrNone), 1, _L("PREQ965.1")) ;
// received values are compared against hard coded expecting values
	TESTL(aCellInfo.iMode == RMobilePhone::ECellModeWcdma);
	TESTL(aCellInfo.iCellId == 327);
	TESTL(aCellInfo.iTimingAdvance == 71);
	TESTL(aCellInfo.iStatus == RMobilePhone::ECellInfoStatusCurrent);

	}

void CNetworkCellInfoTests::GetCellInfoCancelL()
/**
Get Cell Info Cancel
 void RTelSubSessionBase::CancelAsyncRequest(TInt aReqToCancel) const
This method cancels an outstanding asynchronous GetCellInfo request.
*/
	{
	RMobilePhone::TMobilePhoneCellInfoV9 aCellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg aCellInfoPckg(aCellInfo);


	INFO_PRINTF1(_L("Test GetCellInfoCancel()"));

	iPhone.GetCellInfo(iStatus, aCellInfoPckg) ;
	TEST_CHECKL((iStatus == KRequestPending), 1, _L("PREQ965.1")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneGetCellInfo)  ;
	User::WaitForRequest(iStatus);
	TESTL(iStatus == KErrNone || iStatus == KErrCancel);
	if (iStatus == KErrCancel)
		{
		INFO_PRINTF1(_L("CNetworkCellInfoTests::GetCellInfoCancel (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("CNetworkCellInfoTests::GetCellInfoCancel (async & cancel (Request Not Cancelled)) passed"));
		}

	}


void CNetworkCellInfoTests::NotifyCellInfoChangeCancelL()
/**
Notify Cell Information Change Cancel
 void RTelSubSessionBase::CancelAsyncRequest(TInt aReqToCancel) const
This method cancels an outstanding asynchronous NotifyCellInfoChange request.
*/
	{
	RMobilePhone::TMobilePhoneCellInfoV9 aCellInfo;
	RMobilePhone::TMobilePhoneCellInfoV9Pckg aCellInfoPckg(aCellInfo);


	INFO_PRINTF1(_L("Test NotifyCellInfoChangeCancel()"));

	iPhone.NotifyCellInfoChange(iStatus, aCellInfoPckg) ;
	TEST_CHECKL((iStatus == KRequestPending), 1, _L("PREQ965.1")) ;

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCellInfoChange)  ;
	User::WaitForRequest(iStatus);
	TESTL(iStatus == KErrNone || iStatus == KErrCancel);
	if (iStatus == KErrCancel)
		{
		INFO_PRINTF1(_L("CNetworkCellInfoTests::NotifyCellInfoChangeCancelL (async & cancel (Cancelled Request)) passed"));
		}
	else
		{
		INFO_PRINTF1(_L("CNetworkCellInfoTests::NotifyCellInfoChangeCancelL (async & cancel (Request Not Cancelled)) passed"));
		}

	}
//------------------------------------------------------------------

CDetectedNetworkTest::CDetectedNetworkTest()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("DetectedNetworkTest"));
	}
//------------------------------------------------------------------

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
	CTestGetDetectedNetworks* retrieveMobileNets;
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
	CMobilePhoneNetworkList *networkList = NULL;

	//create and put to the CleanUp stack
	retrieveMobileNets = CTestGetDetectedNetworks::NewLC(iPhone);


	INFO_PRINTF1(_L("Test Detected Networks"));

	INFO_PRINTF1(_L("This test can take up to a minute to complete..."));

	// Does phone provide operator information?
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
		return TestStepResult(); 
		}
	RMobilePhone::TMobilePhoneNetworkInfoV1 network;
	TRAPD(r,retrieveMobileNets->Start());

	TRAP(r, CActiveScheduler::Start());
	if(r == KErrNone && retrieveMobileNets->iStatus == KErrNone)
		{

		TRAP(r, (networkList = retrieveMobileNets->RetrieveList()));
		if((r == KErrNone) && (NULL != networkList))
			{
			INFO_PRINTF2(_L("CRetrieveMobilePhoneDetectedNetworks::Start() reports %d networks"), networkList->Enumerate());
			TInt count = networkList->Enumerate();


			for (TInt i=0; i<count; i++)
				{

				network = networkList->GetEntryL(i);

				INFO_PRINTF6(_L("%d = >%S< >%S< country >%S< network >%S< status : "), i,
					&network.iLongName, &network.iShortName,
					&network.iCountryCode, &network.iNetworkId);


				switch(network.iStatus)
					{
					case RMobilePhone::ENetworkStatusUnknown:
						INFO_PRINTF1(_L("Status is unknown "));
					break;

					case RMobilePhone::ENetworkStatusAvailable:
						INFO_PRINTF1(_L("A network that the Mobile Equipment is allowed to register to "));
					break;

					case RMobilePhone::ENetworkStatusCurrent:
						INFO_PRINTF1(_L("The currently registered network "));
					break;

					case RMobilePhone::ENetworkStatusForbidden:
						INFO_PRINTF1(_L("A network that the Mobile Equipment is not allowed to register to "));
					break;

					}

				}
			}
		}
	else if(retrieveMobileNets->iStatus != KErrNone)
		{
		ERR_PRINTF2(_L("CTestGetDetectedNetworks() request reports %d error"), retrieveMobileNets->iStatus.Int());
		}
	CleanupStack::PopAndDestroy(); // retrieveMobileNets;
	delete networkList;
	User::After(5*KOneSecond);		// Give user time to see the test results
	return TestStepResult();
	}
//------------------------------------------------------------------
CServerInfo::CServerInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("ServerInfo"));
	}
//------------------------------------------------------------------

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
	tsyName = KSimtsyName;
	ret=iTelServer.IsSupportedByModule(tsyName, mixin, result);

	if(ret==KErrNone)
		{
		INFO_PRINTF1(_L("Querying TSY about supported functionality is Successful"));
		}
	return TestStepResult();
	}

/**************************************************************/
//
// Testing asynchronous retrieve of detected network list
//
/**************************************************************/

CTestGetDetectedNetworks* CTestGetDetectedNetworks::NewLC(RMobilePhone& aPhone)
	{
    CTestGetDetectedNetworks* r=new(ELeave) CTestGetDetectedNetworks(aPhone);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }

CTestGetDetectedNetworks::CTestGetDetectedNetworks(RMobilePhone& aPhone)
    : CActive(EPriorityNormal), iPhone(aPhone)
	{
	}


void CTestGetDetectedNetworks::ConstructL()
/**
	3.10.10.1 Create an instance of CRetrieveMobilePhoneDetectedNetworks
	CRetrieveMobilePhoneDetectedNetworks* CRetrieveMobilePhoneDetectedNetworks::
			NewL(RMobilePhone& aPhone)
	This method creates an instance of the CActive derived
	CRetrieveMobilePhoneDetectedNetworks class.  The client must supply a reference
	to their RMobilePhone sub-session as a parameter to the NewL as it is
	this sub-session that the list retrieval class will operate on.
*/
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
    }

CTestGetDetectedNetworks::~CTestGetDetectedNetworks()
	{
	delete iRetrieve;
	}

void CTestGetDetectedNetworks::Start()
/**
	3.10.10.2 Start the asynchronous list retrieval
	void CRetrieveMobilePhoneDetectedNetworks::Start(TRequestStatus& aReqStatus)
	This method starts the two-phase list retrieval. The client supplies
	its TRequestStatus as usual for asynchronous requests and then waits
	for it to be completed.
*/
    {
    iRetrieve->Start(iStatus);
    SetActive();
    }

CMobilePhoneNetworkList *CTestGetDetectedNetworks::RetrieveList()
/**
	3.10.10.3 Retrieve the list after completion
	CMobilePhoneNetworkList* CRetrieveMobilePhoneDetectedNetworks::RetrieveListL() const
	This method provides the client with a handle to the list class retrieved
	from the phone. The client will call this method once it's TRequestStatus
	passed into Start() has completed.
	When the client calls this method the ownership of the CMobilePhoneNetworkList
	object will transfer to the client, so the client is then responsible for eventual
	deletion of this object.
	This method will leave with KErrNotFound if there is no valid list object to
	return to the client.
*/
	{
	CMobilePhoneNetworkList* list = NULL ;
	if(iRetrieve)
		{
		TRAP_IGNORE(iRetrieve->RetrieveListL());
		}
	return(list);
	}

void CTestGetDetectedNetworks::RunL()
    {
	//TInt ret=iStatus.Int();
   //User::LeaveIfError(ret);
    CActiveScheduler::Stop();
	}


void CTestGetDetectedNetworks::DoCancel()
    {
    iRetrieve->Cancel();
    }

