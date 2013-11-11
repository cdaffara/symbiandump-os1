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

#include "Te_SimBatteryChargerTest.h"

CSimBatteryChargerTest::CSimBatteryChargerTest() 
	{ 
	SetTestStepName(_L("BatteryChargerTest"));
	}

TVerdict CSimBatteryChargerTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BatteryCharger Functionality"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iPhone.Initialise()==KErrNone);
	
	TUint32 chargerCaps;
	TRequestStatus stat;
	RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfoV1;

	INFO_PRINTF1(_L("Getting the battery charger Caps"));
	iPhone.GetBatteryCaps(chargerCaps);
	TESTL(chargerCaps==TInt(KBatteryChargerCaps));

	INFO_PRINTF1(_L("Getting the battery charger info..."));
	iPhone.GetBatteryInfo(stat, batteryInfoV1);
	User::WaitForRequest(stat);
	TESTL(batteryInfoV1.iStatus==0);
	TESTL(batteryInfoV1.iChargeLevel ==0);
	
	INFO_PRINTF1(_L("Testing battery charger notification"));
	iPhone.NotifyBatteryInfoChange(stat, batteryInfoV1);
	User::WaitForRequest(stat);
	TESTL(batteryInfoV1.iStatus==1);
	TESTL(batteryInfoV1.iChargeLevel ==3);

	INFO_PRINTF1(_L("Testing again battery charger notification"));
	iPhone.NotifyBatteryInfoChange(stat, batteryInfoV1);
	User::WaitForRequest(stat);
	TESTL(batteryInfoV1.iStatus==2);
	TESTL(batteryInfoV1.iChargeLevel ==5);

	INFO_PRINTF1(_L("Testing again battery charger notification"));
	iPhone.NotifyBatteryInfoChange(stat, batteryInfoV1);
	User::WaitForRequest(stat);
	TESTL(batteryInfoV1.iStatus==1);
	TESTL(batteryInfoV1.iChargeLevel ==3);

	INFO_PRINTF1(_L("Testing a final Get Current battery charger info "));
	iPhone.GetBatteryInfo(stat, batteryInfoV1);
	User::WaitForRequest(stat);
	TESTL(batteryInfoV1.iStatus==1);
	TESTL(batteryInfoV1.iChargeLevel ==3);

	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

