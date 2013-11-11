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
 @file The TEFUnit test suite for PlayTone in the SAT.
*/

#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <satcs.h>
#include <ctsy/ltsy/sat_defs.h>


#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <test/mockltsyindicatorids.h>
#include <test/tmockltsydata.h>

#include "ccsatplaytonefu.h"

static const TUint8 KCommandNumber = 1;
static const TUint8 KCommandType   = KPlayTone;
static const TUint8 KSourceDevice = KSim;
static const TUint8 KDestinationDevice = KEarpiece;
static const TUint8 KCmdQualifier = 0;

static const TInt KDelay = 10;

CCSatPlayToneFU::TData::TTone::TTone() 
	{
	Clear();
	}

void CCSatPlayToneFU::TData::TTone::Clear()
	{
	iTone = RSat::KToneNotSet;
	iIfUse = EFalse;
	}

CCSatPlayToneFU::TData::TDuration::TDuration()
	{
	Clear();
	}

void CCSatPlayToneFU::TData::TDuration::Clear()
	{
	iIfUse = EFalse;
	iTimeUnit = KTenthsOfSeconds+1;
	iNumOfUnits = 0;
	}

CTestSuite* CCSatPlayToneFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatPlayToneFU, TestNotifyPlayTonePCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatPlayToneFU, TestNotifyPlayTonePCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatPlayToneFU, TestNotifyPlayTonePCmd0004L);

	ADD_TEST_STEP_ISO_CPP(CCSatPlayToneFU, TestNotifyPlayTonePCmd0001bL);

	END_SUITE;
	}

void CCSatPlayToneFU::PrepareTlvData ( TTlv& aTlvData, TBool aAlphaIdLengthCheck)
	{	
	aTlvData.Begin(KBerTlvProactiveSimCommandTag);
	
	aTlvData.AddTag(KTlvCommandDetailsTag);         // Command details
	aTlvData.AddByte(KCommandNumber);         // Command number     
	aTlvData.AddByte(KCommandType);           // Command type     
	aTlvData.AddByte(KCmdQualifier);      // Command qualifier  
	
	aTlvData.AddTag(KTlvDeviceIdentityTag);         // Device identities          
	aTlvData.AddByte(KSourceDevice);          // Source device     
	aTlvData.AddByte(KDestinationDevice);     // Destination device     

	if (iData.iAlphaId.Length() > 0 || aAlphaIdLengthCheck)
		{
		aTlvData.AddTag(KTlvAlphaIdentifierTag);        // Alpha identifier
		aTlvData.AddData(iData.iAlphaId);
		}

	if ( iData.iTone.iIfUse )
		{
		aTlvData.AddTag(KTlvToneTag);                      // Device identities          
		aTlvData.AddByte(iData.iTone.iTone);                     // Tone
		}

	if ( iData.iDuration.iIfUse )
		{
		aTlvData.AddTag(KTlvDurationTag);
		aTlvData.AddByte(iData.iDuration.iTimeUnit);
		aTlvData.AddByte(iData.iDuration.iNumOfUnits);
		}
	}

void CCSatPlayToneFU::PrepareExpectDataL ()
	{
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TUint8 command = KPlayTone;
	TMockLtsyData1<TUint8> expLtsyData(command);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data);

	CleanupStack::PopAndDestroy(1); // data
	}

void CCSatPlayToneFU::PrepareCompleteDataL ( TInt aResultCode, TBool aAlphaIdLengthCheck )
	{
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TTlv tlv;
	PrepareTlvData( tlv, aAlphaIdLengthCheck );
	
	TPtrC8 tlvDsc = tlv.End();
	TDesC8* tlvDscPtr = &tlvDsc;
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aResultCode, data, KDelay);

	CleanupStack::PopAndDestroy(1); // data
	}

void CCSatPlayToneFU::ProactiveCommandCallL( 
		RSat::TPlayToneV2& aPlayToneData,
		TInt aCompleteErrorCode,
		TInt aResultCode,
		TBool aAlphaIdLengthCheck)
	{
	TRequestStatus requestStatus;
	
	PrepareExpectDataL ();
	
	TPckg< RSat::TPlayToneV2 > setpPck(aPlayToneData);
	iSat.NotifyPlayTonePCmd(requestStatus, setpPck);
		
	PrepareCompleteDataL ( aCompleteErrorCode, aAlphaIdLengthCheck );
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aResultCode, requestStatus.Int());
	}

void CCSatPlayToneFU::TerminalResponseCallL(
		const RSat& aSat,
		RSat::TPCmdResult aGeneralResult)
	{
	RSat::TPlayToneRspV1 playToneRspV2;
	RSat::TPlayToneRspV1Pckg playToneRspV2Pckg(playToneRspV2);
	TRequestStatus requestStatus;
	
	playToneRspV2.iGeneralResult = aGeneralResult;
	playToneRspV2.SetPCmdNumber(KCommandNumber);
	playToneRspV2.iInfoType = RSat::KNoAdditionalInfo;

	PrepareTerminalResponseMockDataL(
			KCommandNumber,
			KCommandType,
			KCmdQualifier,
			KNullDesC16,
			playToneRspV2.iGeneralResult,
			KNullDesC8);

	aSat.TerminalRsp(requestStatus, RSat::EPlayTone, playToneRspV2Pckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	}

void CCSatPlayToneFU::DataInitialization(
		const TData::TTone& aTone,
		const TData::TDuration& aDuration,
		const TDesC8& aAlphaId)
	{
	iData.iAlphaId.Copy(aAlphaId);
	iData.iDuration = aDuration;
	iData.iTone = aTone;
	}

void CCSatPlayToneFU::DataCompareL( const RSat::TPlayToneV2& aPlayToneData )
{
	RSat::TAlphaIdBuf alphaId;
	alphaId.Copy(iData.iAlphaId);
	ASSERT_EQUALS( 0, alphaId.Compare( aPlayToneData.iAlphaId.iAlphaId ) );

	TUint8 durationTimeUnit;
	switch ( aPlayToneData.iDuration.iTimeUnit )
		{
		case RSat::EMinutes:
			{
			// Minutes
			durationTimeUnit = KMinutes;
			break;
			}
		case RSat::ESeconds:
			{
			// Seconds
			durationTimeUnit = KSeconds;
			break;
			}
		case RSat::ETenthsOfSeconds:
			{
			// Tenths of seconds
			durationTimeUnit = KTenthsOfSeconds;
			break;
			}
		default:
			{
			durationTimeUnit = KTenthsOfSeconds+1;
			}
		}

	ASSERT_EQUALS( iData.iDuration.iTimeUnit, durationTimeUnit);
	ASSERT_EQUALS( iData.iDuration.iNumOfUnits, aPlayToneData.iDuration.iNumOfUnits );

	ASSERT_EQUALS( iData.iTone.iTone, aPlayToneData.iTone );
}

//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-PTON-SNPTPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyPlayTonePCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyPlayTonePCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatPlayToneFU::TestNotifyPlayTonePCmd0001L()
	{

	static const RSat::TPCmdResult KGenResultArray[] = {
			RSat::KBackwardModeRequestedByUser,
			RSat::KNoResponseFromUser,
			RSat::KErrorRequiredValuesMissing,
			RSat::KHelpRequestedByUser,
			RSat::KCmdDataNotUnderstood,
			RSat::KCmdBeyondMeCapabilities,
			RSat::KMeUnableToProcessCmd };

	const TInt KSize = sizeof(KGenResultArray)/sizeof(KGenResultArray[0]);

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus requestStatus;

	RSat::TPlayToneV2 playToneData;
	TData::TTone tone;
	TData::TDuration duration;
	DataInitialization(tone, duration);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifyPlayTonePCmd 
 	//-------------------------------------------------------------------------

	// test C - implemented in 0001b function
	
 	//-------------------------------------------------------------------------
	// TEST E: Coverage test(s) for RSat::NotifyPlayTonePCmd
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(requestStatus);
	
	PrepareCompleteDataL ();

	RSat::TAdditionalInfo additionalInfo;
	additionalInfo.Append(KNoCause);
	
	PrepareTerminalResponseMockDataL(
			KCommandNumber,
			KCommandType,
			KCmdQualifier,
			additionalInfo,
			RSat::KMeUnableToProcessCmd,
			KNullDesC8);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyPlayTone::CompleteNotifyL coverage 
 	//-------------------------------------------------------------------------

	ProactiveCommandCallL(playToneData, KErrNone, KErrNone, ETrue);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyPlayTone::CompleteNotifyL coverage 
 	//-------------------------------------------------------------------------

	RSat::TPlayToneRspV1 playToneRspV2;
	RSat::TPlayToneRspV1Pckg playToneRspV2Pckg(playToneRspV2);

	additionalInfo.Zero();
	for (TInt i = 0; i < KSize; i++)
		{
		ProactiveCommandCallL(playToneData);
		
		playToneRspV2.iGeneralResult = KGenResultArray[i];
		playToneRspV2.SetPCmdNumber(KCommandNumber);
		playToneRspV2.iInfoType = RSat::KNoAdditionalInfo;

		RSat::TPCmdResult element = KGenResultArray[i];
		if ( RSat::KCmdBeyondMeCapabilities == element ||
			 RSat::KMeUnableToProcessCmd == element )
			{
			additionalInfo.Zero();
			additionalInfo.Append(KNoCause);
			}

		PrepareTerminalResponseMockDataL(
				KCommandNumber, 
				KCommandType, 
				KCmdQualifier, 
				additionalInfo, 
				playToneRspV2.iGeneralResult,
				KNullDesC8);

		iSat.TerminalRsp(requestStatus, RSat::EPlayTone, playToneRspV2Pckg);
		User::WaitForRequest(requestStatus);

		if ( RSat::KCmdDataNotUnderstood == element ||
			 RSat::KCmdBeyondMeCapabilities == element ||
			 RSat::KMeUnableToProcessCmd == element )
			{
			ASSERT_EQUALS(KErrNone, requestStatus.Int());
			}
		else
			{
			ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
			}
		AssertMockLtsyStatusL();
		}

	//-------------------------------------------------------------------------
	// to increase CSatNotifyPlayTone::CompleteNotifyL coverage 
 	//-------------------------------------------------------------------------

	playToneRspV2.iGeneralResult = RSat::KSuccess;
	playToneRspV2.SetPCmdNumber(KCommandNumber);
	playToneRspV2.iInfoType = RSat::KMeProblem;

	PrepareTerminalResponseMockDataL(
			KCommandNumber, 
			KCommandType, 
			KCmdQualifier, 
			KNullDesC16, 
			playToneRspV2.iGeneralResult,
			KNullDesC8);

	iSat.TerminalRsp(requestStatus, RSat::EPlayTone, playToneRspV2Pckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyPlayTone::CompleteNotifyL coverage 
 	//-------------------------------------------------------------------------

	playToneRspV2.iAdditionalInfo.Append( KNoCause );

	PrepareTerminalResponseMockDataL(
			KCommandNumber, 
			KCommandType, 
			KCmdQualifier, 
			KNullDesC16, 
			playToneRspV2.iGeneralResult,
			KNullDesC8);

	iSat.TerminalRsp(requestStatus, RSat::EPlayTone, playToneRspV2Pckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	ProactiveCommandCallL( playToneData, KErrUnknown, KErrUnknown);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1, this); // this

	}


/**
@SYMTestCaseID BA-CSAT-PTON-SNPTPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifyPlayTonePCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyPlayTonePCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatPlayToneFU::TestNotifyPlayTonePCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus requestStatus;

	RSat::TPlayToneV2 playToneV2;
	RSat::TPlayToneV2Pckg playToneV2Pck(playToneV2);

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifyPlayTonePCmd
 	//-------------------------------------------------------------------------

	PrepareExpectDataL();

	iSat.NotifyPlayTonePCmd(requestStatus, playToneV2Pck);

	iSat.CancelAsyncRequest(ESatNotifyPlayTonePCmd);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1); // this

	}

/**
@SYMTestCaseID BA-CSAT-PTON-SNPTPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifyPlayTonePCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyPlayTonePCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatPlayToneFU::TestNotifyPlayTonePCmd0004L()
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

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	_LIT8(KText, "Dial Tone"); 
	TData::TTone tone;
	TData::TDuration duration;
	DataInitialization(tone, duration, KText);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifyPlayTonePCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	PrepareExpectDataL();
	
	RSat::TPlayToneV2 playTone1;
	RSat::TPlayToneV2Pckg playTonePck1(playTone1);
	iSat.NotifyPlayTonePCmd(requestStatus1, playTonePck1);

	RSat::TPlayToneV2 playTone2;
	RSat::TPlayToneV2Pckg playTonePck2(playTone2);
	sat2.NotifyPlayTonePCmd(requestStatus2, playTonePck2);

	PrepareCompleteDataL();

	User::WaitForRequest(requestStatus1);
	User::WaitForRequest(requestStatus2);

	ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	//probably «KErrServerBusy?is more appropriate result here
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	TerminalResponseCallL(iSat);
	TerminalResponseCallL(sat2);

	DataCompareL( playTone1 );
	DataCompareL( playTone2 );

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CSAT-PTON-SNPTPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyPlayTonePCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyPlayTonePCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatPlayToneFU::TestNotifyPlayTonePCmd0001bL()
	{

	_LIT8(KDialTone, "Dial Tone");
	_LIT8(KSubBusy, "Sub. Busy");
	_LIT8(KCongestion, "Congestion");
	_LIT8(KRPAck, "RP Ack");
	_LIT8(KNoRP, "No RP");
	_LIT8(KSpecInfo, "Spec Info");
	_LIT8(KCallWait, "Call Wait");
	_LIT8(KRingTone, "Ring Tone");
	_LIT8(KLongString, "This command instructs the ME to play an audio tone. Upon receiving this\
command, the ME shall check if it is currently in, or in the process of setting up\
(SET-UP message sent to the network, see GSM\"04.08\"(8)), a speech call. - If the ME I");
	_LIT8(KBeep, "Beep");
	_LIT8(KPositive, "Positive");
	_LIT8(KNegative, "Negative");
	_LIT8(KQuick, "Quick");
	_LIT8(KABORT, "<ABORT>");

	const TPtrC8 KTextPCArray[] = 
	{
		KDialTone(),
		KSubBusy(),
		KCongestion(),
		KRPAck(),
		KNoRP(),
		KSpecInfo(),
		KCallWait(),
		KRingTone(),
		KLongString(),
		KBeep(),
		KPositive(),
		KNegative(),
		KQuick(),
		KABORT(),
		KNullDesC8()
	};

	const TUint8 KSizeArray = sizeof(KTextPCArray)/sizeof(KTextPCArray[0]);

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	//
	// See ETSI TS 102 384 V6.5.0 (2007-02) in subclause 27.22.4.5.1 
	// Expected Sequence 1.1.1 - 1.1.14
	//

	for (TUint8 i = 0; i < KSizeArray; i++)
		{
		RSat::TPCmdResult generalResult = RSat::KSuccess;
		TData::TDuration duration;
		TData::TTone tone;
		RSat::TPlayToneV2 playToneData;

		if ( i <= 7 )
			{
			duration.iNumOfUnits = 0x05;
			}
		else if ( i >= 9 && i <= 11 )
			{
			duration.iNumOfUnits = 0x01;
			}

		duration.iTimeUnit = KSeconds;
		duration.iIfUse = ETrue;
		tone.iIfUse = ETrue;

		switch( i )
			{
			case 0:
				{
				// in case of Expected Sequence 1.1.1
				tone.iTone = RSat::KDialTone;
				break;
				}
			case 1:
				{
				// in case of Expected Sequence 1.1.2
				tone.iTone = RSat::KCalledSubscriberBusy;
				break;
				}
			case 2:
				{
				// in case of Expected Sequence 1.1.3
				tone.iTone = RSat::KCongestion;
				break;
				}
			case 3:
				{
				// in case of Expected Sequence 1.1.4
				tone.iTone = RSat::KRadioPathAcknowledge;
				break;
				}
			case 4:
				{
				// in case of Expected Sequence 1.1.5
				tone.iTone = RSat::KRadioPathNotAvailableCallDropped;
				break;
				}
			case 5:
				{
				// in case of Expected Sequence 1.1.6
				tone.iTone = RSat::KErrorSpecialInfo;
				break;
				}
			case 6:
				{
				// in case of Expected Sequence 1.1.7
				tone.iTone = RSat::KCallWaitingTone;
				break;
				}
			case 7:
				{
				// in case of Expected Sequence 1.1.8
				tone.iTone = RSat::KRingingTone;
				break;
				}
			case 9:
				{
				// in case of Expected Sequence 1.1.10
				tone.iTone = RSat::KGeneralBeep;
				break;
				}
			case 10:
				{
				// in case of Expected Sequence 1.1.11
				tone.iTone = RSat::KPositiveTone;
				break;
				}
			case 11:
				{
				// in case of Expected Sequence 1.1.12
				tone.iTone = RSat::KNegativeTone;
				break;
				}
			case 12:
				{
				// in case of Expected Sequence 1.1.13
				duration.iNumOfUnits = 0x02;
				tone.iTone = RSat::KGeneralBeep;
				break;
				}
			case 13:
				{
				// in case of Expected Sequence 1.1.14
				duration.iNumOfUnits = 0x01;
				duration.iTimeUnit = KMinutes;
				tone.iTone = RSat::KErrorSpecialInfo;
				generalResult = RSat::KPSessionTerminatedByUser;
				break;
				}
			default:
				{
				// in case of Expected Sequence 1.1.9
				duration.iIfUse = EFalse;
				tone.iIfUse = EFalse;
				duration.iTimeUnit = KTenthsOfSeconds+1;
				}
				break;
			}

		DataInitialization(
				tone,
				duration,
				KTextPCArray[i]);
		ProactiveCommandCallL(playToneData);
		TerminalResponseCallL(
				iSat,
				generalResult);
		AssertMockLtsyStatusL();

		DataCompareL( playToneData );
		}

	CleanupStack::PopAndDestroy(this); // this

	}



