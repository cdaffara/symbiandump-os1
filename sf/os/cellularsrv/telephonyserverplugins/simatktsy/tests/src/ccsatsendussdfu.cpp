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
 @file The TEFUnit test suite for SendUSSD in the SAT.
*/

#include "ccsatsendussdfu.h"
#include <etel.h>
#include <etelmm.h>

#include <test/tmockltsydata.h>
#include <cstktsy/ttlv.h>
#include <satcs.h>                  // Etel SAT IPC definitions
#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <test/mockltsyindicatorids.h>

// It means that text is uncompressed and bits 1 to 0 are reserved and have no message class meaning 
//Reference: 3gpp ts 23.038
static const TUint8 KDataCodingGroup4 = 0x40; 
static const TUint8 KPcmdNumber = 1;

_LIT8(KAlphaId8,"7-bit USSD");
_LIT16(KAlphaId16,"7-bit USSD");

CTestSuite* CCSatSendUSSDFU::CreateSuiteL(const TDesC& aName)
	{	
	SUB_SUITE;
	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0002L);	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0004L);
	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001bL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001cL);
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001dL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001eL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001fL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001gL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001hL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001iL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001jL);	
	ADD_TEST_STEP_ISO_CPP(CCSatSendUSSDFU, TestNotifySendUssdPCmd0001kL);
	
	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySendUssdPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
		
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifySendSsPCmd
	// from LTSY. (put this test first just for coverage)
 	//-------------------------------------------------------------------------
	
	// Prepare TLV buffer for CompleteL	
	TTlv localInfoTlv;		
	
	// prepare 7-bit string	
	PrepareTlvWith7BitStr( localInfoTlv, KAlphaId8);
	const TDesC8& tlv = localInfoTlv.End();
		
	// Prepare TLV buffer for CompleteL	
	iMockLTSY.NotifyTerminated(requestStatus);
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(tlv, KErrNone);  
	
	// as TerminalResponse is called from CTSY
	// Prepare here ExpectL for it
	const TUint8 KNoCause = 0;
	PrepareTerminalResponseMockDataWithExpL(KPcmdNumber,	
			0,KNullDesC8, RSat::KMeUnableToProcessCmd,	KNoCause);
			    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(tlv, KErrUnknown);		    
		    
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrUnknown , requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifySendUssdPCmd 
 	//-------------------------------------------------------------------------

	// see test TestNotifySendUssdPCmd0001bL - TestNotifySendUssdPCmd0001iL
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySendUssdPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();	
	OpenSatL();

	TRequestStatus requestStatus;	

	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
			
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
	
	iSat.CancelAsyncRequest(ESatNotifySendUssdPCmd);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());		
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // this
	}


/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySendUssdPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0004L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);

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
	User::LeaveIfError(sat2.Open(phone2));
	CleanupClosePushL(sat2);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifySendUssdPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------
	
	// Post First Notify 
	PrepareMockLDataWithExpL(KSendUssd);
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
	
	// Post Second Notify	
	RSat::TSendUssdV1 ussd2;
	RSat::TSendUssdV1Pckg ussdPck2(ussd2);
	sat2.NotifySendUssdPCmd(requestStatus2, ussdPck2);		
	
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;	
	_LIT(KUssdString,"ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz-1234567890");
	TPtrC strToCompare(KUssdString);
	
	// prepare 7-bit string	
	PrepareTlvWith7BitStr( localInfoTlv, KAlphaId8);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		 		    
	
	// wait for the first Complete
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(RSat::EAlphaIdProvided, ussd.iAlphaId.iStatus);
	ASSERT_EQUALS(0, ussd.iAlphaId.iAlphaId.Compare(KAlphaId16));	// as we got iAlphaId in UNICODE
	ASSERT_EQUALS(static_cast<TUint8> (ESms7BitDcs), ussd.iUssdString.iDcs);	
	ASSERT_EQUALS(0, ussd.iUssdString.iUssdString.Compare(strToCompare));	
	ASSERT_EQUALS(RSat::ENoIconId	, ussd.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(0), ussd.iIconId.iIdentifier);
	
	// wait for the second Complete
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());		
	ASSERT_EQUALS(RSat::EAlphaIdProvided, ussd2.iAlphaId.iStatus);
	ASSERT_EQUALS(0, ussd2.iAlphaId.iAlphaId.Compare(KAlphaId16));	// as we got iAlphaId in UNICODE
	ASSERT_EQUALS(static_cast<TUint8> (ESms7BitDcs), ussd2.iUssdString.iDcs);	
	ASSERT_EQUALS(0, ussd2.iUssdString.iUssdString.Compare(strToCompare));	
	ASSERT_EQUALS(RSat::ENoIconId	, ussd2.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(0), ussd2.iIconId.iIdentifier);
	
		
	_LIT(KTextString,"USSD string received from SS");
	// USSD string received from SS
	// in 7bit codding (taken from 3GPP TS 31.124 V7.4.0 (2008-06)) 
	static const TUint8 textStr[] = {0xD5, 0xE9, 0x94, 0x08, 0x9A, 0xD3, 0xE5, 0x69, 
						0xF7, 0x19, 0x24, 0x2F, 0x8F, 0xCB, 0x69, 0x7B, 0x99,
						0x0C, 0x32, 0xCB, 0xDF, 0x6D, 0xD0, 0x74, 0x0A}; 	
	TBuf8<RSat::KStringMaxSize> text7bit;	
	text7bit.Append(textStr, sizeof (textStr));
	
	// Prepare and call TerminalResponse from the first Client
	PrepareTerminalResponseMockDataWithExpL(
			KPcmdNumber,		
			ESms7BitDcs, 	
			text7bit);		
 	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.iUssdString.iDcs = ESms7BitDcs;
	resp.iUssdString.iUssdString.Copy(KTextString);	
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);	
	
	// Prepare and call TerminalResponse from the second Client	
	// with some error data
	PrepareTerminalResponseMockDataWithExpL(
				KPcmdNumber,		
				ESms7BitDcs, 	
				KNullDesC8,
				RSat::KMeUnableToProcessCmd,
				0,
				EFalse);	
	
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd; 	
	TerminalResponseL(respPckg);
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(5, this); // ..., data, this
	}

/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Expected Sequence 1.1 (SEND USSD, 7-bit data, successful)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001bL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
	
	//
	// Expected Sequence 1.1 (SEND USSD, 7-bit data, successful)
	// see  27.22.4.12 of  3GPP TS 31.124 V7.4.0 (2008-06) 
	//	
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
		
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;	
	_LIT(KUssdString,"ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz-1234567890");
	TPtrC strToCompare(KUssdString);
	
	// prepare 7-bit string	
	PrepareTlvWith7BitStr( localInfoTlv, KAlphaId8);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
		    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(RSat::EAlphaIdProvided, ussd.iAlphaId.iStatus);
	ASSERT_EQUALS(0, ussd.iAlphaId.iAlphaId.Compare(KAlphaId16));	// as we got iAlphaId in UNICODE
	ASSERT_EQUALS(static_cast<TUint8> (ESms7BitDcs), ussd.iUssdString.iDcs);	
	ASSERT_EQUALS(0, ussd.iUssdString.iUssdString.Compare(strToCompare));	
	ASSERT_EQUALS(RSat::ENoIconId	, ussd.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(0), ussd.iIconId.iIdentifier);
	
	// there is no sence in emulation of transmission between ME and USS here
	// so prepare and send Terminal response
	_LIT(KTextString,"USSD string received from SS");
	// USSD string received from SS
	// in 7bit codding (taken from 3GPP TS 31.124 V7.4.0 (2008-06)) 
	static const TUint8 textStr[] = {0xD5, 0xE9, 0x94, 0x08, 0x9A, 0xD3, 0xE5, 0x69, 
						0xF7, 0x19, 0x24, 0x2F, 0x8F, 0xCB, 0x69, 0x7B, 0x99,
						0x0C, 0x32, 0xCB, 0xDF, 0x6D, 0xD0, 0x74, 0x0A}; 	
	TBuf8<RSat::KStringMaxSize> text7bit;	
	text7bit.Append(textStr, sizeof (textStr));
	PrepareTerminalResponseMockDataWithExpL(
			KPcmdNumber,		
			ESms7BitDcs, 	
			text7bit);
		
 	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.iUssdString.iDcs = ESms7BitDcs;
	resp.iUssdString.iUssdString.Copy(KTextString);
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}



/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001c
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Expected Sequence 1.2 (SEND USSD, 8-bit data, successful)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001cL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
	
	//
	// Expected Sequence 1.2 (SEND USSD, 8-bit data, successful)
	// see  27.22.4.12 of  3GPP TS 31.124 V7.4.0 (2008-06) 
	//	
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
		
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;	
	_LIT8(KAlphaId8bit8, "8-bit USSD");
	_LIT16(KAlphaId8bit16, "8-bit USSD");	
	_LIT8(KUssdString,"ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz-1234567890");		
	PrepareTlv(localInfoTlv, ESms8BitDcs|KDataCodingGroup4, KUssdString, KAlphaId8bit8);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
		    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
		
	ASSERT_EQUALS(RSat::EAlphaIdProvided, ussd.iAlphaId.iStatus);
	ASSERT_EQUALS(0, ussd.iAlphaId.iAlphaId.Compare(KAlphaId8bit16)); // as we got 	iAlphaId in UNICODE
	ASSERT_EQUALS(static_cast<TUint8> (ESms8BitDcs|KDataCodingGroup4), ussd.iUssdString.iDcs);	
	
	// prepare 16-bit string	
	TBuf16<RSat::KStringMaxSize> string16bit;	
	string16bit.Copy(KUssdString);	

	ASSERT_EQUALS(0, ussd.iUssdString.iUssdString.Compare(string16bit));	
	ASSERT_EQUALS(RSat::ENoIconId	, ussd.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(0), ussd.iIconId.iIdentifier);
	
	// there is no sence in emulation of transmission  ME <-> USS and ME<->USER  here
	// so prepare and send Terminal response
	_LIT8(KTextString, "USSD string received from SS");
	PrepareTerminalResponseMockDataWithExpL(
			KPcmdNumber,				
			ESms8BitDcs, 	
			KTextString);
		
 	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.iUssdString.iDcs = ESms8BitDcs|KDataCodingGroup4;
	resp.iUssdString.iUssdString.Copy(KTextString);
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001d
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Expected Sequence 1.3 (SEND USSD, UCS2 data, successful)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001dL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
	
	//
	// Expected Sequence 1.3 (SEND USSD, UCS2 data, successful)
	// see  27.22.4.12 of  3GPP TS 31.124 V7.4.0 (2008-06) 
	//	
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
		
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;
	
	// prepare UCS2 string	
	// "Hello" in Russian
	// (taken from 3GPP TS 31.124 V7.4.0 (2008-06)) 
	TUint8 str[] = {0x04, 0x17, 0x04, 0x14, 0x04, 0x20, 0x04, 0x10, 0x04, 0x12, 0x04,
					0x21, 0x04, 0x22, 0x04, 0x12, 0x04, 0x23, 0x04, 0x19, 0x04, 0x22,
					0x04, 0x15};
	TUint16 str16[] = {	0x0417, 0x0414, 0x0420, 0x0410, 0x0412, 0x0421,
						0x0422, 0x0412, 0x0423, 0x0419, 0x0422,	0x0415};	
	TBuf8<RSat::KStringMaxSize> stringUCS2;
	TBuf<RSat::KStringMaxSize> strToCompare;;
	stringUCS2.Append(str, sizeof (str));
	strToCompare.Append(str16, sizeof (str16)/2);
	PrepareTlv(localInfoTlv, ESms16BitDcs|KDataCodingGroup4, stringUCS2, KAlphaId8);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
		    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(RSat::EAlphaIdProvided, ussd.iAlphaId.iStatus);
	ASSERT_EQUALS(0, ussd.iAlphaId.iAlphaId.Compare(KAlphaId16));	// as we got iAlphaId in UNICODE
	ASSERT_EQUALS(static_cast<TUint8> (ESms16BitDcs|KDataCodingGroup4), ussd.iUssdString.iDcs);	
	ASSERT_EQUALS(0, ussd.iUssdString.iUssdString.Compare(strToCompare));	
	ASSERT_EQUALS(RSat::ENoIconId	, ussd.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(0), ussd.iIconId.iIdentifier);
	
	// there is no sence in emulation of transmission between ME and USS here
	// so prepare and send Terminal response
	_LIT(KTextString,"USSD string received from SS");
	
	// "USSD string received from SS"
	static const TUint8 textStr[] = {0x00, 0x55, 0x00, 0x53, 0x00, 0x53, 0x00,
						0x44, 0x00, 0x20, 0x00, 0x73, 0x00, 0x74, 0x00, 0x72, 0x00, 0x69,
						0x00, 0x6E, 0x00, 0x67, 0x00, 0x20, 0x00, 0x72, 0x00, 0x65, 0x00,
						0x63, 0x00, 0x65, 0x00, 0x69, 0x00, 0x76, 0x00, 0x65, 0x00, 0x64,
						0x00, 0x20, 0x00, 0x66, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x6D, 0x00,
						0x20, 0x00, 0x53, 0x00, 0x53};
	TBuf8<RSat::KStringMaxSize> text;	
	text.Append(textStr, sizeof (textStr));
	PrepareTerminalResponseMockDataWithExpL(
			KPcmdNumber,				
			ESms16BitDcs, 	
			text);
		
 	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iUssdString.iDcs = ESms16BitDcs|KDataCodingGroup4;
	resp.iUssdString.iUssdString.Copy(KTextString);
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001e
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Expected Sequence 1.4 ( SEND USSD, 7-bit data, unsuccessful (Return Error) )
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001eL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
	
	//
	// Expected Sequence 1.4 (SEND USSD, 7-bit data, unsuccessful (Return Error))
	// see  27.22.4.12 of  3GPP TS 31.124 V7.4.0 (2008-06) 
	//	
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
		
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;	
	_LIT(KUssdString,"ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz-1234567890");
	TPtrC strToCompare(KUssdString);
		
	// prepare 7-bit string	
	PrepareTlvWith7BitStr( localInfoTlv, KAlphaId8);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
		    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(RSat::EAlphaIdProvided, ussd.iAlphaId.iStatus);
	ASSERT_EQUALS(0, ussd.iAlphaId.iAlphaId.Compare(KAlphaId16));	// as we got iAlphaId in UNICODE
	ASSERT_EQUALS(static_cast<TUint8> (ESms7BitDcs), ussd.iUssdString.iDcs);	
	ASSERT_EQUALS(0, ussd.iUssdString.iUssdString.Compare(strToCompare));	
	ASSERT_EQUALS(RSat::ENoIconId	, ussd.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(0), ussd.iIconId.iIdentifier);
	
	// there is no sence in emulation of transmission between ME and USS here
	// so prepare and send Terminal response	
	const TUint8 EUnknownAlphabet = 0x47; // An error described in 3ggp 31.124
	PrepareTerminalResponseMockDataWithExpL(
			KPcmdNumber,			
			ESms7BitDcs, 	
			KNullDesC8,			
			RSat::KUssdReturnError,
			EUnknownAlphabet);		
 	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KUssdReturnError;		
	resp.iInfoType = RSat::KSatNetworkErrorInfo;	
	resp.iAdditionalInfo.Append(EUnknownAlphabet);
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001f
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Expected Sequence 1.5 ( SEND USSD, 7-bit data, unsuccessful (Reject))
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001fL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
	
	//
	// Expected Sequence 1.5 (SEND USSD, 7-bit data, unsuccessful (Reject))
	// see  27.22.4.12 of  3GPP TS 31.124 V7.4.0 (2008-06) 
	//	
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
		
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;	
	_LIT(KUssdString,"ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz-1234567890");
	TPtrC strToCompare(KUssdString);
	
	// prepare 7-bit string	
	PrepareTlvWith7BitStr( localInfoTlv, KAlphaId8);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
		    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(RSat::EAlphaIdProvided, ussd.iAlphaId.iStatus);
	ASSERT_EQUALS(0, ussd.iAlphaId.iAlphaId.Compare(KAlphaId16));	// as we got iAlphaId in UNICODE
	ASSERT_EQUALS(static_cast<TUint8> (ESms7BitDcs), ussd.iUssdString.iDcs);	
	ASSERT_EQUALS(0, ussd.iUssdString.iUssdString.Compare(strToCompare));	
	ASSERT_EQUALS(RSat::ENoIconId	, ussd.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(0), ussd.iIconId.iIdentifier);
	
	// there is no sence in emulation of transmission between ME and USS here
	// so prepare and send Terminal response		
	PrepareTerminalResponseMockDataWithExpL(
			KPcmdNumber,			
			ESms7BitDcs, 	
			KNullDesC8,
			RSat::KUssdReturnError,
			RSat::KNoSpecificMeProblem);		
 	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KUssdReturnError;		
	resp.iInfoType = RSat::KSatNetworkErrorInfo;	
	resp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}



/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001g
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Expected Sequence 1.7 (SEND USSD, 7-bit data, successful, no alpha identifier)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001gL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
	
	//
	// Expected Sequence 1.7 (SEND USSD, 7-bit data, successful, no alpha identifier)
	// see  27.22.4.12 of  3GPP TS 31.124 V7.4.0 (2008-06) 
	//	
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
		
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;	
	_LIT(KUssdString,"ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz-1234567890");
	TPtrC strToCompare(KUssdString);
	
	// prepare 7-bit string	
	PrepareTlvWith7BitStr( localInfoTlv, KNullDesC8, EFalse);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
		    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(RSat::EAlphaIdNotPresent, ussd.iAlphaId.iStatus);
	ASSERT_EQUALS(static_cast<TUint8> (ESms7BitDcs), ussd.iUssdString.iDcs);	
	ASSERT_EQUALS(0, ussd.iUssdString.iUssdString.Compare(strToCompare));	
	ASSERT_EQUALS(RSat::ENoIconId	, ussd.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(0), ussd.iIconId.iIdentifier);
	
	// there is no sence in emulation of transmission between ME and USS here
	// so prepare and send Terminal response
	_LIT(KTextString,"USSD string received from SS");
	// USSD string received from SS
	// in 7bit codding (taken from 3GPP TS 31.124 V7.4.0 (2008-06)) 
	static const TUint8 textStr[] = {0xD5, 0xE9, 0x94, 0x08, 0x9A, 0xD3, 0xE5, 0x69, 
						0xF7, 0x19, 0x24, 0x2F, 0x8F, 0xCB, 0x69, 0x7B, 0x99,
						0x0C, 0x32, 0xCB, 0xDF, 0x6D, 0xD0, 0x74, 0x0A}; 	
	TBuf8<RSat::KStringMaxSize> text7bit;	
	text7bit.Append(textStr, sizeof (textStr));
	PrepareTerminalResponseMockDataWithExpL(
			KPcmdNumber,		
			ESms7BitDcs, 	
			text7bit);
			
	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.iUssdString.iDcs = ESms7BitDcs;
	resp.iUssdString.iUssdString.Copy(KTextString);
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001h
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Expected Sequence 1.8 (SEND USSD, 7-bit data, successful, null length alpha identifier)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001hL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
	
	//
	// Expected Sequence 1.8 (SEND USSD, 7-bit data, successful, null length alpha identifier)
	// see  27.22.4.12 of  3GPP TS 31.124 V7.4.0 (2008-06) 
	//	
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
		
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;	
	_LIT(KUssdString,"ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz-1234567890");
	TPtrC strToCompare(KUssdString);
	
	// prepare 7-bit string	
	PrepareTlvWith7BitStr( localInfoTlv, KNullDesC8);// KNullDesC8 -- means  null length alpha identifier	
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
		    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(RSat::EAlphaIdNull, ussd.iAlphaId.iStatus);	
	ASSERT_EQUALS(static_cast<TUint8> (ESms7BitDcs), ussd.iUssdString.iDcs);	
	ASSERT_EQUALS(0, ussd.iUssdString.iUssdString.Compare(strToCompare));	
	ASSERT_EQUALS(RSat::ENoIconId	, ussd.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(0), ussd.iIconId.iIdentifier);
	
	// there is no sence in emulation of transmission between ME and USS here
	// so prepare and send Terminal response
	_LIT(KTextString,"USSD string received from SS");
	// USSD string received from SS
	// in 7bit codding (taken from 3GPP TS 31.124 V7.4.0 (2008-06)) 
	static const TUint8 textStr[] = {0xD5, 0xE9, 0x94, 0x08, 0x9A, 0xD3, 0xE5, 0x69, 
						0xF7, 0x19, 0x24, 0x2F, 0x8F, 0xCB, 0x69, 0x7B, 0x99,
						0x0C, 0x32, 0xCB, 0xDF, 0x6D, 0xD0, 0x74, 0x0A}; 	
	TBuf8<RSat::KStringMaxSize> text7bit;	
	text7bit.Append(textStr, sizeof (textStr));
	PrepareTerminalResponseMockDataWithExpL(
			KPcmdNumber,		
			ESms7BitDcs, 	
			text7bit);
		
 	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.iUssdString.iDcs = ESms7BitDcs;
	resp.iUssdString.iUssdString.Copy(KTextString);
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001i
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Expected Sequence 2.1A (SEND USSD, 7-bit data, successful, basic icon self explanatory, successful)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001iL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
	
	//
	// Expected Sequence 2.1A (SEND USSD, 7-bit data, successful, basic icon self explanatory, successful)
	// see  27.22.4.12 of  3GPP TS 31.124 V7.4.0 (2008-06) 
	//	
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
		
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;	
	_LIT(KUssdString,"ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz-1234567890");
	TPtrC strToCompare(KUssdString);
	
	// prepare 7-bit string	
	PrepareTlvWith7BitStr( localInfoTlv, KAlphaId8);
	
	// add data about icon to tlv buffer
	localInfoTlv.AddTag(KTlvIconIdentifierTag);
	TUint8 quailfier = 0x00; // icon is self-explanatory
	localInfoTlv.AddByte(quailfier);
	TUint8 recNumber = 0x01; // record 1 in EF(IMG)
	localInfoTlv.AddByte(recNumber);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
		    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());		
	ASSERT_EQUALS(RSat::EAlphaIdProvided, ussd.iAlphaId.iStatus);
	ASSERT_EQUALS(0, ussd.iAlphaId.iAlphaId.Compare(KAlphaId16));	// as we got iAlphaId in UNICODE
	ASSERT_EQUALS(static_cast<TUint8> (ESms7BitDcs), ussd.iUssdString.iDcs);	
	ASSERT_EQUALS(0, ussd.iUssdString.iUssdString.Compare(strToCompare));	
	ASSERT_EQUALS(RSat::ESelfExplanatory	, ussd.iIconId.iQualifier);
	ASSERT_EQUALS(recNumber, ussd.iIconId.iIdentifier);
	
	// there is no sence in emulation of transmission between ME and USS here
	// so prepare and send Terminal response
	_LIT(KTextString,"USSD string received from SS");
	// USSD string received from SS
	// in 7bit codding (taken from 3GPP TS 31.124 V7.4.0 (2008-06)) 
	static const TUint8 textStr[] = {0xD5, 0xE9, 0x94, 0x08, 0x9A, 0xD3, 0xE5, 0x69, 
						0xF7, 0x19, 0x24, 0x2F, 0x8F, 0xCB, 0x69, 0x7B, 0x99,
						0x0C, 0x32, 0xCB, 0xDF, 0x6D, 0xD0, 0x74, 0x0A}; 	
	TBuf8<RSat::KStringMaxSize> text7bit;	
	text7bit.Append(textStr, sizeof (textStr));
	PrepareTerminalResponseMockDataWithExpL(
			KPcmdNumber,		
			ESms7BitDcs, 	
			text7bit);
		
 	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.iUssdString.iDcs = ESms7BitDcs;
	resp.iUssdString.iUssdString.Copy(KTextString);
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001j
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test for coverage. 
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001jL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	TRequestStatus requestStatus;	
	
	//
	// Test without Ussd string 
	//	
	
	//Prepare data for ExpectL needed by NotifySendUssdPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);
		
	// Post Notify itself
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
	
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, ESms7BitDcs, KNullDesC8, KAlphaId8);
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);	
		    
	// In this case TerminalResponse is called from CTSY
	//so prepare Mock for it 
	PrepareTerminalResponseMockDataWithExpL (KPcmdNumber,		
			ESms7BitDcs, 	
			KNullDesC8,		
			RSat::KErrorRequiredValuesMissing,
			0,
			EFalse, 
			KErrNone,
			EFalse);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	//
	// Test with USSD DCS as a reserved value 
	//	
	
	// Post Notify itself
	PrepareMockLDataWithExpL(KSendUssd);
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
	
	TTlv localInfoTlv2;	
	_LIT8(KSomeString, "SomeString");	
	TUint8 wrongDCS = 0xA0;
	PrepareTlv(localInfoTlv2, wrongDCS, KSomeString, KAlphaId8);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv2.End(), KErrNone);	
		    
	// In this case TerminalResponse is called from CTSY
	//so prepare Mock for it 
	PrepareTerminalResponseMockDataWithExpL (KPcmdNumber,		
			ESms7BitDcs, 	
			KNullDesC8,		
			RSat::KCmdDataNotUnderstood,
			0,
			EFalse, 
			KErrNone,
			EFalse);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());		
	AssertMockLtsyStatusL();
	
	//
	// Test CSatNotifySendUssd::TerminalResponseL
	// with USSD DCS wrong and RSat::KTextStringas info type
	//
	
	//and post that ExpectL
	PrepareMockLDataWithExpL(KSendUssd);		
	// Post Notify itself	
	iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
	
	TTlv localInfoTlv3;	
	PrepareTlv(localInfoTlv3, ESms7BitDcs, KSomeString, KAlphaId8);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv3.End(), KErrNone);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	  
	
	// prepare data for TerminalRsp
	PrepareTerminalResponseMockDataWithExpL (KPcmdNumber,		
			ESmsUnknownOrReservedDcs, 	
			KNullDesC8,		
			RSat::KSuccess,
			0,
			EFalse, 
			KErrNone,
			ETrue);
			
	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;	
	resp.iUssdString.iDcs = wrongDCS;
	resp.iUssdString.iUssdString.Copy(KSomeString);
	TUint8 KSomeAddInfo = 0xAA;
	resp.iAdditionalInfo.Append(KSomeAddInfo);
	resp.iInfoType = RSat::KTextString; 
	resp.SetPCmdNumber(KPcmdNumber);	
	TerminalResponseL(respPckg);

	AssertMockLtsyStatusL();
		
	CleanupStack::PopAndDestroy(2, this); // data, this
	}




/**
@SYMTestCaseID BA-CSAT-SUSSD-SNSUPC-0001k
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test CSatNotifySendUssd::CreateTerminalRespL()
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendUssdPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendUSSDFU::TestNotifySendUssdPCmd0001kL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// test all possible parameters for CSatNotifySendUssd::CreateTerminalRespL()
	
	DoTestTerminalResponseL();	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); //this
	}




//----------------------------------------------------------------------------------------------------------
// Utility functions
//----------------------------------------------------------------------------------------------------------

/**
*  Prepare data for ExpectL() for NotifySendUssdPCmd() and post ExpectL.	
* @param aPCmdNumber - command number	 
*/
void CCSatSendUSSDFU::PrepareMockLDataWithExpL( TUint8 aPCmdNumber )
	{
	TPckg<TUint8> pcmdCodePckg(aPCmdNumber);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, pcmdCodePckg);
	}

/**
* prepare tlv buffer for SEND USSD proactive command 
* @param aTlv - tlv buffer to prepare
* @param aDataCodingScheme -  
* @param aUssdString - Ussd String
* @param aAlphaIdentifier - Alpha Identifier
* @param aIsAlphaIdentifierUsed - flag, "If AlphaIdentifier is used "
*/ 
void CCSatSendUSSDFU::PrepareTlv(TTlv& aTlv, TUint8 aDataCodingScheme, const TDesC8& aUssdString,
								 const TDesC8& aAlphaIdentifier, TBool aIsAlphaIdentifierUsed)
	{	
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(0x01);//ETLV_CommandNumber
	aTlv.AddByte(KSendUssd); //ETLV_TypeOfCommand
	aTlv.AddByte(0x00); //ETLV_CommandQualifier	
	aTlv.AddTag(KTlvDeviceIdentityTag); 
	aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	aTlv.AddByte(KNetwork);
	
	if (aIsAlphaIdentifierUsed)
		{
		aTlv.AddTag(KTlvAlphaIdentifierTag); 
		for (TInt i = 0; i < aAlphaIdentifier.Length(); i++)
			{
			aTlv.AddByte(aAlphaIdentifier[i]);
			}
		}
	
	if (aUssdString.Length())
		{
		aTlv.AddTag(KTlvUssdStringTag);
		aTlv.AddByte(aDataCodingScheme);
		for (TInt i = 0; i < aUssdString.Length(); i++)
			{		
			aTlv.AddByte(aUssdString[i]);
			}
		}
		
	}

/**
 * prepare tlv buffer for SEND USSD proactive command 
 * @param aTlv - tlv buffer to prepare
 * @param aAlphaIdentifier -  Alpha Identifier
 * @param aIsAlphaIdentifierUsed - flag, "If AlphaIdentifier is used "
 */ 
void CCSatSendUSSDFU::PrepareTlvWith7BitStr( TTlv& aTlv, 						
							const TDesC8& aAlphaIdentifier, 
							TBool aIsAlphaIdentifierUsed)
	{
	// prepare 7-bit string	
	// ABCDEFGHIJKLMNOPQRSTUVWXYZ-abcdefghijklmnopqrstuvwxyz-1234567890
	// in 7bit codding (taken from 3GPP TS 31.124 V7.4.0 (2008-06)) 
	static const TUint8 str[] = {0x41, 0xE1, 0x90, 0x58, 0x34, 0x1E, 0x91, 0x49, 0xE5, 0x92, 0xD9, 0x74, 0x3E, 0xA1, 0x51, 0xE9, 
					0x94, 0x5A, 0xB5, 0x5E, 0xB1, 0x59, 0x6D, 0x2B, 0x2C, 0x1E, 0x93, 0xCB, 0xE6, 0x33, 0x3A, 0xAD, 
					0x5E, 0xB3, 0xDB, 0xEE, 0x37, 0x3C, 0x2E, 0x9F, 0xD3, 0xEB, 0xF6, 0x3B, 0x3E, 0xAF, 0x6F, 0xC5, 
					0x64, 0x33, 0x5A, 0xCD, 0x76, 0xC3, 0xE5, 0x60};	
	TBuf8<RSat::KStringMaxSize> string7bit;
	string7bit.Append(str, sizeof (str));	
	PrepareTlv(aTlv, ESms7BitDcs, string7bit, aAlphaIdentifier, aIsAlphaIdentifierUsed);
	}



/**
 * prepare mock for processing of proactive command
 * @param aCompleteTlv - tlv buffer to send
 * @param aError - result for request completion
 */ 
void CCSatSendUSSDFU::CompleteMockDispatchSatPcmdL( TPtrC8 aCompleteTlv, TInt aError )
	{
	RBuf8 data;
	CleanupClosePushL(data);	

	TDesC8* tlvPtr = &aCompleteTlv;	
	TMockLtsyData1<TDesC8*> dspTxtDscPack(tlvPtr);
	dspTxtDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aError, data, 10);
	
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * prepare data and call  iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data);	
 * @param aPCmdNumber - command number	 
 * @param aDataCodingScheme - Coding Scheme
 * @param aTextString - string to compare
 * @param aAdditionalInfoTlv -  tlv buffer
 * @param aGeneralResult - for request completion
 * @param aResultAddInfo - result additional info
 */ 
void CCSatSendUSSDFU::PrepareTerminalResponseMockDataWithExpL(
		TUint8 aPCmdNumber,		
		TUint8 aDataCodingScheme, 	
		const TDesC8& aTextString,		
		RSat::TPCmdResult aGeneralResult,
		TUint8 aResultAddInfo,
		TBool  aIsResultAddInfoUsed,
		TInt aErrorForExpect,
		TBool aUseEmptyText)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 terminalRsp;
	CleanupClosePushL(terminalRsp);
	terminalRsp.CreateL(KTlvMaxSize);
		
	TTlv tlv;
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KSendUssd); //ETLV_TypeOfCommand
	tlv.AddByte(0x00); //ETLV_CommandQualifier

	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KMe);
	tlv.AddByte(KSim);
		
	tlv.AddTag(KTlvResultTag);
	tlv.AddByte(aGeneralResult);
	if (RSat::KSuccess != aGeneralResult &&
			aIsResultAddInfoUsed)
		{
		// add result additional info
		tlv.AddByte(aResultAddInfo);	
		}	
	
	if (aTextString.Length())
		{
		tlv.AddTag(KTlvTextStringTag);
		tlv.AddByte(aDataCodingScheme);
	
		for (TInt i = 0; i < aTextString.Length(); i++)
			{
			tlv.AddByte(aTextString[i]);
			}
		}
	
	if (aUseEmptyText && !aTextString.Length() )
		{		
		// just use tags
		tlv.AddTag(KTlvTextStringTag);
		tlv.AddByte(aDataCodingScheme);			
		}

	terminalRsp.Append(tlv.GetDataWithoutTopLevelTag());	
	
	TDesC8* terminalRspPtr = &terminalRsp;	
	TMockLtsyData1<TDesC8*> termRespData(terminalRspPtr);
	termRespData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data, aErrorForExpect);
	
	CleanupStack::PopAndDestroy(&terminalRsp);
	CleanupStack::PopAndDestroy(&data);
	}

/**
 * prepare data and call iSat.TerminalRsp(reqStatus, aRSatTypeOfCommand, aRspPckg);
 * @param aRspPckg - data for response 
 * @param aExpectedError - error, which is expected from CTSY 
 */ 
void CCSatSendUSSDFU::TerminalResponseL(const TDesC8& aRspPckg, TInt aExpectedError)
	{
	TRequestStatus reqStatus;
	iSat.TerminalRsp(reqStatus, RSat::ESendUssd, aRspPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(aExpectedError, reqStatus.Int());
	}

/**
 * Tests all possible "TERMINAL RESPONSE" cases. 
 * API should return KErrNone for all valid responses and KErrCorrupt 
 * for those that can not be used with "SEND USSD" command  
 */
void CCSatSendUSSDFU::DoTestTerminalResponseL()
	{
	static const struct 
		{
		RSat::TPCmdResult			iGeneralResult;
		RSat::TAdditionalInfoType	iInfoType;
		TBool						iIntentionallyOmmitAdditionalInfo; // skip AdditionalInfo
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
			KErrNone
			},
			{
			// 0x02 - Command performed, with missing information;
			RSat::KMissingInformation,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
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
			KErrNone
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
			KErrNone
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
			KErrNone
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
			EFalse,
			0,
			KErrNone
			},
			{
			// 0x31 - Command type not understood by terminal;
			RSat::KCmdTypeNotUnderstood,
			RSat::KNoAdditionalInfo,
			EFalse,
			0,
			KErrNone
			},
			{ 
			// 0x32 - Command data not understood by terminal;
			RSat::KCmdDataNotUnderstood,
			RSat::KNoAdditionalInfo,
			EFalse,
			0,
			KErrNone
			},
			{
			// 0x33 - Command number not known by terminal;
			RSat::KCmdNumberNotKnown,
			RSat::KNoAdditionalInfo,
			EFalse,
			0,
			KErrNone
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
			KErrCorrupt
			},
			{ 
			// 0x36 - Error, required values are missing;
			RSat::KErrorRequiredValuesMissing,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},			
			{
			// 0x37 - reserved for GSM/3G;
			RSat::KUssdReturnError,
			RSat::KSatNetworkErrorInfo,
			EFalse,
			0,
			KErrNone
			},
			{
			// 0x38 - MultipleCard commands error;
			RSat::KMultipleCardCmdsError,
			RSat::KMeProblem,
			EFalse,
			0,
			KErrCorrupt
			},
			{
			// 0x39 - Interaction with call control by NAA, permanent problem;
			RSat::KInteractionWithCCPermanentError,
			RSat::KNoAdditionalInfo,
			EFalse,
			0,
			KErrNone
			},
			
			{
			// 0x3A - Bearer Independent Protocol error;
			RSat::KBearerIndepProtocolError,
			RSat::KMeProblem, 
			EFalse,
			RSat::KNoSpecificBIPError,
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
			EFalse,
			0,
			KErrCorrupt
			},
			{
			// 0x3C - Frames error;
			RSat::KFramesError,
			RSat::KMeProblem,
			EFalse,
			RSat::KNoSpecificCauseCanBeGiven,
			KErrCorrupt
			},
			{
			// 0x3D - MMS Error.
			RSat::KMMSError,
			RSat::KNoAdditionalInfo,
			EFalse,
			0,
			KErrCorrupt
			}
		};	

	const TInt KNumberOfResponsesToTest = sizeof(KResponsesToTest)/ sizeof(KResponsesToTest[0]); 
	
	_LIT(KTextString,"USSD string received from SS");
	// USSD string received from SS
	// in 7bit codding (taken from 3GPP TS 31.124 V7.4.0 (2008-06)) 
	static const TUint8 textStr[] = {0xD5, 0xE9, 0x94, 0x08, 0x9A, 0xD3, 0xE5, 0x69, 
						0xF7, 0x19, 0x24, 0x2F, 0x8F, 0xCB, 0x69, 0x7B, 0x99,
						0x0C, 0x32, 0xCB, 0xDF, 0x6D, 0xD0, 0x74, 0x0A}; 	
	TBuf8<RSat::KStringMaxSize> text7bit;	
	text7bit.Append(textStr, sizeof (textStr));		
	TTlv tlv;	
	
	
	PrepareTlvWith7BitStr(tlv, KAlphaId8);
	const TDesC8& tlvEnd = tlv.End();
	
	TRequestStatus requestStatus;
	RSat::TSendUssdV1 ussd;
	RSat::TSendUssdV1Pckg ussdPck(ussd);
	
	for(TInt i = 0; i < KNumberOfResponsesToTest; i++)
		{	
		//Prepare data for ExpectL needed by NotifySendUssdPCmd
		//and post that ExpectL
		PrepareMockLDataWithExpL(KSendUssd);
				
		// Post Notify itself		
		iSat.NotifySendUssdPCmd(requestStatus, ussdPck);
				
		// post Complete with KErrNone
		CompleteMockDispatchSatPcmdL(tlvEnd, KErrNone);	 
		User::WaitForRequest(requestStatus);
		ASSERT_EQUALS(KErrNone, requestStatus.Int()); // All TSendUssdV1 members were checked in 001b, 001c and so on tests
		AssertMockLtsyStatusL();			
		
		// now test a "TERMINAL RESPONSE" specific to this loop 
		RSat::TSendUssdRspV1 resp;
		RSat::TSendUssdRspV1Pckg respPckg(resp);		
		resp.SetPCmdNumber(KPcmdNumber);

		if(i < KNumberOfResponsesToTest)
			{		
			TPtrC8 expectText(KNullDesC8);
			switch(KResponsesToTest[i].iGeneralResult)
				{
				case RSat::KSuccess:
		        case RSat::KPartialComprehension:
		        case RSat::KMissingInformation:
		        case RSat::KRefreshAdditionEFRead:
		        case RSat::KSuccessRequestedIconNotDisplayed:
		        case RSat::KModifiedByCallControl:
		        case RSat::KSuccessLimitedService:
		        case RSat::KPerformedWithModifications:
		        case RSat::KRefreshUSIMNotActive:
		        	{
		        	expectText.Set(text7bit);
		        	break;
		        	}
		        default:
		        	{
		        	// Do nothing, expectText is set above
		        	}
				}
			TBool expectAdditionalInfo = EFalse;
			if ( ( RSat::KMeUnableToProcessCmd == KResponsesToTest[i].iGeneralResult ) || 
				 ( RSat::KNetworkUnableToProcessCmd == KResponsesToTest[i].iGeneralResult ) || 
				 ( RSat::KUssdReturnError == KResponsesToTest[i].iGeneralResult) || 
				 ( RSat::KMultipleCardCmdsError == KResponsesToTest[i].iGeneralResult) )
				{
				if((KResponsesToTest[i].iInfoType != RSat::KNoAdditionalInfo) && (!KResponsesToTest[i].iIntentionallyOmmitAdditionalInfo))
					{
					expectAdditionalInfo = ETrue;
					}
				}
			// Prepare ExpectL for TerminalResponseL
			PrepareTerminalResponseMockDataWithExpL(
					KPcmdNumber,		
					ESms7BitDcs, 	
					expectText, // text					
					KResponsesToTest[i].iGeneralResult,
					KResponsesToTest[i].iAdditionalInfo,
					expectAdditionalInfo); // isResultAddInfoUsed		
			
			// prepare data for TerminalResponseL
			resp.iGeneralResult = KResponsesToTest[i].iGeneralResult;
			resp.iInfoType 		= KResponsesToTest[i].iInfoType;
			resp.iUssdString.iDcs = ESms7BitDcs;
			resp.iUssdString.iUssdString.Copy(KTextString);
			if(	resp.iInfoType != RSat::KNoAdditionalInfo && 
				!KResponsesToTest[i].iIntentionallyOmmitAdditionalInfo)
				{
				resp.iAdditionalInfo.Append(KResponsesToTest[i].iAdditionalInfo);
				}
			// call TerminalRsp			
			TerminalResponseL(respPckg, KResponsesToTest[i].iExpectedResult);

			}	
		AssertMockLtsyStatusL();			
		}
	
	
	// now tell Mock to return KErrUnknown when dispatching forthcoming RSat::TerminalRsp()
	// something like  "Test A" of 001 test for TerminalResponse	
	PrepareTerminalResponseMockDataWithExpL(
							KPcmdNumber,		
							ESms7BitDcs, 	
							text7bit,							
							RSat::KSuccess,
							0,	
							EFalse,
							KErrUnknown);

	// call TerminalRsp
	RSat::TSendUssdRspV1 resp;
	RSat::TSendUssdRspV1Pckg respPckg(resp);		
	resp.SetPCmdNumber(KPcmdNumber);
	resp.iGeneralResult	= RSat::KSuccess;
	resp.iInfoType 		= RSat::KNoAdditionalInfo;	
	resp.iUssdString.iDcs = ESms7BitDcs;
	resp.iUssdString.iUssdString.Copy(KTextString);
	TerminalResponseL(respPckg, KErrUnknown);
	
	AssertMockLtsyStatusL();
	}

