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
#include "TE_EtelSatpcmds1c.h"

CTestSatPCmds1c::CTestSatPCmds1c()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSatPCmds1c"));
	}

enum TVerdict CTestSatPCmds1c::doTestStepL()
	{
	INFO_PRINTF1(_L("***********************************************"));
	INFO_PRINTF1(_L("RSat Proactive Commands Functionality suite - 2"));
	INFO_PRINTF1(_L("***********************************************"));

	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	ret=sat.Open(phone);
    TEST(ret==KErrNone);

	// Send DTMF with V1
	RSat::TSendDtmfV1* sendDtmf = new (ELeave) RSat::TSendDtmfV1;
	CleanupStack::PushL(sendDtmf);
	RSat::TSendDtmfV1Pckg* sendDtmfPckg = new (ELeave) RSat::TSendDtmfV1Pckg(*sendDtmf);
	CleanupStack::PushL(sendDtmfPckg);

	sat.NotifySendDtmfPCmd(reqStatus,*sendDtmfPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(sendDtmf->PCmdNumber()==DSATTSY_SEND_DTMF_NUMBER);

	TEST(sendDtmf->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendDtmf->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(sendDtmf->iDtmfString==DSATTSY_DTMF_STRING);
	TEST(sendDtmf->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(sendDtmf->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendDtmfPCmd with RSat::TSendDtmfV1 passed"),iTestCount++);

	// Send DTMF Cancel with V1
	sat.NotifySendDtmfPCmd(reqStatus,*sendDtmfPckg);
	sat.CancelAsyncRequest(ESatNotifySendDtmfPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendDtmfPCmdCancel with RSat::TSendDtmfV1 passed"),iTestCount++);

	// Send DTMF with V5
	RSat::TSendDtmfV5* sendDtmfV5 = new (ELeave) RSat::TSendDtmfV5;
	CleanupStack::PushL(sendDtmfV5);
	RSat::TSendDtmfV5Pckg* sendDtmfV5Pckg = new (ELeave) RSat::TSendDtmfV5Pckg(*sendDtmfV5);
	CleanupStack::PushL(sendDtmfV5Pckg);

	TEST(sendDtmfV5->ExtensionId() == RSat::KSatV5);
	TEST(sendDtmfV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifySendDtmfPCmd(reqStatus,*sendDtmfV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(sendDtmfV5->PCmdNumber()==DSATTSY_SEND_DTMF_NUMBER);

	TEST(sendDtmfV5->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendDtmfV5->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(sendDtmfV5->iDtmfString==DSATTSY_DTMF_STRING);
	TEST(sendDtmfV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(sendDtmfV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	// V5 parameters
	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	TEST(sendDtmfV5->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(sendDtmfV5->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySendDtmfPCmd with RSat::TSendDtmfV5 passed"),iTestCount++);

	// Send DTMF Cancel with V5
	sat.NotifySendDtmfPCmd(reqStatus,*sendDtmfV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySendDtmfPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendDtmfPCmdCancel with RSat::TSendDtmfV5 passed"),iTestCount++);

	// Send Dtmf Terminal Rsp
	sat.NotifySendDtmfPCmd(reqStatus,*sendDtmfPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TSendDtmfRspV1* sendDtmfRsp = new (ELeave) RSat::TSendDtmfRspV1;
	CleanupStack::PushL(sendDtmfRsp);
	RSat::TSendDtmfRspV1Pckg* sendDtmfRspPckg = new (ELeave) RSat::TSendDtmfRspV1Pckg(*sendDtmfRsp);
	CleanupStack::PushL(sendDtmfRspPckg);

	sendDtmfRsp->SetPCmdNumber(sendDtmf->PCmdNumber());
	sendDtmfRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	sendDtmfRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	sendDtmfRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ESendDtmf,*sendDtmfRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Send DTMF - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, sendDtmf);

	// Perform Card APDU
	RSat::TPerformCardApduV2* performCardApdu = new (ELeave) RSat::TPerformCardApduV2;
	CleanupStack::PushL(performCardApdu);
	RSat::TPerformCardApduV2Pckg* performCardApduPckg = new (ELeave) RSat::TPerformCardApduV2Pckg(*performCardApdu);
	CleanupStack::PushL(performCardApduPckg);

	sat.NotifyPerformCardApduPCmd(reqStatus,*performCardApduPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(performCardApdu->PCmdNumber()==DSATTSY_PERFORM_CARD_APDU_NUMBER);

	TEST(performCardApdu->iDestination==DSATTSY_CARD_READER_1);
	TEST(performCardApdu->iCApdu.iCla==DSATTSY_CLA);
	TEST(performCardApdu->iCApdu.iIns==DSATTSY_INS);
	TEST(performCardApdu->iCApdu.iLc==DSATTSY_LC);
	TEST(performCardApdu->iCApdu.iLe==DSATTSY_LE);
	TEST(performCardApdu->iCApdu.iP1==DSATTSY_P1);
	TEST(performCardApdu->iCApdu.iP2==DSATTSY_P2);
	TEST(performCardApdu->iCApdu.iData==DSATTSY_CAPDU);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyPerformCardApduPCmd passed"),iTestCount++);

	// Perform Card APDU Cancel
	sat.NotifyPerformCardApduPCmd(reqStatus,*performCardApduPckg);
	sat.CancelAsyncRequest(ESatNotifyPerformCardApduPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyPerformCardApduCancel passed"),iTestCount++);

	// Perform Card APDU Terminal Rsp
	sat.NotifyPerformCardApduPCmd(reqStatus,*performCardApduPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TPerformCardApduRspV2* performCardApduRsp = new (ELeave) RSat::TPerformCardApduRspV2;
	CleanupStack::PushL(performCardApduRsp);
	RSat::TPerformCardApduRspV2Pckg* performCardApduRspPckg = new (ELeave) RSat::TPerformCardApduRspV2Pckg(*performCardApduRsp);
	CleanupStack::PushL(performCardApduRspPckg);

	performCardApduRsp->SetPCmdNumber(performCardApdu->PCmdNumber());
	performCardApduRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	performCardApduRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	performCardApduRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::EPerformCardApdu,*performCardApduRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Perform Card APDU - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(4, performCardApdu);

	ExtendedTest1(reqStatus);

	sat.Close();
	phone.Close();

	return TestStepResult();
	}

// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1c::ExtendedTest1(TRequestStatus& reqStatus)
	{
	// Notify Power Off Card
	RSat::TPowerOffCardV2 powerOffCard;
	RSat::TPowerOffCardV2Pckg powerOffCardPckg(powerOffCard);

	sat.NotifyPowerOffCardPCmd(reqStatus,powerOffCardPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(powerOffCard.PCmdNumber()==DSATTSY_POWER_OFF_CARD_NUMBER);
	TEST(powerOffCard.iDestination==DSATTSY_CARD_READER_1);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyPowerOffCardPCmd passed"),iTestCount++);

	// Power Off Card Cancel
	sat.NotifyPowerOffCardPCmd(reqStatus,powerOffCardPckg);
	sat.CancelAsyncRequest(ESatNotifyPowerOffCardPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyPowerOffCardPCmdCancel passed"),iTestCount++);

	// Power Off Card Terminal Rsp
	sat.NotifyPowerOffCardPCmd(reqStatus,powerOffCardPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TPowerOffCardRspV2 powerOffCardRsp;
	RSat::TPowerOffCardRspV2Pckg powerOffCardRspPckg(powerOffCardRsp);

	powerOffCardRsp.SetPCmdNumber(powerOffCard.PCmdNumber());
	powerOffCardRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	powerOffCardRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	powerOffCardRsp.iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::EPowerOffCard,powerOffCardRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Power Off Card - passed"),iTestCount++);

	// Power On Card
	RSat::TPowerOnCardV2 powerOnCard;
	RSat::TPowerOnCardV2Pckg powerOnCardPckg(powerOnCard);

	sat.NotifyPowerOnCardPCmd(reqStatus,powerOnCardPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(powerOnCard.PCmdNumber()==DSATTSY_POWER_ON_CARD_NUMBER);
	TEST(powerOnCard.iDestination==DSATTSY_CARD_READER_1);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyPowerOnCardPCmd passed"),iTestCount++);

	// Power On Card Cancel
	sat.NotifyPowerOnCardPCmd(reqStatus,powerOnCardPckg);
	sat.CancelAsyncRequest(ESatNotifyPowerOnCardPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyPowerOnCardPCmdCancel passed"),iTestCount++);

	// Power On Card Terminal Rsp
	sat.NotifyPowerOnCardPCmd(reqStatus,powerOnCardPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TPowerOnCardRspV2 powerOnCardRsp;
	RSat::TPowerOnCardRspV2Pckg powerOnCardRspPckg(powerOnCardRsp);

	powerOnCardRsp.SetPCmdNumber(powerOnCard.PCmdNumber());
	powerOnCardRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	powerOnCardRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	powerOnCardRsp.iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::EPowerOnCard,powerOnCardRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Power On Card - passed"),iTestCount++);

	// Get Reader Status
	RSat::TGetReaderStatusV2 getReaderStatus;
	RSat::TGetReaderStatusV2Pckg getReaderStatusPckg(getReaderStatus);

	sat.NotifyGetReaderStatusPCmd(reqStatus,getReaderStatusPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(getReaderStatus.PCmdNumber()==DSATTSY_GET_READER_STATUS_NUMBER);
	TEST(getReaderStatus.iDestination==DSATTSY_CARD_READER_1);
	TEST(getReaderStatus.iMode==DSATTSY_GET_READER_STATUS_MODE);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetReaderStatusPCmd passed"),iTestCount++);

	// Get Reader Status Cancel
	sat.NotifyGetReaderStatusPCmd(reqStatus,getReaderStatusPckg);
	sat.CancelAsyncRequest(ESatNotifyGetReaderStatusPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetReaderStatusPCmdCancel passed"),iTestCount++);

	// Get Reader Status Terminal Rsp
	sat.NotifyGetReaderStatusPCmd(reqStatus,getReaderStatusPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TGetReaderStatusRspV2 getReaderStatusRsp;
	RSat::TGetReaderStatusRspV2Pckg getReaderStatusRspPckg(getReaderStatusRsp);

	getReaderStatusRsp.SetPCmdNumber(getReaderStatus.PCmdNumber());
	getReaderStatusRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	getReaderStatusRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	getReaderStatusRsp.iAdditionalInfo=DSATTSY_NULL_BUF;
	getReaderStatusRsp.iMode=DSATTSY_GET_READER_STATUS_MODE;

	sat.TerminalRsp(reqStatus,RSat::EGetReaderStatus,getReaderStatusRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Get Reader Status - passed"),iTestCount++);

	ExtendedTest2L(reqStatus);
}

void CTestSatPCmds1c::ExtendedTest2L(TRequestStatus& reqStatus)
	{
	// Run At Command with V2
	RSat::TRunAtCommandV2* runAtCommand = new (ELeave) RSat::TRunAtCommandV2;
	CleanupStack::PushL(runAtCommand);
	RSat::TRunAtCommandV2Pckg* runAtCommandPckg = new (ELeave) RSat::TRunAtCommandV2Pckg(*runAtCommand);
	CleanupStack::PushL(runAtCommandPckg);

	sat.NotifyRunAtCommandPCmd(reqStatus,*runAtCommandPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(runAtCommand->PCmdNumber()==DSATTSY_RUN_AT_COMMAND_NUMBER);
	TEST(runAtCommand->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(runAtCommand->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(runAtCommand->iAtCommand==DSATTSY_AT_COMMAND);
	TEST(runAtCommand->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(runAtCommand->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyRunAtCommandPCmd with RSat::TRunAtCommandV2 passed"),iTestCount++);

	// Run At Command Cancel with V2
	sat.NotifyRunAtCommandPCmd(reqStatus,*runAtCommandPckg);
	sat.CancelAsyncRequest(ESatNotifyRunAtCommandPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyRunAtCommandPCmdCancel with RSat::TRunAtCommandV2 passed"),iTestCount++);

	// Run At Command with V5
	RSat::TRunAtCommandV5* runAtCommandV5 = new (ELeave) RSat::TRunAtCommandV5;
	CleanupStack::PushL(runAtCommandV5);
	RSat::TRunAtCommandV5Pckg* runAtCommandV5Pckg = new (ELeave) RSat::TRunAtCommandV5Pckg(*runAtCommandV5);
	CleanupStack::PushL(runAtCommandV5Pckg);

	TEST(runAtCommandV5->ExtensionId() == RSat::KSatV5);
	TEST(runAtCommandV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifyRunAtCommandPCmd(reqStatus,*runAtCommandV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(runAtCommandV5->PCmdNumber()==DSATTSY_RUN_AT_COMMAND_NUMBER);
	TEST(runAtCommandV5->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(runAtCommandV5->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(runAtCommandV5->iAtCommand==DSATTSY_AT_COMMAND);
	TEST(runAtCommandV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(runAtCommandV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	// Version 5 parameters
	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	TEST(runAtCommandV5->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(runAtCommandV5->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyRunAtCommandPCmd with RSat::TRunAtCommandV5 passed"),iTestCount++);

	// Run At Command Cancel with V2
	sat.NotifyRunAtCommandPCmd(reqStatus,*runAtCommandV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyRunAtCommandPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyRunAtCommandPCmdCancel with RSat::TRunAtCommandV5 passed"),iTestCount++);

	// Run At Command Terminal Rsp
	sat.NotifyRunAtCommandPCmd(reqStatus,*runAtCommandPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TRunAtCommandRspV2* runAtCommandRsp = new (ELeave) RSat::TRunAtCommandRspV2;
	CleanupStack::PushL(runAtCommandRsp);
	RSat::TRunAtCommandRspV2Pckg* runAtCommandRspPckg = new (ELeave) RSat::TRunAtCommandRspV2Pckg(*runAtCommandRsp);
	CleanupStack::PushL(runAtCommandRspPckg);

	runAtCommandRsp->SetPCmdNumber(runAtCommand->PCmdNumber());
	runAtCommandRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	runAtCommandRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	runAtCommandRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ERunAtCommand,*runAtCommandRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Run AT Command - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, runAtCommand);

	// Language Notification
	RSat::TLanguageNotificationV2 languageNotification;
	RSat::TLanguageNotificationV2Pckg languageNotificationPckg(languageNotification);

	sat.NotifyLanguageNotificationPCmd(reqStatus,languageNotificationPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(languageNotification.PCmdNumber()==DSATTSY_LANGUAGE_NOTIFICATION_NUMBER);
	TEST(languageNotification.iNotificationType==DSATTSY_SPECIFIC_LANGUAGE);
	TEST(languageNotification.iLanguage==DSATTSY_FRENCH);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyLanguageNotificationPCmd passed"),iTestCount++);

	// Language Notification Cancel
	sat.NotifyLanguageNotificationPCmd(reqStatus,languageNotificationPckg);
	sat.CancelAsyncRequest(ESatNotifyLanguageNotificationPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyLanguageNotificationPCmdCancel passed"),iTestCount++);

	// Language Notification Terminal Rsp
	sat.NotifyLanguageNotificationPCmd(reqStatus,languageNotificationPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TLanguageNotificationRspV2 languageNotificationRsp;
	RSat::TLanguageNotificationRspV2Pckg languageNotificationRspPckg(languageNotificationRsp);

	languageNotificationRsp.SetPCmdNumber(languageNotification.PCmdNumber());
	languageNotificationRsp.iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	languageNotificationRsp.iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	languageNotificationRsp.iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ELanguageNotification,languageNotificationRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Language Notification - passed"),iTestCount++);

	ExtendedTest3L(reqStatus);
	}

// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1c::ExtendedTest3L(TRequestStatus& reqStatus)
	{
	//
	// Launch Browser
	//

	// Test version 2 command
	RSat::TLaunchBrowserV2*		launchBrowser = new (ELeave) RSat::TLaunchBrowserV2;
	CleanupStack::PushL(launchBrowser);
	RSat::TLaunchBrowserV2Pckg*	launchBrowserPckg = new (ELeave) RSat::TLaunchBrowserV2Pckg(*launchBrowser);
	CleanupStack::PushL(launchBrowserPckg);

	TestLaunchBrowser(launchBrowserPckg);

	CleanupStack::PopAndDestroy(2, launchBrowser);

	// Continue tests
	ExtendedTest4L(reqStatus);
	}

// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1c::ExtendedTest4L(TRequestStatus& reqStatus)
	{
	// Test version 5 command
	RSat::TLaunchBrowserV5*		launchBrowserV5 = new (ELeave) RSat::TLaunchBrowserV5;
	CleanupStack::PushL(launchBrowserV5);
	RSat::TLaunchBrowserV5Pckg*	launchBrowserV5Pckg = new (ELeave) RSat::TLaunchBrowserV5Pckg(*launchBrowserV5);
	CleanupStack::PushL(launchBrowserV5Pckg);

	TEST(launchBrowserV5->ExtensionId() == RSat::KSatV5);
	TEST(launchBrowserV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);
	TestLaunchBrowser(launchBrowserV5Pckg);

	// Launch Browser Terminal Rsp
	RSat::TLaunchBrowserV2*		launchBrowser = new (ELeave) RSat::TLaunchBrowserV2;
	CleanupStack::PushL(launchBrowser);
	RSat::TLaunchBrowserV2Pckg*	launchBrowserPckg = new (ELeave) RSat::TLaunchBrowserV2Pckg(*launchBrowser);
	CleanupStack::PushL(launchBrowserPckg);

	sat.NotifyLaunchBrowserPCmd(reqStatus,*launchBrowserPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TLaunchBrowserRspV2* launchBrowserRsp = new (ELeave) RSat::TLaunchBrowserRspV2;
	CleanupStack::PushL(launchBrowserRsp);
	RSat::TLaunchBrowserRspV2Pckg* launchBrowserRspPckg = new (ELeave) RSat::TLaunchBrowserRspV2Pckg(*launchBrowserRsp);
	CleanupStack::PushL(launchBrowserRspPckg);

	launchBrowserRsp->SetPCmdNumber(launchBrowser->PCmdNumber());
	launchBrowserRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	launchBrowserRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	launchBrowserRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ELaunchBrowser,*launchBrowserRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Launch Browser - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, launchBrowserV5);

	// Continue tests
	ExtendedTest5(reqStatus);
	}

// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1c::ExtendedTest5(TRequestStatus& reqStatus)
	{
	//
	// Get Provisioning File Ref
	//

	RSat::TProvisioningFileRef		fileRef;
	TBuf8<256>						retFile;

	// Get with valid file ref

	fileRef = DSATTSY_FILE_REF_DATA_1;

	sat.GetProvisioningRefFile(reqStatus, fileRef, retFile);
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrNone);
	TEST(retFile == DSATTSY_FILE1);

	INFO_PRINTF2(_L("Test %d - RSat::GetProvisioningRefFile(valid file) passed"),iTestCount++);

	sat.GetProvisioningRefFile(reqStatus, fileRef, retFile);
	sat.CancelAsyncRequest(ESatGetProvisioningRefFile);
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrCancel);

	INFO_PRINTF2(_L("Test %d - RSat::GetProvisioningRefFileCancel Cancel passed"),iTestCount++);

	// Get with invalid file ref

	fileRef = DSATTSY_INVALID_REF_FILE;

	sat.GetProvisioningRefFile(reqStatus, fileRef, retFile);
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrNotFound);

	INFO_PRINTF2(_L("Test %d - RSat::GetProvisioningRefFile(invalid file) passed"),iTestCount++);

	sat.GetProvisioningRefFile(reqStatus, fileRef, retFile);
	sat.CancelAsyncRequest(ESatGetProvisioningRefFile);
	User::WaitForRequest(reqStatus);

	TEST(reqStatus.Int()==KErrCancel);

	INFO_PRINTF2(_L("Test %d - RSat::GetProvisioningRefFileCancel Cancel passed"),iTestCount++);
	}

void CTestSatPCmds1c::TestLaunchBrowser(TDes8* aLaunchBrowserPCmdPckg)
	{
	RSat::TLaunchBrowserV2Pckg* launchBrowserPckg = (RSat::TLaunchBrowserV2Pckg*)aLaunchBrowserPCmdPckg;
	RSat::TLaunchBrowserV2& launchBrowser = (*launchBrowserPckg)();

	RSat::TProvisioningFileRef  file;

	// Perform 5 tests, check comment for each test to see the expected result for that test
	for (TInt browserTest = 0; browserTest < 5; ++browserTest)		// Note start:end indices
		{
		sat.NotifyLaunchBrowserPCmd(reqStatus,*launchBrowserPckg);
		User::WaitForRequest(reqStatus);
		TEST(reqStatus.Int()==KErrNone);

		TEST(launchBrowser.PCmdNumber()			== DSATTSY_LAUNCH_BROWSER_NUMBER);
		TEST(launchBrowser.iBrowserSel			== DSATTSY_BROWSER_SELECTION_MODE);
		TEST(launchBrowser.iBrowserId			== DSATTSY_BROWSER_ID);
		TEST(launchBrowser.iUrl					== DSATTSY_URL);
		TEST(launchBrowser.iBearerList			== DSATTSY_BEARER_LIST);
		TEST(launchBrowser.iText				== DSATTSY_PROXY_NAME);
		TEST(launchBrowser.iAlphaId.iStatus		== DSATTSY_ALPHA_ID1_STATUS);
		TEST(launchBrowser.iAlphaId.iAlphaId	== DSATTSY_ALPHA_ID1);
		TEST(launchBrowser.iIconId.iIdentifier	== DSATTSY_ICON_ID1);
		TEST(launchBrowser.iIconId.iQualifier	== DSATTSY_ICON_QUALIFIER1);

		if (launchBrowser.ExtensionId() == RSat::KSatV5)
			{
			RSat::TLaunchBrowserV5Pckg* aV5PCmdPckg = (RSat::TLaunchBrowserV5Pckg*)aLaunchBrowserPCmdPckg;
			RSat::TLaunchBrowserV5&   launchBrowserV5 = (*aV5PCmdPckg)();

			RSat::TTextAttribute textAttribute;
			textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

			TEST(launchBrowserV5.iTextAttribute.iStatus == textAttribute.iStatus);
			TEST(launchBrowserV5.iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
			}

		// Provisioning file ref checks

		switch (browserTest)
			{
		case 0:
			// expect 1 file ref of 1 byte length
			TEST(launchBrowser.NumberOfFileRef()   == DSATTSY_REF_COUNT_1);
			TEST(launchBrowser.GetFileRef(1, file) == KErrNone);
			TEST(file == DSATTSY_FILE_REF_DATA_1);
			break;

		case 1:
			// expect 1 file ref of 128 bytes length
			TEST(launchBrowser.NumberOfFileRef()   == DSATTSY_REF_COUNT_2);
			TEST(launchBrowser.GetFileRef(1, file) == KErrNone);
			TEST(file == DSATTSY_FILE_REF_DATA_128);
			break;

		case 2:
			// expect 2 file refs, each of 1 byte length
			TEST(launchBrowser.NumberOfFileRef()   == DSATTSY_REF_COUNT_3);
			TEST(launchBrowser.GetFileRef(1, file) == KErrNone);
			TEST(file == DSATTSY_FILE_REF_DATA_1);
			TEST(launchBrowser.GetFileRef(2, file) == KErrNone);
			TEST(file == DSATTSY_FILE_REF_DATA_1);
			break;

		case 3:
			// 1 ref of 128 bytes, test TSY attempts to add 2 refs but 2nd too large so should be rejected
			TEST(launchBrowser.NumberOfFileRef()   == DSATTSY_REF_COUNT_4);
			TEST(launchBrowser.GetFileRef(1, file) == KErrNone);
			TEST(file == DSATTSY_FILE_REF_DATA_128);
			break;

		case 4:
			// 2 refs of 128 bytes & 1 byte
			TEST(launchBrowser.NumberOfFileRef()   == DSATTSY_REF_COUNT_5);
			TEST(launchBrowser.GetFileRef(1, file) == KErrNone);
			TEST(file == DSATTSY_FILE_REF_DATA_128);
			TEST(launchBrowser.GetFileRef(2, file) == KErrNone);
			TEST(file == DSATTSY_FILE_REF_DATA_1);
			// Ask for out-of-range file ref - greater than last index
			TEST(launchBrowser.GetFileRef(3, file) == KErrNotFound);
			// Ask for out-of-range file ref - bigger than max ref
			TEST(launchBrowser.GetFileRef(255, file) == KErrNotFound);
			break;
			}

		launchBrowser.ResetFileRef();

		if (launchBrowser.ExtensionId() == RSat::KSatV2)
			{
			INFO_PRINTF2(_L("Test %d - RSat::NotifyLaunchBrowserPCmd with RSat::TLaunchBrowserV2 passed"),iTestCount++);
			}
		else if (launchBrowser.ExtensionId() == RSat::KSatV5)
			{
			INFO_PRINTF2(_L("Test %d - RSat::NotifyLaunchBrowserPCmd with RSat::TLaunchBrowserV5 passed"),iTestCount++);
			}

		// Launch Browser Cancel
		sat.NotifyLaunchBrowserPCmd(reqStatus,*launchBrowserPckg);
		sat.CancelAsyncRequest(ESatNotifyLaunchBrowserPCmd);

		User::WaitForRequest(reqStatus);
		TEST(reqStatus.Int()==KErrCancel);

		if (launchBrowser.ExtensionId() == RSat::KSatV2)
			INFO_PRINTF2(_L("Test %d - RSat::NotifyLaunchBrowserPCmdCancel with RSat::TLaunchBrowserV2 passed"),iTestCount++);
		else if (launchBrowser.ExtensionId() == RSat::KSatV5)
			INFO_PRINTF2(_L("Test %d - RSat::NotifyLaunchBrowserPCmdCancel with RSat::TLaunchBrowserV5 passed"),iTestCount++);

		}
	}
