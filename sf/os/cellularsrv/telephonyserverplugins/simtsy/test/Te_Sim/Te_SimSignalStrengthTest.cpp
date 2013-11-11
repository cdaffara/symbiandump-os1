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

#include "Te_SimSignalStrengthTest.h"

CSimSignalStrengthTest::CSimSignalStrengthTest() 
	{ 
	SetTestStepName(_L("SignalStrengthTest"));
	}

TVerdict CSimSignalStrengthTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginSignalStrengthTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPhone.Initialise()==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	INFO_PRINTF1(_L("Testing Signal Strength Caps"));
	TUint32 caps;
	TESTL(iPhone.GetSignalCaps(caps)==KErrNone);
	TESTL(caps==  (RMobilePhone::KCapsGetSignalStrength
				 | RMobilePhone::KCapsNotifySignalStrengthChange));

	INFO_PRINTF1(_L("Testing Get Current Signal Strength"));
	TRequestStatus stat0;
	TInt32 signalStrength;
	TInt8 barLevel;
	iPhone.GetSignalStrength(stat0,signalStrength,barLevel);
	iPhone.CancelAsyncRequest(EMobilePhoneGetSignalStrength);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(signalStrength==-20);
	TESTL(barLevel==3);

	INFO_PRINTF1(_L("Testing Signal Strength Notification"));
	iPhone.NotifySignalStrengthChange(stat0,signalStrength,barLevel);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(signalStrength==-30);
	TESTL(barLevel==2);
	CHECKPOINTL(stat0, KErrNone, CHP_NTWK_CASE("D.8"));

	INFO_PRINTF1(_L("Testing Signal Strength Caps"));
	TESTL(iPhone.GetSignalCaps(caps)==KErrNone);
	TESTL(caps==  (RMobilePhone::KCapsGetSignalStrength
				 | RMobilePhone::KCapsNotifySignalStrengthChange));

	INFO_PRINTF1(_L("Testing Signal Strength Notification Cancel"));
	iPhone.NotifySignalStrengthChange(stat0,signalStrength,barLevel);
	User::After(1000000L);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrCancel);

	User::After(2000000L);
	INFO_PRINTF1(_L("Testing a final Get Current Signal Strength"));
	iPhone.GetSignalStrength(stat0,signalStrength,barLevel);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(signalStrength==-40);
	TESTL(barLevel==1);
	CHECKPOINTL(stat0, KErrNone, CHP_NTWK_CASE("D.7"));

	INFO_PRINTF1(_L("Testing Signal Strength Caps again"));
	TESTL(iPhone.GetSignalCaps(caps)==KErrNone);
	TESTL(caps == RMobilePhone::KCapsGetSignalStrength);

	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

