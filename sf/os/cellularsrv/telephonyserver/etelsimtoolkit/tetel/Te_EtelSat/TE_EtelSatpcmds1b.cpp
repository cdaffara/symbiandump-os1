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
#include "TE_EtelSatpcmds1b.h"
#include <utf.h>

CTestSatPCmds1b::CTestSatPCmds1b()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSatPCmds1b"));
	}

enum TVerdict CTestSatPCmds1b::doTestStepL()
	{
	INFO_PRINTF1(_L("***********************************************"));
	INFO_PRINTF1(_L("RSat Proactive Commands Functionality suite - 1"));
	INFO_PRINTF1(_L("***********************************************"));

	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	ret=sat.Open(phone);
    TEST(ret==KErrNone);

	// local variables used throughout the sat tests
	// Boundary error testing

	// Select Item
	RSat::TSelectItemV2* selectItem = new (ELeave) RSat::TSelectItemV2;
	CleanupStack::PushL(selectItem);

	RSat::TSelectItemV2Pckg* selectItemPckg = new (ELeave) RSat::TSelectItemV2Pckg(*selectItem);
	CleanupStack::PushL(selectItemPckg);

	TEST(selectItem->ExtensionId()== RSat::KSatV2);
	TEST(selectItem->NumberOfItems()==0);
	TEST(selectItem->IsRemoveMenu());

	sat.NotifySelectItemPCmd(reqStatus, *selectItemPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(selectItem->MenuContent()==DSATTSY_ITEMS_ACTIONS_AND_ICONS);
	TEST(selectItem->NumberOfItems()==0x03);
	TEST(selectItem->iPreference==DSATTSY_SELECTION_PREFERENCE);

	index=1;
	TEST(selectItem->GetItem(index++,item,action,icon)==KErrNone);

	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(action==0x01);
	TEST(icon==0x01);

	TEST(selectItem->GetItem(index++,item,action,icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(action==0x02);
	TEST(icon==0x02);

	TEST(selectItem->GetItem(index++,item,action,icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(action==0x03);
	TEST(icon==0x03);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySelectItemPCmd with RSat::TSelectItemV2 passed"),iTestCount++);

	// Select Item Cancel
	sat.NotifySelectItemPCmd(reqStatus,*selectItemPckg);
	sat.CancelAsyncRequest(ESatNotifySelectItemPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySelectItemPCmdCancel with RSat::TSelectItemV2 passed"),iTestCount++);

	// Select Item with RSat::TSelectItemV5

	// Text attribute parameters used for tests
	RSat::TTextAttribute textAttribute1, textAttribute2, textAttribute3, textAttribute4;

	textAttribute1.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute1.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	textAttribute2.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS2;
	const TUint8 textAttributeData2[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA2;
	textAttribute2.iTextAttributeData.Append(textAttributeData2, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	textAttribute3.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS3;
	const TUint8 textAttributeData3[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA3;
	textAttribute3.iTextAttributeData.Append(textAttributeData3, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	textAttribute4.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS4;
	const TUint8 textAttributeData4[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA4;
	textAttribute4.iTextAttributeData.Append(textAttributeData4, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	RSat::TSelectItemV5* selectItemV5 = new (ELeave) RSat::TSelectItemV5;
	CleanupStack::PushL(selectItemV5);

	TEST(selectItemV5->ExtensionId()== RSat::KSatV5);
	TEST(selectItemV5->NumberOfItems()==0);
	TEST(selectItemV5->IsRemoveMenu());

	TEST(selectItemV5->iAlphaId.iStatus==RSat::EAlphaIdNotSet);
	TEST(selectItemV5->iHelp==RSat::EHelpNotSet);
	TEST(selectItemV5->iDefaultItemId==0);
	TEST(selectItemV5->iIconId.iQualifier==RSat::EIconQualifierNotSet);
	TEST(selectItemV5->iIconId.iIdentifier==0);
	TEST(selectItemV5->iIconListQualifier==RSat::EIconQualifierNotSet);
	TEST(selectItemV5->iPreference==RSat::ESelectionPreferenceNotSet);
	TEST(selectItemV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	RSat::TSelectItemV5Pckg* selectItemV5Pckg = new (ELeave) RSat::TSelectItemV5Pckg(*selectItemV5);
	CleanupStack::PushL(selectItemV5Pckg);

	sat.NotifySelectItemPCmd(reqStatus, *selectItemV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(selectItemV5->MenuContent()==DSATTSY_ITEMS_WITH_TEXT_ATTRIBUTES_ACTIONS_AND_ICONS);
	TEST(selectItemV5->NumberOfItems()==0x03);
	TEST(selectItemV5->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(selectItemV5->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(selectItemV5->iHelp==DSATTSY_HELP);
	TEST(selectItemV5->iDefaultItemId==DSATTSY_DEFAULT_ITEM);
	TEST(selectItemV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(selectItemV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(selectItemV5->iIconListQualifier==DSATTSY_ICON_QUALIFIER2);
	TEST(selectItemV5->iPreference==DSATTSY_SELECTION_PREFERENCE);

	TEST(selectItemV5->iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(selectItemV5->iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);

	index = 1;
	TEST(selectItemV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);
	TEST(icon==0x01);
	TEST(action==0x01);

	TEST(selectItemV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == textAttribute2.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute2.iTextAttributeData);
	TEST(icon==0x02);
	TEST(action==0x02);

	TEST(selectItemV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == textAttribute3.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute3.iTextAttributeData);
	TEST(icon==0x03);
	TEST(action==0x03);

	TEST(selectItemV5->GetItem(index,item, iTextAttribute, action)==KErrNotFound);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySelectItemPCmd with RSat::TSelectItemV5 passed"),iTestCount++);

	// Select Item Cancel with RSat::TSelectItemV5
	sat.NotifySelectItemPCmd(reqStatus,*selectItemV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySelectItemPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySelectItemPCmdCancel with RSat::TSelectItemV5 passed"),iTestCount++);

	// Select Item Terminal Response
	sat.NotifySelectItemPCmd(reqStatus,*selectItemPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TSelectItemRspV1* selectItemRsp = new (ELeave) RSat::TSelectItemRspV1;
	CleanupStack::PushL(selectItemRsp);
	RSat::TSelectItemRspV1Pckg* selectItemRspPckg = new (ELeave) RSat::TSelectItemRspV1Pckg(*selectItemRsp);
	CleanupStack::PushL(selectItemRspPckg);

	// Testing that PCmdNumber = 0 is not supported as out of range
	TEST(selectItemRsp->SetPCmdNumber(0)==KErrNotSupported);

	selectItemRsp->SetPCmdNumber(selectItem->PCmdNumber());
	selectItemRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	selectItemRsp->iInfoType=DSATTSY_PCMD_RSP_ITEM_ID;
	selectItemRsp->iAdditionalInfo=DSATTSY_RSP_ITEM_ID;

	sat.TerminalRsp(reqStatus,RSat::ESelectItem,*selectItemRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Select Item - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, selectItem);

	// Send Sm with V1
	RSat::TSendSmV1* sendSm = new (ELeave) RSat::TSendSmV1;
	CleanupStack::PushL(sendSm);
	RSat::TSendSmV1Pckg* sendSmPckg = new (ELeave) RSat::TSendSmV1Pckg(*sendSm);
	CleanupStack::PushL(sendSmPckg);

	sat.NotifySendSmPCmd(reqStatus,*sendSmPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(sendSm->PCmdNumber()==DSATTSY_SEND_SM_NUMBER);

	TEST(sendSm->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendSm->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(sendSm->iAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(sendSm->iAddress.iNumberPlan==DSATTSY_NPI);
	TEST(sendSm->iAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(sendSm->iSmsTpdu==DSATTSY_SMS_TPDU);
	TEST(sendSm->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(sendSm->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendSmPCmd with RSat::TSendSmV1 passed"),iTestCount++);

	// Send Sm Cancel with V1
	sat.NotifySendSmPCmd(reqStatus,*sendSmPckg);
	sat.CancelAsyncRequest(ESatNotifySendSmPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendSmPCmdCancel with RSat::TSendSmV1 passed"),iTestCount++);

	// Send Sm with V5
	RSat::TSendSmV5* sendSmV5 = new (ELeave) RSat::TSendSmV5;
	CleanupStack::PushL(sendSmV5);
	RSat::TSendSmV5Pckg* sendSmV5Pckg = new (ELeave) RSat::TSendSmV5Pckg(*sendSmV5);
	CleanupStack::PushL(sendSmV5Pckg);

	TEST(sendSmV5->ExtensionId() == RSat::KSatV5);
	TEST(sendSmV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifySendSmPCmd(reqStatus, *sendSmV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(sendSmV5->PCmdNumber()==DSATTSY_SEND_SM_NUMBER);

	TEST(sendSmV5->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendSmV5->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(sendSmV5->iAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(sendSmV5->iAddress.iNumberPlan==DSATTSY_NPI);
	TEST(sendSmV5->iAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(sendSmV5->iSmsTpdu==DSATTSY_SMS_TPDU);
	TEST(sendSmV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(sendSmV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);

	// V5 parameters

	TEST(sendSmV5->iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(sendSmV5->iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySendSmPCmd with RSat::TSendSmV5 passed"),iTestCount++);

	// Send Sm Cancel with V5
	sat.NotifySendSmPCmd(reqStatus,*sendSmV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySendSmPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendSmPCmdCancel with RSat::TSendSmV5 passed"),iTestCount++);

	// Send Sm Terminal Rsp
	sat.NotifySendSmPCmd(reqStatus,*sendSmPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TSendSmRspV1* sendSmRsp = new (ELeave) RSat::TSendSmRspV1;
	CleanupStack::PushL(sendSmRsp);
	RSat::TSendSmRspV1Pckg* sendSmRspPckg = new (ELeave) RSat::TSendSmRspV1Pckg(*sendSmRsp);
	CleanupStack::PushL(sendSmRspPckg);

	sendSmRsp->SetPCmdNumber(sendSm->PCmdNumber());
	sendSmRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	sendSmRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	sendSmRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ESendSm,*sendSmRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendSmPCmd - Send Sm - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, sendSm);

	// Send Message No Logging
	RSat::TSatSmsV1* satSms = new (ELeave) RSat::TSatSmsV1;
	CleanupStack::PushL(satSms);
	RSat::TSatSmsV1Pckg* satSmsPckg = new (ELeave) RSat::TSatSmsV1Pckg(*satSms);
	CleanupStack::PushL(satSmsPckg);

	satSms->iBuf=DSATTSY_SMS_TPDU;
	satSms->iServiceCenter.iNumberPlan=DSATTSY_NPI;
	satSms->iServiceCenter.iTypeOfNumber=DSATTSY_TON;
	satSms->iServiceCenter.iTelNumber=DSATTSY_TEL_NUMBER;
	TUint16 smsRef;

	sat.SendMessageNoLogging(reqStatus,*satSmsPckg,smsRef);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(smsRef==DSATTSY_SMS_REF);
	INFO_PRINTF2(_L("Test %d - RSat::SendMessageNoLogging passed"),iTestCount++);

	// Send Message No Logging Cancel
	sat.SendMessageNoLogging(reqStatus,*satSmsPckg,smsRef);
	sat.CancelAsyncRequest(ESatSendMessageNoLogging);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::SendMessageNoLoggingCancel passed"),iTestCount++);

	CleanupStack::PopAndDestroy(2, satSms);

	// Send Ss
	RSat::TSendSsV1* sendSs = new (ELeave) RSat::TSendSsV1;
	CleanupStack::PushL(sendSs);
	RSat::TSendSsV1Pckg* sendSsPckg = new (ELeave) RSat::TSendSsV1Pckg(*sendSs);
	CleanupStack::PushL(sendSsPckg);

	sat.NotifySendSsPCmd(reqStatus,*sendSsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(sendSs->PCmdNumber()==DSATTSY_SEND_SS_NUMBER);

	TEST(sendSs->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendSs->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(sendSs->iSsString.iTypeOfNumber==DSATTSY_TON);
	TEST(sendSs->iSsString.iNumberPlan==DSATTSY_NPI);
	TEST(sendSs->iSsString.iSsString==DSATTSY_SS_STRING);
	TEST(sendSs->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(sendSs->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendSsPCmd passed"),iTestCount++);

	// Send Ss Cancel
	sat.NotifySendSsPCmd(reqStatus,*sendSsPckg);
	sat.CancelAsyncRequest(ESatNotifySendSsPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendSsPCmdCancel passed"),iTestCount++);

	// Send Ss Terminal Rsp
	sat.NotifySendSsPCmd(reqStatus,*sendSsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TSendSsRspV2* sendSsRsp = new (ELeave) RSat::TSendSsRspV2;
	CleanupStack::PushL(sendSsRsp);
	RSat::TSendSsRspV2Pckg* sendSsRspPckg = new (ELeave) RSat::TSendSsRspV2Pckg(*sendSsRsp);
	CleanupStack::PushL(sendSsRspPckg);

	sendSsRsp->SetPCmdNumber(sendSs->PCmdNumber());

	RSat::TCallControlV1* ccRequestedAction = new (ELeave) RSat::TCallControlV1;
	CleanupStack::PushL(ccRequestedAction);

	sendSsRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	sendSsRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	sendSsRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sendSsRsp->iUssdString.iUssdString=DSATTSY_DEFAULT_TEXT;
	sendSsRsp->iUssdString.iDcs=DSATTSY_DCS;

	RSat::TCallSetUpParams* params = new (ELeave) RSat::TCallSetUpParams;
	CleanupStack::PushL(params);

	params->iCcp1					=DSATTSY_CCP1;
	params->iSubAddress				=DSATTSY_SUBADDRESS;
	params->iCcp2					=DSATTSY_CCP2;
	params->iAddress.iTypeOfNumber	=DSATTSY_TON;
	params->iAddress.iNumberPlan		=DSATTSY_NPI;
	params->iAddress.iTelNumber		=DSATTSY_TEL_NUMBER;

	RSat::TAlphaId* nullBuf = new (ELeave) RSat::TAlphaId;
	CleanupStack::PushL(nullBuf);

	nullBuf->iAlphaId=DSATTSY_NULL_BUF;
	nullBuf->iStatus=DSATTSY_ALPHAID_STATUS;
	ccRequestedAction->SetCcGeneralResult(DSATTSY_CC_RESULT);
	ccRequestedAction->SetAlphaId(DSATTSY_NO_ALPHAID, *nullBuf);
	ccRequestedAction->SetCallSetUpDetails(*params);
	ccRequestedAction->SetBCRepeatIndicator(RSat::EBCSequentialMode);

	sendSsRsp->iCcRequestedAction=*ccRequestedAction;

	sendSsRsp->iGeneralResult2=DSATTSY_PCMD_RESULT_SUCCESS;
	sendSsRsp->iInfoType2=DSATTSY_PCMD_RSP_NO_INFO;
	sendSsRsp->iAdditionalInfo2=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ESendSs,*sendSsRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp -Send Ss - passed"),iTestCount++);

	// Send Ussd
	RSat::TSendUssdV1* sendUssd = new (ELeave) RSat::TSendUssdV1;
	CleanupStack::PushL(sendUssd);
	RSat::TSendUssdV1Pckg* sendUssdPckg = new (ELeave) RSat::TSendUssdV1Pckg(*sendUssd);
	CleanupStack::PushL(sendUssdPckg);

	sat.NotifySendUssdPCmd(reqStatus,*sendUssdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(sendUssd->PCmdNumber()==DSATTSY_SEND_USSD_NUMBER);

	TEST(sendUssd->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendUssd->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(sendUssd->iUssdString.iDcs==DSATTSY_DCS);
	TEST(sendUssd->iUssdString.iUssdString==DSATTSY_USSD_STRING);
	TEST(sendUssd->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(sendUssd->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendUssdPCmd passed"),iTestCount++);

	// Send Ussd Cancel
	sat.NotifySendUssdPCmd(reqStatus,*sendUssdPckg);
	sat.CancelAsyncRequest(ESatNotifySendUssdPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySendUssdPCmdCancel passed"), iTestCount++);

	// Send Ussd Terminal Rsp
	sat.NotifySendUssdPCmd(reqStatus,*sendUssdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TSendUssdRspV1* sendUssdRsp = new (ELeave) RSat::TSendUssdRspV1;
	CleanupStack::PushL(sendUssdRsp);
	RSat::TSendUssdRspV1Pckg* sendUssdRspPckg = new (ELeave) RSat::TSendUssdRspV1Pckg(*sendUssdRsp);
	CleanupStack::PushL(sendUssdRspPckg);

	sendUssdRsp->SetPCmdNumber(sendUssd->PCmdNumber());

	sendUssdRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	sendUssdRsp->iInfoType=DSATTSY_PCMD_RSP_TEXT_STRING;
	sendUssdRsp->iAdditionalInfo=DSATTSY_NETWORK_TEXT_STRING;

	sendUssdRsp->iCcRequestedAction=*ccRequestedAction;

	sendUssdRsp->iGeneralResult2=DSATTSY_PCMD_RESULT_SUCCESS;
	sendUssdRsp->iInfoType2=DSATTSY_PCMD_RSP_NO_INFO;
	sendUssdRsp->iAdditionalInfo2=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ESendUssd,*sendUssdRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Send USSD - passed"),iTestCount++);

	// Set Up Call
	RSat::TSetUpCallV1* setUpCall = new (ELeave) RSat::TSetUpCallV1;
	CleanupStack::PushL(setUpCall);
	RSat::TSetUpCallV1Pckg* setUpCallPckg = new (ELeave) RSat::TSetUpCallV1Pckg(*setUpCall);
	CleanupStack::PushL(setUpCallPckg);

	sat.NotifySetUpCallPCmd(reqStatus,*setUpCallPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpCall->PCmdNumber()==DSATTSY_SET_UP_CALL_NUMBER);

	TEST(setUpCall->iType==DSATTSY_SET_UP_CALL_TYPE);
	TEST(setUpCall->iAlphaIdConfirmationPhase.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(setUpCall->iAlphaIdConfirmationPhase.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(setUpCall->iIconIdConfirmationPhase.iIdentifier==DSATTSY_ICON_ID1);
	TEST(setUpCall->iIconIdConfirmationPhase.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(setUpCall->iAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(setUpCall->iAddress.iNumberPlan==DSATTSY_NPI);
	TEST(setUpCall->iAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(setUpCall->iCapabilityConfigParams==DSATTSY_CCP1);
	TEST(setUpCall->iSubAddress==DSATTSY_SUBADDRESS);
	TEST(setUpCall->iDuration.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(setUpCall->iDuration.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(setUpCall->iAlphaIdCallSetUpPhase.iStatus==DSATTSY_ALPHA_ID2_STATUS);
	TEST(setUpCall->iAlphaIdCallSetUpPhase.iAlphaId==DSATTSY_ALPHA_ID2);
	TEST(setUpCall->iIconIdCallSetUpPhase.iIdentifier==DSATTSY_ICON_ID2);
	TEST(setUpCall->iIconIdCallSetUpPhase.iQualifier==DSATTSY_ICON_QUALIFIER2);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpCallPCmd RSat::TSetUpCallV1 passed"),iTestCount++);

	// Set Up Call Cancel
	sat.NotifySetUpCallPCmd(reqStatus,*setUpCallPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpCallPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpCallPCmdCancel RSat::TSetUpCallV1 passed"), iTestCount++);

	// Set Up Call
	RSat::TSetUpCallV5* setUpCallV5 = new (ELeave) RSat::TSetUpCallV5;
	CleanupStack::PushL(setUpCallV5);
	RSat::TSetUpCallV5Pckg* setUpCallV5Pckg = new (ELeave) RSat::TSetUpCallV5Pckg(*setUpCallV5);
	CleanupStack::PushL(setUpCallV5Pckg);

	TEST(setUpCallV5->ExtensionId() == RSat::KSatV5);
	TEST(setUpCallV5->iTextAttributeConfirmationPhase.iStatus == RSat::ETextAttributeNotSet);
	TEST(setUpCallV5->iTextAttributeCallSetUpPhase.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifySetUpCallPCmd(reqStatus,*setUpCallV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpCallV5->PCmdNumber()==DSATTSY_SET_UP_CALL_NUMBER);

	TEST(setUpCallV5->iType==DSATTSY_SET_UP_CALL_TYPE);
	TEST(setUpCallV5->iAlphaIdConfirmationPhase.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(setUpCallV5->iAlphaIdConfirmationPhase.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(setUpCallV5->iIconIdConfirmationPhase.iIdentifier==DSATTSY_ICON_ID1);
	TEST(setUpCallV5->iIconIdConfirmationPhase.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(setUpCallV5->iAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(setUpCallV5->iAddress.iNumberPlan==DSATTSY_NPI);
	TEST(setUpCallV5->iAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(setUpCallV5->iCapabilityConfigParams==DSATTSY_CCP1);
	TEST(setUpCallV5->iSubAddress==DSATTSY_SUBADDRESS);
	TEST(setUpCallV5->iDuration.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(setUpCallV5->iDuration.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(setUpCallV5->iAlphaIdCallSetUpPhase.iStatus==DSATTSY_ALPHA_ID2_STATUS);
	TEST(setUpCallV5->iAlphaIdCallSetUpPhase.iAlphaId==DSATTSY_ALPHA_ID2);
	TEST(setUpCallV5->iIconIdCallSetUpPhase.iIdentifier==DSATTSY_ICON_ID2);
	TEST(setUpCallV5->iIconIdCallSetUpPhase.iQualifier==DSATTSY_ICON_QUALIFIER2);
	// Version 5 parameters
	TEST(setUpCallV5->iTextAttributeConfirmationPhase.iStatus == textAttribute1.iStatus);
	TEST(setUpCallV5->iTextAttributeConfirmationPhase.iTextAttributeData == textAttribute1.iTextAttributeData);
	TEST(setUpCallV5->iTextAttributeCallSetUpPhase.iStatus == textAttribute1.iStatus);
	TEST(setUpCallV5->iTextAttributeCallSetUpPhase.iTextAttributeData == textAttribute1.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpCallPCmd with RSat::TSetUpCallV5 passed"),iTestCount++);

	// Set Up Call Cancel
	sat.NotifySetUpCallPCmd(reqStatus,*setUpCallV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpCallPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpCallPCmdCancel with RSat::TSetUpCallV5 passed"), iTestCount++);

	// Set Up Call Terminal Rsp
	sat.NotifySetUpCallPCmd(reqStatus,*setUpCallPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TSetUpCallRspV2* setUpCallRspV2 = new (ELeave) RSat::TSetUpCallRspV2;
	CleanupStack::PushL(setUpCallRspV2);
	RSat::TSetUpCallRspV2Pckg* setUpCallRspV2Pckg = new (ELeave) RSat::TSetUpCallRspV2Pckg(*setUpCallRspV2);
	CleanupStack::PushL(setUpCallRspV2Pckg);

	setUpCallRspV2->SetPCmdNumber(DSATTSY_SET_UP_CALL_NUMBER);
	setUpCallRspV2->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	setUpCallRspV2->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	setUpCallRspV2->iAdditionalInfo=DSATTSY_NULL_BUF;
	setUpCallRspV2->iCcRequestedAction=*ccRequestedAction;

	setUpCallRspV2->iGeneralResult2=DSATTSY_PCMD_RESULT_SUCCESS;
	setUpCallRspV2->iInfoType2=DSATTSY_PCMD_RSP_NO_INFO;
	setUpCallRspV2->iAdditionalInfo2=DSATTSY_NULL_BUF;

	setUpCallRspV2->iUssdString.iUssdString=DSATTSY_DEFAULT_TEXT;
	setUpCallRspV2->iUssdString.iDcs=DSATTSY_DCS;

	sat.TerminalRsp(reqStatus,RSat::ESetUpCall,*setUpCallRspV2Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Set Up Call passed"),iTestCount++);

	CleanupStack::PopAndDestroy(17, sendSs);

	// Refresh
	RSat::TRefreshV2* refresh = new (ELeave) RSat::TRefreshV2;
	CleanupStack::PushL(refresh);
	RSat::TRefreshV2Pckg* refreshPckg = new (ELeave) RSat::TRefreshV2Pckg(*refresh);
	CleanupStack::PushL(refreshPckg);

	sat.NotifyRefreshPCmd(reqStatus,*refreshPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(refresh->PCmdNumber()==DSATTSY_REFRESH_NUMBER);

	TEST(refresh->iType==DSATTSY_REFRESH_TYPE);
	TEST(refresh->iFileList.Length()==2);
 	TEST(refresh->iFileList[0]==DSATTSY_REFRESH_ADN_EF);
 	TEST(refresh->iFileList[1]==DSATTSY_REFRESH_FDN_EF);
	TEST(refresh->iAid==DSATTSY_AID);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyRefreshPCmd passed"),iTestCount++);

	// Refresh Cancel
	sat.NotifyRefreshPCmd(reqStatus,*refreshPckg);
	sat.CancelAsyncRequest(ESatNotifyRefreshPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyRefreshPCmdCancel passed"),iTestCount++);

	// Refresh Required
	sat.NotifyRefreshRequired(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyRefreshRequired passed"),iTestCount++);

	// Refresh Required Cancel
	sat.NotifyRefreshRequired(reqStatus);
	sat.CancelAsyncRequest(ESatNotifyRefreshRequired);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyRefreshRequiredCancel passed"),iTestCount++);

	// Refresh Required
	sat.NotifyRefreshRequired(reqStatus,*refreshPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(refresh->iType==DSATTSY_REFRESH_TYPE);
	TEST(refresh->iFileList.Length()==2);
 	TEST(refresh->iFileList[0]==DSATTSY_REFRESH_FDN_EF);
 	TEST(refresh->iFileList[1]==DSATTSY_REFRESH_ADN_EF);
	TEST(refresh->iAid==DSATTSY_AID);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyRefreshRequiredParam passed"),iTestCount++);

	sat.NotifyRefreshRequired(reqStatus,*refreshPckg);
	sat.CancelAsyncRequest(ESatNotifyRefreshRequiredParam);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyRefreshRequiredParamCancel passed"),iTestCount++);

	// Refresh Terminal Response
	sat.NotifyRefreshPCmd(reqStatus,*refreshPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TRefreshRspV1* refreshRsp = new (ELeave) RSat::TRefreshRspV1;
	CleanupStack::PushL(refreshRsp);
	RSat::TRefreshRspV1Pckg* refreshRspPckg = new (ELeave) RSat::TRefreshRspV1Pckg(*refreshRsp);
	CleanupStack::PushL(refreshRspPckg);

	refreshRsp->SetPCmdNumber(refresh->PCmdNumber());
	refreshRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	refreshRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	refreshRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ERefresh,*refreshRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Refresh - passed"),iTestCount++);

	// Refresh Required Response
	RSat::TRefreshRspV1* refreshAllowedRsp = new (ELeave) RSat::TRefreshRspV1;
	CleanupStack::PushL(refreshAllowedRsp);
	RSat::TRefreshRspV1Pckg* refreshAllowedRspPckg = new (ELeave) RSat::TRefreshRspV1Pckg(*refreshAllowedRsp);
	CleanupStack::PushL(refreshAllowedRspPckg);

	refreshAllowedRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	refreshAllowedRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	refreshAllowedRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.RefreshAllowed(reqStatus, *refreshAllowedRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::RefreshAllowed - passed"),iTestCount++);

	// Refresh Required Response Cancel
	sat.RefreshAllowed(reqStatus, *refreshAllowedRspPckg);
	sat.CancelAsyncRequest(ESatRefreshAllowed);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::RefreshAllowedCancel passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, refresh);

	// Set Up Event List
	RSat::TSetUpEventListV1* setUpEventList = new (ELeave) RSat::TSetUpEventListV1;
	CleanupStack::PushL(setUpEventList);
	RSat::TSetUpEventListV1Pckg* setUpEventListPckg = new (ELeave) RSat::TSetUpEventListV1Pckg(*setUpEventList);
	CleanupStack::PushL(setUpEventListPckg);

	sat.NotifySetUpEventListPCmd(reqStatus,*setUpEventListPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpEventList->PCmdNumber()==DSATTSY_SET_UP_EVENT_LIST_NUMBER);

	TEST(setUpEventList->iType==DSATTSY_SET_UP_EVENT_LIST_TYPE);
	TEST(setUpEventList->iEvents==DSATTSY_EVENTS);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpEventList passed"),iTestCount++);

	// Set Up Event List Cancel
	sat.NotifySetUpEventListPCmd(reqStatus,*setUpEventListPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpEventListPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpEventListPCmdCancel passed"),iTestCount++);

	// Set Up Event List Terminal Response
	sat.NotifySetUpEventListPCmd(reqStatus,*setUpEventListPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TSetUpEventListRspV1* setUpEventListRsp = new (ELeave) RSat::TSetUpEventListRspV1;
	CleanupStack::PushL(setUpEventListRsp);
	RSat::TSetUpEventListRspV1Pckg* setUpEventListRspPckg = new (ELeave) RSat::TSetUpEventListRspV1Pckg(*setUpEventListRsp);
	CleanupStack::PushL(setUpEventListRspPckg);

	setUpEventListRsp->SetPCmdNumber(setUpEventList->PCmdNumber());
	setUpEventListRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	setUpEventListRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	setUpEventListRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ESetUpEventList,*setUpEventListRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Set Up Event List - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(4, setUpEventList);

	// Set Up Idle Mode Text with V1
	RSat::TSetUpIdleModeTextV1* setUpIdleModeText = new (ELeave) RSat::TSetUpIdleModeTextV1;
	CleanupStack::PushL(setUpIdleModeText);
	RSat::TSetUpIdleModeTextV1Pckg* setUpIdleModeTextPckg = new (ELeave) RSat::TSetUpIdleModeTextV1Pckg(*setUpIdleModeText);
	CleanupStack::PushL(setUpIdleModeTextPckg);

	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpIdleModeText->PCmdNumber()==DSATTSY_SET_UP_IDLE_MODE_TEXT_NUMBER);

   	TEST(setUpIdleModeText->iType==DSATTSY_IDLE_MODE_TXT_TYPE);
 	TEST(setUpIdleModeText->iText==DSATTSY_IDLE_MODE_TEXT);
   	TEST(setUpIdleModeText->iIconId.iIdentifier==DSATTSY_ICON_ID1);
   	TEST(setUpIdleModeText->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
   	TEST(setUpIdleModeText->iCodingScheme==DSATTSY_TEXT_ENCODING_SCHEME);
 	TPtrC idleModeText(DSATTSY_IDLE_MODE_TEXT);
 	TEST(setUpIdleModeText->iText.Length() == idleModeText.Length()); // 276
 	TEST(setUpIdleModeText->iText.Size() == idleModeText.Size()); // 552
 	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeText (Unicode) with RSat::TSetUpIdleModeTextV1 passed"),iTestCount++);

 	// Set Up Idle Mode Text Cancel with V1
 	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextPckg);
 	sat.CancelAsyncRequest(ESatNotifySetUpIdleModeTextPCmd);

 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus.Int()==KErrCancel);
 	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeTextPCmdCancel with RSat::TSetUpIdleModeTextV1passed"),iTestCount++);

 	// Issue another request. The DTSY will set the idle text mode to an 8-bit string
 	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextPckg);
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus.Int()==KErrNone);

 	TEST(setUpIdleModeText->PCmdNumber()==DSATTSY_SET_UP_IDLE_MODE_TEXT_NUMBER);

 	TPtrC8 textIdleMode8(DSATTSY_IDLE_MODE_TEXT_8BIT);
   	TEST(setUpIdleModeText->iType==DSATTSY_IDLE_MODE_TXT_TYPE);
   	TBuf8<276> text8;
   	CnvUtfConverter::ConvertFromUnicodeToUtf8(text8,setUpIdleModeText->iText);
 	TEST(text8==DSATTSY_IDLE_MODE_TEXT_8BIT);
   	TEST(setUpIdleModeText->iIconId.iIdentifier==DSATTSY_ICON_ID1);
   	TEST(setUpIdleModeText->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
   	TEST(setUpIdleModeText->iCodingScheme==DSATTSY_TEXT_ENCODING_SCHEME_8);

 	TEST(setUpIdleModeText->iText.Length() == textIdleMode8.Length()); // 276
 	TEST(setUpIdleModeText->iText.Size() == 2*textIdleMode8.Size()); // 2*276

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeText (8-bit) with RSat::TSetUpIdleModeTextV1 passed"),iTestCount++);

	// Set Up Idle Mode Text Cancel with V1
	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpIdleModeTextPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeTextPCmdCancel passed"),iTestCount++);

	// Set Up Idle Mode Text with V5
	RSat::TSetUpIdleModeTextV5* setUpIdleModeTextV5 = new (ELeave) RSat::TSetUpIdleModeTextV5;
	CleanupStack::PushL(setUpIdleModeTextV5);
	RSat::TSetUpIdleModeTextV5Pckg* setUpIdleModeTextV5Pckg = new (ELeave) RSat::TSetUpIdleModeTextV5Pckg(*setUpIdleModeTextV5);
	CleanupStack::PushL(setUpIdleModeTextV5Pckg);

	TEST(setUpIdleModeTextV5->ExtensionId() == RSat::KSatV5);
	TEST(setUpIdleModeTextV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpIdleModeTextV5->PCmdNumber()==DSATTSY_SET_UP_IDLE_MODE_TEXT_NUMBER);

   	TEST(setUpIdleModeTextV5->iType==DSATTSY_IDLE_MODE_TXT_TYPE);
 	TEST(setUpIdleModeTextV5->iText==DSATTSY_IDLE_MODE_TEXT);
   	TEST(setUpIdleModeTextV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
   	TEST(setUpIdleModeTextV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
   	TEST(setUpIdleModeTextV5->iCodingScheme==DSATTSY_TEXT_ENCODING_SCHEME);

 	TEST(setUpIdleModeTextV5->iText.Length() == idleModeText.Length()); // 276
 	TEST(setUpIdleModeTextV5->iText.Size() == idleModeText.Size()); // 552

	// V5 parameters
	TEST(setUpIdleModeTextV5->iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(setUpIdleModeTextV5->iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);

 	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeText (Unicode) with RSat::TSetUpIdleModeTextV5 passed"),iTestCount++);

 	// Set Up Idle Mode Text Cancel
 	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextV5Pckg);
 	sat.CancelAsyncRequest(ESatNotifySetUpIdleModeTextPCmd);

 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus.Int()==KErrCancel);
 	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeTextPCmdCancel with RSat::TSetUpIdleModeTextV5 passed"),iTestCount++);

 	// Issue another request. The DTSY will set the idle text mode to an 8-bit string
 	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextV5Pckg);
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus.Int()==KErrNone);

 	TEST(setUpIdleModeTextV5->PCmdNumber()==DSATTSY_SET_UP_IDLE_MODE_TEXT_NUMBER);

 	TPtrC8 textIdleMode8_V5(DSATTSY_IDLE_MODE_TEXT_8BIT);
   	TEST(setUpIdleModeTextV5->iType==DSATTSY_IDLE_MODE_TXT_TYPE);
   	TBuf8<276> text8_V5;
   	CnvUtfConverter::ConvertFromUnicodeToUtf8(text8_V5,setUpIdleModeText->iText);
 	TEST(text8==DSATTSY_IDLE_MODE_TEXT_8BIT);
   	TEST(setUpIdleModeTextV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
   	TEST(setUpIdleModeTextV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
   	TEST(setUpIdleModeTextV5->iCodingScheme==DSATTSY_TEXT_ENCODING_SCHEME_8);

 	TEST(setUpIdleModeTextV5->iText.Length() == textIdleMode8_V5.Length()); // 276
 	TEST(setUpIdleModeTextV5->iText.Size() == 2*textIdleMode8_V5.Size()); // 2*276

	TEST(setUpIdleModeTextV5->iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(setUpIdleModeTextV5->iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeText (8-bit) with RSat::TSetUpIdleModeTextV5 passed"),iTestCount++);

	// Set Up Idle Mode Text Cancel
	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpIdleModeTextPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeTextPCmdCancel with RSat::TSetUpIdleModeTextV5 passed"),iTestCount++);

	// Set Up Idle Mode Text Terminal Rsp
	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TSetUpIdleModeTextRspV1* setUpIdleModeTextRsp = new (ELeave) RSat::TSetUpIdleModeTextRspV1;
	CleanupStack::PushL(setUpIdleModeTextRsp);
	RSat::TSetUpIdleModeTextRspV1Pckg* setUpIdleModeTextRspPckg = new (ELeave) RSat::TSetUpIdleModeTextRspV1Pckg(*setUpIdleModeTextRsp);
	CleanupStack::PushL(setUpIdleModeTextRspPckg);

	setUpIdleModeTextRsp->SetPCmdNumber(setUpIdleModeText->PCmdNumber());
	setUpIdleModeTextRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	setUpIdleModeTextRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	setUpIdleModeTextRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::ESetUpIdleModeText,*setUpIdleModeTextRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Set Up Idle Mode Text - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, setUpIdleModeText);

	sat.Close();
	phone.Close();

	return TestStepResult();
	}
