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
 @file The TEFUnit test suite for GetChannelStatus in the SAT.
*/

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


#include "ccsatgetchannelstatusfu.h"

const TUint KDelay = 10;
const TUint8 KCmdId = 1;
const TUint8 KCmdQualifier = 0x00;
const TUint8 KNoChannelStatusByte1 = 0x00;
const TUint8 KChannelStatusByte2 = 0x00;
const TUint8 KChannel1StatusOk = 0x81;

CTestSuite* CCSatGetChannelStatusFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatGetChannelStatusFU, TestNotifyGetChannelStatusPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatGetChannelStatusFU, TestNotifyGetChannelStatusPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatGetChannelStatusFU, TestNotifyGetChannelStatusPCmd0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-CHAN-SGCHANSPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RSat::NotifyGetChannelStatusPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyGetChannelStatusPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetChannelStatusFU::TestNotifyGetChannelStatusPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	RBuf8 data;
	CleanupClosePushL(data);

	TTlv tlv;	
	
	PrepareTlv( tlv );
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	TRequestStatus notifyStatus;
	RSat::TGetChannelStatusV2 chan;
	RSat::TGetChannelStatusV2Pckg chanPck(chan);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of RSat::NotifyGetChannelStatusPCmd,
	// no channel connected, passing no data about channels
 	//-------------------------------------------------------------------------
	PrepareDispatchL();

	iSat.NotifyGetChannelStatusPCmd(notifyStatus, chanPck);

	CompleteDispatchL( *tlvDscPtr );
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS( KCmdId, chan.PCmdNumber());

	//terminal response	
	RSat::TGetChannelStatusRspV2 terminalRsp;
	RSat::TGetChannelStatusRspV2Pckg terminalRspPckg(terminalRsp);
	terminalRsp.SetPCmdNumber(KCmdId);
	terminalRsp.iGeneralResult = RSat::KSuccess;
	terminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	terminalRsp.iAdditionalInfo = KNullDesC;

	GenerateTerminalResponseL(KCmdId,	KGetChannelStatus,	KCmdQualifier,
			RSat::EGetChannelStatus, terminalRspPckg,	terminalRsp.iAdditionalInfo, 
			terminalRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of RSat::NotifyGetChannelStatusPCmd,
	// no channel connected, passing "No Channel available"
 	//-------------------------------------------------------------------------
	PrepareDispatchL();

	iSat.NotifyGetChannelStatusPCmd(notifyStatus, chanPck);

	CompleteDispatchL( *tlvDscPtr );
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS( KCmdId, chan.PCmdNumber());

	//terminal response	
	terminalRsp.iInfoType = RSat::KChannelStatusInfo;
	terminalRsp.iAdditionalInfo.Append(KNoChannelStatusByte1);
	terminalRsp.iAdditionalInfo.Append(KChannelStatusByte2);

	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvChannelStatusTag );
    termRespAddInfo.AddByte(KNoChannelStatusByte1);	
    termRespAddInfo.AddByte(KNoChannelStatusByte1);	
	
	GenerateTerminalResponseL(KCmdId,	KGetChannelStatus,	KCmdQualifier,
			RSat::EGetChannelStatus, terminalRspPckg,	KNullDesC, 
			terminalRsp.iGeneralResult, termRespAddInfo.GetDataWithoutTopLevelTag());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C3: Successful completion request of RSat::NotifyGetChannelStatusPCmd,
	// Channel 1 connected
 	//-------------------------------------------------------------------------
	PrepareDispatchL();

	iSat.NotifyGetChannelStatusPCmd(notifyStatus, chanPck);

	CompleteDispatchL( *tlvDscPtr );
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS( KCmdId, chan.PCmdNumber());

	//terminal response	
	terminalRsp.iInfoType = RSat::KChannelStatusInfo;
	terminalRsp.iAdditionalInfo.Zero();
	terminalRsp.iAdditionalInfo.Append(KChannel1StatusOk);
	terminalRsp.iAdditionalInfo.Append(KChannelStatusByte2);

	termRespAddInfo.Begin(KTlvChannelStatusTag);
    termRespAddInfo.AddByte(KChannel1StatusOk);	
    termRespAddInfo.AddByte(KChannelStatusByte2);	
	
	GenerateTerminalResponseL(KCmdId,	KGetChannelStatus,	KCmdQualifier,
			RSat::EGetChannelStatus, terminalRspPckg,	KNullDesC, 
			terminalRsp.iGeneralResult, termRespAddInfo.GetDataWithoutTopLevelTag());

	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C4: Testing all variants of terminal response 
 	//-------------------------------------------------------------------------
	
	DoTestTerminalResponseL();	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	PrepareDispatchL();

	iSat.NotifyGetChannelStatusPCmd(notifyStatus, chanPck);

	CompleteDispatchL( *tlvDscPtr, KErrUnknown );
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrUnknown, notifyStatus.Int());

	AssertMockLtsyStatusL();	
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifyGetChannelStatusPCmd from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	    

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrNone, data, KDelay);

	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);

	
	PrepareTerminalResponseMockDataL( KCmdId, KGetChannelStatus, KCmdQualifier,
			addInfo, RSat::KMeUnableToProcessCmd, KNullDesC8);	
	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-CHAN-SGCHANSPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RSat::NotifyGetChannelStatusPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyGetChannelStatusPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetChannelStatusFU::TestNotifyGetChannelStatusPCmd0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	TRequestStatus notifyStatus;
	RSat::TGetChannelStatusV2 chan;
	RSat::TGetChannelStatusV2Pckg chanPck(chan);

	PrepareDispatchL( );

	iSat.NotifyGetChannelStatusPCmd(notifyStatus, chanPck);
	
	iSat.CancelAsyncRequest(ESatNotifyGetChannelStatusPCmd);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCancel, notifyStatus.Int());

	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(1, this); 
	}


/**
@SYMTestCaseID BA-CSAT-CHAN-SGCHANSPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RSat::NotifyGetChannelStatusPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyGetChannelStatusPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetChannelStatusFU::TestNotifyGetChannelStatusPCmd0004L()
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
	User::LeaveIfError(sat2.Open(phone2));
	CleanupClosePushL(sat2);
	
	TTlv tlv;	
	
	PrepareTlv( tlv );

	TRequestStatus notifyStatus;
	RSat::TGetChannelStatusV2 chan;
	RSat::TGetChannelStatusV2Pckg chanPck(chan);
	
	PrepareDispatchL();

	iSat.NotifyGetChannelStatusPCmd(notifyStatus, chanPck);

	CompleteDispatchL( tlv.End() );

	TRequestStatus notifyStatus2;
	RSat::TGetChannelStatusV2 chan2;
	RSat::TGetChannelStatusV2Pckg chanPck2(chan2);

	sat2.NotifyGetChannelStatusPCmd(notifyStatus2, chanPck2);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	ASSERT_EQUALS( KCmdId, chan.PCmdNumber());
	
	User::WaitForRequest(notifyStatus2);
	
	//probably «KErrServerBusy?is more appropriate result here
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());
	ASSERT_EQUALS( KCmdId, chan2.PCmdNumber());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); 
	}

/**
 * prepare tlv buffer for GetChannelStatus proactive command 
 * @param aTlv - tlv buffer to prepare
 */   	
void CCSatGetChannelStatusFU::PrepareTlv(TTlv& aTlv ) 
	{
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(KCmdId);//ETLV_CommandNumber
	aTlv.AddByte(KGetChannelStatus); //ETLV_TypeOfCommand
	aTlv.AddByte(KCmdQualifier); //ETLV_CommandQualifier

	aTlv.AddTag(KTlvDeviceIdentityTag); 
	aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	aTlv.AddByte(KMe); //ETLV_DestinationDeviceIdentity
	}

/**
 * prepare mock for processing of proactive command
 * @param aTlvDsc - tlv buffer to send
 * @param aRes - result for request completion
 */   	
void CCSatGetChannelStatusFU::CompleteDispatchL(TPtrC8 aTlvDsc, TInt aRes)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TDesC8* tlvDscPtr = &aTlvDsc;
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aRes, data, KDelay);

	CleanupStack::PopAndDestroy(1); 
	}

/**
 * prepare complete for dispatch of sat notification  
 */   	
void CCSatGetChannelStatusFU::PrepareDispatchL()
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TUint8 pcmdCode = KGetChannelStatus;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
 	
	CleanupStack::PopAndDestroy(1); 
	}


/**
 * Tests all possible "TERMINAL RESPONSE" cases. 
 * API should return KErrNone for all valid responses and KErrCorrupt 
 * for those that can not be used with "Get Channel Status" command  
 */
void CCSatGetChannelStatusFU::DoTestTerminalResponseL()
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
			KErrNone
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
			RSat::KNoAdditionalInfo, 
			ETrue,
			0,
			KErrNone
			},
			{
			// 0x21 - Network currently unable to process command;
			RSat::KNetworkUnableToProcessCmd,
			RSat::KNoAdditionalInfo,
			EFalse,
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
			EFalse,
			0,
			KErrCorrupt
			},
			{
			// 0x24 - Action in contradiction with the current timer state;
			RSat::KContradictionWithTimerState,
			RSat::KNoAdditionalInfo,
			EFalse,
			0,
			KErrCorrupt
			},
			{
			// 0x25 - Interaction with call control by NAA, temporary problem;
			RSat::KInteractionWithCCTemporaryError,
			RSat::KNoAdditionalInfo,
			EFalse,
			0,
			KErrCorrupt
			},
			{
			// 0x26 - Launch browser generic error code;
			RSat::KLaunchBrowserError,
			RSat::KNoAdditionalInfo,
			EFalse,
			0,
			KErrCorrupt
			},
			{
			// 0x27 - MMS temporary problem.
			RSat::KMMSTemporaryProblem,
			RSat::KNoAdditionalInfo,
			EFalse,
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
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrCorrupt
			},
			{
			// 0x38 - MultipleCard commands error;
			RSat::KMultipleCardCmdsError,
			RSat::KNoAdditionalInfo,
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
			KErrCorrupt
			},
			{
			// 0x3A - Bearer Independent Protocol error;
			RSat::KBearerIndepProtocolError,
			RSat::KNoAdditionalInfo, 
			EFalse,
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
			RSat::KNoAdditionalInfo,
			EFalse,
			0,
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

	TTlv tlv;	
	
	PrepareTlv( tlv );
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	TRequestStatus notifyStatus;
	RSat::TGetChannelStatusV2 chan;
	RSat::TGetChannelStatusV2Pckg chanPck(chan);

	for(TInt i = 0; i <= KNumberOfResponsesToTest; i++)
		{				
		// notify request
		PrepareDispatchL();
		iSat.NotifyGetChannelStatusPCmd(notifyStatus, chanPck);
		CompleteDispatchL( *tlvDscPtr );		
		User::WaitForRequest(notifyStatus);
		ASSERT_EQUALS(KErrNone, notifyStatus.Int());

		AssertMockLtsyStatusL();	
		
		// now test a "TERMINAL RESPONSE" specific to this loop 
		RSat::RSat::TGetChannelStatusRspV2  rsp;
		RSat::RSat::TGetChannelStatusRspV2Pckg rspPckg(rsp);
		rsp.SetPCmdNumber(KCmdId);

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
			GenerateTerminalResponseL(KCmdId,	KGetChannelStatus,	KCmdQualifier,
					RSat::EGetChannelStatus, rspPckg,	rsp.iAdditionalInfo, 
					rsp.iGeneralResult, KNullDesC8, KResponsesToTest[i].iExpectedResult);
			}
		else
			{
			// now tell Mock to return KErrUnknown when dispatching forthcoming RSat::TerminalRsp()
			rsp.iGeneralResult	= RSat::KSuccess;
			rsp.iInfoType 		= RSat::KNoAdditionalInfo;
			
			PrepareTerminalResponseMockDataL( KCmdId, KGetChannelStatus, KCmdQualifier,
					rsp.iAdditionalInfo, rsp.iGeneralResult, KNullDesC8, KErrUnknown);

			TerminalResponseL( RSat::EGetChannelStatus, rspPckg, KErrUnknown);
			}
		
		AssertMockLtsyStatusL();			
		}
	}
