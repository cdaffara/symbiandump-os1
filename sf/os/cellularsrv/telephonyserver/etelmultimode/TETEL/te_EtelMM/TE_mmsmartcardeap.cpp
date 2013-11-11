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
// Tests ETelMM's API for Smart Card Application EAP support.  This uses
// the dummy TSY (dmmtsy.tsy) to run basic tests on the
// RMobileSmartCardEap.
// 
//

/**
 @file
 @internalComponent
*/


#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmsmartcardeap.h"
#include "testdef.h"

/**
Each test step initialises it's own name
*/
CTestSmartCardEap::CTestSmartCardEap()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMSmartCardBasedEap"));
	}

enum TVerdict CTestSmartCardEap::doTestStepL()
	{
	INFO_PRINTF1(_L("Test SmartCard Application based EAP Functionality"));

	RMobilePhone mmPhone;
	TInt ret = mmPhone.Open(iTelServer, DMMTSY_PHONE_NAME);
	TEST(ret == KErrNone);

	// Vars to be used throughout test
	TRequestStatus reqStatus = KErrNone;
	RMobilePhone::TAID myAID;
	RMobileSmartCardEap::TEapType myEapType;
	RMobileSmartCardEap mmScEap, mmScEap2, mmScEap3;

	// Test opening RMobileSmartCardEap sub-session to bad eap method/AID
	_LIT8(badAID, "\xFF\xEE\xDD");
	_LIT8(badMethod, "BadEap");

	myAID = badAID();
	myEapType = badMethod();

	//      Open a handle and don't initialise; to be used in TestScEapL() for a little more coverage
	ret = mmScEap3.Open(mmPhone, myAID, myEapType);
	TEST(ret == KErrNone);
	TEST(!mmScEap3.IsEapMethodOwner());

	//      Open with immediate close should have no problems
	ret = mmScEap.Open(mmPhone, myAID, myEapType);
	TEST(ret == KErrNone);
	TEST(!mmScEap.IsEapMethodOwner());
	mmScEap.Close();

	ret = mmScEap.Open(mmPhone, myAID, myEapType);
	TEST(ret == KErrNone);
	TEST(!mmScEap.IsEapMethodOwner());
	mmScEap.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNotFound);
	//      Although the sub-session does not technically work, the owner is still this instance!
	TEST(mmScEap.IsEapMethodOwner());
	mmScEap.Close();

	// Successful opening of RMobileSmartCardEap sub-session
	TPtrC8 hardCodedEapMethod(DMMTSY_PHONE_EAP_METHOD);
	myAID = DMMTSY_PHONE_EAPAPP_AID();
	myEapType = hardCodedEapMethod;
	ret = mmScEap2.Open(mmPhone, myAID, myEapType);
	TEST(ret == KErrNone);
	mmScEap2.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(mmScEap2.IsEapMethodOwner());
	//      Test re-initialising a sub-session that owns the EAP method lock
	mmScEap2.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(mmScEap2.IsEapMethodOwner());

	// Test failure to init RMobileSmartCardEap sub-session to <AID,method> already in use
	ret = mmScEap.Open(mmPhone, myAID, myEapType);
	TEST(ret == KErrNone);
	mmScEap.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrInUse);
	TEST(!mmScEap.IsEapMethodOwner());

	// Test the two active RMobileSmartCardEap sub-sessions' functionality (owner will be the only one to pass everything)
	TestScEapL(reqStatus, mmScEap2, mmScEap, mmScEap3); // status, owner, non-owner, uninitialised handle

	// Release and re-init the valid sub-session on mmScEap (at the same time, run a notification and some gets)
	RMobileSmartCardEap::TEapMethodAccessStatus accessStatus(RMobileSmartCardEap::EEapMethodAvailable);
	RMobileSmartCardEap::TEapMethodAccessStatus accessStatus2(RMobileSmartCardEap::EEapMethodAvailable);
	RMobileSmartCardEap::TEapMethodAccessStatus accessStatusNotify(RMobileSmartCardEap::EEapMethodAvailable);

	mmScEap.NotifyEapMethodAccessStatusChange(reqStatus, accessStatusNotify);
	ret = mmScEap.GetEapMethodAccessStatus(accessStatus);
	TEST(ret == KErrNone);
	TEST(accessStatus == RMobileSmartCardEap::EEapMethodInUseApplicationActive);

	ret = mmScEap2.GetEapMethodAccessStatus(accessStatus2);
	TEST(ret == KErrNone);
	TEST(accessStatus2 == RMobileSmartCardEap::EEapMethodInUseApplicationActive);

	ret = mmScEap2.ReleaseEapMethod();
	TEST(ret == KErrNone);
	TEST(!mmScEap2.IsEapMethodOwner());

	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(accessStatusNotify == RMobileSmartCardEap::EEapMethodUnableToInitialise);

	//      2 secs wait because dummy tsy will take this long to make EAP method available again
	User::After(2000000);

	ret = mmScEap.GetEapMethodAccessStatus(accessStatus);
	TEST(ret == KErrNone);
	TEST(accessStatus == RMobileSmartCardEap::EEapMethodAvailable);

	ret = mmScEap2.GetEapMethodAccessStatus(accessStatus2);
	TEST(ret == KErrNone);
	TEST(accessStatus2 == RMobileSmartCardEap::EEapMethodAvailable);

	mmScEap.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(mmScEap.IsEapMethodOwner());

	// Continue testing functionality with sub-sessions reversed
	// TODO - somefunc (reqStatus, mmScEap2, mmScEap) // status, owner, non-owner (possibly need a second thread to test non-owner)
	TestScEapL(reqStatus, mmScEap, mmScEap2, mmScEap3);

	// Close all sub-sessions and end test
	mmScEap.Close();
	mmScEap2.Close();
	mmScEap3.Close();
	mmPhone.Close(); 
	return TestStepResult();
	}

/**
Tests the EAP-capable application's functionality for two instances of
RMobileSmartCardEap; one which owns a lock on the <EAPType,AID>, which
should pass all requests; and another which does not own the lock and
should fail (KErrInUse) all requests.

@param aOwnerScEap An RMobileSmartCardEap that has successfully called
                   the InitialiseEapMethod request.

@param aNonownerScEap An RMobileSmartCardEap that has opened the sub-
                      session but failed to InitialiseEapMethod because
                      aOwnerScEap initialised first.
*/
void CTestSmartCardEap::TestScEapL(TRequestStatus& aReqStatus, RMobileSmartCardEap& aOwnerScEap,
                                   RMobileSmartCardEap& aNonownerScEap, RMobileSmartCardEap& aUnInitScEap)
	{
	// local variables used throughout the phone tests
	TInt iTestCount = 0;

	// Test GetUserIdentity
	RMobileSmartCardEap::TEapUserIdentityV6 userId;
	RMobileSmartCardEap::TEapUserIdentityV6Pckg userIdPckg(userId);
	RMobileSmartCardEap::TEapUserIdType eapIdType = DMMTSY_PHONE1_EAP_IDTYPE;

	//      call
	INFO_PRINTF2(_L("Test %d - RMobileSmartCardEap::GetEapSimUserIdentity async."), iTestCount++);

	aOwnerScEap.GetUserIdentity(aReqStatus, eapIdType, userIdPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrNone);
	TEST(userId.iEapId == DMMTSY_PHONE1_EAP_UID);

	//      call and cancel
	eapIdType = DMMTSY_PHONE2_EAP_IDTYPE;
	INFO_PRINTF2(_L("Test %d - RMobileSmartCardEap::GetEapSimUserIdentity async & cancel."), iTestCount++);

	aOwnerScEap.GetUserIdentity(aReqStatus, eapIdType, userIdPckg);
	aOwnerScEap.CancelAsyncRequest(EMobileSmartCardEapGetUserIdentity);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrCancel);

	//      non-owner GetUserIdentity
	aNonownerScEap.GetUserIdentity(aReqStatus, eapIdType, userIdPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrInUse);

	//      uninitialised GetUserIdentity
	aUnInitScEap.GetUserIdentity(aReqStatus, eapIdType, userIdPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrBadHandle);

	// Test GetAuthenticationStatus
	RMobileSmartCardEap::TEapAuthStatus authStatus;

	//      call
	INFO_PRINTF2(_L("Test %d - RMobileSmartCardEap::GetAuthenticationStatus async."), iTestCount++);

	aOwnerScEap.GetAuthenticationStatus(aReqStatus, authStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrNone);
	TEST(authStatus == DMMTSY_PHONE_EAPAUTHSTATUS);

	//      call and cancel
	INFO_PRINTF2(_L("Test %d - RMobileSmartCardEap::GetAuthenticationStatus async & cancel."), iTestCount++);

	aOwnerScEap.GetAuthenticationStatus(aReqStatus, authStatus);
	aOwnerScEap.CancelAsyncRequest(EMobileSmartCardEapGetAuthenticationStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrCancel);

	//      non-owner GetAuthenticationStatus
	aNonownerScEap.GetAuthenticationStatus(aReqStatus, authStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrInUse);

	//      uninitialised GetAuthenticationStatus
	aUnInitScEap.GetAuthenticationStatus(aReqStatus, authStatus);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrBadHandle);

	// Test GetEapKey
	RMobileSmartCardEap::TEapKeyV6 eapKey;
	RMobileSmartCardEap::TEapKeyV6Pckg eapKeyPckg(eapKey);

	RMobileSmartCardEap::TEapKeyTag eapKeyTag = DMMTSY_PHONE1_EAPKEYTAG;

	//      call
	INFO_PRINTF2(_L("Test %d - RMobileSmartCardEap::GetEapKey async."), iTestCount++);

	aOwnerScEap.GetEapKey(aReqStatus, eapKeyTag, eapKeyPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrNone);
	TEST(eapKey.iEapKey == DMMTSY_PHONE1_EAPKEY);

	//      call and cancel
	INFO_PRINTF2(_L("Test %d - RMobileSmartCardEap::GetEapKey async & cancel."), iTestCount++);

	eapKeyTag = DMMTSY_PHONE2_EAPKEYTAG;

	aOwnerScEap.GetEapKey(aReqStatus, eapKeyTag, eapKeyPckg);
	aOwnerScEap.CancelAsyncRequest(EMobileSmartCardEapGetEapKey);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrCancel);

	//      non-owner GetEapKey
	aNonownerScEap.GetEapKey(aReqStatus, eapKeyTag, eapKeyPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrInUse);

	//      uninitialised GetEapKey
	aUnInitScEap.GetEapKey(aReqStatus, eapKeyTag, eapKeyPckg);
	User::WaitForRequest(aReqStatus);
	TEST(aReqStatus == KErrBadHandle);

	// Test CAsyncSmartCardEapAuthentication
	INFO_PRINTF2(_L("Test %d - CAsyncSmartCardEapAuthentication async."), iTestCount++);
	TPtrC8 reqPacketPtr(DMMTSY_PHONE1_EAPPACKET);
	CTestEapAuthenticateData* eapAuth = CTestEapAuthenticateData::NewLC(aOwnerScEap, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	TInt err = eapAuth->ValidateEapResponse();
	TEST(err == KErrNone);
	CleanupStack::PopAndDestroy(eapAuth);

	// Test CAsyncSmartCardEapAuthentication with bigger mem allocation
	reqPacketPtr.Set(DMMTSY_PHONE1_EAPPACKET2);
	eapAuth = CTestEapAuthenticateData::NewLC(aOwnerScEap, static_cast<TDesC8&>(reqPacketPtr));
	eapAuth->Start();
	CActiveScheduler::Start();
	err = eapAuth->ValidateEapResponse();
	TEST(err == KErrNone);
	CleanupStack::PopAndDestroy(eapAuth);

	//      non-owner CAsyncSmartCardEapAuthentication
	CTestEapAuthenticateData* eapAuthFail = CTestEapAuthenticateData::NewLC(aNonownerScEap, static_cast<TDesC8&>(reqPacketPtr));
	eapAuthFail->Start();
	CActiveScheduler::Start();
	err = eapAuthFail->ValidateEapResponse();
	TEST(err == KErrInUse);
	CleanupStack::PopAndDestroy(eapAuthFail);

	//      uninitialised CAsyncSmartCardEapAuthentication
	CTestEapAuthenticateData* eapAuthFail2 = CTestEapAuthenticateData::NewLC(aUnInitScEap, static_cast<TDesC8&>(reqPacketPtr));
	eapAuthFail2->Start();
	CActiveScheduler::Start();
	err = eapAuthFail2->ValidateEapResponse();
	TEST(err == KErrBadHandle);
	CleanupStack::PopAndDestroy(eapAuthFail2);

	//      non-owner ReleaseEapMethod
	TInt ret = aNonownerScEap.ReleaseEapMethod();
	TEST(ret == KErrInUse);

	//      uninitialised ReleaseEapMethod();
	ret = aUnInitScEap.ReleaseEapMethod();
	TEST(ret == KErrBadHandle);
	}

CTestEapAuthenticateData* CTestEapAuthenticateData::NewLC(RMobileSmartCardEap& aPhone, TDesC8& aDes)
	{
	CTestEapAuthenticateData* self = new(ELeave) CTestEapAuthenticateData(aPhone, aDes);
	CleanupStack::PushL(self);
	self->ConstructL(aDes);
	return self;
	}

CTestEapAuthenticateData::CTestEapAuthenticateData(RMobileSmartCardEap& aPhone, TDesC8& aDes)
: CActive(EPriorityStandard), iPhone(aPhone), iDes(aDes), iAuthResp(NULL)
	{
	}

CTestEapAuthenticateData::~CTestEapAuthenticateData()
	{
	delete iTwoPhaseRetrieve;
	}

void CTestEapAuthenticateData::ConstructL(TDesC8& aDes)
	{
    CActiveScheduler::Add(this);
	iTwoPhaseRetrieve = CAsyncSmartCardEapAuthentication::NewL(iPhone, aDes);
	}

void CTestEapAuthenticateData::Start()
	{
	iTwoPhaseRetrieve->Start(iStatus);
	SetActive();
	}

void CTestEapAuthenticateData::RunL()
	{
	if (iStatus == KErrNone)
		{
		iAuthResp = &(iTwoPhaseRetrieve->RetrieveEapResponse());
		}

	CActiveScheduler::Stop();
	}

TInt CTestEapAuthenticateData::ValidateEapResponse()
	{
	TInt ret = KErrNone;

	// iStatus will be an error if the request data is invalid.
	if (iStatus != KErrNone)
		{
		ret = iStatus.Int();
		}
	else if (iDes == DMMTSY_PHONE1_EAPPACKET &&
	         (*iAuthResp).Compare(DMMTSY_PHONE2_EAPPACKET) != 0)
		{
		ret = KErrArgument;
		}
	else if (iDes == DMMTSY_PHONE1_EAPPACKET2 &&
	         (*iAuthResp).Compare(DMMTSY_PHONE2_EAPPACKET2) != 0)
		{
		ret = KErrArgument;
		}

	return ret;
	}

void CTestEapAuthenticateData::DoCancel()
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
	SetTestStepName(_L("TestMMSmartCardBasedEapBadClient"));
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

	ret = telServer.LoadPhoneModule(DMMTSY_MODULE_NAME);

	RMobilePhone mmPhone;
	ret = mmPhone.Open(telServer, DMMTSY_PHONE_NAME);

	RMobileSmartCardEap mmEap;
	RMobilePhone::TAID myAID;
	RMobileSmartCardEap::TEapType myEapType;
	TRequestStatus reqStatus;

	TPtrC8 hardCodedEapMethod(DMMTSY_PHONE_EAP_METHOD);
	myAID = DMMTSY_PHONE_EAPAPP_AID();
	myEapType = hardCodedEapMethod;
	ret = mmEap.Open(mmPhone, myAID, myEapType);
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
	mmPhone.Close();
	telServer.Close();

	delete cleanup;

	return KErrNone;
	}

const TUint KDefaultHeapSize = 0x4000;

/**
Test RMobileSmartCardEap initialisation after another client dies.

@see InitialiseAndDie
*/
enum TVerdict CTestInitEapWithoutClosing::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Bad Client with SmartCard Application based EAP Functionality"));

	RMobilePhone mmPhone;
	TInt ret = mmPhone.Open(iTelServer, DMMTSY_PHONE_NAME);
	TESTL(ret == KErrNone);

	// switches off "just-in-time" thread break-points
	// Not needed as TestExecute disables JIT.
//	User::SetJustInTime(EFalse);

	RThread badThread;
	_LIT(badThreadName, "BadThread");
	ret = badThread.Create(badThreadName, InitialiseAndDie, KDefaultStackSize,
	                       KDefaultHeapSize, KDefaultHeapSize, NULL);
	TEST(ret == KErrNone);

	TRequestStatus tStatus;
	badThread.Logon(tStatus);
	badThread.Resume();
	User::WaitForRequest(tStatus);

	TInt exitReason = badThread.ExitReason();
	TEST(exitReason == 666);
	TEST(badThread.ExitType() == EExitPanic);
	badThread.Close();

	// bring back "just-in-time" break-points
	// Not needed as TestExecute disables JIT.
//	User::SetJustInTime(ETrue);

	RMobileSmartCardEap mmEap;
	RMobilePhone::TAID myAID;
	RMobileSmartCardEap::TEapType myEapType;
	TRequestStatus reqStatus;

	TPtrC8 hardCodedEapMethod(DMMTSY_PHONE_EAP_METHOD);
	myAID = DMMTSY_PHONE_EAPAPP_AID();
	myEapType = hardCodedEapMethod;
	ret = mmEap.Open(mmPhone, myAID, myEapType);
	TEST(ret == KErrNone);

	RMobileSmartCardEap::TEapMethodAccessStatus accessStatus(RMobileSmartCardEap::EEapMethodAvailable);
	ret = mmEap.GetEapMethodAccessStatus(accessStatus);
	TEST(ret == KErrNone);
	TEST(accessStatus == RMobileSmartCardEap::EEapMethodAvailable);

	mmEap.InitialiseEapMethod(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus == KErrNone);
	TEST(mmEap.IsEapMethodOwner());

	mmEap.Close();
	mmPhone.Close();

	return TestStepResult();
	}
