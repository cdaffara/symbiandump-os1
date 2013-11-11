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
#include "TE_EtelSatenvelopecmds.h"

CTestSatEnvelopeCmds::CTestSatEnvelopeCmds()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestSatEnvelopeCmds"));
	}

enum TVerdict CTestSatEnvelopeCmds::doTestStepL()
	{
	INFO_PRINTF1(_L("**************************************"));
	INFO_PRINTF1(_L("Test RSat Envelope Cmds Functionality"));
	INFO_PRINTF1(_L("**************************************"));

	RPhone phone;
	TInt ret=phone.Open(iTelServer,DSATTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RSat sat;
	ret=sat.Open(phone);
    TEST(ret==KErrNone);

	//
	// Local variables used throughout the sat tests
	//
	TRequestStatus reqStatus;
	iTestCount=1;

	//
	// Get ME side SAT profile for V2
	//
	RSat::TSatProfileV2 profile;
	RSat::TSatProfileV2Pckg profilePckg(profile);

	TEST(profile.ExtensionId() == RSat::KSatV2);

	sat.GetMeSideSatProfile(reqStatus, profilePckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	TEST(profile.iSatProfileByte1==0x01);
	TEST(profile.iSatProfileByte2==0x02);
	TEST(profile.iSatProfileByte3==0x03);
	TEST(profile.iSatProfileByte4==0x04);
	TEST(profile.iSatProfileByte5==0x05);
	TEST(profile.iSatProfileByte6==0x06);
	TEST(profile.iSatProfileByte7==0x07);
	TEST(profile.iSatProfileByte8==0x08);
	TEST(profile.iSatProfileByte9==0x09);
	TEST(profile.iSatProfileByte10==0x0A);
	TEST(profile.iSatProfileByte11==0x0B);
	TEST(profile.iSatProfileByte12==0x0C);
	TEST(profile.iSatProfileByte13==0x0D);
	TEST(profile.iSatProfileByte14==0x0E);
	TEST(profile.iSatProfileByte15==0x0F);
	TEST(profile.iSatProfileByte16==0x10);
	TEST(profile.iSatProfileByte17==0x20);
	TEST(profile.iSatProfileByte18==0x30);
	TEST(profile.iNumOfChannels==DSATTSY_NUMBER_OF_CHANNELS);
	TEST(profile.iScreenHeight==DSATTSY_SCREEN_HEIGHT);
	TEST(profile.iScreenWidth==DSATTSY_SCREEN_WIDTH);
	TEST(profile.iTiaEiaProtoVersion==DSATTSY_PROTO_VERSION);
	TEST(profile.iWidthReduction==DSATTSY_WIDTH_REDUCTION);

	INFO_PRINTF2(_L("Test %d - RSat::GetMeSideSatProfile with RSat::TSatProfileV2 passed"),iTestCount++);

	sat.GetMeSideSatProfile(reqStatus, profilePckg);
	sat.CancelAsyncRequest(ESatGetMeSideSatProfile);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::GetMeSideSatProfileCancel with RSat::TSatProfileV2 passed"),iTestCount++);

	//
	// Client SAT profile indication
	//
	TEST(sat.ClientSatProfileIndication(profilePckg)==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::ClientSatProfileIndication with RSat::TSatProfileV2 passed"),iTestCount++);

	//
	// Get ME side SAT profile for V5
	//
	RSat::TSatProfileV5 profileV5;
	RSat::TSatProfileV5Pckg profileV5Pckg(profileV5);

	TEST(profileV5.ExtensionId() == RSat::KSatV5);

	sat.GetMeSideSatProfile(reqStatus, profileV5Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// Pre-version 5 parameters in SAT profile
	TEST(profileV5.iSatProfileByte1==0x01);
	TEST(profileV5.iSatProfileByte2==0x02);
	TEST(profileV5.iSatProfileByte3==0x03);
	TEST(profileV5.iSatProfileByte4==0x04);
	TEST(profileV5.iSatProfileByte5==0x05);
	TEST(profileV5.iSatProfileByte6==0x06);
	TEST(profileV5.iSatProfileByte7==0x07);
	TEST(profileV5.iSatProfileByte8==0x08);
	TEST(profileV5.iSatProfileByte9==0x09);
	TEST(profileV5.iSatProfileByte10==0x0A);
	TEST(profileV5.iSatProfileByte11==0x0B);
	TEST(profileV5.iSatProfileByte12==0x0C);
	TEST(profileV5.iSatProfileByte13==0x0D);
	TEST(profileV5.iSatProfileByte14==0x0E);
	TEST(profileV5.iSatProfileByte15==0x0F);
	TEST(profileV5.iSatProfileByte16==0x10);
	TEST(profileV5.iSatProfileByte17==0x20);
	TEST(profileV5.iSatProfileByte18==0x30);
	TEST(profileV5.iNumOfChannels==DSATTSY_NUMBER_OF_CHANNELS);
	TEST(profileV5.iScreenHeight==DSATTSY_SCREEN_HEIGHT);
	TEST(profileV5.iScreenWidth==DSATTSY_SCREEN_WIDTH);
	TEST(profileV5.iTiaEiaProtoVersion==DSATTSY_PROTO_VERSION);
	TEST(profileV5.iWidthReduction==DSATTSY_WIDTH_REDUCTION);
	// Version 5 parameters in profile
	// (test also that the TSatProfileByte28 and TSatProfileByte29
	// have been defined correctly as each should set a different bit).
	TEST(profileV5.iSatProfileByte28==
			(RSat::KCapsTextAttributeAlignmentLeft | RSat::KCapsTextAttributeAlignmentCentre |
			 RSat::KCapsTextAttributeAlignmentRight | RSat::KCapsTextAttributeFontSizeNormal |
			 RSat::KCapsTextAttributeFontSizeLarge | RSat::KCapsTextAttributeFontSizeSmall)); // 0x3F
	TEST(profileV5.iSatProfileByte29==(RSat::KCapsTextAttributeStyleNormal |
									 RSat::KCapsTextAttributeStyleBold |
									 RSat::KCapsTextAttributeStyleItalic |
									 RSat::KCapsTextAttributeStyleUnderline |
									 RSat::KCapsTextAttributeStyleStrikethrough |
									 RSat::KCapsTextAttributeStyleForeground |
									 RSat::KCapsTextAttributeStyleBackground)); // 0x7F

	INFO_PRINTF2(_L("Test %d - RSat::GetMeSideSatProfile with RSat::TSatProfileV5 passed"),iTestCount++);

	sat.GetMeSideSatProfile(reqStatus, profileV5Pckg);
	sat.CancelAsyncRequest(ESatGetMeSideSatProfile);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::GetMeSideSatProfileCancel with RSat::TSatProfileV5 passed"),iTestCount++);

	//
	// Client SAT profile indication for V5
	//
	TEST(sat.ClientSatProfileIndication(profileV5Pckg)==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::ClientSatProfileIndication RSat::TSatProfileV5 passed"),iTestCount++);

	// Menu Selection
	RSat::TMenuSelectionV1 selection;
	RSat::TMenuSelectionV1Pckg selectionPckg(selection);

	selection.iHelp		=DSATTSY_HELP_REQUESTED;
	selection.iItemId	=DSATTSY_ITEM_ID;

	sat.MenuSelection(reqStatus, selectionPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	INFO_PRINTF2(_L("Test %d - RSat::MenuSelection passed"),iTestCount++);

	// Menu Selection Cancel
	sat.MenuSelection(reqStatus,selectionPckg);
	sat.CancelAsyncRequest(ESatMenuSelection);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF2(_L("Test %d - RSat::MenuSelectionCancel passed"),iTestCount++);

	// Event Download

	// Test each event

	RSat::TEventList				eventList;
	RSat::TEventDownloadBaseV2		eventBaseV2;
	RSat::TEventDownloadBaseV2Pckg	eventBaseV2Pckg( eventBaseV2 );

	// KUserActivity

	eventList = RSat::KUserActivity;

	sat.EventDownload( reqStatus, eventList, eventBaseV2Pckg );	// No associated data
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownload(KUserActivity) passed"),iTestCount++);

	// Event Download Cancel

	sat.EventDownload( reqStatus, eventList, eventBaseV2Pckg );	// No associated data
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownloadCancel passed"),iTestCount++);

	// KIdleScreenAvailable

	eventList = RSat::KIdleScreenAvailable;

	sat.EventDownload( reqStatus, eventList,  eventBaseV2Pckg );	// No associated data
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownload(KIdleScreenAvailable) passed"),iTestCount++);

	sat.EventDownload( reqStatus, eventList,  eventBaseV2Pckg );	// No associated data
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);

	// KCardReaderStatus

	RSat::TCardReaderStatusEventV2		eventCardStatus;
	RSat::TCardReaderStatusEventV2Pckg	eventCardStatusPckg(eventCardStatus);

	eventList						  = RSat::KCardReaderStatus;
	eventCardStatus.iCardReaderStatus = DSATTSY_EVENT_CARD_READER_STATUS;

	sat.EventDownload( reqStatus, eventList, eventCardStatusPckg );
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownload(KCardReaderStatus) passed"),iTestCount++);

	sat.EventDownload( reqStatus, eventList, eventCardStatusPckg );
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);

	// KLanguageSelection

	RSat::TLanguageSelectionEventV2		languageSelect;
	RSat::TLanguageSelectionEventV2Pckg	languageSelectPckg(languageSelect);

	eventList				 = RSat::KLanguageSelection;
	languageSelect.iLanguage = DSATTSY_EVENT_LANGUAGE_SELECTION;

	sat.EventDownload( reqStatus, eventList, languageSelectPckg );
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownload(KLanguageSelection) passed"),iTestCount++);

	sat.EventDownload( reqStatus, eventList, languageSelectPckg );
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);

	// KBrowserTermination

	RSat::TBrowserTerminationEventV2		browserTerm;
	RSat::TBrowserTerminationEventV2Pckg	browserTermPckg( browserTerm );

	eventList		   = RSat::KBrowserTermination;
	browserTerm.iCause = DSATTSY_EVENT_BROWSER_TERMINATION_CAUSE;

	sat.EventDownload( reqStatus, eventList, browserTermPckg );
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownload(KBrowserTermination) passed"),iTestCount++);

	sat.EventDownload( reqStatus, eventList, browserTermPckg );
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);

	// KDataAvailable

	RSat::TDataAvailableEventV2		dataAvailable;
	RSat::TDataAvailableEventV2Pckg	dataAvailablePckg( dataAvailable );

	eventList	 = RSat::KDataAvailable;

	dataAvailable.iStatus.Append( DSATTSY_EVENT_DATA_AVAILABLE_STATUS );
	dataAvailable.iLength = DSATTSY_EVENT_DATA_AVAILABLE_LENGTH;

	sat.EventDownload( reqStatus, eventList, dataAvailablePckg );
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownload(KDataAvailable) passed"),iTestCount++);

	sat.EventDownload( reqStatus, eventList, dataAvailablePckg );
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);

	// KChannelStatus

	RSat::TChannelStatusEventV2		channelStatus;
	RSat::TChannelStatusEventV2Pckg	channelStatusPckg( channelStatus );

	eventList			  = RSat::KChannelStatus;
	channelStatus.iStatus = DSATTSY_EVENT_CHANNEL_STATUS;

	sat.EventDownload( reqStatus, eventList, channelStatusPckg );
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownload(KChannelStatus) passed"),iTestCount++);

	sat.EventDownload( reqStatus, eventList, channelStatusPckg );
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);

	// KDisplayParamsChanges

	RSat::TDisplayParamsEventV2		displayParams;
	RSat::TDisplayParamsEventV2Pckg	displayParamsPckg( displayParams );

	eventList	 = RSat::KDisplayParamsChanges;

	displayParams.iSatProfileByte14 = DSATTSY_EVENT_DISPLAY_PROF14;
	displayParams.iScreenHeight     = DSATTSY_SCREEN_HEIGHT;
	displayParams.iSatProfileByte15 = DSATTSY_EVENT_DISPLAY_PROF15;
	displayParams.iScreenWidth		= DSATTSY_SCREEN_WIDTH;
	displayParams.iSatProfileByte16 = DSATTSY_EVENT_DISPLAY_PROF16;
	displayParams.iWidthReduction   = DSATTSY_WIDTH_REDUCTION;

	sat.EventDownload( reqStatus, eventList, displayParamsPckg );
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownload(KDisplayParams) passed"),iTestCount++);

	sat.EventDownload( reqStatus, eventList, displayParamsPckg );
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);

	// KLocalConnection

	RSat::TLocalConnectionEventV2		localConnection;
	RSat::TLocalConnectionEventV2Pckg	localConnectionPckg( localConnection );

	eventList	 = RSat::KLocalConnection;
	localConnection.iRecord.iBearerId  = DSATTSY_EVENT_LOCAL_BEARER_ID;
	localConnection.iRecord.iServiceId = DSATTSY_EVENT_LOCAL_SERVICE_ID;
	localConnection.iRecord.iRecord	   = DSATTSY_EVENT_LOCAL_SERVICE_RECORD;
	localConnection.iAddress.iCoding   = DSATTSY_EVENT_LOCAL_ADDR_CODING ;
	localConnection.iAddress.iAddress  = DSATTSY_EVENT_LOCAL_ADDRESS;

	sat.EventDownload( reqStatus, eventList, localConnectionPckg );
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	INFO_PRINTF2(_L("Test %d - RSat::EventDownload(KLocalConnection) passed"),iTestCount++);

	sat.EventDownload( reqStatus, eventList, displayParamsPckg );
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);
	
	/** Testing the Release 6 features supported by the envelope commands*/
	/**
	Testing Event Download envelope command w.r.t NetworkSearchModeChangeEvent which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT324
	*/
	RSat::TNetworkSearchModeChangeEventV6 networkSearchModeChangeEvent;	
	RSat::TNetworkSearchModeChangeEventV6Pckg networkSearchModeChangeEventPckg(networkSearchModeChangeEvent);
	
	eventList=RSat::KNetworkSearchModeChange;
	networkSearchModeChangeEvent.iNetworkSearchMode=DSATTSY_NETWORK_SEARCH_MODE1;
	
	sat.EventDownload(reqStatus,eventList,networkSearchModeChangeEventPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
    INFO_PRINTF2(_L("Test %d - EventDownload - NetworkSearchModeChangeEvent- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the Event Download envelope command
    @test TSAT325
    */
    
    //Cancel	
	sat.EventDownload(reqStatus,eventList,networkSearchModeChangeEventPckg);
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	//BrowsingStatusEvent 
	
	/**
	Testing Event Download envelope command w.r.t TBrowsingStatusEventV6 which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT326
	*/
	RSat::TBrowsingStatusEventV6 browsingStatusEvent;
	RSat::TBrowsingStatusEventV6Pckg	browsingStatusEventPckg(browsingStatusEvent);

	eventList=RSat::KBrowsingStatusChange;
	browsingStatusEvent.iBrowserStatus = DSATTSY_BROWSING_STATUS1;
	
	sat.EventDownload(reqStatus,eventList,browsingStatusEventPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
    INFO_PRINTF2(_L("Test %d - EventDownload - BrowsingStatusEvent- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the Event Download envelope command
    @test TSAT327
    */
    
    //Cancel	
	sat.EventDownload(reqStatus,eventList, browsingStatusEventPckg);
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	//FramesInformationChangedEvent
	
	/**
	Testing Event Download envelope command w.r.t FramesInformationChangedEvent which has been updated
    as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT328
	*/
	RSat::TFramesInformationChangedEventV6 framesInfoChangedEvent;
	RSat::TFramesInformationChangedEventV6Pckg	framesInfoChangedEventPckg(framesInfoChangedEvent);

	eventList=RSat::KFramesInformationChange;
	framesInfoChangedEvent.iFramesInformation.iFrameId = DSATTSY_FRAMES_INFORMATION_FRAMEID;
	framesInfoChangedEvent.iFramesInformation.iFrameList= DSATTSY_FRAMES_INFORMATION_FRAME_LIST;
	
	sat.EventDownload(reqStatus,eventList, framesInfoChangedEventPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
    INFO_PRINTF2(_L("Test %d - EventDownload - framesInformationChangedEvent- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the Event Download envelope command
    @test TSAT329
    */
    
    //Cancel	
	sat.EventDownload(reqStatus,eventList, framesInfoChangedEventPckg);
	sat.CancelAsyncRequest(ESatEventDownload);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	/**
	Testing Multimedia Messaging Service Transfer Status envelope command which has been newly
	added as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT299
	*/
	RSat::TMmsTransferStatusV6 mmsTransferStatus;
	RSat::TMmsTransferStatusV6Pckg	mmsTransferStatusPckg(mmsTransferStatus);

	mmsTransferStatus.iDeviceId = DSATTSYV6_DEVICE_ID3;
	mmsTransferStatus.iMultimediaMessageId =DSATTSYV6_MULTIMEDIA_MESSAGE_ID1;
	mmsTransferStatus.iMultimediaMessageStatus =DSATTSYV6_MULTIMEDIA_MESSAGE_STATUS;
	
	sat.MmsTransferStatus(reqStatus, mmsTransferStatusPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
    INFO_PRINTF2(_L("Test %d - RSat::MmsTransferStatus - Multimedia Messaging Service Transfer Status - passed"),iTestCount++);
    
    /**
    Testing Cancel API for the Multimedia Messaging  Service Transfer Status envelope command
    @test TSAT300
    */
    
    //Cancel	
	sat.MmsTransferStatus(reqStatus, mmsTransferStatusPckg);
	sat.CancelAsyncRequest(ESatMmsTransferStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	/**
	Testing Multimedia Messaging  Service Notificaiton Download envelope command which has been newly
	added as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT301
	*/
	RSat::TMmsNotificationDownloadV6 mmsNotificationDownload;
	RSat::TMmsNotificationDownloadV6Pckg	mmsNotificationDownloadPckg(mmsNotificationDownload);

	mmsNotificationDownload.iDeviceId = DSATTSYV6_DEVICE_ID1;
	mmsNotificationDownload.iLastEnvelope = DSATTSYV6_LAST_ENVELOPE1; 
	mmsNotificationDownload.iMMSNotification = DSATTSYV6_MMS_NOTIFICATION1;
	
	sat.MmsNotificationDownload(reqStatus, mmsNotificationDownloadPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
    INFO_PRINTF2(_L("Test %d - RSat::MmsNotificationDownload - Multimedia Messaging  Service Notificaiton Download - passed"),iTestCount++);
    
    /**
    Testing Cancel API for the Multimedia Messaging  Notification Download envelope command
    @test TSAT302
    */
    
    //Cancel	
	sat.MmsNotificationDownload(reqStatus, mmsNotificationDownloadPckg);
	sat.CancelAsyncRequest(ESatMmsNotificationDownload);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
		
	/**
	Testing Ussd Data Download command which has been newly
	added as part of the Release 6 Features of USIM Application ToolKit
	@test TSAT303
	@test TSAT313
	*/
	RSat::TUssdDataDownloadV6 ussdDataDownload;
	RSat::TUssdDataDownloadV6Pckg	ussdDataDownloadPckg(ussdDataDownload);

	ussdDataDownload.iDeviceId = DSATTSYV6_DEVICE_ID1;
	ussdDataDownload.iUssdString.iUssdString = DSATTSY_USSD_STRING;

	sat.UssdDataDownload(reqStatus, ussdDataDownloadPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
    INFO_PRINTF2(_L("Test %d - UssdDataDownload - Ussd Data Download command- passed"),iTestCount++);
    
    /**
    Testing Cancel API for the Ussd Data Download command
    @test TSAT304
    @test TSAT314
    */
    
    //Cancel	
	sat.UssdDataDownload(reqStatus, ussdDataDownloadPckg);
	sat.CancelAsyncRequest(ESatUssdDataDownload);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	
	sat.Close();
	phone.Close();

	return TestStepResult();

	}
