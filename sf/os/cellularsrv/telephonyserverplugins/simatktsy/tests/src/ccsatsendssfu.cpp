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
 @file The TEFUnit test suite for SendSS in the SAT.
*/

#include "ccsatsendssfu.h"
#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <etelsat.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/mmtsy_names.h>
#include <satcs.h>                 
#include <ctsy/ltsy/sat_defs.h>
#include <cstktsy/ttlv.h>

#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h> 
#include <test/mockltsyindicatorids.h>
#include <test/tmockltsydata.h>


static const TUint8 KCmdId = 1;
static const TUint8 KCmdDetails = 0x00;
// 1 bit always on, ton - international number, npi - isdn
static const TUint8 KInternationalNumIsdn = (0x80 | 0x10 | 0x01);
static const TInt KDelay = 10; 

// please see documentation for RSat::KSsReturnError
// facilityNotSupported (according to TS 29.080), CODE local:21 (defined in TS 29.002) 
static const TUint8 KNetworkError = 0x15;

_LIT8(KNoAlpha, "No Alpha");
_LIT8(KSSString, "**21*01234567890123456789*10#");
_LIT8(KSSStringWithPlus, "**21*+01234567890123456789*10#");

// see 3GPP TS 31.124 TERMINAL RESPONSE: SEND SS 1.1.1A
_LIT16(KSSOpCodeAndParameters,"\x0A\xA0\x1A\x04\x01\x21\x30\x15\x30\x13\x83\
\x01\x00\x84\x01\x07\x89\x0B\x91\x10\x32\x54\x76\x98\x10\x32\x54\x76\x98");


CTestSuite* CCSatSendSSFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatSendSSFU, TestNotifySendSsPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendSSFU, TestNotifySendSsPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendSSFU, TestNotifySendSsPCmd0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-SSS-SNSSPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySendSsPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendSsPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendSSFU::TestNotifySendSsPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	

	TTlv tlv;	
	
	TRequestStatus notifyStatus;
	RSat::TSendSsV1 sendSs; 
	RSat::TSendSsV1Pckg sendSsPck(sendSs); 
	
	RBuf8 data;
	CleanupClosePushL(data);
	
 	//-------------------------------------------------------------------------
	// TEST C1: Successful completion request of RSat::NotifySendSsPCmd
	// with folowing negative terminal response without concrete error value
	//-------------------------------------------------------------------------
	
	_LIT8(KSSString1, "*#31#");
	PrepareTlv( tlv, KSSString1, KNoAlpha);

	PrepareDispatchL();
	iSat.NotifySendSsPCmd(notifyStatus, sendSsPck);
	
	CompleteDispatchL( tlv.End());	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());	
	ASSERT_EQUALS( RSat::EInternationalNumber, sendSs.iSsString.iTypeOfNumber);	
	ASSERT_EQUALS( RSat::EIsdnNumberPlan, sendSs.iSsString.iNumberPlan);	
	ASSERT_EQUALS( 0, sendSs.iSsString.iSsString.Compare(KSSString1));	
	ASSERT_EQUALS( RSat::ENoIconId, sendSs.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::EAlphaIdNotPresent, sendSs.iAlphaId.iStatus);
	ASSERT_EQUALS( KCmdId, sendSs.PCmdNumber());

	RSat::TSendSsRspV2 rsp;
	RSat::TSendSsRspV2Pckg rspPckg(rsp);
	rsp.SetPCmdNumber(KCmdId);
	
	rsp.iGeneralResult = RSat::KSsReturnError;
	rsp.iInfoType = RSat::KSatNetworkErrorInfo;
	
	rsp.iAdditionalInfo.Zero();	
	rsp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);

	GenerateTerminalResponseL(KCmdId,	KSendSs,	KCmdDetails,
			RSat::ESendSs, rspPckg,	rsp.iAdditionalInfo, 
			rsp.iGeneralResult);
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C2: Completion of NotifySendSsPCmd() with a too large SS string in data
	//-------------------------------------------------------------------------

	data.CreateL(RSat::KStringMaxSize);
	data.AppendFill('*', data.MaxLength());
	
	PrepareTlv( tlv, data, KNullDesC8);

	PrepareDispatchL();
	iSat.NotifySendSsPCmd(notifyStatus, sendSsPck);
	
	CompleteDispatchL( tlv.End());	
	
	PrepareTerminalResponseMockDataL( KCmdId, KSendSs, KCmdDetails,
			KNullDesC16, RSat::KCmdDataNotUnderstood, KNullDesC8);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCorrupt, notifyStatus.Int());	

	data.Close();
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifySendSsPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------
	
	PrepareTlv( tlv, KSSString1, KNullDesC8);
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	
	TRequestStatus mocklStatus;
	iMockLTSY.NotifyTerminated(mocklStatus);	    
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrNone, data);
	
	rsp.iAdditionalInfo.Zero();	
	rsp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);

	PrepareTerminalResponseMockDataL( KCmdId, KSendSs, KCmdDetails,
			rsp.iAdditionalInfo, RSat::KMeUnableToProcessCmd, KNullDesC8);
	
	User::WaitForRequest(mocklStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mocklStatus.Int());

	
 	//-------------------------------------------------------------------------
	// TEST C3: Testing all possible results for Terminal Response
	//-------------------------------------------------------------------------
		
	DoTestTerminalResponseL();	

 	//-------------------------------------------------------------------------
	// TEST C4: Successful completion request of RSat::NotifySendSsPCmd
	// with following negative terminal response with RSat::KMeBusyOnSs error 
	//-------------------------------------------------------------------------
	
	PrepareDispatchL();
	iSat.NotifySendSsPCmd(notifyStatus, sendSsPck);
	
	CompleteDispatchL( *tlvDscPtr );	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());	
	ASSERT_EQUALS( RSat::EInternationalNumber, sendSs.iSsString.iTypeOfNumber);	
	ASSERT_EQUALS( RSat::EIsdnNumberPlan, sendSs.iSsString.iNumberPlan);	
	ASSERT_EQUALS( 0, sendSs.iSsString.iSsString.Compare(KSSString1));	
	ASSERT_EQUALS( RSat::ENoIconId, sendSs.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::EAlphaIdNull, sendSs.iAlphaId.iStatus);
	ASSERT_EQUALS( KCmdId, sendSs.PCmdNumber());

	rsp.iGeneralResult = RSat::KSsReturnError;
	rsp.iInfoType = RSat::KSatNetworkErrorInfo;
	rsp.iAdditionalInfo.Zero();
	rsp.iAdditionalInfo.Append(KNetworkError);

	GenerateTerminalResponseL(KCmdId,	KSendSs,	KCmdDetails,
			RSat::ESendSs, rspPckg,	rsp.iAdditionalInfo, rsp.iGeneralResult);
	
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C5: completion of NotifySendSsPCmd() with missing data - passing no SS string
	//-------------------------------------------------------------------------

	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KCmdId);//ETLV_CommandNumber
	tlv.AddByte(KSendSs); //ETLV_TypeOfCommand
	tlv.AddByte(KCmdDetails); //ETLV_CommandQualifier

	PrepareDispatchL();
	iSat.NotifySendSsPCmd(notifyStatus, sendSsPck);
	
	CompleteDispatchL( tlv.End());	

	PrepareTerminalResponseMockDataL( KCmdId, KSendSs, KCmdDetails,
			KNullDesC16, RSat::KErrorRequiredValuesMissing, KNullDesC8);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCorrupt, notifyStatus.Int());	
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST C6: Successful completion request of
	// RSat::NotifySendSsPCmd
	// see 3GPP TS 31.124 clause 27.22.4.11.1.4.2
	//-------------------------------------------------------------------------

	_LIT8(KAlpha, "Call Forward");
	_LIT16(KAlpha16, "Call Forward");
	
	PrepareTlv( tlv, KSSString, KAlpha);
	tlv.End();

	PrepareDispatchL();

	iSat.NotifySendSsPCmd(notifyStatus, sendSsPck);

	CompleteDispatchL( *tlvDscPtr);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());	

	ASSERT_EQUALS( RSat::EInternationalNumber, sendSs.iSsString.iTypeOfNumber);	
	ASSERT_EQUALS( RSat::EIsdnNumberPlan, sendSs.iSsString.iNumberPlan);	
	ASSERT_EQUALS( 0, sendSs.iSsString.iSsString.Compare(KSSStringWithPlus));	
	ASSERT_EQUALS( RSat::ENoIconId, sendSs.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::EAlphaIdProvided, sendSs.iAlphaId.iStatus);
	ASSERT_EQUALS( 0, sendSs.iAlphaId.iAlphaId.Compare(KAlpha16));
	ASSERT_EQUALS( KCmdId, sendSs.PCmdNumber());
	
	//terminal response	
		
	rsp.iGeneralResult = RSat::KSuccess;
	rsp.iInfoType = RSat::KSendSsInfo;
	rsp.iAdditionalInfo.Zero();
	rsp.iAdditionalInfo.Append(KSSOpCodeAndParameters);

// first byte from additional info is deleted in CSatNotifySendSs::TerminalResponseL()
// Let MockLTsy expect this behaviour 
	rsp.iAdditionalInfo.Delete( 0, 1 );
	
    PrepareTerminalResponseMockDataL(
            KCmdId,
            KSendSs,
            KCmdDetails,
            rsp.iAdditionalInfo,
            rsp.iGeneralResult,
            KNullDesC8);

    rsp.iAdditionalInfo.Zero();
    rsp.iAdditionalInfo.Append( KSSOpCodeAndParameters );
    
    TerminalResponseL( RSat::ESendSs, rspPckg);
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	PrepareDispatchL();

	iSat.NotifySendSsPCmd(notifyStatus, sendSsPck);

	CompleteDispatchL( *tlvDscPtr, KErrUnknown);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrUnknown, notifyStatus.Int());	

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CSAT-SSS-SNSSPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySendSsPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySendSsPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendSSFU::TestNotifySendSsPCmd0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifySendSsPCmd
 	//-------------------------------------------------------------------------
	TRequestStatus notifyStatus;
	RSat::TSendSsV1 sendSs; 
	RSat::TSendSsV1Pckg sendSsPck(sendSs); 

	PrepareDispatchL();

	iSat.NotifySendSsPCmd(notifyStatus, sendSsPck);
	
	iSat.CancelAsyncRequest(ESatNotifySendSsPCmd);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS( KErrCancel, notifyStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(1, this); 	
	}



/**
@SYMTestCaseID BA-CSAT-SSS-SNSSPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySendSsPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySendSsPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendSSFU::TestNotifySendSsPCmd0004L()
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
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RSat sat2;	
	ret = sat2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(sat2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifySendSsPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------
	
	TTlv tlv;			
	PrepareTlv( tlv, KSSString, KNullDesC8);
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	PrepareDispatchL();

	TRequestStatus notifyStatus;
	TRequestStatus notifyStatus2;
	RSat::TSendSsV1 sendSs; 
	RSat::TSendSsV1 sendSs2; 
	RSat::TSendSsV1Pckg sendSsPck(sendSs); 
	RSat::TSendSsV1Pckg sendSsPck2(sendSs2); 
	
	iSat.NotifySendSsPCmd(notifyStatus, sendSsPck);
	sat2.NotifySendSsPCmd(notifyStatus2, sendSsPck2);

	CompleteDispatchL( *tlvDscPtr);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());	
	ASSERT_EQUALS( RSat::EInternationalNumber, sendSs.iSsString.iTypeOfNumber);	
	ASSERT_EQUALS( RSat::EIsdnNumberPlan, sendSs.iSsString.iNumberPlan);	
	ASSERT_EQUALS( 0, sendSs.iSsString.iSsString.Compare(KSSStringWithPlus));	
	ASSERT_EQUALS( RSat::ENoIconId, sendSs.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::EAlphaIdNull, sendSs.iAlphaId.iStatus);
	ASSERT_EQUALS( KCmdId, sendSs.PCmdNumber());

	User::WaitForRequest(notifyStatus2);
	//probably «KErrServerBusy?is more appropriate result here
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());	
	ASSERT_EQUALS( RSat::EInternationalNumber, sendSs2.iSsString.iTypeOfNumber);	
	ASSERT_EQUALS( RSat::EIsdnNumberPlan, sendSs2.iSsString.iNumberPlan);	
	ASSERT_EQUALS( 0, sendSs2.iSsString.iSsString.Compare(KSSStringWithPlus));	
	ASSERT_EQUALS( RSat::ENoIconId, sendSs2.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::EAlphaIdNull, sendSs2.iAlphaId.iStatus);
	ASSERT_EQUALS( KCmdId, sendSs2.PCmdNumber());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); //sat2 phone2 telServer2 this	
	}



/**
* prepare tlv buffer for refresh proactive command 
* @param aTlv - tlv buffer to prepare
* @param aCmdQualifier - type of command
*/   	
void CCSatSendSSFU::PrepareTlv(TTlv& aTlv, const TDesC8& aSsString, const TDesC8& aAlpha) 
	{
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(KCmdId);//ETLV_CommandNumber
	aTlv.AddByte(KSendSs); //ETLV_TypeOfCommand
	aTlv.AddByte(KCmdDetails); //ETLV_CommandQualifier

	aTlv.AddTag(KTlvDeviceIdentityTag); 
	aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	aTlv.AddByte(KNetwork); //ETLV_DestinationDeviceIdentity
	
	if(0 != aAlpha.Compare(KNoAlpha))
		{			
		aTlv.AddTag(KTlvAlphaIdentifierTag);
		aTlv.AddData(aAlpha);//ETLV_AlphaIdentifier
		}
	
	aTlv.AddTag(KTlvSsStringTag);
	aTlv.AddByte(KInternationalNumIsdn); // TON NPI
	TBuf8<RSat::KStringMaxSize> tempString;
	TSatUtility::AsciiToBCD( aSsString, tempString );
	aTlv.AddData(tempString);	//SS string		
	}

/**
* prepare mock complete for processing of proactive command
* @param aTlvDsc - tlv buffer to send
* @param aRes - result for request completion
*/   	
void CCSatSendSSFU::CompleteDispatchL(const TDesC8& aTlvDsc, TInt aRes)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TDesC8* tlvDscPtr = const_cast<TDesC8*>(&aTlvDsc);
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aRes, data, KDelay);

	CleanupStack::PopAndDestroy(1); 
	}

/**
* prepare mock for dispatch of sat notification  
*/   	
void CCSatSendSSFU::PrepareDispatchL()
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TUint8 pcmdCode = KSendSs;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
 	
	CleanupStack::PopAndDestroy(1); 
	}


/**
 * Tests all possible "TERMINAL RESPONSE" cases. 
 * API should return KErrNone for all valid responses and KErrCorrupt 
 * for those that can not be used with "Send SS" command  
 */
void CCSatSendSSFU::DoTestTerminalResponseL()
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
			// 0x00 - Command performed successfully;
			// no additional info provided
			RSat::KPartialComprehension,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x01 - Command performed with partial comprehension;
			// no additional info provided
			RSat::KPartialComprehension,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x01 - Command performed with partial comprehension;
			// correct additional info provided
			RSat::KPartialComprehension,
			RSat::KSendSsInfo,
			EFalse,
			0,
			KErrNone
			},
			{
			// 0x02 - Command performed, with missing information;
			// no additional info provided
			RSat::KMissingInformation,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x02 - Command performed, with missing information;
			// correct additional info provided
			RSat::KMissingInformation,
			RSat::KSendSsInfo,
			EFalse,
			0,
			KErrNone
			},
			{
			// 0x03 - REFRESH performed with additional EFs read;
			// no additional info provided
			RSat::KRefreshAdditionEFRead,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x03 - REFRESH performed with additional EFs read;
			// correct additional info provided
			RSat::KRefreshAdditionEFRead,
			RSat::KSendSsInfo,
			EFalse,
			0,
			KErrCorrupt
			},
			{ 
			// 0x04 - Command performed successfully, but requested icon could not be displayed;
			// no additional info provided
			RSat::KSuccessRequestedIconNotDisplayed,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{ 
			// 0x04 - Command performed successfully, but requested icon could not be displayed;
			// correct additional info provided
			RSat::KSuccessRequestedIconNotDisplayed,
			RSat::KSendSsInfo,
			EFalse,
			0,
			KErrNone
			},
			{
			// 0x05 - Command performed, but modified by call control by NAA;
			RSat::KModifiedByCallControl,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
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
			RSat::KNoSpecificMeProblem,
			KErrNone
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
			RSat::KMeProblem,
			EFalse,
			RSat::KNoSpecificBrowserError,
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
			KErrNone
			},
			{ 
			// 0x32 - Command data not understood by terminal;
			RSat::KCmdDataNotUnderstood,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			{
			// 0x33 - Command number not known by terminal;
			RSat::KCmdNumberNotKnown,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
			},
			//0x34  is processed in a special way, checked in TestNotifySendSsPCmd0001L() 
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
			KErrCorrupt
			},
			{
			// 0x38 - MultipleCard commands error;
			RSat::KMultipleCardCmdsError,
			RSat::KMeProblem,
			EFalse,
			RSat::KNoSpecificMultiCardError,
			KErrCorrupt
			},
			{
			// 0x39 - Interaction with call control by NAA, permanent problem;
			RSat::KInteractionWithCCPermanentError,
			RSat::KMeProblem,
			EFalse,
			RSat::KNoSpecificInteractionError,
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
			RSat::KMeProblem,
			EFalse,
			RSat::KNoSpecificCauseCanBeGiven,
			KErrCorrupt
			},
			{
			// 0x3D - MMS Error.
			RSat::KMMSError,
			RSat::KMeProblem,
			EFalse,
			0,
			KErrCorrupt
			}
		};	

	const TInt KNumberOfResponsesToTest = sizeof(KResponsesToTest)/ sizeof(KResponsesToTest[0]); 

	TTlv tlv;	
	PrepareTlv( tlv, KSSString, KNullDesC8);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	
	TRequestStatus notifyStatus;
	RSat::TSendSsV1 sendSs; 
	RSat::TSendSsV1Pckg sendSsPck(sendSs); 

	RSat::TSendSsRspV2* rsp = new(ELeave) RSat::TSendSsRspV2();
	CleanupStack::PushL(rsp);
	RSat::TSendSsRspV2Pckg* rspPckg = new(ELeave) RSat::TSendSsRspV2Pckg(*rsp);
	CleanupStack::PushL(rspPckg);
	
	for(TInt i = 0; i <= KNumberOfResponsesToTest; i++)
		{				
		// notify request
		PrepareDispatchL();
		iSat.NotifySendSsPCmd(notifyStatus, sendSsPck);
		CompleteDispatchL( *tlvDscPtr );
		User::WaitForRequest(notifyStatus);
		ASSERT_EQUALS(KErrNone, notifyStatus.Int());

		AssertMockLtsyStatusL();	
		
		// now test a "TERMINAL RESPONSE" specific to this loop 
		rsp->SetPCmdNumber(KCmdId);
		rsp->iAdditionalInfo.Zero();

		if(i < KNumberOfResponsesToTest)
			{
			// looping through KResponsesToTest table
			rsp->iGeneralResult	= KResponsesToTest[i].iGeneralResult;
			rsp->iInfoType 		= KResponsesToTest[i].iInfoType;
			if ( !KResponsesToTest[i].iIntentionallyOmmitAdditionalInfo )
				{
				if ( rsp->iInfoType == RSat::KSendSsInfo )
					{
					rsp->iAdditionalInfo.Append( KSSOpCodeAndParameters );
					
// first byte from additional info is deleted in CSatNotifySendSs::TerminalResponseL()
// Let MockLTsy expect this behaviour 
					if( rsp->iGeneralResult == RSat::KSuccess ||
					       rsp->iGeneralResult == RSat::KPartialComprehension ||
					       rsp->iGeneralResult == RSat::KMissingInformation ||
					       rsp->iGeneralResult == RSat::KSuccessRequestedIconNotDisplayed )
					    {
					    rsp->iAdditionalInfo.Delete( 0, 1 );
					    }
					}
				else
					{
					rsp->iAdditionalInfo.Append( KResponsesToTest[i].iAdditionalInfo );
					}
				}
			PrepareTerminalResponseMockDataL(
			        KCmdId,
			        KSendSs,
			        KCmdDetails,
			        rsp->iAdditionalInfo,
			        rsp->iGeneralResult,
			        KNullDesC8,
			        KResponsesToTest[i].iExpectedResult);
			
			if ( (!KResponsesToTest[i].iIntentionallyOmmitAdditionalInfo && rsp->iInfoType == RSat::KSendSsInfo ) 
			     &&
			     (rsp->iGeneralResult == RSat::KSuccess ||
                       rsp->iGeneralResult == RSat::KPartialComprehension ||
                       rsp->iGeneralResult == RSat::KMissingInformation ||
                       rsp->iGeneralResult == RSat::KSuccessRequestedIconNotDisplayed) )
			    {
                rsp->iAdditionalInfo.Zero();
                rsp->iAdditionalInfo.Append( KSSOpCodeAndParameters );
			    }
			
			TerminalResponseL( RSat::ESendSs, *rspPckg, KResponsesToTest[i].iExpectedResult);
			}
		else
			{
			// now tell Mock to return KErrUnknown when dispatching forthcoming RSat::TerminalRsp()
			rsp->iGeneralResult	= RSat::KSuccess;
			rsp->iInfoType 		= RSat::KSendSsInfo;
			rsp->iAdditionalInfo.Append(KSSOpCodeAndParameters);

// first byte from additional info is deleted in CSatNotifySendSs::TerminalResponseL()
// Let MockLTsy expect this behaviour 

			rsp->iAdditionalInfo.Delete( 0, 1 );
			
			PrepareTerminalResponseMockDataL( KCmdId, KSendSs, KCmdDetails,
					rsp->iAdditionalInfo, rsp->iGeneralResult, KNullDesC8, KErrUnknown);

			rsp->iAdditionalInfo.Zero();
			rsp->iAdditionalInfo.Append( KSSOpCodeAndParameters );
			
			TerminalResponseL( RSat::ESendSs, *rspPckg, KErrUnknown);
			}
		
		AssertMockLtsyStatusL();		
		}	
	
	CleanupStack::PopAndDestroy(2); 	
	}



