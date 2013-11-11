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

#include "Te_SimIndicatorTest.h"

CSimIndicatorTest::CSimIndicatorTest() 
	{ 
	SetTestStepName(_L("IndicatorTest"));
	}

void CSimIndicatorTest::PrintIndicatorValues(TInt aIndicator)
	{
	switch (aIndicator)
		{
	case 0:
		INFO_PRINTF1(_L("Indicator: Nothing indicated"));
		break;
	case 1:
		INFO_PRINTF1(_L("Indicator: Charger connected"));
		break;
	case 2:
		INFO_PRINTF1(_L("Indicator: Network Available"));
		break;
	case 3:
		INFO_PRINTF1(_L("Indicator: Network available & Charger connected"));
		break;
	case 4:
		INFO_PRINTF1(_L("Indicator: Call in progress"));
		break;
	case 5:
		INFO_PRINTF1(_L("Indicator: Call in progress & Charger connected"));
		break;
	case 6:
		INFO_PRINTF1(_L("Indicator: Call in progress & Network available"));
		break;
	case 7:
		INFO_PRINTF1(_L("Indicator: Call in progress, Network available & Charger connected"));
		break;
	default:
		break;
		}
	}

TVerdict CSimIndicatorTest::doTestStepL()
	{
	INFO_PRINTF1(_L("RIndicators Functionality"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iPhone.Initialise()==KErrNone);
	
	TUint32 indic, actionCaps, indCaps;
	TRequestStatus stat;

	INFO_PRINTF1(_L("Getting the indicators Caps"));
	iPhone.GetIndicatorCaps(actionCaps, indCaps);
	TESTL(actionCaps==TInt(KIndicatorActionCaps));
	TESTL(indCaps==TInt(KIndicatorIndCaps));

	INFO_PRINTF1(_L("Getting the indicators"));
	iPhone.GetIndicator(stat, indic);
	User::WaitForRequest(stat);

	PrintIndicatorValues(indic);
	TESTL(indic==0); // Initial values

	INFO_PRINTF1(_L("Testing indicators notification..."));

	// First trigger will be network available
	iPhone.NotifyIndicatorChange(stat, indic);
	User::WaitForRequest(stat);
	PrintIndicatorValues(indic);
	TESTL(indic==2);

	// Second trigger will be network available and charger connected
	iPhone.NotifyIndicatorChange(stat, indic);
	User::WaitForRequest(stat);
	PrintIndicatorValues(indic);
	TESTL(indic==3);

	// Third trigger will be charger connected
	iPhone.NotifyIndicatorChange(stat, indic);
	User::WaitForRequest(stat);
	PrintIndicatorValues(indic);
	TESTL(indic==1);

	// Fourth trigger will be network available and charger connected
	iPhone.NotifyIndicatorChange(stat, indic);
	User::WaitForRequest(stat);
	PrintIndicatorValues(indic);
	TESTL(indic==3);

	// Fifth trigger will be network available
	iPhone.NotifyIndicatorChange(stat, indic);
	User::WaitForRequest(stat);
	PrintIndicatorValues(indic);
	TESTL(indic==2);

	//Dial a voice call
	TESTL(iLine.Open(iPhone,KVoiceLineName)==KErrNone);
	INFO_PRINTF1(_L("Opened Line object"));

	TName callName2;
	TESTL(iCall.OpenNewCall(iLine,callName2)==KErrNone);
	INFO_PRINTF1(_L("Opened Call object"));

	TRequestStatus reqStatus0, reqStatus1;
	RMobileCall::TMobileCallStatus callStatus;
	iCall.NotifyMobileCallStatusChange(reqStatus0,callStatus);

	INFO_PRINTF1(_L("Testing again indicators notification"));
	iPhone.NotifyIndicatorChange(stat, indic);

	INFO_PRINTF2(_L("Dialling %S "),&KTelephoneNumber);
	iCall.Dial(reqStatus1,KTelephoneNumber);

	User::WaitForRequest(reqStatus0);		// Wait for the Call Status Change notification
	TESTL(reqStatus0==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusDialling);
	INFO_PRINTF1(_L("...Call State is Dialling"));

	User::WaitForRequest(stat);
	PrintIndicatorValues(indic);
//	TESTL(indic==7);	//battery charger status, network status and call in progress

	User::WaitForRequest(reqStatus1);
	iCall.Close();
	iLine.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

