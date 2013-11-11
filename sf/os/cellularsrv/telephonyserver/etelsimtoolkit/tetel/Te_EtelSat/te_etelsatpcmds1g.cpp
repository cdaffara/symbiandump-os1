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
// The class definition of a TestExecute class for testing EtelSat
// release 7 features.
// 
//

#include "Te_EtelSatTestStepBase.h"
#include "te_etelsatpcmds1g.h"

CTestSatPCmds1g::CTestSatPCmds1g()
	{ 
	SetTestStepName(_L("TestSatPCmds1g"));
	} // CTestSatPCmds1g::CTestSatPCmds1g


enum TVerdict CTestSatPCmds1g::doTestStepL()
/**
 * @SYMTestCaseID TEL-ETEL-NA-4009
 * @SYMREQ PREQ2064 
 * @SYMTestCaseDesc Test Release 7 EtelSat APIs
 * @SYMTestPriority High
 * @SYMTestActions Calls RSat Proactive Commands
 * @SYMTestExpectedResults Pass, client and TSY data is consistent.
 * @SYMTestType CT
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("SAT Release 7 tests"));
	
	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	ret=sat.Open(phone);
	TEST(ret==KErrNone);

	//
	// Test OPEN CHANNEL proactive command when UICC is in server mode
	// Test RSat::TOpenUiccServerModeChannelV7
	// TSAT379, TSAT380
	//
	
	RSat::TOpenUiccServerModeChannelV7* openUiccServerModeChannelV7 = 
		new (ELeave) RSat::TOpenUiccServerModeChannelV7;
	CleanupStack::PushL(openUiccServerModeChannelV7);
		
	RSat::TOpenUiccServerModeChannelV7Pckg* openChannelV7Pckg = 
		new (ELeave) RSat::TOpenUiccServerModeChannelV7Pckg(*openUiccServerModeChannelV7);
	CleanupStack::PushL(openChannelV7Pckg);

	// Assert default parameters are correct
	TEST(openUiccServerModeChannelV7->ExtensionId() == RSat::KSatV7);
	TEST(openUiccServerModeChannelV7->iLinkEst == RSat::ELinkEstablishmentNotSet);
	TEST(openUiccServerModeChannelV7->iReconnectionMode == RSat::EReconnectionModeNotSet);
	TEST(openUiccServerModeChannelV7->iBufferSize == 0);
	TEST(openUiccServerModeChannelV7->iFrameId == 0);
	TEST(openUiccServerModeChannelV7->iBearer.iType == RSat::EBearerTypeNotSet);
	TEST(openUiccServerModeChannelV7->iSimMeInterface.iTransportProto == RSat::EProtocolNotSet);
	TEST(openUiccServerModeChannelV7->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);
	
	openUiccServerModeChannelV7->iPCmdType = DSATTSY_UICC_SERVER_MODE;
	
	TRequestStatus reqStatus = KErrNotFound;
	sat.NotifyOpenChannelPCmd(reqStatus, *openChannelV7Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	// Assert version is correct
	TEST(openUiccServerModeChannelV7->ExtensionId() == RSat::KSatV7);
	
	// Assert parameters from RSat::TOpenChannelBaseV2
	TEST(openUiccServerModeChannelV7->PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(openUiccServerModeChannelV7->iPCmdType==DSATTSY_UICC_SERVER_MODE);
	TEST(openUiccServerModeChannelV7->iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(openUiccServerModeChannelV7->iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(openUiccServerModeChannelV7->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(openUiccServerModeChannelV7->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(openUiccServerModeChannelV7->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(openUiccServerModeChannelV7->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(openUiccServerModeChannelV7->iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(openUiccServerModeChannelV7->iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(openUiccServerModeChannelV7->iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(openUiccServerModeChannelV7->iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(openUiccServerModeChannelV7->iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(openUiccServerModeChannelV7->iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(openUiccServerModeChannelV7->iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	
	// Assert parameters from RSat::TOpenUiccServerModeChannelV7
	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
	TEST(openUiccServerModeChannelV7->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(openUiccServerModeChannelV7->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	
	TEST(openUiccServerModeChannelV7->iFrameId == DSATTSYV6_FRAME_IDENTIFIER1);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd (RSat::TOpenUiccServerModeChannelV7)"), iTestCount++);
	
	// Open Channel Cancel
	sat.NotifyOpenChannelPCmd(reqStatus,*openChannelV7Pckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);
	TEST(reqStatus.Int()==KErrCancel);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenUiccServerModeChannelV7)"), iTestCount++);
	
	//
	// Open Channel Terminal Rsp when response PCmd is RSat::EOpenChannelUiccServerMode
	// TSAT381, TSAT382
	//
	
	RSat::TOpenChannelRspV2* openChannelRsp = new (ELeave) RSat::TOpenChannelRspV2;
	CleanupStack::PushL(openChannelRsp);
	RSat::TOpenChannelRspV2Pckg* openChannelRspPckg = 
		new (ELeave) RSat::TOpenChannelRspV2Pckg(*openChannelRsp);
	CleanupStack::PushL(openChannelRspPckg);

	openChannelRsp->SetPCmdNumber(DSATTSY_OPEN_CHANNEL_NUMBER);
	openChannelRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	openChannelRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	openChannelRsp->iAdditionalInfo=DSATTSY_NULL_BUF;
	openChannelRsp->iBearer.iType=DSATTSY_CSD_BEARER_TYPE;
	openChannelRsp->iBearer.iParams=DSATTSY_BEARER_PARAMS;
	openChannelRsp->iBufferSize=DSATTSY_BUFFER_SIZE;

	sat.TerminalRsp(reqStatus,RSat::EOpenChannelUiccServerMode, *openChannelRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Open Channel UICC server mode rsp"),iTestCount++);
	
	// Terminal response cancel
	sat.TerminalRsp(reqStatus, RSat::EOpenChannelUiccServerMode, *openChannelRspPckg);
	sat.CancelAsyncRequest(ESatTerminalRsp);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Open Channel cancel UICC server mode rsp"),iTestCount++);
	
	sat.Close();
	phone.Close();
	
	CleanupStack::PopAndDestroy(4, openUiccServerModeChannelV7);
	
	return TestStepResult();
	} // CTestSatPCmds1g::doTestStepL
	
