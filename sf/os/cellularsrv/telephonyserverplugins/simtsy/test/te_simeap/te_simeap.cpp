// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#include "te_simeap.h"


_LIT8(KConfigScAuthAIDAscii, "AID1");
_LIT8(KConfigScAuthAUTN, "AUTN_IN");
_LIT8(KConfigScAuthRAND, "RAND_IN");
_LIT8(KConfigScAuthRES, "123456789");
_LIT8(KConfigScAuthIK, "1234567890");
_LIT8(KConfigScAuthCK, "1234567899");
_LIT8(KConfigScAuthKC, "12345678");
_LIT8(KConfigScAuthAUTS, "1234567897");
_LIT8(KConfigScAuthAUTNbin, "\x00\x01\x02\x03\x04\x05");
_LIT8(KConfigScAuthRANDbin, "\x00\x01\x02\x03\x06\x07");
_LIT8(KConfigScAuthRESbin, "\x00\x00\x00\x11\x11\x11");
_LIT8(KConfigScAuthIKbin, "\x00\x00\x00\x11\x11\x22");
_LIT8(KConfigScAuthCKbin, "\x00\x00\x00\x11\x22\x33");
_LIT8(KConfigScAuthKCbin, "\x00\x01\x00\x02\x03\x04\x05\x06");
_LIT8(KConfigScAuthAIDBin, "\x00\x01\x00\x02");

_LIT8(KConfigEapListAID, "AID00");
_LIT8(KConfigEapListLabel, "EAPAID");
_LIT8(KConfigEapListEapList, "1723");

_LIT8(KConfigScEapAID1, "\x00\x01\x00\x00\x02");
_LIT8(KConfigScEapType1, "17");
_LIT8(KConfigScEapId1, "EAP_ID1");
_LIT8(KConfigScEapPsId1, "EAP_PseudonymID1");
_LIT8(KConfigScEapKey1, "EAP_KEY1");
_LIT8(KConfigScEapKey1EMSK, "EAP_KEY1EXT");
_LIT8(KConfigScEapChal11, "CHALLENGE_DATA1");
_LIT8(KConfigScEapResp11, "RESPONSE_DATA1");
const RMobileSmartCardEap::TEapAuthStatus KEapAuthStatus1_1 = RMobileSmartCardEap::EAuthenticating;
_LIT8(KConfigScEapChal12, "CHALLENGE_DATA2");
_LIT8(KConfigScEapResp12, "RESPONSE_DATA2");
const RMobileSmartCardEap::TEapAuthStatus KEapAuthStatus1_2 = RMobileSmartCardEap::EAuthenticating;
_LIT8(KConfigScEapChal13, "CHALLENGE_DATA3");
_LIT8(KConfigScEapResp13, "RESPONSE_DATA3");
const RMobileSmartCardEap::TEapAuthStatus KEapAuthStatus1_3 = RMobileSmartCardEap::EAuthenticated;

_LIT8(KConfigScEapAID2, "\x00\x02\x00\x00\x02");
_LIT8(KConfigScEapType2, "23");
_LIT8(KConfigScEapId2, "\x00\x01\x02\x03\x04\x05");
_LIT8(KConfigScEapKey2, "\x00\x01\x02\x03\x04\x66");
_LIT8(KConfigScEapChal21, "\x00\x00\x00\x11\x11\x11\x22\x22\x22");
_LIT8(KConfigScEapResp21, "\xAA\xBB\xCC\x00\xDD\xEE\xFF");
const RMobileSmartCardEap::TEapAuthStatus KEapAuthStatus2_1 = RMobileSmartCardEap::EAuthenticating;
_LIT8(KConfigScEapChal22, "\x00\x00\x00\x12\x12\x12\x22\x22\x22");
_LIT8(KConfigScEapResp22, "\xAB\xBB\xCC\x00\xDD\xEE\xFF");
const RMobileSmartCardEap::TEapAuthStatus KEapAuthStatus2_2 = RMobileSmartCardEap::EAuthenticated;

_LIT8(KConfigScReInitEapAID, "\x00\x00\x00\x00\xEE");
_LIT8(KConfigScReInitEapType1, "22");
_LIT8(KConfigScReInitEapType2, "11");

_LIT8(KConfigScEapAIDBadCli, "\x00\x03\x00\x00\x02");
_LIT8(KConfigScEapTypeBadCli, "25");


CTestSmartCardEap::CTestSmartCardEap()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSmartCardEap"));
	}

TVerdict CTestSmartCardEap::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestSmartCardEap"));



	// Vars to be used throughout test
	TRequestStatus reqStatus = KErrNone;
	RMobilePhone::TAID myAID = KConfigScEapAID1();
	RMobileSmartCardEap::TEapType myEapType = KConfigScEapType1();
	RMobileSmartCardEap simScEap, simScEap2;
	// simScEap0 will be a handle to same sub-session as simScEap.
	// simScEap0 is used for mainly testing the
	// NotifyEapMethodAccessStatusChange() notification.
	RMobileSmartCardEap simScEap0;

	// A quick open/close
	RMobileSmartCardEap simScEapQuick;
	TInt ret = simScEapQuick.Open(iPhone, myAID, myEapType);
	TESTL(ret == KErrNone);
	simScEapQuick.Close();

	// Test opening handle with bad params
	INFO_PRINTF1(_L("Attempting to create handle to sub-session with non-existent <AID,EAPType>"));
	_LIT8(badAID, "badAID");
	_LIT8(badEAP, "badEAP");
	myAID = badAID();
	myEapType = badEAP();
	ret = simScEap.Open(iPhone, myAID, myEapType);
	TESTL(ret == KErrNotFound);

	// Test opening 2 handles to the same sub-session using good params
	// for a handle
	INFO_PRINTF1(_L("Creating handle to sub-session"));
	myAID = KConfigScEapAID1();
	myEapType = KConfigScEapType1();
	ret = simScEap0.Open(iPhone, myAID, myEapType);
	CleanupClosePushL(simScEap0);
	TESTL(ret == KErrNone);
	TEST(!simScEap0.IsEapMethodOwner());

	ret = simScEap.Open(iPhone, myAID, myEapType);
	CleanupClosePushL(simScEap);
	TESTL(ret == KErrNone);
	TEST(!simScEap.IsEapMethodOwner());

	// Test notification for only one of the handles; the one that does
	// NOT initialise.
	RMobileSmartCardEap::TEapMethodAccessStatus eapAccessStatNotif = RMobileSmartCardEap::EEapMethodInUseApplicationInactive;
	TRequestStatus reqStatusNotif = KErrNone;
	simScEap0.NotifyEapMethodAccessStatusChange(reqStatusNotif, eapAccessStatNotif);

	// Test GetEapMethodAccessStatus()
	INFO_PRINTF1(_L("RMobileSmartCardEap::GetEapMethodAccessStatus sync."));
	RMobileSmartCardEap::TEapMethodAccessStatus eapAccessStat = RMobileSmartCardEap::EEapMethodInUseApplicationInactive;
	simScEap0.GetEapMethodAccessStatus(eapAccessStat);
	TEST(eapAccessStat == RMobileSmartCardEap::EEapMethodAvailable);
	simScEap.GetEapMethodAccessStatus(eapAccessStat);
	TEST(eapAccessStat == RMobileSmartCardEap::EEapMethodAvailable);

	// Initialise only one of the handles
	INFO_PRINTF1(_L("Initialising handle to sub-session"));
	simScEap.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);
	TESTL(simScEap.IsEapMethodOwner());
	simScEap.InitialiseEapMethod(reqStatus); // second initialise should do nothing and succeed
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	// new status should be achieved
	simScEap0.GetEapMethodAccessStatus(eapAccessStat);
	TEST(eapAccessStat == RMobileSmartCardEap::EEapMethodInUseApplicationActive);
	simScEap.GetEapMethodAccessStatus(eapAccessStat);
	TEST(eapAccessStat == RMobileSmartCardEap::EEapMethodInUseApplicationActive);

	// Notification should have now completed.
	User::WaitForRequest(reqStatusNotif);
	TEST(reqStatusNotif == KErrNone);
	TEST(eapAccessStatNotif == RMobileSmartCardEap::EEapMethodInUseApplicationActive);

	// re-post notification for later!
	simScEap0.NotifyEapMethodAccessStatusChange(reqStatusNotif, eapAccessStatNotif);

	// Test GetUserIdentity
	INFO_PRINTF1(_L("RMobileSmartCardEap::GetEapSimUserIdentity async."));
	RMobileSmartCardEap::TEapUserIdentityV6 userId;
	RMobileSmartCardEap::TEapUserIdentityV6Pckg userIdPckg(userId);
	RMobileSmartCardEap::TEapUserIdType eapIdType = RMobileSmartCardEap::EPermanentIdentity;
	RMobileSmartCardEap::TEapUserIdType eapPsIdType = RMobileSmartCardEap::EPseudonymIdentity;

	// EPermanentIdentity
	simScEap.GetUserIdentity(reqStatus, eapIdType, userIdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(userId.iEapId == KConfigScEapId1());
	// EPseudonymIdentity
	simScEap.GetUserIdentity(reqStatus, eapPsIdType, userIdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(userId.iEapId == KConfigScEapPsId1());
	// bad id tag
	simScEap.GetUserIdentity(reqStatus, static_cast<RMobileSmartCardEap::TEapUserIdType>(5), userIdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrArgument);

	// Test GetAuthenticationStatus
	INFO_PRINTF1(_L("RMobileSmartCardEap::GetAuthenticationStatus async."));
	RMobileSmartCardEap::TEapAuthStatus authStatus = RMobileSmartCardEap::EHeld;

	simScEap.GetAuthenticationStatus(reqStatus, authStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(authStatus == RMobileSmartCardEap::ENoAuthStarted);

	// Test GetEapKey
	INFO_PRINTF1(_L("RMobileSmartCardEap::GetEapKey async."));
	RMobileSmartCardEap::TEapKeyV6 eapKey;
	RMobileSmartCardEap::TEapKeyV6Pckg eapKeyPckg(eapKey);
	RMobileSmartCardEap::TEapKeyTag eapKeyTag = RMobileSmartCardEap::EEapKeyMSK;
	RMobileSmartCardEap::TEapKeyTag eapExtKeyTag = RMobileSmartCardEap::EEapKeyEMSK;

	// MSK
	simScEap.GetEapKey(reqStatus, eapKeyTag, eapKeyPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(eapKey.iEapKey == KConfigScEapKey1());
	// EMSK
	simScEap.GetEapKey(reqStatus, eapExtKeyTag, eapKeyPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(eapKey.iEapKey == KConfigScEapKey1EMSK());
	// bad key tag
	simScEap.GetEapKey(reqStatus, static_cast<RMobileSmartCardEap::TEapKeyTag>(5), eapKeyPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrArgument);

	CActiveScheduler* iSimEapTestScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(iSimEapTestScheduler);
	CActiveScheduler::Install(iSimEapTestScheduler);

	// Test CAsyncSmartCardEapAuthentication *3 as specified in config file
	INFO_PRINTF1(_L("CAsyncSmartCardEapAuthentication async."));
	// 1st
	TPtrC8 reqPacketPtr(KConfigScEapChal11);
	CTestSimEapAuthenticateData* eapAuth = CTestSimEapAuthenticateData::NewLC(simScEap, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	ret = eapAuth->ValidateEapResponse(KConfigScEapResp11());
	TEST(ret == KErrNone);
	CleanupStack::PopAndDestroy(eapAuth);

	simScEap.GetAuthenticationStatus(reqStatus, authStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(authStatus == KEapAuthStatus1_1);

	// bad authenticate creeps in!
	_LIT8(badAuthChall, "what????");
	reqPacketPtr.Set(badAuthChall);
	eapAuth = CTestSimEapAuthenticateData::NewLC(simScEap, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	ret = eapAuth->ValidateEapResponse(badAuthChall());
	TEST(ret == KErrCorrupt);
	CleanupStack::PopAndDestroy(eapAuth);

	// 2nd
	reqPacketPtr.Set(KConfigScEapChal12);
	eapAuth = CTestSimEapAuthenticateData::NewLC(simScEap, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	ret = eapAuth->ValidateEapResponse(KConfigScEapResp12());
	TEST(ret == KErrNone);
	CleanupStack::PopAndDestroy(eapAuth);

	simScEap.GetAuthenticationStatus(reqStatus, authStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(authStatus == KEapAuthStatus1_2);

	// third authenticate will be tested after running some requests on
	// a second sub-session, simScEap2.

	// run a second sub-session at the same time
	INFO_PRINTF1(_L("Opening and submitting requests on a second sub-session"));
	myAID = KConfigScEapAID2();
	myEapType = KConfigScEapType2();
	ret = simScEap2.Open(iPhone, myAID, myEapType);
	CleanupClosePushL(simScEap2);
	TESTL(ret == KErrNone);
	TEST(!simScEap2.IsEapMethodOwner());

	simScEap2.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);
	TESTL(simScEap2.IsEapMethodOwner());

	// EPermanentIdentity
	simScEap2.GetUserIdentity(reqStatus, eapIdType, userIdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(userId.iEapId == KConfigScEapId2());

	// EPseudonymIdentity - which doesn't exist for this sub-session!
	simScEap2.GetUserIdentity(reqStatus, eapPsIdType, userIdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNotFound);

	simScEap2.GetAuthenticationStatus(reqStatus, authStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(authStatus == RMobileSmartCardEap::ENoAuthStarted);

	// MSK
	simScEap2.GetEapKey(reqStatus, eapKeyTag, eapKeyPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(eapKey.iEapKey == KConfigScEapKey2());

	// EMSK - which doesn't exist for this sub-session!
	simScEap2.GetEapKey(reqStatus, eapExtKeyTag, eapKeyPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNotFound);

	// 2* Authenticate as per config.txt
	reqPacketPtr.Set(KConfigScEapChal21);
	eapAuth = CTestSimEapAuthenticateData::NewLC(simScEap2, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	ret = eapAuth->ValidateEapResponse(KConfigScEapResp21());
	TEST(ret == KErrNone);
	CleanupStack::PopAndDestroy(eapAuth);
	simScEap2.GetAuthenticationStatus(reqStatus, authStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(authStatus == KEapAuthStatus2_1);

	reqPacketPtr.Set(KConfigScEapChal22);
	eapAuth = CTestSimEapAuthenticateData::NewLC(simScEap2, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	ret = eapAuth->ValidateEapResponse(KConfigScEapResp22());
	TEST(ret == KErrNone);
	CleanupStack::PopAndDestroy(eapAuth);
	simScEap2.GetAuthenticationStatus(reqStatus, authStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(authStatus == KEapAuthStatus2_2);

	// see if first sub-session still works
	reqPacketPtr.Set(KConfigScEapChal13);
	eapAuth = CTestSimEapAuthenticateData::NewLC(simScEap, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	ret = eapAuth->ValidateEapResponse(KConfigScEapResp13());
	TEST(ret == KErrNone);
	CleanupStack::PopAndDestroy(eapAuth);

	simScEap.GetAuthenticationStatus(reqStatus, authStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(authStatus == KEapAuthStatus1_3);

	// one more attempt at authenticate on each sub-session for an
	// error (i.e. no more entries in config.txt!)
	INFO_PRINTF1(_L("Test one too many authenticates"));
	reqPacketPtr.Set(KConfigScEapChal13);
	eapAuth = CTestSimEapAuthenticateData::NewLC(simScEap, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	ret = eapAuth->ValidateEapResponse(KConfigScEapResp13());
	TEST(ret == KErrAccessDenied); // it's just an error code that simtsy returns in such a use case
	CleanupStack::PopAndDestroy(eapAuth);
	reqPacketPtr.Set(KConfigScEapChal21);
	eapAuth = CTestSimEapAuthenticateData::NewLC(simScEap2, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	ret = eapAuth->ValidateEapResponse(KConfigScEapResp21());
	TEST(ret == KErrAccessDenied); // it's just an error code that simtsy returns in such a use case
	CleanupStack::PopAndDestroy(eapAuth);

	// Release simScEap handle and check completion of simScEap0's
	// previously placed notification, and do a couple more get status.
	INFO_PRINTF1(_L("Releasing first handle"));
	ret = simScEap.ReleaseEapMethod();
	TESTL(ret == KErrNone);
	User::WaitForRequest(reqStatusNotif);
	TEST(reqStatusNotif == KErrNone);
	TEST(eapAccessStatNotif == RMobileSmartCardEap::EEapMethodAvailable);
	simScEap0.GetEapMethodAccessStatus(eapAccessStat);
	TEST(eapAccessStat == RMobileSmartCardEap::EEapMethodAvailable);
	simScEap.GetEapMethodAccessStatus(eapAccessStat);
	TEST(eapAccessStat == RMobileSmartCardEap::EEapMethodAvailable);

	// Now that simScEap has released, simScEap0 should be able to
	// initialise, but no more config data, so KErrNotFound should be
	// returned.  simScEap0 takes ownership anyway!
	simScEap0.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNotFound);
	TEST(simScEap0.IsEapMethodOwner());

	// kill both sub-sessions and active scheduler
	CleanupStack::PopAndDestroy(&simScEap2);
	CleanupStack::PopAndDestroy(iSimEapTestScheduler);
	CleanupStack::PopAndDestroy(&simScEap);
	CleanupStack::PopAndDestroy(&simScEap0);


	// Initialise two further sub-sessions, for stress testing, and
	// ensuring simulator's array indices do not get mangled.

	// Open both sub-sessions
	RMobileSmartCardEap simScEapReInit_1, simScEapReInit_2;
	myAID = KConfigScReInitEapAID();
	myEapType = KConfigScReInitEapType1();
	ret = simScEapReInit_1.Open(iPhone, myAID, myEapType);
	TESTL(ret == KErrNone);
	CleanupClosePushL(simScEapReInit_1);
	myEapType = KConfigScReInitEapType2();
	ret = simScEapReInit_2.Open(iPhone, myAID, myEapType);
	TESTL(ret == KErrNone);
	CleanupClosePushL(simScEapReInit_2);

	// Initialise both sub-sessions
	simScEapReInit_1.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	simScEapReInit_2.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);

	// Neither sub-session contains any data, so any request will
	// return an error.  Plus, making the following two erroneous
	// requests will give more branch coverage to them in simtsy! :-)

	simScEapReInit_1.GetUserIdentity(reqStatus, eapIdType, userIdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNotFound);

	simScEapReInit_2.GetEapKey(reqStatus, eapKeyTag, eapKeyPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNotFound);

	// now they are both in use
	simScEapReInit_1.GetEapMethodAccessStatus(eapAccessStat);
	TEST(eapAccessStat == RMobileSmartCardEap::EEapMethodInUseApplicationActive);
	simScEapReInit_2.GetEapMethodAccessStatus(eapAccessStat);
	TEST(eapAccessStat == RMobileSmartCardEap::EEapMethodInUseApplicationActive);

	ret = simScEapReInit_1.ReleaseEapMethod();
	TESTL(ret == KErrNone);
	ret = simScEapReInit_2.ReleaseEapMethod();
	TESTL(ret == KErrNone);

	// kill 'em; I've had enough
	CleanupStack::PopAndDestroy(&simScEapReInit_2);
	CleanupStack::PopAndDestroy(&simScEapReInit_1);

	return TestStepResult();
	}

CTestSimEapAuthenticateData* CTestSimEapAuthenticateData::NewLC(RMobileSmartCardEap& aPhone, TDesC8& aDes)
	{
	CTestSimEapAuthenticateData* self = new(ELeave) CTestSimEapAuthenticateData(aPhone, aDes);
	CleanupStack::PushL(self);
	self->ConstructL(aDes);
	return self;
	}

CTestSimEapAuthenticateData::CTestSimEapAuthenticateData(RMobileSmartCardEap& aPhone, TDesC8& aDes)
: CActive(EPriorityStandard), iPhone(aPhone), iDes(aDes), iAuthResp(NULL)
	{
	}

CTestSimEapAuthenticateData::~CTestSimEapAuthenticateData()
	{
	delete iTwoPhaseRetrieve;
	iTwoPhaseRetrieve = NULL;
	}

void CTestSimEapAuthenticateData::ConstructL(TDesC8& aDes)
	{
    CActiveScheduler::Add(this);
	iTwoPhaseRetrieve = CAsyncSmartCardEapAuthentication::NewL(iPhone, aDes);
	}

void CTestSimEapAuthenticateData::Start()
	{
	iTwoPhaseRetrieve->Start(iStatus);
	SetActive();
	}

void CTestSimEapAuthenticateData::RunL()
	{
	if (iStatus == KErrNone)
		{
		iAuthResp = &(iTwoPhaseRetrieve->RetrieveEapResponse());
		}

	CActiveScheduler::Stop();
	}

/**
Validates if the EAP authenticate response is the same as the
aExpectedResponse the user expects.

@param aExpectedResponse The response that this active object should
                         have retrieved from the TSY.
@return iStatus of this active object if it has not completed
        successfully, KErrCorrupt if the data retrieved from TSY is not
        the same as the aExpectedResponse, or KErrNone if all is well.
*/
TInt CTestSimEapAuthenticateData::ValidateEapResponse(const TDesC8& aExpectedResponse)
	{
	TInt ret = KErrNone;

	// iStatus will be an error if the request data is invalid.
	if (iStatus != KErrNone)
		{
		ret = iStatus.Int();
		}
	else if ((*iAuthResp) != aExpectedResponse)
		{
		ret = KErrCorrupt;
		}

	return ret;
	}

void CTestSimEapAuthenticateData::DoCancel()
	{
	iTwoPhaseRetrieve->Cancel();
	CActiveScheduler::Stop();
	}


// Now test for bad clients who open but never close the phone //

/**
Each test step initialises its own name.
*/
CTestInitEapWithoutClosing::CTestInitEapWithoutClosing()
	{
	SetTestStepName(_L("TestSmartCardBasedEapBadClient"));
	}

/**
Thread entry point, which initialises an RMobileSmartCardEap sub-
session, but dies without closing the handle.
*/
TInt InitialiseAndDie(TAny* /*aArg*/)
	{
	// New cleanup-trap for this thread
	CTrapCleanup* cleanup = CTrapCleanup::New();

	RTelServer telServer;
	TInt ret = telServer.Connect();

	ret = telServer.LoadPhoneModule(KSimtsyName);

	RMobilePhone simPhone;
	ret = simPhone.Open(telServer, KPhoneName);

	RMobileSmartCardEap mmEap;
	RMobilePhone::TAID myAID = KConfigScEapAIDBadCli();
	RMobileSmartCardEap::TEapType myEapType = KConfigScEapTypeBadCli();
	TRequestStatus reqStatus;

	ret = mmEap.Open(simPhone, myAID, myEapType);
	mmEap.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);

	TInt kill;
	// Return a different panic code depending on previous actions.
	// 666 is success!
	if (ret != KErrNone || reqStatus != KErrNone)
		{
		kill = 999;
		}
	else
		{
		kill = 666;
		}

	_LIT(paniced, "paniced");
	TPtrC panicedPtr(paniced);
	User::Panic(panicedPtr, kill);

	// Cannot reach this point!  Thread has died.
	simPhone.Close();
	telServer.Close();

	delete cleanup;

	return KErrNone;
	}

const TUint KDefaultHeapSize = 0x1000;

/**
Test RMobileSmartCardEap initialisation after another client dies.

@see InitialiseAndDie()
*/
enum TVerdict CTestInitEapWithoutClosing::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Bad Client with SmartCard Application based EAP Functionality"));

	RThread badThread;
	_LIT(badThreadName, "BadThread");
	TInt ret = badThread.Create(badThreadName, InitialiseAndDie, KDefaultStackSize,
	                            KDefaultHeapSize, KDefaultHeapSize, NULL);
	TESTL(ret == KErrNone);

	TRequestStatus tStatus;
	badThread.Logon(tStatus);
	badThread.Resume();
	User::WaitForRequest(tStatus);

	TInt exitReason = badThread.ExitReason();
	TESTL(exitReason == 666);
	TESTL(badThread.ExitType() == EExitPanic);
	badThread.Close();

	RMobileSmartCardEap simEap;
	RMobilePhone::TAID myAID = KConfigScEapAIDBadCli();
	RMobileSmartCardEap::TEapType myEapType = KConfigScEapTypeBadCli();
	ret = simEap.Open(iPhone, myAID, myEapType);
	CleanupClosePushL(simEap);
	TESTL(ret == KErrNone);

	RMobileSmartCardEap::TEapMethodAccessStatus accessStatus(RMobileSmartCardEap::EEapMethodAvailable);
	ret = simEap.GetEapMethodAccessStatus(accessStatus);
	TEST(ret == KErrNone);
	TEST(accessStatus == RMobileSmartCardEap::EEapMethodAvailable);

	simEap.InitialiseEapMethod(tStatus);
	User::WaitForRequest(tStatus);
	TEST(tStatus == KErrNone);
	TEST(simEap.IsEapMethodOwner());

	CleanupStack::PopAndDestroy(&simEap);

	return TestStepResult();
	}

// Test extended functionality of RMobilePhone //

CTestSmartCardAppAuthenticate::CTestSmartCardAppAuthenticate()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSmartCardAppAuthenticate"));
	}

TVerdict CTestSmartCardAppAuthenticate::doTestStepL()
	{
	TRequestStatus reqStatus;

	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;

	authenticateData.iAUTN = KConfigScAuthAUTN();
	authenticateData.iRAND = KConfigScAuthRAND();

	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);

	// Success
	iPhone.SmartCardAuthenticate(reqStatus, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	TEST(authenticateData.iAUTN == KConfigScAuthAUTN());
	TEST(authenticateData.iRAND == KConfigScAuthRAND());
	TEST(authenticateData.iRES == KConfigScAuthRES());
	TEST(authenticateData.iIK == KConfigScAuthIK());
	TEST(authenticateData.iCK == KConfigScAuthCK());
	TEST(authenticateData.iKc == KConfigScAuthKC());

	// Failure
	iPhone.SmartCardAuthenticate(reqStatus, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrGeneral);

	TEST(authenticateData.iAUTN == KConfigScAuthAUTN());
	TEST(authenticateData.iRAND == KConfigScAuthRAND());	
	TEST(authenticateData.iAUTS == KConfigScAuthAUTS());	

	// Now with specific AID
	RMobilePhone::TAID scAID = KConfigScAuthAIDAscii();

	iPhone.SmartCardAuthenticate(reqStatus, scAID, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	TEST(authenticateData.iAUTN == KConfigScAuthAUTN());
	TEST(authenticateData.iRAND == KConfigScAuthRAND());
	TEST(authenticateData.iRES == KConfigScAuthRES());
	TEST(authenticateData.iIK == KConfigScAuthIK());
	TEST(authenticateData.iCK == KConfigScAuthCK());
	TEST(authenticateData.iKc == KConfigScAuthKC());

	// Specific AID and fail
	iPhone.SmartCardAuthenticate(reqStatus, scAID, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrTotalLossOfPrecision); // same error as in config.txt
	                                              // (there's nothing special about this returned error code.)

	TEST(authenticateData.iAUTN == KConfigScAuthAUTN());
	TEST(authenticateData.iRAND == KConfigScAuthRAND());	
	TEST(authenticateData.iAUTS == KConfigScAuthAUTS());	

	// All binary data
	authenticateData.iAUTN = KConfigScAuthAUTNbin();
	authenticateData.iRAND = KConfigScAuthRANDbin();

	scAID = KConfigScAuthAIDBin();

	iPhone.SmartCardAuthenticate(reqStatus, scAID, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	TEST(authenticateData.iAUTN == KConfigScAuthAUTNbin());
	TEST(authenticateData.iRAND == KConfigScAuthRANDbin());
	TEST(authenticateData.iRES == KConfigScAuthRESbin());
	TEST(authenticateData.iIK == KConfigScAuthIKbin());
	TEST(authenticateData.iCK == KConfigScAuthCKbin());
	TEST(authenticateData.iKc == KConfigScAuthKCbin());

	return TestStepResult();
	}

CTestGetSmartCardAppEapList::CTestGetSmartCardAppEapList()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetSmartCardAppEapList"));
	}

TVerdict CTestGetSmartCardAppEapList::doTestStepL()
	{
	TRequestStatus reqStatus;
	RMobilePhone::TUiccApplicationEapInfoV6 scAppInfo;

	RMobilePhone::TUiccApplicationEapInfoV6Pckg scAppInfoPckg(scAppInfo);

	iPhone.GetSmartCardApplicationInfo(reqStatus, 0, scAppInfoPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	TEST(scAppInfo.iAID == KConfigEapListAID());
	TEST(scAppInfo.iLabel == KConfigEapListLabel());	
	TEST(scAppInfo.iEapTypeList == KConfigEapListEapList());

	return TestStepResult();
	}

CTestSmartCardAppAuthenticate2::CTestSmartCardAppAuthenticate2()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSmartCardAppAuthenticate2"));
	}

TVerdict CTestSmartCardAppAuthenticate2::doTestStepL()
	{
	// This is to test that the smart card authentication will work with EAP-SIM.
	TRequestStatus reqStatus;

	RMobilePhone::TSmartCardAuthenticateDataV6 authenticateData;

	authenticateData.iAUTN = _L8("");
	authenticateData.iRAND = KConfigScAuthRAND();
	authenticateData.iRES  = _L8("");
	authenticateData.iIK   = _L8("");
	authenticateData.iCK   = _L8("");
	authenticateData.iKc   = _L8("");

	RMobilePhone::TSmartCardAuthenticateDataV6Pckg authenticateDataPckg(authenticateData);

	// Success
	iPhone.SmartCardAuthenticate(reqStatus, authenticateDataPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	TEST(authenticateData.iAUTN == _L8(""));
	TEST(authenticateData.iRAND == KConfigScAuthRAND());
	TEST(authenticateData.iRES  == KConfigScAuthRES());
	TEST(authenticateData.iIK   == _L8(""));
	TEST(authenticateData.iCK   == _L8(""));
	TEST(authenticateData.iKc   == KConfigScAuthKC());	

	return TestStepResult();
	}
