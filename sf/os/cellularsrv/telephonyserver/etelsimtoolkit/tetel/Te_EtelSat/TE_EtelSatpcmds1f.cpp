// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests added for new functionality in 9.4
// 
//

/**
 @file
*/

#include "Te_EtelSatTestStepBase.h"
#include "TE_EtelSatpcmds1f.h"
#include <utf.h>

CTestSatPCmds1f::CTestSatPCmds1f()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSatPCmds1f"));
	}
	
enum TVerdict CTestSatPCmds1f::doTestStepL()
	{
    
    INFO_PRINTF1(_L("***********************************************"));
	INFO_PRINTF1(_L("RSat Proactive Commands Functionality suite"));
	INFO_PRINTF1(_L("***********************************************"));
	
	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	ret=sat.Open(phone);
    TEST(ret==KErrNone);
    
	/**
    Testing Release 6 Features of Universal Subscriber Identity Module(USIM) 
    Application ToolKit
    
    Release6Test1() function call is made to test the newly added Retrieve(\Submit\Display) Multimedia 
    proactive commands.
    
    @test TSAT289
    @test TSAT290
    @test TSAT291
    @test TSAT292
	@test TSAT293
	@test TSAT294
    @test TSAT340
    @test TSAT341
    @test TSAT342
    */
   
	Release6Test1L();
	
	/**
	Release6Test2() function call is made to test the newly added proactive commands 
	SetFrames and GetFrameStatus.
	
	@test TSAT295
	@test TSAT296
	@test TSAT297
	@test TSAT298
	@test TSAT343
    @test TSAT344
	*/
	
	Release6Test2L();
	
	/**
	Release6Test3() function call is made to test the existing proactive commands with the new V6 class added.
	LocalInfo, GetMeSideSatProfile are the existing commands tested here.
	
	@test TSAT304
	@test TSAT307
 	@test TSAT306
 	@test TSAT308
	@test TSAT309
	@test TSAT366
    @test TSAT310
    @test TSAT367
	@test TSAT368
	*/
	
	Release6Test3L();
	
	/**
	Release6Test4() function call is made to test the existing proactive commands with the new V6 class added.
	Send Ussd, Play Tone , SetUpEventList, LaunchBrowser, DisplayText are the existing commands tested here.
	
	@test TSAT311
	@test TSAT371
    @test TSAT312
    @test TSAT372
	@test TSAT320
	@test TSAT338
    @test TSAT339
	@test TSAT322
    @test TSAT323
	@test TSAT330
	@test TSAT368
  	@test TSAT331
   	@test TSAT369
	@test TSAT332
    @test TSAT333
	*/
	
	Release6Test4L();
	
	/**
	Release6Test5() function call is made to test the existing proactive commands with the new V6 class added.
	GetInkey, GetInput, SelectItem, SendSm, SetUpCall, SetUpIdleModeText are the existing commands tested here.

	@test TSAT334
    @test TSAT335
	@test TSAT336
    @test TSAT337
	@test TSAT340
    @test TSAT341
	@test TSAT342
    @test TSAT343
	@test TSAT344
    @test TSAT345
	@test TSAT346
	@test TSAT347
	*/
	
	Release6Test5L();
	
	/**
	Release6Test6() function call is made to test the existing proactive commands with the new V6 class added.
	RunAtCommand, SendDtmf, CloseChannel, ReceiveData are the existing commands tested here.
	
	@test TSAT348
	@test TSAT349
	@test TSAT350	
	@test TSAT351
   	@test TSAT352
   	@test TSAT353
	@test TSAT354
	@test TSAT355
	*/
	
	Release6Test6L();
	
	/**
	Release6Test7() function call is made to test the existing proactive commands with the new V6 class added.
	SendData, GetServiceSearch , GetServiceInfo, SendSs, Open Channel are the existing commands tested here.
	
	@test TSAT356
    @test TSAT357
	@test TSAT358
	@test TSAT359
    @test TSAT360
    @test TSAT361
	@test TSAT369
	@test TSAT362
	@test TSAT363
	@test TSAT370
	@test TSAT375
	@test TSAT376
	@test TSAT377
	@test TSAT378
	*/
	
	Release6Test7L();
	
	sat.Close();
	phone.Close();
	return TestStepResult();
	}
	
void CTestSatPCmds1f::Release6Test1L()
	{
	/**
	Testing Retrieve Multimedia Message Proactive command which has been newly
	added as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT289
	*/	
	RSat::TRetrieveMultimediaMessageV6* retrieveMM = new (ELeave) RSat::TRetrieveMultimediaMessageV6;
	CleanupStack::PushL(retrieveMM);
	
	RSat::TRetrieveMultimediaMessageV6Pckg* retrieveMMPckg = new (ELeave) RSat::TRetrieveMultimediaMessageV6Pckg(*retrieveMM);
	CleanupStack::PushL(retrieveMMPckg);

	sat.NotifyRetrieveMultimediaMsgPCmd(reqStatus, *retrieveMMPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(retrieveMM->iDestination == DSATTSYV6_DEVICE_ID1);	
	TEST(retrieveMM->iAlphaId.iAlphaId== DSATTSYV6_ALPHAID1);
	TEST(retrieveMM->iAlphaId.iStatus == DSATTSYV6_ALPHAID_STATUS2);
	TEST(retrieveMM->iIconId.iIdentifier  == DSATTSYV6_ICON_ID1);	
	TEST(retrieveMM->iIconId.iQualifier  == DSATTSYV6_ICON_QUALIFIER3);	
	TEST(retrieveMM->iMultimediaContentId  == DSATTSYV6_MULTIMEDIA_CONTENT_ID1);
	TEST(retrieveMM->iMultimediaMessageRef  == DSATTSYV6_MULTIMEDIA_MESSAGE_REF1);
	TEST(retrieveMM->iMultimediaMessageId  == DSATTSYV6_MULTIMEDIA_MESSAGE_ID1);	
  	
  	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
  	
  	TEST(retrieveMM->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(retrieveMM->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
    		
    /**
    Testing Cancel API for the Retrieve Multimedia Message Proactive command
    @test TSAT290
    */
    
    //Cancel	
	sat.NotifyRetrieveMultimediaMsgPCmd(reqStatus, *retrieveMMPckg);
	sat.CancelAsyncRequest(ESatNotifyRetrieveMultimediaMsgPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
    
    /**
    Testing Terminal Response for the Retrieve Multimedia Message Proactive command
    @test TSAT340
    */
    
    //Response
   	RSat::TRetrieveMultimediaMessageRspV6* retrieveMMRsp = new (ELeave) RSat::TRetrieveMultimediaMessageRspV6;
	CleanupStack::PushL(retrieveMMRsp);
	
	RSat::TRetrieveMultimediaMessageRspV6Pckg* retrieveMMRspPckg = new (ELeave) RSat::TRetrieveMultimediaMessageRspV6Pckg(*retrieveMMRsp);
	CleanupStack::PushL(retrieveMMRspPckg);
		
	retrieveMMRsp->SetPCmdNumber(DSATTSY_RETRIEVE_MULTIMEDIA_MESSAGE);
	retrieveMMRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	retrieveMMRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;	
	
	sat.NotifyRetrieveMultimediaMsgPCmd(reqStatus, *retrieveMMPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	

	sat.TerminalRsp(reqStatus,RSat::ERetrieveMultimediaMsg,*retrieveMMRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Retrieve Multimedia Message - passed"),iTestCount++);	
    
    CleanupStack::PopAndDestroy(4);
    
    /**
	Testing Submit Multimedia Message Proactive command which has been newly
	added as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT291
	*/
	RSat::TSubmitMultimediaMessageV6* submitMM = new (ELeave) RSat::TSubmitMultimediaMessageV6;
	CleanupStack::PushL(submitMM);
	
	RSat::TSubmitMultimediaMessageV6Pckg* submitMMPckg = new (ELeave) RSat::TSubmitMultimediaMessageV6Pckg(*submitMM);
	CleanupStack::PushL(submitMMPckg);

	sat.NotifySubmitMultimediaMsgPCmd(reqStatus, *submitMMPckg);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(submitMM->iDestination == DSATTSYV6_DEVICE_ID1);	
	TEST(submitMM->iAlphaId.iAlphaId== DSATTSYV6_ALPHAID1);
	TEST(submitMM->iAlphaId.iStatus == DSATTSYV6_ALPHAID_STATUS2);
	TEST(submitMM->iIconId.iIdentifier  == DSATTSYV6_ICON_ID1);	
	TEST(submitMM->iIconId.iQualifier  == DSATTSYV6_ICON_QUALIFIER3);	
	TEST(submitMM->iMultimediaMessageId  == DSATTSYV6_MULTIMEDIA_MESSAGE_ID1);	
  	TEST(submitMM->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(submitMM->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);		
    TEST(submitMM->iSubmissionFile == DSATTSYV6_SUBMISSION_FILE1);
    
    /**
    Testing Cancel API for the Submit Multimedia Message Proactive command
    @test TSAT292
    */
    
    //Cancel	
	sat.NotifySubmitMultimediaMsgPCmd(reqStatus, *submitMMPckg);
	sat.CancelAsyncRequest(ESatNotifySubmitMultimediaMsgPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
       
    /**
    Testing Terminal Response for the Submit Multimedia Message Proactive command
    @test TSAT341
    */
    
    //Response
   	RSat::TSubmitMultimediaMessageRspV6* submitMMRsp = new (ELeave) RSat::TSubmitMultimediaMessageRspV6;
	CleanupStack::PushL(submitMMRsp);
	
	RSat::TSubmitMultimediaMessageRspV6Pckg* submitMMRspPckg = new (ELeave) RSat::TSubmitMultimediaMessageRspV6Pckg(*submitMMRsp);
	CleanupStack::PushL(submitMMRspPckg);
		
	submitMMRsp->SetPCmdNumber(DSATTSY_SUBMIT_MULTIMEDIA_MESSAGE);
	submitMMRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	submitMMRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;	
	
	sat.NotifySubmitMultimediaMsgPCmd(reqStatus, *submitMMPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	

	sat.TerminalRsp(reqStatus,RSat::ESubmitMultimediaMsg,*submitMMRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Submit Multimedia Message - passed"),iTestCount++);
	
    CleanupStack::PopAndDestroy(4);
    
	/**
	Testing Display Multimedia Message Proactive command which has been newly
	added as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT293
	*/
	RSat::TDisplayMultimediaMessageV6* displayMM = new (ELeave) RSat::TDisplayMultimediaMessageV6;
	CleanupStack::PushL(displayMM);
	
	RSat::TDisplayMultimediaMessageV6Pckg* displayMMPckg = new (ELeave) RSat::TDisplayMultimediaMessageV6Pckg(*displayMM);
	CleanupStack::PushL(displayMMPckg);

	sat.NotifyDisplayMultimediaMsgPCmd(reqStatus, *displayMMPckg);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(displayMM->iDestination == DSATTSYV6_DEVICE_ID3);
	TEST(displayMM->iDisplayPriority == DSATTSYV6_DISPLAY_PRIORITY1);
	TEST(displayMM->iSubmissionFile == DSATTSYV6_SUBMISSION_FILE1);
	TEST(displayMM->iMultimediaMessageId == DSATTSYV6_MULTIMEDIA_MESSAGE_ID1);
	TEST(displayMM->iImmediateRsp == DSATTSYV6_IMMEDIATE_RESPONSE1);
    
    /**
    Testing Cancel API for the Display Multimedia Message Proactive command
    @test TSAT294
    */
    
    //Cancel	
	sat.NotifyDisplayMultimediaMsgPCmd(reqStatus, *displayMMPckg);
	sat.CancelAsyncRequest(ESatNotifyDisplayMultimediaMsgPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
    
    /**
    Testing Terminal Response for the Display Multimedia Message Proactive command
    @test TSAT342
    */
    
    //Response
   	RSat::TDisplayMultimediaMessageRspV6* displayMMRsp = new (ELeave) RSat::TDisplayMultimediaMessageRspV6;
	CleanupStack::PushL(displayMMRsp);
	
	RSat::TDisplayMultimediaMessageRspV6Pckg* displayMMRspPckg = new (ELeave) RSat::TDisplayMultimediaMessageRspV6Pckg(*displayMMRsp);
	CleanupStack::PushL(displayMMRspPckg);
	
	displayMMRsp->SetPCmdNumber(DSATTSY_DISPLAY_MULTIMEDIA_MESSAGE);
	displayMMRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	displayMMRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;	
	
	sat.NotifyDisplayMultimediaMsgPCmd(reqStatus, *displayMMPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	

	sat.TerminalRsp(reqStatus,RSat::EDisplayMultimediaMsg,*displayMMRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Display Multimedia Message - passed"),iTestCount++);
	
	CleanupStack::PopAndDestroy(4);
	}
	
void CTestSatPCmds1f::Release6Test2L()
	{
	/**
	Testing Set Frames Proactive command which has been newly
	added as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT295
	*/
	RSat::TSetFramesV6* setFrames = new (ELeave) RSat::TSetFramesV6;
	CleanupStack::PushL(setFrames);
	
	RSat::TSetFramesV6Pckg* setFramesPckg = new (ELeave) RSat::TSetFramesV6Pckg(*setFrames);
	CleanupStack::PushL(setFramesPckg);

	sat.NotifySetFramesPCmd(reqStatus, *setFramesPckg);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(setFrames->iDestination == DSATTSYV6_DEVICE_ID1);
    TEST(setFrames->iDefaultFrameId == DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
    TEST(setFrames->iFrameId == DSATTSYV6_FRAME_IDENTIFIER1);
    TEST(setFrames->iFrameLayout.iFramesLayout == DSATTSYV6_FRAME_LAYOUT_FORMAT1);
    TEST(setFrames->iFrameLayout.iFrameLayoutBuf == DSATTSYV6_FRAME_LAYOUT1);
    
    /**
    Testing Cancel API for the Set Frames Proactive command
    @test TSAT296
    */
    
    //Cancel	
	sat.NotifySetFramesPCmd(reqStatus, *setFramesPckg);
	sat.CancelAsyncRequest(ESatNotifySetFramesPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
    
    /**
    Testing Terminal Response for the Set Frames Proactive command
    @test TSAT343
    */
    
    //Response
   	RSat::TSetFramesRspV6* setFramesRsp = new (ELeave) RSat::TSetFramesRspV6;
	CleanupStack::PushL(setFramesRsp);
	
	RSat::TSetFramesRspV6Pckg* setFramesRspPckg = new (ELeave) RSat::TSetFramesRspV6Pckg(*setFramesRsp);
	CleanupStack::PushL(setFramesRspPckg);
		
	setFramesRsp->SetPCmdNumber(DSATTSY_SET_FRAMES);
	setFramesRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	setFramesRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;	
	setFramesRsp->iFramesInformation.iFrameId=DSATTSY_FRAMES_INFORMATION_FRAMEID;
	setFramesRsp->iFramesInformation.iFrameList=DSATTSY_FRAMES_INFORMATION_FRAME_LIST;
	
	sat.NotifySetFramesPCmd(reqStatus, *setFramesPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	

	sat.TerminalRsp(reqStatus,RSat::ESetFrames,*setFramesRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Set Frames - passed"),iTestCount++);
	
	CleanupStack::PopAndDestroy(4);
		
	/**
	Testing Get Frames Status Proactive command which has been newly
	added as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT297
	*/
	RSat::TGetFramesStatusV6* getFramesStatus = new (ELeave) RSat::TGetFramesStatusV6;
	CleanupStack::PushL(getFramesStatus);
	
	RSat::TGetFramesStatusV6Pckg* getFramesStatusPckg = new (ELeave) RSat::TGetFramesStatusV6Pckg(*getFramesStatus);
	CleanupStack::PushL(getFramesStatusPckg);

	sat.NotifyGetFramesStatusPCmd(reqStatus, *getFramesStatusPckg);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(getFramesStatus->iDestination == DSATTSYV6_DEVICE_ID1);
    
    /**
    Testing Cancel API for the Get Frames Status Proactive command
    @test TSAT298
    */
    
    //Cancel	
	sat.NotifyGetFramesStatusPCmd(reqStatus, *getFramesStatusPckg);
	sat.CancelAsyncRequest(ESatNotifyGetFramesStatusPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
    
    /**
    Testing Terminal Response for the Get Frames Status Proactive command
    @test TSAT344
    */
    
    //Response
   	RSat::TGetFramesStatusRspV6* getFramesStatusRsp = new (ELeave) RSat::TGetFramesStatusRspV6;
	CleanupStack::PushL(getFramesStatusRsp);
	
	RSat::TGetFramesStatusRspV6Pckg* getFramesStatusRspPckg = new (ELeave) RSat::TGetFramesStatusRspV6Pckg(*getFramesStatusRsp);
	CleanupStack::PushL(getFramesStatusRspPckg);
	
	getFramesStatusRsp->SetPCmdNumber(DSATTSY_GET_FRAMES_STATUS);
	getFramesStatusRsp->iGeneralResult=DSATTSY_PCMD_RESULT_SUCCESS;
	getFramesStatusRsp->iInfoType=DSATTSY_PCMD_RSP_NO_INFO;	
	getFramesStatusRsp->iFramesInformation.iFrameId=DSATTSY_FRAMES_INFORMATION_FRAMEID;
	getFramesStatusRsp->iFramesInformation.iFrameList=DSATTSY_FRAMES_INFORMATION_FRAME_LIST;
	
	sat.NotifyGetFramesStatusPCmd(reqStatus, *getFramesStatusPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);	

	sat.TerminalRsp(reqStatus,RSat::EGetFramesStatus,*getFramesStatusRspPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::TerminalRsp - Get Frames Status - passed"),iTestCount++);
	
	CleanupStack::PopAndDestroy(4);
	
	}
	
void CTestSatPCmds1f::Release6Test3L()
	{	
	/**
	Testing the LocalInfo proactive command which has been updated 
	to support the Release 6 Features of USIM ToolKit
	@test TSAT304
	@test TSAT307
	*/
	RSat::TLocalInfoV6* localInfoV6 = new (ELeave) RSat::TLocalInfoV6;
	CleanupStack::PushL(localInfoV6);
	
	RSat::TLocalInfoV6Pckg* localInfoV6Pckg = new (ELeave) RSat::TLocalInfoV6Pckg(*localInfoV6);
	CleanupStack::PushL(localInfoV6Pckg);


	sat.NotifyLocalInfoPCmd(reqStatus, *localInfoV6Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(localInfoV6->iUtranMeasurementQualifier=DSATTSYV6_LOCAL_INFOV6); 
	TEST(localInfoV6->iInfoType = RSat::KProvideLocalInfoSearchMode); 
	TEST(localInfoV6->iDevideId = RSat::KKeypad);
    INFO_PRINTF2(_L("Test %d - NotifyLocalInfoPCmd -LocalInfo proactive command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the LocalInfo proactive command
    @test TSAT306
    @test TSAT308
    */
    
    //Cancel	
	sat.NotifyLocalInfoPCmd(reqStatus, *localInfoV6Pckg);
	sat.CancelAsyncRequest(ESatNotifyLocalInfoPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
		
	/**
	Testing the API GetMeSideSatProfile  which has been updated 
	to support the Release 6 Features of USIM ToolKit
	@test TSAT309
	@test TSAT366
	*/
	RSat::TSatProfileV6* profileV6 = new (ELeave) RSat::TSatProfileV6;
	CleanupStack::PushL(profileV6);
	
	RSat::TSatProfileV6Pckg* profileV6Pckg = new (ELeave) RSat::TSatProfileV6Pckg(*profileV6);
	CleanupStack::PushL(profileV6Pckg);
	
	sat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

		// Pre-version 5 parameters in SAT profile
	TEST(profileV6->iSatProfileByte1==0x01);
	TEST(profileV6->iSatProfileByte2==0x02);
	TEST(profileV6->iSatProfileByte3==0x03);
	TEST(profileV6->iSatProfileByte4==0x04);
	TEST(profileV6->iSatProfileByte5==0x05);
	TEST(profileV6->iSatProfileByte6==0x06);
	TEST(profileV6->iSatProfileByte7==0x07);
	TEST(profileV6->iSatProfileByte8==0x08);
	TEST(profileV6->iSatProfileByte9==0x09);
	TEST(profileV6->iSatProfileByte10==0x0A);
	TEST(profileV6->iSatProfileByte11==0x0B);
	TEST(profileV6->iSatProfileByte12==0x0C);
	TEST(profileV6->iSatProfileByte13==0x0D);
	TEST(profileV6->iSatProfileByte14==0x0E);
	TEST(profileV6->iSatProfileByte15==0x0F);
	TEST(profileV6->iSatProfileByte16==0x10);
	TEST(profileV6->iSatProfileByte17==0x20);
	TEST(profileV6->iSatProfileByte18==0x30);
	TEST(profileV6->iNumOfChannels==DSATTSY_NUMBER_OF_CHANNELS);
	TEST(profileV6->iScreenHeight==DSATTSY_SCREEN_HEIGHT);
	TEST(profileV6->iScreenWidth==DSATTSY_SCREEN_WIDTH);
	TEST(profileV6->iTiaEiaProtoVersion==DSATTSY_PROTO_VERSION);
	TEST(profileV6->iWidthReduction==DSATTSY_WIDTH_REDUCTION);
	// Version 5 parameters in profile
	// (test also that the TSatProfileByte28 and TSatProfileByte29
	// have been defined correctly as each should set a different bit).
	TEST(profileV6->iSatProfileByte28==
			(RSat::KCapsTextAttributeAlignmentLeft | RSat::KCapsTextAttributeAlignmentCentre |
			 RSat::KCapsTextAttributeAlignmentRight | RSat::KCapsTextAttributeFontSizeNormal |
			 RSat::KCapsTextAttributeFontSizeLarge | RSat::KCapsTextAttributeFontSizeSmall)); // 0x3F
	TEST(profileV6->iSatProfileByte29==(RSat::KCapsTextAttributeStyleNormal |
									 RSat::KCapsTextAttributeStyleBold |
									 RSat::KCapsTextAttributeStyleItalic |
									 RSat::KCapsTextAttributeStyleUnderline |
									 RSat::KCapsTextAttributeStyleStrikethrough |
									 RSat::KCapsTextAttributeStyleForeground |
									 RSat::KCapsTextAttributeStyleBackground)); // 0x7F
	TEST(profileV6->iSatProfileByte21==
			(RSat::KCapsWML | RSat::KCapsXHTML |
			 RSat::KCapsHTML | RSat::KCapsCHTML)); // 0x0F
	TEST(profileV6->iSatProfileByte22==(RSat::KCapsProvideLocalInfoUTRAN | RSat::KCapsProvideLocalInfoBatteryState |
			 RSat::KCapsPlayThemedAndMelodyTone | RSat::KCapsSetUpCallMultimediaCall |
			 RSat::KCapsRefreshGBA	| RSat::KCapsRetrieveMultimediaMessage |
			 RSat::KCapsSubmitMultimediaMessage | RSat::KCapsDisplayMultimediaMesage)); // 0xFF
	TEST(profileV6->iSatProfileByte23==
			(RSat::KCapsSetFrames  | RSat::KCapsGetFramesStatus  |
			 RSat::KCapsMmsNotificationDownload | RSat::KCapsProvideLocalInfoMEID |
			 RSat::KCapsProvideLocalInfoNmrUTRAN | RSat::KCapsUssdDataDownload));  //0xE7
	TEST(profileV6->iSatProfileByte24==(RSat::KCapsMaxNumFramesBit1 | RSat::KCapsMaxNumFramesBit2 |
		 RSat::KCapsMaxNumFramesBit3 | RSat::KCapsMaxNumFramesBit4)); // 0x0F								 
	TEST(profileV6->iSatProfileByte25==(RSat::KCapsBrowsingStatusEvent | RSat::KCapsMmsTransferStatusEvent |
	   	RSat::KCapsFrameParametersChangeEvent)); // 0x07
	INFO_PRINTF2(_L("Test %d - RSat::GetMeSideSatProfile with RSat::TSatProfileV6 passed"),iTestCount++);
    
    /**
    Testing Cancel API for the GetMeSideProfile  command
    @test TSAT310
    @test TSAT367
    */
    
    //Cancel	
	sat.GetMeSideSatProfile(reqStatus, *profileV6Pckg);
	sat.CancelAsyncRequest(ESatGetMeSideSatProfile);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);

	/**
	Testing the API GetMeSideSatProfile  which has been updated 
	to support the Release 6 Features of USIM ToolKit
	@test TSAT368
	*/

	TEST(sat.ClientSatProfileIndication(*profileV6Pckg)==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::ClientSatProfileIndication RSat::TSatProfileV6 passed"),iTestCount++);
	CleanupStack::PopAndDestroy(2);
	
	}
	
void CTestSatPCmds1f::Release6Test4L()
	{
	/**
	Testing Send Ussd Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT311
	@test TSAT371
	*/
	RSat::TSendUssdV6* sendUssd = new (ELeave) RSat::TSendUssdV6;
	CleanupStack::PushL(sendUssd);
	
	RSat::TSendUssdV6Pckg* sendUssdPckg = new (ELeave) RSat::TSendUssdV6Pckg(*sendUssd);
	CleanupStack::PushL(sendUssdPckg);
	
	sat.NotifySendUssdPCmd(reqStatus, *sendUssdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(sendUssd->iAlphaId.iAlphaId		== DSATTSY_ALPHA_ID1);
	TEST(sendUssd->iAlphaId.iStatus		== DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendUssd->iUssdString.iDcs		== DSATTSY_DCS);
	TEST(sendUssd->iUssdString.iUssdString == DSATTSY_USSD_STRING);
	TEST(sendUssd->iIconId.iIdentifier		== DSATTSY_ICON_ID1);
	TEST(sendUssd->iIconId.iQualifier		== DSATTSY_ICON_QUALIFIER1);

  	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
		
	TEST(sendUssd->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(sendUssd->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	
    INFO_PRINTF2(_L("Test %d - UssdDataDownload - Ussd Data Download command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the Send Ussd Proactive command
    @test TSAT312
    @test TSAT372
    */
    
    //Cancel	
	sat.NotifySendUssdPCmd(reqStatus, *sendUssdPckg);
	sat.CancelAsyncRequest(ESatNotifySendUssdPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
		
	/**
	Testing Play Tone Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT320
	@test TSAT338
	*/
	RSat::TPlayToneV6* playTone = new (ELeave) RSat::TPlayToneV6;
	CleanupStack::PushL(playTone);
	
	RSat::TPlayToneV6Pckg* playTonePckg = new (ELeave) RSat::TPlayToneV6Pckg(*playTone);
	CleanupStack::PushL(playTonePckg);
		
	sat.NotifyPlayTonePCmd(reqStatus, *playTonePckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(playTone->iAlphaId.iAlphaId		== DSATTSY_ALPHA_ID1);
	TEST(playTone->iAlphaId.iStatus		== DSATTSY_ALPHA_ID1_STATUS);
	TEST(playTone->iTone		== DSATTSY_TONEV6);
	TEST(playTone->iDuration.iTimeUnit == DSATTSY_TIME_UNIT);
	TEST(playTone->iDuration.iNumOfUnits	==DSATTSY_NUM_OF_UNITS);
	TEST(playTone->iIconId.iIdentifier		== DSATTSY_ICON_ID1);
	TEST(playTone->iIconId.iQualifier		== DSATTSY_ICON_QUALIFIER1);
	TEST(playTone->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(playTone->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(playTone->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - NotifyPlayTone - Play Tone command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the Play Tone command
    @test TSAT321
    @test TSAT339
    */
    
    //Cancel	
	sat.NotifyPlayTonePCmd(reqStatus, *playTonePckg);
	sat.CancelAsyncRequest(ESatNotifyPlayTonePCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
		
	/**
	Testing SetUpEventList Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT322
	*/
	RSat::TSetUpEventListV1* setUpEventList = new (ELeave) RSat::TSetUpEventListV1;
	CleanupStack::PushL(setUpEventList);
	
	RSat::TSetUpEventListV1Pckg* setUpEventListPckg = new (ELeave) RSat::TSetUpEventListV1Pckg(*setUpEventList);
	CleanupStack::PushL(setUpEventListPckg);

	sat.NotifySetUpEventListPCmd(reqStatus, *setUpEventListPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
    INFO_PRINTF2(_L("Test %d - SetUpEventList - SetUp EventList command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the SetUpEventList Proactive command
    @test TSAT323
    */
    
    //Cancel	
	sat.NotifySetUpEventListPCmd(reqStatus, *setUpEventListPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpEventListPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
			
	/**
	Testing LaunchBrowser Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT330
	@test TSAT368
	*/
	RSat::TLaunchBrowserV6* launchBrowser = new (ELeave) RSat::TLaunchBrowserV6;
	CleanupStack::PushL(launchBrowser);
	
	RSat::TLaunchBrowserV6Pckg* launchBrowserPckg = new (ELeave) RSat::TLaunchBrowserV6Pckg(*launchBrowser);
	CleanupStack::PushL(launchBrowserPckg);
	
	sat.NotifyLaunchBrowserPCmd(reqStatus, *launchBrowserPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(launchBrowser->iBrowserSel		== DSATTSY_BROWSER_SELECTION_MODE);
	TEST(launchBrowser->iBrowserId		== DSATTSY_BROWSER_ID);
	TEST(launchBrowser->iUrl				== DSATTSY_URL);
	TEST(launchBrowser->iBearerList		== DSATTSY_BEARER_LIST);
	TEST(launchBrowser->iText			== DSATTSY_PROXY_NAME);
	TEST(launchBrowser->iAlphaId.iStatus	== DSATTSY_ALPHA_ID1_STATUS);
	TEST(launchBrowser->iAlphaId.iAlphaId	== DSATTSY_ALPHA_ID1);
	TEST(launchBrowser->iIconId.iIdentifier	== DSATTSY_ICON_ID1);
	TEST(launchBrowser->iIconId.iQualifier  == DSATTSY_ICON_QUALIFIER1);
	TEST(launchBrowser->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(launchBrowser->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(launchBrowser->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - NotifyLaunchBrowserPCmd - Launch Browser command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the LauchBrowser Proactive command
    @test TSAT331
   	@test TSAT369
    */
    
    //Cancel	
	sat.NotifyLaunchBrowserPCmd(reqStatus, *launchBrowserPckg);
	sat.CancelAsyncRequest(ESatNotifyLaunchBrowserPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
	
	/**
	Testing DisplayText Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT332
	*/
	RSat::TDisplayTextV6* displayText = new (ELeave) RSat::TDisplayTextV6;
	CleanupStack::PushL(displayText);
	
	RSat::TDisplayTextV6Pckg* displayTextPckg = new (ELeave) RSat::TDisplayTextV6Pckg(*displayText);
	CleanupStack::PushL(displayTextPckg);
	

	sat.NotifyDisplayTextPCmd(reqStatus, *displayTextPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(displayText->iPriority				==DSATTSY_TEXT_PRIORITY);
	TEST(displayText->iClearScreenTrigger	==DSATTSY_CLEAR_SCREEN_TRIGGER);
	TEST(displayText->iText					==DSATTSY_TEXT_TO_DISPLAY);
	TEST(displayText->iIconId.iIdentifier	==DSATTSY_ICON_ID1);
	TEST(displayText->iIconId.iQualifier	==DSATTSY_ICON_QUALIFIER1);
	TEST(displayText->iImmediateRsp			==DSATTSY_IMMEDIATE_RSP);
	TEST(displayText->iDuration.iTimeUnit ==DSATTSY_TIME_UNIT);
	TEST(displayText->iDuration.iNumOfUnits ==DSATTSY_NUM_OF_UNITS);
    TEST(displayText->iTextAttribute.iStatus == textAttribute.iStatus);
    TEST(displayText->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(displayText->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - NotifyDisplayTextPCmd - DisplayText command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the DisplayText Proactive command
    @test TSAT333
    */
    
    //Cancel	
	sat.NotifyDisplayTextPCmd(reqStatus, *displayTextPckg);
	sat.CancelAsyncRequest(ESatNotifyDisplayTextPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
	
	}
	
void CTestSatPCmds1f::Release6Test5L()
	{
	/**
	Testing GetInkey Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT334
	*/
	RSat::TGetInkeyV6* getInkey = new (ELeave) RSat::TGetInkeyV6;
	CleanupStack::PushL(getInkey);
	
	RSat::TGetInkeyV6Pckg* getInkeyPckg = new (ELeave) RSat::TGetInkeyV6Pckg(*getInkey);
	CleanupStack::PushL(getInkeyPckg);

	sat.NotifyGetInkeyPCmd(reqStatus, *getInkeyPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(getInkey->iRspFormat			==DSATTSY_USER_RSP_FORMAT);
	TEST(getInkey->iText				==DSATTSY_TEXT_TO_DISPLAY);
	TEST(getInkey->iIconId.iIdentifier	==DSATTSY_ICON_ID1);
	TEST(getInkey->iIconId.iQualifier	==DSATTSY_ICON_QUALIFIER1);
	TEST(getInkey->iHelp				==DSATTSY_HELP);
	TEST(getInkey->iDuration.iTimeUnit ==DSATTSY_TIME_UNIT);
	TEST(getInkey->iDuration.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(getInkey->iMode ==DSATTSY_GET_INKEY_RSP_MODE);
    
  	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);    
    
    TEST(getInkey->iTextAttribute.iStatus == textAttribute.iStatus);   
    TEST(getInkey->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(getInkey->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - NotifyGetInkeyPCmd - GetInkey command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the GetInkey Proactive command
    @test TSAT335
    */
    
    //Cancel	
	sat.NotifyGetInkeyPCmd(reqStatus, *getInkeyPckg);
	sat.CancelAsyncRequest(ESatNotifyGetInkeyPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
	
	/**
	Testing GetInput Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT336
	*/
	RSat::TGetInputV6* getInput = new (ELeave) RSat::TGetInputV6;
	CleanupStack::PushL(getInput);
	
	RSat::TGetInputV6Pckg* getInputPckg = new (ELeave) RSat::TGetInputV6Pckg(*getInput);
	CleanupStack::PushL(getInputPckg);
	
	sat.NotifyGetInputPCmd(reqStatus, *getInputPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(getInput->iInputDisplayOption		== DSATTSY_INPUT_DISPLAY_OPTION);
	TEST(getInput->iRspFormat				== DSATTSY_GET_INPUT_RSP_FORMAT);
	TEST(getInput->iText					== DSATTSY_TEXT_TO_DISPLAY);
	TEST(getInput->iRspLength.iMinRspLength	== DSATTSY_MIN_RSP_LENGTH);
	TEST(getInput->iRspLength.iMaxRspLength	== DSATTSY_MAX_RSP_LENGTH);
	TEST(getInput->iDefaultText				== DSATTSY_DEFAULT_TEXT);
	TEST(getInput->iIconId.iIdentifier		== DSATTSY_ICON_ID1);
	TEST(getInput->iIconId.iQualifier		== DSATTSY_ICON_QUALIFIER1);
	TEST(getInput->iHelp					== DSATTSY_HELP);
    TEST(getInput->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(getInput->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(getInput->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - NotifyGetInputPCmd - GetInput command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the DisplayText Proactive command
    @test TSAT337
    */
    
    //Cancel	
	sat.NotifyGetInputPCmd(reqStatus, *getInputPckg);
	sat.CancelAsyncRequest(ESatNotifyGetInputPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
		
	/**
	Testing SelectItem Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT340
	*/
	RSat::TSelectItemV6* selectItem = new (ELeave) RSat::TSelectItemV6;
	CleanupStack::PushL(selectItem);
	
	RSat::TSelectItemV6Pckg* selectItemPckg = new (ELeave) RSat::TSelectItemV6Pckg(*selectItem);
	CleanupStack::PushL(selectItemPckg);
	
	sat.NotifySelectItemPCmd(reqStatus, *selectItemPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(selectItem->iPresentationType	==DSATTSY_PRESENTATION_TYPE);
	TEST(selectItem->iHelp				==DSATTSY_HELP);
	TEST(selectItem->iAlphaId.iAlphaId	==DSATTSY_ALPHA_ID1);
	TEST(selectItem->iAlphaId.iStatus	==DSATTSY_ALPHA_ID1_STATUS);
	TEST(selectItem->iDefaultItemId		==DSATTSY_DEFAULT_ITEM);
	TEST(selectItem->iIconId.iIdentifier ==DSATTSY_ICON_ID1);
	TEST(selectItem->iIconId.iQualifier	==DSATTSY_ICON_QUALIFIER1);
	TEST(selectItem->iIconListQualifier	==DSATTSY_ICON_QUALIFIER2);
	TEST(selectItem->iPreference=DSATTSY_SELECTION_PREFERENCE);	
    TEST(selectItem->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(selectItem->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(selectItem->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - SelectItemPCmd - SelectItem command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the SelectItem Proactive command
    @test TSAT341
    */
    
    //Cancel	
	sat.NotifySelectItemPCmd(reqStatus, *selectItemPckg);
	sat.CancelAsyncRequest(ESatNotifySelectItemPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - SelectItemPCmd - SelectItem cancel command- passed"),iTestCount++);
	
	CleanupStack::PopAndDestroy(2);
	
	/**
	Testing SendSm Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT342
	*/
	RSat::TSendSmV6* sendSm = new (ELeave) RSat::TSendSmV6;
	CleanupStack::PushL(sendSm);
	
	RSat::TSendSmV6Pckg* sendSmPckg = new (ELeave) RSat::TSendSmV6Pckg(*sendSm);
	CleanupStack::PushL(sendSmPckg);

	sat.NotifySendSmPCmd(reqStatus, *sendSmPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(sendSm->iAlphaId.iAlphaId	==DSATTSY_ALPHA_ID1);
	TEST(sendSm->iAlphaId.iStatus	==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendSm->iAddress.iTypeOfNumber	==DSATTSY_TON);
	TEST(sendSm->iAddress.iNumberPlan	==DSATTSY_NPI);
	TEST(sendSm->iAddress.iTelNumber	==DSATTSY_TEL_NUMBER);
	TEST(sendSm->iSmsTpdu				==DSATTSY_SMS_TPDU); 
	TEST(sendSm->iIconId.iIdentifier	==DSATTSY_ICON_ID1);
	TEST(sendSm->iIconId.iQualifier		==DSATTSY_ICON_QUALIFIER1);	
    TEST(sendSm->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(sendSm->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(sendSm->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - SendSmPCmd - SendSm command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the SendSm Proactive command
    @test TSAT343
    */
    
    //Cancel	
	sat.NotifySendSmPCmd(reqStatus, *sendSmPckg);
	sat.CancelAsyncRequest(ESatNotifySendSmPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
	
	/**
	Testing SetUpCall Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT344
	*/
	RSat::TSetUpCallV6* setUpCall = new (ELeave) RSat::TSetUpCallV6;
	CleanupStack::PushL(setUpCall);
	
	RSat::TSetUpCallV6Pckg* setUpCallPckg = new (ELeave) RSat::TSetUpCallV6Pckg(*setUpCall);
	CleanupStack::PushL(setUpCallPckg);

	sat.NotifySetUpCallPCmd(reqStatus, *setUpCallPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(setUpCall->iType								==DSATTSY_SET_UP_CALL_TYPE);
	TEST(setUpCall->iAlphaIdConfirmationPhase.iAlphaId	==DSATTSY_ALPHA_ID1);
	TEST(setUpCall->iAlphaIdConfirmationPhase.iStatus	==DSATTSY_ALPHA_ID1_STATUS);
	TEST(setUpCall->iIconIdConfirmationPhase.iIdentifier==DSATTSY_ICON_ID1);
	TEST(setUpCall->iIconIdConfirmationPhase.iQualifier	==DSATTSY_ICON_QUALIFIER1);
	TEST(setUpCall->iAddress.iTypeOfNumber				==DSATTSY_TON);
	TEST(setUpCall->iAddress.iNumberPlan				==DSATTSY_NPI);
	TEST(setUpCall->iAddress.iTelNumber					==DSATTSY_TEL_NUMBER);
	TEST(setUpCall->iCapabilityConfigParams			==DSATTSY_CCP1);
	TEST(setUpCall->iSubAddress							==DSATTSY_SUBADDRESS);
	TEST(setUpCall->iDuration.iTimeUnit					==DSATTSY_TIME_UNIT);
	TEST(setUpCall->iDuration.iNumOfUnits				==DSATTSY_NUM_OF_UNITS);
	TEST(setUpCall->iAlphaIdCallSetUpPhase.iAlphaId		==DSATTSY_ALPHA_ID2);		
	TEST(setUpCall->iAlphaIdCallSetUpPhase.iStatus		==DSATTSY_ALPHA_ID2_STATUS);		
	TEST(setUpCall->iIconIdCallSetUpPhase.iIdentifier ==DSATTSY_ICON_ID2);
	TEST(setUpCall->iIconIdCallSetUpPhase.iQualifier	==DSATTSY_ICON_QUALIFIER2);
	TEST(setUpCall->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - SetUpCall - SetUp Call command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the SetUpCall Proactive command
    @test TSAT345
    */
    
    //Cancel	
	sat.NotifySetUpCallPCmd(reqStatus, *setUpCallPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpCallPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);

	CleanupStack::PopAndDestroy(2);
	
	/**
	Testing SetUpIdleModeText Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT346
	@test TSAT347
	*/
	RSat::TSetUpIdleModeTextV6* setUpIdleModeText = new (ELeave) RSat::TSetUpIdleModeTextV6;
	CleanupStack::PushL(setUpIdleModeText);
	
	RSat::TSetUpIdleModeTextV6Pckg* setUpIdleModeTextPckg = new (ELeave) RSat::TSetUpIdleModeTextV6Pckg(*setUpIdleModeText);
	CleanupStack::PushL(setUpIdleModeTextPckg);

	TEST(setUpIdleModeText->ExtensionId() == RSat::KSatV6);
	TEST(setUpIdleModeText->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(setUpIdleModeText->PCmdNumber()==DSATTSY_SET_UP_IDLE_MODE_TEXT_NUMBER);

   	TEST(setUpIdleModeText->iType==DSATTSY_IDLE_MODE_TXT_TYPE);
 	TEST(setUpIdleModeText->iText==DSATTSY_IDLE_MODE_TEXT);
   	TEST(setUpIdleModeText->iIconId.iIdentifier==DSATTSY_ICON_ID1);
   	TEST(setUpIdleModeText->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
   	TEST(setUpIdleModeText->iCodingScheme==DSATTSY_TEXT_ENCODING_SCHEME);

	TEST(setUpIdleModeText->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(setUpIdleModeText->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

 	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeText (Unicode) with RSat::TSetUpIdleModeTextV6 passed"),iTestCount++);

 	// Set Up Idle Mode Text Cancel
 	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextPckg);
 	sat.CancelAsyncRequest(ESatNotifySetUpIdleModeTextPCmd);

 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus.Int()==KErrCancel);
 	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeTextPCmdCancel with RSat::TSetUpIdleModeTextV6 passed"),iTestCount++);

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

	TEST(setUpIdleModeText->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(setUpIdleModeText->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);

	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeText (8-bit) with RSat::TSetUpIdleModeTextV6 passed"),iTestCount++);

	// Set Up Idle Mode Text Cancel
	sat.NotifySetUpIdleModeTextPCmd(reqStatus,*setUpIdleModeTextPckg);
	sat.CancelAsyncRequest(ESatNotifySetUpIdleModeTextPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifySetUpIdleModeTextPCmdCancel with RSat::TSetUpIdleModeTextV6 passed"),iTestCount++);

    INFO_PRINTF2(_L("Test %d - SetUpCall - SetUp Call command- passed"),iTestCount++);  
	
	CleanupStack::PopAndDestroy(2);
	
	}
	
void CTestSatPCmds1f::Release6Test6L()
	{
	/**
	Testing RunAtCommand Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT348
	*/
	RSat::TRunAtCommandV6* runAtCmd = new (ELeave) RSat::TRunAtCommandV6;
	CleanupStack::PushL(runAtCmd);
	
	RSat::TRunAtCommandV6Pckg* runAtCmdPckg = new (ELeave) RSat::TRunAtCommandV6Pckg(*runAtCmd);
	CleanupStack::PushL(runAtCmdPckg);

	sat.NotifyRunAtCommandPCmd(reqStatus, *runAtCmdPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(runAtCmd->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(runAtCmd->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(runAtCmd->iAtCommand==DSATTSY_AT_COMMAND);
	TEST(runAtCmd->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(runAtCmd->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);	
    
   	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);   
    
    TEST(runAtCmd->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(runAtCmd->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(runAtCmd->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - RunAtCmd - TRunAtCommandV6- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the RunAtCommand Proactive command
    @test TSAT349
    */
    
    //Cancel	
	sat.NotifyRunAtCommandPCmd(reqStatus, *runAtCmdPckg);
	sat.CancelAsyncRequest(ESatNotifyRunAtCommandPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);		
    
    CleanupStack::PopAndDestroy(2);
    
    /**
	Testing SendDtmf Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT350
	*/
	RSat::TSendDtmfV6* sendDtmf = new (ELeave) RSat::TSendDtmfV6;
	CleanupStack::PushL(sendDtmf);
	
	RSat::TSendDtmfV6Pckg* sendDtmfPckg = new (ELeave) RSat::TSendDtmfV6Pckg(*sendDtmf);
	CleanupStack::PushL(sendDtmfPckg);

	sat.NotifySendDtmfPCmd(reqStatus, *sendDtmfPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(sendDtmf->iAlphaId.iAlphaId	==DSATTSY_ALPHA_ID1);
	TEST(sendDtmf->iAlphaId.iStatus	==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendDtmf->iDtmfString		==DSATTSY_DTMF_STRING);
	TEST(sendDtmf->iIconId.iIdentifier ==DSATTSY_ICON_ID1);
	TEST(sendDtmf->iIconId.iQualifier	==DSATTSY_ICON_QUALIFIER1);	
    TEST(sendDtmf->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(sendDtmf->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(sendDtmf->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - SendDtmf - TSendDtmfV6- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the SendDtmf Proactive command
    @test TSAT351
    */
    
    //Cancel	
	sat.NotifySendDtmfPCmd(reqStatus, *sendDtmfPckg);
	sat.CancelAsyncRequest(ESatNotifySendDtmfPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);

    CleanupStack::PopAndDestroy(2);
    	
	/**
	Testing CloseChannel Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT352
	*/
	RSat::TCloseChannelV6* closeChannel = new (ELeave) RSat::TCloseChannelV6;
	CleanupStack::PushL(closeChannel);
	
	RSat::TCloseChannelV6Pckg* closeChannelPckg = new (ELeave) RSat::TCloseChannelV6Pckg(*closeChannel);
	CleanupStack::PushL(closeChannelPckg);

	sat.NotifyCloseChannelPCmd(reqStatus, *closeChannelPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(closeChannel->iDestination==DSATTSY_CARD_READER_1);
	TEST(closeChannel->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(closeChannel->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(closeChannel->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(closeChannel->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);	
    TEST(closeChannel->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(closeChannel->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(closeChannel->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - CloseChannel - TCloseChannelV6- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the CloseChannel Proactive command
    @test TSAT353
    */
    
    //Cancel	
	sat.NotifyCloseChannelPCmd(reqStatus, *closeChannelPckg);
	sat.CancelAsyncRequest(ESatNotifyCloseChannelPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
	
	/**
	Testing ReceiveData Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT354
	*/
	RSat::TReceiveDataV6* receiveData = new (ELeave) RSat::TReceiveDataV6;
	CleanupStack::PushL(receiveData);
	
	RSat::TReceiveDataV6Pckg* receiveDataPckg = new (ELeave) RSat::TReceiveDataV6Pckg(*receiveData);
	CleanupStack::PushL(receiveDataPckg);

	sat.NotifyReceiveDataPCmd(reqStatus, *receiveDataPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(receiveData->iDestination==DSATTSY_CARD_READER_1);
	TEST(receiveData->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(receiveData->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(receiveData->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(receiveData->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(receiveData->iChannelDataLength==DSATTSY_DATA_LENGTH);	
    TEST(receiveData->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(receiveData->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(receiveData->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - ReceiveData - TReceiveDataV6- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the ReceiveData Proactive command
    @test TSAT355
    */
    
    //Cancel	
	sat.NotifyReceiveDataPCmd(reqStatus, *receiveDataPckg);
	sat.CancelAsyncRequest(ESatNotifyReceiveDataPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
    
   	CleanupStack::PopAndDestroy(2);
    
	}
	
void CTestSatPCmds1f::Release6Test7L()
	{
   	/**
	Testing SendData Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT356
	*/
	RSat::TSendDataV6* sendData = new (ELeave) RSat::TSendDataV6;
	CleanupStack::PushL(sendData);
	
	RSat::TSendDataV6Pckg* sendDataPckg = new (ELeave) RSat::TSendDataV6Pckg(*sendData);
	CleanupStack::PushL(sendDataPckg);
	

	sat.NotifySendDataPCmd(reqStatus, *sendDataPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(sendData->iDestination==DSATTSY_CARD_READER_1);
	TEST(sendData->iMode==DSATTSY_SEND_DATA_MODE);
	TEST(sendData->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendData->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(sendData->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(sendData->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(sendData->iChannelData==DSATTSY_CHANNEL_DATA);	

  	RSat::TTextAttribute textAttribute;
	textAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
	const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
	textAttribute.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
	
    TEST(sendData->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(sendData->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(sendData->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - SendData - TSendDataV6- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the SendData Proactive command
    @test TSAT357
    */
    
    //Cancel	
	sat.NotifySendDataPCmd(reqStatus, *sendDataPckg);
	sat.CancelAsyncRequest(ESatNotifySendDataPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
    CleanupStack::PopAndDestroy(2);
    
  	/**
	Testing GetServiceSearch command of the Release 6 Features of USIM Application ToolKit
	@test TSAT358
	*/
	RSat::TServiceSearchV6* serviceSearch = new (ELeave) RSat::TServiceSearchV6;
	CleanupStack::PushL(serviceSearch);
	
	RSat::TServiceSearchV6Pckg* serviceSearchPckg = new (ELeave) RSat::TServiceSearchV6Pckg(*serviceSearch);
	CleanupStack::PushL(serviceSearchPckg);

	sat.NotifyServiceSearchPCmd(reqStatus, *serviceSearchPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(serviceSearch->iAlphaId.iStatus=DSATTSY_ALPHA_ID1_STATUS);
	TEST(serviceSearch->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(serviceSearch->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(serviceSearch->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(serviceSearch->iSearch.iBearerId==DSATTSY_BEARER_TECH_INDEPENDENT);
	TEST(serviceSearch->iSearch.iSearchParam==DSATTSY_SERVICE_SEARCH_PARAMS);
	TEST(serviceSearch->iFilter.iBearerId==DSATTSY_BEARER_TECH_INDEPENDENT);
	TEST(serviceSearch->iFilter.iDeviceFilter==DSATTSY_FILTER_INFO);	
    TEST(serviceSearch->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(serviceSearch->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(serviceSearch->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - ServiceSearch - TServiceSearchV6- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the ServiceSearch Proactive command
    @test TSAT359
    */
    
    //Cancel	
	sat.NotifyServiceSearchPCmd(reqStatus, *serviceSearchPckg);
	sat.CancelAsyncRequest(ESatNotifyServiceSearchPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
	
	/**
	Testing GetServiceInfo command of the Release 6 Features of USIM Application ToolKit
	@test TSAT360
	*/
	RSat::TGetServiceInfoV6* getServiceInfo = new (ELeave) RSat::TGetServiceInfoV6;
	CleanupStack::PushL(getServiceInfo);
	
	RSat::TGetServiceInfoV6Pckg* getServiceInfoPckg = new (ELeave) RSat::TGetServiceInfoV6Pckg(*getServiceInfo);
	CleanupStack::PushL(getServiceInfoPckg);
	
	sat.NotifyGetServiceInfoPCmd(reqStatus, *getServiceInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(getServiceInfo->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(getServiceInfo->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(getServiceInfo->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(getServiceInfo->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(getServiceInfo->iAttributeInfo.iBearerId==DSATTSY_BEARER_TECH_INDEPENDENT);
	TEST(getServiceInfo->iAttributeInfo.iAttributeInfo==DSATTSY_ATTRIBUTE_INFO);	
    TEST(getServiceInfo->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(getServiceInfo->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(getServiceInfo->iFrameId==DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1);
	
    INFO_PRINTF2(_L("Test %d - getServiceInfo - TGetServiceInfoV6- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the GetServiceInfo Proactive command
    @test TSAT361
    */
    
    //Cancel	
	sat.NotifyGetServiceInfoPCmd(reqStatus, *getServiceInfoPckg);
	sat.CancelAsyncRequest(ESatNotifyGetServiceInfoPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);	
	
	/**
	Testing SendSs Proactive command which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT369
	*/
	RSat::TSendSsV6* sendSs = new (ELeave) RSat::TSendSsV6;
	CleanupStack::PushL(sendSs);
	
	RSat::TSendSsV6Pckg* sendSsPckg = new (ELeave) RSat::TSendSsV6Pckg(*sendSs);
	CleanupStack::PushL(sendSsPckg);

	sat.NotifySendSsPCmd(reqStatus, *sendSsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(sendSs->iAlphaId.iAlphaId	==DSATTSY_ALPHA_ID1);
	TEST(sendSs->iAlphaId.iStatus	==DSATTSY_ALPHA_ID1_STATUS);
	TEST(sendSs->iSsString.iTypeOfNumber	==DSATTSY_TON);
	TEST(sendSs->iSsString.iNumberPlan	==DSATTSY_NPI);
	TEST(sendSs->iSsString.iSsString	==DSATTSY_SS_STRING);
	TEST(sendSs->iIconId.iIdentifier	==DSATTSY_ICON_ID1);
	TEST(sendSs->iIconId.iQualifier	==DSATTSY_ICON_QUALIFIER1);
    TEST(sendSs->iTextAttribute.iStatus == textAttribute.iStatus);     
    TEST(sendSs->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
    INFO_PRINTF2(_L("Test %d - SendSs - SendSs command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the SendSs Proactive command
    @test TSAT370
    */
    
    //Cancel	
	sat.NotifySendSsPCmd(reqStatus, *sendSsPckg);
	sat.CancelAsyncRequest(ESatNotifySendSsPCmd);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	CleanupStack::PopAndDestroy(2);
	
	// Now test Open Channel with version 6 of classes

	/**
	Open Channel Cs with V6
	@test TSAT362
	@test TSAT363
	*/
	
	RSat::TOpenCsChannelV6* openCsChannelV6 = new (ELeave) RSat::TOpenCsChannelV6;
	CleanupStack::PushL(openCsChannelV6);
	
	RSat::TOpenCsChannelV6Pckg* openCsChannelV6Pckg = new (ELeave) RSat::TOpenCsChannelV6Pckg(*openCsChannelV6);
	CleanupStack::PushL(openCsChannelV6Pckg);
	
	openCsChannelV6->iPCmdType		= DSATTSY_CS_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus, *openCsChannelV6Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(openCsChannelV6->PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(openCsChannelV6->iPCmdType==DSATTSY_CS_BEARER);
	TEST(openCsChannelV6->iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(openCsChannelV6->iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(openCsChannelV6->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(openCsChannelV6->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(openCsChannelV6->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(openCsChannelV6->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(openCsChannelV6->iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(openCsChannelV6->iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(openCsChannelV6->iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(openCsChannelV6->iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(openCsChannelV6->iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(openCsChannelV6->iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(openCsChannelV6->iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openCsChannelV6->iAddress.iTypeOfNumber==DSATTSY_TON);
	TEST(openCsChannelV6->iAddress.iNumberPlan==DSATTSY_NPI);
	TEST(openCsChannelV6->iAddress.iTelNumber==DSATTSY_TEL_NUMBER);
	TEST(openCsChannelV6->iSubAddress==DSATTSY_SUBADDRESS);
	TEST(openCsChannelV6->iDuration1.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openCsChannelV6->iDuration1.iNumOfUnits==DSATTSY_NUM_OF_UNITS);
	TEST(openCsChannelV6->iDuration2.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openCsChannelV6->iDuration2.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(openCsChannelV6->iLocalAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(openCsChannelV6->iLocalAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openCsChannelV6->iUserLogin==DSATTSY_LOGIN);
	TEST(openCsChannelV6->iUserPassword==DSATTSY_PASSWORD);
	TEST(openCsChannelV6->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(openCsChannelV6->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(openCsChannelV6->iFrameId==DSATTSYV6_FRAME_IDENTIFIER1);
	
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - CS channel (RSat::TOpenCsChannelV6) - passed"),iTestCount++);

	// Open Channel Cancel
	sat.NotifyOpenChannelPCmd(reqStatus,*openCsChannelV6Pckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenCsChannelV6) passed"),iTestCount++);

	CleanupStack::PopAndDestroy(2);
	
	/**
	Open Channel Gprs with V6
	@test TSAT375
	@test TSAT376
	*/
	
	RSat::TOpenGprsChannelV6* openGprsChannelV6 = new (ELeave) RSat::TOpenGprsChannelV6;
	CleanupStack::PushL(openGprsChannelV6);
	
	RSat::TOpenGprsChannelV6Pckg* openGprsChannelV6Pckg = new (ELeave) RSat::TOpenGprsChannelV6Pckg(*openGprsChannelV6);
	CleanupStack::PushL(openGprsChannelV6Pckg);

	openGprsChannelV6->iPCmdType=DSATTSY_GPRS_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus,*openGprsChannelV6Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(openGprsChannelV6->PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(openGprsChannelV6->iPCmdType==DSATTSY_GPRS_BEARER);
	TEST(openGprsChannelV6->iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(openGprsChannelV6->iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(openGprsChannelV6->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(openGprsChannelV6->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(openGprsChannelV6->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(openGprsChannelV6->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(openGprsChannelV6->iBearer.iType==DSATTSYV6_GPRS_BEARER_TYPE1);
	TEST(openGprsChannelV6->iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(openGprsChannelV6->iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(openGprsChannelV6->iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(openGprsChannelV6->iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(openGprsChannelV6->iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(openGprsChannelV6->iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openGprsChannelV6->iAccessName==DSATTSY_ACCESS_NAME);
	TEST(openGprsChannelV6->iLocalAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(openGprsChannelV6->iLocalAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openGprsChannelV6->iUserLogin==DSATTSY_LOGIN);
	TEST(openGprsChannelV6->iUserPassword==DSATTSY_PASSWORD);	
	TEST(openGprsChannelV6->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(openGprsChannelV6->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(openGprsChannelV6->iFrameId==DSATTSYV6_FRAME_IDENTIFIER1);
	
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - GPRS Channel (RSat::TOpenGprsChannelV6) - passed"),iTestCount++);

	// Open Channel Cancel with V6
	sat.NotifyOpenChannelPCmd(reqStatus,*openGprsChannelV6Pckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenGprsChannelV6) passed"),iTestCount++);

	CleanupStack::PopAndDestroy(2);
	
	/**
	Open Channel LocalLinks with version 6
	@test TSAT377
	@test TSAT378
	*/
	
	RSat::TOpenLocalLinksChannelV6* openLocalChannelV6 = new(ELeave) RSat::TOpenLocalLinksChannelV6;
	CleanupStack::PushL(openLocalChannelV6);

	RSat::TOpenLocalLinksChannelV6Pckg* openLocalChannelV6Pckg = new(ELeave) RSat::TOpenLocalLinksChannelV6Pckg(*openLocalChannelV6);
	CleanupStack::PushL(openLocalChannelV6Pckg);

	TEST(openLocalChannelV6->ExtensionId() == RSat::KSatV6);
	TEST(openLocalChannelV6->iTextAttribute.iStatus == RSat::ETextAttributeNotSet);

	openLocalChannelV6->iPCmdType=DSATTSY_LOCAL_BEARER;

	sat.NotifyOpenChannelPCmd(reqStatus,*openLocalChannelV6Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(openLocalChannelV6->PCmdNumber()==DSATTSY_OPEN_CHANNEL_NUMBER);
	TEST(openLocalChannelV6->iPCmdType==DSATTSY_LOCAL_BEARER);
	TEST(openLocalChannelV6->iLinkEst==DSATTSY_ON_DEMAND_ESTABLISHMENT);
	TEST(openLocalChannelV6->iReconnectionMode==DSATTSY_AUTO_RECONNECTION);
	TEST(openLocalChannelV6->iAlphaId.iStatus==DSATTSY_ALPHA_ID1_STATUS);
	TEST(openLocalChannelV6->iAlphaId.iAlphaId==DSATTSY_ALPHA_ID1);
	TEST(openLocalChannelV6->iIconId.iIdentifier==DSATTSY_ICON_ID1);
	TEST(openLocalChannelV6->iIconId.iQualifier==DSATTSY_ICON_QUALIFIER1);
	TEST(openLocalChannelV6->iBearer.iType==DSATTSY_CSD_BEARER_TYPE);
	TEST(openLocalChannelV6->iBearer.iParams==DSATTSY_BEARER_PARAMS);
	TEST(openLocalChannelV6->iBufferSize==DSATTSY_BUFFER_SIZE);
	TEST(openLocalChannelV6->iSimMeInterface.iPrtNumber==DSATTSY_PORT_1);
	TEST(openLocalChannelV6->iSimMeInterface.iTransportProto==DSATTSY_TCP_PROTO);
	TEST(openLocalChannelV6->iDestinationAddress.iType==DSATTSY_IPV4_TYPE);
	TEST(openLocalChannelV6->iDestinationAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openLocalChannelV6->iDuration1.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openLocalChannelV6->iDuration1.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(openLocalChannelV6->iDuration2.iTimeUnit==DSATTSY_TIME_UNIT);
	TEST(openLocalChannelV6->iDuration2.iNumOfUnits==DSATTSY_TIME_UNIT);
	TEST(openLocalChannelV6->iRemoteAddress.iCoding==DSATTSY_CODING);
	TEST(openLocalChannelV6->iRemoteAddress.iAddress==DSATTSY_ADDRESS);
	TEST(openLocalChannelV6->iUserPassword==DSATTSY_PASSWORD);
	TEST(openLocalChannelV6->iTextAttribute.iStatus == textAttribute.iStatus);
	TEST(openLocalChannelV6->iTextAttribute.iTextAttributeData == textAttribute.iTextAttributeData);
	TEST(openLocalChannelV6->iFrameId==DSATTSYV6_FRAME_IDENTIFIER1);
	
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd - Local Channel (RSat::TOpenLocalLinksChannelV6) - passed"),iTestCount++);

	// Open Channel Cancel with version 6
	sat.NotifyOpenChannelPCmd(reqStatus,*openLocalChannelV6Pckg);
	sat.CancelAsyncRequest(ESatNotifyOpenChannelPCmd);

	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::NotifyOpenChannelPCmd Cancel (RSat::TOpenLocalLinksChannelV6) passed"),iTestCount++);

	CleanupStack::PopAndDestroy(2);	
	}

