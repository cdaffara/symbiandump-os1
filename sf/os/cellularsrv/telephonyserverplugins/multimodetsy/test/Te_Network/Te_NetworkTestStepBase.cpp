// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_NetworkTestStepBase.h"

TVerdict CNetworkTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KMmtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}
	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KMmtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}
//
//	This test requires a mobile phone to be connected to the serial port
//      specified in the .cfg file for the initialise below to work
//
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KMmtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	// Change this process and thread to a higher priority so that we can be in control
	// when performing tests such as monitoring async request status values.
	//
	iOrgProcessPriority = RProcess().Priority();
	iOrgThreadPriority  = RThread().Priority();

	RProcess().SetPriority(EPriorityHigh);
	RThread().SetPriority(EPriorityMuchMore);

	return TestStepResult();
	}

TVerdict CNetworkTestStep::doTestStepPostambleL()
	{
	//
	// Restore the process and thread priority...
	//
	RProcess().SetPriority(iOrgProcessPriority);
	RThread().SetPriority(iOrgThreadPriority);

	iPhone.Close();
	iTelServer.UnloadPhoneModule(KMmtsyName);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CNetworkTestStep::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
	{

	TRequestStatus timerStatus;
	RTimer timer ;
	timer.CreateLocal() ;
	timer.After(timerStatus,aNumberOfMicroSeconds);

	User::WaitForRequest(aStatus, timerStatus);
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	
	else
		{
		INFO_PRINTF1(_L("Time is over!!!")) ;
		}

	timer.Close() ;
	}


//This function validates Network Info. and phone's registration Status. .
void CNetworkTestStep::validateNetworkInfo(RMobilePhone::TMobilePhoneNetworkInfoV1& aNetwork, RMobilePhone::TMobilePhoneRegistrationStatus& aStatus)
	{

	if (aStatus == RMobilePhone::ENotRegisteredNoService)
		{
		TEST(aNetwork.iShortName.Length()==0);		// To be confirmed
		TEST(aNetwork.iLongName.Length()==0);
		}

	/* ????????????????????
	if ((aStatus == RMobilePhone::ERegisteredOnHomeNetwork) || (aStatus == RMobilePhone::ERegisteredRoaming))
		if (aNetwork.iNetworkId.Length() != 0)
			TESTL(aNetwork.iCountryCode.Length()!=0);		// if we know the cell, surely we know the country?
	if (aNetwork.iNetworkId.Length() == 0)
		return;
	*/

	// NB. Can't infer aNetwork.iStatus from aStatus because some phones can't tell us the operator
	if (aNetwork.iStatus == RMobilePhone::ENetworkStatusCurrent)
		{
		TEST(aStatus==RMobilePhone::ERegisteredOnHomeNetwork ||
			  aStatus==RMobilePhone::ERegisteredRoaming ||
			  aStatus==RMobilePhone::ERegistrationUnknown); // Motorola TPort workaround (does not support Reg. info)
		}
	}

// This function displays the Phone's Registration Status.
void CNetworkTestStep::PrintRegistration(RMobilePhone::TMobilePhoneRegistrationStatus&  aStatus)
	{

	switch (aStatus)
		{
		case RMobilePhone::ERegistrationUnknown:
			INFO_PRINTF1(_L("Registration status is unknown"));
		break;

		case RMobilePhone::ENotRegisteredNoService:
			INFO_PRINTF1(_L("Not registered. The ME can not detect any other networks and is not currently searching a new operator to register to"));
		break;

		case RMobilePhone::ENotRegisteredEmergencyOnly:
			INFO_PRINTF1(_L("Not registered. The ME can detect other networks on which it is possible to make emergency calls only"));
		break;

		case RMobilePhone::ENotRegisteredSearching:
			INFO_PRINTF1(_L("Not registered, but the ME is currently searching a new operator to register to"));
		break;

		case RMobilePhone::ERegisteredOnHomeNetwork:
			INFO_PRINTF1(_L("Registered on home network."));
		break;

		case RMobilePhone::ERegistrationDenied:
			INFO_PRINTF1(_L("Registration denied."));
		break;

		case RMobilePhone::ERegisteredRoaming:
			INFO_PRINTF1(_L("Registered, roaming"));
		break;

        case RMobilePhone::ERegisteredBusy:
			INFO_PRINTF1(_L("Registered, busy"));
        break;
		}
	}

//This function displays the Network Status.
void CNetworkTestStep::PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV1&  aNetworkInfo,
							   RMobilePhone::TMobilePhoneLocationAreaV1& aArea)
	{
	// Prints the status part ...
	INFO_PRINTF1(_L("The Status of the Network : "));
	INFO_PRINTF1(_L("----------------------------"));


	switch(aNetworkInfo.iStatus)
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
	INFO_PRINTF2(_L("\tNetwork Long name  = >%S<"), &aNetworkInfo.iLongName);
	INFO_PRINTF2(_L("\tNetwork Short name = >%S<"), &aNetworkInfo.iShortName);

	// Prints the MCC (Mobile Country Code) and the MNC (Mobile Network Code)
	//???INFO_PRINTF1(_L("\tNetwork Numbers    = country >%S< network >%S<"), &bNetworkInfo.iCountryCode, &bNetworkInfo.iNetworkId);

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
	}

void CNetworkTestStep::PrintNetworkMode(RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode)

/**
3.10.5 Get Current Network Mode
TInt RMobilePhone::Get(TMobilePhoneNetworkMode& aNetworkMode) const

This method returns information about the current network mode. The aNetworkMode
parameter returned is one of the enumerated TMobilePhoneNetworkMode values detailed .
* @test GT83-TNETWORK-004.00 Test retrieval of the phone's current mode (Generic)
*/
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
			INFO_PRINTF1(_L("Network mode is unknown "));
		break;
		}
	}

void CNetworkTestStep::PrintSelectionSettings(RMobilePhone::TMobilePhoneNetworkSelectionV1& aSetting)
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

