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

#include "Te_SimNitzTest.h"

CSimNitzTest::CSimNitzTest() 
	{ 
	SetTestStepName(_L("NitzTest"));
	}

TVerdict CSimNitzTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginNitzTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	const TInt32 KNitzFieldsUsed0=	(RMobilePhone::KCapsTimeAvailable
								    |RMobilePhone::KCapsTimezoneAvailable
									|RMobilePhone::KCapsDSTAvailable);
	const TInt KYear0=			2001;
	const TMonth KMonth0=		EOctober;
	const TInt KDay0=			15;
	const TInt KHour0=			10;
	const TInt KMinute0=		15;
	const TInt KSecond0=		0;
	const TInt KMicroSecond0=	0;
	const TInt KTimeZone0=		1234;
	const TInt KDST0=			5678;
	_LIT(KShortNetworkId0,"");
	_LIT(KLongNetworkId0,"");

	const TInt32 KNitzFieldsUsed1=	(RMobilePhone::KCapsTimeAvailable
								    |RMobilePhone::KCapsTimezoneAvailable
									|RMobilePhone::KCapsDSTAvailable
									|RMobilePhone::KCapsShortNameAvailable
									|RMobilePhone::KCapsLongNameAvailable);
	const TInt KYear1=			2002;
	const TMonth KMonth1=		ENovember;
	const TInt KDay1=			16;
	const TInt KHour1=			11;
	const TInt KMinute1=		16;
	const TInt KSecond1=		1;
	const TInt KMicroSecond1=	1;
	const TInt KTimeZone1=		9012;
	const TInt KDST1=			3456;
	_LIT(KShortNetworkId1,"Voda");
	_LIT(KLongNetworkId1,"Vodafone_UK");

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPhone.Initialise()==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	INFO_PRINTF1(_L("Test Retrieve current NITZ information"));
	RMobilePhone::TMobilePhoneNITZ nitzInfo;
	TESTL(iPhone.GetNITZInfo(nitzInfo)==KErrNone);
	TESTL(nitzInfo.iNitzFieldsUsed==KNitzFieldsUsed0);
	TESTL(nitzInfo.Year()==KYear0);
	TESTL(nitzInfo.Month()==KMonth0);
	TESTL(nitzInfo.Day()==KDay0);
	TESTL(nitzInfo.Hour()==KHour0);
	TESTL(nitzInfo.Minute()==KMinute0);
	TESTL(nitzInfo.Second()==KSecond0);
	TESTL(nitzInfo.MicroSecond()==KMicroSecond0);
	TESTL(nitzInfo.iTimeZone==KTimeZone0);
	TESTL(nitzInfo.iDST==KDST0);
	TESTL(nitzInfo.iShortNetworkId==KShortNetworkId0);
	TESTL(nitzInfo.iLongNetworkId==KLongNetworkId0);

	INFO_PRINTF1(_L("Test Notification of NITZ information changing"));
	TRequestStatus stat0;
	iPhone.NotifyNITZInfoChange(stat0,nitzInfo);
	User::WaitForRequest(stat0);
	TESTL(nitzInfo.iNitzFieldsUsed==KNitzFieldsUsed1);
	TESTL(nitzInfo.Year()==KYear1);
	TESTL(nitzInfo.Month()==KMonth1);
	TESTL(nitzInfo.Day()==KDay1);
	TESTL(nitzInfo.Hour()==KHour1);
	TESTL(nitzInfo.Minute()==KMinute1);
	TESTL(nitzInfo.Second()==KSecond1);
	TESTL(nitzInfo.MicroSecond()==KMicroSecond1);
	TESTL(nitzInfo.iTimeZone==KTimeZone1);
	TESTL(nitzInfo.iDST==KDST1);
	TESTL(nitzInfo.iShortNetworkId==KShortNetworkId1);
	TESTL(nitzInfo.iLongNetworkId==KLongNetworkId1);

	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

