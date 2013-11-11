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

#include "Te_SimNetworkStatusTest.h"

CSimNetworkStatusTest::CSimNetworkStatusTest() 
	{ 
	SetTestStepName(_L("NetworkStatusTest"));
	}

TVerdict CSimNetworkStatusTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginNetworkStatusTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	_LIT(KCountryCode0,"ABCD");
	_LIT(KNetworkId0,"Network0");
	_LIT(KDisplayTag0,"Vodafone");
	_LIT(KShortName0,"Voda");
	_LIT(KLongName0,"Vodafone_UK");
	_LIT(KCountryCode3,"MNOP");
	_LIT(KNetworkId3,"Network3");
	_LIT(KDisplayTag3,"Orange");
	_LIT(KShortName3,"Orgn");
	_LIT(KLongName3,"Orange_UK");
	TInt egprsIndicator=0;
	TInt hsdpaIndicator=0;
	TInt hsupaIndicator=1;	
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPhone.Initialise()==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	INFO_PRINTF1(_L("Testing Network Caps"));
	TUint32 caps;
	TESTL(iPhone.GetNetworkCaps(caps)==KErrNone);
	TESTL(caps==(RMobilePhone::KCapsGetRegistrationStatus |
				 RMobilePhone::KCapsNotifyRegistrationStatus | 
				 RMobilePhone::KCapsGetCurrentMode |
				 RMobilePhone::KCapsGetCurrentNetwork | 
				 RMobilePhone::KCapsNotifyCurrentNetwork |
				 RMobilePhone::KCapsGetHomeNetwork));

	INFO_PRINTF1(_L("Testing Home Network"));
	TRequestStatus stat0,stat2;
	RMobilePhone::TMobilePhoneNetworkInfoV1 netInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg netInfoPckg(netInfo);
	iPhone.GetHomeNetwork(stat0,netInfoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(netInfo.iMode==RMobilePhone::ENetworkModeGsm);
	TESTL(netInfo.iStatus==RMobilePhone::ENetworkStatusUnknown);
	TESTL(netInfo.iBandInfo==RMobilePhone::EBandUnknown); 
	TESTL(netInfo.iCountryCode==KCountryCode0);
	TESTL(netInfo.iAnalogSID.Length()==0);
	TESTL(netInfo.iNetworkId==KNetworkId0);
	TESTL(netInfo.iDisplayTag==KDisplayTag0);
	TESTL(netInfo.iShortName==KShortName0);
	TESTL(netInfo.iLongName==KLongName0);

	INFO_PRINTF1(_L("Testing Get Current Network"));
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	iPhone.GetCurrentNetwork(stat0,netInfoPckg,area);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNotFound);
	CHECKPOINTL(stat0,KErrNotFound,CHP_NTWK_CASE("D.3"));
	
	INFO_PRINTF1(_L("Testing Get Current Registration Status"));
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
	iPhone.GetNetworkRegistrationStatus(stat0,regStatus);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(regStatus==RMobilePhone::ENotRegisteredSearching);
	CHECKPOINTL(stat0,KErrNone,CHP_NTWK_CASE("D.1"));

	INFO_PRINTF1(_L("Testing Registration Status Change Notification"));
	iPhone.NotifyNetworkRegistrationStatusChange(stat0,regStatus);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(regStatus==RMobilePhone::ERegisteredOnHomeNetwork);
	CHECKPOINTL(stat0,KErrNone,CHP_NTWK_CASE("D.2"));

	INFO_PRINTF1(_L("Testing Network Information Change Notification"));
	iPhone.NotifyCurrentNetworkChange(stat0,netInfoPckg,area);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(netInfo.iMode==RMobilePhone::ENetworkModeGsm);
	TESTL(netInfo.iStatus==RMobilePhone::ENetworkStatusAvailable);
//	TESTL(netInfo.iBandInfo==RMobilePhone::EBandUnknown);
	TESTL(netInfo.iCountryCode==KCountryCode3);
	TESTL(netInfo.iAnalogSID.Length()==0);
	TESTL(netInfo.iNetworkId==KNetworkId3);
	TESTL(netInfo.iDisplayTag==KDisplayTag3);
	TESTL(netInfo.iShortName==KShortName3);
	TESTL(netInfo.iLongName==KLongName3);
	CHECKPOINTL(stat0,KErrNone,CHP_NTWK_CASE("D.4"));
	
    //Testing the new TMobilePhoneNetworkInfoV8 class
 	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8NoLoc;
 	RMobilePhone::TMobilePhoneNetworkInfoV8Pckg networkInfoV8NoLocPckg(networkInfoV8NoLoc);
	TRequestStatus stat1;
	
	INFO_PRINTF1(_L("Testing Network Information Change Notification (No Location Method)"));
	iPhone.NotifyCurrentNetworkChange(stat1,networkInfoV8NoLocPckg);
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);
	TESTL(networkInfoV8NoLoc.iMode==RMobilePhone::ENetworkModeGsm);
	TESTL(networkInfoV8NoLoc.iStatus==RMobilePhone::ENetworkStatusUnknown);
	TESTL(networkInfoV8NoLoc.iBandInfo==RMobilePhone::EBandUnknown); 
	TESTL(networkInfoV8NoLoc.iCountryCode==KCountryCode0);
	TESTL(networkInfoV8NoLoc.iNetworkId==KNetworkId0);
	TESTL(networkInfoV8NoLoc.iDisplayTag==KDisplayTag0);
	TESTL(networkInfoV8NoLoc.iShortName==KShortName0);
	TESTL(networkInfoV8NoLoc.iLongName==KLongName0);
	TESTL(networkInfoV8NoLoc.iAccess == RMobilePhone::ENetworkAccessGsm);
	TESTL(networkInfoV8NoLoc.iHsdpaAvailableIndicator == TBool(hsdpaIndicator));
	TESTL(networkInfoV8NoLoc.iEgprsAvailableIndicator == TBool(egprsIndicator));
 
	//Call NotifyCurrentNetworkChange() to get the cached value from the previous network change
	iPhone.NotifyCurrentNetworkChange(stat0,netInfoPckg,area);	
	User::WaitForRequest(stat0);

	INFO_PRINTF1(_L("Testing Network Information Change Notification (EtelMM + ISV Method)"));	
	iPhone.NotifyCurrentNetworkChange(stat0,netInfoPckg,area);
	iPhone.NotifyCurrentNetworkChange(stat1,networkInfoV8NoLocPckg);
	User::WaitForRequest(stat0);
	User::WaitForRequest(stat1);
	TESTL((stat0==KErrNone) && (stat1==KErrNone));
	
	TESTL(netInfo.iMode==RMobilePhone::ENetworkModeGsm);
	TESTL(netInfo.iStatus==RMobilePhone::ENetworkStatusAvailable);
	TESTL(netInfo.iBandInfo==RMobilePhone::EBandUnknown);
	TESTL(netInfo.iCountryCode==KCountryCode3);
	TESTL(netInfo.iAnalogSID.Length()==0);
	TESTL(netInfo.iNetworkId==KNetworkId3);
	TESTL(netInfo.iDisplayTag==KDisplayTag3);
	TESTL(netInfo.iShortName==KShortName3);
	TESTL(netInfo.iLongName==KLongName3);
	
	TESTL(networkInfoV8NoLoc.iMode==RMobilePhone::ENetworkModeGsm);
	TESTL(networkInfoV8NoLoc.iStatus==RMobilePhone::ENetworkStatusAvailable);
	TESTL(networkInfoV8NoLoc.iBandInfo==RMobilePhone::EBandUnknown); 
	TESTL(networkInfoV8NoLoc.iCountryCode==KCountryCode3);
	TESTL(networkInfoV8NoLoc.iNetworkId==KNetworkId3);
	TESTL(networkInfoV8NoLoc.iDisplayTag==KDisplayTag3);
	TESTL(networkInfoV8NoLoc.iShortName==KShortName3);
	TESTL(networkInfoV8NoLoc.iLongName==KLongName3);
	TESTL(networkInfoV8NoLoc.iAccess == RMobilePhone::ENetworkAccessGsm);
	TESTL(networkInfoV8NoLoc.iHsdpaAvailableIndicator == TBool(hsdpaIndicator));
	TESTL(networkInfoV8NoLoc.iEgprsAvailableIndicator == TBool(egprsIndicator));
	
	//Testing the RMobilePhone::GetHomeNetwork() and RMobilePhone::GetCurrentNetwork APIs for the new TMobilePhoneNetworkInfoV8 class
	
	INFO_PRINTF1(_L("Testing Home Network for TMobilePhoneNetworkInfoV8 class"));
	iPhone.GetHomeNetwork(stat0,networkInfoV8NoLocPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(networkInfoV8NoLoc.iMode==RMobilePhone::ENetworkModeGsm);
	TESTL(networkInfoV8NoLoc.iStatus==RMobilePhone::ENetworkStatusUnknown);
	TESTL(networkInfoV8NoLoc.iBandInfo==RMobilePhone::EBandUnknown); 
	TESTL(networkInfoV8NoLoc.iCountryCode==KCountryCode0);
	TESTL(networkInfoV8NoLoc.iAnalogSID.Length()==0);
	TESTL(networkInfoV8NoLoc.iNetworkId==KNetworkId0);
	TESTL(networkInfoV8NoLoc.iDisplayTag==KDisplayTag0);
	TESTL(networkInfoV8NoLoc.iAccess == RMobilePhone::ENetworkAccessGsm);
	TESTL(networkInfoV8NoLoc.iHsdpaAvailableIndicator == EFalse);
	TESTL(networkInfoV8NoLoc.iEgprsAvailableIndicator == EFalse);
	TESTL(networkInfoV8NoLoc.iShortName==KShortName0);
	TESTL(networkInfoV8NoLoc.iLongName==KLongName0);
	TESTL(networkInfoV8NoLoc.iHsupaAvailableIndicator==TBool(hsupaIndicator));
	
	INFO_PRINTF1(_L("Testing GetCurrentNetwork for TMobilePhoneNetworkInfoV8 class"));
	iPhone.GetCurrentNetwork(stat0,networkInfoV8NoLocPckg);
	User::WaitForRequest(stat0);
	TESTL(stat1==KErrNone);
	TESTL(networkInfoV8NoLoc.iMode==RMobilePhone::ENetworkModeGsm);
	TESTL(networkInfoV8NoLoc.iStatus==RMobilePhone::ENetworkStatusAvailable);
	TESTL(networkInfoV8NoLoc.iBandInfo==RMobilePhone::EBandUnknown);
	TESTL(networkInfoV8NoLoc.iCountryCode==KCountryCode3);
	TESTL(networkInfoV8NoLoc.iAnalogSID.Length()==0);
	TESTL(networkInfoV8NoLoc.iNetworkId==KNetworkId3);
	TESTL(networkInfoV8NoLoc.iDisplayTag==KDisplayTag3);
	TESTL(networkInfoV8NoLoc.iAccess == RMobilePhone::ENetworkAccessGsm);
	TESTL(networkInfoV8NoLoc.iHsdpaAvailableIndicator == EFalse);
	TESTL(networkInfoV8NoLoc.iEgprsAvailableIndicator == EFalse);
	TESTL(networkInfoV8NoLoc.iHsupaAvailableIndicator==TBool(hsupaIndicator));	
	
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

