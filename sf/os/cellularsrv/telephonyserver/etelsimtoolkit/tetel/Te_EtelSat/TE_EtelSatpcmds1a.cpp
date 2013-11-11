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
#include "TE_EtelSatpcmds1a.h"

CTestSatPCmds1a::CTestSatPCmds1a()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSatPCmds1a"));
	}

enum TVerdict CTestSatPCmds1a::doTestStepL()
	{
	INFO_PRINTF1(_L("*************************************"));
	INFO_PRINTF1(_L("RSat Proactive Commands Functionality"));
	INFO_PRINTF1(_L("*************************************"));

	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	ret=sat.Open(phone);
    TEST(ret==KErrNone);

	// local variables used throughout the sat tests
	iTestCount=1;

	// Display Text with RSat::TDisplayTextV2
	RSat::TDisplayTextV2* displayText = new (ELeave) RSat::TDisplayTextV2;
	CleanupStack::PushL(displayText);

	RSat::TDisplayTextV2Pckg* displayTextPckg = new (ELeave) RSat::TDisplayTextV2Pckg(*displayText);
	CleanupStack::PushL(displayTextPckg);

	sat.NotifyDisplayTextPCmd(reqStatus,*displayTextPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(displayText->PCmdNumber()==DSATTSY_DISPLAY_TEXT_NUMBER);
	TEST(displayText->iPriority==DSATTSY_TEXT_PRIORITY);
	TEST(displayText->iClearScreenTrigger==DSATTSY_CLEAR_SCREEN_TRIGGER);
	TEST(displayText->iText==DSATTSY_TEXT_TO_DISPLAY);
	TEST(displayText->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(displayText->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(displayText->iImmediateRsp==DSATTSY_IMMEDIATE_RSP);
	TEST(displayText->iDuration.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(displayText->iDuration.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyDisplayTextPCmd with RSat::TDisplayTextV2 passed"),iTestCount++);

	//Display Text Cancel with RSat::TDisplayTextV2
	sat.NotifyDisplayTextPCmd(reqStatus,*displayTextPckg);
	sat.CancelAsyncRequest(ESatNotifyDisplayTextPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyDisplayTextPCmdCancel with RSat::TDisplayTextV2 passed"),iTestCount++);

	// Display Text with RSat::TDisplayTextV5
	RSat::TDisplayTextV5* displayTextV5 = new (ELeave) RSat::TDisplayTextV5;
	CleanupStack::PushL(displayTextV5);
	RSat::TDisplayTextV5Pckg* displayTextV5Pckg = new (ELeave) RSat::TDisplayTextV5Pckg(*displayTextV5);
	CleanupStack::PushL(displayTextV5Pckg);

	TEST(displayTextV5->ExtensionId() == RSat::KSatV5);
	TEST(displayTextV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifyDisplayTextPCmd(reqStatus,*displayTextV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(displayTextV5->PCmdNumber()==DSATTSY_DISPLAY_TEXT_NUMBER);
	TEST(displayTextV5->iPriority==DSATTSY_TEXT_PRIORITY);
	TEST(displayTextV5->iClearScreenTrigger==DSATTSY_CLEAR_SCREEN_TRIGGER);
	TEST(displayTextV5->iText==DSATTSY_TEXT_TO_DISPLAY);
	TEST(displayTextV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(displayTextV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(displayTextV5->iImmediateRsp==DSATTSY_IMMEDIATE_RSP);
	TEST(displayTextV5->iDuration.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(displayTextV5->iDuration.iNumOfUnits==DSATTSY_NUM_OF_UNITS);

	// V5 parameters
	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

	TEST(displayTextV5->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(displayTextV5->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyDisplayTextPCmd with RSat::TDisplayTextV5 passed"),iTestCount++);

	//Display Text Cancel with RSat::TDisplayTextV5
	sat.NotifyDisplayTextPCmd(reqStatus,*displayTextV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyDisplayTextPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyDisplayTextPCmdCancel with RSat::TDisplayTextV5 passed"),iTestCount++);

	// Display Text Terminal Response
	sat.NotifyDisplayTextPCmd(reqStatus, *displayTextPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TDisplayTextRspV1* displayTextRsp = new (ELeave) RSat::TDisplayTextRspV1;
	CleanupStack::PushL(displayTextRsp);

	RSat::TDisplayTextRspV1Pckg* displayTextRspPckg = new (ELeave) RSat::TDisplayTextRspV1Pckg(*displayTextRsp);
	CleanupStack::PushL(displayTextRspPckg);

	displayTextRsp->SetPCmdNumber(displayText->PCmdNumber());
	displayTextRsp->iGeneralResult = DSATTSY_PCMD_RESULT_SUCCESS;
	displayTextRsp->iInfoType = DSATTSY_PCMD_RSP_NO_INFO;
	displayTextRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus, RSat::EDisplayText, *displayTextRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp passed"),iTestCount++);

	// Display Text Terminal Rsp Cancel
	// This will be the only Terminal Rsp cancel
	// no need to test it for each PCmd
	sat.TerminalRsp(reqStatus, RSat::EDisplayText, *displayTextRspPckg);
	sat.CancelAsyncRequest(ESatTerminalRsp);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRspCancel passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, displayText);

	// GetInkey with V2
	RSat::TGetInkeyV2* getInkey = new (ELeave) RSat::TGetInkeyV2;
	CleanupStack::PushL(getInkey);
	RSat::TGetInkeyV2Pckg* getInkeyPckg = new (ELeave) RSat::TGetInkeyV2Pckg(*getInkey);
	CleanupStack::PushL(getInkeyPckg);

	sat.NotifyGetInkeyPCmd(reqStatus,*getInkeyPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(getInkey->PCmdNumber()==DSATTSY_GET_INKEY_NUMBER);
	TEST(getInkey->iRspFormat==RSat::EDigitOnly);
	TEST(getInkey->iText==DSATTSY_TEXT_TO_DISPLAY);
	TEST(getInkey->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(getInkey->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(getInkey->iHelp==RSat::ENoHelpAvailable);
	TEST(getInkey->iDuration.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(getInkey->iDuration.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(getInkey->iMode==DSATTSY_GET_INKEY_RSP_MODE);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetInkeyPCmd with RSat::TGetInkeyV2 passed"),iTestCount++);

	// Get Inkey Cancel with V2
	sat.NotifyGetInkeyPCmd(reqStatus,*getInkeyPckg);
	sat.CancelAsyncRequest(ESatNotifyGetInkeyPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::GetInkeyCancel with RSat::TGetInkeyV2 passed"),iTestCount++);

	// GetInkey with V5
	RSat::TGetInkeyV5* getInkeyV5 = new (ELeave) RSat::TGetInkeyV5;
	CleanupStack::PushL(getInkeyV5);
	RSat::TGetInkeyV5Pckg* getInkeyV5Pckg = new (ELeave) RSat::TGetInkeyV5Pckg(*getInkeyV5);
	CleanupStack::PushL(getInkeyV5Pckg);

	TEST(getInkeyV5->ExtensionId() == RSat::KSatV5);
	TEST(getInkeyV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifyGetInkeyPCmd(reqStatus,*getInkeyV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(getInkeyV5->PCmdNumber()==DSATTSY_GET_INKEY_NUMBER);
	TEST(getInkeyV5->iRspFormat==RSat::EDigitOnly);
	TEST(getInkeyV5->iText==DSATTSY_TEXT_TO_DISPLAY);
	TEST(getInkeyV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(getInkeyV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(getInkeyV5->iHelp==RSat::ENoHelpAvailable);
	TEST(getInkeyV5->iDuration.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(getInkeyV5->iDuration.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(getInkeyV5->iMode==DSATTSY_GET_INKEY_RSP_MODE);
	// Version 5 parameters
	TEST(getInkeyV5->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(getInkeyV5->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetInkeyPCmd with RSat::TGetInkeyV5 passed"),iTestCount++);

	// Get Inkey Cancel with V5
	sat.NotifyGetInkeyPCmd(reqStatus,*getInkeyV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyGetInkeyPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::GetInkeyCancel with RSat::TGetInkeyV5 passed"),iTestCount++);

	// GetInkey Terminal Response
	sat.NotifyGetInkeyPCmd(reqStatus,*getInkeyPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TGetInkeyRspV2* getInkeyRsp = new (ELeave) RSat::TGetInkeyRspV2;
	CleanupStack::PushL(getInkeyRsp);
	RSat::TGetInkeyRspV2Pckg* getInkeyRspPckg = new (ELeave) RSat::TGetInkeyRspV2Pckg(*getInkeyRsp);
	CleanupStack::PushL(getInkeyRspPckg);

	getInkeyRsp->SetPCmdNumber(getInkey->PCmdNumber());
	getInkeyRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	getInkeyRsp->iInfoType=DSATTSY_PCMD_RSP_TEXT_STRING;
	getInkeyRsp->iRspFormat=getInkey->iRspFormat;
	getInkeyRsp->iAdditionalInfo=DSATTSY_USER_TEXT_INPUT;
	getInkeyRsp->iDuration.iTimeUnit=DSATTSY_TIME_UNIT;
	getInkeyRsp->iDuration.iNumOfUnits=DSATTSY_NUM_OF_UNITS;

	sat.TerminalRsp(reqStatus, RSat::EGetInkey,*getInkeyRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::Get Inkey Terminal Rsp passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, getInkey);

	// further tests
	ExtendedTest1L(reqStatus);

	sat.Close();
	phone.Close();

	return TestStepResult();
	}

	// Further tests, seperated to prevent stack overflow
void CTestSatPCmds1a::ExtendedTest1L(TRequestStatus& reqStatus)
	{
	// Get Input with V1
	RSat::TGetInputV1* getInput = new (ELeave) RSat::TGetInputV1;
	CleanupStack::PushL(getInput);
	RSat::TGetInputV1Pckg* getInputPckg = new (ELeave) RSat::TGetInputV1Pckg(*getInput);
	CleanupStack::PushL(getInputPckg);

	sat.NotifyGetInputPCmd(reqStatus,*getInputPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(getInput->PCmdNumber()==DSATTSY_GET_INPUT_NUMBER);
	TEST(getInput->iInputDisplayOption==DSATTSY_INPUT_DISPLAY_OPTION);
	TEST(getInput->iRspFormat==DSATTSY_GET_INPUT_RSP_FORMAT);
	TEST(getInput->iText==DSATTSY_TEXT_TO_DISPLAY);
	TEST(getInput->iRspLength.iMinRspLength==DSATTSY_MIN_RSP_LENGTH);
	TEST(getInput->iRspLength.iMaxRspLength==DSATTSY_MAX_RSP_LENGTH);
	TEST(getInput->iDefaultText==DSATTSY_DEFAULT_TEXT);
	TEST(getInput->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(getInput->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(getInput->iHelp==DSATTSY_HELP);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetInputPCmd with RSat::TGetInputV1 passed"),iTestCount++);

	// Get Input Cancel
	sat.NotifyGetInputPCmd(reqStatus,*getInputPckg);
	sat.CancelAsyncRequest(ESatNotifyGetInputPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetInputPCmd with RSat::TGetInputV1 passed"),iTestCount++);

	// Get Input with V5
	RSat::TGetInputV5* getInputV5 = new (ELeave) RSat::TGetInputV5;
	CleanupStack::PushL(getInputV5);
	RSat::TGetInputV5Pckg* getInputV5Pckg = new (ELeave) RSat::TGetInputV5Pckg(*getInputV5);
	CleanupStack::PushL(getInputV5Pckg);

	TEST(getInputV5->ExtensionId() == RSat::KSatV5);
	TEST(getInputV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifyGetInputPCmd(reqStatus,*getInputV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(getInputV5->PCmdNumber()==DSATTSY_GET_INPUT_NUMBER);
	TEST(getInputV5->iInputDisplayOption==DSATTSY_INPUT_DISPLAY_OPTION);
	TEST(getInputV5->iRspFormat==DSATTSY_GET_INPUT_RSP_FORMAT);
	TEST(getInputV5->iText==DSATTSY_TEXT_TO_DISPLAY);
	TEST(getInputV5->iRspLength.iMinRspLength==DSATTSY_MIN_RSP_LENGTH);
	TEST(getInputV5->iRspLength.iMaxRspLength==DSATTSY_MAX_RSP_LENGTH);
	TEST(getInputV5->iDefaultText==DSATTSY_DEFAULT_TEXT);
	TEST(getInputV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(getInputV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(getInputV5->iHelp==DSATTSY_HELP);
	// Version 5 parameters
	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
	TEST(getInputV5->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(getInputV5->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyGetInputPCmd with RSat::TGetInputV5 passed"),iTestCount++);

	// Get Input Cancel
	sat.NotifyGetInputPCmd(reqStatus,*getInputV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyGetInputPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::GetInputCancel with RSat::TGetInputV5 passed"),iTestCount++);

	// Get Input Terminal Rsp
	sat.NotifyGetInputPCmd(reqStatus,*getInputPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TGetInputRspV1* getInputRsp = new (ELeave) RSat::TGetInputRspV1;
	CleanupStack::PushL(getInputRsp);
	RSat::TGetInputRspV1Pckg* getInputRspPckg = new (ELeave) RSat::TGetInputRspV1Pckg(*getInputRsp);
	CleanupStack::PushL(getInputRspPckg);

	getInputRsp->SetPCmdNumber(getInput->PCmdNumber());
	getInputRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	getInputRsp->iInfoType=DSATTSY_PCMD_RSP_TEXT_STRING;
	getInputRsp->iRspFormat=getInput->iRspFormat;
	getInputRsp->iAdditionalInfo=DSATTSY_USER_TEXT_INPUT;

	sat.TerminalRsp(reqStatus,RSat::EGetInput,*getInputRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Get Input - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, getInput);

	// Play Tone with V2
	RSat::TPlayToneV2* playTone = new (ELeave) RSat::TPlayToneV2;
	CleanupStack::PushL(playTone);
	RSat::TPlayToneV2Pckg* playTonePckg = new (ELeave) RSat::TPlayToneV2Pckg(*playTone);
	CleanupStack::PushL(playTonePckg);

	sat.NotifyPlayTonePCmd(reqStatus,*playTonePckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(playTone->PCmdNumber()==DSATTSY_PLAY_TONE_NUMBER);
	TEST(playTone->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(playTone->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(playTone->iTone==DSATTSY_TONE);
	TEST(playTone->iDuration.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(playTone->iDuration.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(playTone->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(playTone->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyPlayTonePCmd with RSat::TPlayToneV2 passed"),iTestCount++);

	// Play Tone Cancel with V2
	sat.NotifyPlayTonePCmd(reqStatus,*playTonePckg);
	sat.CancelAsyncRequest(ESatNotifyPlayTonePCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyPlayTonePCmdCancel with RSat::TPlayToneV2 passed"),iTestCount++);

	// Play Tone with V5
	RSat::TPlayToneV5* playToneV5 = new (ELeave) RSat::TPlayToneV5;
	CleanupStack::PushL(playToneV5);
	RSat::TPlayToneV5Pckg* playToneV5Pckg = new (ELeave) RSat::TPlayToneV5Pckg(*playToneV5);
	CleanupStack::PushL(playToneV5Pckg);

	TEST(playToneV5->ExtensionId() == RSat::KSatV5);
	TEST(playToneV5->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifyPlayTonePCmd(reqStatus,*playToneV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(playToneV5->PCmdNumber()==DSATTSY_PLAY_TONE_NUMBER);
	TEST(playToneV5->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(playToneV5->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(playToneV5->iTone==DSATTSY_TONE);
	TEST(playToneV5->iDuration.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(playToneV5->iDuration.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(playToneV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(playToneV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	// Version 5 parameter
	TEST(playToneV5->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(playToneV5->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifyPlayTonePCmd with RSat::TPlayToneV5 passed"),iTestCount++);

	// Play Tone Cancel with V5
	sat.NotifyPlayTonePCmd(reqStatus,*playToneV5Pckg);
	sat.CancelAsyncRequest(ESatNotifyPlayTonePCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyPlayTonePCmdCancel with RSat::TPlayToneV5 passed"),iTestCount++);

	// Play Tone Terminal Rsp
	sat.NotifyPlayTonePCmd(reqStatus,*playTonePckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	RSat::TPlayToneRspV1* playToneRsp = new (ELeave) RSat::TPlayToneRspV1;
	CleanupStack::PushL(playToneRsp);
	RSat::TPlayToneRspV1Pckg* playToneRspPckg = new (ELeave) RSat::TPlayToneRspV1Pckg(*playToneRsp);
	CleanupStack::PushL(playToneRspPckg);

	playToneRsp->SetPCmdNumber(playTone->PCmdNumber());
	playToneRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	playToneRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	playToneRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus,RSat::EPlayTone,*playToneRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Get Inkey - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, playTone);

	ExtendedTest2L(reqStatus);
}

void CTestSatPCmds1a::ExtendedTest2L(TRequestStatus& reqStatus)
	{
	// Set Up Menu
	RSat::TSetUpMenuV2* setUpMenu = new (ELeave) RSat::TSetUpMenuV2;
	CleanupStack::PushL(setUpMenu);
	RSat::TSetUpMenuV2Pckg* setUpMenuPckg = new RSat::TSetUpMenuV2Pckg(*setUpMenu);
	CleanupStack::PushL(setUpMenuPckg);

	// Set Up Menu containing only items

	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenu->PCmdNumber()==DSATTSY_SET_UP_MENU_NUMBER);
	TEST(!setUpMenu->IsRemoveMenu());
	TEST(setUpMenu->MenuContent()==DSATTSY_ITEMS);
	TEST(setUpMenu->NumberOfItems()==0x03);
	TEST(setUpMenu->iPreference==DSATTSY_SELECTION_PREFERENCE);

	index=1;
	TEST(setUpMenu->GetItem(index++,item,action)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(action==KErrNotFound);

	TEST(setUpMenu->GetItemIcon(index++,item,icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(icon==KErrNotFound);

	TEST(setUpMenu->GetItem(index,item)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);

	/* Test the case where the client use the wrong Get method
	This is OK - the icon and/or action not available is just
	returned as KErrNotFound*/

	icon=0x00;
	TEST(setUpMenu->GetItemIcon(index,item,icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==KErrNotFound);

	icon=0x00;
	action=0x00;
	TEST(setUpMenu->GetItem(index,item,action,icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==KErrNotFound);
	TEST(action==KErrNotFound);

	action=0x00;
	TEST(setUpMenu->GetItem(index,item,action)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(action==KErrNotFound);

	// Test error cases
	TUint action4=0x04;
	TUint icon4=0x04;

	TEST(setUpMenu->AddItem(item, action4)==KErrNotSupported);
	TEST(setUpMenu->AddItemIcon(item, icon4)==KErrNotSupported);
	TEST(setUpMenu->AddItem(item, action4,icon4)==KErrNotSupported);
	TEST(setUpMenu->GetItem(++index,item)==KErrNotFound);

	TEST(setUpMenu->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(setUpMenu->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(setUpMenu->iHelp==DSATTSY_HELP);
	TEST(setUpMenu->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(setUpMenu->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(setUpMenu->iIconListQualifier==DSATTSY_ICON_QUALIFIER2);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd - Items - passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel - Items - passed"),iTestCount++);

	// Set Up Menu - Item & Actions
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenu->MenuContent()==DSATTSY_ITEMS_AND_ACTIONS);
	TEST(setUpMenu->NumberOfItems()==0x03);

	index=1;
	TEST(setUpMenu->GetItem(index++,item, action, icon)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(action==0x01);
	TEST(icon==KErrNotFound);

	TEST(setUpMenu->GetItem(index++,item, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(action==0x02);

	icon=0x00;
	TEST(setUpMenu->GetItem(index++,item, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(action==0x03);
	TEST(icon==KErrNotFound);

	index=1;
	TEST(setUpMenu->GetItem(index++,item)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	TEST(setUpMenu->GetItemIcon(index++,item, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(icon==KErrNotFound);

	TEST(setUpMenu->GetItem(index++,item)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);

	// Test error cases
	TEST(setUpMenu->GetItem(index++,item, action)==KErrNotFound);
	TEST(setUpMenu->AddItem(item)==KErrNotSupported);
	TEST(setUpMenu->AddItem(item, action, icon)==KErrNotSupported);
	TEST(setUpMenu->AddItemIcon(item, icon)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd - Items and actions - passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel - Items and actions - passed"),iTestCount++);

	// Set Up Menu - Item & Icons
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenu->MenuContent()==DSATTSY_ITEMS_AND_ICONS);
	TEST(setUpMenu->NumberOfItems()==0x03);

	index=1;
	TEST(setUpMenu->GetItem(index++,item,action,icon)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(icon==0x01);
	TEST(action==KErrNotFound);

	TEST(setUpMenu->GetItemIcon(index++,item, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(icon==0x02);

	TEST(setUpMenu->GetItemIcon(index++,item, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==0x03);

	index=1;
	TEST(setUpMenu->GetItem(index++,item)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	TEST(setUpMenu->GetItem(index++,item, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(action==KErrNotFound);

	TEST(setUpMenu->GetItem(index++,item, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==0x03);
	TEST(action==KErrNotFound);

	// Test error cases
	TEST(setUpMenu->GetItem(index++,item)==KErrNotFound);
	TEST(setUpMenu->AddItem(item)==KErrNotSupported);
	TEST(setUpMenu->AddItem(item, action, icon)==KErrNotSupported);
	TEST(setUpMenu->AddItem(item, action)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd - Items and icons - passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel - Items and icons - passed"),iTestCount++);

	// Set Up Menu - Items & Icons & Actions
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenu->MenuContent()==DSATTSY_ITEMS_ACTIONS_AND_ICONS);
	TEST(setUpMenu->NumberOfItems()==0x03);

	index=1;
	TEST(setUpMenu->GetItem(index++,item,action,icon)==KErrNone);

	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(action==0x01);
	TEST(icon==0x01);

	TEST(setUpMenu->GetItem(index++,item,action,icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(action==0x02);
	TEST(icon==0x02);

	TEST(setUpMenu->GetItem(index++,item,action,icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(action==0x03);
	TEST(icon==0x03);

	TEST(setUpMenu->GetItem(index++,item, action, icon)==KErrNotFound);

	index=1;
	TEST(setUpMenu->GetItem(index++,item,action)==KErrNone);

	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(action==0x01);

	TEST(setUpMenu->GetItem(index++,item)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);

	TEST(setUpMenu->GetItemIcon(index++,item, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==0x03);

	// Test error cases
	TEST(setUpMenu->GetItem(index++,item)==KErrNotFound);
	TEST(setUpMenu->AddItem(item)==KErrNotSupported);
	TEST(setUpMenu->AddItemIcon(item, icon)==KErrNotSupported);
	TEST(setUpMenu->AddItem(item, action)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd - Items, actions and icons - passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel passed"),iTestCount++);

	//
	// TSetUpMenuV5 tests
	//

	/*  General format of the tests TMenuItemBaseV5::GetItem
		and TMenuItemBaseV5::AddItem tests:

		RSat::TSetUpMenuV5 added support for text attributes
		in menus.
		This added the following four new types of menu structure:

		1. Menu items with item text attributes only
		2. Menu items with item text attributes and item next
		   actions only
		3. Menu items with item text attributes and item icon IDs only
		4. Menu items with item text attributes and item next actions
		   and icon IDs.

		The following set of tests set up menus containing each
		of these structures and for each one:

		1. Uses the correct TMenuItemBaseV5::GetItem / GetItemIcon
		   method to retrieve all of the information
		   present in each of the menu items.
		2. Uses the other TMenuItemBaseV5::GetItem / GetItemIcon
		   methods to retrieve information about the menu items.
		3. Uses the base class TMenuItemBaseV1::GetItem / GetItemIcon
		   methods to retrieve information about the menu items.
		4. Tests error cases where the user tries to add new items
		   to the menu using the wrong TMenuItemBaseV5::AddItem
		   method.

		The tests then do steps 1, 2 and 4 immediately above with
		each of the existing menu structures:

		1. Menu items without item text attributes only
		2. Menu items without item text attributes but with item next
		   actions only
		3. Menu items without item text attributes but with item icon IDs only
		4. Menu items without item text attributes but with item next actions
		   and icon IDs.

		This ensures that the version 5 menus are backwards compatible with
		the version 1 and 2 menus.
		(step 3 has been tested in the TMenuItemBaseV1 test code).
	*/

	//
	// TSetUpMenuV5 - Set Up Menu - Items with text attributes
	//

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

	RSat::TSetUpMenuV5* setUpMenuV5 = new (ELeave) RSat::TSetUpMenuV5;
	CleanupStack::PushL(setUpMenuV5);
	RSat::TSetUpMenuV5Pckg* setUpMenuV5Pckg = new (ELeave) RSat::TSetUpMenuV5Pckg(*setUpMenuV5);
	CleanupStack::PushL(setUpMenuV5Pckg);

	// Test default parameters set up correctly
	// when the RSat::TSetUpMenuV5 object is created.
	TEST(setUpMenuV5->ExtensionId()==RSat::KSatV5);
	TEST(setUpMenuV5->iAlphaId.iStatus==RSat::EAlphaIdNotSet);
	TEST(setUpMenuV5->iHelp==RSat::EHelpNotSet);
	TEST(setUpMenuV5->iIconId.iQualifier==RSat::EIconQualifierNotSet);
	TEST(setUpMenuV5->iIconId.iIdentifier==0);
	TEST(setUpMenuV5->iIconListQualifier==RSat::EIconQualifierNotSet);
	TEST(setUpMenuV5->iPreference==RSat::ESelectionPreferenceNotSet);
	TEST(setUpMenuV5->iTextAttribute.iStatus==RSat::ETextAttributeNotSet);
	TEST(setUpMenuV5->MenuContent()==0);
	TEST(setUpMenuV5->IsRemoveMenu());

	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenuV5->PCmdNumber()==DSATTSY_SET_UP_MENU_NUMBER);
	TEST(!setUpMenuV5->IsRemoveMenu());
	TEST(setUpMenuV5->MenuContent()==DSATTSY_ITEMS_WITH_TEXT_ATTRIBUTES);
	TEST(setUpMenuV5->NumberOfItems()==0x04);
	TEST(setUpMenuV5->iPreference==DSATTSY_SELECTION_PREFERENCE);
	TEST(setUpMenuV5->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(setUpMenuV5->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(setUpMenuV5->iHelp==DSATTSY_HELP);
	TEST(setUpMenuV5->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(setUpMenuV5->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(setUpMenuV5->iIconListQualifier==DSATTSY_ICON_QUALIFIER2);
	TEST(setUpMenuV5->iTextAttribute.iStatus==textAttribute1.iStatus);
	TEST(setUpMenuV5->iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);

	// Test GetItem methods for getting the item and the text attribute
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == textAttribute2.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute2.iTextAttributeData);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == textAttribute3.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute3.iTextAttributeData);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == textAttribute4.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute4.iTextAttributeData);

	TEST(setUpMenuV5->GetItem(index++,item, iTextAttribute)==KErrNotFound);

	// Test other TMenuItemBaseV5::GetItem methods which get the item and the text attribute
	// The item and text attribute should be returned correctly.
	// Since there are no icons or actions in the menu, if the client uses one
	// of the GetItem methods that requests them, then KErrNotFound should be returned
	// via the parameter supplied by the client.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);
	TEST(action==KErrNotFound);

	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == textAttribute2.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute2.iTextAttributeData);
	TEST(icon==KErrNotFound);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == textAttribute3.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute3.iTextAttributeData);
	TEST(action==KErrNotFound);
	TEST(icon==KErrNotFound);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == textAttribute4.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute4.iTextAttributeData);
	TEST(action==KErrNotFound);

	TEST(setUpMenuV5->GetItem(index++,item, iTextAttribute)==KErrNotFound);

	TEST(setUpMenuV5->AddItem(item, textAttribute1)==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::GetItem and GetItemIcon with TSetUpMenuV5 containing items and text attributes passed"),iTestCount++);

	// Test TMenuItemBaseV1::GetItem methods which do not get the text attribute.
	// It should still be possible to get the item even when the menu item contains
	// text attribute information as well.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	TEST(setUpMenuV5->GetItem(index++, item, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(action==KErrNotFound);

	TEST(setUpMenuV5->GetItemIcon(index++, item, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==KErrNotFound);

	TEST(setUpMenuV5->GetItem(index++, item, action, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(action==KErrNotFound);
	TEST(icon==KErrNotFound);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::GetItem and GetItemIcon with TSetUpMenuV5 containing items and text attributes passed"),iTestCount++);

	// Test Error Cases - adding actions or icons when the menu can now
	// only contain items ALL with item text attributes.
	TUint action5=0x05;
	TUint icon5=0x05;
	// TMenuItemBaseV1::AddItem methods
	TEST(setUpMenuV5->AddItem(item)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::AddItem and AddItemIcon with TSetUpMenuV5 containing items and text attributes passed"),iTestCount++);

	// TMenuItemBaseV5::AddItem methods
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, textAttribute1, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem with TSetUpMenuV5 containing items and text attributes passed"),iTestCount++);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd with TSetUpMenuV5 containing items and text attributes passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel with TSetUpMenuV5 containing items and text attributes passed"),iTestCount++);

	//
	// TSetUpMenuV5 - Set Up Menu - Items with text attributes and actions
	//
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenuV5->PCmdNumber()==DSATTSY_SET_UP_MENU_NUMBER);
	TEST(!setUpMenuV5->IsRemoveMenu());
	TEST(setUpMenuV5->MenuContent()==DSATTSY_ITEMS_WITH_TEXT_ATTRIBUTES_AND_ACTIONS);
	TEST(setUpMenuV5->NumberOfItems()==0x04);

	// Test GetItem methods for getting the item, the text attribute and action
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);
	TEST(action==0x01);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == textAttribute2.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute2.iTextAttributeData);
	TEST(action==0x02);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == textAttribute3.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute3.iTextAttributeData);
	TEST(action==0x03);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == textAttribute4.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute4.iTextAttributeData);
	TEST(action==0x04);

	TEST(setUpMenuV5->GetItem(index,item, iTextAttribute, action)==KErrNotFound);

	// Test other TMenuItemBaseV5::GetItem methods which get the item, the text attribute
	// and the icon.
	// The item and text attribute and the action should be returned correctly.
	// Since there are no icons in the menu, if the client uses one
	// of the GetItem methods that requests them, then KErrNotFound should be returned
	// via the parameter supplied by the client.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == textAttribute2.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute2.iTextAttributeData);
	TEST(action==0x02);
	TEST(icon==KErrNotFound);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == textAttribute3.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute3.iTextAttributeData);

	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == textAttribute4.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute4.iTextAttributeData);
	TEST(icon==KErrNotFound);

	TEST(setUpMenuV5->AddItem(item, textAttribute1, action)==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::GetItem and GetItemIcon with TSetUpMenuV5 containing items, item text attributes and actions passed"),iTestCount++);

	// Test GetItem methods which do not get the text attribute
	// It should still be possible to get the item and action even when
	// the menu item contains
	// text attribute information as well.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	TEST(setUpMenuV5->GetItem(index++, item, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(action==0x02);

	TEST(setUpMenuV5->GetItemIcon(index++, item, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==KErrNotFound);

	TEST(setUpMenuV5->GetItem(index++, item, action, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(action==0x04);
	TEST(icon==KErrNotFound);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::GetItem and GetItemIcon with TSetUpMenuV5 containing items, item text attributes and actions passed"),iTestCount++);

	// Test Error Cases - adding icons when the menu can now only contain
	// items with item text attributes and actions.
	action5=0x00;
	icon5=0x00;

	TEST(setUpMenuV5->AddItem(item)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::AddItem with TSetUpMenuV5 containing items, item text attributes and actions passed"),iTestCount++);

	TEST(setUpMenuV5->AddItem(item, textAttribute1)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, textAttribute1, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem with TSetUpMenuV5 containing items, item text attributes and actions passed"),iTestCount++);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd with TSetUpMenuV5 containing items, item text attributes and actions passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel with TSetUpMenuV5 containing items, item text attributes and actions passed"),iTestCount++);


	// TSetUpMenuV5 - Set Up Menu - Items with text attributes and icons
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenuV5->PCmdNumber()==DSATTSY_SET_UP_MENU_NUMBER);
	TEST(!setUpMenuV5->IsRemoveMenu());
	TEST(setUpMenuV5->MenuContent()==DSATTSY_ITEMS_WITH_TEXT_ATTRIBUTES_AND_ICONS);
	TEST(setUpMenuV5->NumberOfItems()==0x04);

	// Get all the items, text attributes and icons in the menu
	index = 1;
	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);
	TEST(icon==0x01);

	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == textAttribute2.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute2.iTextAttributeData);
	TEST(icon==0x02);

	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == textAttribute3.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute3.iTextAttributeData);
	TEST(icon==0x03);

	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == textAttribute4.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute4.iTextAttributeData);
	TEST(icon==0x04);

	TEST(setUpMenuV5->GetItemIcon(index,item, iTextAttribute, action)==KErrNotFound);

	TEST(setUpMenuV5->AddItemIcon(item, textAttribute1, icon)==KErrNone);

	// Test other TMenuItemBaseV5::GetItem methods which get the
	// item and the text attribute.
	// The item and text attribute and icon (when requested)
	//  should be returned correctly.
	// Since there are no actions in the menu, if the client uses one
	// of the GetItem methods that requests one, then KErrNotFound should
	// be returned
	// via the parameter supplied by the client.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == textAttribute2.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute2.iTextAttributeData);
	TEST(action==KErrNotFound);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == textAttribute3.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute3.iTextAttributeData);
	TEST(action==KErrNotFound);
	TEST(icon==0x03);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == textAttribute4.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute4.iTextAttributeData);
	TEST(action==KErrNotFound);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::GetItem and GetItemIcon with TSetUpMenuV5 containing items, item text attributes and icons passed"),iTestCount++);

	// Test GetItem methods which do not get the text attribute
	// It should still be possible to get the item even when the menu item contains
	// text attribute information as well.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	TEST(setUpMenuV5->GetItem(index++, item, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(action==KErrNotFound);

	TEST(setUpMenuV5->GetItemIcon(index++, item, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==0x03);

	TEST(setUpMenuV5->GetItem(index++, item, action, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(action==KErrNotFound);
	TEST(icon==0x04);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::GetItem and GetItemIcon with TSetUpMenuV5 containing items, item text attributes and icons passed"),iTestCount++);

	// Test Error Cases - adding actions when the menu can now only contain
	// items ALL with item text attributes and icons.
	action5=0x00;
	icon5=0x00;

	TEST(setUpMenuV5->AddItem(item)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::AddItem and AddItemIcon with TSetUpMenuV5 containing items, item text attributes and icons passed"),iTestCount++);

	TEST(setUpMenuV5->AddItem(item, textAttribute1)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem with TSetUpMenuV5 containing items, item text attributes and icons passed"),iTestCount++);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd with TSetUpMenuV5 containing items, item text attributes and icons passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel with TSetUpMenuV5 containing items, item text attributes and icons passed"),iTestCount++);

	//
	// TSetUpMenuV5 - Set Up Menu - Items with text attributes, actions and icons
	//
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenuV5->PCmdNumber()==DSATTSY_SET_UP_MENU_NUMBER);
	TEST(!setUpMenuV5->IsRemoveMenu());
	TEST(setUpMenuV5->MenuContent()==DSATTSY_ITEMS_WITH_TEXT_ATTRIBUTES_ACTIONS_AND_ICONS);
	TEST(setUpMenuV5->NumberOfItems()==0x04);

	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);
	TEST(icon==0x01);
	TEST(action==0x01);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == textAttribute2.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute2.iTextAttributeData);
	TEST(icon==0x02);
	TEST(action==0x02);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == textAttribute3.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute3.iTextAttributeData);
	TEST(icon==0x03);
	TEST(action==0x03);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == textAttribute4.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute4.iTextAttributeData);
	TEST(icon==0x04);
	TEST(action==0x04);

	TEST(setUpMenuV5->GetItem(index++,item, iTextAttribute, action)==KErrNotFound);

	TEST(setUpMenuV5->AddItem(item, iTextAttribute, action, icon)==KErrNone);

	// Test other TMenuItemBaseV5::GetItem methods which get the item and the text attribute
	// The item and text attribute should be returned correctly.
	// Where requested, the icon and action should also be returned correctly.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == textAttribute1.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute1.iTextAttributeData);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == textAttribute2.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute2.iTextAttributeData);
	TEST(action==0x02);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == textAttribute3.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute3.iTextAttributeData);
	TEST(action==0x03);
	TEST(icon==0x03);

	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == textAttribute4.iStatus);
	TEST(iTextAttribute.iTextAttributeData == textAttribute4.iTextAttributeData);
	TEST(icon==0x04);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::GetItem and GetItemIcon with TSetUpMenuV5 containing items, item text attributes, actions and icons passed"),iTestCount++);

	// Test GetItem methods which do not get the text attribute
	// It should still be possible to get the item even when the menu item contains
	// text attribute, icon and action information as well.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	TEST(setUpMenuV5->GetItem(index++, item, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(action==0x02);

	TEST(setUpMenuV5->GetItemIcon(index++, item, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==0x03);

	TEST(setUpMenuV5->GetItem(index++, item, action, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(action==0x04);
	TEST(icon==0x04);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::GetItem and GetItemIcon with TSetUpMenuV5 containing items, item text attributes, actions and icons passed"),iTestCount++);

	// Test Error Cases
	action5=0x00;
	icon5=0x00;

	TEST(setUpMenuV5->AddItem(item)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::AddItem and AddItemIcon with TSetUpMenuV5 containing items, item text attributes, actions and icons passed"),iTestCount++);

	TEST(setUpMenuV5->AddItem(item, textAttribute1)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, textAttribute1, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem and AddItemIcon with TSetUpMenuV5 containing items, item text attributes, actions and icons passed"),iTestCount++);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd with TSetUpMenuV5 containing items, item text attributes, actions and icons passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel with TSetUpMenuV5 containing items, item text attributes, actions and icons passed"),iTestCount++);

	//
	// TSetUpMenuV5 - Set Up Menu - Items with NO text attributes, actions or icons (i.e. just plain items)
	//
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenuV5->PCmdNumber()==DSATTSY_SET_UP_MENU_NUMBER);
	TEST(!setUpMenuV5->IsRemoveMenu());
	TEST(setUpMenuV5->MenuContent()==DSATTSY_ITEMS);
	TEST(setUpMenuV5->NumberOfItems()==0x04);

	index = 1;
	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNotFound);

	TEST(setUpMenuV5->AddItem(item)==KErrNone);

	// Test other TMenuItemBaseV5::GetItem which get the item.
	// The item should be returned correctly.
	// If the client also requests to get the,
	// action or icon, KErrNotFound should be returned in
	// the relevant parameter.
	// Since the menu does not contain text attributes, the
	// TTextAttribute::iStatus will contain
	// RSat::ETextAttributeNotPresent after the request.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(action==KErrNotFound);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(action==KErrNotFound);
	TEST(icon==KErrNotFound);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(icon==KErrNotFound);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::GetItem and GetItemIcon with TSetUpMenuV5 containing only items with no text attributes passed"),iTestCount++);

	// Test Error Cases
	action5=0x00;
	icon5=0x00;
	TEST(setUpMenuV5->AddItem(item, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::AddItem and AddItemIcon with TSetUpMenuV5 containing only items with no text attributes passed"),iTestCount++);

	TEST(setUpMenuV5->AddItem(item, textAttribute1)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, textAttribute1, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem and AddItemIcon with TSetUpMenuV5 containing only items with no text attributes passed"),iTestCount++);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd with TSetUpMenuV5 containing only items with no text attributes passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel with TSetUpMenuV5 containing only items with no text attributes passed"),iTestCount++);

	//
	// TSetUpMenuV5 - Set Up Menu - Items and actions (no text attributes)
	//
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenuV5->PCmdNumber()==DSATTSY_SET_UP_MENU_NUMBER);
	TEST(!setUpMenuV5->IsRemoveMenu());
	TEST(setUpMenuV5->MenuContent()==DSATTSY_ITEMS_AND_ACTIONS);
	TEST(setUpMenuV5->NumberOfItems()==0x04);

	// Test GetItem methods for getting the item and action
	index = 1;
	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	TEST(setUpMenuV5->GetItem(index, item, iTextAttribute, action)==KErrNotFound);

	TEST(setUpMenuV5->AddItem(item, action)==KErrNone);

	// Test other TMenuItemBaseV5::GetItem methods which get the item, the text attribute
	// and the icon.
	// The item and text attribute and the action should be returned correctly.
	// Since there are no icons in the menu, if the client uses one
	// of the GetItem methods that requests them, then KErrNotFound should be returned
	// via the parameter supplied by the client.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(action==0x02);
	TEST(icon==KErrNotFound);

	TEST(setUpMenuV5->GetItem(index++, item, action)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(action==0x03);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(icon==KErrNotFound);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::GetItem and GetItemIcon with TSetUpMenuV5 containing items and actions only (no text attributes) passed"),iTestCount++);

	// Test Error Cases - adding icons when the menu can now only contain
	// items with item text attributes and actions.
	action5=0x00;
	icon5=0x00;

	TEST(setUpMenuV5->AddItem(item)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::AddItem and AddItemIcon with TSetUpMenuV5 containing items and actions only (no text attributes) passed"),iTestCount++);

	TEST(setUpMenuV5->AddItem(item, textAttribute1)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, textAttribute1, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem and AddItemIcon with TSetUpMenuV5 containing items and actions only (no text attributes) passed"),iTestCount++);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd with TSetUpMenuV5 containing items and actions only (no text attributes) passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel with TSetUpMenuV5 containing items and actions only (no text attributes) passed"),iTestCount++);

	//
	// TSetUpMenuV5 - Set Up Menu - Items with NO text attributes but with icons
	//
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenuV5->PCmdNumber()==DSATTSY_SET_UP_MENU_NUMBER);
	TEST(!setUpMenuV5->IsRemoveMenu());
	TEST(setUpMenuV5->MenuContent()==DSATTSY_ITEMS_AND_ICONS);
	TEST(setUpMenuV5->NumberOfItems()==0x04);

	// Get all the items and icons in the menu
	index = 1;
	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(icon==0x01);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(icon==0x02);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(icon==0x03);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(icon==0x04);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);

	TEST(setUpMenuV5->GetItemIcon(index, item, iTextAttribute, icon)==KErrNotFound);

	TEST(setUpMenuV5->AddItemIcon(item, icon)==KErrNone);

	// Test other TMenuItemBaseV5::GetItem methods which get the
	// item and the text attribute.
	// The item and text attribute and icon (when requested)
	//  should be returned correctly.
	// Since there are no actions in the menu, if the client uses one
	// of the GetItem methods that requests one, then KErrNotFound should
	// be returned
	// via the parameter supplied by the client.
	index = 1;

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(action==KErrNotFound);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(action==KErrNotFound);
	TEST(icon==0x03);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(action==KErrNotFound);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::GetItem and GetItemIcon with TSetUpMenuV5 containing items with no text attributes but with icons passed"),iTestCount++);

	// Test Error Cases - adding actions when the menu can now only contain
	// items ALL with item text attributes and icons.
	action5=0x00;
	icon5=0x00;

	TEST(setUpMenuV5->AddItem(item)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::AddItem with TSetUpMenuV5 containing items with no text attributes but with icons passed"),iTestCount++);

	TEST(setUpMenuV5->AddItem(item, textAttribute1)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, textAttribute1, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem and AddItemIcon with TSetUpMenuV5 containing items with no text attributes but with icons passed"),iTestCount++);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd with TSetUpMenuV5 containing items with no text attributes but with icons passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel with TSetUpMenuV5 containing items with no text attributes but with icons passed"),iTestCount++);

	//
	// TSetUpMenuV5 - Set Up Menu - Items with actions and icons but no text attributes
	//
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenuV5->PCmdNumber()==DSATTSY_SET_UP_MENU_NUMBER);
	TEST(!setUpMenuV5->IsRemoveMenu());
	TEST(setUpMenuV5->MenuContent()==DSATTSY_ITEMS_ACTIONS_AND_ICONS);
	TEST(setUpMenuV5->NumberOfItems()==0x04);

	index = 1;
	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(icon==0x01);
	TEST(action==0x01);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(icon==0x02);
	TEST(action==0x02);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(icon==0x03);
	TEST(action==0x03);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(icon==0x04);
	TEST(action==0x04);

	TEST(setUpMenuV5->GetItem(index++,item, iTextAttribute, action, icon)==KErrNotFound);

	TEST(setUpMenuV5->AddItem(item, action, icon)==KErrNone);

	// Test other TMenuItemBaseV5::GetItem methods which get the item and the text attribute
	// The item and text attribute should be returned correctly.
	// Where requested, the icon and action should also be returned correctly.
	index = 1;
	TEST(setUpMenuV5->GetItem(index++, item)==KErrNone);
	TEST(item.iItemId==0x01);
	TEST(item.iItemString==DSATTSY_ITEM1);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, iTextAttribute, action)==KErrNone);
	TEST(item.iItemId==0x02);
	TEST(item.iItemString==DSATTSY_ITEM2);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(action==0x02);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItem(index++, item, action, icon)==KErrNone);
	TEST(item.iItemId==0x03);
	TEST(item.iItemString==DSATTSY_ITEM3);
	TEST(action==0x03);
	TEST(icon==0x03);

	iTextAttribute.iStatus = RSat::ETextAttributeNotSet;
	TEST(setUpMenuV5->GetItemIcon(index++, item, iTextAttribute, icon)==KErrNone);
	TEST(item.iItemId==0x04);
	TEST(item.iItemString==DSATTSY_ITEM4);
	TEST(iTextAttribute.iStatus == RSat::ETextAttributeNotPresent);
	TEST(icon==0x04);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::GetItem and GetItemIcon with TSetUpMenuV5 containing items, actions and icons but no text attributes passed"),iTestCount++);

	// Test Error Cases
	action5=0x00;
	icon5=0x00;

	TEST(setUpMenuV5->AddItem(item)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::AddItem and AddItemIcon with TSetUpMenuV5 containing items, actions and icons but no text attributes passed"),iTestCount++);

	TEST(setUpMenuV5->AddItem(item, textAttribute1)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItemIcon(item, textAttribute1, icon5)==KErrNotSupported);
	TEST(setUpMenuV5->AddItem(item, textAttribute1, action5, icon5)==KErrNotSupported);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem and AddItemIcon with TSetUpMenuV5 containing items, actions and icons but no text attributes passed"),iTestCount++);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd with TSetUpMenuV5 containing items, actions and icons but no text attributes passed"),iTestCount++);

	// Set Up Menu Cancel
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuV5Pckg);
	sat.CancelAsyncRequest(ESatNotifySetUpMenuPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmdCancel with TSetUpMenuV5 containing items, actions and icons but no text attributes passed"),iTestCount++);

	// Set Up Menu - Remove Menu
	sat.NotifySetUpMenuPCmd(reqStatus,*setUpMenuPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpMenu->IsRemoveMenu());
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd - Remove menu - passed"),iTestCount++);

	// Set Up Menu Terminal Rsp
	RSat::TSetUpMenuRspV1* setUpMenuRsp = new (ELeave) RSat::TSetUpMenuRspV1;
	CleanupStack::PushL(setUpMenuRsp);
	RSat::TSetUpMenuRspV1Pckg* setUpMenuRspPckg = new (ELeave) RSat::TSetUpMenuRspV1Pckg(*setUpMenuRsp);
	CleanupStack::PushL(setUpMenuRspPckg);

	setUpMenuRsp->SetPCmdNumber(setUpMenu->PCmdNumber());
	setUpMenuRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	setUpMenuRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;
	setUpMenuRsp->iAdditionalInfo=DSATTSY_NULL_BUF;

	sat.TerminalRsp(reqStatus, RSat::ESetUpMenu, *setUpMenuRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpMenuPCmd - Set Up Menu - passed"),iTestCount++);

	CleanupStack::PopAndDestroy(6, setUpMenu);

	ExtendedTest3L();
	}

void CTestSatPCmds1a::ExtendedTest3L()
	{
	RSat::TMenuItemBaseV1* menu = new (ELeave) RSat::TMenuItemBaseV1;		// Load menu then reset
	CleanupStack::PushL(menu);

	TInt i;

	for (i=0; i<69; i++)
		{
		TEST(menu->AddItem(item)==KErrNone);
		}
	TEST(menu->AddItem(item)==KErrNoMemory);

	menu->Reset();
	TEST(menu->NumberOfItems() == 0);

	for (i=0; i<69; i++)			// Reload menu 1 for other tests...
		{
		TEST(menu->AddItem(item)==KErrNone);
		}
	TEST(menu->AddItem(item)==KErrNoMemory);

	RSat::TMenuItemBaseV1* menu2 = new (ELeave) RSat::TMenuItemBaseV1;
	CleanupStack::PushL(menu2);

	for (i=0; i<54 ; i++)
		{
		TEST(menu2->AddItem(item,action)==KErrNone);
		}
	TEST(menu2->AddItem(item,action)==KErrNoMemory);

	RSat::TMenuItemBaseV1* menu3 = new (ELeave) RSat::TMenuItemBaseV1;
	CleanupStack::PushL(menu3);

	for (i=0; i<54 ; i++)
		{
		TEST(menu3->AddItemIcon(item,icon)==KErrNone);
		}
	TEST(menu3->AddItemIcon(item,icon)==KErrNoMemory);

	RSat::TMenuItemBaseV1* menu4 = new (ELeave) RSat::TMenuItemBaseV1;
	CleanupStack::PushL(menu4);

	for (i=0; i<44 ; i++)
		{
		TEST(menu4->AddItem(item,action, icon)==KErrNone);
		}
	TEST(menu4->AddItem(item,action, icon)==KErrNoMemory);

	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV1::AddItem - passed"),iTestCount++);

	RSat::TMenuItemBaseV5* menu5 = new (ELeave) RSat::TMenuItemBaseV5;
	CleanupStack::PushL(menu5);

	for (i=0; i<44; i++)
		{
		TEST(menu5->AddItem(item, iTextAttribute)==KErrNone);
		}
	TEST(menu5->AddItem(item, iTextAttribute)==KErrNoMemory);
	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem - Items with text attributes - passed"),iTestCount++);

	menu5->Reset();
	TEST(menu5->NumberOfItems() == 0);

	for (i=0; i<37; i++)
		{
		TEST(menu5->AddItem(item, iTextAttribute, action)==KErrNone);
		}
	TEST(menu5->AddItem(item, iTextAttribute, action)==KErrNoMemory);
	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem - Items with text attributes and actions - passed"),iTestCount++);

	menu5->Reset();
	TEST(menu5->NumberOfItems() == 0);

	for (i=0; i<37; i++)
		{
		TEST(menu5->AddItemIcon(item, iTextAttribute, icon)==KErrNone);
		}
	TEST(menu5->AddItemIcon(item, iTextAttribute, icon)==KErrNoMemory);
	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem - Items with text attributes and icons - passed"),iTestCount++);

	menu5->Reset();
	TEST(menu5->NumberOfItems() == 0);

	for (i=0; i<32; i++)
		{
		TEST(menu5->AddItem(item, iTextAttribute, action, icon)==KErrNone);
		}
	TEST(menu5->AddItem(item, iTextAttribute, action, icon)==KErrNoMemory);
	INFO_PRINTF2(_L("Test %d - RSat::TMenuItemBaseV5::AddItem - Items with text attributes and actions and icons - passed"),iTestCount++);

	menu5->Reset();
	TEST(menu5->NumberOfItems() == 0);

	CleanupStack::PopAndDestroy(5, menu);
	}



