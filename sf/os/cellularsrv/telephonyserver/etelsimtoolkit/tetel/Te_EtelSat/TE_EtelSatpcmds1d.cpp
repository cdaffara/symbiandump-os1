// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_EtelSatTestStepBase.h"
#include "TE_EtelSatpcmds1d.h"

CTestSatPCmds1d::CTestSatPCmds1d()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSatPCmds1d"));
	}

enum TVerdict CTestSatPCmds1d::doTestStepL()
	{
	INFO_PRINTF1(_L("***********************************************"));
	INFO_PRINTF1(_L("RSat Proactive Commands Functionality suite - 2"));
	INFO_PRINTF1(_L("***********************************************"));

	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	ret=sat.Open(phone);
    TEST(ret==KErrNone);

	// Open Channel Cs with V2
	RSat::TOpenCsChannelV2Pckg openCsChannelPckg(iOpenCsChannel);

	iOpenCsChannel.iPCmdType		= DSATTSY_CS_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus, openCsChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iOpenCsChannel.PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(iOpenCsChannel.iPCmdType==DSATTSY_CS_BEARER);
	TEST(iOpenCsChannel.iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(iOpenCsChannel.iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(iOpenCsChannel.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iOpenCsChannel.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iOpenCsChannel.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iOpenCsChannel.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iOpenCsChannel.iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(iOpenCsChannel.iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(iOpenCsChannel.iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(iOpenCsChannel.iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(iOpenCsChannel.iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(iOpenCsChannel.iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenCsChannel.iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenCsChannel.iAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(iOpenCsChannel.iAddress.iNumberPlan==DSATTSY_NPI);
	TEST(iOpenCsChannel.iAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(iOpenCsChannel.iSubAddress==DSATTSY_SUBADDRESS);
	TEST(iOpenCsChannel.iDuration1.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(iOpenCsChannel.iDuration1.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(iOpenCsChannel.iDuration2.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(iOpenCsChannel.iDuration2.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(iOpenCsChannel.iLocalAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenCsChannel.iLocalAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenCsChannel.iUserLogin==DSATTSY_LOGIN);
	TEST(iOpenCsChannel.iUserPassword==DSATTSY_PASSWORD);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - CS channel (RSat::TOpenCsChannelV2) - passed"),iTestCount++);

	// Open Channel Cancel
	sat.NotifyOpenChannelPCmd(reqStatus,openCsChannelPckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenCsChannelV2) passed"),iTestCount++);

	// Open Channel Gprs with V4
	RSat::TOpenGprsChannelV4Pckg openGprsChannelPckg(iOpenGprsChannel);

	iOpenGprsChannel.iPCmdType=DSATTSY_GPRS_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus,openGprsChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iOpenGprsChannel.PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(iOpenGprsChannel.iPCmdType==DSATTSY_GPRS_BEARER);
	TEST(iOpenGprsChannel.iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(iOpenGprsChannel.iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(iOpenGprsChannel.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iOpenGprsChannel.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iOpenGprsChannel.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iOpenGprsChannel.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iOpenGprsChannel.iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(iOpenGprsChannel.iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(iOpenGprsChannel.iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(iOpenGprsChannel.iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(iOpenGprsChannel.iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(iOpenGprsChannel.iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenGprsChannel.iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenGprsChannel.iAccessName==DSATTSY_ACCESS_NAME);
	TEST(iOpenGprsChannel.iLocalAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenGprsChannel.iLocalAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenGprsChannel.iUserLogin==DSATTSY_LOGIN);
	TEST(iOpenGprsChannel.iUserPassword==DSATTSY_PASSWORD);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - GPRS Channel (RSat::TOpenGprsChannelV4) - passed"),iTestCount++);

	// Open Channel Cancel with V4
	sat.NotifyOpenChannelPCmd(reqStatus,openGprsChannelPckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenGprsChannelV4) passed"),iTestCount++);

	// Further tests, seperated to prevent stack overflow
	ExtendedTest1L(reqStatus, openCsChannelPckg, openGprsChannelPckg);

	sat.Close();
	phone.Close();

	return TestStepResult();
	}

	// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1d::ExtendedTest1L(TRequestStatus& reqStatus,
									RSat::TOpenCsChannelV2Pckg& aOpenCsChannelPckg,
									RSat::TOpenGprsChannelV4Pckg& aOpenGprsChannelPckg)
	{

	// Open Channel Local with version 2

	RSat::TOpenLocalLinksChannelV2* openLocalChannel = new(ELeave) RSat::TOpenLocalLinksChannelV2;
	CleanupStack::PushL(openLocalChannel);

	RSat::TOpenLocalLinksChannelV2Pckg* openLocalChannelPckg = new(ELeave) RSat::TOpenLocalLinksChannelV2Pckg(*openLocalChannel);
	CleanupStack::PushL(openLocalChannelPckg);

	openLocalChannel->iPCmdType=DSATTSY_LOCAL_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus,*openLocalChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(openLocalChannel->PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(openLocalChannel->iPCmdType==DSATTSY_LOCAL_BEARER);
	TEST(openLocalChannel->iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(openLocalChannel->iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(openLocalChannel->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(openLocalChannel->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(openLocalChannel->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(openLocalChannel->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(openLocalChannel->iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(openLocalChannel->iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(openLocalChannel->iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(openLocalChannel->iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(openLocalChannel->iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(openLocalChannel->iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(openLocalChannel->iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openLocalChannel->iDuration1.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openLocalChannel->iDuration1.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(openLocalChannel->iDuration2.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openLocalChannel->iDuration2.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(openLocalChannel->iRemoteAddress.iCoding==DSATTSY_CODING);
	TEST(openLocalChannel->iRemoteAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openLocalChannel->iUserPassword==DSATTSY_PASSWORD);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - Local Channel (RSat::TOpenLocalLinksChannelV2) - passed"),iTestCount++);

	// Open Channel Cancel with version 2
	sat.NotifyOpenChannelPCmd(reqStatus,*openLocalChannelPckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenLocalLinksChannelV2) passed"),iTestCount++);

	// START Open channel "AnyBearer" tests
	//	1. expect CS returned
	//	2. expect GPRS returned
	//  3. expect LocalLink returned
	//  4. expect error

	// [1] Test for CS returned

	iOpenCsChannel.iPCmdType = DSATTSY_ANY_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus, aOpenCsChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iOpenCsChannel.PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(iOpenCsChannel.iPCmdType==DSATTSY_CS_BEARER);

	sat.GetOpenChannelPCmd(reqStatus, aOpenCsChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iOpenCsChannel.iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(iOpenCsChannel.iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(iOpenCsChannel.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iOpenCsChannel.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iOpenCsChannel.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iOpenCsChannel.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iOpenCsChannel.iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(iOpenCsChannel.iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(iOpenCsChannel.iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(iOpenCsChannel.iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(iOpenCsChannel.iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(iOpenCsChannel.iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenCsChannel.iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenCsChannel.iAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(iOpenCsChannel.iAddress.iNumberPlan==DSATTSY_NPI);
	TEST(iOpenCsChannel.iAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(iOpenCsChannel.iSubAddress==DSATTSY_SUBADDRESS);
	TEST(iOpenCsChannel.iDuration1.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(iOpenCsChannel.iDuration1.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(iOpenCsChannel.iDuration2.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(iOpenCsChannel.iDuration2.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(iOpenCsChannel.iLocalAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenCsChannel.iLocalAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenCsChannel.iUserLogin==DSATTSY_LOGIN);
	TEST(iOpenCsChannel.iUserPassword==DSATTSY_PASSWORD);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - AnyBearer, expect CS channel - passed"),iTestCount++);

	// Open Channel Cancel
	sat.NotifyOpenChannelPCmd(reqStatus,aOpenCsChannelPckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel passed"),iTestCount++);

	// [2] Open Channel Gprs

	iOpenGprsChannel.iPCmdType = DSATTSY_ANY_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus,aOpenGprsChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iOpenGprsChannel.PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(iOpenGprsChannel.iPCmdType==DSATTSY_GPRS_BEARER);

	sat.GetOpenChannelPCmd(reqStatus, aOpenGprsChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iOpenGprsChannel.iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(iOpenGprsChannel.iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(iOpenGprsChannel.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iOpenGprsChannel.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iOpenGprsChannel.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iOpenGprsChannel.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iOpenGprsChannel.iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(iOpenGprsChannel.iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(iOpenGprsChannel.iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(iOpenGprsChannel.iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(iOpenGprsChannel.iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(iOpenGprsChannel.iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenGprsChannel.iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenGprsChannel.iAccessName==DSATTSY_ACCESS_NAME);
	TEST(iOpenGprsChannel.iLocalAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenGprsChannel.iLocalAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenGprsChannel.iUserLogin==DSATTSY_LOGIN);
	TEST(iOpenGprsChannel.iUserPassword==DSATTSY_PASSWORD);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - AnyBearer, expect GPRS channel - passed"),iTestCount++);

	// Open Channel Cancel

	sat.NotifyOpenChannelPCmd(reqStatus,aOpenGprsChannelPckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel passed"),iTestCount++);

	// [3] Open Channel Local

	openLocalChannel->iPCmdType = DSATTSY_ANY_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus,*openLocalChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(openLocalChannel->PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(openLocalChannel->iPCmdType==DSATTSY_LOCAL_BEARER);

	sat.GetOpenChannelPCmd(reqStatus, *openLocalChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(openLocalChannel->iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(openLocalChannel->iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(openLocalChannel->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(openLocalChannel->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(openLocalChannel->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(openLocalChannel->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(openLocalChannel->iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(openLocalChannel->iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(openLocalChannel->iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(openLocalChannel->iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(openLocalChannel->iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(openLocalChannel->iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(openLocalChannel->iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openLocalChannel->iDuration1.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openLocalChannel->iDuration1.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(openLocalChannel->iDuration2.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openLocalChannel->iDuration2.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(openLocalChannel->iRemoteAddress.iCoding==DSATTSY_CODING);
	TEST(openLocalChannel->iRemoteAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openLocalChannel->iUserPassword==DSATTSY_PASSWORD);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - AnyBearer, expect Local channel - passed"),iTestCount++);

	// Open Channel Cancel
	sat.NotifyOpenChannelPCmd(reqStatus,*openLocalChannelPckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel passed"),iTestCount++);

	// [4] Test for CS returned - actually expect error (just using CS for convenience)

	// Open Base channel

	RSat::TOpenChannelBaseV2Pckg openBaseChannelPckg(iOpenBaseChannel);

	iOpenBaseChannel.iPCmdType = DSATTSY_ANY_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus, openBaseChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	sat.NotifyOpenChannelPCmd(reqStatus, openBaseChannelPckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);

	TEST(iOpenBaseChannel.PCmdNumber() == DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(iOpenBaseChannel.iPCmdType    == RSat::EPCmdTypeNotSet);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - AnyBearer, expect error - passed"),iTestCount++);

	// END Open channel "AnyBearer" tests

	// Open Channel Terminal Rsp
	RSat::TOpenChannelRspV2Pckg openChannelRspPckg(iOpenChannelRsp);

	iOpenChannelRsp.SetPCmdNumber(DSATTSY_OPEN_CHANNEL_NUMBER);
	iOpenChannelRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	iOpenChannelRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	iOpenChannelRsp.iAdditionalInfo=DSATTSY_NULL_BUF;
	iOpenChannelRsp.iBearer.iType=DSATTSY_CSD_BEARER_TYPE;
	iOpenChannelRsp.iBearer.iParams=DSATTSY_BEARER_PARAMS;
	iOpenChannelRsp.iBufferSize=DSATTSY_BUFFER_SIZE;

	sat.TerminalRsp(reqStatus,RSat::EOpenChannelLocal,openChannelRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Open Channel - passed"),iTestCount++);

	// continue the tests
	ExtendedTest2L(reqStatus);

	CleanupStack::PopAndDestroy(openLocalChannelPckg);
	CleanupStack::PopAndDestroy(openLocalChannel);

	}

void CTestSatPCmds1d::ExtendedTest2L(TRequestStatus& reqStatus)
	{
	// Now test Open Channel with version 5 of classes

	// Open Channel Cs with V5
	RSat::TOpenCsChannelV5Pckg openCsChannelV5Pckg(iOpenCsChannelV5);

	iOpenCsChannelV5.iPCmdType		= DSATTSY_CS_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus, openCsChannelV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iOpenCsChannelV5.PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(iOpenCsChannelV5.iPCmdType==DSATTSY_CS_BEARER);
	TEST(iOpenCsChannelV5.iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(iOpenCsChannelV5.iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(iOpenCsChannelV5.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iOpenCsChannelV5.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iOpenCsChannelV5.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iOpenCsChannelV5.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iOpenCsChannelV5.iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(iOpenCsChannelV5.iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(iOpenCsChannelV5.iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(iOpenCsChannelV5.iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(iOpenCsChannelV5.iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(iOpenCsChannelV5.iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenCsChannelV5.iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenCsChannelV5.iAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(iOpenCsChannelV5.iAddress.iNumberPlan==DSATTSY_NPI);
	TEST(iOpenCsChannelV5.iAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(iOpenCsChannelV5.iSubAddress==DSATTSY_SUBADDRESS);
	TEST(iOpenCsChannelV5.iDuration1.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(iOpenCsChannelV5.iDuration1.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(iOpenCsChannelV5.iDuration2.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(iOpenCsChannelV5.iDuration2.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(iOpenCsChannelV5.iLocalAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenCsChannelV5.iLocalAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenCsChannelV5.iUserLogin==DSATTSY_LOGIN);
	TEST(iOpenCsChannelV5.iUserPassword==DSATTSY_PASSWORD);

	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	TEST(iOpenCsChannelV5.iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(iOpenCsChannelV5.iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - CS channel (RSat::TOpenCsChannelV5) - passed"),iTestCount++);

	// Open Channel Cancel
	sat.NotifyOpenChannelPCmd(reqStatus,openCsChannelV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenCsChannelV5) passed"),iTestCount++);

	// Open Channel Gprs with V5
	RSat::TOpenGprsChannelV5Pckg openGprsChannelV5Pckg(iOpenGprsChannelV5);

	iOpenGprsChannelV5.iPCmdType=DSATTSY_GPRS_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus,openGprsChannelV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iOpenGprsChannelV5.PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(iOpenGprsChannelV5.iPCmdType==DSATTSY_GPRS_BEARER);
	TEST(iOpenGprsChannelV5.iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(iOpenGprsChannelV5.iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(iOpenGprsChannelV5.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iOpenGprsChannelV5.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iOpenGprsChannelV5.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iOpenGprsChannelV5.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iOpenGprsChannelV5.iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(iOpenGprsChannelV5.iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(iOpenGprsChannelV5.iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(iOpenGprsChannelV5.iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(iOpenGprsChannelV5.iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(iOpenGprsChannelV5.iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenGprsChannelV5.iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenGprsChannelV5.iAccessName==DSATTSY_ACCESS_NAME);
	TEST(iOpenGprsChannelV5.iLocalAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(iOpenGprsChannelV5.iLocalAddress.iAddress==DSATTSY_ADDRESS);
	TEST(iOpenGprsChannelV5.iUserLogin==DSATTSY_LOGIN);
	TEST(iOpenGprsChannelV5.iUserPassword==DSATTSY_PASSWORD);

	TEST(iOpenGprsChannelV5.iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(iOpenGprsChannelV5.iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - GPRS Channel (RSat::TOpenGprsChannelV5) - passed"),iTestCount++);

	// Open Channel Cancel with V5
	sat.NotifyOpenChannelPCmd(reqStatus,openGprsChannelV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenGprsChannelV5) passed"),iTestCount++);

	// continue the tests
	ExtendedTest3L(reqStatus);
	}

void CTestSatPCmds1d::ExtendedTest3L(TRequestStatus& reqStatus)
	{
	// Open Channel Local with version 5
	RSat::TOpenLocalLinksChannelV5* openLocalChannelV5 = new(ELeave) RSat::TOpenLocalLinksChannelV5;
	CleanupStack::PushL(openLocalChannelV5);

	RSat::TOpenLocalLinksChannelV5Pckg* openLocalChannelV5Pckg = new(ELeave) RSat::TOpenLocalLinksChannelV5Pckg(*openLocalChannelV5);
	CleanupStack::PushL(openLocalChannelV5Pckg);

	TEST(openLocalChannelV5->ExtensionId() == RSat::KSatV5);
	TEST(openLocalChannelV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	openLocalChannelV5->iPCmdType=DSATTSY_LOCAL_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus,*openLocalChannelV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(openLocalChannelV5->PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(openLocalChannelV5->iPCmdType==DSATTSY_LOCAL_BEARER);
	TEST(openLocalChannelV5->iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(openLocalChannelV5->iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(openLocalChannelV5->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(openLocalChannelV5->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(openLocalChannelV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(openLocalChannelV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(openLocalChannelV5->iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(openLocalChannelV5->iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(openLocalChannelV5->iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(openLocalChannelV5->iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(openLocalChannelV5->iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(openLocalChannelV5->iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(openLocalChannelV5->iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openLocalChannelV5->iDuration1.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openLocalChannelV5->iDuration1.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(openLocalChannelV5->iDuration2.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openLocalChannelV5->iDuration2.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(openLocalChannelV5->iRemoteAddress.iCoding==DSATTSY_CODING);
	TEST(openLocalChannelV5->iRemoteAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openLocalChannelV5->iUserPassword==DSATTSY_PASSWORD);

	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	TEST(openLocalChannelV5->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(openLocalChannelV5->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - Local Channel (RSat::TOpenLocalLinksChannelV5) - passed"),iTestCount++);

	// Open Channel Cancel with version 5
	sat.NotifyOpenChannelPCmd(reqStatus,*openLocalChannelV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenLocalLinksChannelV5) passed"),iTestCount++);

	CleanupStack::PopAndDestroy(openLocalChannelV5Pckg);
	CleanupStack::PopAndDestroy(openLocalChannelV5);



	// Continue the tests
	ExtendedTest4L(reqStatus);
	}

// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1d::ExtendedTest4L(TRequestStatus& reqStatus)
	{
	// Close Channel with V2
	RSat::TCloseChannelV2Pckg closeChannelPckg(iCloseChannel);

	sat.NotifyCloseChannelPCmd(reqStatus,closeChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iCloseChannel.PCmdNumber()==DSATTSY_CLOSE_CHANNEL_NUMBER);
	TEST(iCloseChannel.iDestination==DSATTSY_CARD_READER_1);
	TEST(iCloseChannel.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iCloseChannel.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iCloseChannel.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iCloseChannel.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyCloseChannelPCmd with RSat::TCloseChannelV2 passed"),iTestCount++);

	// Close Channel Cancel with V2
	sat.NotifyCloseChannelPCmd(reqStatus,closeChannelPckg);
	sat.CancelAsyncRequest(ESatNotifyCloseChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyCloseChannelPCmd Cancel with RSat::TCloseChannelV2 passed"),iTestCount++);

	// Close Channel with V5
	RSat::TCloseChannelV5Pckg closeChannelV5Pckg(iCloseChannelV5);

	TEST(iCloseChannelV5.ExtensionId() == RSat::KSatV5);
	TEST(iCloseChannelV5.iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifyCloseChannelPCmd(reqStatus,closeChannelV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iCloseChannelV5.PCmdNumber()==DSATTSY_CLOSE_CHANNEL_NUMBER);
	TEST(iCloseChannelV5.iDestination==DSATTSY_CARD_READER_1);
	TEST(iCloseChannelV5.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iCloseChannelV5.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iCloseChannelV5.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iCloseChannelV5.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	// V5 parameters
	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	TEST(iCloseChannelV5.iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(iCloseChannelV5.iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyCloseChannelPCmd with RSat::TCloseChannelV5 passed"),iTestCount++);

	// Close Channel Cancel with V5
	sat.NotifyCloseChannelPCmd(reqStatus,closeChannelV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyCloseChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyCloseChannelPCmd Cancel with RSat::TCloseChannelV5 passed"),iTestCount++);

	// Close Channel Terminal Rsp
	sat.NotifyCloseChannelPCmd(reqStatus,closeChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TCloseChannelRspV2* closeChannelRsp = new(ELeave) RSat::TCloseChannelRspV2;
	CleanupStack::PushL(closeChannelRsp);

	RSat::TCloseChannelRspV2Pckg* closeChannelRspPckg = new(ELeave)	RSat::TCloseChannelRspV2Pckg(*closeChannelRsp);
	CleanupStack::PushL(closeChannelRspPckg);

	closeChannelRsp->SetPCmdNumber(iCloseChannel.PCmdNumber());
	closeChannelRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	closeChannelRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	closeChannelRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ECloseChannel,*closeChannelRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Close Channel - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(closeChannelRspPckg);
	CleanupStack::PopAndDestroy(closeChannelRsp);

	// Continue the tests
	ExtendedTest5L(reqStatus);
	}

void CTestSatPCmds1d::ExtendedTest5L(TRequestStatus& reqStatus)
	{
	// Receive Data with V2
	RSat::TReceiveDataV2Pckg receiveDataPckg(iReceiveData);

	sat.NotifyReceiveDataPCmd(reqStatus,receiveDataPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iReceiveData.PCmdNumber()==DSATTSY_RECEIVE_DATA_NUMBER);
	TEST(iReceiveData.iDestination==DSATTSY_CARD_READER_1);
	TEST(iReceiveData.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iReceiveData.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iReceiveData.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iReceiveData.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iReceiveData.iChannelDataLength==DSATTSY_DATA_LENGTH);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyReceiveDataPCmd with RSat::TReceiveDataV2 passed"),iTestCount++);

	// Receive Data Cancel with V2
	sat.NotifyReceiveDataPCmd(reqStatus,receiveDataPckg);
	sat.CancelAsyncRequest(ESatNotifyReceiveDataPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyReceiveDataPCmdCancel with RSat::TReceiveDataV2 passed"),iTestCount++);

	// Receive Data with V5
	RSat::TReceiveDataV5Pckg receiveDataV5Pckg(iReceiveDataV5);

	TEST(iReceiveDataV5.ExtensionId() == RSat::KSatV5);
	TEST(iReceiveDataV5.iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifyReceiveDataPCmd(reqStatus,receiveDataV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iReceiveDataV5.PCmdNumber()==DSATTSY_RECEIVE_DATA_NUMBER);
	TEST(iReceiveDataV5.iDestination==DSATTSY_CARD_READER_1);
	TEST(iReceiveDataV5.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iReceiveDataV5.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iReceiveDataV5.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iReceiveDataV5.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iReceiveDataV5.iChannelDataLength==DSATTSY_DATA_LENGTH);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyReceiveDataPCmd with RSat::TReceiveDataV5 passed"),iTestCount++);

	// Receive Data Cancel with V5
	sat.NotifyReceiveDataPCmd(reqStatus,receiveDataV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyReceiveDataPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyReceiveDataPCmdCancel with RSat::TReceiveDataV5 passed"),iTestCount++);

	// Receive Data Terminal Rsp
	sat.NotifyReceiveDataPCmd(reqStatus,receiveDataPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TReceiveDataRspV2Pckg receiveDataRspPckg(iReceiveDataRsp);

	iReceiveDataRsp.SetPCmdNumber(iReceiveData.PCmdNumber());
	iReceiveDataRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	iReceiveDataRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	iReceiveDataRsp.iAdditionalInfo=DSATTSY_NULL_BUF;
	iReceiveDataRsp.iChannelDataLength=DSATTSY_DATA_LENGTH;

	sat.TerminalRsp(reqStatus,RSat::EReceiveData,receiveDataRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Receive Data - passed"),iTestCount++);

	// continue the tests
	ExtendedTest6L(reqStatus);
	}

// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1d::ExtendedTest6L(TRequestStatus& reqStatus)
	{
	// Send Data with V2
	RSat::TSendDataV2Pckg sendDataPckg(iSendData);

	sat.NotifySendDataPCmd(reqStatus,sendDataPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iSendData.PCmdNumber()==DSATTSY_SEND_DATA_NUMBER);
	TEST(iSendData.iDestination==DSATTSY_CARD_READER_1);
	TEST(iSendData.iMode==DSATTSY_SEND_DATA_MODE);
	TEST(iSendData.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iSendData.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iSendData.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iSendData.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iSendData.iChannelData==DSATTSY_CHANNEL_DATA);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySendDataPCmd with RSat::TSendDataV2 passed"),iTestCount++);

	// Send Data Cancel with V2
	sat.NotifySendDataPCmd(reqStatus,sendDataPckg);
	sat.CancelAsyncRequest(ESatNotifySendDataPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendDataCancel with RSat::TSendDataV2 passed"),iTestCount++);

	// Send Data with V5
	RSat::TSendDataV5Pckg sendDataV5Pckg(iSendDataV5);

	TEST(iSendDataV5.ExtensionId() == RSat::KSatV5);
	TEST(iSendDataV5.iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifySendDataPCmd(reqStatus,sendDataV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iSendDataV5.PCmdNumber()==DSATTSY_SEND_DATA_NUMBER);
	TEST(iSendDataV5.iDestination==DSATTSY_CARD_READER_1);
	TEST(iSendDataV5.iMode==DSATTSY_SEND_DATA_MODE);
	TEST(iSendDataV5.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iSendDataV5.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iSendDataV5.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iSendDataV5.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iSendDataV5.iChannelData==DSATTSY_CHANNEL_DATA);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySendDataPCmd with RSat::TSendDataV5 passed"),iTestCount++);

	// Send Data Cancel with V5
	sat.NotifySendDataPCmd(reqStatus,sendDataV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySendDataPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendDataCancel with RSat::TSendDataV5passed"),iTestCount++);

	// Send Data Terminal Rsp
	sat.NotifySendDataPCmd(reqStatus,sendDataPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TSendDataRspV2Pckg sendDataRspPckg(iSendDataRsp);

	iSendDataRsp.SetPCmdNumber(iSendData.PCmdNumber());
	iSendDataRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	iSendDataRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	iSendDataRsp.iAdditionalInfo=DSATTSY_NULL_BUF;
	iSendDataRsp.iChannelDataLength=DSATTSY_DATA_LENGTH;

	sat.TerminalRsp(reqStatus,RSat::ESendData,sendDataRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Send Data - passed"),iTestCount++);

	// Get Channel Status
	RSat::TGetChannelStatusV2Pckg getChannelStatusPckg(iGetChannelStatus);

	sat.NotifyGetChannelStatusPCmd(reqStatus,getChannelStatusPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iGetChannelStatus.PCmdNumber()==DSATTSY_GET_CHANNEL_STATUS_NUMBER);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetChannelStatusPCmd passed"),iTestCount++);

	// Get Channel Status Cancel
	sat.NotifyGetChannelStatusPCmd(reqStatus,getChannelStatusPckg);
	sat.CancelAsyncRequest(ESatNotifyGetChannelStatusPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetChannelStatusPCmd Cancel passed"),iTestCount++);

	// Get Channel Status Terminal Rsp
	sat.NotifyGetChannelStatusPCmd(reqStatus,getChannelStatusPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TGetChannelStatusRspV2Pckg getChannelStatusRspPckg(iGetChannelStatusRsp);

	iGetChannelStatusRsp.SetPCmdNumber(iGetChannelStatus.PCmdNumber());
	iGetChannelStatusRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	iGetChannelStatusRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	iGetChannelStatusRsp.iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::EGetChannelStatus,getChannelStatusRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - GetChannelStatus - passed"),iTestCount++);

	// Declare Service
	RSat::TDeclareServiceV2Pckg declareServicePckg(iDeclareService);

	sat.NotifyDeclareServicePCmd(reqStatus,declareServicePckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iDeclareService.PCmdNumber()==DSATTSY_DECLARE_SERVICE_NUMBER);
	TEST(iDeclareService.iType==DSATTSY_DECLARE_SERVICE_TYPE);
	TEST(iDeclareService.iServiceRecord.iBearerId==DSATTSY_BEARER_NOT_PRESENT);
	TEST(iDeclareService.iServiceRecord.iServiceId==DSATTSY_NULL_BEARER_ID);
	TEST(iDeclareService.iServiceRecord.iRecord==DSATTSY_NULL_NARROW_BUF);
	TEST(iDeclareService.iInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(iDeclareService.iInterface.iTransportProto==DSATTSY_TCP_PROTO);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyDeclareServicePCmd passed"),iTestCount++);

	// Declare Service Cancel
	sat.NotifyDeclareServicePCmd(reqStatus,declareServicePckg);
	sat.CancelAsyncRequest(ESatNotifyDeclareServicePCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyDeclareServicePCmd Cancel passed"),iTestCount++);

	// Declare Service Terminal Rsp
	sat.NotifyDeclareServicePCmd(reqStatus,declareServicePckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	ExtendedTest7L(reqStatus);
	}

// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1d::ExtendedTest7L(TRequestStatus& reqStatus)
	{
	RSat::TDeclareServiceRspV2* declareServiceRsp = new(ELeave) RSat::TDeclareServiceRspV2;
	CleanupStack::PushL(declareServiceRsp);

	RSat::TDeclareServiceRspV2Pckg* declareServiceRspPckg = new(ELeave) RSat::TDeclareServiceRspV2Pckg(*declareServiceRsp);
	CleanupStack::PushL(declareServiceRspPckg);

	declareServiceRsp->SetPCmdNumber(iDeclareService.PCmdNumber());
	declareServiceRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	declareServiceRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	declareServiceRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::EDeclareService,*declareServiceRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Declare Service - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(declareServiceRspPckg);
	CleanupStack::PopAndDestroy(declareServiceRsp);

	// Service Search with V2
	RSat::TServiceSearchV2Pckg serviceSearchPckg(iServiceSearch);

	sat.NotifyServiceSearchPCmd(reqStatus,serviceSearchPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iServiceSearch.PCmdNumber()==DSATTSY_SERVICE_SEARCH_NUMBER);
	TEST(iServiceSearch.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iServiceSearch.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iServiceSearch.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iServiceSearch.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iServiceSearch.iSearch.iBearerId==DSATTSY_BEARER_TECH_INDEPENDENT);
	TEST(iServiceSearch.iSearch.iSearchParam==DSATTSY_SERVICE_SEARCH_PARAMS);
	TEST(iServiceSearch.iFilter.iBearerId==DSATTSY_BEARER_TECH_INDEPENDENT);
	TEST(iServiceSearch.iFilter.iDeviceFilter==DSATTSY_FILTER_INFO);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyServiceSearchPCmd with RSat::TServiceSearchV2 passed"),iTestCount++);

	// Service Search Cancel with V2
	sat.NotifyServiceSearchPCmd(reqStatus,serviceSearchPckg);
	sat.CancelAsyncRequest(ESatNotifyServiceSearchPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyServiceSearchPCmd Cancel with RSat::TServiceSearchV2 passed"),iTestCount++);

	// Service Search with V5
	RSat::TServiceSearchV5Pckg serviceSearchV5Pckg(iServiceSearchV5);

	TEST(iServiceSearchV5.ExtensionId() == RSat::KSatV5);
	TEST(iServiceSearchV5.iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifyServiceSearchPCmd(reqStatus,serviceSearchV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iServiceSearchV5.PCmdNumber()==DSATTSY_SERVICE_SEARCH_NUMBER);
	TEST(iServiceSearchV5.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iServiceSearchV5.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iServiceSearchV5.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iServiceSearchV5.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iServiceSearchV5.iSearch.iBearerId==DSATTSY_BEARER_TECH_INDEPENDENT);
	TEST(iServiceSearchV5.iSearch.iSearchParam==DSATTSY_SERVICE_SEARCH_PARAMS);
	TEST(iServiceSearchV5.iFilter.iBearerId==DSATTSY_BEARER_TECH_INDEPENDENT);
	TEST(iServiceSearchV5.iFilter.iDeviceFilter==DSATTSY_FILTER_INFO);
	// V5 parameters
	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	TEST(iServiceSearchV5.iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(iServiceSearchV5.iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyServiceSearchPCmd with RSat::TServiceSearchV5 passed"),iTestCount++);

	// Service Search Cancel with V5
	sat.NotifyServiceSearchPCmd(reqStatus,serviceSearchV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyServiceSearchPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyServiceSearchPCmd Cancel with RSat::TServiceSearchV5 passed"),iTestCount++);

	// Service Search Terminal Rsp
	sat.NotifyServiceSearchPCmd(reqStatus,serviceSearchPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TServiceSearchRspV2Pckg serviceSearchRspPckg(iServiceSearchRsp);

	iServiceSearchRsp.SetPCmdNumber(iServiceSearch.PCmdNumber());
	iServiceSearchRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	iServiceSearchRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	iServiceSearchRsp.iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::EServiceSearch,serviceSearchRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Service Search - passed"),iTestCount++);

	ExtendedTest8L(reqStatus);
	}

// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1d::ExtendedTest8L(TRequestStatus& reqStatus)
	{
	// Get Service Information with V2
	RSat::TGetServiceInfoV2Pckg getServiceInfoPckg(iGetServiceInfo);

	sat.NotifyGetServiceInfoPCmd(reqStatus,getServiceInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iGetServiceInfo.PCmdNumber()==DSATTSY_GET_SERVICE_INFO_NUMBER);
	TEST(iGetServiceInfo.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iGetServiceInfo.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iGetServiceInfo.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iGetServiceInfo.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iGetServiceInfo.iAttributeInfo.iBearerId==DSATTSY_BEARER_TECH_INDEPENDENT);
	TEST(iGetServiceInfo.iAttributeInfo.iAttributeInfo==DSATTSY_ATTRIBUTE_INFO);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetServiceInfoPCmd with RSat::TGetServiceInfoV2 passed"),iTestCount++);

	// Get Service Information Cancel with V2
	sat.NotifyGetServiceInfoPCmd(reqStatus,getServiceInfoPckg);
	sat.CancelAsyncRequest(ESatNotifyGetServiceInfoPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetServiceInfoPCmd Cancel with RSat::TGetServiceInfoV2 passed"),iTestCount++);

	// Get Service Information with V5
	RSat::TGetServiceInfoV5Pckg getServiceInfoV5Pckg(iGetServiceInfoV5);

	TEST(iGetServiceInfoV5.ExtensionId() == RSat::KSatV5);
	TEST(iGetServiceInfoV5.iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifyGetServiceInfoPCmd(reqStatus,getServiceInfoV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(iGetServiceInfoV5.PCmdNumber()==DSATTSY_GET_SERVICE_INFO_NUMBER);
	TEST(iGetServiceInfoV5.iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(iGetServiceInfoV5.iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(iGetServiceInfoV5.iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(iGetServiceInfoV5.iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(iGetServiceInfoV5.iAttributeInfo.iBearerId==DSATTSY_BEARER_TECH_INDEPENDENT);
	TEST(iGetServiceInfoV5.iAttributeInfo.iAttributeInfo==DSATTSY_ATTRIBUTE_INFO);

	// V5 parameters
	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	TEST(iGetServiceInfoV5.iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(iGetServiceInfoV5.iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetServiceInfoPCmd with RSat::TGetServiceInfoV5 passed"),iTestCount++);

	// Get Service Information Cancel with V5
	sat.NotifyGetServiceInfoPCmd(reqStatus,getServiceInfoV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyGetServiceInfoPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetServiceInfoPCmd Cancel with RSat::TGetServiceInfoV5 passed"),iTestCount++);

	// Get Service Information Terminal Rsp
	sat.NotifyGetServiceInfoPCmd(reqStatus,getServiceInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TGetServiceInfoRspV2Pckg getServiceInfoRspPckg(iGetServiceInfoRsp);

	iGetServiceInfoRsp.SetPCmdNumber(iGetServiceInfo.PCmdNumber());
	iGetServiceInfoRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	iGetServiceInfoRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	iGetServiceInfoRsp.iAdditionalInfo=DSATTSY_NULL_BUF;
	iGetServiceInfoRsp.iRecordInfo.iBearerId=DSATTSY_BEARER_NOT_PRESENT;
	iGetServiceInfoRsp.iRecordInfo.iServiceId=DSATTSY_NULL_BEARER_ID;
	iGetServiceInfoRsp.iRecordInfo.iRecord=DSATTSY_NULL_NARROW_BUF;

	sat.TerminalRsp(reqStatus,RSat::EGetServiceInformation,getServiceInfoRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Get Service Info - passed"),iTestCount++);
	}
