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
// The TEFUnit test suite for DTMF in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsydtmffu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>


CTestSuite* CCTsyDTMFFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestGetDTMFCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestGetDTMFCaps0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestGetDTMFCaps00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestStopDTMFTone0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestReadDTMFTones0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestSendDTMFTones0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestSendDTMFTones0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestSendDTMFTones0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestSendDTMFTones0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestSendDTMFTones0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestStartDTMFTone0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestNotifyDTMFEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestNotifyDTMFCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestNotifyDTMFCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestNotifyDTMFCapsChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestNotifyStopInDTMFString0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestNotifyStopInDTMFString0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestNotifyStopInDTMFString0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestContinueDTMFStringSending0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDTMFFU, TestContinueDTMFStringSending0005L);

	END_SUITE;
	}


//
// Actual test cases
//


void CCTsyDTMFFU::AuxTestGetDTMFCaps0001L(
		RMobilePhone::TMobileService aMobileService,
		const TDesC& aLineName,
		TUint32 aExpCaps)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST: Successful completion request of
	// RMobilePhone::GetDTMFCaps when there is no calls.
 	//-------------------------------------------------------------------------

	TUint32 caps(0);
	TInt ret = iPhone.GetDTMFCaps(caps);
	ASSERT_EQUALS(KErrNone, ret);
	AssertMockLtsyStatusL();
	TUint32 expCapsNoCalls(0);
	ASSERT_EQUALS(expCapsNoCalls, caps);

 	//-------------------------------------------------------------------------
	// TEST: Successful completion request of
	// RMobilePhone::GetDTMFCaps when there is a call.
 	//-------------------------------------------------------------------------

	// invoke call info change
	RLine line;
	OpenLineLC(line, aLineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, aMobileService, RMobileCall::EStatusConnected);

	// request execute and check results
	ret = iPhone.GetDTMFCaps(caps);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(aExpCaps, caps);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // call, line, this

	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MGDC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetDTMFCaps for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetDTMFCaps for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestGetDTMFCaps0001L()
	{

	TUint32 expCaps = 
			RMobilePhone::KCapsSendDTMFString |
			RMobilePhone::KCapsSendDTMFSingleTone;

	AuxTestGetDTMFCaps0001L(RMobilePhone::EVoiceService, KMmTsyVoice1LineName, expCaps);

	// test for voice call from auxilliary voice line
	// returned caps are wrong
	AuxTestGetDTMFCaps0001L(RMobilePhone::EAuxVoiceService, KMmTsyVoice2LineName, expCaps);
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MGDC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetDTMFCaps for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetDTMFCaps for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestGetDTMFCaps0006L()
	{

	TUint32 expCaps(0);
	AuxTestGetDTMFCaps0001L(RMobilePhone::ECircuitDataService, KMmTsyDataLineName, expCaps);
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MGDC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetDTMFCaps for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetDTMFCaps for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestGetDTMFCaps00011L()
	{

	TUint32 expCaps(0);
	AuxTestGetDTMFCaps0001L(RMobilePhone::EFaxService, KMmTsyFaxLineName, expCaps);
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MSTPDT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::StopDTMFTone
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::StopDTMFTone
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestStopDTMFTone0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);
	
	// start dtmf tone
	TChar tone('*');
	TMockLtsyData1<TChar> toneLtsyData( tone );
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneStartDTMFTone, data);
	TInt err = iPhone.StartDTMFTone(tone);
	ASSERT_EQUALS(KErrNone, err);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStopDTMFTone, KErrNotSupported);

	err = iPhone.StopDTMFTone();
	ASSERT_EQUALS(KErrNotSupported, err);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::StopDTMFTone when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStopDTMFTone);

	err = iPhone.StopDTMFTone();
	ASSERT_EQUALS(KErrNone, err);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::StopDTMFTone
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneStopDTMFTone, KErrNone);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MRDT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ReadDTMFTones
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ReadDTMFTones
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestReadDTMFTones0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	TRequestStatus reqStatus;
	TBuf<1> buf;
	iPhone.ReadDTMFTones(reqStatus, buf);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MSDT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendDTMFTones
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendDTMFTones
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestSendDTMFTones0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when there is no voice calls in the active and connected state
 	//-------------------------------------------------------------------------

	TInfoName tones(_L("*102#"));
	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, tones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrEtelCallNotActive, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	TMockLtsyData1<TInfoName> tonesLtsyData( tones );
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data, KErrNotSupported);

	iPhone.SendDTMFTones(reqStatus, tones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);
	iMockLTSY.CompleteL(EMobilePhoneSendDTMFTones, KErrGeneral);
	iPhone.SendDTMFTones(reqStatus, tones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SendDTMFTones when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);
	iMockLTSY.CompleteL(EMobilePhoneSendDTMFTones, KErrNone);
	iPhone.SendDTMFTones(reqStatus, tones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SendDTMFTones
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneSendDTMFTones, KErrNone);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MSDT-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SendDTMFTones
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SendDTMFTones
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestSendDTMFTones0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	//-------------------------------------------------------------------------
	// failure to dispatch cancelling request to LTSY
 	//-------------------------------------------------------------------------
 	
	TInfoName tones(_L("*102#"));
	TMockLtsyData1<TInfoName> tonesLtsyData( tones );
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);
	iMockLTSY.CompleteL(EMobilePhoneSendDTMFTones, KErrNone, 10);

	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, tones);

	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTonesCancel, KErrNotSupported);

	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);

	// request completes as there has not been the cancel
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// failure on completion cancelling request
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);

	iPhone.SendDTMFTones(reqStatus, tones);

	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTonesCancel, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneSendDTMFTonesCancel, KErrGeneral);
	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// successful cancelling
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);

	iPhone.SendDTMFTones(reqStatus, tones);

	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTonesCancel);
	iMockLTSY.CompleteL(EMobilePhoneSendDTMFTonesCancel, KErrNone);
	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);

	// the cancel was done
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Unsolicited completion of RMobilePhone::SendDTMFTones cancelling from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//send completion
	iMockLTSY.CompleteL(EMobilePhoneSendDTMFTonesCancel, KErrNone);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MSDT-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendDTMFTones with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendDTMFTones with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestSendDTMFTones0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::SendDTMFTones
 	//-------------------------------------------------------------------------

	// passing of empty DTMF tones
	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, KNullDesC);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();

	// passing of too long DTMF tones
	TBuf< KDtmfStringMaxLength + 1 > tooLongTones;
	tooLongTones.SetMax();
	iPhone.SendDTMFTones(reqStatus, tooLongTones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); // call,line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MSDT-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SendDTMFTones
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SendDTMFTones
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestSendDTMFTones0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line2;
	OpenLineLC(line2, KMmTsyVoice1LineName);

	RCall call2;
	OpenIncomingCallLC(line2, call2, 2, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SendDTMFTones
 	//-------------------------------------------------------------------------

	// setting and execute 1st request
	TInfoName tones(_L("*102#"));
	TMockLtsyData1<TInfoName> tonesLtsyData( tones );
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);
	iMockLTSY.CompleteL(EMobilePhoneSendDTMFTones, KErrNone);
	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, tones);

	// setting and execute 2nd request
	TBuf<10> tones2(_L("*105#"));
	TRequestStatus reqStatus2;
	phone2.SendDTMFTones(reqStatus2, tones2);

	// completion and check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); // call2, line2, phone2, telServer2, call, line, data, this

	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MSDT-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SendDTMFTones with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SendDTMFTones and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestSendDTMFTones0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SendDTMFTones
 	//-------------------------------------------------------------------------

	TInfoName tones(_L("*102#"));
	TMockLtsyData1<TInfoName> tonesLtsyData( tones );
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);

	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, tones);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Increase coverage: case cancelling
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);
	iPhone.SendDTMFTones(reqStatus, tones);

	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTonesCancel);
	iPhone.CancelAsyncRequest(EMobilePhoneSendDTMFTones);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // call, line, data, this

	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MSTDT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::StartDTMFTone
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::StartDTMFTone
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestStartDTMFTone0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	TChar tone('*');
	TMockLtsyData1<TChar> toneLtsyData( tone );
	toneLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneStartDTMFTone, data, KErrNotSupported);
	TInt err = iPhone.StartDTMFTone(tone);
	ASSERT_EQUALS(KErrNotSupported, err);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::StartDTMFTone when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneStartDTMFTone, data);
	err = iPhone.StartDTMFTone(tone);
	ASSERT_EQUALS(KErrNone, err);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MNDE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyDTMFEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyDTMFEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestNotifyDTMFEvent0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	RBuf8 data;
	
	RMobilePhone::TMobilePhoneDTMFEvent event(RMobilePhone::EStopDtmfTone);	
	RMobilePhone::TMobilePhoneDTMFEvent startEvent(RMobilePhone::EStartDtmfTone);	
	TRequestStatus reqStatus;
	
    TMockLtsyData1< RMobilePhone::TMobilePhoneDTMFEvent > toneEvent( startEvent );
    toneEvent.SerialiseL(data);

	//-------------------------------------------------------------------------
	// TEST A: Test API NotifyDTMFEvent
 	//-------------------------------------------------------------------------

	iPhone.NotifyDTMFEvent(reqStatus, event);
	iMockLTSY.CompleteL(EMobilePhoneNotifyDTMFEvent, KErrNone, data);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());  
	ASSERT_EQUALS(RMobilePhone::EStartDtmfTone, event);

	//-------------------------------------------------------------------------
	// TEST B: Test Cancelling a previous request NotifyDTMFEvent
 	//-------------------------------------------------------------------------
	iPhone.NotifyDTMFEvent(reqStatus, event);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyDTMFEvent);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());	
		
	//-------------------------------------------------------------------------
	// TEST C: Negative test NotifyDTMFEvent
	// Test when client has not requested to be notified.
	// Should not fall over.
 	//-------------------------------------------------------------------------
	
	iMockLTSY.CompleteL(EMobilePhoneNotifyDTMFEvent, KErrNone, data);

	CleanupStack::PopAndDestroy(3, this); // call, line, this	
	}


	
	
/**
@SYMTestCaseID BA-CTSY-DTMF-MNDCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyDTMFCapsChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyDTMFCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestNotifyDTMFCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyDTMFCapsChange when result is not cached.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TUint32 caps(0);
	iPhone.NotifyDTMFCapsChange(reqStatus, caps);

	// invoke call info change
	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	// wait for completion NotifyIndicatorChange request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	TUint32 expCaps = RMobilePhone::KCapsSendDTMFString | 
	                  RMobilePhone::KCapsSendDTMFSingleTone;
	ASSERT_EQUALS(expCaps, caps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MNDCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyDTMFCapsChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyDTMFCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestNotifyDTMFCapsChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyDTMFCapsChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus reqStatus;
	TUint32 caps(0);
	iPhone.NotifyDTMFCapsChange(reqStatus, caps);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyDTMFCapsChange);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MNDCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyDTMFCapsChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyDTMFCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestNotifyDTMFCapsChange0004L()
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

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyDTMFCapsChange
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TUint32 caps(0);
	iPhone.NotifyDTMFCapsChange(reqStatus, caps);

	TRequestStatus reqStatus2;
	TUint32 caps2(0);
	phone2.NotifyDTMFCapsChange(reqStatus2, caps2);

	// invoke call info change
	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	// wait for completion NotifyIndicatorChange request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	TUint32 expCaps = RMobilePhone::KCapsSendDTMFString | 
	                  RMobilePhone::KCapsSendDTMFSingleTone;
	ASSERT_EQUALS(expCaps, caps);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(expCaps, caps2);

	CleanupStack::PopAndDestroy(2); // line, call
	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MNSDS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyStopInDTMFString
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyStopInDTMFString
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestNotifyStopInDTMFString0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyStopInDTMFString when result is not cached.
 	//-------------------------------------------------------------------------

	// execute request
	TRequestStatus reqStatus;
	iPhone.NotifyStopInDTMFString(reqStatus);

	// send dtmf tones request and invoke notify complete
	TInfoName tones(_L("45w123"));
	TMockLtsyData1<TInfoName> tonesLtsyData( tones );
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);

	tones.Copy(_L("123"));
	data.Close();
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data);
	
	TRequestStatus sendDtmfStatus;
	tones.Copy(_L("45w123"));
	iPhone.SendDTMFTones(sendDtmfStatus, tones);

	// wait for completion notify request
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	// abortion of send Dtmf request
	TInt err = iPhone.ContinueDTMFStringSending(EFalse);
	ASSERT_EQUALS(KErrNone, err);
	
	User::WaitForRequest(sendDtmfStatus);
	ASSERT_EQUALS(KErrAbort, sendDtmfStatus.Int());

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyStopInDTMFString
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	tones.Copy(_L("456"));
	data.Close();
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MNSDS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyStopInDTMFString
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyStopInDTMFString
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestNotifyStopInDTMFString0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyStopInDTMFString
 	//-------------------------------------------------------------------------
 	
	// execute request
	TRequestStatus reqStatus;
	iPhone.NotifyStopInDTMFString(reqStatus);
	
	// send dtmf tones request
	TInfoName tones(_L("45w123"));
	TMockLtsyData1<TInfoName> tonesLtsyData( tones );
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);

	// notify completion
	tones.Copy(_L("123"));
	data.Close();
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data, 10);

	TRequestStatus sendDtmfStatus;
	tones.Copy(_L("45w123"));
	iPhone.SendDTMFTones(sendDtmfStatus, tones);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyStopInDTMFString);

	// wait for completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	User::WaitForRequest(sendDtmfStatus);
	ASSERT_EQUALS(KErrTimedOut, sendDtmfStatus.Int());

	CleanupStack::PopAndDestroy(4); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MNSDS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyStopInDTMFString
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyStopInDTMFString
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestNotifyStopInDTMFString0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyStopInDTMFString
 	//-------------------------------------------------------------------------

	// execute requests
	TRequestStatus reqStatus;
	iPhone.NotifyStopInDTMFString(reqStatus);
	
	TRequestStatus reqStatus2;
	phone2.NotifyStopInDTMFString(reqStatus2);
	
	// send dtmf tones request and invoke notify complete
	TInfoName tones(_L("45w123"));
	TMockLtsyData1<TInfoName> tonesLtsyData( tones );
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);

	tones.Copy(_L("123"));
	data.Close();
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data);

	TRequestStatus sendDtmfStatus;
	tones.Copy(_L("45w123"));
	iPhone.SendDTMFTones(sendDtmfStatus, tones);

	// wait for notify requests completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());

	// abortion of send Dtmf request
	TInt err = iPhone.ContinueDTMFStringSending(EFalse);
	ASSERT_EQUALS(KErrNone, err);
	
	User::WaitForRequest(sendDtmfStatus);
	ASSERT_EQUALS(KErrAbort, sendDtmfStatus.Int());

	CleanupStack::PopAndDestroy(6, this); // call, line, phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MCDSS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ContinueDTMFStringSending
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ContinueDTMFStringSending
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestContinueDTMFStringSending0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// notify stop in dtmf string request
	TRequestStatus mockLtsyStatus;
	iPhone.NotifyStopInDTMFString(mockLtsyStatus);

	// setting expected data for send dtmf tones request
	TInfoName tones(_L("45w123"));
	TMockLtsyData1<TInfoName> tonesLtsyData( tones );
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);

	// setting data for notify stop in dtmf string
	TInfoName stopTones(_L("123"));
	TMockLtsyData1<TInfoName> stopLtsyData( stopTones );
	data.Close();
	stopLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data);
	
	// send dtmf tones
	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, tones);

	// wait for stop in dtmf string
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// setting expected data for ContinueDTMFStringSending request
	data.Close();
	stopLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneContinueDTMFStringSending, data, KErrNotSupported);
	
	// ContinueDTMFStringSending request
	TInt err = iPhone.ContinueDTMFStringSending(ETrue);
	ASSERT_EQUALS(KErrNotSupported, err);

	// SendDTMFTones request completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::ContinueDTMFStringSending when result is not cached.
 	//-------------------------------------------------------------------------

	// notify stop in dtmf string request
	iPhone.NotifyStopInDTMFString(mockLtsyStatus);

	// send dtmf tones request
	data.Close();
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);
	data.Close();
	stopLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data);
	iPhone.SendDTMFTones(reqStatus, tones);

	// wait for stop in dtmf string
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ContinueDTMFStringSending request
	data.Close();
	stopLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneContinueDTMFStringSending, data);
	err = iPhone.ContinueDTMFStringSending(ETrue);
	ASSERT_EQUALS(KErrNone, err);

	// invoke SendDTMFTones request completion
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneSendDTMFTones, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// SendDTMFTones request completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: completion request when parameter is EFalse
 	//-------------------------------------------------------------------------

	// notify stop in dtmf string request
	iPhone.NotifyStopInDTMFString(mockLtsyStatus);

	// send dtmf tones request
	data.Close();
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);
	data.Close();
	stopLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data);
	iPhone.SendDTMFTones(reqStatus, tones);

	// wait for stop in dtmf string
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ContinueDTMFStringSending request
	err = iPhone.ContinueDTMFStringSending(EFalse);
	ASSERT_EQUALS(KErrNone, err);

	// SendDTMFTones request completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrAbort, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: completion request when continue tones is empty
 	//-------------------------------------------------------------------------

	// notify stop in dtmf string request
	iPhone.NotifyStopInDTMFString(mockLtsyStatus);

	// send dtmf tones request
	data.Close();
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);
	stopTones.Copy(KNullDesC);
	data.Close();
	stopLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data);
	iPhone.SendDTMFTones(reqStatus, tones);

	// wait for stop in dtmf string
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ContinueDTMFStringSending request
	err = iPhone.ContinueDTMFStringSending(ETrue);
	ASSERT_EQUALS(KErrNone, err);

	// SendDTMFTones request completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: completion request when continue tones is null
 	//-------------------------------------------------------------------------

	// notify stop in dtmf string request
	iPhone.NotifyStopInDTMFString(mockLtsyStatus);

	// send dtmf tones request
	data.Close();
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);
	// pack special constant _L("null")
	// for recognize null pointer packing test case in CMockPhoneMessHandler::Complete
	stopTones.Copy(_L("null"));
	data.Close();
	stopLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data);
	iPhone.SendDTMFTones(reqStatus, tones);

	// wait for stop in dtmf string
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// ContinueDTMFStringSending request
	err = iPhone.ContinueDTMFStringSending(ETrue);
	ASSERT_EQUALS(KErrNone, err);

	// SendDTMFTones request completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: completion request when DTMF sending not going on
 	//-------------------------------------------------------------------------

	// ContinueDTMFStringSending request
	err = iPhone.ContinueDTMFStringSending(ETrue);
	ASSERT_EQUALS(KErrNotReady, err);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // line, call, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DTMF-MCDSS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::ContinueDTMFStringSending with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ContinueDTMFStringSending and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDTMFFU::TestContinueDTMFStringSending0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);

	RCall call;
	OpenIncomingCallLC(line, call, 1, RMobilePhone::EVoiceService, RMobileCall::EStatusConnected);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::ContinueDTMFStringSending
 	//-------------------------------------------------------------------------

	// notify stop in dtmf string request
	TRequestStatus mockLtsyStatus;
	iPhone.NotifyStopInDTMFString(mockLtsyStatus);

	// setting expected data for send dtmf tones request
	TInfoName tones(_L("43w123"));
	TMockLtsyData1<TInfoName> tonesLtsyData( tones );
	tonesLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneSendDTMFTones, data);

	// setting data for notify stop in dtmf string
	TInfoName stopTones(_L("123"));
	TMockLtsyData1<TInfoName> stopLtsyData( stopTones );
	data.Close();
	stopLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyStopInDTMFString, KErrNone, data);
	
	// send dtmf tones
	TRequestStatus reqStatus;
	iPhone.SendDTMFTones(reqStatus, tones);

	// wait for stop in dtmf string
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// setting expected data for ContinueDTMFStringSending request
	data.Close();
	stopLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneContinueDTMFStringSending, data);
	
	// ContinueDTMFStringSending request
	TInt err = iPhone.ContinueDTMFStringSending(ETrue);
	ASSERT_EQUALS(KErrNone, err);

	// SendDTMFTones request completion
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // call, line, data, this

	}
