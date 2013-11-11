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

#include "Te_SimPhBkLockInfoTest.h"

CSimPhBkLockInfoTest::CSimPhBkLockInfoTest() 
	{ 
	SetTestStepName(_L("LockInfoTest"));
	}

TVerdict CSimPhBkLockInfoTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPhBkLockInfoTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TRequestStatus aReqStatus;
	RMobilePhone::TMobilePhoneLockInfoV1 asyncLockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg asyncLockInfoPckg(asyncLockInfo);

	INFO_PRINTF1(_L("ELockICC"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockICC,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNone);
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusUnlocked);
	TESTL(asyncLockInfo.iSetting == RMobilePhone::ELockSetDisabled);

	INFO_PRINTF1(_L("ELockPin2"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockPin2,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNone);
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusLockUnknown);
	TESTL(asyncLockInfo.iSetting == RMobilePhone::ELockSetUnknown);

	INFO_PRINTF1(_L("ELockHiddenKey"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockHiddenKey,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNone);
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusLocked);
	TESTL(asyncLockInfo.iSetting == RMobilePhone::ELockSetEnabled);

	INFO_PRINTF1(_L("ELockUSimApp"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockUSimApp,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNone);
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusLockUnknown);
	TESTL(asyncLockInfo.iSetting == RMobilePhone::ELockSetUnknown);

	INFO_PRINTF1(_L("ELockSecondUSimApp"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockSecondUSimApp,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNone);
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusLockUnknown);
	TESTL(asyncLockInfo.iSetting == RMobilePhone::ELockSetUnknown);

	INFO_PRINTF1(_L("ELockUniversalPin"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockUniversalPin,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNone);
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusLockUnknown);
	TESTL(asyncLockInfo.iSetting == RMobilePhone::ELockSetUnknown);

	INFO_PRINTF1(_L("ELockPhoneDevice"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockPhoneDevice,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNotSupported);

	INFO_PRINTF1(_L("ELockPhoneToICC"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockPhoneToICC,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNotSupported);

	INFO_PRINTF1(_L("ELockPhoneToFirstICC"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockPhoneToFirstICC,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNotSupported);

	INFO_PRINTF1(_L("ELockOTA"));
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockOTA,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	TESTL(aReqStatus == KErrNotSupported);

	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

