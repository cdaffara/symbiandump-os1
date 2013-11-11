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

#include "Te_SimPhBkPhoneSecurityTest.h"

CSimPhBkPhoneSecurityTest::CSimPhBkPhoneSecurityTest() 
	{ 
	SetTestStepName(_L("PhoneSecurityTest"));
	}

TVerdict CSimPhBkPhoneSecurityTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPhBkPhoneSecurityTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TRequestStatus aReqStatus;
	TRequestStatus stat0;
	RMobilePhone::TMobilePhoneLockInfoV1 asyncLockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg asyncLockInfoPckg(asyncLockInfo);

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
	
	RMobilePhone::TMobilePhoneLock whichLock = RMobilePhone::ELockICC;
	INFO_PRINTF1(_L("GetLockInfo test"));
	iPhone.GetLockInfo(aReqStatus,whichLock,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("H.3"));
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusUnlocked);
	TESTL(asyncLockInfo.iSetting == RMobilePhone::ELockSetDisabled);
	
	INFO_PRINTF1(_L("NotifyLockInfoChange test (triggered)"));
	iPhone.NotifyLockInfoChange(aReqStatus, whichLock,asyncLockInfoPckg);
	iPhone.SetLockSetting(stat0,whichLock,RMobilePhone::ELockSetEnabled);
	User::WaitForRequest(stat0);
	User::WaitForRequest(aReqStatus);
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusUnlocked);
	TESTL(asyncLockInfo.iSetting == RMobilePhone::ELockSetEnabled);
	CHECKPOINT_EXL(stat0==KErrNone, CHP_APPS_CASE("H.6"));
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("H.5"));

	INFO_PRINTF1(_L("ChangeSecurityCode test"));
	RMobilePhone::TMobilePhonePasswordChangeV1 passwordChange;
	RMobilePhone::TMobilePhonePasswordChangeV1 wrongPasswordChange;
	passwordChange.iOldPassword = _L("1111");
	passwordChange.iNewPassword = _L("2222");
	wrongPasswordChange.iOldPassword = _L("1111111");
	wrongPasswordChange.iNewPassword = _L("2222");

	RMobilePhone::TMobilePhoneSecurityCode whichCode = RMobilePhone::ESecurityCodePin1;
	
	iPhone.ChangeSecurityCode(aReqStatus,whichCode,wrongPasswordChange);
	User::WaitForRequest(aReqStatus);
	CHECKPOINT_EXL(aReqStatus==KErrArgument, CHP_APPS_CASE("H.8"));

	iPhone.ChangeSecurityCode(aReqStatus,whichCode,passwordChange);
	User::WaitForRequest(aReqStatus);
	CHECKPOINT_EXL(aReqStatus==KErrNone, CHP_APPS_CASE("H.9"));

	RMobilePhone::TMobilePassword code = _L("2222");
	RMobilePhone::TMobilePassword unblockCode = _L("1111");
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;

	iPhone.NotifySecurityEvent(stat0,securityEvent);
	iPhone.VerifySecurityCode(aReqStatus,whichCode,code,unblockCode);
	User::WaitForRequest(stat0);
	User::WaitForRequest(aReqStatus);
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("H.11"));
	CHECKPOINTL(stat0, KErrNone, CHP_APPS_CASE("H.10"));

	TESTL(securityEvent == RMobilePhone::EPin1Verified);
	ret=iPhone.AbortSecurityCode(whichCode);	
	CHECKPOINTL(ret, KErrNone, CHP_APPS_CASE("H.13"));

	// reset initial security code
	passwordChange.iOldPassword = _L("2222");
	passwordChange.iNewPassword = _L("1111");
	iPhone.ChangeSecurityCode(aReqStatus,RMobilePhone::ESecurityCodePin1,passwordChange);
	User::WaitForRequest(aReqStatus);

	
	INFO_PRINTF1(_L("GetSecurityCodeInfo test"));
	const TInt testSecurityCodeNr = 4;
	RMobilePhone::TMobilePhoneSecurityCode testSecurityCode[testSecurityCodeNr]= 
		{
		RMobilePhone::ESecurityCodePin1,
		RMobilePhone::ESecurityCodePin2,
		RMobilePhone::ESecurityCodePuk1,
		RMobilePhone::ESecurityCodePuk2
		};
	
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg securityCodeInfoPckg(securityCodeInfo);

	RMobilePhone::TMobilePhoneSecurityCode notifySecurityCode;	
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 notifySecurityCodeInfo;
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg notifySecurityCodeInfoPckg(notifySecurityCodeInfo);
	RMobilePhone::TMobilePassword wrongCode = _L("1234");
	RMobilePhone::TMobilePassword correctCode = _L("1111");

	// check security code supporting GetSecurityCodeInfo
	for (TInt testSecurityCodeCnt=0; testSecurityCodeCnt<testSecurityCodeNr; testSecurityCodeCnt++)
		{
		whichCode = testSecurityCode[testSecurityCodeCnt];
		notifySecurityCodeInfo.iRemainingEntryAttempts = 0;
		iPhone.GetSecurityCodeInfo(aReqStatus,whichCode,securityCodeInfoPckg);
		User::WaitForRequest(aReqStatus);
		CHECKPOINT_EXL(aReqStatus==KErrNone, CHP_APPS_CASE("H.14"));
		TESTL(securityCodeInfo.iRemainingEntryAttempts == 3);
		}

	// check security code NOT supporting GetSecurityCodeInfo
	whichCode = RMobilePhone::ESecurityCodePhonePassword;
	iPhone.GetSecurityCodeInfo(aReqStatus,whichCode,securityCodeInfoPckg);
	User::WaitForRequest(aReqStatus);
	CHECKPOINT_EXL(aReqStatus==KErrNotFound, CHP_APPS_CASE("H.14"));


	INFO_PRINTF1(_L("NotifySecurityCodeInfoChange test"));
	// check security code supporting NotifySecurityCodeInfoChange only
	for (TInt testSecurityCodeCnt=0; testSecurityCodeCnt<testSecurityCodeNr; testSecurityCodeCnt++)
		{
		whichCode = testSecurityCode[testSecurityCodeCnt];
		notifySecurityCodeInfo.iRemainingEntryAttempts = 0;
		iPhone.NotifySecurityCodeInfoChange(stat0,notifySecurityCode,notifySecurityCodeInfoPckg);
		iPhone.VerifySecurityCode(aReqStatus,whichCode,wrongCode,wrongCode);
		User::WaitForRequest(aReqStatus);
		User::WaitForRequest(stat0);
		CHECKPOINTL(aReqStatus, KErrArgument, CHP_APPS_CASE("H.15"));
		TESTL(notifySecurityCode == whichCode);
		TESTL(notifySecurityCodeInfo.iRemainingEntryAttempts == 2);
		
		notifySecurityCodeInfo.iRemainingEntryAttempts = 0;
		iPhone.NotifySecurityCodeInfoChange(stat0,notifySecurityCode,notifySecurityCodeInfoPckg);
		iPhone.VerifySecurityCode(aReqStatus,whichCode,correctCode,correctCode);
		User::WaitForRequest(aReqStatus);
		User::WaitForRequest(stat0);
		CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("H.15"));
		TESTL(notifySecurityCode == whichCode);
		TESTL(notifySecurityCodeInfo.iRemainingEntryAttempts == 3);
		}

	INFO_PRINTF1(_L("Check that security code get blocked"));
	// block all codes
	for (TInt testSecurityCodeCnt=0; testSecurityCodeCnt<testSecurityCodeNr; testSecurityCodeCnt++)
		{
		whichCode = testSecurityCode[testSecurityCodeCnt];
		do
			{
			iPhone.VerifySecurityCode(aReqStatus,whichCode,wrongCode,wrongCode);
			User::WaitForRequest(aReqStatus);		
			iPhone.GetSecurityCodeInfo(aReqStatus,whichCode,securityCodeInfoPckg);
			User::WaitForRequest(aReqStatus);
			} 
		while (securityCodeInfo.iRemainingEntryAttempts > 0);
		}
	// check the corresponding lock is blocked
	// PIN1 os for ELockICC
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockICC,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("H.16"));
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusBlocked);

	// PIN2 os for ELockPin2
	iPhone.GetLockInfo(aReqStatus,RMobilePhone::ELockPin2,asyncLockInfoPckg);
	User::WaitForRequest(aReqStatus);
	CHECKPOINTL(aReqStatus, KErrNone, CHP_APPS_CASE("H.16"));
	TESTL(asyncLockInfo.iStatus == RMobilePhone::EStatusBlocked);
	
	// there is no coresponding lock for PUK1 and PUK2
	
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

