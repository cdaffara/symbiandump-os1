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
 @file The TEFUnit test suite for SendSMS in the SAT.
*/

#include <etel.h>
#include <etelmm.h>

#include <satcs.h>
#include <ctsy/ltsy/sat_defs.h>

#include <test/tmockltsydata.h>
#include <test/mockltsyindicatorids.h>
#include <ctsy/ltsy/mltsydispatchsatinterface.h>
//#include "csatnotificationstsy.h"
#include "ccsatsendsmsfu.h"

const TUint8 KTestPCmdNumber = 1;
_LIT8(KDialingNumberString8,"112233445566778");
_LIT16(KDialingNumberString16,"112233445566778");


CTestSuite* CCSatSendSMSFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatSendSMSFU, TestNotifySendSmPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendSMSFU, TestNotifySendSmPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendSMSFU, TestNotifySendSmPCmd0004L);

	END_SUITE;
	}

//
// Actual test cases
//

/**
@SYMTestCaseID BA-CSAT-SSMS-SNSSMPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySendSmPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendSmPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendSMSFU::TestNotifySendSmPCmd0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of RSat::NotifySendSmPCmd 
 	//-------------------------------------------------------------------------
	// Run some of test sequences listed in 3GPP TS 31.124 clause 27.22.4.10 "SEND SHORT MESSAGE"
	DoPositiveNotifySendSmPCmdTestL(ETestSequence14);
	DoPositiveNotifySendSmPCmdTestL(ETestSequence15);
	DoPositiveNotifySendSmPCmdTestL(ETestSequence16);
	DoPositiveNotifySendSmPCmdTestL(ETestSequence17);
	DoPositiveNotifySendSmPCmdTestL(ETestSequence18);
	DoPositiveNotifySendSmPCmdTestL(ETestSequence31A);
	DoPositiveNotifySendSmPCmdTestL(ETestSequence31B);
	DoPositiveNotifySendSmPCmdTestL(ETestSequence32A);
	DoPositiveNotifySendSmPCmdTestL(ETestSequence32B);	

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifySendSmPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	// Create unsolicted "SEND SHORT MESSAGE"
	TTpdu tpdu;
	MockCompleteSendSmPCmdL(tpdu,ETestUnsolictedCompletion);

	// Prime Mock for forthcoming "TERMINAL RESPONSE" that is invoked internally by SAT TSY
	MockPrimeSendSmTerminalResponseL(ETestUnsolictedCompletion);

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifySendSmPCmd
 	//-------------------------------------------------------------------------
	DoPositiveNotifySendSmPCmdTestL(ETestMissingAddressType);
	DoPositiveNotifySendSmPCmdTestL(ETestMissingDiallingNumber);
	DoPositiveNotifySendSmPCmdTestL(ETestSmsCommand);
	DoPositiveNotifySendSmPCmdTestL(ETestRelativeVPF);
	DoPositiveNotifySendSmPCmdTestL(ETestEnchancedVPF);	
	DoPositiveNotifySendSmPCmdTestL(ETestAbsoluteVPF);
	DoPositiveNotifySendSmPCmdTestL(ETestRelativeVPFPacked);
	DoPositiveNotifySendSmPCmdTestL(ETestEnchancedVPFPacked); 

	DoNegativeNotifySendSmPCmdTestL(ETestTooBigSmsCommand);	
	DoNegativeNotifySendSmPCmdTestL(ETestTooBigSmsSubmit8bit);
	DoNegativeNotifySendSmPCmdTestL(ETestTooBigSmsSubmit7bit);
	DoNegativeNotifySendSmPCmdTestL(ETestTooBigSmsSubmit8bitPacked);	
	DoNegativeNotifySendSmPCmdTestL(ETestTooBigSmsSubmit7bitPacked);
	DoNegativeNotifySendSmPCmdTestL(ETestMissingSmsTpdu);	

	DoTestTerminalResponseL();
	
	DoPositiveNotifySendSmPCmdTestL(ETestAbsoluteVPFPacked);  

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	tpdu.Zero();
	MockPrimeSendSmPCmdL(tpdu,ETestSequence11,KErrUnknown);

	TRequestStatus reqStatus;

	RSat::TSendSmV1 sendSm;
	RSat::TSendSmV1Pckg sendSmPckg(sendSm);
	iSat.NotifySendSmPCmd(reqStatus,sendSmPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrUnknown, reqStatus.Int());	

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SSMS-SNSSMPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySendSmPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySendSmPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendSMSFU::TestNotifySendSmPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// tell Mock about forthcoming RSat::NotifySendSmPCmd
	const TUint8 KPcmdCode = KSendShortMessage;
	const TPckgC<TUint8> KPcmdCodePckg(KPcmdCode);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,KPcmdCodePckg);

	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifySendSmPCmd
 	//-------------------------------------------------------------------------
	
	TRequestStatus reqStatus;

	RSat::TSendSmV1 sendSm;
	RSat::TSendSmV1Pckg sendSmPckg(sendSm);
	iSat.NotifySendSmPCmd(reqStatus,sendSmPckg);
	iSat.CancelAsyncRequest(ESatNotifySendSmPCmd);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}

/**
@SYMTestCaseID BA-CSAT-SSMS-SNSSMPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySendSmPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySendSmPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendSMSFU::TestNotifySendSmPCmd0004L()
	{
	// Note: Most likely this test should be fixed.
	// Currently *all simultenious* RSat::NotifyXXXXPCmd() requests complete
	// with KErrNone when corresponding proactive command arrives succesfully.
	// That's the problem because once a RSat client gets a proactive command
	// it should reply with appropriate "TERMINAL RESPONSE" after processing it.
	// So if there are *two or more* independant clients listening for the same 
	// proactive command then *two or more* "TERMINAL RESPONSE"s for that command
	// are likely. Such behaviour seems to be erroneous because once a SIM application 
	// has issued a proactive command it would expect a *single* "TERMINAL RESPONSE" 
	// in reply but not two or more.

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
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RSat sat2;
	ret = sat2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(sat2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifySendSmPCmd
 	//-------------------------------------------------------------------------

	// Prime Mock with "SEND SHORT MESSAGE" proactive command
	TTpdu tpdu;
	MockPrimeSendSmPCmdL(tpdu, ETestSequence11);
	
	// Invoke 1-st client RSat::NotifySendSmPCmd()
	TRequestStatus reqStatus1;

	RSat::TSendSmV1 sendSm1;
	RSat::TSendSmV1Pckg sendSmPckg1(sendSm1);
	iSat.NotifySendSmPCmd(reqStatus1,sendSmPckg1);

	// Invoke 2-nd client RSat::NotifySendSmPCmd()
	TRequestStatus reqStatus2;
	RSat::TSendSmV1 sendSm2;
	RSat::TSendSmV1Pckg sendSmPckg2(sendSm2);
	sat2.NotifySendSmPCmd(reqStatus2,sendSmPckg2);

	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus2);

	// Check both results
	ASSERT_EQUALS(KErrNone, reqStatus1.Int());
	ASSERT_EQUALS(KErrNone, reqStatus2.Int()); // KErrNone or KErrServerBusy ? See the note at the top.     
	ASSERT_EQUALS(KTestPCmdNumber, sendSm1.PCmdNumber());
	ASSERT_EQUALS(KTestPCmdNumber, sendSm2.PCmdNumber());
	ASSERT_EQUALS(sendSm1.iAlphaId.iStatus, ExpectedAlphaIdStatus(ETestSequence11));
	ASSERT_EQUALS(sendSm2.iAlphaId.iStatus, ExpectedAlphaIdStatus(ETestSequence11));
	ASSERT_TRUE(sendSm1.iAlphaId.iAlphaId == ExpectedAlphaId(ETestSequence11));
	ASSERT_TRUE(sendSm2.iAlphaId.iAlphaId == ExpectedAlphaId(ETestSequence11));
	ASSERT_EQUALS(sendSm1.iAddress.iTypeOfNumber, ExpectedTypeOfNumber(ETestSequence11));
	ASSERT_EQUALS(sendSm2.iAddress.iTypeOfNumber, ExpectedTypeOfNumber(ETestSequence11));
	ASSERT_EQUALS(sendSm1.iAddress.iNumberPlan, ExpectedNumberPlan(ETestSequence11));
	ASSERT_EQUALS(sendSm2.iAddress.iNumberPlan, ExpectedNumberPlan(ETestSequence11));
	ASSERT_TRUE(sendSm1.iAddress.iTelNumber == KDialingNumberString16);
	ASSERT_TRUE(sendSm2.iAddress.iTelNumber == KDialingNumberString16);
	ASSERT_TRUE(sendSm1.iSmsTpdu == tpdu);		
	ASSERT_TRUE(sendSm2.iSmsTpdu == tpdu);		

	// Prime MOCK with appropriate "TERMINAL RESPONSE" 	
	RSat::TSendSmRspV1 rsp;
	RSat::TSendSmRspV1Pckg rspPckg(rsp);
	FillResponse(rsp,ETestSequence11);
	MockPrimeSendSmTerminalResponseL(ETestSequence11,rsp);
	
	// Invoke 1-st RSat:TerminalRsp	
	TRequestStatus rspStatus1;
	iSat.TerminalRsp(rspStatus1,RSat::ESendSm,rspPckg);
	User::WaitForRequest(rspStatus1);
	ASSERT_EQUALS(KErrNone, rspStatus1.Int());

	// Prime MOCK once again with "TERMINAL RESPONSE"
	MockPrimeSendSmTerminalResponseL(ETestSequence11,rsp);
	
	// Invoke 2-nd RSat:TerminalRsp
	TRequestStatus rspStatus2;
	sat2.TerminalRsp(rspStatus2,RSat::ESendSm,rspPckg);
	User::WaitForRequest(rspStatus2);
	ASSERT_EQUALS(KErrNone, rspStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}

// *************************** 
// * TLV tags used for testing
// ***************************
#if 1
// TLV tags without CR bit
const TUint8 KTestCommandDetailsTag  = KTlvCommandDetailsTag;
const TUint8 KTestDeviceIdentityTag  = KTlvDeviceIdentityTag;
const TUint8 KTestAlphaIdentifierTag = KTlvAlphaIdentifierTag;
const TUint8 KTestAddressTag         = KTlvAddressTag;
const TUint8 KTestSmsTpduTag         = KTlvSmsTpduTag;
const TUint8 KTestResultTag          = KTlvResultTag;
const TUint8 KTestIconIdentifierTag  = KTlvIconIdentifierTag;
#else
// TLV tags with CR (Comprehension Required) bit set. 
// It is not quite clear how exacly it should be used in "TERMINAL RESPONSE"
// but current SAT TSY implementation never sets it there for tags other 
// than "command details" though terminal responses listed in TS 31.124 have it set
// for all tags. Is that a defect ??
const TUint8 KTestCommandDetailsTag  = (KTlvCommandDetailsTag  | KTagCrMask);
const TUint8 KTestDeviceIdentityTag  = (KTlvDeviceIdentityTag  | KTagCrMask);
const TUint8 KTestAlphaIdentifierTag = (KTlvAlphaIdentifierTag | KTagCrMask);
const TUint8 KTestAddressTag         = (KTlvAddressTag | KTagCrMask);
const TUint8 KTestSmsTpduTag         = (KTlvSmsTpduTag | KTagCrMask);
const TUint8 KTestResultTag          = (KTlvResultTag  | KTagCrMask);
const TUint8 KTestIconIdentifierTag  = (KTlvIconIdentifierTag | KTagCrMask);
#endif

// ******************************
// * TLV values used for testing
// ******************************

// Type of address, see ETSI TS 102 223 (v8.1.0) clause 8.1
// Possible TON (i.e. "Type Of Number") values:
// 		0x00 (000) - Unknown
// 		0x01 (001) - International Number
// 		0x02 (010) - National Number
// 		0x03 (011) - Network Specific Number
//	the below values are defined in 3GPP TS 23.040 V8.2.0
//		0x04 (100) Subscriber number
//		0x05 (101) Alphanumeric, (coded according to 3GPP TS 23.038 [9] GSM 7-bit default alphabet)
//		0x06 (110) Abbreviated number
//		0x07 (111) Reserved for extension	

// Possible NPI (i.e. "Numbering Plan Identification") values:
// 		0x00 (0000) - Unknown
// 		0x01 (0001) - ISDN/telephony numbering plan (ITU-T Recommendations E.164 and E.163)
// 		0x03 (0011) - Data numbering plan (ITU-T Recommendation X.121)
// 		0x04 (0100) - Telex numbering plan (ITU-T Recommendation F.69)
// 		0x09 (1001) - Private numbering plan
// 		0x0F (1111) - Reserved for extension
//	the below values are defined in 3GPP TS 23.040 V8.2.0
//		0x05 (0101) Service Centre Specific plan
//		0x06 (0110) Service Centre Specific plan
//		0x08 (1000) National numbering plan
//		0x0A (1010) ERMES numbering plan (ETSI DE/PS 3 01-3)
const TUint8 KTestTypeOfAddress = (0x80 | (0x01 << 4) | 0x01); // International number + ISDN/telephony numbering plan   

// Icon identifier (a number of a record in EFimg)
const TUint8 KTestIconIdentifier = 1; 

// TP-Protocol-Identifier (TP-PID) values, see 3GPP TS 23.040 V8.2.0, clause 9.2.3.9
//		0x40 (01000000) Short Message Type 0
//		0x41 (01000001) Replace Short Message Type 1
//		0x42 (01000010) Replace Short Message Type 2
//		0x43 (01000011) Replace Short Message Type 3
//		0x44 (01000100) Replace Short Message Type 4
//		0x45 (01000101) Replace Short Message Type 5
//		0x46 (01000110) Replace Short Message Type 6
//		0x46 (01000111) Replace Short Message Type 7
//		0x5E (01011110) Enhanced Message Service (Obsolete)
//		0x5F (01011111) Return Call Message
//		0x7C (01111100) ANSI-136 R-DATA
//		0x7D (01111101) ME Data download
//		0x7E (01111110) ME De-personalization Short Message
//		0x7F (01111111) (U)SIM Data download
const TUint8 KShortMessageType0 = 0x40; //Short message type 0


//**********************
// Strings used in tests
//**********************

_LIT8(KDestinationAddressString8,"012345678");
_LIT8(KShortDestinationAddressString8,"01");
_LIT8(KTestMessageString8,"Test Message");
_LIT8(KSendSmString8,"Send SM");
_LIT16(KSendSmString16,"Send SM");
_LIT8(KShortMessageString8,"Short Message");
_LIT16(KShortMessageString16,"Short Message");
_LIT8(KNoIconString8,"NO ICON");
_LIT16(KNoIconString16,"NO ICON");

// GSM7 coded "Short Message"
_LIT8(KShortMessageStringGSM7,"\x53\xF4\x5B\x4E\x07\x35\xCB\xF3\x79\xF8\x5C\x06");

// GSM7 encoded "The address data object holds the RP_Destination_Address"
_LIT8(KSequence14AlphaString8,
"\x54\x68\x65\x20\x61\x64\x64\x72\x65\x73\x73\x20\x64\x61\x74\x61\
\x20\x6F\x62\x6A\x65\x63\x74\x20\x68\x6F\x6C\x64\x73\x20\x74\x68\
\x65\x20\x52\x50\x11\x44\x65\x73\x74\x69\x6E\x61\x74\x69\x6F\x6E\
\x11\x41\x64\x64\x72\x65\x73\x73"); 

_LIT(KSequence14AlphaString16,
"The address data object holds the RP_Destination_Address");

_LIT8(KSequence15AlphaString8,
"The address data object holds the RP Destination Address");

_LIT(KSequence15AlphaString16,
"The address data object holds the RP Destination Address");

_LIT8(KSequence16AlphaString8,
"Two types are defined: - A short message to be sent to the network in an \
SMS-SUBMIT message, or an SMS-COMMAND message, where the user data can \
be passed transparently; - A short message to be sent to the network in an \
SMS-SUBMIT ");

_LIT(KSequence16AlphaString16,
"Two types are defined: - A short message to be sent to the network in an \
SMS-SUBMIT message, or an SMS-COMMAND message, where the user data can \
be passed transparently; - A short message to be sent to the network in an \
SMS-SUBMIT ");

_LIT8(KLongUserDataString8,
"Two types are defined: - A short message to be sent to the network in an \
SMS-SUBMIT message, or an SMS-COMMAND message, where the user data can \
be passed transp");

// GSM7 encoded K160CharsString 
//"Two types are defined: - A short message to be sent to the network in an " 
//"SMS-SUBMIT message, or an SMS-COMMAND message, where the user data can "
//"be passed transp"
_LIT8(KLongUserDataStringGSM7,
"\xD4\xFB\x1B\x44\xCF\xC3\xCB\x73\x50\x58\x5E\x06\
\x91\xCB\xE6\xB4\xBB\x4C\xD6\x81\x5A\xA0\x20\x68\
\x8E\x7E\xCB\xE9\xA0\x76\x79\x3E\x0F\x9F\xCB\x20\
\xFA\x1B\x24\x2E\x83\xE6\x65\x37\x1D\x44\x7F\x83\
\xE8\xE8\x32\xC8\x5D\xA6\xDF\xDF\xF2\x35\x28\xED\
\x06\x85\xDD\xA0\x69\x73\xDA\x9A\x56\x85\xCD\x24\
\x15\xD4\x2E\xCF\xE7\xE1\x73\x99\x05\x7A\xCB\x41\
\x61\x37\x68\xDA\x9C\xB6\x86\xCF\x66\x33\xE8\x24\
\x82\xDA\xE5\xF9\x3C\x7C\x2E\xB3\x40\x77\x74\x59\
\x5E\x06\xD1\xD1\x65\x50\x7D\x5E\x96\x83\xC8\x61\
\x7A\x18\x34\x0E\xBB\x41\xE2\x32\x08\x1E\x9E\xCF\
\xCB\x64\x10\x5D\x1E\x76\xCF\xE1");

_LIT8(KSingleSpaceUserData8," ");

// *********************
// Below TPDUs are taken from 3GPP TS 31.124 (see  SMS-PP (SEND SHORT MESSAGE) 
// codings for each test sequence where command qualifier is set to "packing required")
// The only difference is that MR values (the 2-nd byte) are "0" instead of "1" as 
// in the original document.
// *********************

_LIT8(KSequence12ExpectedTpdu,
"\x01\x00\x09\x91\x10\x32\x54\x76\xF8\x40\xF0\x07\xD3\xB2\x9B\x0C\x9A\x36\x01");

_LIT8(KSequence14ExpectedTpdu,
"\x01\x00\x09\x91\x10\x32\x54\x76\xF8\x40\xF0\
\xA0\xD4\xFB\x1B\x44\xCF\xC3\xCB\x73\x50\x58\x5E\
\x06\x91\xCB\xE6\xB4\xBB\x4C\xD6\x81\x5A\xA0\x20\
\x68\x8E\x7E\xCB\xE9\xA0\x76\x79\x3E\x0F\x9F\xCB\
\x20\xFA\x1B\x24\x2E\x83\xE6\x65\x37\x1D\x44\x7F\
\x83\xE8\xE8\x32\xC8\x5D\xA6\xDF\xDF\xF2\x35\x28\
\xED\x06\x85\xDD\xA0\x69\x73\xDA\x9A\x56\x85\xCD\
\x24\x15\xD4\x2E\xCF\xE7\xE1\x73\x99\x05\x7A\xCB\
\x41\x61\x37\x68\xDA\x9C\xB6\x86\xCF\x66\x33\xE8\
\x24\x82\xDA\xE5\xF9\x3C\x7C\x2E\xB3\x40\x77\x74\
\x59\x5E\x06\xD1\xD1\x65\x50\x7D\x5E\x96\x83\xC8\
\x61\x7A\x18\x34\x0E\xBB\x41\xE2\x32\x08\x1E\x9E\
\xCF\xCB\x64\x10\x5D\x1E\x76\xCF\xE1");

// *************************
// Fake TP-VP values used in
// *************************
_LIT8(KFakeRelativeValidityValue,"\x00");
_LIT8(KFakeAbsoluteOrEnchancedValidityValue,"\x00\x00\x00\x00\x00\x00\x00");

// *********************
// below 3 TPDUs are made of KSequence12ExpectedTpdu combined with 
// KFakeRelativeValidityValue and KFakeAbsoluteOrEnchancedValidityValue appropriately
// Once the above strings are changed the below should be changed too 
// *********************
_LIT8(KTestRelativeVPFPackedExpectedTpdu,
"\x11\x00\x09\x91\x10\x32\x54\x76\xF8\x40\xF0\x00\x07\xD3\xB2\x9B\x0C\x9A\x36\x01");

_LIT8(KTestEnchancedVPFPackedExpectedTpdu,
"\x09\x00\x09\x91\x10\x32\x54\x76\xF8\x40\xF0\x00\x00\x00\x00\x00\x00\x00\x07\xD3\xB2\x9B\x0C\x9A\x36\x01");

_LIT8(KTestAbsoluteVPFPackedExpectedTpdu,
"\x19\x00\x09\x91\x10\x32\x54\x76\xF8\x40\xF0\x00\x00\x00\x00\x00\x00\x00\x07\xD3\xB2\x9B\x0C\x9A\x36\x01");

/**
 * Executes "positive" RSat::NotifySendSmPCmd() test identified by aTC 
 * @param aTC testcase identifier 
 */
void CCSatSendSMSFU::DoPositiveNotifySendSmPCmdTestL(TSendSmTC aTC)
	{
	// Prime Mock with appopriate "SEND SHORT MESSAGE" proactive command
	TTpdu tpdu;
	MockPrimeSendSmPCmdL(tpdu, aTC);
	
	// Invoke RSat::NotifySendSmPCmd()
	TRequestStatus reqStatus;

	RSat::TSendSmV1 sendSm;
	RSat::TSendSmV1Pckg sendSmPckg(sendSm);
	iSat.NotifySendSmPCmd(reqStatus,sendSmPckg);
	User::WaitForRequest(reqStatus);

	// Check the result
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(KTestPCmdNumber, sendSm.PCmdNumber());
	if(AlphaIdIsUsed(aTC))
		{
		ASSERT_EQUALS(sendSm.iAlphaId.iStatus, ExpectedAlphaIdStatus(aTC));
		ASSERT_TRUE(sendSm.iAlphaId.iAlphaId == ExpectedAlphaId(aTC));
		}
	else
		{		
		ASSERT_EQUALS(RSat::EAlphaIdNotPresent, sendSm.iAlphaId.iStatus);
		}
	ASSERT_EQUALS(sendSm.iAddress.iTypeOfNumber, ExpectedTypeOfNumber(aTC));
	ASSERT_EQUALS(sendSm.iAddress.iNumberPlan, ExpectedNumberPlan(aTC));
	if(CompleteAddressIsUsed(aTC))
		{
		ASSERT_TRUE(sendSm.iAddress.iTelNumber == KDialingNumberString16);
		}	
	if(IsIconRelatedTC(aTC))
		{
		ASSERT_EQUALS(sendSm.iIconId.iQualifier, ExpectedIconQualifier(aTC));
		ASSERT_EQUALS(sendSm.iIconId.iIdentifier, KTestIconIdentifier);
		}
	if(PackingIsUsed(aTC))
		{
		ASSERT_TRUE(sendSm.iSmsTpdu == ExpectedTpdu(aTC));				
		}
	else
		{
		ASSERT_TRUE(sendSm.iSmsTpdu == tpdu);		
		}

	// Prime MOCK with appropriate "TERMINAL RESPONSE" 	
	RSat::TSendSmRspV1 rsp;
	FillResponse(rsp,aTC);
	MockPrimeSendSmTerminalResponseL(aTC,rsp);
	
	// Invoke RSat::TerminalRsp
	RSat::TSendSmRspV1Pckg rspPckg(rsp);
	TerminalResponseL(RSat::ESendSm,rspPckg);
	}

/**
 * Executes "negative" RSat::NotifySendSmPCmd() test identified by aTC 
 * @param aTC testcase identifier 
 */
void CCSatSendSMSFU::DoNegativeNotifySendSmPCmdTestL(TSendSmTC aTC)
	{
	// Tell Mock to issue "SEND SHORT MESSAGE" with some corrupted data 
	TTpdu tpdu;
	MockPrimeSendSmPCmdL(tpdu, aTC);

	// Prime Mock with appropriate "TERMINAL RESPONSE" because 
	// in case of error SAT TSY inokes it internally
	MockPrimeSendSmTerminalResponseL(aTC);
		
	// Call RSat::NotifySendSmPCmd()
	TRequestStatus reqStatus;

	RSat::TSendSmV1 sendSm;
	RSat::TSendSmV1Pckg sendSmPckg(sendSm);
	iSat.NotifySendSmPCmd(reqStatus,sendSmPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCorrupt, reqStatus.Int());
	}

/**
 * Tests all possible "TERMINAL RESPONSE" cases. 
 * API should return KErrNone for all valid responses and KErrCorrupt 
 * for those that can not be used with "SEND SHORT MESSAGE" command  
 */
void CCSatSendSMSFU::DoTestTerminalResponseL()
	{
	static const struct 
		{
		RSat::TPCmdResult			iGeneralResult;
		RSat::TAdditionalInfoType	iInfoType;
		TBool						iIntentionallyOmmitAdditionalInfo;
		TUint8						iAdditionalInfo;
		TInt						iExpectedResult;
		} 
	KResponsesToTest[] = 
		{
			{
			// 0x01 - Command performed with partial comprehension;
			RSat::KPartialComprehension,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x02 - Command performed, with missing information;
			RSat::KMissingInformation,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x03 - REFRESH performed with additional EFs read;
			RSat::KRefreshAdditionEFRead,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{ 
			// 0x04 - Command performed successfully, but requested icon could not be displayed;
			RSat::KSuccessRequestedIconNotDisplayed,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{
			// 0x05 - Command performed, but modified by call control by NAA;
			RSat::KModifiedByCallControl,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{
			// 0x06 - Command performed successfully, limited service;
			RSat::KSuccessLimitedService,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x07 - Command performed with modification;
			RSat::KPerformedWithModifications,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x08 - REFRESH performed but indicated NAA was not active;
			RSat::KRefreshUSIMNotActive,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x09 - Command performed successfully, tone not played;
			RSat::KPlayTonePerformedSuccessfully,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x10 - Proactive UICC session terminated by the user;
			RSat::KPSessionTerminatedByUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x11 - Backward move in the proactive UICC session requested by the user;
			RSat::KBackwardModeRequestedByUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt			
			},
			{
			// 0x12 - No response from user;
			RSat::KNoResponseFromUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt			
			},
			{
			// 0x13 - Help information required by the user;
			RSat::KHelpRequestedByUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt			
			},
			{
			// 0x14 - reserved for GSM/3G.
			RSat::KUssdTransactionTerminatedByUser,
			RSat::KNoAdditionalInfo, 
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x20 - terminal currently unable to process command;
			RSat::KMeUnableToProcessCmd,
			RSat::KMeProblem,
			EFalse,
			0,
			KErrNone
			},
			{
			// 0x20 without additional information (erroneous)
			RSat::KMeUnableToProcessCmd,
			RSat::KMeProblem, 
			ETrue, // Intentional error !
			0,
			KErrCorrupt
			},	
			{
			// 0x21 - Network currently unable to process command;
			RSat::KNetworkUnableToProcessCmd,
			RSat::KSatNetworkErrorInfo,
			EFalse,
			0,
			KErrCorrupt
			},
			{
			// 0x21 without additional information (erroneous)
			RSat::KNetworkUnableToProcessCmd,
			RSat::KSatNetworkErrorInfo,
			ETrue, // Intentional error !
			0,
			KErrCorrupt
			},
			{
			// 0x22 - User did not accept the proactive command;
			RSat::KPCmdNotAcceptedByUser,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x23 - User cleared down call before connection or network release;
			RSat::KCallClearedBeforeConnectionOrReleased,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x24 - Action in contradiction with the current timer state;
			RSat::KContradictionWithTimerState,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x25 - Interaction with call control by NAA, temporary problem;
			RSat::KInteractionWithCCTemporaryError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x26 - Launch browser generic error code;
			RSat::KLaunchBrowserError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x26 without additional information (erroneous)
			RSat::KLaunchBrowserError,
			RSat::KMeProblem, 
			ETrue, // Intentional error !
			0,
			KErrCorrupt
			},
			{
			// 0x27 - MMS temporary problem.
			RSat::KMMSTemporaryProblem,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x30 - Command beyond terminal's capabilities;
			RSat::KCmdBeyondMeCapabilities,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{
			// 0x31 - Command type not understood by terminal;
			RSat::KCmdTypeNotUnderstood,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{ 
			// 0x32 - Command data not understood by terminal;
			RSat::KCmdDataNotUnderstood,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x33 - Command number not known by terminal;
			RSat::KCmdNumberNotKnown,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x34 - reserved for GSM/3G;
			RSat::KSsReturnError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x35 - reserved for GSM/3G;
			RSat::KSmsRpError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{ 
			// 0x36 - Error, required values are missing;
			RSat::KErrorRequiredValuesMissing,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x37 - reserved for GSM/3G;
			RSat::KUssdReturnError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x38 - MultipleCard commands error;
			RSat::KMultipleCardCmdsError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x38 without additional information (erroneous)
			RSat::KMultipleCardCmdsError,
			RSat::KMeProblem,
			ETrue, // Intentional error
			0,
			KErrCorrupt
			},
			{
			// 0x39 - Interaction with call control by NAA, permanent problem;
			RSat::KInteractionWithCCPermanentError,
			RSat::KControlInteraction,
			EFalse,
			0,
			KErrNone
			},
			{
			// 0x39 without additional information (erroneous)
			RSat::KInteractionWithCCPermanentError,
			RSat::KControlInteraction,
			ETrue, // Intentional error
			0,
			KErrCorrupt
			},
			{
			// 0x3A - Bearer Independent Protocol error;
			RSat::KBearerIndepProtocolError,
			RSat::KNoAdditionalInfo, 
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x3A without additional information (erroneous)
			RSat::KBearerIndepProtocolError,
			RSat::KMeProblem, 
			ETrue, // Intentional error
			0,
			KErrCorrupt
			},
			{
			// 0x3B - Access Technology unable to process command;
			RSat::KAccessTechUnableProcessCmd,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x3C - Frames error;
			RSat::KFramesError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x3C without additional information (erroneous)
			RSat::KFramesError,
			RSat::KMeProblem,
			ETrue, // Intentional error
			0,
			KErrCorrupt
			},
			{
			// 0x3D - MMS Error.
			RSat::KMMSError,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x3D without additional information (erroneous)
			RSat::KMMSError,
			RSat::KMeProblem,
			ETrue, // Intensional error !
			0,
			KErrCorrupt
			}
		};	

	const TInt KNumberOfResponsesToTest = sizeof(KResponsesToTest)/ sizeof(KResponsesToTest[0]); 

	for(TInt i = 0; i <= KNumberOfResponsesToTest; i++)
		{
		// Prime Mock with some successfull "SEND SHORT MESSAGE" proactive command
		TTpdu tpdu;
		MockPrimeSendSmPCmdL(tpdu, ETestSequence11);
		
		// Invoke RSat::NotifySendSmPCmd() to get the result
		TRequestStatus reqStatus;

		RSat::TSendSmV1 sendSm;
		RSat::TSendSmV1Pckg sendSmPckg(sendSm);
		iSat.NotifySendSmPCmd(reqStatus,sendSmPckg);
		User::WaitForRequest(reqStatus);
		ASSERT_EQUALS(KErrNone, reqStatus.Int());

		// now test a "TERMINAL RESPONSE" specific to this loop 
		TInt expectedResult; 

		RSat::TSendSmRspV1 rsp;
		rsp.SetPCmdNumber(KTestPCmdNumber);

		if(i < KNumberOfResponsesToTest)
			{
			// looping through KResponsesToTest table
			rsp.iGeneralResult	= KResponsesToTest[i].iGeneralResult;
			rsp.iInfoType 		= KResponsesToTest[i].iInfoType;
			if(rsp.iInfoType != RSat::KNoAdditionalInfo && 
			   !KResponsesToTest[i].iIntentionallyOmmitAdditionalInfo)
				{
				rsp.iAdditionalInfo.Append(KResponsesToTest[i].iAdditionalInfo);
				}
			expectedResult = KResponsesToTest[i].iExpectedResult;
			// tell Mock to dispatch the forthcoming RSat::TerminalRsp() succesfully
			MockPrimeSendSmTerminalResponseL(ETestSequence11,rsp);
			}
		else
			{
			// finished with KResponsesToTest table
			// now tell Mock to return KErrUnknown when dispatching forthcoming RSat::TerminalRsp()
			rsp.iGeneralResult	= RSat::KSuccess;
			rsp.iInfoType 		= RSat::KNoAdditionalInfo;	
			expectedResult = KErrUnknown;
			MockPrimeSendSmTerminalResponseL(ETestSequence11,rsp,expectedResult);			
			}
				
		// and finally send "TERMINAL RESPONSE" and analyse the result 	
		RSat::TSendSmRspV1Pckg rspPckg(rsp);
		iSat.TerminalRsp(reqStatus, RSat::ESendSm, rspPckg);
		User::WaitForRequest(reqStatus);
		ASSERT_EQUALS(reqStatus.Int(),expectedResult);
		
		AssertMockLtsyStatusL();
		}
	}

/**
 * Given a testcase primes MOCK with appropriate "SEND SHORT MESSAGE" data
 * @param aTpdu created SMS TPDU 
 * @param aTC testcase identifier 
 * @param aResultCode result code to be returned by RSat::NotifySendSmPCmd  
 */
void CCSatSendSMSFU::MockPrimeSendSmPCmdL(TTpdu& aTpdu, TSendSmTC aTC, TInt aResultCode)
	{	
	// prime Mock with pending "SEND SHORT MESSAGE" request
	const TUint8 KPcmdCode = KSendShortMessage;
	const TPckgC<TUint8> KPcmdCodePckg(KPcmdCode);

	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,KPcmdCodePckg);

	// trigger "SEND SHORT MESSAGE" event
	MockCompleteSendSmPCmdL(aTpdu,aTC,aResultCode);
	}

/**
 * Given a testcase primes MOCK with appropriate "TERMINAL RESPONSE" data
 * @param aTC testcase identifier 
 */
void CCSatSendSMSFU::MockPrimeSendSmTerminalResponseL(TSendSmTC aTC)
	{
	RSat::TSendSmRspV1 rsp;
	FillResponse(rsp,aTC);
	MockPrimeSendSmTerminalResponseL(aTC,rsp);
	}

/**
 * Given a testcase and filled RSat::TSendSmRspV1 strucuture primes MOCK with appropriate "TERMINAL RESPONSE" data
 * @param aTC testcase identifier 
 * @param aRsp a result to be passed to RSat::TerminalRsp()
 * @param aResultCode result code to be returned by RSat::TerminalRsp()
 */
void CCSatSendSMSFU::MockPrimeSendSmTerminalResponseL(TSendSmTC aTC, const RSat::TSendSmRspV1& aRsp, TInt aResultCode)
	{
	RBuf8 data;
	data.CleanupClosePushL();

	TTlv tlv;		
	const TUint8 qualifier = CommandQualifier(aTC); 
	TDesC8* dataPtr = &const_cast<TDesC8&>(CreateSendSmTerminalResponseTlv(tlv,qualifier,aRsp));
	TMockLtsyData1<TDesC8*> termRespData(dataPtr);
	termRespData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data,aResultCode);
	CleanupStack::PopAndDestroy();
	}

/**
 * Triggers "SEND SHORT MESSAGE" event
 * @param aTpdu created SMS TPDU
 * @param aTC testcase identifier
 * @param aResultCode result code to be returned by RSat::NotifySendSmPCmd
 */
void CCSatSendSMSFU::MockCompleteSendSmPCmdL(TTpdu& aTpdu, TSendSmTC aTC, TInt aResultCode)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	CreateSmsTpdu(aTpdu,aTC);

	TTlvBuf<KTlvMaxSize + 1> tlv;	
	TDesC8* dataPtr = &const_cast<TDesC8&>(CreateSendSmTlv(tlv,aTpdu,aTC));
	TMockLtsyData1<TDesC8*> tlvPack(dataPtr);
	tlvPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId,aResultCode,data);

	CleanupStack::PopAndDestroy();	 // data	
	}

/**
 * returns ETrue if command qualifier is set to "packing required" for a given testcase
 * @param aTC testcase identifier
 * @return ETrue if "packing required" EFalse otherwise 
 */
TBool CCSatSendSMSFU::PackingIsUsed(TSendSmTC aTC)
	{
	TBool result;
	switch(aTC)
		{
		case ETestSequence12:
		case ETestSequence14: 
		case ETestTooBigSmsSubmit8bitPacked:
		case ETestTooBigSmsSubmit7bitPacked:
		case ETestRelativeVPFPacked:
		case ETestEnchancedVPFPacked:
		case ETestAbsoluteVPFPacked:
			result = ETrue;
			break;

		default:
			result = EFalse;
		}
	return result;
	}

/**
 * returns expected TPDU for a given testcase
 * Should be called only if PackingIsUsed() returns ETrue
 * @param aTC testcase identifier
 * @return a reference to a descriptor containing expected TPDU 
 */
const TDesC8& CCSatSendSMSFU::ExpectedTpdu(TSendSmTC aTC)
	{
	__ASSERT_ALWAYS(PackingIsUsed(aTC),User::Invariant());
	switch(aTC)
		{
		case ETestSequence12:
			return KSequence12ExpectedTpdu();

		case ETestRelativeVPFPacked:
			return KTestRelativeVPFPackedExpectedTpdu();

		case ETestEnchancedVPFPacked:
			return KTestEnchancedVPFPackedExpectedTpdu();

		case ETestAbsoluteVPFPacked:
			return KTestAbsoluteVPFPackedExpectedTpdu();

		default:
			// ETestSequence14:
			return KSequence14ExpectedTpdu();			
			
		}
	}

/**
 * Given a testcase returns corresponding TP-VPF value
 * that should be used in the testcase,  see clause 9.2.3.3 of the 3GPP TS 23.040
 * @param aTC testcase identifier
 * @return TP-VPF value 
 */
TUint8 CCSatSendSMSFU::ValidityPeriodFormat(TSendSmTC aTC)
	{
	TUint8 vpf;
	switch(aTC)
		{
		case ETestRelativeVPF:
		case ETestRelativeVPFPacked:
			vpf = KSmsVPFRelative;
			break;

		case ETestEnchancedVPF:
		case ETestEnchancedVPFPacked:
			vpf = KSmsVPFEnhanced;
			break;

		case ETestAbsoluteVPF:
		case ETestAbsoluteVPFPacked:
			vpf = KSmsVPFAbsolute;
			break;

		default:
			vpf = 0;			
		}
	return vpf;
	}

/**
 * Given a testcase returns corresponding destination address 
 * that should be used in the testcase
 * @param aTC testcase identifier
 * @return a reference to a descriptor containing destination address 
 */
const TDesC8& CCSatSendSMSFU::DestinationAddress(TSendSmTC aTC)
	{
	if(aTC == ETestSequence16)
		{
		return KShortDestinationAddressString8();
		}
	else
		{
		return KDestinationAddressString8();
		}
	}

/**
 * Given a testcase returns corresponding data coding scheme 
 * that should be used in the testcase
 * @param aTC testcase identifier
 * @return data coding scheme value 
 */
TUint8 CCSatSendSMSFU::DataCodingScheme(TSendSmTC aTC)
	{
	// Possible TP-DSC (TP-Data-Coding-Scheme) values (see 3GPP TS 23.038 clause 4)
	// 	0xF0 - Message coding: GSM 7 bit default alphabet
	//         Message class : Class 0

	// 	0xF1 - Message coding: GSM 7 bit default alphabet
	//         Message class : Class 1 default meaning: ME-specific.

	// 	0xF2 - Message coding: GSM 7 bit default alphabet
	//         Message class : Class 2 (U)SIM-specific message.

	// 	0xF3 - Message coding: GSM 7 bit default alphabet
	//         Message class : Class 3 default meaning: TE specific

	// 	0xF4 - Message coding: 8-bit data
	//         Message class : Class 0

	// 	0xF5 - Message coding: 8-bit data
	//         Message class : Class 1 default meaning: ME-specific.

	// 	0xF6 - Message coding: 8-bit data
	//         Message class : Class 2 (U)SIM-specific message.

	// 	0xF7 - Message coding: 8-bit data
	//         Message class : Class 3 default meaning: TE specific
	switch(aTC)
		{
		case ETestSequence13:
		case ETestSequence15:
		case ETestSequence16:
		case ETestTooBigSmsSubmit7bit:
			return 0xF0; // GSM 7 bit default alphabet + Class 0

		default:
			return 0xF4; // 8-bit + Class 0
		}	
	}

/**
 * Given a testcase returns corresponding TP-User-Data-Length value 
 * that should be used in the testcase
 * @param aTC testcase identifier
 * @return TP-UDL value 
 */
TUint8 CCSatSendSMSFU::UserDataLength(TSendSmTC aTC)
	{
	switch(aTC)
		{
		case ETestSequence12:
		case ETestRelativeVPFPacked:
		case ETestEnchancedVPFPacked:
		case ETestAbsoluteVPFPacked:
			return KSendSmString8().Length();

		case ETestSequence13:
			return KShortMessageString8().Length();

		case ETestSequence14:
		case ETestSequence15:
			return KLongUserDataString8().Length(); 

		case ETestSequence16:
			return 	KSingleSpaceUserData8().Length();

		default:
			return KTestMessageString8().Length();
		}
	}

/**
 * Given a testcase returns corresponding TP-User-Data value 
 * that should be used in the testcase
 * @param aTC testcase identifier
 * @return a reference to a descriptor containing TP-UD 
 */
const TDesC8& CCSatSendSMSFU::UserData(TSendSmTC aTC)
	{
	switch(aTC)
		{
		case ETestSequence12:
		case ETestRelativeVPFPacked:
		case ETestEnchancedVPFPacked:
		case ETestAbsoluteVPFPacked:
			return KSendSmString8();

		case ETestSequence13:
			// return GSM 7 bit encoded string   
			return KShortMessageStringGSM7();

		case ETestSequence14:
			return KLongUserDataString8(); 

		case ETestSequence15:
			return KLongUserDataStringGSM7();

		case ETestSequence16:
			return 	KSingleSpaceUserData8();

		default:
			return KTestMessageString8();
		}
	}

/**
 * Given a testcase returns corresponding command qualifier 
 * that should be used in the testcase
 * @param aTC testcase identifier
 * @return command qualifier value 
 */
TUint8 CCSatSendSMSFU::CommandQualifier(TSendSmTC aTC)
	{
	// possible "command qualifier" values for "SEND SHORT MESSAGE" are
	//		0 = packing not required;
	//		1 = SMS packing by the ME required.
	return static_cast<TUint8>(PackingIsUsed(aTC));
	}

/**
 * returns ETrue if alpha identifier is used in a given testcase
 * @param aTC testcase identifier
 * @return ETrue if alpha identifier is used 
 */
TBool CCSatSendSMSFU::AlphaIdIsUsed(TSendSmTC aTC)
	{
	TBool result = ETrue;
	if(aTC == ETestSequence18)
		{
		result = EFalse;
		}
	return result;
	}

/**
 * Given a testcase returns corresponding alpha identifier 
 * that should be used in the testcase
 * @param aTC testcase identifier
 * @return a reference to a descriptor containing alpha identifier 
 */
const TDesC8& CCSatSendSMSFU::AlphaId(TSendSmTC aTC)
	{
	switch(aTC)
		{
		case ETestSequence31A:
		case ETestSequence31B:
			return KNoIconString8();

		case ETestSequence13:
			return KShortMessageString8();

		case ETestSequence14:
			return KSequence14AlphaString8(); 

		case ETestSequence15:
			return KSequence15AlphaString8(); 

		case ETestSequence16:
			return KSequence16AlphaString8();
			
		case ETestSequence17:
			return KNullDesC8();

		default:
			return KSendSmString8();		
		}	
	}

/**
 * Given a testcase returns an alpha identifier 
 * that should be returned by RSat API
 * @param aTC testcase identifier
 * @return a reference to a descriptor containing expected alpha identifier 
 */
const TDesC& CCSatSendSMSFU::ExpectedAlphaId(TSendSmTC aTC)
	{
	switch(aTC)
		{
		case ETestSequence31A:
		case ETestSequence31B:
			return KNoIconString16();

		case ETestSequence13:
			return KShortMessageString16();

		case ETestSequence14:
			return KSequence14AlphaString16();

		case ETestSequence15:
			return KSequence15AlphaString16(); 

		case ETestSequence16:
			return KSequence16AlphaString16();

		case ETestSequence17:
			return KNullDesC();

		default:
			return KSendSmString16();		
		}	
	}

/**
 * Given a testcase returns alpha identifier status 
 * that should be returned by RSat API
 * Should be called only if AlphaIdIsUsed() returns ETrue
 * @param aTC testcase identifier
 * @return expected alpha identifier status 
 */
RSat::TAlphaIdStatus CCSatSendSMSFU::ExpectedAlphaIdStatus(TSendSmTC aTC)
	{
	__ASSERT_ALWAYS(AlphaIdIsUsed(aTC),User::Invariant());
	RSat::TAlphaIdStatus status = RSat::EAlphaIdProvided;
	if(aTC == ETestSequence17)
		{
		status = RSat::EAlphaIdNull;
		}
	return status;
	}

/**
 * Returns ETrue if address TLV is completely initialized for a given testcase
 * @param aTC testcase identifier
 * @return ETrue if TLV is completely initialized 
 */
TBool CCSatSendSMSFU::CompleteAddressIsUsed(TSendSmTC aTC)
	{
	TBool result = ETrue;
	if(aTC == ETestSequence16 || aTC == ETestMissingAddressType || aTC == ETestMissingDiallingNumber)
		{
		result = EFalse;
		}
	return result;
	}

/**
 * Given a testcase returns a numbering plan that should be returned by RSat API
 * @param aTC testcase identifier
 * @return a numbering plan 
 */
RSat::TNumberingPlan CCSatSendSMSFU::ExpectedNumberPlan(TSendSmTC aTC)
	{	
	if(CompleteAddressIsUsed(aTC))
		{
		// All "SEND SHORT MESSAGE" test sequences listed in 3GPP TS 31.124 
		// except 1.6 are using this NPI 
		return RSat::EIsdnNumberPlan;
		}

	if(aTC == ETestSequence16)
		{
		// Address TLV is omitted in this test
		return RSat::ENumberingPlanNotSet;
		}
	
	// aTC is either ETestMissingAddressType or ETestMissingDiallingNumber 
	return RSat::EUnknownNumberingPlan;
	}

/**
 * Given a testcase returns a type of a dialling number that should be returned by RSat API
 * @param aTC testcase identifier
 * @return expected TON value 
 */
RSat::TTypeOfNumber CCSatSendSMSFU::ExpectedTypeOfNumber(TSendSmTC aTC)
	{
	if(CompleteAddressIsUsed(aTC))
		{
		// All "SEND SHORT MESSAGE" test sequences listed in 3GPP TS 31.124 
		// except 1.6 are using this TON 
		return RSat::EInternationalNumber;
		}

	if(aTC == ETestSequence16)
		{
		// Address TLV is omitted in this test
		return RSat::ETypeOfNumberNotSet;
		}
	
	// aTC is either ETestMissingAddressType or ETestMissingDiallingNumber 
	return RSat::EUnknownNumber;
	}

/**
 * Returns ETrue if the given testcase is icon-related
 * @param aTC testcase identifier
 * @return ETrue is the testcase is icon-related 
 */
TBool CCSatSendSMSFU::IsIconRelatedTC(TSendSmTC aTC)
	{
	TBool result = EFalse;
	if(	aTC == ETestSequence31A || 
		aTC == ETestSequence31B || 
		aTC == ETestSequence32A || 
		aTC == ETestSequence32B )
		{
		result = ETrue;
		}
	return result;
	}

/**
 * Given a testcase returns corresponding TLV tag for icon data 
 * that should be used in the testcase
 * @param aTC testcase identifier
 * @return TLV tag value 
 */
TUint8 CCSatSendSMSFU::IconTlvTag(TSendSmTC aTC)
	{
	TUint8 tag;
	if(aTC == ETestSequence31A || aTC == ETestSequence31B)
		{
		tag = KTestIconIdentifierTag;
		}
	else
		{
		tag = KTlvIconIdentifierTag;
		}
	return tag;
	}

/**
 * Given a testcase returns corresponding icon qualifier 
 * that should be used in the testcase
 * @param aTC testcase identifier
 * @return icon qualifier 
 */
TUint8 CCSatSendSMSFU::IconQualifier(TSendSmTC aTC)
	{
	TUint quailfier;
	if(aTC == ETestSequence31A || aTC == ETestSequence31B)
		{
		quailfier = 0; // "self-explanatory"
		}
	else
		{
		quailfier = 1; // "not self-explanatory"
		}
	return quailfier;
	}

/**
 * Given a testcase returns an icon qualifier that is expected to be
 * returned by RSat API
 * @param aTC testcase identifier
 * @return expected icon qualifier 
 */
RSat::TIconQualifier CCSatSendSMSFU::ExpectedIconQualifier(TSendSmTC aTC)
	{
	RSat::TIconQualifier quailfier;
	if(aTC == ETestSequence31A || aTC == ETestSequence31B)
		{
		quailfier = RSat::ESelfExplanatory;
		}
	else
		{
		quailfier = RSat::ENotSelfExplanatory;
		}
	return quailfier;
	}

/**
 * Given a testcase fills RSat::TSendSmRspV1 appropriately
 * @param aTC testcase identifier
 * @param aRsp a response to fill
 */
void CCSatSendSMSFU::FillResponse(RSat::TSendSmRspV1& aRsp,TSendSmTC aTC)
	{
	aRsp.SetPCmdNumber(KTestPCmdNumber);	
	aRsp.iInfoType = RSat::KNoAdditionalInfo;

	switch(aTC)
		{
		case ETestSequence31B:
		case ETestSequence32B:
			aRsp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;
			break;

		case ETestTooBigSmsSubmit7bit:
		case ETestTooBigSmsSubmit8bit:
		case ETestTooBigSmsSubmit7bitPacked:
		case ETestTooBigSmsSubmit8bitPacked:
		case ETestTooBigSmsCommand:
			aRsp.iGeneralResult = RSat::KCmdDataNotUnderstood;
			break;

		case ETestMissingSmsTpdu:
			aRsp.iGeneralResult = RSat::KErrorRequiredValuesMissing;
			break;

		case ETestUnsolictedCompletion:
			aRsp.iGeneralResult = RSat::KMeUnableToProcessCmd;
			aRsp.iInfoType = RSat::KMeProblem;
			aRsp.iAdditionalInfo.Append(0);
			break;
			
		default:
			aRsp.iGeneralResult = RSat::KSuccess;			
		}	
	}

/**
 *  Given a testcase creates appropriate "SMS-SUBMIT" TPDU
 * @param aTC testcase identifier
 * @param aTpdu a TPDU to create
 */
void CCSatSendSMSFU::CreateSmsSubmitTpdu(TTpdu& aTpdu, TSendSmTC aTC)
	{
	// See 3GPP TS 23.040 v8.2.0 clause 9.2.2.2 for details
		
	// TP-MTI (TP-Message-Type-Indicator, first 2 bits)
	// Possible values:
	// 		0x00 (00) SMS-DELIVER (in the direction SC(i.e."Service Center") to MS(i.e."Mobile Station"))
	// 		0x00 (00) SMS-DELIVER REPORT (in the direction MS to SC)
	// 		0x02 (10) SMS-STATUS-REPORT (in the direction SC to MS)
	// 		0x02 (10) SMS-COMMAND (in the direction MS to SC)
	// 		0x01 (01) SMS-SUBMIT (in the direction MS to SC)
	// 		0x01 (01) SMS-SUBMIT-REPORT (in the direction SC to MS)
	// 		0x03 (11) Reserved	
	TUint8 tpduProtocolByte = 0x01; // SMS-SUBMIT        

	// TP-VPF (TP-Validity-Period-Format, 4-th and 5-th bits) 
	// Parameter indicating whether or not the TP-VP field is present
	// Possible values:
	// 		0x00 (00) TP-VP field not present
	// 		0x02 (10) TP-VP field present - relative format
	// 		0x01 (01) TP-VP field present - enhanced format
	// 		0x03 (11) TP-VP field present - absolute format	
	const TUint8 KValidityPeriodFormat = ValidityPeriodFormat(aTC);
	tpduProtocolByte |= KValidityPeriodFormat; 

	aTpdu.Append(tpduProtocolByte);
	
	// TP-MR (TP-Message-Reference)
	aTpdu.Append(0x00);
	
	// TP-DA (TP-Destination-Address)
	const TDesC8& destinationAddress = DestinationAddress(aTC); 
	//Number of semi-octets
	aTpdu.Append(destinationAddress.Length()); 
	//Type of address
	aTpdu.Append(KTestTypeOfAddress);
	// Address value
	TBuf8<KMaxAddressLength >> 1> bcdAddress;
	TSatUtility::AsciiToBCD(destinationAddress,bcdAddress);
	aTpdu.Append(bcdAddress);

	// TP-PID (TP-Protocol-Identifier)
	aTpdu.Append(KShortMessageType0);  
	
	// TP-DCS (TP-Data-Coding Scheme)
 	aTpdu.Append(DataCodingScheme(aTC));

 	// TP-VP (TP-Validity-Period) 
	// Note: there is no need to set any meaningfull TP-VP values.
	// The only thing SAT TSY cares about is TP-VP's length
	if(KValidityPeriodFormat == KSmsVPFRelative)
		{
		// 1-byte TP-VP
		aTpdu.Append(KFakeRelativeValidityValue); 
		}
	else if(KValidityPeriodFormat == KSmsVPFAbsolute || 
			 KValidityPeriodFormat == KSmsVPFEnhanced)
		{
		// 7-bytes TP-VP
		aTpdu.Append(KFakeAbsoluteOrEnchancedValidityValue); 
		}
	
	// TP-UDL (TP-User-Data-Length)
	aTpdu.Append(UserDataLength(aTC));
	
	// TP-UD (TP-User-Data)
	aTpdu.Append(UserData(aTC));		
	}

/**
 *  Creates simple "SMS-COMMAND" TPDU
 * @param aTpdu a TPDU to create
 */
void CCSatSendSMSFU::CreateSmsCommandTpdu(TTpdu& aTpdu)
	{
	// See 3GPP TS 23.040 v8.2.0 clause 9.2.2.4 for details

	// set TP-MTI to "SMS-COMMAND", set TP-UDHI and TP-SRR to zero 
	TUint8 tpduProtocolByte = 0x02;          
	aTpdu.Append(tpduProtocolByte);
	
	// TP-MR (TP-Message-Reference)
	aTpdu.Append(0x00);

	// TP-PID (TP-Protocol-Identifier)
	aTpdu.Append(KShortMessageType0);  

	// TP-CT (TP-Command-Type)
	aTpdu.Append(0x01); // Cancel Status Report Request relating to previously 	submitted short message  

	// TP-MN (TP-Message-Number)
	aTpdu.Append(0x00);

	// TP-DA (TP-Destination-Address)
	//Number of semi-octets
	aTpdu.Append(KDestinationAddressString8().Length()); 
	//Type of address
	aTpdu.Append(KTestTypeOfAddress);
	// Address value
	TBuf8<KMaxAddressLength >> 1> bcdAddress;
	TSatUtility::AsciiToBCD(KDestinationAddressString8,bcdAddress);
	aTpdu.Append(bcdAddress);

	// TP-CDL (TP-Command-Data-Length)
	aTpdu.Append(0x00);
	}

/**
 * Given a testcase creates SMS TPDU with too big TP-UDL or TP-CDL value
 * @param aTC testcase identifier
 * @param aTpdu a TPDU to create
 */
void CCSatSendSMSFU::CreateCorruptedSmsTpdu(TTpdu& aTpdu, TSendSmTC aTC)
	{
	TUint8 smsType;
	if(aTC == ETestTooBigSmsCommand)
		{
		smsType = 0x02; //SMS-COMMAND
		}
	else
		{
		smsType = 0x01; //SMS-SUBMIT		
		}
	aTpdu.Append(smsType);

	// TP-MR (TP-Message-Reference)
	aTpdu.Append(0x00);
	
	if(aTC == ETestTooBigSmsCommand)
		{
		// TP-PID (TP-Protocol-Identifier)
		aTpdu.Append(KShortMessageType0);  

		// TP-CT (TP-Command-Type)
		aTpdu.Append(0x01); // Cancel Status Report Request relating to previously 	submitted short message  

		// TP-MN (TP-Message-Number)
		aTpdu.Append(0x00);		
		}

	// TP-DA (TP-Destination-Address)
	//Number of semi-octets
	aTpdu.Append(KDestinationAddressString8().Length()); 
	//Type of address
	aTpdu.Append(KTestTypeOfAddress);
	// Address value
	TBuf8<KMaxAddressLength >> 1> bcdAddress;
	TSatUtility::AsciiToBCD(KDestinationAddressString8,bcdAddress);
	aTpdu.Append(bcdAddress);
	
	if(aTC != ETestTooBigSmsCommand)
		{
		// TP-PID (TP-Protocol-Identifier)
		aTpdu.Append(KShortMessageType0);  
		
		// TP-DCS (TP-Data-Coding Scheme)
	 	aTpdu.Append(DataCodingScheme(aTC));
		}

	// TP-CDL (TP-Command-Data-Length) / TP-UDL (TP-User-Data-Length)
	aTpdu.Append(KSmsMaxSize + 1);  	
	}

/**
 * Given a testcase creates appropriate SMS TPDU
 * @param aTC testcase identifier
 * @param aTpdu a TPDU to create
 */
void CCSatSendSMSFU::CreateSmsTpdu(TTpdu& aTpdu, TSendSmTC aTC)
	{
	switch(aTC)
		{
		case ETestSmsCommand:
			CreateSmsCommandTpdu(aTpdu);
			break;

		case ETestTooBigSmsSubmit7bit:
		case ETestTooBigSmsSubmit8bit:
		case ETestTooBigSmsSubmit7bitPacked:
		case ETestTooBigSmsSubmit8bitPacked:
		case ETestTooBigSmsCommand:
			CreateCorruptedSmsTpdu(aTpdu,aTC);
			break;
			
		default:
			CreateSmsSubmitTpdu(aTpdu,aTC);		
		}
	}

/**
 * Given a testcase and SMS TPDU creates appropriate "SEND SHORT MESSAGE" TLV data  
 * @param aTlv a TLV to create
 * @param aTpdu SMS TPDU
 * @param aTC testcase identifier
 */
const TDesC8& CCSatSendSMSFU::CreateSendSmTlv(TTlvBase& aTlv, const TTpdu& aTpdu, TSendSmTC aTC)
	{
	aTlv.Begin(KBerTlvProactiveSimCommandTag); 

	// ----------------
	// command details
	// ----------------
	aTlv.AddTag(KTestCommandDetailsTag);
	aTlv.AddByte(KTestPCmdNumber);  
	aTlv.AddByte(KSendShortMessage);
	aTlv.AddByte(CommandQualifier(aTC));

	// ------------------
	// device identities
	// ------------------
	aTlv.AddTag(KTestDeviceIdentityTag);
	aTlv.AddByte(KSim);
	aTlv.AddByte(KNetwork);

	// ---------------------------
	// alpha identifier (optional)
	// ---------------------------	
	if(AlphaIdIsUsed(aTC))
		{
		aTlv.AddTag(KTestAlphaIdentifierTag);
		aTlv.AddData(AlphaId(aTC));
		}
	
	// ------------------
	// address (optional)
	// ------------------
	if(CompleteAddressIsUsed(aTC))
		{
		// add fully initalized address TLV
		aTlv.AddTag(KTestAddressTag);
		aTlv.AddByte(KTestTypeOfAddress); 
	
		// Dialing number string
		TBuf8<RSat::KMaxMobileTelNumberSize >> 1> dialingNumberString;
		TSatUtility::AsciiToBCD(KDialingNumberString8,dialingNumberString);

		aTlv.AddData(dialingNumberString);
		}
	else
		{
		if(aTC == ETestMissingAddressType || aTC == ETestMissingDiallingNumber)
			{
			// add partially initialized address TLV
			// first add the tag ...
			aTlv.AddTag(KTestAddressTag);
			if(aTC == ETestMissingDiallingNumber)
				{
				// ... now add NPI and TON but not the dialling number
				aTlv.AddByte(KTestTypeOfAddress); 
				}
			}
		}
	
	if(aTC != ETestMissingSmsTpdu)
		{
		// --------
		// SMS TPDU
		// --------	
		aTlv.AddTag(KTestSmsTpduTag);
		aTlv.AddData(aTpdu);
		
		// ---------------
		// Icon identifier 
		// ---------------
		if(IsIconRelatedTC(aTC)) 
			{		
			aTlv.AddTag(IconTlvTag(aTC));
			aTlv.AddByte(IconQualifier(aTC)); 			
			aTlv.AddByte(KTestIconIdentifier);
			}
		}

	const TDesC8& result = aTlv.End();

	return result;
	}

/**
 * Given a testcase and filled RSat::TSendSmRspV1 structure creates appropriate "TERMINAL RESPONSE" TLV 
 * @param aTlv a TLV to create
 * @param aCommandQualifier a command qualifer to be placed to TLV 
 * @param aRsp terminal response data
 */
const TDesC8& CCSatSendSMSFU::CreateSendSmTerminalResponseTlv(TTlvBase& aTlv,
																	TUint8 aCommandQualifier,
																	const RSat::TSendSmRspV1& aRsp)
	{
	// ----------------
	// command details
	// ----------------
	aTlv.AddTag(KTestCommandDetailsTag);
	aTlv.AddByte(KTestPCmdNumber);  
	aTlv.AddByte(KSendShortMessage);
	aTlv.AddByte(aCommandQualifier);

	// ------------------
	// device identities
	// ------------------
	aTlv.AddTag(KTestDeviceIdentityTag);
	aTlv.AddByte(KMe);
	aTlv.AddByte(KSim);

	// ------------------
	// result
	// ------------------
	aTlv.AddTag(KTestResultTag);
	aTlv.AddByte(aRsp.iGeneralResult);
	if(aRsp.iInfoType != RSat::KNoAdditionalInfo )
		{
		TBuf8<RSat::KAdditionalInfoMaxSize> info;
		info.Copy(aRsp.iAdditionalInfo);
		aTlv.AddData(info);
		}

	const TDesC8& result = aTlv.GetDataWithoutTopLevelTag();

	return result;	
	}

