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
// The TEFUnit test suite for PhoneCapability in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyphonecapabilityfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include "MmTsy_conf.h"

CTestSuite* CCTsyPhoneCapabilityFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneCapabilityFU, TestNotifyCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneCapabilityFU, TestNotifyCapsChange0001aL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneCapabilityFU, TestNotifyCapsChange0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneCapabilityFU, TestNotifyCapsChange0001cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneCapabilityFU, TestNotifyCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneCapabilityFU, TestNotifyCapsChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneCapabilityFU, TestGetMultimodeCaps0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PCAP-PCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::NotifyCapsChange
@SYMTestPriority High
@SYMTestActions Invokes RPhone::NotifyCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneCapabilityFU::TestNotifyCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// invoke caps change, invoke completion of not requested NotifyCapsChange
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    line.Close();

	RLine line2;

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPhone::NotifyCapsChange.
 	//-------------------------------------------------------------------------

	TRequestStatus status;
	RPhone::TCaps caps;
	iPhone.NotifyCapsChange(status, caps);

	// invoke caps change (open data line)
	err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
	
	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(
			RPhone::KCapsData |
			RPhone::KCapsEventModemDetection |
			RPhone::KCapsStealCommPort),
			caps.iFlags);

	iPhone.NotifyCapsChange(status, caps);

	// invoke caps change (open auxilliary voice line)
	err = line2.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)


	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(
	           RPhone::KCapsData |
	           RPhone::KCapsEventModemDetection |
	           RPhone::KCapsStealCommPort |
	           RPhone::KCapsVoice) ,
	            caps.iFlags);
	
	iPhone.NotifyCapsChange(status, caps);

	// invoke caps change
    line.Close();

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(RPhone::KCapsVoice), caps.iFlags);

	iPhone.NotifyCapsChange(status, caps);

	line2.Close();

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(0), caps.iFlags);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PCAP-PCC-0001a
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::NotifyCapsChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RPhone::NotifyCapsChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneCapabilityFU::TestNotifyCapsChange0001aL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPhone::NotifyCapsChange.
 	//-------------------------------------------------------------------------

	TRequestStatus status;
	RPhone::TCaps caps;
	iPhone.NotifyCapsChange(status, caps);

	// invoke caps change
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(RPhone::KCapsVoice), caps.iFlags);

	iPhone.NotifyCapsChange(status, caps);

	// invoke caps change
    line.Close();
    
	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(0), caps.iFlags);

	iPhone.NotifyCapsChange(status, caps);
	
	// invoke caps change
	// now we open auxilliary voice line
	err = line.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(RPhone::KCapsVoice), caps.iFlags);

	iPhone.NotifyCapsChange(status, caps);

	// invoke caps change
    line.Close();

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(0), caps.iFlags);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PCAP-PCC-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::NotifyCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RPhone::NotifyCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneCapabilityFU::TestNotifyCapsChange0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPhone::NotifyCapsChange.
 	//-------------------------------------------------------------------------

	TRequestStatus status;
	RPhone::TCaps caps;
	iPhone.NotifyCapsChange(status, caps);

	// invoke caps change
	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(
			RPhone::KCapsData |
			RPhone::KCapsEventModemDetection |
			RPhone::KCapsStealCommPort),
			caps.iFlags);

	iPhone.NotifyCapsChange(status, caps);

	// invoke caps change
    line.Close();

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint>(0), caps.iFlags);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PCAP-PCC-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::NotifyCapsChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RPhone::NotifyCapsChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneCapabilityFU::TestNotifyCapsChange0001cL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RLine line2;

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPhone::NotifyCapsChange.
 	//-------------------------------------------------------------------------

	TRequestStatus status;
	RPhone::TCaps caps;
	iPhone.NotifyCapsChange(status, caps);

	// open fax line
	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
	// open voice line
	err = line2.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	// fax line availability does not have sense for phone capabilities in CTSY
	// so request completes after voice line is opened with RPhone::KCapsVoice
	ASSERT_EQUALS(static_cast<TUint>(RPhone::KCapsVoice), caps.iFlags);

	iPhone.NotifyCapsChange(status, caps);

	// close fax line
    line.Close();
	// close voice line
    line2.Close();

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	AssertMockLtsyStatusL();
	// fax line availability does not have sense for phone capabilities in CTSY
	// so request completes after closing voice line
	ASSERT_EQUALS(static_cast<TUint>(0), caps.iFlags);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PCAP-PCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPhone::NotifyCapsChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPhone::NotifyCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneCapabilityFU::TestNotifyCapsChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Test cancelling of RPhone::NotifyCapsChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus status;
	RPhone::TCaps caps;
	iPhone.NotifyCapsChange(status, caps);

	iPhone.NotifyCapsChangeCancel();

	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrCancel, status.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PCAP-PCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPhone::NotifyCapsChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPhone::NotifyCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneCapabilityFU::TestNotifyCapsChange0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	// open voice line
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPhone::NotifyCapsChange
 	//-------------------------------------------------------------------------

	// request notifys
	TRequestStatus status;
	RPhone::TCaps caps;
	iPhone.NotifyCapsChange(status, caps);

	TRequestStatus status2;
	RPhone::TCaps caps2;
	phone2.NotifyCapsChange(status2, caps2);

	// invoke caps change
    line.Close();

	// check results
	User::WaitForRequest(status);
	ASSERT_EQUALS(KErrNone, status.Int());
	TUint expCaps(0);
	ASSERT_EQUALS(expCaps, caps.iFlags);

	User::WaitForRequest(status2);
	ASSERT_EQUALS(KErrNone, status2.Int());
	ASSERT_EQUALS(expCaps, caps2.iFlags);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CTSY-PCAP-MGMC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetMultimodeCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetMultimodeCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneCapabilityFU::TestGetMultimodeCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST: Successful completion request of RMobilePhone::GetMultimodeCaps.
 	//-------------------------------------------------------------------------

	TUint32 caps;
	TInt err = iPhone.GetMultimodeCaps(caps);
	ASSERT_EQUALS(KErrNone, err);
	// CTSY allways returns KMultimodeCaps
	TUint32 expCaps(KMultimodeCaps);
	ASSERT_EQUALS(expCaps, caps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}
