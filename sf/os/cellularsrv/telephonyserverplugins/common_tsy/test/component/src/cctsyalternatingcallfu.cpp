// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The TEFUnit test suite for AlternatingCall in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyalternatingcallfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyAlternatingCallFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestSwitchAlternatingCall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestSwitchAlternatingCall0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestSwitchAlternatingCall0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestNotifyAlternatingCallSwitch0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestNotifyAlternatingCallSwitch0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestNotifyAlternatingCallSwitch0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestGetAlternatingCallCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestGetAlternatingCallMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestSetAlternatingCallMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestNotifyAlternatingCallCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyAlternatingCallFU, TestNotifyAlternatingCallModeChange0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-ALTC-CSAC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SwitchAlternatingCall for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SwitchAlternatingCall for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestSwitchAlternatingCall0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	mobileCall.SwitchAlternatingCall(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line

	err = line.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	mobileCall.SwitchAlternatingCall(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, line, this
		
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-CSAC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SwitchAlternatingCall for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SwitchAlternatingCall for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestSwitchAlternatingCall0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	mobileCall.SwitchAlternatingCall(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-CSAC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SwitchAlternatingCall for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SwitchAlternatingCall for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestSwitchAlternatingCall0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	mobileCall.SwitchAlternatingCall(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-CNACS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyAlternatingCallSwitch for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyAlternatingCallSwitch for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestNotifyAlternatingCallSwitch0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	mobileCall.NotifyAlternatingCallSwitch(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // mobileCall, line
	
	err = line.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	mobileCall.NotifyAlternatingCallSwitch(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-CNACS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyAlternatingCallSwitch for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyAlternatingCallSwitch for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestNotifyAlternatingCallSwitch0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	mobileCall.NotifyAlternatingCallSwitch(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-CNACS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyAlternatingCallSwitch for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyAlternatingCallSwitch for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestNotifyAlternatingCallSwitch0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileCall mobileCall;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
	
	err = OpenNewCall(line, mobileCall, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(mobileCall);

	TRequestStatus reqStatus;
	mobileCall.NotifyAlternatingCallSwitch(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // mobileCall, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-MGACC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetAlternatingCallCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetAlternatingCallCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestGetAlternatingCallCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TUint32 caps(0);
	TInt ret = iPhone.GetAlternatingCallCaps(caps);
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); // this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-MCALCM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetAlternatingCallMode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetAlternatingCallMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestGetAlternatingCallMode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobilePhoneAlternatingCallMode mode(RMobilePhone::EAlternatingModeUnspecified);
	RMobilePhone::TMobileService firstService(RMobilePhone::EServiceUnspecified);
	
	TInt ret = iPhone.GetAlternatingCallMode(mode, firstService);
	ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); // this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-MSACM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetAlternatingCallMode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetAlternatingCallMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestSetAlternatingCallMode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobilePhoneAlternatingCallMode mode(RMobilePhone::EAlternatingModeUnspecified);
	RMobilePhone::TMobileService firstService(RMobilePhone::EServiceUnspecified);

	TRequestStatus reqStatus;
	iPhone.SetAlternatingCallMode(reqStatus, mode, firstService);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); // this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-MNACCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyAlternatingCallCapsChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyAlternatingCallCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestNotifyAlternatingCallCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TUint32 caps(0);

	TRequestStatus reqStatus;
	iPhone.NotifyAlternatingCallCapsChange(reqStatus, caps);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); // this
	
	}


/**
@SYMTestCaseID BA-CTSY-ALTC-MNACMC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyAlternatingCallModeChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyAlternatingCallModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyAlternatingCallFU::TestNotifyAlternatingCallModeChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobilePhone::TMobilePhoneAlternatingCallMode mode(RMobilePhone::EAlternatingModeUnspecified);
	RMobilePhone::TMobileService firstService(RMobilePhone::EServiceUnspecified);

	TRequestStatus reqStatus;
	iPhone.NotifyAlternatingCallModeChange(reqStatus, mode, firstService);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); // this
	
	}



TInt CCTsyAlternatingCallFU::OpenNewCall(RLine& aLine, RMobileCall& aMobileCall, const TDesC& aLineName)
    {
    _LIT(KDoubleColon, "::");

    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(aLineName);
    name.Append(KDoubleColon);

   	return aMobileCall.OpenNewCall(aLine, name);
    }
