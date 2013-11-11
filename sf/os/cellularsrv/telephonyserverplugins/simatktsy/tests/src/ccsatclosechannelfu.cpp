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
 @file The TEFUnit test suite for CloseChannel in the SAT.
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


#include "ccsatclosechannelfu.h"

const TUint KDelay = 10;

CTestSuite* CCSatCloseChannelFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatCloseChannelFU, TestNotifyCloseChannelPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatCloseChannelFU, TestNotifyCloseChannelPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatCloseChannelFU, TestNotifyCloseChannelPCmd0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-CHAN-SCCHANPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RSat::NotifyCloseChannelPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyCloseChannelPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatCloseChannelFU::TestNotifyCloseChannelPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	RBuf8 data;
	CleanupClosePushL(data);

	_LIT8(KAlpha, "Alpha");
	_LIT16(KAlpha16, "Alpha");
	TUint8 KPCmdNumber = 1;
	TUint8 cmdQualifier(0x00);	
	TTlv tlv;	
	
	PrepareTlv( tlv, KPCmdNumber, cmdQualifier, KChannel1, KAlpha);
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	TRequestStatus notifyStatus;
	RSat::TCloseChannelV2 closeChan;
	RSat::TCloseChannelV2Pckg closeChanPck(closeChan);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of RSat::NotifyCloseChannelPCmd 
 	//-------------------------------------------------------------------------
	PrepareDispatchL();

	iSat.NotifyCloseChannelPCmd(notifyStatus, closeChanPck);

	CompleteDispatchL( *tlvDscPtr );
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());

	ASSERT_EQUALS( KPCmdNumber, closeChan.PCmdNumber());
	ASSERT_EQUALS( RSat::ENoIconId, closeChan.iIconId.iQualifier);	
	ASSERT_EQUALS( RSat::KChannel1, closeChan.iDestination);
	ASSERT_EQUALS( RSat::EAlphaIdProvided, closeChan.iAlphaId.iStatus);	
	ASSERT_EQUALS(0, closeChan.iAlphaId.iAlphaId.Compare(KAlpha16));	

	//terminal response	
	RSat::TCloseChannelRspV2 terminalRsp;
	RSat::TCloseChannelRspV2Pckg terminalRspPckg(terminalRsp);
	terminalRsp.SetPCmdNumber(KPCmdNumber);
	terminalRsp.iGeneralResult = RSat::KSuccess;
	terminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	terminalRsp.iAdditionalInfo = KNullDesC;

	GenerateTerminalResponseL(KPCmdNumber,	KCloseChannel,	cmdQualifier,
			RSat::ECloseChannel, terminalRspPckg,	terminalRsp.iAdditionalInfo, 
			terminalRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();
	
	// testing all variants of terminal response
	DoTestTerminalResponseL();	
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifyCloseChannelPCmd from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	    

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrNone, data, KDelay);

	RSat::TAdditionalInfo addInfo;
	addInfo.Zero();
	addInfo.Append(RSat::KNoSpecificMeProblem);
	
	PrepareTerminalResponseMockDataL( KPCmdNumber, KCloseChannel, cmdQualifier,
			addInfo, RSat::KMeUnableToProcessCmd, KNullDesC8);	
	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	PrepareDispatchL();

	iSat.NotifyCloseChannelPCmd(notifyStatus, closeChanPck);

	CompleteDispatchL( *tlvDscPtr, KErrUnknown );
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrUnknown, notifyStatus.Int());

	AssertMockLtsyStatusL();	

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifyCloseChannelPCmd
 	//-------------------------------------------------------------------------
	
	// TLV buffer without any channel Id and device identifier
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KCloseChannel); //ETLV_TypeOfCommand
	tlv.AddByte(cmdQualifier); //ETLV_CommandQualifier
	tlv.AddTag(KTlvAlphaIdentifierTag);
	tlv.AddData(KNullDesC8);//ETLV_AlphaIdentifier

	PrepareDispatchL();

	iSat.NotifyCloseChannelPCmd(notifyStatus, closeChanPck);

	CompleteDispatchL( tlv.End() );

	PrepareTerminalResponseMockDataL( KPCmdNumber, KCloseChannel, cmdQualifier,
			KNullDesC, RSat::KErrorRequiredValuesMissing, KNullDesC8);	
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCorrupt, notifyStatus.Int());
	AssertMockLtsyStatusL();
	
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-CHAN-SCCHANPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RSat::NotifyCloseChannelPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyCloseChannelPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatCloseChannelFU::TestNotifyCloseChannelPCmd0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	TRequestStatus notifyStatus;
	RSat::TCloseChannelV2 closeChan;
	RSat::TCloseChannelV2Pckg closeChanPck(closeChan);

	PrepareDispatchL( );

	iSat.NotifyCloseChannelPCmd(notifyStatus, closeChanPck);
	
	iSat.CancelAsyncRequest(ESatNotifyCloseChannelPCmd);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCancel, notifyStatus.Int());

	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(1, this); 
	}


/**
@SYMTestCaseID BA-CSAT-CHAN-SCCHANPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RSat::NotifyCloseChannelPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyCloseChannelPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatCloseChannelFU::TestNotifyCloseChannelPCmd0004L()
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
	
	TUint8 KPCmdNumber = 2;
	TUint8 cmdQualifier(0x00);
	TTlv tlv;	
	
	PrepareTlv( tlv, KPCmdNumber, cmdQualifier, KChannel1);

	TRequestStatus notifyStatus;
	RSat::TCloseChannelV2 closeChan;
	RSat::TCloseChannelV2Pckg closeChanPck(closeChan);
	
	PrepareDispatchL();

	iSat.NotifyCloseChannelPCmd(notifyStatus, closeChanPck);

	CompleteDispatchL( tlv.End() );

	TRequestStatus notifyStatus2;
	RSat::TCloseChannelV2 closeChan2;
	RSat::TCloseChannelV2Pckg closeChanPck2(closeChan2);

	sat2.NotifyCloseChannelPCmd(notifyStatus2, closeChanPck2);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());

	ASSERT_EQUALS( KPCmdNumber, closeChan.PCmdNumber());
	ASSERT_EQUALS( RSat::ENoIconId, closeChan.iIconId.iQualifier);	
	ASSERT_EQUALS( RSat::KChannel1, closeChan.iDestination);
	ASSERT_EQUALS( RSat::EAlphaIdNotPresent, closeChan.iAlphaId.iStatus);
	
	User::WaitForRequest(notifyStatus2);
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());

	ASSERT_EQUALS( KPCmdNumber, closeChan2.PCmdNumber());
	ASSERT_EQUALS( RSat::ENoIconId, closeChan2.iIconId.iQualifier);	
	ASSERT_EQUALS( RSat::KChannel1, closeChan2.iDestination);
	ASSERT_EQUALS( RSat::EAlphaIdNotPresent, closeChan2.iAlphaId.iStatus);
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); 
	}


void CCSatCloseChannelFU::PrepareTlv(TTlv& aTlv, TUint8 aCmdNum, TUint8 aCmdQualifier, 
		TUint8 aChan, const TDesC8& aAlpha ) 
	{
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(aCmdNum);//ETLV_CommandNumber
	aTlv.AddByte(KCloseChannel); //ETLV_TypeOfCommand
	aTlv.AddByte(aCmdQualifier); //ETLV_CommandQualifier

	aTlv.AddTag(KTlvDeviceIdentityTag); 
	aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	aTlv.AddByte(aChan); //ETLV_DestinationDeviceIdentity
	
	if(0 < aAlpha.Size())
		{			
		aTlv.AddTag(KTlvAlphaIdentifierTag);
		aTlv.AddData(aAlpha);//ETLV_AlphaIdentifier
		}
	
	}


void CCSatCloseChannelFU::CompleteDispatchL(TPtrC8 aTlvDsc, TInt aRes)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TDesC8* tlvDscPtr = &aTlvDsc;
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aRes, data, KDelay);

	CleanupStack::PopAndDestroy(1); 
	}

void CCSatCloseChannelFU::PrepareDispatchL()
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TUint8 pcmdCode = KCloseChannel;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
 	
	CleanupStack::PopAndDestroy(1); 
	}


void CCSatCloseChannelFU::DoTestTerminalResponseL()
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
			RSat::KMeProblem, 
			EFalse,
			RSat::KNoSpecificBIPError,
			KErrNone
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
			KErrNone
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

	_LIT8(KAlpha, "Alpha");
	TUint8 KPCmdNumber = 1;
	TUint8 cmdQualifier(0x00);	
	TTlv tlv;	
	
	PrepareTlv( tlv, KPCmdNumber, cmdQualifier, KChannel1, KAlpha);
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	TRequestStatus notifyStatus;
	RSat::TCloseChannelV2 closeChan;
	RSat::TCloseChannelV2Pckg closeChanPck(closeChan);

	for(TInt i = 0; i <= KNumberOfResponsesToTest; i++)
		{				
		// notify request
		PrepareDispatchL();
		iSat.NotifyCloseChannelPCmd(notifyStatus, closeChanPck);
		CompleteDispatchL( *tlvDscPtr );		
		User::WaitForRequest(notifyStatus);
		ASSERT_EQUALS(KErrNone, notifyStatus.Int());

		AssertMockLtsyStatusL();	
		
		// now test a "TERMINAL RESPONSE" specific to this loop 
		RSat::RSat::TCloseChannelRspV2  rsp;
		RSat::RSat::TCloseChannelRspV2Pckg rspPckg(rsp);
		rsp.SetPCmdNumber(KPCmdNumber);

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
			GenerateTerminalResponseL(KPCmdNumber,	KCloseChannel,	cmdQualifier,
					RSat::ECloseChannel, rspPckg,	rsp.iAdditionalInfo, 
					rsp.iGeneralResult, KNullDesC8, KResponsesToTest[i].iExpectedResult);
			}
		else
			{
			// now tell Mock to return KErrUnknown when dispatching forthcoming RSat::TerminalRsp()
			rsp.iGeneralResult	= RSat::KSuccess;
			rsp.iInfoType 		= RSat::KNoAdditionalInfo;
			
			PrepareTerminalResponseMockDataL( KPCmdNumber, KCloseChannel, cmdQualifier,
					rsp.iAdditionalInfo, rsp.iGeneralResult, KNullDesC8, KErrUnknown);

			TerminalResponseL( RSat::ECloseChannel, rspPckg, KErrUnknown);
			}
		
		AssertMockLtsyStatusL();			
		}
	}
