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

#include "Te_SimGetSubscriberIdTest.h"

CSimGetSubscriberIdTest::CSimGetSubscriberIdTest() 
	{ 
	SetTestStepName(_L("GetSubscriberIdTest"));
	}

TVerdict CSimGetSubscriberIdTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginGetSubscriberIdTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPhone.Initialise()==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	INFO_PRINTF1(_L("Test Retrieve Subscriber ID"));
	TRequestStatus stat0;
	RMobilePhone::TMobilePhoneSubscriberId subscriberId;
	iPhone.GetSubscriberId(stat0,subscriberId);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(subscriberId==KTestSubscriberId);
	CHECKPOINTL(stat0, KErrNone, CHP_NTWK_CASE("D.9"));

	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

