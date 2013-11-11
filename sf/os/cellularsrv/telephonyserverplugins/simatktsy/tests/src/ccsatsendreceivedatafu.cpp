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
 @file The TEFUnit test suite for SendReceiveData in the SAT.
*/

#include "ccsatsendreceivedatafu.h"
#include <etel.h>
#include <etelmm.h>
#include <satcs.h>
#include <test/tmockltsydata.h>

// define used constants
const TUint8 KSendDataStoreMode = 0x00; // constant for store mode
const TUint8 KReceiveDataQualifier = 0x00; // RFU
const TUint8 KIconOne = 1; // record 1 in EFIMG file on sim
const TUint8 KChannelDataLength = 200; // should be non zero

// additional information for RSat::KNetworkUnableToProcessCmd error
// in processing proactive command
// please see documentation for RSat::KNetworkUnableToProcessCmd
const TUint8 KRequestedFacilityNotImplemented = 69/*see 3GPP TS 24.008*/ | 0x80;

// define some channel data
_LIT8(KChannelData, "\x00\x01\x02\x03\x04\x05\x06\x07");

_LIT8(KBasicIconAlpha, "Basic Icon");

//"Hello" in Russian, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
_LIT8(KRussianHello, "\x80\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\
\x04\x22\x04\x12\x04\x23\x04\x19\x04\x22\x04\x15");
//"Hello" in Russian in unicode for checking results
_LIT16(KRussianHelloU, "\x0417\x0414\x0420\x0410\x0412\x0421\x0422\x0412\
\x0423\x0419\x0422\x0415");

//"Hello" in Chinese, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
_LIT8(KChineseHello, "\x80\x4F\x60\x59\x7D");
//"Hello" in Chinese in unicode for checking results
_LIT16(KChineseHelloU, "\x4F60\x597D");

//Characters in Katakana, first byte "\x80" is 0x80 UCS2 coding type for Tlv data
_LIT8(KKatakanaChars, "\x80\x30\xEB");
//Characters in Katakana in unicode for checking results
_LIT16(KKatakanaCharsU, "\x30EB");

CTestSuite* CCSatSendReceiveDataFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatSendReceiveDataFU, TestNotifyReceiveDataPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendReceiveDataFU, TestNotifyReceiveDataPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatSendReceiveDataFU, TestNotifyReceiveDataPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendReceiveDataFU, TestNotifyReceiveDataPCmd0004L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendReceiveDataFU, TestNotifySendDataPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendReceiveDataFU, TestNotifySendDataPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatSendReceiveDataFU, TestNotifySendDataPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatSendReceiveDataFU, TestNotifySendDataPCmd0004L);

	END_SUITE;
	}


/**
Fills in channel data descriptor
@param aData TDes8 or TDes16 descriptor data to be prepared
*/
template <class T>
LOCAL_C void PrepareChannelData(T& aData)
	{
	for (TUint8 i = 0; i < KChannelDataLength; i++)
		{
		aData.Append(i);
		}
	}


/**
Resets RSat::SendDataV2 data fields
@param aSendData data to be reset
*/
LOCAL_C void ResetSendData(RSat::TSendDataV2& aData)
	{
	// reset public members
	aData.iDestination = RSat::KDeviceIdNotSet;
	aData.iMode = RSat::ESendDataModeNotSet;
	aData.iAlphaId.iStatus = RSat::EAlphaIdNotSet;
	aData.iAlphaId.iAlphaId.Zero();
	aData.iIconId.iQualifier = RSat::EIconQualifierNotSet;
	aData.iIconId.iIdentifier = 0;
	aData.iChannelData.Zero();

	// reset protected members;
	aData.SetPCmdNumber(0);
	}


/**
Resets RSat::ReceiveDataV2 data fields
@param aReceiveData data to be reset
*/
LOCAL_C void ResetReceiveData(RSat::TReceiveDataV2& aData)
	{
	// reset public members
	aData.iDestination = RSat::KDeviceIdNotSet;
	aData.iAlphaId.iStatus = RSat::EAlphaIdNotSet;
	aData.iAlphaId.iAlphaId.Zero();
	aData.iIconId.iQualifier = RSat::EIconQualifierNotSet;
	aData.iIconId.iIdentifier = 0;
	aData.iChannelDataLength = 0;

	// reset protected members;
	aData.SetPCmdNumber(0);
	}


/**
Prepares SendData proactive command complete data for MockLtsy
@param aChannelData Channel data
@param aDelay Delay value for MockLtsy completion
*/
void CCSatSendReceiveDataFU::PrepareSendDataCompleteDataL(
		const TDesC8* aChannelData,
		TInt aDelay)
	{
	PrepareSendDataCompleteDataL(
			aChannelData, KPCmdNumberOne, KBipSendDataImmediately, RSat::KChannel1,
			NULL, RSat::EIconQualifierNotSet, KErrNone, aDelay);
	}

/**
Prepares SendData proactive command complete data for MockLtsy
@param aChannelData Channel data
@param aPCmdNumber Proactive command number.
@param aCmdQual Command qualifier
@param aDestDeviceId Command destination device Id
@param aAlphaId Alpha identifier
@param aIconQual Type of icon
@param aResult Proactive command notify result
@param aDelay Delay value for MockLtsy completion
*/
void CCSatSendReceiveDataFU::PrepareSendDataCompleteDataL(
		const TDesC8* aChannelData,
		TUint8 aPCmdNumber,
		TUint8 aCmdQual,
		TUint8 aDestDeviceId,
		const TDesC8* aAlphaId,
		RSat::TIconQualifier aIconQual,
		TInt aResult,
		TInt aDelay)
	{
	TTlv tlv;
	ProactiveCmdTlvBegin(tlv, aPCmdNumber, KSendData, aCmdQual, aDestDeviceId);
	
	if ( aChannelData )
		{
		tlv.AddTag(KTlvChannelDataTag);
		tlv.AddData(*aChannelData);//ETLV_ChannelDataString
		}
	
	if ( aAlphaId )
		{			
		tlv.AddTag(KTlvAlphaIdentifierTag);
		tlv.AddData(*aAlphaId);//ETLV_AlphaIdentifier
		}

	if ( ( RSat::ESelfExplanatory == aIconQual ) ||
			( RSat::ENotSelfExplanatory == aIconQual ) )
		{
		tlv.AddTag(KTlvIconIdentifierTag);
		tlv.AddByte((RSat::ESelfExplanatory == aIconQual)?0:1);//ETLV_IconQualifier
		tlv.AddByte(KIconOne);//ETLV_IconIdentifier
		}

	PrepareMockWithCompleteDataForPCmdNotifyL(tlv.End(), aResult, aDelay);
	}


/**
Tests for successful completion of RSat::NotifySendDataPCmd request
@param aChannelData Channel data
@param aCmdQual Command qualifier
*/
void CCSatSendReceiveDataFU::TestNotifySendDataPCmdL(
		const TDesC8* aChannelData,
		TUint8 aCmdQual)
	{
	TestNotifySendDataPCmdL(
			aChannelData, KPCmdNumberOne, aCmdQual, RSat::KChannel1,
			NULL, RSat::EIconQualifierNotSet, KErrNone, NULL);
	}


/**
Tests for completion of RSat::NotifySendDataPCmd request
@param aChannelData Channel data
@param aError Proactive command notify error code for mock data
*/
void CCSatSendReceiveDataFU::TestNotifySendDataPCmdErrorL(
		const TDesC8* aChannelData,
		TInt aError)
	{
	TestNotifySendDataPCmdL(
			aChannelData, KPCmdNumberOne, KBipSendDataImmediately, RSat::KChannel1,
			NULL, RSat::EIconQualifierNotSet, aError, NULL);
	}


/**
Tests for successful completion of RSat::NotifySendDataPCmd request
@param aChannelData Channel data
@param aDestDeviceId Command destination device Id
*/
void CCSatSendReceiveDataFU::TestNotifySendDataPCmdL(
		const TDesC8* aChannelData,
		RSat::TDeviceId aDestDeviceId)
	{
	TestNotifySendDataPCmdL(
			aChannelData, KPCmdNumberOne, KBipSendDataImmediately, aDestDeviceId,
			NULL, RSat::EIconQualifierNotSet, KErrNone, NULL);
	}


/**
Tests for successful completion of RSat::NotifySendDataPCmd request
@param aChannelData Channel data
@param aAlphaId Alpha identifier
@param aAlphaIdCheck Alpha identifier value to ckeck with returned value
@param aIconQual Type of icon
*/
void CCSatSendReceiveDataFU::TestNotifySendDataPCmdL(
		const TDesC8* aChannelData,
		const TDesC8* aAlphaId,
		const TDesC16* aAlphaIdCheck,
		RSat::TIconQualifier aIconQual)
	{
	TestNotifySendDataPCmdL(
			aChannelData, KPCmdNumberOne, KBipSendDataImmediately, RSat::KChannel1,
			aAlphaId, aIconQual, KErrNone, aAlphaIdCheck);
	}


/**
Tests for completion of RSat::NotifySendDataPCmd request
@param aChannelData Channel data
@param aPCmdNumber Proactive command number
@param aCmdQual Command qualifier
@param aDestDeviceId Command destination device Id
@param aAlphaId Alpha identifier
@param aIconQual Type of icon
@param aResult Proactive command notify result
@param aAlphaIdCheck Alpha identifier value to ckeck with returned value
*/
void CCSatSendReceiveDataFU::TestNotifySendDataPCmdL(
		const TDesC8* aChannelData,
		TUint8 aPCmdNumber,
		TUint8 aCmdQual,
		RSat::TDeviceId aDestDeviceId,
		const TDesC8* aAlphaId,
		RSat::TIconQualifier aIconQual,
		TInt aResult,
		const TDesC16* aAlphaIdCheck)
	{
	// write expected notify request in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KSendData);
	
	// write completion data in mockLtsy to complete notify request
	TUint8 destDeviceId = ( RSat::KDeviceIdNotSet==aDestDeviceId )?KDeviceIdNotSet:aDestDeviceId;
	PrepareSendDataCompleteDataL(
			aChannelData, aPCmdNumber, aCmdQual, destDeviceId,
			aAlphaId, aIconQual, aResult);
	
	// send notify request to CSAT
	RSat::TSendDataV2 sendData;
	RSat::TSendDataV2Pckg sendDataPckg(sendData);
	TRequestStatus requestStatus;
	iSat.NotifySendDataPCmd(requestStatus, sendDataPckg);
	
	// wait for the request completion
	User::WaitForRequest(requestStatus);
	
	// check expected results with recieved data
	ASSERT_EQUALS(aResult, requestStatus.Int());

	if ( KErrNone == aResult )
		{
		// proactive command number
		ASSERT_EQUALS(aPCmdNumber, sendData.PCmdNumber());
		
		ASSERT_EQUALS(aDestDeviceId, sendData.iDestination);
		
		if ( KBipSendDataImmediately & aCmdQual )
			{
			ASSERT_EQUALS(RSat::ESendDataImmediately, sendData.iMode);
			}
		else
			{
			ASSERT_EQUALS(RSat::EStoreDataInTxBuffer, sendData.iMode);
			}
		
		if ( aChannelData )
			{
			ASSERT_EQUALS( 0, aChannelData->Compare(sendData.iChannelData) );
			}
		else
			{
			ASSERT_EQUALS( 0, KNullDesC8().Compare(sendData.iChannelData) );
			}

		if ( aAlphaId && ( 0 < aAlphaId->Length() ) )
			{
			ASSERT_EQUALS(RSat::EAlphaIdProvided, sendData.iAlphaId.iStatus);
			if ( aAlphaIdCheck )
				{
				ASSERT_EQUALS( 0, aAlphaIdCheck->Compare(
						sendData.iAlphaId.iAlphaId));
				}
			else
				{
				ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(
												*aAlphaId,
												sendData.iAlphaId.iAlphaId));
				}
			}
		else if ( aAlphaId && ( 0 == aAlphaId->Length() ) )
			{
			ASSERT_EQUALS( RSat::EAlphaIdNull, sendData.iAlphaId.iStatus );
			}
		else
			{
			ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendData.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendData.iAlphaId.iStatus) );
			}

		if (RSat::EIconQualifierNotSet != aIconQual)
			{
			ASSERT_EQUALS(aIconQual, sendData.iIconId.iQualifier);
			ASSERT_EQUALS(KIconOne, sendData.iIconId.iIdentifier);
			}
		else
			{
			ASSERT_EQUALS(RSat::ENoIconId, sendData.iIconId.iQualifier);
			}
		}

	AssertMockLtsyStatusL();
	}


/**
Handles sending a terminal response
@param aResp terminal response data
*/
void CCSatSendReceiveDataFU::GenerateSendDataTerminalResponseL(
		const RSat::TSendDataRspV2& aResp)
	{
	GenerateSendDataTerminalResponseL(aResp, KBipSendDataImmediately);
	}


/**
Handles sending a terminal response
@param aResp terminal response data
@param aCmdQual Command qualifier
@param aExpResult Expected result of TerminalRsp() request
*/
void CCSatSendReceiveDataFU::GenerateSendDataTerminalResponseL(
		const RSat::TSendDataRspV2& aResp,
		TUint8 aCmdQual,
		TInt aExpResult)
	{
	TTlv additionalInfoTlv;
	TPtrC8 additionalInfoTlvData(KNullDesC8);

	TPtrC genResultAddInfo(KNullDesC);
	if ( RSat::KMeProblem == aResp.iInfoType )
		{
		// prepare additional info for general result for expected data in mockLtsy
		genResultAddInfo.Set( aResp.iAdditionalInfo );
		}
	else if ( CommandPerformedSuccessfully(aResp.iGeneralResult) )
		{
		// prepare additional info Tlv data for expected data in mockLtsy
		additionalInfoTlv.AddTag(KTlvChannelDataLengthTag);
		additionalInfoTlv.AddByte(aResp.iChannelDataLength);//ETLV_ChannelDataLength

		additionalInfoTlvData.Set( additionalInfoTlv.GetDataWithoutTopLevelTag() );
		}

	RSat::RSat::TSendDataRspV2Pckg respPckg(aResp);
	GenerateTerminalResponseL(
			aResp.PCmdNumber(),
			KSendData,
			aCmdQual,
			RSat::ESendData,
			respPckg,
			genResultAddInfo,
			aResp.iGeneralResult,
			additionalInfoTlvData,
			aExpResult);

	AssertMockLtsyStatusL();
	}


/**
Prepares ReceiveData proactive command complete data for MockLtsy
@param aChannelDataLength Channel data length
@param aDelay Delay value for MockLtsy completion
*/
void CCSatSendReceiveDataFU::PrepareReceiveDataCompleteDataL(
		const TUint8* aChannelDataLength,
		TInt aDelay)
	{
	PrepareReceiveDataCompleteDataL(
			KPCmdNumberOne, aChannelDataLength, NULL, RSat::EIconQualifierNotSet,
			KErrNone, aDelay, RSat::KChannel1);
	}


/**
Prepares ReceiveData proactive command complete data for MockLtsy
@param aPCmdNumber Proactive command number.
@param aChannelDataLength Channel data length
@param aAlphaId Alpha identifier
@param aIconQual Type of icon
@param aResult Proactive command notify result
@param aDelay Delay value for MockLtsy completion
@param aDestDeviceId Command destination device Id
*/
void CCSatSendReceiveDataFU::PrepareReceiveDataCompleteDataL(
		TUint8 aPCmdNumber,
		const TUint8* aChannelDataLength,
		const TDesC8* aAlphaId,
		RSat::TIconQualifier aIconQual,
		TInt aResult,
		TInt aDelay,
		TUint8 aDestDeviceId)
	{
	TTlv tlv;
	ProactiveCmdTlvBegin(tlv, aPCmdNumber, KReceiveData, KReceiveDataQualifier, aDestDeviceId);

	if ( aChannelDataLength )
		{			
		tlv.AddTag(KTlvChannelDataLengthTag);
		tlv.AddByte(*aChannelDataLength);//ETLV_ChannelDataLength
		}

	if ( aAlphaId )
		{			
		tlv.AddTag(KTlvAlphaIdentifierTag);
		tlv.AddData(*aAlphaId);//ETLV_AlphaIdentifier
		}

	if ( ( RSat::ESelfExplanatory == aIconQual ) ||
			( RSat::ENotSelfExplanatory == aIconQual ) )
		{
		tlv.AddTag(KTlvIconIdentifierTag);
		tlv.AddByte((RSat::ESelfExplanatory == aIconQual)?0:1);//ETLV_IconQualifier
		tlv.AddByte(KIconOne);//ETLV_IconIdentifier
		}

	PrepareMockWithCompleteDataForPCmdNotifyL(tlv.End(), aResult, aDelay);
	}


/**
Tests for completion of RSat::NotifyReceiveDataPCmd request with specified
 channel data length and RSat::KChannel1 as the destination device to process
 the command
@param aChannelDataLength Channel data length
@param aResult Proactive command notify result
*/
void CCSatSendReceiveDataFU::TestNotifyReceiveDataPCmdL(
		const TUint8* aChannelDataLength,
		TInt aResult)
	{
	TestNotifyReceiveDataPCmdL(
			KPCmdNumberOne, aChannelDataLength, NULL, RSat::EIconQualifierNotSet,
			aResult, NULL, RSat::KChannel1);
	}


/**
Tests for successful completion of RSat::NotifyReceiveDataPCmd request with
 specified channel data length and destination device to process the command
@param aChannelDataLength Channel data length
@param aDestDeviceId Command destination device Id
*/
void CCSatSendReceiveDataFU::TestNotifyReceiveDataPCmdL(
		const TUint8* aChannelDataLength,
		RSat::TDeviceId aDestDeviceId)
	{
	TestNotifyReceiveDataPCmdL(
			KPCmdNumberOne, aChannelDataLength, NULL, RSat::EIconQualifierNotSet,
			KErrNone, NULL, aDestDeviceId);
	}


/**
Tests for successful completion of RSat::NotifyReceiveDataPCmd request
@param aChannelDataLength Channel data length
@param aAlphaId Alpha identifier
@param aAlphaIdCheck Alpha identifier value to ckeck with returned value
@param aIconQual Type of icon
*/
void CCSatSendReceiveDataFU::TestNotifyReceiveDataPCmdL(
		const TUint8* aChannelDataLength,
		const TDesC8* aAlphaId,
		const TDesC16* aAlphaIdCheck,
		RSat::TIconQualifier aIconQual)
	{
	TestNotifyReceiveDataPCmdL(
			KPCmdNumberOne, aChannelDataLength, aAlphaId, aIconQual,
			KErrNone, aAlphaIdCheck, RSat::KChannel1);
	}


/**
Tests for completion of RSat::NotifyReceiveDataPCmd request
@param aPCmdNumber Proactive command number
@param aChannelDataLength Channel data length
@param aAlphaId Alpha identifier
@param aIconQual Type of icon
@param aResult Proactive command notify result
@param aAlphaIdCheck Alpha identifier value to ckeck with returned value
@param aDestDeviceId Command destination device Id
*/
void CCSatSendReceiveDataFU::TestNotifyReceiveDataPCmdL(
		TUint8 aPCmdNumber,
		const TUint8* aChannelDataLength,
		const TDesC8* aAlphaId,
		RSat::TIconQualifier aIconQual,
		TInt aResult,
		const TDesC16* aAlphaIdCheck,
		RSat::TDeviceId aDestDeviceId)
	{
	// write expected request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KReceiveData);
	
	// write completion data in mockLtsy to complete notify request
	TUint8 destDeviceId = ( RSat::KDeviceIdNotSet==aDestDeviceId )?KDeviceIdNotSet:aDestDeviceId;
	PrepareReceiveDataCompleteDataL(
			aPCmdNumber, aChannelDataLength, aAlphaId,
			aIconQual, aResult, KDefaultNoDelay, destDeviceId);
	
	// send notify request to CSAT
	RSat::TReceiveDataV2 receiveData;
	RSat::TReceiveDataV2Pckg receiveDataPckg(receiveData);
	TRequestStatus requestStatus;
	iSat.NotifyReceiveDataPCmd(requestStatus, receiveDataPckg);
	
	// wait for notify request completion
	User::WaitForRequest(requestStatus);
	
	// check expected results with recieved data
	ASSERT_EQUALS(aResult, requestStatus.Int());

	if ( KErrNone == aResult )
		{
		ASSERT_EQUALS(aPCmdNumber, receiveData.PCmdNumber());

		ASSERT_EQUALS(aDestDeviceId, receiveData.iDestination);
		
		if ( aChannelDataLength )
			{
			ASSERT_EQUALS(*aChannelDataLength, receiveData.iChannelDataLength);
			}
		else
			{
			ASSERT_EQUALS(static_cast<TUint8>(0), receiveData.iChannelDataLength);
			}
		
		if ( aAlphaId && ( 0 < aAlphaId->Length() ) )
			{
			ASSERT_EQUALS(RSat::EAlphaIdProvided, receiveData.iAlphaId.iStatus);
			if ( aAlphaIdCheck )
				{
				ASSERT_EQUALS( 0, aAlphaIdCheck->Compare(
						receiveData.iAlphaId.iAlphaId));
				}
			else
				{
				ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(
												*aAlphaId,
												receiveData.iAlphaId.iAlphaId));
				}
			}
		else if ( aAlphaId && ( 0 == aAlphaId->Length() ) )
			{
			ASSERT_EQUALS( RSat::EAlphaIdNull, receiveData.iAlphaId.iStatus );
			}
		else
			{
			ASSERT_TRUE( (RSat::EAlphaIdNotPresent == receiveData.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == receiveData.iAlphaId.iStatus) );
			}

		if (RSat::EIconQualifierNotSet != aIconQual)
			{
			ASSERT_EQUALS(aIconQual, receiveData.iIconId.iQualifier);
			ASSERT_EQUALS(KIconOne, receiveData.iIconId.iIdentifier);
			}
		else
			{
			ASSERT_EQUALS(RSat::ENoIconId, receiveData.iIconId.iQualifier);
			}
		}

	AssertMockLtsyStatusL();
	}


/**
Handles sending a terminal response
@param aResp terminal response data
@param aExpResult Expected result of TerminalRsp() request
*/
void CCSatSendReceiveDataFU::GenerateReceiveDataTerminalResponseL(
		const RSat::TReceiveDataRspV2& aResp,
		TInt aExpResult)
	{
	TTlv additionalInfoTlv;
	TPtrC8 additionalInfoTlvData(KNullDesC8);

	TPtrC genResultAddInfo(KNullDesC);
	if ( RSat::KMeProblem == aResp.iInfoType )
		{
		// prepare additional info for general result for expected data in mockLtsy
		genResultAddInfo.Set( aResp.iAdditionalInfo );
		}
	else if ( CommandPerformedSuccessfully(aResp.iGeneralResult) )
		{
		// prepare additional info Tlv data for expected data in mockLtsy
		additionalInfoTlv.AddTag(KTlvChannelDataTag);
		TBuf8<RSat::KAdditionalInfoMaxSize> channelData;
		channelData.Copy(aResp.iAdditionalInfo);
		additionalInfoTlv.AddData(channelData);//ETLV_ChannelDataString
		
		additionalInfoTlv.AddTag(KTlvChannelDataLengthTag);
		additionalInfoTlv.AddByte(aResp.iChannelDataLength);//ETLV_ChannelDataLength

		additionalInfoTlvData.Set( additionalInfoTlv.GetDataWithoutTopLevelTag() );
		}

	RSat::RSat::TReceiveDataRspV2Pckg respPckg(aResp);
	GenerateTerminalResponseL(
			aResp.PCmdNumber(),
			KReceiveData,
			KReceiveDataQualifier,
			RSat::EReceiveData,
			respPckg,
			genResultAddInfo,
			aResp.iGeneralResult,
			additionalInfoTlvData,
			aExpResult);

	AssertMockLtsyStatusL();
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-SRD-SRDPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyReceiveDataPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyReceiveDataPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendReceiveDataFU::TestNotifyReceiveDataPCmd0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifyReceiveDataPCmd 
 	//-------------------------------------------------------------------------

	// test for successful completion of RSat::NotifyReceiveDataPCmd request
	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// generate terminal response to RECEIVE DATA proactive command
	RSat::TReceiveDataRspV2 resp;
	resp.SetPCmdNumber(KPCmdNumberOne);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KChannelData;
	// fills in recieved data
	PrepareChannelData(resp.iAdditionalInfo);
	// fills in the size of available data to recieve
	resp.iChannelDataLength = 0xFF; // more than 255 bytes still available
	
	GenerateReceiveDataTerminalResponseL( resp );

	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifyReceiveDataPCmd
 	//-------------------------------------------------------------------------
	// containing text to display to the user
 	//-------------------------------------------------------------------------

	// test for successful completion of RSat::NotifyReceiveDataPCmd request
	_LIT8(KAlphaId, "Receive Data");
	TestNotifyReceiveDataPCmdL(&KChannelDataLength, &KAlphaId);
	
	// generate terminal response to RECEIVE DATA proactive command
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// containing text with null data object
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength, &KNullDesC8);
	
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// containing a self explanatory icon to display to the user, successful
 	//-------------------------------------------------------------------------

	RSat::TIconQualifier iconQual = RSat::ESelfExplanatory;
	TestNotifyReceiveDataPCmdL(
			&KChannelDataLength, &KBasicIconAlpha, NULL, iconQual);
	
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// containing text & not self-explanatory icon, successful
 	//-------------------------------------------------------------------------

	iconQual = RSat::ENotSelfExplanatory;
	TestNotifyReceiveDataPCmdL(
			&KChannelDataLength, &KBasicIconAlpha, NULL, iconQual);
	
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// successful, UCS2 text in Cyrillic
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(
			&KChannelDataLength, &KRussianHello, &KRussianHelloU);
	
	GenerateReceiveDataTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// successful, UCS2 text in Chinese
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(
			&KChannelDataLength, &KChineseHello, &KChineseHelloU);
	
	GenerateReceiveDataTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// successful, UCS2 text in Katakana
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(
			&KChannelDataLength, &KKatakanaChars, &KKatakanaCharsU);
	
	GenerateReceiveDataTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifyReceiveDataPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------

	// write completion data in mockLtsy to complete notify
	// receive data proactive command request
	PrepareReceiveDataCompleteDataL(&KChannelDataLength);
	
	// write expected request data in mockLtsy
	// for terminal response sent by CSAT
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(
			KPCmdNumberOne, KReceiveData, KReceiveDataQualifier,
			addInfo, RSat::KMeUnableToProcessCmd);

	// wait for completion of requests processing by mockLtsy
	WaitForMockLTSYTerminated();
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	// test for unsuccessful completion of notify
	// receive data proactive command request
	TestNotifyReceiveDataPCmdL(&KChannelDataLength, KErrUnknown);

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SRD-SRDPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyReceiveDataPCmd when problems in processing command
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyReceiveDataPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendReceiveDataFU::TestNotifyReceiveDataPCmd0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifyReceiveDataPCmd
 	//-------------------------------------------------------------------------
	// containing a self explanatory icon to display to the user, but
	// requested icon could not be displayed
 	//-------------------------------------------------------------------------

	RSat::TIconQualifier iconQual = RSat::ESelfExplanatory;
	TestNotifyReceiveDataPCmdL(
			&KChannelDataLength, &KBasicIconAlpha, NULL, iconQual);
	
	// generate terminal response to RECEIVE DATA proactive command
	RSat::TReceiveDataRspV2 resp;
	resp.SetPCmdNumber(KPCmdNumberOne);
	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;
	resp.iInfoType = RSat::KChannelData;
	// fills in recieved data
	PrepareChannelData(resp.iAdditionalInfo);
	// fills in the size of available data to recieve
	resp.iChannelDataLength = 0xFF; // more than 255 bytes still available

	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// with text & not self-explanatory icon, 
	// requested icon could not be displayed
 	//-------------------------------------------------------------------------

	iconQual = RSat::ENotSelfExplanatory;
	TestNotifyReceiveDataPCmdL(
			&KChannelDataLength, &KBasicIconAlpha, NULL, iconQual);
	
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// missing channel data length
 	//-------------------------------------------------------------------------

	// write expected request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KReceiveData);

	// write completion data in mockLtsy to complete notify request
	// with data missing channel data length
	PrepareReceiveDataCompleteDataL(NULL);
	
	// write expected request data in mockLtsy
	// for terminal response sent by CSAT
	PrepareTerminalResponseMockDataL(
			KPCmdNumberOne, KReceiveData, KReceiveDataQualifier,
			KNullDesC, RSat::KErrorRequiredValuesMissing);

	// send notify recieve data proactive command to CSAT
	RSat::TReceiveDataV2 receiveData;
	RSat::TReceiveDataV2Pckg receiveDataPckg(receiveData);

	TRequestStatus requestStatus;
	iSat.NotifyReceiveDataPCmd(requestStatus, receiveDataPckg);
	
	// wait for unsuccessful request completion
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// network unable to process command
 	//-------------------------------------------------------------------------

	// test for successful notify request completion
	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// unsupported error due to procesing the command
	resp.iGeneralResult = RSat::KNetworkUnableToProcessCmd; // invalid for this command
	resp.iInfoType = RSat::KSatNetworkErrorInfo;
	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(KRequestedFacilityNotImplemented);
	resp.iChannelDataLength = 0; 
	GenerateReceiveDataTerminalResponseL( resp, KErrCorrupt );

 	//-------------------------------------------------------------------------
	// Me unable to process command
	//-------------------------------------------------------------------------

	// test for successful notify request completion
	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);
	GenerateReceiveDataTerminalResponseL( resp );

	//-------------------------------------------------------------------------
	// command type not understood
 	//-------------------------------------------------------------------------

	// test for successful notify request completion
	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KCmdTypeNotUnderstood;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// command data not understood
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KCmdDataNotUnderstood;
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// bearer indep protocol error
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KBearerIndepProtocolError;
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Append(RSat::KChannelIdNotValid);
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// command number not known
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KCmdNumberNotKnown;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// command beyond me capabilities
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KCmdBeyondMeCapabilities;
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// proactive session terminated by user
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KPSessionTerminatedByUser;
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// missing device identities
 	//-------------------------------------------------------------------------
	
	// write expected notify request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KReceiveData);

	// write completion data in mockLtsy to complete notify request
	PrepareReceiveDataCompleteDataL(
			KPCmdNumberOne, &KChannelDataLength, NULL, RSat::EIconQualifierNotSet,
			KErrNone, KDefaultNoDelay, KDeviceIdNotSet);
	
	// write expected request data in mockLtsy
	// for unsuccessful terminal response sent by CSAT
	PrepareTerminalResponseMockDataL(
			KPCmdNumberOne, KReceiveData, KReceiveDataQualifier,
			KNullDesC, RSat::KErrorRequiredValuesMissing);

	// send notify request to CSAT
	iSat.NotifyReceiveDataPCmd(requestStatus, receiveDataPckg);
	
	// wait for notify request completion
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// missing information, successful
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KMissingInformation;
	resp.iInfoType = RSat::KChannelData;
	// fills in recieved data
	PrepareChannelData(resp.iAdditionalInfo);
	// fills in the size of available data to recieve
	resp.iChannelDataLength = 0xFF; // more than 255 bytes still available
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// partial comprehension, successful
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	resp.iGeneralResult = RSat::KPartialComprehension;
	GenerateReceiveDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// interaction with CC permanent error
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// unsupported error due to procesing the command
	resp.iGeneralResult = RSat::KInteractionWithCCPermanentError; // invalid for this command
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(RSat::KNoSpecificInteractionError);
	resp.iChannelDataLength = 0;
	GenerateReceiveDataTerminalResponseL( resp, KErrCorrupt );

 	//-------------------------------------------------------------------------
	// Me unable to process command, empty additional info
 	//-------------------------------------------------------------------------

	TestNotifyReceiveDataPCmdL(&KChannelDataLength);
	
	// sent terminal response on the command about
	// ME problem due to procesing the command
	// without addition info on the problem
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iAdditionalInfo.Zero(); // invalid for this command
	GenerateReceiveDataTerminalResponseL( resp, KErrCorrupt );

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SRD-SRDPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifyReceiveDataPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyReceiveDataPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendReceiveDataFU::TestNotifyReceiveDataPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifyReceiveDataPCmd
 	//-------------------------------------------------------------------------

	// write expected notify request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KReceiveData);
	
	// write completion data in mockLtsy to complete notify request

	// delay for completion is used for make cancel before
	// the completion arrives to CSATTSY
	PrepareReceiveDataCompleteDataL(&KChannelDataLength, KDefaultDelay);
	
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	
	// write expected request data in mockLtsy
	// for unsuccessful terminal response sent by CSAT
	// on unsolicited notify request completion
	PrepareTerminalResponseMockDataL(
			KPCmdNumberOne, KReceiveData, KReceiveDataQualifier,
			addInfo, RSat::KMeUnableToProcessCmd);

	// send notify request to CSAT
	RSat::TReceiveDataV2 receiveData;
	RSat::TReceiveDataV2Pckg receiveDataPckg(receiveData);
	TRequestStatus requestStatus;
	iSat.NotifyReceiveDataPCmd(requestStatus, receiveDataPckg);
	
	// cancel notify request
	iSat.CancelAsyncRequest(ESatNotifyReceiveDataPCmd);
	
	// wait for completion of cancelling
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	// Wait for completion of request processing by mockLtsy
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SRD-SRDPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifyReceiveDataPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyReceiveDataPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendReceiveDataFU::TestNotifyReceiveDataPCmd0004L()
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
	// Test A: Test multiple clients requesting RSat::NotifyReceiveDataPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	// write expected notify request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KReceiveData);
	
	// write completion data in mockLtsy to complete notify request

	// delay for completion is used for make the 2nd request before
	// the completion arrives to CSATTSY
	PrepareReceiveDataCompleteDataL(&KChannelDataLength, KDefaultDelay);
	
	// send notify requests to CSAT
	RSat::TReceiveDataV2 receiveData;
	RSat::TReceiveDataV2 receiveData2;
	RSat::TReceiveDataV2Pckg receiveDataPckg(receiveData);
	RSat::TReceiveDataV2Pckg receiveData2Pckg(receiveData2);
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	iSat.NotifyReceiveDataPCmd(requestStatus, receiveDataPckg);
	sat2.NotifyReceiveDataPCmd(requestStatus2, receiveData2Pckg);
	
	// wait for completion of notify requests
	User::WaitForRequest(requestStatus);
	User::WaitForRequest(requestStatus2);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KPCmdNumberOne, receiveData.PCmdNumber());
	ASSERT_EQUALS(RSat::KChannel1, receiveData.iDestination);
	ASSERT_EQUALS(KChannelDataLength, receiveData.iChannelDataLength);
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == receiveData.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == receiveData.iAlphaId.iStatus) );
	ASSERT_EQUALS(RSat::ENoIconId, receiveData.iIconId.iQualifier);

	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(KPCmdNumberOne, receiveData2.PCmdNumber());
	ASSERT_EQUALS(RSat::KChannel1, receiveData2.iDestination);
	ASSERT_EQUALS(KChannelDataLength, receiveData2.iChannelDataLength);
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == receiveData2.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == receiveData2.iAlphaId.iStatus) );
	ASSERT_EQUALS(RSat::ENoIconId, receiveData2.iIconId.iQualifier);

	AssertMockLtsyStatusL();

	// send terminal response on the proactive command
	RSat::TReceiveDataRspV2 resp;
	resp.SetPCmdNumber(KPCmdNumberOne);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KChannelData;
	// fills in recieved data
	PrepareChannelData(resp.iAdditionalInfo);
	// fills in the size of available data to recieve
	resp.iChannelDataLength = 0xFF; // more than 255 bytes are available
	
	// both clients send the terminal response
	GenerateReceiveDataTerminalResponseL( resp );
	GenerateReceiveDataTerminalResponseL( resp );

	//-------------------------------------------------------------------------
	// Test B: Test multiple clients requesting RSat::NotifyReceiveDataPCmd
	// when they both pass different T-class versions
 	//-------------------------------------------------------------------------	

	// When multiple clients requesting RSat::NotifyReceiveDataPCmd and they both pass
	// different packaged parameter versions only the last request completes, because
	// etel reposts request with new parameter size and looses previous requests.
	// May be it would be worth to make ESatNotifyReceiveDataPCmd ipc requests flow controlled
	// by CSATTSY instead of multiple completed by ETEL. Similar defects are 51, 58, 71 and 78
	ERR_PRINTF2(_L("<font color=Orange>$CSATKnownFailure: defect id = %d</font>"), 86);
	ASSERT_TRUE( EFalse );

	// write expected notify request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KReceiveData);
	
	// write completion data in mockLtsy to complete notify request

	// delay for completion is used for make the 2nd request before
	// the completion arrives to CSATTSY
	PrepareReceiveDataCompleteDataL(&KChannelDataLength, KDefaultDelay);
	
	// send notify requests to CSAT
	RSat::TReceiveDataV5 receiveDataV5;
	RSat::TReceiveDataV5Pckg receiveDataV5Pckg(receiveDataV5);
	ResetReceiveData(receiveData);
	iSat.NotifyReceiveDataPCmd(requestStatus, receiveDataPckg);
	sat2.NotifyReceiveDataPCmd(requestStatus2, receiveDataV5Pckg);
	
	// wait for completion of notify requests
	User::WaitForRequest(requestStatus2);
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KPCmdNumberOne, receiveData.PCmdNumber());
	ASSERT_EQUALS(RSat::KChannel1, receiveData.iDestination);
	ASSERT_EQUALS(KChannelDataLength, receiveData.iChannelDataLength);
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == receiveData.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == receiveData.iAlphaId.iStatus) );
	ASSERT_EQUALS(RSat::ENoIconId, receiveData.iIconId.iQualifier);

	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(KPCmdNumberOne, receiveDataV5.PCmdNumber());
	ASSERT_EQUALS(RSat::KChannel1, receiveDataV5.iDestination);
	ASSERT_EQUALS(KChannelDataLength, receiveDataV5.iChannelDataLength);
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == receiveDataV5.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == receiveDataV5.iAlphaId.iStatus) );
	ASSERT_EQUALS(RSat::ENoIconId, receiveDataV5.iIconId.iQualifier);
	ASSERT_TRUE( RSat::ETextAttributeProvided !=
										receiveDataV5.iTextAttribute.iStatus );

	AssertMockLtsyStatusL();

	// send terminal response on the proactive command
	// both clients send the terminal response
	GenerateReceiveDataTerminalResponseL( resp );
	GenerateReceiveDataTerminalResponseL( resp );

	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CSAT-SRD-SSDPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySendDataPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendDataPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendReceiveDataFU::TestNotifySendDataPCmd0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifySendDataPCmd 
 	//-------------------------------------------------------------------------
	// SEND DATA, immediate mode
 	//-------------------------------------------------------------------------

	// test for successful completion of RSat::NotifySendDataPCmd request
	TestNotifySendDataPCmdL( &KChannelData );
	
	// generate terminal response to SEND DATA proactive command
	RSat::TSendDataRspV2 resp;
	resp.SetPCmdNumber(KPCmdNumberOne);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	// fills in available space in the Tx buffer
	resp.iChannelDataLength = 0xFF; // more than 255 bytes
 	GenerateSendDataTerminalResponseL( resp );

	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifySendDataPCmd
 	//-------------------------------------------------------------------------
	// SEND DATA, Store mode
 	//-------------------------------------------------------------------------

	// test for successful completion of RSat::NotifySendDataPCmd request
	TBuf8<KChannelDataLength> channelData;
	PrepareChannelData(channelData);
	TestNotifySendDataPCmdL( &channelData, KSendDataStoreMode );
	
	// generate terminal response to SEND DATA proactive command
	GenerateSendDataTerminalResponseL( resp, KSendDataStoreMode );

 	//-------------------------------------------------------------------------
	// containing text to display to user
 	//-------------------------------------------------------------------------

	_LIT8(KAlphaId, "Send Data");
	TestNotifySendDataPCmdL(&KChannelData, &KAlphaId);
	
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// containing text of zero length
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL(&KChannelData, &KNullDesC8);
	
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// containing self explanatory icon to display to user, successful
 	//-------------------------------------------------------------------------

	RSat::TIconQualifier iconQual = RSat::ESelfExplanatory;
	TestNotifySendDataPCmdL(&KChannelData, &KBasicIconAlpha, NULL, iconQual);
	
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// containing text & not self-explanatory icon, successful
 	//-------------------------------------------------------------------------

	iconQual = RSat::ENotSelfExplanatory;
	TestNotifySendDataPCmdL(&KChannelData, &KBasicIconAlpha, NULL, iconQual);
	
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// successful, UCS2 text in Cyrillic
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL(&KChannelData, &KRussianHello, &KRussianHelloU);
	
	GenerateSendDataTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// successful, UCS2 text in Chinese
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL(&KChannelData, &KChineseHello, &KChineseHelloU);
	
	GenerateSendDataTerminalResponseL( resp );
	
 	//-------------------------------------------------------------------------
	// successful, UCS2 text in Katakana
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL(&KChannelData, &KKatakanaChars, &KKatakanaCharsU);
	
	GenerateSendDataTerminalResponseL( resp );
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifySendDataPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------

	// write completion data in mockLtsy to complete notify
	// send data proactive command request
	PrepareSendDataCompleteDataL( &KChannelData );
	
	// write expected request data in mockLtsy
	// for terminal response sent by CSAT
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(
			KPCmdNumberOne, KSendData, KBipSendDataImmediately,
			addInfo, RSat::KMeUnableToProcessCmd);

	// wait for completion of requests processing by mockLtsy
	WaitForMockLTSYTerminated();
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	// test for unsuccessful completion of notify
	// send data proactive command request
	TestNotifySendDataPCmdErrorL(&KChannelData, KErrUnknown);

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SRD-SSDPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifySendDataPCmd when problems in processing command
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifySendDataPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendReceiveDataFU::TestNotifySendDataPCmd0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifySendDataPCmd
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.5(ETSI TS 131 124), SEND DATA,
	// immediate mode with a bad channel identifier
 	//-------------------------------------------------------------------------

	// test for successful completion of notify request
	TestNotifySendDataPCmdL(&KChannelData, RSat::KChannel2);
	
	// send unsuccessful terminal response on the proactive command
	RSat::TSendDataRspV2 resp;
	resp.SetPCmdNumber(KPCmdNumberOne);
	resp.iGeneralResult = RSat::KBearerIndepProtocolError;
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Append(RSat::KChannelIdNotValid);
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// self explanatory icon, requested icon could not be displayed, successful
 	//-------------------------------------------------------------------------

	// test for successful completion of notify request
	RSat::TIconQualifier iconQual = RSat::ESelfExplanatory;
	TestNotifySendDataPCmdL(&KChannelData, &KBasicIconAlpha, NULL, iconQual);
	
	// send terminal response
	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	// fills in available space in the Tx buffer
	resp.iChannelDataLength = 0xFF; // more than 255 bytes 
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// Alpha identifier & BASIC-ICON, not self-explanatory, 
	// requested icon could not be displayed
 	//-------------------------------------------------------------------------

	iconQual = RSat::ENotSelfExplanatory;
	TestNotifySendDataPCmdL(&KChannelData, &KBasicIconAlpha, NULL, iconQual);
	
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// missing channel data
 	//-------------------------------------------------------------------------

	// write expected notify request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KSendData);

	// write completion data in mockLtsy to complete notify
	// send data proactive command request
	// with missing channel data
	PrepareSendDataCompleteDataL( NULL );
	
	// write expected request data in mockLtsy
	// for terminal response sent by CSAT
	PrepareTerminalResponseMockDataL(
			KPCmdNumberOne, KSendData, KBipSendDataImmediately,
			KNullDesC, RSat::KErrorRequiredValuesMissing);

	// send notify request
	RSat::TSendDataV2 sendData;
	RSat::TSendDataV2Pckg sendDataPckg(sendData);
	TRequestStatus requestStatus;
	iSat.NotifySendDataPCmd(requestStatus, sendDataPckg);
	
	// wait for notify request completion
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// network unable to process cmd
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KNetworkUnableToProcessCmd;
	resp.iInfoType = RSat::KSatNetworkErrorInfo;
	resp.iAdditionalInfo.Append(KRequestedFacilityNotImplemented);
	resp.iChannelDataLength = 0; 
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// command type not understood
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KCmdTypeNotUnderstood;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// command data not understood
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KCmdDataNotUnderstood;
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// bearer indep protocol error 
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KBearerIndepProtocolError;
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Append(RSat::KChannelIdNotValid);
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// command number not known
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KCmdNumberNotKnown;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// command beyond me capabilities
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KCmdBeyondMeCapabilities;
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// proactive session terminated by user
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KPSessionTerminatedByUser;
	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// missing device identities
 	//-------------------------------------------------------------------------

	// write expected notify request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KSendData);

	// write completion data in mockLtsy to complete notify request
	PrepareSendDataCompleteDataL(
			&KChannelData, KPCmdNumberOne, KBipSendDataImmediately, KDeviceIdNotSet,
			NULL, RSat::EIconQualifierNotSet, KErrNone, KDefaultNoDelay);

	// write expected request data in mockLtsy
	// for unsuccessful terminal response sent by CSAT
	PrepareTerminalResponseMockDataL(
			KPCmdNumberOne, KSendData, KBipSendDataImmediately,
			KNullDesC, RSat::KErrorRequiredValuesMissing);

	// send notify request to CSAT
	iSat.NotifySendDataPCmd(requestStatus, sendDataPckg);
	
	// wait for notify request completion
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Me unable to process command, empty additional info
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// ME problem due to procesing the command
	// without addition info on the problem
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iInfoType = RSat::KMeProblem;
	GenerateSendDataTerminalResponseL( resp, KBipSendDataImmediately, KErrCorrupt );

 	//-------------------------------------------------------------------------
	// interaction with CC permanent error
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// unsupported error due to procesing the command
	resp.iGeneralResult = RSat::KInteractionWithCCPermanentError; // invalid for this command
	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(RSat::KNoSpecificInteractionError);
	GenerateSendDataTerminalResponseL( resp, KBipSendDataImmediately, KErrCorrupt );

 	//-------------------------------------------------------------------------
	// missing information ( successful )
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KMissingInformation;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.iChannelDataLength = 0xFF; 

	GenerateSendDataTerminalResponseL( resp );

 	//-------------------------------------------------------------------------
	// partial comprehension ( successful )
 	//-------------------------------------------------------------------------

	TestNotifySendDataPCmdL( &KChannelData );
	
	// sent terminal response on the command about
	// an error due to procesing the command
	resp.iGeneralResult = RSat::KPartialComprehension;
	GenerateSendDataTerminalResponseL( resp );

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SRD-SSDPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifySendDataPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifySendDataPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendReceiveDataFU::TestNotifySendDataPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifySendDataPCmd
 	//-------------------------------------------------------------------------

	// write expected notify request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KSendData);
	
	// write completion data in mockLtsy to complete notify request

	// delay for completion is used for make cancel before
	// the completion arrives to CSATTSY
	PrepareSendDataCompleteDataL(&KChannelData, KDefaultDelay);
	
	// write expected request data in mockLtsy
	// for unsuccessful terminal response sent by CSAT
	// on unsolicited notify request completion
	RSat::TAdditionalInfo addInfo;
	addInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(
			KPCmdNumberOne, KSendData, KBipSendDataImmediately,
			addInfo, RSat::KMeUnableToProcessCmd);

	// send notify request to CSAT
	RSat::TSendDataV2 sendData;
	RSat::TSendDataV2Pckg sendDataPckg(sendData);
	TRequestStatus requestStatus;
	iSat.NotifySendDataPCmd(requestStatus, sendDataPckg);
	
	// cancel notify request
	iSat.CancelAsyncRequest(ESatNotifySendDataPCmd);
	
	// wait for completion of cancelling
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	// Wait for completion of request processing by mockLtsy
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CSAT-SRD-SSDPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifySendDataPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifySendDataPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatSendReceiveDataFU::TestNotifySendDataPCmd0004L()
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
	// Test A: Test multiple clients requesting RSat::NotifySendDataPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	// write expected notify request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KSendData);

	// write completion data in mockLtsy to complete notify request

	// delay for completion is used for make the 2nd request before
	// the completion arrives to CSATTSY
	PrepareSendDataCompleteDataL(&KChannelData, KDefaultDelay);
	
	// send notify requests to CSAT
	RSat::TSendDataV2 sendData;
	RSat::TSendDataV2 sendData2;
	RSat::TSendDataV2Pckg sendDataPckg(sendData);
	RSat::TSendDataV2Pckg sendDataPckg2(sendData2);
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	iSat.NotifySendDataPCmd(requestStatus, sendDataPckg);
	sat2.NotifySendDataPCmd(requestStatus2, sendDataPckg2);
	
	// wait for completion of notify requests
	User::WaitForRequest(requestStatus);
	User::WaitForRequest(requestStatus2);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KPCmdNumberOne, sendData.PCmdNumber());
	ASSERT_EQUALS(RSat::KChannel1, sendData.iDestination);
	ASSERT_EQUALS(RSat::ESendDataImmediately, sendData.iMode);
	ASSERT_EQUALS( 0, KChannelData().Compare(sendData.iChannelData) );
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendData.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendData.iAlphaId.iStatus) );
	ASSERT_EQUALS(RSat::ENoIconId, sendData.iIconId.iQualifier);

	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(KPCmdNumberOne, sendData2.PCmdNumber());
	ASSERT_EQUALS(RSat::KChannel1, sendData2.iDestination);
	ASSERT_EQUALS(RSat::ESendDataImmediately, sendData2.iMode);
	ASSERT_EQUALS( 0, KChannelData().Compare(sendData2.iChannelData) );
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendData2.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendData2.iAlphaId.iStatus) );
	ASSERT_EQUALS(RSat::ENoIconId, sendData2.iIconId.iQualifier);

	AssertMockLtsyStatusL();
	
	// send terminal response on the proactive command
	RSat::TSendDataRspV2 resp;
	resp.SetPCmdNumber(KPCmdNumberOne);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	// more than 255 bytes of space available in the Tx buffer
	resp.iChannelDataLength = 0xFF; 
	
	// both clients send the terminal response
	GenerateSendDataTerminalResponseL( resp );
	GenerateSendDataTerminalResponseL( resp );

	//-------------------------------------------------------------------------
	// Test B: Test multiple clients requesting RSat::NotifySendDataPCmd
	// when they both pass different T-class versions
 	//-------------------------------------------------------------------------	

	// When multiple clients requesting RSat::NotifySendDataPCmd and they both
	// pass different packaged parameter versions only the last request completes,
	// because etel reposts request with new parameter size and looses previous
	// requests. May be it would be worth to make ESatNotifySendDataPCmd ipc
	// requests flow controlled by CSATTSY instead of multiple completed by ETEL.
	// Similar defects are 51, 58, 71, 78 and 86
	ERR_PRINTF2(_L("<font color=Orange>$CSATKnownFailure: defect id = %d</font>"), 90);
	ASSERT_TRUE( EFalse );

	// write expected notify request data in mockLtsy
	PrepareMockWithExpDataForPCmdNotifyL(KSendData);

	// write completion data in mockLtsy to complete notify request

	// delay for completion is used for make the 2nd request before
	// the completion arrives to CSATTSY
	PrepareSendDataCompleteDataL(&KChannelData, KDefaultDelay);
	
	// send notify requests to CSAT
	RSat::TSendDataV5 sendDataV5;
	RSat::TSendDataV5Pckg sendDataV5Pckg(sendDataV5);
	ResetSendData(sendData);
	iSat.NotifySendDataPCmd(requestStatus, sendDataPckg);
	sat2.NotifySendDataPCmd(requestStatus2, sendDataV5Pckg);
	
	// wait for completion of notify requests
	User::WaitForRequest(requestStatus2);
	User::WaitForRequest(requestStatus);
	
	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KPCmdNumberOne, sendData.PCmdNumber());
	ASSERT_EQUALS(RSat::KChannel1, sendData.iDestination);
	ASSERT_EQUALS(RSat::ESendDataImmediately, sendData.iMode);
	ASSERT_EQUALS( 0, KChannelData().Compare(sendData.iChannelData) );
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendData.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendData.iAlphaId.iStatus) );
	ASSERT_EQUALS(RSat::ENoIconId, sendData.iIconId.iQualifier);

	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(KPCmdNumberOne, sendDataV5.PCmdNumber());
	ASSERT_EQUALS(RSat::KChannel1, sendDataV5.iDestination);
	ASSERT_EQUALS(RSat::ESendDataImmediately, sendDataV5.iMode);
	ASSERT_EQUALS( 0, KChannelData().Compare(sendDataV5.iChannelData) );
	ASSERT_TRUE( (RSat::EAlphaIdNotPresent == sendDataV5.iAlphaId.iStatus) ||
					(RSat::EAlphaIdNotSet == sendDataV5.iAlphaId.iStatus) );
	ASSERT_EQUALS(RSat::ENoIconId, sendDataV5.iIconId.iQualifier);
	ASSERT_TRUE( RSat::ETextAttributeProvided !=
											sendDataV5.iTextAttribute.iStatus );

	AssertMockLtsyStatusL();
	
	// send terminal response on the proactive command
	// both clients send the terminal response
	GenerateSendDataTerminalResponseL( resp );
	GenerateSendDataTerminalResponseL( resp );

	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}


