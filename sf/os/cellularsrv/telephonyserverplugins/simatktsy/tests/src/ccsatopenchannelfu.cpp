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
 @file The TEFUnit test suite for OpenChannel in the SAT.
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

#include "ccsatopenchannelfu.h"

_LIT8(KUser8, "UserLog");
_LIT16(KUser16, "UserLog");

_LIT8(KPwd8, "UserPwd");
_LIT16(KPwd16, "UserPwd");

//Precedence Class: 02
//Delay Class: 04
//Reliability Class: 05
//Peak throughput class: 05
//Mean throughput class: 16
//Packet data protocol: 02 (IP)
_LIT8(KBearerParams, "\x02\x04\x05\x05\x10\x02");

//01.01.01.01
_LIT8(KAddress, "\x01\x01\x01\x01");

// 1 bit always on, ton - international number, npi - isdn
static const TUint8 KInternationalNumIsdn = (0x80 | 0x10 | 0x01);

//immediate link establishment
static const TUint8 KCmdQualifier = 0x01;
static const TUint16 KBufferSize = 1400;

static const TUint8 KCmdId = 0x01;

//channel 1, Link established or Packet data service activated	
static const TUint8 KChannelStatusByte1 = 0x81;
// no more info
static const TUint8 KChannelStatusByte2 = 0x00;

// 9600
static const TUint8 KBearerByte1 = 0x07; 
// async UDI
static const TUint8 KBearerByte2 = 0x00; 
// non-transparent
static const TUint8 KBearerByte3 = 0x01; 

static const TUint8 KUseTCP = 0x02;
static const TUint8 KUseUDP = 0x01;

static const TUint16 KTcpPort = 44444;


CTestSuite* CCSatOpenChannelFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatOpenChannelFU, TestNotifyOpenChannelPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatOpenChannelFU, TestNotifyOpenChannelPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatOpenChannelFU, TestNotifyOpenChannelPCmd0004L);
	ADD_TEST_STEP_ISO_CPP(CCSatOpenChannelFU, TestNotifyOpenChannelPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatOpenChannelFU, TestNotifyOpenChannelPCmd0001cL);
	ADD_TEST_STEP_ISO_CPP(CCSatOpenChannelFU, TestNotifyOpenChannelPCmd0001dL);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-CHAN-SCHANPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RSat::NotifyOpenChannelPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyOpenChannelPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatOpenChannelFU::TestNotifyOpenChannelPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	RBuf8 data;
	CleanupClosePushL(data);

	_LIT8(KNum, "112233445566778");	
	TTlv tlv;	
	
	PrepareTlv( tlv, KCmdQualifier, KInternationalNumIsdn, KNum, KBipDefaultBearer);
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	TRequestStatus notifyStatus;
	ReinitChannel(iOpenCsChannel);
	RSat::TOpenCsChannelV2Pckg csChanPck(iOpenCsChannel);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of RSat::NotifyOpenChannelPCmd 
 	//-------------------------------------------------------------------------

	PrepareAndCompleteDispatchL( *tlvDscPtr );

	iSat.NotifyOpenChannelPCmd(notifyStatus, csChanPck);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());
	
	ASSERT_EQUALS( 0, iOpenCsChannel.iUserPassword.Compare(KNullDesC16));
	ASSERT_EQUALS( 0, iOpenCsChannel.iUserLogin.Compare(KNullDesC16));
	ASSERT_EQUALS( 0, iOpenCsChannel.iSubAddress.Compare(KNullDesC16));
	ASSERT_EQUALS( RSat::EProtocolNotPresent, iOpenCsChannel.iSimMeInterface.iTransportProto);
	ASSERT_EQUALS( RSat::ENoAutomaticReconnection, iOpenCsChannel.iReconnectionMode);
	ASSERT_EQUALS( RSat::EAnyBearer, iOpenCsChannel.iPCmdType);
	ASSERT_EQUALS( RSat::EAddressNotSet, iOpenCsChannel.iLocalAddress.iType);
	ASSERT_EQUALS( RSat::EImmediate, iOpenCsChannel.iLinkEst);
	ASSERT_EQUALS( RSat::ENoIconId, iOpenCsChannel.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::ETimeUnitNotSet, iOpenCsChannel.iDuration1.iTimeUnit);
	ASSERT_EQUALS( RSat::ETimeUnitNotSet, iOpenCsChannel.iDuration2.iTimeUnit);
	ASSERT_EQUALS( RSat::EAddressNotPresent, iOpenCsChannel.iDestinationAddress.iType);
	ASSERT_EQUALS( KBufferSize, iOpenCsChannel.iBufferSize);
	ASSERT_EQUALS( RSat::EAlphaIdNotPresent, iOpenCsChannel.iAlphaId.iStatus);
	ASSERT_EQUALS( RSat::ETypeOfNumberNotSet, iOpenCsChannel.iAddress.iTypeOfNumber);
	ASSERT_EQUALS( RSat::ENumberingPlanNotSet, iOpenCsChannel.iAddress.iNumberPlan);
	ASSERT_EQUALS( RSat::EDefaultBearer, iOpenCsChannel.iBearer.iType);
	ASSERT_EQUALS( 0, iOpenCsChannel.iBearer.iParams.Compare(KNullDesC8));

	//terminal response	
	RSat::TOpenChannelRspV2 terminalRsp;
	RSat::TOpenChannelRspV2Pckg terminalRspPckg(terminalRsp);
	terminalRsp.SetPCmdNumber(KCmdId);
	terminalRsp.iGeneralResult = RSat::KSuccess;
	terminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	terminalRsp.iAdditionalInfo = KNullDesC;
	terminalRsp.iBufferSize = KBufferSize;

	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvBufferSizeTag );
	termRespAddInfo.AddByte( TUint8( ( KBufferSize  & 0xFF00 ) >> 8 ) );
	termRespAddInfo.AddByte( TUint8( KBufferSize  & 0x00FF ) );
	
	GenerateTerminalResponseL(KCmdId,	KOpenChannel,	KCmdQualifier,
			RSat::EOpenChannelCs, terminalRspPckg,	terminalRsp.iAdditionalInfo, 
			terminalRsp.iGeneralResult, termRespAddInfo.GetDataWithoutTopLevelTag());
	
	AssertMockLtsyStatusL();

	// passing unsupported bearer type - KBipLocalLinkBearer	
	PrepareTlv( tlv, KCmdQualifier, KInternationalNumIsdn, KNum, KBipLocalLinkBearer);
	tlv.End();

	PrepareAndCompleteDispatchL( *tlvDscPtr );

	iSat.NotifyOpenChannelPCmd(notifyStatus, csChanPck);

	TTlv termRespAddInfo2;
    termRespAddInfo2.AddTag( KTlvBearerDescriptionTag );
    termRespAddInfo2.AddByte( (TUint8)RSat::EBearerTypeNotSet );
	termRespAddInfo2.AddByte( 0x00 );
	termRespAddInfo2.AddTag( KTlvBufferSizeTag );
	termRespAddInfo2.AddByte( 0x00 );
	termRespAddInfo2.AddByte( 0x00 );
	
	PrepareTerminalResponseMockDataL( KCmdId, KOpenChannel, KCmdQualifier,
			KNullDesC, RSat::KCmdDataNotUnderstood, termRespAddInfo2.GetDataWithoutTopLevelTag());
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCorrupt, notifyStatus.Int());

	AssertMockLtsyStatusL();

	// passing tlv buffer without buffer size and bearer description
	
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KCmdId);//ETLV_CommandNumber
	tlv.AddByte(KOpenChannel); //ETLV_TypeOfCommand
	tlv.AddByte(KCmdQualifier); //ETLV_CommandQualifier
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KMe); //ETLV_DestinationDeviceIdentity
	tlv.End();
	
	PrepareAndCompleteDispatchL( *tlvDscPtr );

	iSat.NotifyOpenChannelPCmd(notifyStatus, csChanPck);
	
	PrepareTerminalResponseMockDataL( KCmdId, KOpenChannel, KCmdQualifier,
			KNullDesC, RSat::KErrorRequiredValuesMissing, termRespAddInfo2.GetDataWithoutTopLevelTag());
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCorrupt, notifyStatus.Int());

	AssertMockLtsyStatusL();	
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifyOpenChannelPCmd from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);	    
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrNone, data, 10);
	
	TBuf<1> additionalInfo(0);
	additionalInfo.Append( RSat::KNoSpecificMeProblem );
	PrepareTerminalResponseMockDataL( KCmdId, KOpenChannel, KCmdQualifier,
			additionalInfo, RSat::KMeUnableToProcessCmd, termRespAddInfo2.GetDataWithoutTopLevelTag());
 		
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
		
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	PrepareAndCompleteDispatchL( *tlvDscPtr, KErrUnknown );

	iSat.NotifyOpenChannelPCmd(notifyStatus, csChanPck);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrUnknown, notifyStatus.Int());

	AssertMockLtsyStatusL();	
	
	// testing all TerminalResponse variants 
	DoTestTerminalResponseL();
		
	CleanupStack::PopAndDestroy(2, this); 
	}

void CCSatOpenChannelFU::ReinitChannel(RSat::TOpenCsChannelV2 &aChannel)
	{
	aChannel.iAddress.iTypeOfNumber = RSat::ETypeOfNumberNotSet;
	aChannel.iAddress.iNumberPlan = RSat::ENumberingPlanNotSet;
	aChannel.iAddress.iTelNumber.Zero();
	aChannel.iSubAddress.Zero();
	aChannel.iDuration1.iTimeUnit = RSat::ETimeUnitNotSet;
	aChannel.iDuration1.iNumOfUnits = 0;
	aChannel.iDuration2.iTimeUnit = RSat::ETimeUnitNotSet;
	aChannel.iDuration2.iNumOfUnits = 0;
	aChannel.iLocalAddress.iType = RSat::EAddressNotSet;
	aChannel.iLocalAddress.iAddress.Zero();
	aChannel.iUserLogin.Zero(); 
	aChannel.iUserPassword.Zero();
	aChannel.iPCmdType = RSat::EPCmdTypeNotSet;
	aChannel.iLinkEst = RSat::ELinkEstablishmentNotSet;
	aChannel.iReconnectionMode = RSat::EReconnectionModeNotSet; 
	aChannel.iAlphaId.iStatus = RSat::EAlphaIdNotSet;
	aChannel.iAlphaId.iAlphaId.Zero();
	aChannel.iIconId.iQualifier = RSat::EIconQualifierNotSet; 
	aChannel.iIconId.iIdentifier = 0; 
	aChannel.iBearer.iType = RSat::EBearerTypeNotSet; 
	aChannel.iBearer.iParams.Zero();
	aChannel.iBufferSize = 0;
	aChannel.iSimMeInterface.iTransportProto = RSat::EProtocolNotSet; 
	aChannel.iSimMeInterface.iPrtNumber = 0; 
	aChannel.iDestinationAddress.iType = RSat::EAddressNotSet; 
	aChannel.iDestinationAddress.iAddress.Zero(); 
	//aChannel.iPCmdNumber = 0; 
	//aChannel.iExtensionId = 0; 
	}
/**
@SYMTestCaseID BA-CSAT-CHAN-SCHANPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RSat::NotifyOpenChannelPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyOpenChannelPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatOpenChannelFU::TestNotifyOpenChannelPCmd0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	TRequestStatus notifyStatus;
	RSat::TOpenCsChannelV2 csChan;
	RSat::TOpenCsChannelV2Pckg csChanPck(csChan);

	PrepareDispatchL( );

	iSat.NotifyOpenChannelPCmd(notifyStatus, csChanPck);
	
	iSat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrCancel, notifyStatus.Int());

	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(1, this); 
	}



/**
@SYMTestCaseID BA-CSAT-CHAN-SCHANPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RSat::NotifyOpenChannelPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyOpenChannelPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatOpenChannelFU::TestNotifyOpenChannelPCmd0004L()
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
	
	_LIT8(KNum, "112233445566778");	
	TUint8 bearerType(KBipDefaultBearer);
	TTlv tlv;	
	
	PrepareTlv( tlv, KCmdQualifier, KInternationalNumIsdn, KNum, bearerType);

	TRequestStatus notifyStatus;
	RSat::TOpenCsChannelV2 csChan;
	RSat::TOpenCsChannelV2Pckg csChanPck(csChan);
	
	PrepareAndCompleteDispatchL( tlv.End() );

	iSat.NotifyOpenChannelPCmd(notifyStatus, csChanPck);

	TRequestStatus notifyStatus2;
	RSat::TOpenCsChannelV2 csChan2;
	RSat::TOpenCsChannelV2Pckg csChanPck2(csChan2);

	sat2.NotifyOpenChannelPCmd(notifyStatus2, csChanPck2);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());

	ASSERT_EQUALS( 0, csChan.iUserPassword.Compare(KNullDesC16));
	ASSERT_EQUALS( 0, csChan.iUserLogin.Compare(KNullDesC16));
	ASSERT_EQUALS( 0, csChan.iSubAddress.Compare(KNullDesC16));
	ASSERT_EQUALS( RSat::EProtocolNotPresent, csChan.iSimMeInterface.iTransportProto);
	ASSERT_EQUALS( RSat::ENoAutomaticReconnection, csChan.iReconnectionMode);
	ASSERT_EQUALS( RSat::EAnyBearer, csChan.iPCmdType);
	ASSERT_EQUALS( RSat::EAddressNotSet, csChan.iLocalAddress.iType);
	ASSERT_EQUALS( RSat::EImmediate, csChan.iLinkEst);
	ASSERT_EQUALS( RSat::ENoIconId, csChan.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::ETimeUnitNotSet, csChan.iDuration1.iTimeUnit);
	ASSERT_EQUALS( RSat::ETimeUnitNotSet, csChan.iDuration2.iTimeUnit);
	ASSERT_EQUALS( RSat::EAddressNotPresent, csChan.iDestinationAddress.iType);
	ASSERT_EQUALS( KBufferSize, csChan.iBufferSize);
	ASSERT_EQUALS( RSat::EAlphaIdNotPresent, csChan.iAlphaId.iStatus);
	ASSERT_EQUALS( RSat::ETypeOfNumberNotSet, csChan.iAddress.iTypeOfNumber);
	ASSERT_EQUALS( RSat::ENumberingPlanNotSet, csChan.iAddress.iNumberPlan);
	ASSERT_EQUALS( RSat::EDefaultBearer, csChan.iBearer.iType);
	ASSERT_EQUALS( 0, csChan.iBearer.iParams.Compare(KNullDesC8));
	
	User::WaitForRequest(notifyStatus2);
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());

	ASSERT_EQUALS( 0, csChan2.iUserPassword.Compare(KNullDesC16));
	ASSERT_EQUALS( 0, csChan2.iUserLogin.Compare(KNullDesC16));
	ASSERT_EQUALS( 0, csChan2.iSubAddress.Compare(KNullDesC16));
	ASSERT_EQUALS( RSat::EProtocolNotPresent, csChan2.iSimMeInterface.iTransportProto);
	ASSERT_EQUALS( RSat::ENoAutomaticReconnection, csChan2.iReconnectionMode);
	ASSERT_EQUALS( RSat::EAnyBearer, csChan2.iPCmdType);
	ASSERT_EQUALS( RSat::EAddressNotSet, csChan2.iLocalAddress.iType);
	ASSERT_EQUALS( RSat::EImmediate, csChan2.iLinkEst);
	ASSERT_EQUALS( RSat::ENoIconId, csChan2.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::ETimeUnitNotSet, csChan2.iDuration1.iTimeUnit);
	ASSERT_EQUALS( RSat::ETimeUnitNotSet, csChan2.iDuration2.iTimeUnit);
	ASSERT_EQUALS( RSat::EAddressNotPresent, csChan2.iDestinationAddress.iType);
	ASSERT_EQUALS( KBufferSize, csChan2.iBufferSize);
	ASSERT_EQUALS( RSat::EAlphaIdNotPresent, csChan2.iAlphaId.iStatus);
	ASSERT_EQUALS( RSat::ETypeOfNumberNotSet, csChan2.iAddress.iTypeOfNumber);
	ASSERT_EQUALS( RSat::ENumberingPlanNotSet, csChan2.iAddress.iNumberPlan);
	ASSERT_EQUALS( RSat::EDefaultBearer, csChan2.iBearer.iType);
	ASSERT_EQUALS( 0, csChan2.iBearer.iParams.Compare(KNullDesC8));
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); 	
	}



/**
@SYMTestCaseID BA-CSAT-CHAN-SCHANPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RSat::NotifyOpenChannelPCmd, GPRS, no alpha, no network access name
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyOpenChannelPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatOpenChannelFU::TestNotifyOpenChannelPCmd0001bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	_LIT8(KNum, "112233445566778");	
	TUint8 bearerType(KBipGprsBearer);
	TTlv tlv;	
	
	PrepareTlv( tlv, KCmdQualifier, KInternationalNumIsdn, KNum, bearerType);

	TRequestStatus notifyStatus;
	RSat::TOpenGprsChannelV4 gprsChan;
	RSat::TOpenGprsChannelV4Pckg gprsChanPck(gprsChan);

	//see ETSI TS 100 607 Expected Sequence 2.1 
	//(OPEN CHANNEL, immediate link establishment, GPRS, no local address, no alpha identifier, no network access name)

	PrepareAndCompleteDispatchL( tlv.End() );

	iSat.NotifyOpenChannelPCmd(notifyStatus, gprsChanPck);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());

	ASSERT_EQUALS( 0, gprsChan.iUserPassword.Compare(KPwd16));
	ASSERT_EQUALS( 0, gprsChan.iUserLogin.Compare(KUser16));
	ASSERT_EQUALS( RSat::EUdp, gprsChan.iSimMeInterface.iTransportProto);
	ASSERT_EQUALS( RSat::ENoAutomaticReconnection, gprsChan.iReconnectionMode);
	ASSERT_EQUALS( RSat::EGprsBearer, gprsChan.iPCmdType);
	ASSERT_EQUALS( RSat::EAddressNotSet, gprsChan.iLocalAddress.iType);
	ASSERT_EQUALS( RSat::EImmediate, gprsChan.iLinkEst);
	ASSERT_EQUALS( RSat::ENoIconId, gprsChan.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::EIPv4Address, gprsChan.iDestinationAddress.iType);
	ASSERT_EQUALS( 0, gprsChan.iDestinationAddress.iAddress.Compare(KAddress));
	ASSERT_EQUALS( KBufferSize, gprsChan.iBufferSize);
	ASSERT_EQUALS( RSat::EAlphaIdNotPresent, gprsChan.iAlphaId.iStatus);
	ASSERT_EQUALS( 0, gprsChan.iAccessName.Compare(KNullDesC8));
	ASSERT_EQUALS( RSat::EGPRSBearer, gprsChan.iBearer.iType);
	ASSERT_EQUALS( 0, gprsChan.iBearer.iParams.Compare(KBearerParams));
		
	//terminal response	
	RSat::TOpenChannelRspV2 terminalRsp;
	RSat::TOpenChannelRspV2Pckg terminalRspPckg(terminalRsp);
	terminalRsp.SetPCmdNumber(KCmdId);
	terminalRsp.iGeneralResult = RSat::KSuccess;
	terminalRsp.iInfoType = RSat::KChannelStatusInfo;
	terminalRsp.iAdditionalInfo.Append(KChannelStatusByte1);;
	terminalRsp.iAdditionalInfo.Append(KChannelStatusByte2);;
	terminalRsp.iBufferSize = KBufferSize;
	terminalRsp.iBearer.iType = RSat::EGPRSBearer;
	terminalRsp.iBearer.iParams.Append(KBearerParams);

	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvChannelStatusTag );
    termRespAddInfo.AddByte(KChannelStatusByte1);	
    termRespAddInfo.AddByte(KChannelStatusByte2);	
	termRespAddInfo.AddTag( KTlvBearerDescriptionTag );
    termRespAddInfo.AddByte( bearerType );
    termRespAddInfo.AddData(KBearerParams);
	termRespAddInfo.AddTag( KTlvBufferSizeTag );
	termRespAddInfo.AddByte( TUint8( ( KBufferSize & 0xFF00 ) >> 8 ) );
	termRespAddInfo.AddByte( TUint8( KBufferSize & 0x00FF ) );
    
	GenerateTerminalResponseL(KCmdId,	KOpenChannel,	KCmdQualifier,
			RSat::EOpenChannelGprs, terminalRspPckg,	KNullDesC, 
			terminalRsp.iGeneralResult, termRespAddInfo.GetDataWithoutTopLevelTag());
	
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(1, this); 	
	}


/**
@SYMTestCaseID BA-CSAT-CHAN-SCHANPC-0001c
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RSat::NotifyOpenChannelPCmd, GPRS, alpha, network access name 
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyOpenChannelPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatOpenChannelFU::TestNotifyOpenChannelPCmd0001cL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	TUint8 bearerType(KBipGprsBearer);
	_LIT8(KNetworkAcessName, ".TestGp.rs");
	_LIT8(KAlpha, "Open ID");
	_LIT16(KAlpha16, "Open ID");
	TTlv tlv;	 
	
	PrepareTlv( tlv, KCmdQualifier, 0, KNullDesC8, bearerType, 
			KNetworkAcessName, KAlpha, KUseTCP, EFalse);	// false - IPv6

	TRequestStatus notifyStatus;
	RSat::TOpenGprsChannelV4 gprsChan;
	RSat::TOpenGprsChannelV4Pckg gprsChanPck(gprsChan);

	//(OPEN CHANNEL, immediate link establishment GPRS, alpha identifier, with network access name)

	PrepareAndCompleteDispatchL( tlv.End() );

	iSat.NotifyOpenChannelPCmd(notifyStatus, gprsChanPck);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());

	ASSERT_EQUALS( 0, gprsChan.iUserPassword.Compare(KPwd16));
	ASSERT_EQUALS( 0, gprsChan.iUserLogin.Compare(KUser16));
	ASSERT_EQUALS( RSat::ETcp, gprsChan.iSimMeInterface.iTransportProto);
	ASSERT_EQUALS( RSat::ENoAutomaticReconnection, gprsChan.iReconnectionMode);
	ASSERT_EQUALS( RSat::EGprsBearer, gprsChan.iPCmdType);
	ASSERT_EQUALS( RSat::EAddressNotSet, gprsChan.iLocalAddress.iType);
	ASSERT_EQUALS( RSat::EImmediate, gprsChan.iLinkEst);
	ASSERT_EQUALS( RSat::ENoIconId, gprsChan.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::EIPv6Address, gprsChan.iDestinationAddress.iType);
	ASSERT_EQUALS( 0, gprsChan.iDestinationAddress.iAddress.Compare(KAddress));
	ASSERT_EQUALS( KBufferSize, gprsChan.iBufferSize);
	ASSERT_EQUALS( RSat::EAlphaIdProvided, gprsChan.iAlphaId.iStatus);
	ASSERT_EQUALS( 0, gprsChan.iAlphaId.iAlphaId.Compare(KAlpha16));	
	ASSERT_EQUALS( 0, gprsChan.iAccessName.Compare(KNetworkAcessName));
	ASSERT_EQUALS( RSat::EGPRSBearer, gprsChan.iBearer.iType);
	ASSERT_EQUALS( 0, gprsChan.iBearer.iParams.Compare(KBearerParams));
	
	//terminal response	
	RSat::TOpenChannelRspV2 terminalRsp;
	RSat::TOpenChannelRspV2Pckg terminalRspPckg(terminalRsp);
	terminalRsp.SetPCmdNumber(KCmdId);
	terminalRsp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	terminalRsp.iInfoType = RSat::KMeProblem;
	terminalRsp.iAdditionalInfo.Append(RSat::KMeBusyOnCall);
	terminalRsp.iBufferSize = KBufferSize;
	terminalRsp.iBearer.iType = RSat::EGPRSBearer;
	terminalRsp.iBearer.iParams.Append(KBearerParams);

	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvBearerDescriptionTag );
    termRespAddInfo.AddByte( bearerType );
    termRespAddInfo.AddData(KBearerParams);
	termRespAddInfo.AddTag( KTlvBufferSizeTag );
	termRespAddInfo.AddByte( TUint8( ( KBufferSize  & 0xFF00 ) >> 8 ) );
	termRespAddInfo.AddByte( TUint8( KBufferSize  & 0x00FF ) );
    
	GenerateTerminalResponseL(KCmdId,	KOpenChannel,	KCmdQualifier,
			RSat::EOpenChannelGprs, terminalRspPckg,	terminalRsp.iAdditionalInfo, 
			terminalRsp.iGeneralResult, termRespAddInfo.GetDataWithoutTopLevelTag());
	
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(1, this); 	
	}


/**
@SYMTestCaseID BA-CSAT-CHAN-SCHANPC-0001d
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RSat::NotifyOpenChannelPCmd, NULL alpha identifier
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyOpenChannelPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatOpenChannelFU::TestNotifyOpenChannelPCmd0001dL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	TUint8 cmdQualifier(0x02);	//on demand, automatic reconnection
	TUint8 bearerType(KBipGprsBearer);
	_LIT8(KNetworkAcessName, ".TestGp.rs");
	TTlv tlv;	 
	
	PrepareTlv( tlv, cmdQualifier, 0, KNullDesC8, bearerType, 
			KNetworkAcessName, KNullDesC8, 66);	// no protocol

	TRequestStatus notifyStatus;
	RSat::TOpenGprsChannelV4 gprsChan;
	RSat::TOpenGprsChannelV4Pckg gprsChanPck(gprsChan);

	//(OPEN CHANNEL, immediate link establishment GPRS, NULL alpha identifier, with network access name)

	PrepareAndCompleteDispatchL( tlv.End() );

	iSat.NotifyOpenChannelPCmd(notifyStatus, gprsChanPck);

	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());

	ASSERT_EQUALS( 0, gprsChan.iUserPassword.Compare(KPwd16));
	ASSERT_EQUALS( 0, gprsChan.iUserLogin.Compare(KUser16));
	ASSERT_EQUALS( RSat::EProtocolNotPresent, gprsChan.iSimMeInterface.iTransportProto);
	ASSERT_EQUALS( RSat::EAutomaticReconnection, gprsChan.iReconnectionMode);
	ASSERT_EQUALS( RSat::EGprsBearer, gprsChan.iPCmdType);
	ASSERT_EQUALS( RSat::EIPv4Address, gprsChan.iLocalAddress.iType);
	ASSERT_EQUALS( 0, gprsChan.iLocalAddress.iAddress.Compare(KAddress));
	ASSERT_EQUALS( RSat::EOnDemand, gprsChan.iLinkEst);
	ASSERT_EQUALS( RSat::ENoIconId, gprsChan.iIconId.iQualifier);
	ASSERT_EQUALS( RSat::EAddressNotPresent, gprsChan.iDestinationAddress.iType);
	ASSERT_EQUALS( KBufferSize, gprsChan.iBufferSize);
	ASSERT_EQUALS( RSat::EAlphaIdNull, gprsChan.iAlphaId.iStatus);
	ASSERT_EQUALS( 0, gprsChan.iAccessName.Compare(KNetworkAcessName));
	ASSERT_EQUALS( RSat::EGPRSBearer, gprsChan.iBearer.iType);
	ASSERT_EQUALS( 0, gprsChan.iBearer.iParams.Compare(KBearerParams));
	
	//terminal response	
	RSat::TOpenChannelRspV2 terminalRsp;
	RSat::TOpenChannelRspV2Pckg terminalRspPckg(terminalRsp);
	terminalRsp.SetPCmdNumber(KCmdId);
	terminalRsp.iGeneralResult = RSat::KBearerIndepProtocolError;
	terminalRsp.iInfoType = RSat::KMeProblem;
	terminalRsp.iAdditionalInfo.Append(RSat::KMeBusyOnCall);
	terminalRsp.iBufferSize = KBufferSize;
	terminalRsp.iBearer.iType = RSat::EGPRSBearer;
	terminalRsp.iBearer.iParams.Append(KBearerParams);

	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvBearerDescriptionTag );
    termRespAddInfo.AddByte( bearerType );
    termRespAddInfo.AddData(KBearerParams);
	termRespAddInfo.AddTag( KTlvBufferSizeTag );
	termRespAddInfo.AddByte( TUint8( ( KBufferSize & 0xFF00 ) >> 8 ) );
	termRespAddInfo.AddByte( TUint8( KBufferSize & 0x00FF ) );
    
	GenerateTerminalResponseL(KCmdId,	KOpenChannel,	cmdQualifier,
			RSat::EOpenChannelGprs, terminalRspPckg,	terminalRsp.iAdditionalInfo, 
			terminalRsp.iGeneralResult, termRespAddInfo.GetDataWithoutTopLevelTag());
	
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(1, this); 	
	}




void CCSatOpenChannelFU::PrepareTlv(TTlv& aTlv, TUint8 aCmdQualifier, TUint8 aTon, 
		const TDesC8& aNum, TUint8 aBearer, const TDesC8& aNetworkAccessName, 
		const TDesC8& aAlpha, TUint8 aUdpTcp, TBool aIPv4 ) 
	{
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(KCmdId);//ETLV_CommandNumber
	aTlv.AddByte(KOpenChannel); //ETLV_TypeOfCommand
	aTlv.AddByte(aCmdQualifier); //ETLV_CommandQualifier

	aTlv.AddTag(KTlvDeviceIdentityTag); 
	aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	aTlv.AddByte(KMe); //ETLV_DestinationDeviceIdentity

	if(0 != aAlpha.Compare(KNoAlpha))
		{
		aTlv.AddTag(KTlvAlphaIdentifierTag); 
		aTlv.AddData(aAlpha);
		}
	
	if(0 < aNum.Length())
		{
		aTlv.AddTag(KTlvAddressTag); 
		aTlv.AddByte(aTon); 
		TBuf8<KMaxAddressLength> bcdSetUpCallNo;
		TSatUtility::AsciiToBCD(aNum,bcdSetUpCallNo);
		aTlv.AddData(bcdSetUpCallNo);
		}
	
	aTlv.AddTag(KTlvBearerDescriptionTag);	
	aTlv.AddByte(aBearer);	
	switch(aBearer)
		{
		case KBipCsdBearer:
			aTlv.AddByte(KBearerByte1);
			aTlv.AddByte(KBearerByte2);
			aTlv.AddByte(KBearerByte3);
			break;	
		case KBipGprsBearer:								
			aTlv.AddData(KBearerParams);
			break;
		default:
			break;
		}

	aTlv.AddTag(KTlvBufferSizeTag);	
	aTlv.AddByte( TUint8( ( KBufferSize & 0xFF00 ) >> 8 ) );
	aTlv.AddByte( TUint8( KBufferSize & 0x00FF ) );

	if(0 < aNetworkAccessName.Length())
		{
		aTlv.AddTag(KTlvNetworkAccessNameTag); 
		aTlv.AddData(aNetworkAccessName); 
		}
			
	if(KBipGprsBearer == aBearer)
		{
		aTlv.AddTag(KTlvTextStringTag);		// user login	
		aTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
		aTlv.AddData(KUser8);	
		aTlv.AddTag(KTlvTextStringTag);		// user password
		aTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
		aTlv.AddData(KPwd8);	
		
		if((KUseUDP == aUdpTcp) || (KUseTCP == aUdpTcp))
			{
			aTlv.AddTag(KTlvSimMeTransportLevelTag);	
			aTlv.AddByte(aUdpTcp);		//UDP/TCP
			aTlv.AddByte( TUint8( ( KTcpPort & 0xFF00 ) >> 8 ) );
			aTlv.AddByte( TUint8( KTcpPort & 0x00FF ) );
			}

		aTlv.AddTag(KTlvOtherAddressTag);
		if(aIPv4)
			{
			aTlv.AddByte(KIPV4);
			}
		else
			{
			aTlv.AddByte(KIPV6);
			}
		aTlv.AddData(KAddress);		//	01.01.01.01
		}
	}


void CCSatOpenChannelFU::PrepareAndCompleteDispatchL(TPtrC8 aTlvDsc, TInt aRes)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	PrepareDispatchL();
	
	TDesC8* tlvDscPtr = &aTlvDsc;
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aRes, data, 10);

	CleanupStack::PopAndDestroy(1); 
	}

void CCSatOpenChannelFU::PrepareDispatchL(void)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TUint8 pcmdCode = KOpenChannel;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
 	
	CleanupStack::PopAndDestroy(1); 
	}

void CCSatOpenChannelFU::DoTestTerminalResponseL()
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
			KErrNone
			},
			{
			// 0x08 - REFRESH performed but indicated NAA was not active;
			RSat::KRefreshUSIMNotActive,
			RSat::KNoAdditionalInfo,
			ETrue,
			0,
			KErrNone
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
			KErrNone
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
			KErrNone
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

	TTlv tlv;	
	PrepareTlv( tlv, KCmdQualifier, KInternationalNumIsdn, _L8("112233445566778"), KBipDefaultBearer);
	
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	
	TRequestStatus notifyStatus;
	ReinitChannel(iOpenCsChannel);
	RSat::TOpenCsChannelV2Pckg csChanPck(iOpenCsChannel);	

	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvBufferSizeTag );
	termRespAddInfo.AddByte( TUint8( ( KBufferSize  & 0xFF00 ) >> 8 ) );
	termRespAddInfo.AddByte( TUint8( KBufferSize  & 0x00FF ) );
	const TDesC8* tlvAddPtr = &termRespAddInfo.GetDataWithoutTopLevelTag();

	for(TInt i = 0; i <= KNumberOfResponsesToTest; i++)
		{				
		// notify request
		PrepareAndCompleteDispatchL( *tlvDscPtr );
		iSat.NotifyOpenChannelPCmd(notifyStatus, csChanPck);
		User::WaitForRequest(notifyStatus);
		ASSERT_EQUALS(KErrNone, notifyStatus.Int());

		AssertMockLtsyStatusL();	
		
		// now test a "TERMINAL RESPONSE" specific to this loop 
		RSat::TOpenChannelRspV2 rsp;
		RSat::TOpenChannelRspV2Pckg rspPckg(rsp);
		rsp.SetPCmdNumber(KCmdId);
		rsp.iBufferSize = KBufferSize;

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
			GenerateTerminalResponseL(KCmdId,	KOpenChannel,	KCmdQualifier,
					RSat::EOpenChannelCs, rspPckg,	rsp.iAdditionalInfo, 
					rsp.iGeneralResult, *tlvAddPtr, KResponsesToTest[i].iExpectedResult);
			}
		else
			{
			// now tell Mock to return KErrUnknown when dispatching forthcoming RSat::TerminalRsp()
			rsp.iGeneralResult	= RSat::KSuccess;
			rsp.iInfoType 		= RSat::KNoAdditionalInfo;
			
			PrepareTerminalResponseMockDataL( KCmdId, KOpenChannel, KCmdQualifier,
					rsp.iAdditionalInfo, rsp.iGeneralResult, *tlvAddPtr, KErrUnknown);

			TerminalResponseL( RSat::EOpenChannelCs, rspPckg, KErrUnknown);
			}
		
		AssertMockLtsyStatusL();			
		}
	}

