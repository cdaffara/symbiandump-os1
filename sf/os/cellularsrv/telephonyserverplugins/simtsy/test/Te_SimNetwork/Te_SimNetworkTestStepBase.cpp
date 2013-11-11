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

#include "Te_SimNetworkTestStepBase.h"

TVerdict CSimNetworkTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 0);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed resetting"));
		User::Leave(ret);
		}

	iTestScheduler=new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iTestScheduler);	

	ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		delete iTestScheduler;
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to initialise the phone"));
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	RPhone::TCaps phoneCaps;
	ret = iPhone.GetCaps(phoneCaps) ;
	if ((KErrNone != ret) || !(phoneCaps.iFlags & RPhone::KCapsVoice))
		{
		INFO_PRINTF2(_L("This phone does not support Voice Calls (caps=0x%x)"), phoneCaps.iFlags);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		delete iTestScheduler;
		User::Leave(ret);
		}

	iOrgProcessPriority = RProcess().Priority();
	iOrgThreadPriority  = RThread().Priority();

	RProcess().SetPriority(EPriorityHigh);
	RThread().SetPriority(EPriorityMuchMore);

	return TestStepResult();
	}

TVerdict CSimNetworkTestStepBase::doTestStepPostambleL()
	{
	RProcess().SetPriority(iOrgProcessPriority);
	RThread().SetPriority(iOrgThreadPriority);

	iPhone.Close();
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();
	delete iTestScheduler;

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CSimNetworkTestStepBase::validateNetworkInfoL(RMobilePhone::TMobilePhoneNetworkInfoV1& aNetwork, RMobilePhone::TMobilePhoneRegistrationStatus& aStatus)
/**
 * This function validates Network Info. and phone's registration Status. .
 */
	{
	if (aStatus == RMobilePhone::ENotRegisteredNoService)
		{
		TESTL(aNetwork.iShortName.Length()==0);		// To be confirmed
		TESTL(aNetwork.iLongName.Length()==0);
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
			TESTL(aStatus==RMobilePhone::ERegisteredOnHomeNetwork ||
				 aStatus==RMobilePhone::ERegisteredRoaming ||
				 aStatus==RMobilePhone::ERegistrationUnknown); // Motorola TPort workaround (does not support Reg. info)
		}
	}

void CSimNetworkTestStepBase::PrintRegistration(RMobilePhone::TMobilePhoneRegistrationStatus&  aStatus)
/**
 * This function displays the Phone's Registration Status.
 * @param Mobile Phone Registration Status
 */
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

void CSimNetworkTestStepBase::PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV1&  aNetworkInfo,
							   RMobilePhone::TMobilePhoneLocationAreaV1& aArea, TBool aPrintArea)
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

	switch(aNetworkInfo.iMode)
		{
		case RMobilePhone::ENetworkModeUnknown:
			INFO_PRINTF1(_L("The network mode is unknown."));
			break;

		case RMobilePhone::ENetworkModeUnregistered:
			INFO_PRINTF1(_L("the network mode is unregistered."));
			break;

		case RMobilePhone::ENetworkModeGsm:
			INFO_PRINTF1(_L("the network mode is GSM."));
			break;

		case RMobilePhone::ENetworkModeAmps:
			INFO_PRINTF1(_L("the network mode is AMPS."));
			break;
			
		case RMobilePhone::ENetworkModeCdma95:
			INFO_PRINTF1(_L("the network mode is CDMA 95."));
			break;

		case RMobilePhone::ENetworkModeCdma2000:
			INFO_PRINTF1(_L("the network mode is CDMA 2000."));
			break;
			
		case RMobilePhone::ENetworkModeWcdma:
			INFO_PRINTF1(_L("the network mode is WCDMA."));
			break;
			
		case RMobilePhone::ENetworkModeTdcdma:
			INFO_PRINTF1(_L("the network mode is TDCDMA."));
			break;
			
		}
	// Prints the MCC (Mobile Country Code) and the MNC (Mobile Network Code)
	//???INFO_PRINTF1(_L("\tNetwork Numbers    = country >%S< network >%S<"), &bNetworkInfo.iCountryCode, &bNetworkInfo.iNetworkId);

	// TMobilePhoneLocationAreaV1
	if(aPrintArea)
		{
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
	}
void CSimNetworkTestStepBase::PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV2&  aNetworkInfo,
							   RMobilePhone::TMobilePhoneLocationAreaV1& aArea, TBool aPrintArea)
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

	switch(aNetworkInfo.iMode)
		{
		case RMobilePhone::ENetworkModeUnknown:
			INFO_PRINTF1(_L("The network mode is unknown."));
			break;

		case RMobilePhone::ENetworkModeUnregistered:
			INFO_PRINTF1(_L("the network mode is unregistered."));
			break;

		case RMobilePhone::ENetworkModeGsm:
			INFO_PRINTF1(_L("the network mode is GSM."));
			break;

		case RMobilePhone::ENetworkModeAmps:
			INFO_PRINTF1(_L("the network mode is AMPS."));
			break;
			
		case RMobilePhone::ENetworkModeCdma95:
			INFO_PRINTF1(_L("the network mode is CDMA 95."));
			break;

		case RMobilePhone::ENetworkModeCdma2000:
			INFO_PRINTF1(_L("the network mode is CDMA 2000."));
			break;
			
		case RMobilePhone::ENetworkModeWcdma:
			INFO_PRINTF1(_L("the network mode is WCDMA."));
			break;
			
		case RMobilePhone::ENetworkModeTdcdma:
			INFO_PRINTF1(_L("the network mode is TDCDMA."));
			break;
			
		}
	// Prints the MCC (Mobile Country Code) and the MNC (Mobile Network Code)
	//???INFO_PRINTF1(_L("\tNetwork Numbers    = country >%S< network >%S<"), &bNetworkInfo.iCountryCode, &bNetworkInfo.iNetworkId);

	// TMobilePhoneLocationAreaV1
	if(aPrintArea)
		{
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
	INFO_PRINTF2(_L("\tType of Network Access  = >%d<"), aNetworkInfo.iAccess);
	}
	
void CSimNetworkTestStepBase::PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV5&  aNetworkInfo,
							   RMobilePhone::TMobilePhoneLocationAreaV1& aArea, TBool aPrintArea)
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

	switch(aNetworkInfo.iMode)
		{
		case RMobilePhone::ENetworkModeUnknown:
			INFO_PRINTF1(_L("The network mode is unknown."));
			break;

		case RMobilePhone::ENetworkModeUnregistered:
			INFO_PRINTF1(_L("the network mode is unregistered."));
			break;

		case RMobilePhone::ENetworkModeGsm:
			INFO_PRINTF1(_L("the network mode is GSM."));
			break;

		case RMobilePhone::ENetworkModeAmps:
			INFO_PRINTF1(_L("the network mode is AMPS."));
			break;
			
		case RMobilePhone::ENetworkModeCdma95:
			INFO_PRINTF1(_L("the network mode is CDMA 95."));
			break;

		case RMobilePhone::ENetworkModeCdma2000:
			INFO_PRINTF1(_L("the network mode is CDMA 2000."));
			break;
			
		case RMobilePhone::ENetworkModeWcdma:
			INFO_PRINTF1(_L("the network mode is WCDMA."));
			break;
			
		case RMobilePhone::ENetworkModeTdcdma:
			INFO_PRINTF1(_L("the network mode is TDCDMA."));
			break;
			
		}

	// Prints the MCC (Mobile Country Code) and the MNC (Mobile Network Code)
	//???INFO_PRINTF1(_L("\tNetwork Numbers    = country >%S< network >%S<"), &bNetworkInfo.iCountryCode, &bNetworkInfo.iNetworkId);

	// TMobilePhoneLocationAreaV1
	if(aPrintArea)
		{
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
	INFO_PRINTF2(_L("\tType of Network Access  = >%d<"), aNetworkInfo.iAccess);
	INFO_PRINTF2(_L("\tAvailability of Hsdpa  = >%d<"), aNetworkInfo.iHsdpaAvailableIndicator);
	INFO_PRINTF2(_L("\tAvailability of Egprs = >%d<"), aNetworkInfo.iEgprsAvailableIndicator);
	}	
void CSimNetworkTestStepBase::PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV8&  aNetworkInfo,
							   RMobilePhone::TMobilePhoneLocationAreaV1& aArea, TBool aPrintArea)
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

	switch(aNetworkInfo.iMode)
		{
		case RMobilePhone::ENetworkModeUnknown:
			INFO_PRINTF1(_L("The network mode is unknown."));
			break;

		case RMobilePhone::ENetworkModeUnregistered:
			INFO_PRINTF1(_L("the network mode is unregistered."));
			break;

		case RMobilePhone::ENetworkModeGsm:
			INFO_PRINTF1(_L("the network mode is GSM."));
			break;

		case RMobilePhone::ENetworkModeAmps:
			INFO_PRINTF1(_L("the network mode is AMPS."));
			break;
			
		case RMobilePhone::ENetworkModeCdma95:
			INFO_PRINTF1(_L("the network mode is CDMA 95."));
			break;

		case RMobilePhone::ENetworkModeCdma2000:
			INFO_PRINTF1(_L("the network mode is CDMA 2000."));
			break;
			
		case RMobilePhone::ENetworkModeWcdma:
			INFO_PRINTF1(_L("the network mode is WCDMA."));
			break;
			
		case RMobilePhone::ENetworkModeTdcdma:
			INFO_PRINTF1(_L("the network mode is TDCDMA."));
			break;
			
		}
	// Prints the MCC (Mobile Country Code) and the MNC (Mobile Network Code)
	//???INFO_PRINTF1(_L("\tNetwork Numbers    = country >%S< network >%S<"), &bNetworkInfo.iCountryCode, &bNetworkInfo.iNetworkId);

	// TMobilePhoneLocationAreaV1
	if(aPrintArea)
		{
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
	INFO_PRINTF2(_L("\tType of Network Access  = >%d<"), aNetworkInfo.iAccess);
	INFO_PRINTF2(_L("\tAvailability of Hsdpa  = >%d<"), aNetworkInfo.iHsdpaAvailableIndicator);
	INFO_PRINTF2(_L("\tAvailability of Egprs = >%d<"), aNetworkInfo.iEgprsAvailableIndicator);
	INFO_PRINTF2(_L("\tAvailability of Hsupa = >%d<"), aNetworkInfo.iHsupaAvailableIndicator);
	}
void CSimNetworkTestStepBase::PrintNetworkStatusISV(RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1&  aNetworkInfo,
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

	switch(aNetworkInfo.iMode)
		{
		case RMobilePhone::ENetworkModeUnknown:
			INFO_PRINTF1(_L("The network mode is unknown."));
			break;

		case RMobilePhone::ENetworkModeUnregistered:
			INFO_PRINTF1(_L("the network mode is unregistered."));
			break;

		case RMobilePhone::ENetworkModeGsm:
			INFO_PRINTF1(_L("the network mode is GSM."));
			break;

		case RMobilePhone::ENetworkModeAmps:
			INFO_PRINTF1(_L("the network mode is AMPS."));
			break;
			
		case RMobilePhone::ENetworkModeCdma95:
			INFO_PRINTF1(_L("the network mode is CDMA 95."));
			break;

		case RMobilePhone::ENetworkModeCdma2000:
			INFO_PRINTF1(_L("the network mode is CDMA 2000."));
			break;
			
		case RMobilePhone::ENetworkModeWcdma:
			INFO_PRINTF1(_L("the network mode is WCDMA."));
			break;
			
		case RMobilePhone::ENetworkModeTdcdma:
			INFO_PRINTF1(_L("the network mode is TDCDMA."));
			break;
			
		}

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
