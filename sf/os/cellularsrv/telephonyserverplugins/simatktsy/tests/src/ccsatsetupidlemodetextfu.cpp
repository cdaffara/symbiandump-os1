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
 @file The TEFUnit test suite for SetUpIdleModeText in the SAT.
*/

#include "ccsatsetupidlemodetextfu.h"
#include <etel.h>
#include <etelmm.h>
#include <satcs.h>
#include <test/tmockltsydata.h>

const TUint8 KPCmdNumber = 1;
const TUint8 KSetUpIdleModeTextPCmdQualifier = 0x00; // always zero, reserved for future use
const TUint8 KIcon = 1; // record 1 in EFIMG
LOCAL_C void ResetSetUpIdleModeTextData(RSat::TSetUpIdleModeTextV1&);

CTestSuite* CCSatSetUpIdleModeTextFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatSetUpIdleModeTextFU, TestNotifySetUpIdleModeTextPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpIdleModeTextFU, TestNotifySetUpIdleModeTextPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpIdleModeTextFU, TestNotifySetUpIdleModeTextPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatSetUpIdleModeTextFU, TestNotifySetUpIdleModeTextPCmd0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-SUIMT-SNSUIMPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpIdleModeTextPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpIdleModeTextPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpIdleModeTextFU::TestNotifySetUpIdleModeTextPCmd0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifySetUpIdleModeTextPCmd 
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.1(ETSI TS 102 384), display idle mode text
 	//-------------------------------------------------------------------------

	_LIT8(KIdleModeText, "Idle Mode Text");
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	RSat::TSetUpIdleModeTextRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.SetPCmdNumber(KPCmdNumber);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifySetUpIdleModeTextPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------

	PrepareSetUpIdleModeTextCompleteDataL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSetUpIdleModeText, KSetUpIdleModeTextPCmdQualifier,
			addInfo, RSat::KMeUnableToProcessCmd);

	WaitForMockLTSYTerminated();
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifySetUpIdleModeTextPCmd
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.2(ETSI TS 102 384), replace idle mode text
 	//-------------------------------------------------------------------------

	// 1.2.1
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
	// 1.2.2
	_LIT8(KIdleModeText12, "Toolkit Test");
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText12);

	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.3(ETSI TS 102 384), remove idle mode text
 	//-------------------------------------------------------------------------

	// 1.3.1
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
	// 1.3.2
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::ERemoveExistingIdleModeText);

	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.7(ETSI TS 102 384), large text string
 	//-------------------------------------------------------------------------

	_LIT16(KIdleModeText17u, "The SIM shall supply a text string, which shall\
 be displayed by the ME as an idle mode text if the ME is able to do it.The\
 presentation style is left as an implementation decision to the ME\
 manufacturer. The idle mode text shall be displayed in a manner that ensures\
 that n");

	TBuf8<RSat::KIdleModeTextMaxSize> packed7BitText;
	ASSERT_EQUALS(KErrNone, UnicodeToPacked7L(KIdleModeText17u, packed7BitText));

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, packed7BitText, K7BitDefaultSmsDCS,
			RSat::EIconQualifierNotSet, 0, KErrNone, &KIdleModeText17u);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 2.1A(ETSI TS 102 384),
	// Icon is self-explanatory, successful
 	//-------------------------------------------------------------------------

	_LIT8(KIdleModeText2, "Idle Text");
	RSat::TIconQualifier iconQual = RSat::ESelfExplanatory;
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText2, K8BitDCS,
			iconQual, KIcon);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 2.2A(ETSI TS 102 384),
	// Icon is not self-explanatory, successful
 	//-------------------------------------------------------------------------

	iconQual = RSat::ENotSelfExplanatory;
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText, K8BitDCS,
			iconQual, KIcon);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 3.1(ETSI TS 102 384), UCS2 alphabet text in Cyrillic
 	//-------------------------------------------------------------------------

	// "Hello" in Russian
	_LIT8(KRussianHello, "\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\
\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15");
	// "Hello" in Russian in unicode for checking results
	_LIT16(KRussianHelloU, "\x0417\x0414\x0420\x0410\x0412\x0421\
\x0422\x0412\x0423\x0419\x0422\x0415");

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KRussianHello, KUCS2DCS,
			RSat::EIconQualifierNotSet, 0, KErrNone, &KRussianHelloU);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 5.1(ETSI TS 102 384), UCS2 alphabet text in Chinese
 	//-------------------------------------------------------------------------

	// "Hello" in Chinese
	_LIT8(KChineseHello, "\x4F\x60\x59\x7D");
	// "Hello" in Chinese in unicode for checking results
	_LIT16(KChineseHelloU, "\x4F60\x597D");

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KChineseHello, KUCS2DCS,
			RSat::EIconQualifierNotSet, 0, KErrNone, &KChineseHelloU);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 6.1(ETSI TS 102 384), UCS2 alphabet text in Katakana
 	//-------------------------------------------------------------------------

	// Characters in Katakana
	_LIT8(KKatakanaChars, "\x00\x38\x00\x30\x30\xEB\x00\x30");
	// Characters in Katakana in unicode for checking results
	_LIT16(KKatakanaCharsU, "\x0038\x0030\x30EB\x0030");

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KKatakanaChars, KUCS2DCS,
			RSat::EIconQualifierNotSet, 0, KErrNone, &KKatakanaCharsU);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText, K8BitDCS,
			RSat::EIconQualifierNotSet, 0, KErrUnknown);

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SUIMT-SNSUIMPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySetUpIdleModeTextPCmd when problems in processing command
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySetUpIdleModeTextPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpIdleModeTextFU::TestNotifySetUpIdleModeTextPCmd0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// missing text tag
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSetUpIdleModeText);
	
	PrepareSetUpIdleModeTextCompleteDataL( KPCmdNumber );
	
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSetUpIdleModeText, KSetUpIdleModeTextPCmdQualifier,
			KNullDesC, RSat::KCmdDataNotUnderstood);
	
	RSat::TSetUpIdleModeTextV1 setUpIdleModeTextData;
	RSat::TSetUpIdleModeTextV1Pckg setUpIdleModeTextDataPckg(setUpIdleModeTextData);
	TRequestStatus requestStatus;
	iSat.NotifySetUpIdleModeTextPCmd(requestStatus, setUpIdleModeTextDataPckg);
	
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// command beyond me capabilities
 	//-------------------------------------------------------------------------

	_LIT8(KIdleModeText, "Idle Mode Text");
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	RSat::TSetUpIdleModeTextRspV1 resp;
	resp.iGeneralResult = RSat::KCmdBeyondMeCapabilities;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.SetPCmdNumber(KPCmdNumber);

	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// generic mobile equipment(phone) problem in processing command
 	//-------------------------------------------------------------------------

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iInfoType = RSat::KMeProblem;
	// No specific cause can be given.
	resp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// me problem without additional info
 	//-------------------------------------------------------------------------

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	resp.iAdditionalInfo.Zero();
	GenerateSetUpIdleModeTextTerminalResponseL( resp, KErrCorrupt );
	
 	//-------------------------------------------------------------------------
	// proactive session terminated by user
 	//-------------------------------------------------------------------------

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	resp.iGeneralResult = RSat::KPSessionTerminatedByUser;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// backward mode requested by user
 	//-------------------------------------------------------------------------

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	resp.iGeneralResult = RSat::KBackwardModeRequestedByUser;
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// no response from user
 	//-------------------------------------------------------------------------

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	resp.iGeneralResult = RSat::KNoResponseFromUser;
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// error required values missing
 	//-------------------------------------------------------------------------

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	resp.iGeneralResult = RSat::KErrorRequiredValuesMissing;
	GenerateSetUpIdleModeTextTerminalResponseL( resp, KErrCorrupt );
	
 	//-------------------------------------------------------------------------
	// help requested by user
 	//-------------------------------------------------------------------------

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	resp.iGeneralResult = RSat::KHelpRequestedByUser;
	GenerateSetUpIdleModeTextTerminalResponseL( resp, KErrCorrupt );
	
 	//-------------------------------------------------------------------------
	// command data not understood 
 	//-------------------------------------------------------------------------

	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText);
	
	resp.iGeneralResult = RSat::KCmdDataNotUnderstood;
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
	//-------------------------------------------------------------------------
	// Expected Sequence 2.1B(ETSI TS 102 384),
	// Icon is self-explanatory, requested icon could not be displayed
 	//-------------------------------------------------------------------------

	RSat::TIconQualifier iconQual = RSat::ESelfExplanatory;
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText, K8BitDCS,
			iconQual, KIcon);
	
	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;

	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 2.2B(ETSI TS 102 384),
	// Icon is not self-explanatory, requested icon could not be displayed
 	//-------------------------------------------------------------------------

	iconQual = RSat::ENotSelfExplanatory;
	TestNotifySetUpIdleModeTextPCmdL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText, K8BitDCS,
			iconQual, KIcon);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// Expected Sequence 2.4(ETSI TS 102 384),
	// Icon is not self-explanatory, null text data object
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSetUpIdleModeText);
	
	PrepareSetUpIdleModeTextCompleteDataL(
			KPCmdNumber, RSat::ERemoveExistingIdleModeText, KNullDesC8, K8BitDCS,
			iconQual, KIcon);
	
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSetUpIdleModeText, KSetUpIdleModeTextPCmdQualifier,
			KNullDesC, RSat::KCmdDataNotUnderstood);
	
	ResetSetUpIdleModeTextData(setUpIdleModeTextData);
	iSat.NotifySetUpIdleModeTextPCmd(requestStatus, setUpIdleModeTextDataPckg);
	
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SUIMT-SNSUIMPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySetUpIdleModeTextPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySetUpIdleModeTextPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpIdleModeTextFU::TestNotifySetUpIdleModeTextPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifySelectItemPCmd
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KSetUpIdleModeText);
	
	_LIT8(KIdleModeText, "Idle Mode Text");

	PrepareSetUpIdleModeTextCompleteDataL(
			KPCmdNumber, RSat::EUpdateIdleModeText, KIdleModeText, K8BitDCS,
			RSat::EIconQualifierNotSet, 0, KErrNone, 10);
	
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(
			KPCmdNumber, KSetUpIdleModeText, KSetUpIdleModeTextPCmdQualifier,
			addInfo, RSat::KMeUnableToProcessCmd);

	RSat::TSetUpIdleModeTextV1 setUpIdleModeTextData;
	RSat::TSetUpIdleModeTextV1Pckg setUpIdleModeTextDataPckg(
			setUpIdleModeTextData);

	TRequestStatus requestStatus;
	iSat.NotifySetUpIdleModeTextPCmd(
			requestStatus,
			setUpIdleModeTextDataPckg);
	
	iSat.CancelAsyncRequest(ESatNotifySetUpIdleModeTextPCmd);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SUIMT-SNSUIMPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySetUpIdleModeTextPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySetUpIdleModeTextPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSetUpIdleModeTextFU::TestNotifySetUpIdleModeTextPCmd0004L()
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
	// Test A: Test multiple clients requesting RSat::NotifySetUpIdleModeTextPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	// prepare mock data
	RSat::TSetUpIdleModeTextType type = RSat::EUpdateIdleModeText;
	_LIT8(KIdleModeText, "Idle Mode Text");
	
	PrepareMockWithExpDataForPCmdNotifyL(KSetUpIdleModeText);
	PrepareSetUpIdleModeTextCompleteDataL(
			KPCmdNumber, type, KIdleModeText, K8BitDCS,
			RSat::EIconQualifierNotSet, 0, KErrNone, 10);
	
	// send requests
	RSat::TSetUpIdleModeTextV1 setUpIdleModeTextData;
	RSat::TSetUpIdleModeTextV1 setUpIdleModeTextData2;
	RSat::TSetUpIdleModeTextV1Pckg setUpIdleModeTextDataPckg(
			setUpIdleModeTextData);
	RSat::TSetUpIdleModeTextV1Pckg setUpIdleModeTextDataPckg2(
			setUpIdleModeTextData2);
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	iSat.NotifySetUpIdleModeTextPCmd(
			requestStatus,
			setUpIdleModeTextDataPckg);
	sat2.NotifySetUpIdleModeTextPCmd(
			requestStatus2,
			setUpIdleModeTextDataPckg2);
	
	// wait for completion
	User::WaitForRequest(requestStatus);
	User::WaitForRequest(requestStatus2);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(KPCmdNumber, setUpIdleModeTextData.PCmdNumber());
	ASSERT_EQUALS(RSat::ENoIconId, setUpIdleModeTextData.iIconId.iQualifier);
	ASSERT_EQUALS(type, setUpIdleModeTextData.iType);
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(
							KIdleModeText,
							setUpIdleModeTextData.iText));

	ASSERT_EQUALS(KPCmdNumber, setUpIdleModeTextData2.PCmdNumber());
	ASSERT_EQUALS(RSat::ENoIconId, setUpIdleModeTextData2.iIconId.iQualifier);
	ASSERT_EQUALS(type, setUpIdleModeTextData2.iType);
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(
							KIdleModeText,
							setUpIdleModeTextData2.iText));

	AssertMockLtsyStatusL();
	
	// generate terminal response
	RSat::TSetUpIdleModeTextRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.SetPCmdNumber(KPCmdNumber);
	
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	
	//-------------------------------------------------------------------------
	// Test B: Test multiple clients requesting RSat::NotifySetUpIdleModeTextPCmd
	// when they both pass different T-class versions
 	//-------------------------------------------------------------------------	

	// When multiple clients requesting RSat::NotifySetUpIdleModeTextPCmd and
	// they both pass different packaged parameter versions only the last
	// request completes, because etel reposts request with new parameter size
	// and looses previous requests. May be it would be worth to make
	// ESatNotifySetUpIdleModeTextPCmd ipc requests flow controlled by CSATTSY
	// instead of multiple completed by ETEL. Similar defects are 51 and 58
	ERR_PRINTF2(_L("<font color=Orange>$CSATKnownFailure: defect id = %d</font>"), 71);
	// Following assert was added to prevent test execution hang
	ASSERT_TRUE( EFalse );

	PrepareMockWithExpDataForPCmdNotifyL(KSetUpIdleModeText);
	PrepareSetUpIdleModeTextCompleteDataL(
			KPCmdNumber, type, KIdleModeText, K8BitDCS,
			RSat::EIconQualifierNotSet, 0, KErrNone, 10);
	
	// send requests
	ResetSetUpIdleModeTextData(setUpIdleModeTextData);

	RSat::TSetUpIdleModeTextV5 setUpIdleModeTextDataV5;
	RSat::TSetUpIdleModeTextV5Pckg setUpIdleModeTextDataV5Pckg(
			setUpIdleModeTextDataV5);

	iSat.NotifySetUpIdleModeTextPCmd(
			requestStatus,
			setUpIdleModeTextDataPckg);
	sat2.NotifySetUpIdleModeTextPCmd(
			requestStatus2,
			setUpIdleModeTextDataV5Pckg);
	
	// wait for completion
	User::WaitForRequest(requestStatus2);
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(KPCmdNumber, setUpIdleModeTextData.PCmdNumber());
	ASSERT_EQUALS(RSat::ENoIconId, setUpIdleModeTextData.iIconId.iQualifier);
	ASSERT_EQUALS(type, setUpIdleModeTextData.iType);
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(
							KIdleModeText,
							setUpIdleModeTextData.iText));

	ASSERT_EQUALS(KPCmdNumber, setUpIdleModeTextDataV5.PCmdNumber());
	ASSERT_EQUALS(RSat::ENoIconId, setUpIdleModeTextDataV5.iIconId.iQualifier);
	ASSERT_EQUALS(type, setUpIdleModeTextDataV5.iType);
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(
							KIdleModeText,
							setUpIdleModeTextDataV5.iText));
	ASSERT_EQUALS( static_cast<TBool>(ETrue), 
			static_cast<TBool>( RSat::ETextAttributeProvided !=
					setUpIdleModeTextDataV5.iTextAttribute.iStatus ) );

	AssertMockLtsyStatusL();
	
	// generate terminal response
	GenerateSetUpIdleModeTextTerminalResponseL( resp );
	GenerateSetUpIdleModeTextTerminalResponseL( resp );

	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}


/**
Resets RSat::TSetUpIdleModeTextV1 data fields
@param aSetUpIdleModeTextData data to be reset
*/
LOCAL_C void ResetSetUpIdleModeTextData(RSat::TSetUpIdleModeTextV1& aSetUpIdleModeTextData)
	{
	// reset public members
	aSetUpIdleModeTextData.iType = RSat::ESetUpIdleModeTextTypeNotSet;
	aSetUpIdleModeTextData.iText.SetLength(0);
	aSetUpIdleModeTextData.iIconId.iQualifier = RSat::EIconQualifierNotSet;
	aSetUpIdleModeTextData.iIconId.iIdentifier = 0;
	aSetUpIdleModeTextData.iCodingScheme = RSat::E7bitPacked;

	// reset protected members;
	aSetUpIdleModeTextData.SetPCmdNumber(0);
	}


/**
Prepares SetUpIdleModeText proactive command complete data for Mock
@param aPCmdNumber Proactive command number.
@param aType type of command
@param aText Text to display
@param aCodingScheme Coding scheme
@param aIconQual Type of icon
@param aIcon Icon address
@param aResult Proactive command notify result
@param aDelay Delay value for MockLtsy completion
*/
void CCSatSetUpIdleModeTextFU::PrepareSetUpIdleModeTextCompleteDataL(
		TUint8 aPCmdNumber,
		RSat::TSetUpIdleModeTextType aType,
		const TDesC8& aText,
		TUint8 aCodingScheme,
		RSat::TIconQualifier aIconQual,
		TUint8 aIcon,
		TInt aResult,
		TInt aDelay)
	{
	TTlv tlv;
	
	// The destination device for the proactive command is the Mobile Equipment
	ProactiveCmdTlvBegin(
			tlv,
			aPCmdNumber,
			KSetUpIdleModeText,
			KSetUpIdleModeTextPCmdQualifier,
			RSat::KME );

	if (RSat::EUpdateIdleModeText == aType)
		{			
		tlv.AddTag(KTlvTextStringTag);
		tlv.AddByte(aCodingScheme); //ETLV_DataCodingScheme
		tlv.AddData(aText);
		}
	else if (RSat::ERemoveExistingIdleModeText == aType)
		{
		tlv.AddTag(KTlvTextStringTag);
		}
	else
		{
		// add nothing to Tlv
		}

	if(RSat::EIconQualifierNotSet != aIconQual)
		{
		tlv.AddTag(KTlvIconIdentifierTag);
		// in accordance with ETSI TS 102 223 bit 1 of icon qualifier is
		// coded by 0 when icon is self-explanatory, otherwise by 1
		// bits 2 to 8 are RFU
		tlv.AddByte((RSat::ESelfExplanatory == aIconQual)?0:1); // ETLV_IconQualifier
		tlv.AddByte(aIcon); // ETLV_IconIdentifier
		}

	PrepareMockWithCompleteDataForPCmdNotifyL(tlv.End(), aResult, aDelay);
	}


/**
Prepares and calls SetUpIdleModeText proactive command
@param aPCmdNumber Proactive command number
@param aType type of command
@param aText Text to display
@param aCodingScheme Coding scheme
@param aIconQual Type of icon
@param aIcon Icon address
@param aResult Proactive command notify result
@param aTextCheck Text value to ckeck with returned value
*/
void CCSatSetUpIdleModeTextFU::TestNotifySetUpIdleModeTextPCmdL(
		TUint8 aPCmdNumber,
		RSat::TSetUpIdleModeTextType aType,
		const TDesC8& aText,
		TUint8 aCodingScheme,
		RSat::TIconQualifier aIconQual,
		TUint8 aIcon,
		TInt aResult,
		const TDesC16* aTextCheck)
	{
	PrepareMockWithExpDataForPCmdNotifyL(KSetUpIdleModeText);
	
	PrepareSetUpIdleModeTextCompleteDataL(
			aPCmdNumber, aType, aText, aCodingScheme, aIconQual, aIcon, aResult);
	
	RSat::TSetUpIdleModeTextV1 setUpIdleModeTextData;
	RSat::TSetUpIdleModeTextV1Pckg setUpIdleModeTextDataPckg(setUpIdleModeTextData);
	TRequestStatus requestStatus;
	iSat.NotifySetUpIdleModeTextPCmd(requestStatus, setUpIdleModeTextDataPckg);
	
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(aResult, requestStatus.Int());

	// check results
	if ( KErrNone == aResult )
		{
		ASSERT_EQUALS(aPCmdNumber, setUpIdleModeTextData.PCmdNumber());
		
		if (RSat::EIconQualifierNotSet != aIconQual)
			{
			ASSERT_EQUALS(aIconQual, setUpIdleModeTextData.iIconId.iQualifier);
			ASSERT_EQUALS(aIcon, setUpIdleModeTextData.iIconId.iIdentifier);
			}
		else
			{
			ASSERT_EQUALS(RSat::ENoIconId, setUpIdleModeTextData.iIconId.iQualifier);
			}
		
		if ( RSat::EUpdateIdleModeText == aType )
			{
			ASSERT_EQUALS(RSat::EUpdateIdleModeText, setUpIdleModeTextData.iType);
			if ( aTextCheck )
				{
				ASSERT_EQUALS( 0, aTextCheck->Compare(setUpIdleModeTextData.iText));
				}
			else
				{
				ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(aText, setUpIdleModeTextData.iText));
				}
			}
		else
			{
			ASSERT_EQUALS(RSat::ERemoveExistingIdleModeText, setUpIdleModeTextData.iType);
			}
		}

	// check MockLtsy status to ensure the normal flow of the test
	AssertMockLtsyStatusL();
	}


/**
Handles sending a terminal response
@param aResp terminal response data
@param aExpResult Expected result of TerminalRsp() request
*/
void CCSatSetUpIdleModeTextFU::GenerateSetUpIdleModeTextTerminalResponseL(const RSat::TSetUpIdleModeTextRspV1& aResp, TInt aExpResult)
	{

	TPtrC genResultAddInfo(KNullDesC);
	if ( RSat::KMeProblem == aResp.iInfoType )
		{
		genResultAddInfo.Set( aResp.iAdditionalInfo );
		}

	RSat::RSat::TSetUpIdleModeTextRspV1Pckg respPckg(aResp);
	GenerateTerminalResponseL(
			aResp.PCmdNumber(),
			KSetUpIdleModeText,
			KSetUpIdleModeTextPCmdQualifier,
			RSat::ESetUpIdleModeText,
			respPckg,
			genResultAddInfo,
			aResp.iGeneralResult,
			KNullDesC8,
			aExpResult);

	// check MockLtsy status to ensure the normal flow of the terminal response
	AssertMockLtsyStatusL();
	}


