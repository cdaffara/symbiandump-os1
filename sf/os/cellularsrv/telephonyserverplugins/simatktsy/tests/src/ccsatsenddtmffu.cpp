// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file The TEFUnit test suite for SendDTMF in the SAT.
*/

#include "ccsatsenddtmffu.h"
#include <etel.h>
#include <etelmm.h>
#include <satcs.h>
#include <test/tmockltsydata.h>

// define used constants
const TUint8 KPCmdNumber = 1;
const TUint8 KSendDtmfQualifierNormal = 0x00;
const TUint8 KIcon = 1; // record 1 in EFIMG

_LIT8(KDtmf,"\xC1\xF2");
_LIT8(KSendDtmfAlpha, "Send DTMF");
_LIT8(KBasicIconAlpha, "Basic Icon");

// declare utility function
LOCAL_C void ResetSendDtmfData(RSat::TSendDtmfV1&);

CTestSuite* CCSatSendDTMFFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatSendDTMFFU, TestNotifySendDtmfPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendDTMFFU, TestNotifySendDtmfPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatSendDTMFFU, TestNotifySendDtmfPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendDTMFFU, TestNotifySendDtmfPCmd0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-SD-SNSDTPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySendDtmfPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendDtmfPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendDTMFFU::TestNotifySendDtmfPCmd0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifySendDtmfPCmd 
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.1(ETSI TS 131 124), SEND DTMF, normal
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	RSat::TSendDtmfRspV1 resp;
	resp.SetPCmdNumber(KPCmdNumber);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifySendDtmfPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------

	PrepareSendDtmfCompleteDataL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSendDtmf, KSendDtmfQualifierNormal,
			addInfo, RSat::KMeUnableToProcessCmd);

	WaitForMockLTSYTerminated();
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifySendDtmfPCmd
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.2(ETSI TS 131 124),
	// SEND DTMF, containing alpha identifier
 	//-------------------------------------------------------------------------

	_LIT8(KDtmf12, "\x21\x43\x65\x87\x09"); // "1234567890"
	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf12, &KSendDtmfAlpha);
	
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// Expected Sequence 1.3(ETSI TS 131 124),
	// SEND DTMF, containing alpha identifier with null data object
 	//-------------------------------------------------------------------------

	// "1" pause pause pause pause pause pause pause pause pause pause "2"
	_LIT8(KDtmf13, "\xC1\xCC\xCC\xCC\xCC\x2C");
	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf13, &KNullDesC8);
	
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// Expected Sequence 2.1A(ETSI TS 131 124),
	// SEND DTMF, BASIC ICON self explanatory, successful
 	//-------------------------------------------------------------------------

	RSat::TIconQualifier iconQual = RSat::ESelfExplanatory;
	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf, &KBasicIconAlpha,
			iconQual);
	
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// Expected Sequence 2.3A(ETSI TS 131 124), SEND DTMF,
	// Alpha identifier & BASIC-ICON, not self-explanatory, successful
 	//-------------------------------------------------------------------------

	iconQual = RSat::ENotSelfExplanatory;
	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf, &KSendDtmfAlpha,
			iconQual);
	
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// Expected Sequence 3.1(ETSI TS 131 124), SEND DTMF,
	// successful, UCS2 text in Cyrillic
 	//-------------------------------------------------------------------------

	//"Hello" in Russian, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
	_LIT8(KRussianHello, "\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\
\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15");
	//"Hello" in Russian in unicode for checking results
	_LIT16(KRussianHelloU, "\x0417\x0414\x0420\x0410\x0412\x0421\x0422\x0412\
\x0423\x0419\x0422\x0415");

	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf, &KRussianHello,
			RSat::EIconQualifierNotSet, KErrNone, &KRussianHelloU);
	
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 5.1(ETSI TS 131 124), SEND DTMF,
	// successful, UCS2 text in Chinese
 	//-------------------------------------------------------------------------

	//"Hello" in Chinese, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
	_LIT8(KChineseHello, "\x80\x4F\x60\x59\x7D");
	//"Hello" in Chinese in unicode for checking results
	_LIT16(KChineseHelloU, "\x4F60\x597D");

	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf, &KChineseHello,
			RSat::EIconQualifierNotSet, KErrNone, &KChineseHelloU);
	
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 6.1(ETSI TS 131 124), SEND DTMF,
	// successful, UCS2 text in Katakana
 	//-------------------------------------------------------------------------

	//Characters in Katakana, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
	_LIT8(KKatakanaChars, "\x80\x30\xEB");
	//Characters in Katakana in unicode for checking results
	_LIT16(KKatakanaCharsU, "\x30EB");

	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf, &KKatakanaChars,
			RSat::EIconQualifierNotSet, KErrNone, &KKatakanaCharsU);
	
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf,
			NULL, RSat::EIconQualifierNotSet,
			KErrUnknown);

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SD-SNSDTPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySendDtmfPCmd when problems in processing command
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendDtmfPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendDTMFFU::TestNotifySendDtmfPCmd0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 1.4(ETSI TS 131 124),
	// SEND DTMF, mobile is not in a speech call
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	RSat::TSendDtmfRspV1 resp;
	resp.SetPCmdNumber(KPCmdNumber);
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Append(RSat::KNotInSpeechCall);

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// Expected Sequence 2.1B(ETSI TS 131 124), SEND DTMF,
	// BASIC ICON self explanatory, requested icon could not be displayed
 	//-------------------------------------------------------------------------

	RSat::TIconQualifier iconQual = RSat::ESelfExplanatory;
	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf, &KBasicIconAlpha,
			iconQual);
	
	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// Expected Sequence 2.3B(ETSI TS 131 124), SEND DTMF,
	// Alpha identifier & BASIC-ICON, not self-explanatory, 
	// requested icon could not be displayed
 	//-------------------------------------------------------------------------

	iconQual = RSat::ENotSelfExplanatory;
	TestNotifySendDtmfPCmdL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf, &KSendDtmfAlpha,
			iconQual);
	
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// SEND DTMF, DTMF string length is more then its max length
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSendDtmf);

	// define very large dtmf
	// "1" pause pause "1" pause "1" pause "1" ... pause "1" pause "1"
	TBuf8< RSat::KDtmfStringMaxSize + 1 > veryLargeDtmf;
	veryLargeDtmf.SetMax();
	veryLargeDtmf.Fill(0x1C);
	veryLargeDtmf[0] = 0xC1;
	
	// set device id to KDeviceIdNotSet to skip Device identities in Tlv data
	// and make possible insertion of very large Dtmf in Tlv data
	PrepareSendDtmfCompleteDataL(
			KPCmdNumber, KSendDtmfQualifierNormal, &veryLargeDtmf,
			NULL, RSat::EIconQualifierNotSet, KErrNone, KDefaultNoDelay,
			KDeviceIdNotSet);
	
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSendDtmf, KSendDtmfQualifierNormal,
			KNullDesC, RSat::KCmdBeyondMeCapabilities);

	RSat::TSendDtmfV1 sendDtmfData;
	RSat::TSendDtmfV1Pckg sendDtmfDataPckg(sendDtmfData);
	TRequestStatus requestStatus;

	iSat.NotifySendDtmfPCmd(requestStatus, sendDtmfDataPckg);
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// SEND DTMF, missing DTMF string
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSendDtmf);

	PrepareSendDtmfCompleteDataL(KPCmdNumber, KSendDtmfQualifierNormal);
	
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSendDtmf, KSendDtmfQualifierNormal,
			KNullDesC, RSat::KErrorRequiredValuesMissing);

	iSat.NotifySendDtmfPCmd(requestStatus, sendDtmfDataPckg);
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// SEND DTMF, DTMF string is empty
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSendDtmf);

	PrepareSendDtmfCompleteDataL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KNullDesC8);
	
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSendDtmf, KSendDtmfQualifierNormal,
			KNullDesC, RSat::KCmdDataNotUnderstood);

	ResetSendDtmfData(sendDtmfData);
	iSat.NotifySendDtmfPCmd(requestStatus, sendDtmfDataPckg);
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// SEND DTMF, DTMF string is empty (first byte of the dtmf string is 0xFF)
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSendDtmf);

	_LIT8(KEmptyDtmf,"\xFF");
	PrepareSendDtmfCompleteDataL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KEmptyDtmf);
	
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSendDtmf, KSendDtmfQualifierNormal,
			KNullDesC, RSat::KCmdDataNotUnderstood);

	ResetSendDtmfData(sendDtmfData);
	iSat.NotifySendDtmfPCmd(requestStatus, sendDtmfDataPckg);
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// SEND DTMF, command type not understood
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KCmdTypeNotUnderstood;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// SEND DTMF, command data not understood
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KCmdDataNotUnderstood;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// SEND DTMF, command number not known
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KCmdNumberNotKnown;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// SEND DTMF, command beyond me capabilities
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KCmdBeyondMeCapabilities;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// SEND DTMF, partial comprehension
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KPartialComprehension;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// SEND DTMF, missing information
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KMissingInformation;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// SEND DTMF, proactive session terminated by user
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KPSessionTerminatedByUser;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// SEND DTMF, required values missing
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KErrorRequiredValuesMissing;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

 	//-------------------------------------------------------------------------
	// SEND DTMF, Me unable to process commandd, empty additional info
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iInfoType = RSat::KMeProblem;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal, KErrCorrupt );

 	//-------------------------------------------------------------------------
	// SEND DTMF, invalid error code
 	//-------------------------------------------------------------------------

	TestNotifySendDtmfPCmdL(KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf);
	
	resp.iGeneralResult = RSat::KLaunchBrowserError; // invalid for this command

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal, KErrCorrupt );

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SD-SNSDTPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySendDtmfPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySendDtmfPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendDTMFFU::TestNotifySendDtmfPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifySendDtmfPCmd
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSendDtmf);
	
	// delay for completion is used to cancel
	PrepareSendDtmfCompleteDataL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf,
			NULL, RSat::EIconQualifierNotSet,
			KErrNone, KDefaultDelay);
	
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSendDtmf, KSendDtmfQualifierNormal,
			addInfo, RSat::KMeUnableToProcessCmd);

	RSat::TSendDtmfV1 sendDtmfData;
	
	RSat::TSendDtmfV1Pckg sendDtmfDataPckg(sendDtmfData);
	TRequestStatus requestStatus;
	
	iSat.NotifySendDtmfPCmd(requestStatus, sendDtmfDataPckg);
	
	iSat.CancelAsyncRequest(ESatNotifySendDtmfPCmd);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SD-SNSDTPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySendDtmfPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySendDtmfPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendDTMFFU::TestNotifySendDtmfPCmd0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RSat sat2;
	ret = sat2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(sat2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifySendDtmfPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSendDtmf);

	// delay for completion is used for make the 2nd request before
	// the completion arrives to CSATTSY
	PrepareSendDtmfCompleteDataL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf,
			NULL, RSat::EIconQualifierNotSet,
			KErrNone, KDefaultDelay);
	
	RSat::TSendDtmfV1 sendDtmfData;
	RSat::TSendDtmfV1 sendDtmfData2;
	RSat::TSendDtmfV1Pckg sendDtmfDataPckg(sendDtmfData);
	RSat::TSendDtmfV1Pckg sendDtmfData2Pckg(sendDtmfData2);
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	iSat.NotifySendDtmfPCmd(requestStatus, sendDtmfDataPckg);
	sat2.NotifySendDtmfPCmd(requestStatus2, sendDtmfData2Pckg);
	
	User::WaitForRequest(requestStatus);
	User::WaitForRequest(requestStatus2);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS( KPCmdNumber, sendDtmfData.PCmdNumber() );
	ASSERT_EQUALS( 0, KDtmf().Compare(sendDtmfData.iDtmfString) );
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendDtmfData.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendDtmfData.iAlphaId.iStatus) );
	ASSERT_EQUALS( RSat::ENoIconId, sendDtmfData.iIconId.iQualifier );

	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS( KPCmdNumber, sendDtmfData2.PCmdNumber() );
	ASSERT_EQUALS( 0, KDtmf().Compare(sendDtmfData2.iDtmfString) );
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendDtmfData2.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendDtmfData2.iAlphaId.iStatus) );
	ASSERT_EQUALS( RSat::ENoIconId, sendDtmfData2.iIconId.iQualifier );

	AssertMockLtsyStatusL();

	RSat::TSendDtmfRspV1 resp;
	resp.SetPCmdNumber(KPCmdNumber);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );
	
	//-------------------------------------------------------------------------
	// Test B: Test multiple clients requesting RSat::NotifySendDtmfPCmd
	// when they both pass different T-class versions
 	//-------------------------------------------------------------------------	

	PrepareMockWithExpDataForPCmdNotifyL(KSendDtmf);

	// delay for completion is used for make the 2nd request before
	// the completion arrives to CSATTSY
	PrepareSendDtmfCompleteDataL(
			KPCmdNumber, KSendDtmfQualifierNormal, &KDtmf,
			NULL, RSat::EIconQualifierNotSet,
			KErrNone, KDefaultDelay);
	
	ResetSendDtmfData(sendDtmfData);
	RSat::TSendDtmfV5 sendDtmfDataV5;
	RSat::TSendDtmfV5Pckg sendDtmfDataV5Pckg(sendDtmfDataV5);

	iSat.NotifySendDtmfPCmd(requestStatus, sendDtmfDataPckg);
	sat2.NotifySendDtmfPCmd(requestStatus2, sendDtmfDataV5Pckg);
	
	// When multiple clients requesting RSat::NotifySendDtmfPCmd and they both pass
	// different packaged parameter versions only the last request completes, because
	// etel reposts request with new parameter size and looses previous requests.
	// May be it would be worth to make ESatNotifySendDtmfPCmd ipc requests flow controlled
	// by CSATTSY instead of multiple completed by ETEL. Similar defects are 51, 58 and 71
	ERR_PRINTF2(_L("<font color=Orange>$CSATKnownFailure: defect id = %d</font>"), 78);
	// assert below added to avoid the test hang here
	ASSERT_TRUE( EFalse );

	User::WaitForRequest(requestStatus2);
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS( KPCmdNumber, sendDtmfData.PCmdNumber() );
	ASSERT_EQUALS( 0, KDtmf().Compare(sendDtmfData.iDtmfString) );
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendDtmfData.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendDtmfData.iAlphaId.iStatus) );
	ASSERT_EQUALS( RSat::ENoIconId, sendDtmfData.iIconId.iQualifier );

	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS( KPCmdNumber, sendDtmfDataV5.PCmdNumber() );
	ASSERT_EQUALS( 0, KDtmf().Compare(sendDtmfDataV5.iDtmfString) );
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendDtmfDataV5.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendDtmfDataV5.iAlphaId.iStatus) );
	ASSERT_EQUALS( RSat::ENoIconId, sendDtmfDataV5.iIconId.iQualifier );
	ASSERT_TRUE( RSat::ETextAttributeProvided !=
										sendDtmfDataV5.iTextAttribute.iStatus );

	AssertMockLtsyStatusL();

	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );
	GenerateSendDtmfTerminalResponseL( resp, KSendDtmfQualifierNormal );

	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}


/**
Resets RSat::TSendDtmfV1 data fields
@param aSendDtmfData data to be reset
*/
LOCAL_C void ResetSendDtmfData(RSat::TSendDtmfV1& aSendDtmfData)
	{
	// reset public members
	aSendDtmfData.iAlphaId.iStatus = RSat::EAlphaIdNotSet;
	aSendDtmfData.iAlphaId.iAlphaId.Zero();
	aSendDtmfData.iDtmfString.Zero();
	aSendDtmfData.iIconId.iQualifier = RSat::EIconQualifierNotSet;
	aSendDtmfData.iIconId.iIdentifier = 0;

	// reset protected members;
	aSendDtmfData.SetPCmdNumber(0);
	}


/**
Prepares SendDtmf proactive command complete data for Mock
@param aPCmdNumber Proactive command number.
@param aCmdQual Command qualifier
@param aDtmf Dtmf string
@param aAlphaId Alpha identifier
@param aIconQual Type of icon
@param aResult Proactive command notify result
@param aDelay Delay value for MockLtsy completion
@param aDestDeviceId command destination device Id
*/
void CCSatSendDTMFFU::PrepareSendDtmfCompleteDataL(
		TUint8 aPCmdNumber,
		TUint8 aCmdQual,
		const TDesC8* aDtmf,
		const TDesC8* aAlphaId,
		RSat::TIconQualifier aIconQual,
		TInt aResult,
		TInt aDelay,
		TUint8 aDestDeviceId)
	{
	TTlv tlv;
	ProactiveCmdTlvBegin(tlv, aPCmdNumber, KSendDtmf, aCmdQual, aDestDeviceId);

	// add alphaId tag if alphaId string present
	if ( aAlphaId )
		{			
		tlv.AddTag(KTlvAlphaIdentifierTag);
		tlv.AddData(*aAlphaId);//ETLV_AlphaIdentifier
		}

	// add dtmf tag if dtmf string present
	if ( aDtmf )
		{
		tlv.AddTag(KTlvDtmfStringTag);
		tlv.AddData(*aDtmf);//ETLV_DtmfString
		}

	// For RSat::EIconQualifierNotSet and RSat::ENoIconId does not add tag
	if ( ( RSat::ESelfExplanatory == aIconQual ) ||
			( RSat::ENotSelfExplanatory == aIconQual ) )
		{
		tlv.AddTag(KTlvIconIdentifierTag);
		tlv.AddByte((RSat::ESelfExplanatory == aIconQual)?0:1);//ETLV_IconQualifier
		tlv.AddByte(KIcon);//ETLV_IconIdentifier
		}

	PrepareMockWithCompleteDataForPCmdNotifyL(tlv.End(), aResult, aDelay);
	}


/**
Prepares and calls SendDtmf proactive command
@param aPCmdNumber Proactive command number
@param aCmdQual Command qualifier
@param aDtmf Dtmf string
@param aAlphaId Alpha identifier
@param aIconQual Type of icon
@param aResult Proactive command notify result
@param aAlphaIdCheck Alpha identifier value to ckeck with returned value
*/
void CCSatSendDTMFFU::TestNotifySendDtmfPCmdL(
		TUint8 aPCmdNumber,
		TUint8 aCmdQual,
		const TDesC8* aDtmf,
		const TDesC8* aAlphaId,
		RSat::TIconQualifier aIconQual,
		TInt aResult,
		const TDesC16* aAlphaIdCheck)
	{
	PrepareMockWithExpDataForPCmdNotifyL(KSendDtmf);
	
	PrepareSendDtmfCompleteDataL(
			aPCmdNumber, aCmdQual, aDtmf,
			aAlphaId, aIconQual, aResult);
	
	RSat::TSendDtmfV1 sendDtmfData;

	TRequestStatus requestStatus;
	RSat::TSendDtmfV1Pckg sendDtmfDataPckg(sendDtmfData);
	iSat.NotifySendDtmfPCmd(requestStatus, sendDtmfDataPckg);
	
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(aResult, requestStatus.Int());

	// check results
	if ( KErrNone == aResult )
		{
		ASSERT_EQUALS(aPCmdNumber, sendDtmfData.PCmdNumber());
		
		if ( aAlphaId && ( 0 < aAlphaId->Length() ) )
			{
			ASSERT_EQUALS(RSat::EAlphaIdProvided, sendDtmfData.iAlphaId.iStatus);
			if ( aAlphaIdCheck )
				{
				ASSERT_EQUALS( 0, aAlphaIdCheck->Compare(
										sendDtmfData.iAlphaId.iAlphaId));
				}
			else
				{
				ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(
												*aAlphaId,
												sendDtmfData.iAlphaId.iAlphaId));
				}
			}
		else if ( aAlphaId && ( 0 == aAlphaId->Length() ) )
			{
			ASSERT_EQUALS( RSat::EAlphaIdNull, sendDtmfData.iAlphaId.iStatus );
			}
		else
			{
			ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendDtmfData.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendDtmfData.iAlphaId.iStatus) );
			}

		if ( aDtmf )
			{
			ASSERT_EQUALS( 0, aDtmf->Compare(sendDtmfData.iDtmfString) );
			}
		else
			{
			ASSERT_EQUALS( 0, KNullDesC8().Compare(sendDtmfData.iDtmfString) );
			}

		if (RSat::EIconQualifierNotSet != aIconQual)
			{
			ASSERT_EQUALS(aIconQual, sendDtmfData.iIconId.iQualifier);
			ASSERT_EQUALS(KIcon, sendDtmfData.iIconId.iIdentifier);
			}
		else
			{
			ASSERT_EQUALS(RSat::ENoIconId, sendDtmfData.iIconId.iQualifier);
			}
		}

	AssertMockLtsyStatusL();
	}


/**
Handles sending a terminal response
@param aResp terminal response data
@param aCmdQual Command qualifier
@param aExpResult Expected result of TerminalRsp() request
*/
void CCSatSendDTMFFU::GenerateSendDtmfTerminalResponseL(
		const RSat::TSendDtmfRspV1& aResp,
		TUint8 aCmdQual,
		TInt aExpResult)
	{

	TPtrC genResultAddInfo(KNullDesC);
	if ( RSat::KMeProblem == aResp.iInfoType )
		{
		genResultAddInfo.Set( aResp.iAdditionalInfo );
		}

	RSat::RSat::TSendDtmfRspV1Pckg respPckg(aResp);
	GenerateTerminalResponseL(
			aResp.PCmdNumber(),
			KSendDtmf,
			aCmdQual,
			RSat::ESendDtmf,
			respPckg,
			genResultAddInfo,
			aResp.iGeneralResult,
			KNullDesC8,
			aExpResult);

	AssertMockLtsyStatusL();
	}


