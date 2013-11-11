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

#include "Te_SimPhBkSecurityCapsTest.h"

CSimPhBkSecurityCapsTest::CSimPhBkSecurityCapsTest() 
	{ 
	SetTestStepName(_L("SecurityCapsTest"));
	}

TVerdict CSimPhBkSecurityCapsTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPhBkSecurityCapsTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TRequestStatus aReqStatus;

	TUint32 secCaps;
	TUint32 expectedSecCaps = RMobilePhone::KCapsLockPhone | \
							RMobilePhone::KCapsLockICC | \
							RMobilePhone::KCapsAccessPin1 | \
							RMobilePhone::KCapsAccessPin2 | \
							RMobilePhone::KCapsAccessPhonePassword | \
							RMobilePhone::KCapsAccessSPC | \
							RMobilePhone::KCapsAccessHiddenKey | \
							RMobilePhone::KCapsAccessUSIMAppPin | \
							RMobilePhone::KCapsAccessUSIMAppSecondPin | \
							RMobilePhone::KCapsAccessUniversalPin;  

	INFO_PRINTF1(_L("GetSecurityCaps test"));
	ret=iPhone.GetSecurityCaps(secCaps);
	CHECKPOINTL(ret, KErrNone, CHP_APPS_CASE("H.1"));
	TESTL(secCaps == expectedSecCaps);

	INFO_PRINTF1(_L("Notify Security Caps change"));
	iPhone.NotifySecurityCapsChange(aReqStatus,secCaps);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNotSupported);
	
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

