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
// Dummy Sat TSY classes implementation.
// 
//

/**
 @file
*/
#include "sattsy.h"
#include "testdef.h"
#include "Etelsat.h"
#include <et_clsvr.h>
#include "Dsatlog.h"
#include <satcs.h>
#include <utf.h>

#include "coretsy.h"

/**************************************************************************/
//
//	CSatDSatTsy
//
/**************************************************************************/

CSatDSatTsy* CSatDSatTsy::NewL(CPhoneFactoryDummyBase* aFac)
	{
	CSatDSatTsy* subsession=new(ELeave) CSatDSatTsy(aFac);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CSatDSatTsy::Init()
	{}

CSatDSatTsy::CSatDSatTsy(CPhoneFactoryDummyBase* aFac)
	:CSubSessionExtDummyBase(aFac)
	{
	iTsyAsyncReqHandle=0;
	iNotifyCallControlRequest=0;
	iNotifyMoSmControlRequest=0;
	iNotifyDisplayTextPCmd=0;
	iNotifyGetInkeyPCmd=0;
	iNotifyGetInputPCmd=0;
	iNotifyPlayTonePCmd=0;
	iNotifySetUpMenuPCmd=0;
	iNotifySelectItemPCmd=0;
	iNotifySendSmPCmd=0;
	iNotifySendSsPCmd=0;
	iNotifySendUssdPCmd=0;
	iNotifySetUpCallPCmd=0;
	iNotifyRefreshPCmd=0;
	iNotifySetUpEventListPCmd=0;
	iNotifySetUpIdleModeTextPCmd=0;
	iNotifySendDtmfPCmd=0;
	iGetIcon=0;
	iGetImageInstance=0;
	iGetClut=0;
	iDisplayTextTerminalRsp=0;
	iGetInkeyTerminalRsp=0;
	iGetInputTerminalRsp=0;
	iPlayToneTerminalRsp=0;
	iSetUpMenuTerminalRsp=0;
	iSelectItemTerminalRsp=0;
	iSendSmTerminalRsp=0;
	iSendSsTerminalRsp=0;
	iSendUssdTerminalRsp=0;
	iSetUpCallTerminalRsp=0;
	iRefreshTerminalRsp=0;
	iSetUpEventListTerminalRsp=0;
	iSetUpIdleModeTextTerminalRsp=0;
	iSendDtmfTerminalRsp=0;
	iGetMeSideSatProfile=0;
	iClientSatProfileIndication=0;
	iMenuSelection=0;
	iEventDownload=0;
	iNotifyPerformCardApduPCmd=0;
	iNotifyPowerOffCardPCmd=0;
	iNotifyPowerOnCardPCmd=0;
	iNotifyGetReaderStatusPCmd=0;
	iNotifyRunAtCommandPCmd=0;
	iNotifyLanguageNotificationPCmd=0;
	iGetProvisioningRefFile=0;
	iNotifyOpenChannelPCmd=0;
	iNotifyCloseChannelPCmd=0;
	iNotifyReceiveDataPCmd=0;
	iNotifySendDataPCmd=0;
	iNotifyGetChannelStatusPCmd=0;
	iNotifyServiceSearchPCmd=0;
	iNotifyGetServiceInfoPCmd=0;
	iNotifyDeclareServicePCmd=0;
	iNotifyLaunchBrowserPCmd=0;
	iNotifyLaunchBrowserCount=0;
	iNotifySetUpMenuTestNumber=0;
	iNotifyCallControlRequestTestNumber=0;
	iNotifyOpenChannelTestNumber=0;
	iSendMessageNoLogging=0;
	iNotifyProactiveSimSessionEnd=0;
	iNotifyProactiveSimSessionStart=0;
	iNotifyTimerExpiration=0;
	iNotifySmsPpDownload=0;
	iNotifyCbDownload=0;
	iNotifyTimerManagtPCmd=0;
	iNotifyLocalInfoPCmd=0;
	iNotifyPollingIntervalPCmd=0;
	iNotifyPollingOffPCmd=0;
	iTimerMgmtTerminalRsp=0;
	iLocalInfoTerminalRsp=0;
	iPollingIntervalTerminalRsp=0;
	iPollingOffTerminalRsp=0;
	iNotifyTsyStateUpdated=0;
	iNotifyRefreshRequired=0;
	iRefreshAllowed=0;
	iNotifyRetrieveMultimediaMsgPCmd=0;
	iNotifySubmitMultimediaMsgPCmd=0;
	iNotifyDisplayMultimediaMsgPCmd=0;
	iNotifySetFramesPCmd=0;
	iNotifyGetFramesStatusPCmd=0;
	iRetrieveMultimediaMsgTerminalRsp=0;
	iSubmitMultimediaMsgTerminalRsp=0;
	iDisplayMultimediaMsgTerminalRsp=0;
	iSetFramesTerminalRsp=0;
	iGetFramesStatusTerminalRsp=0;
	iMmsNotificationDownload=0;
	iMmsTransferStatus=0;
	iUssdDataDownload=0;
	}

void CSatDSatTsy::ConstructL()
	{
	LOGTEXT(_L8("CSatDSatTsy: Entered constructor"));
	}

CSatDSatTsy::~CSatDSatTsy()
	{
	LOGTEXT(_L8("CSatDSatTsy: Entered destructor"));
	}

CTelObject* CSatDSatTsy::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
* Opening object(s) from RSat is not supported
*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CSatDSatTsy::OpenNewObjectL(TDes& /*aNewName*/)
/**
* Opening object(s) from RSat is not supported	
*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSatDSatTsy::ReqModeL(const TInt aIpc)
/**
* ReqModeL is called from the server's CTelObject::ReqAnalyserL
* in order to check the type of request it has.
*
* The following are example request types for this dummy TSY
* All TSYs do not have to have these request types but they have been given
* "sensible" values in this test code
*/
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case ESatNotifyProactiveSimSessionStart:
		case ESatNotifyProactiveSimSessionEnd:
		case ESatNotifyCallControlRequest:
		case ESatNotifyMoSmControlRequest:
		case ESatNotifyDisplayTextPCmd:
		case ESatNotifyGetInkeyPCmd:
		case ESatNotifyGetInputPCmd:
		case ESatNotifyPlayTonePCmd:
		case ESatNotifySetUpMenuPCmd:
		case ESatNotifySelectItemPCmd:
		case ESatNotifySendSmPCmd:
		case ESatNotifySendSsPCmd:
		case ESatNotifySendUssdPCmd:
		case ESatNotifySetUpCallPCmd:
		case ESatNotifyRefreshPCmd:
		case ESatNotifySetUpEventListPCmd:
		case ESatNotifySetUpIdleModeTextPCmd:
		case ESatNotifySendDtmfPCmd:
		case ESatNotifyPerformCardApduPCmd:
		case ESatNotifyPowerOffCardPCmd:
		case ESatNotifyPowerOnCardPCmd:
		case ESatNotifyGetReaderStatusPCmd:
		case ESatNotifyRunAtCommandPCmd:
		case ESatNotifyLanguageNotificationPCmd:
		case ESatNotifyLaunchBrowserPCmd:
		case ESatNotifyOpenChannelPCmd:
		case ESatNotifyCloseChannelPCmd:
		case ESatNotifyReceiveDataPCmd:
		case ESatNotifySendDataPCmd:
		case ESatNotifyGetChannelStatusPCmd:
		case ESatNotifyServiceSearchPCmd:
		case ESatNotifyGetServiceInfoPCmd:
		case ESatNotifyDeclareServicePCmd:
		case ESatNotifyTimerExpiration:
		case ESatNotifySmsPpDownload:
		case ESatNotifyCbDownload:
		case ESatNotifyTimerMgmtPCmd:							
		case ESatNotifyLocalInfoPCmd:
		case ESatNotifyPollingIntervalPCmd:
		case ESatNotifyPollingOffPCmd:		
		case ESatNotifyTsyStateUpdated:
		case ESatNotifyRefreshRequired:
		case ESatNotifyRefreshRequiredParam:
		case ESatNotifyRetrieveMultimediaMsgPCmd:
		case ESatNotifySubmitMultimediaMsgPCmd:
		case ESatNotifyDisplayMultimediaMsgPCmd:
		case ESatNotifySetFramesPCmd:
		case ESatNotifyGetFramesStatusPCmd:
			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break;

		case ESatGetIcon:
		case ESatGetImageInstance:
		case ESatGetClut:
		case ESatTerminalRsp:
		case ESatGetMeSideSatProfile:
		case ESatClientSatProfileIndication:
		case ESatMenuSelection:
		case ESatEventDownload:
		case ESatUsatClientReadyIndication:
		case ESatGetProvisioningRefFile:
		case ESatSendMessageNoLogging:
		case ESatRefreshAllowed:
		case ETestCustomIPC1:
		case ESatGetOpenChannelPCmd:
		case ESatMmsNotificationDownload:
		case ESatMmsTransferStatus:
		case ESatUssdDataDownload:
			break;

		case ESatNotifyProactiveSimSessionStartCancel:
		case ESatNotifyProactiveSimSessionEndCancel:
		case ESatGetIconCancel:
		case ESatGetImageInstanceCancel:
		case ESatGetClutCancel:
		case ESatTerminalRspCancel:
		case ESatGetMeSideSatProfileCancel:
		case ESatMenuSelectionCancel:
		case ESatNotifyCallControlRequestCancel:
		case ESatNotifyMoSmControlRequestCancel:
		case ESatEventDownloadCancel:
		case ESatNotifyDisplayTextPCmdCancel:
 		case ESatNotifyGetInkeyPCmdCancel:
		case ESatNotifyGetInputPCmdCancel:
		case ESatNotifyPlayTonePCmdCancel:
		case ESatNotifySetUpMenuPCmdCancel:
		case ESatNotifySelectItemPCmdCancel:
		case ESatNotifySendSmPCmdCancel:
		case ESatNotifySendSsPCmdCancel:
		case ESatNotifySendUssdPCmdCancel:
		case ESatNotifySetUpCallPCmdCancel:
		case ESatNotifyRefreshPCmdCancel:
		case ESatNotifySetUpEventListPCmdCancel:
		case ESatNotifySetUpIdleModeTextPCmdCancel:
		case ESatNotifySendDtmfPCmdCancel:
		case ESatGetProvisioningRefFileCancel:
		case ESatNotifyPerformCardApduPCmdCancel:
		case ESatNotifyPowerOffCardPCmdCancel:
		case ESatNotifyPowerOnCardPCmdCancel:
		case ESatNotifyGetReaderStatusPCmdCancel:
		case ESatNotifyRunAtCommandPCmdCancel:
		case ESatNotifyLanguageNotificationPCmdCancel:
		case ESatNotifyLaunchBrowserPCmdCancel:
		case ESatNotifyOpenChannelPCmdCancel:
		case ESatNotifyCloseChannelPCmdCancel:
		case ESatNotifyReceiveDataPCmdCancel:
		case ESatNotifySendDataPCmdCancel:
		case ESatNotifyGetChannelStatusPCmdCancel:
		case ESatNotifyServiceSearchPCmdCancel:
		case ESatNotifyGetServiceInfoPCmdCancel:
		case ESatNotifyDeclareServicePCmdCancel:
		case ESatSendMessageNoLoggingCancel:
		case ESatNotifyTimerExpirationCancel:
		case ESatNotifySmsPpDownloadCancel:
		case ESatNotifyCbDownloadCancel:		
		case ESatNotifyTimerMgmtPCmdCancel:							
		case ESatNotifyLocalInfoPCmdCancel:
		case ESatNotifyPollingIntervalPCmdCancel:
		case ESatNotifyPollingOffPCmdCancel:	
		case ESatNotifyTsyStateUpdatedCancel:
		case ESatNotifyRefreshRequiredCancel:
		case ESatRefreshAllowedCancel:
		case ESatNotifyRefreshRequiredParamCancel:
		case ESatGetOpenChannelPCmdCancel:
		case ESatNotifyRetrieveMultimediaMsgPCmdCancel:
		case ESatNotifySubmitMultimediaMsgPCmdCancel:
		case ESatNotifyDisplayMultimediaMsgPCmdCancel:
		case ESatNotifySetFramesPCmdCancel:
		case ESatNotifyGetFramesStatusPCmdCancel:
		case ESatMmsNotificationDownloadCancel:
		case ESatMmsTransferStatusCancel:
		case ESatUssdDataDownloadCancel:
			// The server should not use ReqMode on Cancel requests
			User::Leave(KErrNotSupported);
			break;
	
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	return ret;
	}

TInt CSatDSatTsy::RegisterNotification(const TInt aIpc)
/**
* RegisterNotification is called when the server recognises that this notification
* is being posted for the first time on this sub-session object.
*
* It enables the TSY to "turn on" any regular notification messages that it may 
* receive from the phone
*/
	{
	switch (aIpc)
		{
		case ESatNotifyProactiveSimSessionStart:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Proactive Sim Session Start"));
			return KErrNone;
		case ESatNotifyProactiveSimSessionEnd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Proactive Sim Session Stop"));
			return KErrNone;
		case ESatNotifyCallControlRequest:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Call Control Request"));
			return KErrNone;
		case ESatNotifyMoSmControlRequest:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - MO SM Control Request"));
			return KErrNone;
		case ESatNotifyDisplayTextPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Display Text PCmd"));
			return KErrNone;
		case ESatNotifyGetInkeyPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Get Inkey PCmd"));
			return KErrNone;
		case ESatNotifyGetInputPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Get Input PCmd"));
			return KErrNone;
		case ESatNotifyPlayTonePCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Play Tone PCmd"));
			return KErrNone;
		case ESatNotifySetUpMenuPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Set Up Menu PCmd"));
			return KErrNone;
		case ESatNotifySelectItemPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Select Item PCmd"));
			return KErrNone;
		case ESatNotifySendSmPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Send SM PCmd"));
			return KErrNone;
		case ESatNotifySendSsPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Send SS PCmd"));
			return KErrNone;
		case ESatNotifySendUssdPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Send USSD PCmd"));
			return KErrNone;
		case ESatNotifySetUpCallPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Set Up Call PCmd"));
			return KErrNone;
		case ESatNotifyRefreshPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Refresh PCmd"));
			return KErrNone;
		case ESatNotifySetUpEventListPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Set Up Event List PCmd"));
			return KErrNone;
		case ESatNotifySetUpIdleModeTextPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Set Up Idle Mode Text PCmd"));
			return KErrNone;
		case ESatNotifySendDtmfPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Send DTMF PCmd"));
			return KErrNone;
		case ESatNotifyPerformCardApduPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Perform Card APDU"));
			return KErrNone;
		case ESatNotifyPowerOffCardPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Power Off Card"));
			return KErrNone;
		case ESatNotifyPowerOnCardPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Power On Card"));
			return KErrNone;
		case ESatNotifyGetReaderStatusPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Get Reader Status"));
			return KErrNone;
		case ESatNotifyRunAtCommandPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Run AT Command"));
			return KErrNone;
		case ESatNotifyLanguageNotificationPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Language Notification"));
			return KErrNone;
		case ESatNotifyLaunchBrowserPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Launch Browser"));
			return KErrNone;
		case ESatNotifyOpenChannelPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Open Channel"));
			return KErrNone;
		case ESatNotifyCloseChannelPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Close Channel"));
			return KErrNone;
		case ESatNotifyReceiveDataPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Receive Data"));
			return KErrNone;
		case ESatNotifySendDataPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Send Data"));
			return KErrNone;
		case ESatNotifyGetChannelStatusPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Get Channel Status"));
			return KErrNone;
		case ESatNotifyServiceSearchPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Service Search"));
			return KErrNone;
		case ESatNotifyGetServiceInfoPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Get Service Info"));
			return KErrNone;
		case ESatNotifyDeclareServicePCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Declare Service"));
			return KErrNone;
		case ESatNotifyTimerExpiration:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Timer expiration"));
			return KErrNone;
		case ESatNotifySmsPpDownload:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Sms PP Dowmload"));
			return KErrNone;
		case ESatNotifyCbDownload:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - CB Download"));
			return KErrNone;
		case ESatNotifyTimerMgmtPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Timer Managt"));
			return KErrNone;
		case ESatNotifyLocalInfoPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Local Info"));
			return KErrNone;
		case ESatNotifyPollingIntervalPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Polling Interval"));
			return KErrNone;
		case ESatNotifyPollingOffPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Polling Pff"));
			return KErrNone;
		case ESatNotifyTsyStateUpdated:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - TSY State Updated"));
			return KErrNone;
		case ESatNotifyRefreshRequired:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Refresh Required"));
			return KErrNone;
		case ESatNotifyRefreshRequiredParam:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Refresh Required Param"));
			return KErrNone;
		case ESatNotifyRetrieveMultimediaMsgPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Retrieve Multimedia Message"));
			return KErrNone;
		case ESatNotifySubmitMultimediaMsgPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Submit Multimedia Message"));
			return KErrNone;
		case ESatNotifyDisplayMultimediaMsgPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Display Multimedia Message"));
			return KErrNone;
		case  ESatNotifySetFramesPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Set Frames"));
			return KErrNone;
		case  ESatNotifyGetFramesStatusPCmd:
			LOGTEXT(_L8("CSatDSatTsy: RegisterNotification - Get Frames Status"));
			return KErrNone;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CSatDSatTsy: Register error, unknown IPC"));
			return KErrNotSupported;
		}
	}

TInt CSatDSatTsy::DeregisterNotification(const TInt aIpc)
/**
* DeregisterNotification is called when the server recognises that this notification
* will not be posted again because the last client to have a handle on this sub-session
* object has just closed the handle.
*
* It enables the TSY to "turn off" any regular notification messages that it may 
* receive from the phone
*/
	{
	switch (aIpc)
		{
		case ESatNotifyProactiveSimSessionStart:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Proactive Sim Session Start"));
			return KErrNone;
		case ESatNotifyProactiveSimSessionEnd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Proactive Sim Session Stop"));
			return KErrNone;
		case ESatNotifyCallControlRequest:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Call Control Request"));
			return KErrNone;
		case ESatNotifyMoSmControlRequest:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - MO SM Control Request"));
			return KErrNone;
		case ESatNotifyDisplayTextPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Display Text PCmd"));
			return KErrNone;
		case ESatNotifyGetInkeyPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Get Inkey PCmd"));
			return KErrNone;
		case ESatNotifyGetInputPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Get Input PCmd"));
			return KErrNone;
		case ESatNotifyPlayTonePCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Play Tone PCmd"));
			return KErrNone;
		case ESatNotifySetUpMenuPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Set Up Menu PCmd"));
			return KErrNone;
		case ESatNotifySelectItemPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Select Item PCmd"));
			return KErrNone;
		case ESatNotifySendSmPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Send SM PCmd"));
			return KErrNone;
		case ESatNotifySendSsPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Send SS PCmd"));
			return KErrNone;
		case ESatNotifySendUssdPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Send USSD PCmd"));
			return KErrNone;
		case ESatNotifySetUpCallPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Set Up Call PCmd"));
			return KErrNone;
		case ESatNotifyRefreshPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Refresh PCmd"));
			return KErrNone;
		case ESatNotifySetUpEventListPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Set Up Event List PCmd"));
			return KErrNone;
		case ESatNotifySetUpIdleModeTextPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Set Up Idle Mode Text PCmd"));
			return KErrNone;
		case ESatNotifySendDtmfPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Send DTMF PCmd"));
			return KErrNone;
		case ESatNotifyPerformCardApduPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Perform Card APDU"));
			return KErrNone;
		case ESatNotifyPowerOffCardPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Power Off Card"));
			return KErrNone;
		case ESatNotifyPowerOnCardPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Power On Card"));
			return KErrNone;
		case ESatNotifyGetReaderStatusPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Get Reader Status"));
			return KErrNone;
		case ESatNotifyRunAtCommandPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Run AT Command"));
			return KErrNone;
		case ESatNotifyLanguageNotificationPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Language Notification"));
			return KErrNone;
		case ESatNotifyLaunchBrowserPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Launch Browser"));
			return KErrNone;
		case ESatNotifyOpenChannelPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Open Channel"));
			return KErrNone;
		case ESatNotifyCloseChannelPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Close Channel"));
			return KErrNone;
		case ESatNotifyReceiveDataPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Receive Data"));
			return KErrNone;
		case ESatNotifySendDataPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Send Data"));
			return KErrNone;
		case ESatNotifyGetChannelStatusPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Get Channel Status"));
			return KErrNone;
		case ESatNotifyServiceSearchPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Service Search"));
			return KErrNone;
		case ESatNotifyGetServiceInfoPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Get Service Info"));
			return KErrNone;
		case ESatNotifyDeclareServicePCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Declare Service"));
			return KErrNone;		
		case ESatNotifyTimerExpiration:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Timer expiration"));
			return KErrNone;
		case ESatNotifySmsPpDownload:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Sms PP Dowmload"));
			return KErrNone;
		case ESatNotifyCbDownload:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - CB Download"));
			return KErrNone;
		case ESatNotifyTimerMgmtPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Timer Managt"));
			return KErrNone;
		case ESatNotifyLocalInfoPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Local Info"));
			return KErrNone;
		case ESatNotifyPollingIntervalPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Polling Interval"));
			return KErrNone;
		case ESatNotifyPollingOffPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Polling Pff"));
			return KErrNone;
		case ESatNotifyTsyStateUpdated:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - TSY State Updated"));
			return KErrNone;
		case ESatNotifyRefreshRequired:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Refresh Required"));
			return KErrNone;
		case ESatNotifyRefreshRequiredParam:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Refresh Required Param"));
			return KErrNone;
		case ESatNotifyRetrieveMultimediaMsgPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Retrieve Multimedia Message"));
			return KErrNone;
		case ESatNotifySubmitMultimediaMsgPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Submit Multimedia Message"));
			return KErrNone;
		case ESatNotifyDisplayMultimediaMsgPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Display Multimedia Message"));
			return KErrNone;
		case  ESatNotifySetFramesPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Set Frames"));
			return KErrNone;
		case  ESatNotifyGetFramesStatusPCmd:
			LOGTEXT(_L8("CSatDSatTsy: De-registerNotification - Get Frames Status"));
			return KErrNone;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CSatDSatTsy: Deregister error, unknown IPC"));
			return KErrNotSupported;
		}
	}

TInt CSatDSatTsy::NumberOfSlotsL(const TInt aIpc)
/**
* NumberOfSlotsL is called by the server when it is registering a new notification
* It enables the TSY to tell the server how many buffer slots to allocate for
* "repost immediately" notifications that may trigger before clients collect them
*/
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
		case ESatNotifyProactiveSimSessionStart:
		case ESatNotifyProactiveSimSessionEnd:
			LOGTEXT(_L8("CSatDSatTsy: Registered with 1 slot"));
			numberOfSlots=1;
			break;

		case ESatNotifyCallControlRequest:
		case ESatNotifyMoSmControlRequest:
		case ESatNotifyDisplayTextPCmd:
		case ESatNotifyGetInkeyPCmd:
		case ESatNotifyGetInputPCmd:
		case ESatNotifyPlayTonePCmd:
		case ESatNotifySetUpMenuPCmd:
		case ESatNotifySelectItemPCmd:
		case ESatNotifySendSmPCmd:
		case ESatNotifySendSsPCmd:
		case ESatNotifySendUssdPCmd:
		case ESatNotifySetUpCallPCmd:
		case ESatNotifyRefreshPCmd:
		case ESatNotifySetUpEventListPCmd:
		case ESatNotifySetUpIdleModeTextPCmd:
		case ESatNotifySendDtmfPCmd:
		case ESatNotifyPerformCardApduPCmd:
		case ESatNotifyPowerOffCardPCmd:
		case ESatNotifyPowerOnCardPCmd:
		case ESatNotifyGetReaderStatusPCmd:
		case ESatNotifyRunAtCommandPCmd:
		case ESatNotifyLanguageNotificationPCmd:
		case ESatNotifyLaunchBrowserPCmd:
		case ESatNotifyOpenChannelPCmd:
		case ESatNotifyCloseChannelPCmd:
		case ESatNotifyReceiveDataPCmd:
		case ESatNotifySendDataPCmd:
		case ESatNotifyGetChannelStatusPCmd:
		case ESatNotifyServiceSearchPCmd:
		case ESatNotifyGetServiceInfoPCmd:
		case ESatNotifyDeclareServicePCmd:
		case ESatNotifyTimerExpiration:
		case ESatNotifySmsPpDownload:
		case ESatNotifyCbDownload:		
		case ESatNotifyTimerMgmtPCmd:							
		case ESatNotifyLocalInfoPCmd:
		case ESatNotifyPollingIntervalPCmd:
		case ESatNotifyPollingOffPCmd:
		case ESatNotifyTsyStateUpdated:
		case ESatNotifyRefreshRequired:
		case ESatNotifyRefreshRequiredParam:
		case ESatNotifyRetrieveMultimediaMsgPCmd:							
		case ESatNotifySubmitMultimediaMsgPCmd:
		case ESatNotifyDisplayMultimediaMsgPCmd:
		case ESatNotifySetFramesPCmd :
		case ESatNotifyGetFramesStatusPCmd: 		
			LOGTEXT(_L8("CSatDSatTsy: Registered with 2 slot"));
			numberOfSlots=2;
			break;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CSatDSatTsy: Number of Slots error, unknown IPC"));
			User::Leave(KErrNotSupported);
			break;
		}  
	return numberOfSlots;
	}


TInt CSatDSatTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
/**
* ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
* for the TSY to process
* A request handle, request type and request data are passed to the TSY
*/
	{
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
		case ESatNotifyProactiveSimSessionStart:
			return NotifyProactiveSimSessionStart(aTsyReqHandle);
		case ESatNotifyProactiveSimSessionEnd:
			return NotifyProactiveSimSessionEnd(aTsyReqHandle);
		case ESatNotifyCallControlRequest:
			return NotifyCallControlRequest(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyMoSmControlRequest:
			return NotifyMoSmControlRequest(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyDisplayTextPCmd:
			return NotifyDisplayTextPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyGetInkeyPCmd:
			return NotifyGetInkeyPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyGetInputPCmd:
			return NotifyGetInputPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyPlayTonePCmd:
			return NotifyPlayTonePCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySetUpMenuPCmd:
			return NotifySetUpMenuPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySelectItemPCmd:
			return NotifySelectItemPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySendSmPCmd:
			return NotifySendSmPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySendSsPCmd:
			return NotifySendSsPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySendUssdPCmd:
			return NotifySendUssdPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySetUpCallPCmd:
			return NotifySetUpCallPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyRefreshPCmd:
			return NotifyRefreshPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySetUpEventListPCmd:
			return NotifySetUpEventListPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySetUpIdleModeTextPCmd:
			return NotifySetUpIdleModeTextPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySendDtmfPCmd:
			return NotifySendDtmfPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatGetIcon:
			return GetIcon(aTsyReqHandle,
			REINTERPRET_CAST(TUint8*, dataPtr),
			REINTERPRET_CAST(RSat::TIcon*, dataPtr2));
		case ESatGetImageInstance:
			return GetImageInstance(aTsyReqHandle,
			aPackage.Des1n(),
			aPackage.Des2n());
		case ESatGetClut:
			return GetClut(aTsyReqHandle,
			REINTERPRET_CAST(RSat::TInstanceNumberAndOffset*, dataPtr),
			aPackage.Des2n());
		case ESatTerminalRsp:
			return TerminalRsp(aTsyReqHandle,
				REINTERPRET_CAST(RSat::TPCmd*, dataPtr),aPackage.Des2n());
		case ESatGetMeSideSatProfile:
			return GetMeSideSatProfile(aTsyReqHandle, aPackage.Des1n());
		case ESatClientSatProfileIndication:
			return ClientSatProfileIndication(aTsyReqHandle, aPackage.Des1n());
		case ESatMenuSelection:
			return MenuSelection(aTsyReqHandle, aPackage.Des1n());
		case ESatEventDownload:
			return EventDownload(aTsyReqHandle, 
				REINTERPRET_CAST(RSat::TEventList*, dataPtr),
				aPackage.Des2n());
		case ESatRefreshAllowed:
			return RefreshAllowed(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyPerformCardApduPCmd:
			return NotifyPerformCardApduPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyPowerOffCardPCmd:
			return NotifyPowerOffCardPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyPowerOnCardPCmd:
			return NotifyPowerOnCardPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyGetReaderStatusPCmd:
			return NotifyGetReaderStatusPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyRunAtCommandPCmd:
			return NotifyRunAtCommandPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyLanguageNotificationPCmd:
			return NotifyLanguageNotificationPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyLaunchBrowserPCmd:
			return NotifyLaunchBrowserPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyOpenChannelPCmd:
			return NotifyOpenChannelPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatGetOpenChannelPCmd:
			return GetOpenChannelPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyCloseChannelPCmd:
			return NotifyCloseChannelPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyReceiveDataPCmd:
			return NotifyReceiveDataPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySendDataPCmd:
			return NotifySendDataPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyGetChannelStatusPCmd:
			return NotifyGetChannelStatusPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyServiceSearchPCmd:
			return NotifyServiceSearchPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyGetServiceInfoPCmd:
			return NotifyGetServiceInfoPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyDeclareServicePCmd:
			return NotifyDeclareServicePCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatUsatClientReadyIndication:
			return UsatClientReadyIndication(aTsyReqHandle);
		case ESatGetProvisioningRefFile:
			return GetProvisioningRefFile(aTsyReqHandle, 
				REINTERPRET_CAST(RSat::TProvisioningFileRef*, dataPtr),
				aPackage.Des2n());
		case ESatSendMessageNoLogging:
			return SendMessageNoLogging(aTsyReqHandle, 
				aPackage.Des1n(),
				REINTERPRET_CAST(TUint16*, dataPtr2));				
		case ESatNotifyTimerExpiration:
			return NotifyTimerExpiration(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySmsPpDownload:
			return NotifySmsPpDownload(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyCbDownload:
			return NotifyCbDownload(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyTimerMgmtPCmd:
			return NotifyTimerManagtPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyLocalInfoPCmd:
			return NotifyLocalInfoPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyPollingIntervalPCmd:
			return NotifyPollingIntervalPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyPollingOffPCmd:
			return NotifyPollingOffPCmd(aTsyReqHandle, aPackage.Des1n());			
		case ESatNotifyTsyStateUpdated:
			return NotifyTsyStateUpdated(aTsyReqHandle, 
				REINTERPRET_CAST(RSat::TPCmd*, dataPtr));
		case ESatNotifyRefreshRequired:
			return NotifyRefreshRequired(aTsyReqHandle);
		case ESatNotifyRefreshRequiredParam:
			return NotifyRefreshRequired(aTsyReqHandle, aPackage.Des1n());
		case ETestCustomIPC1:
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
		case ESatNotifyRetrieveMultimediaMsgPCmd:
			return NotifyRetrieveMultimediaMsgPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifySubmitMultimediaMsgPCmd:
			return NotifySubmitMultimediaMsgPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatNotifyDisplayMultimediaMsgPCmd:
			return NotifyDisplayMultimediaMsgPCmd(aTsyReqHandle, aPackage.Des1n());
		case  ESatNotifySetFramesPCmd:
			return NotifySetFramesPCmd(aTsyReqHandle, aPackage.Des1n());			
		case  ESatNotifyGetFramesStatusPCmd:
			return NotifyGetFramesStatusPCmd(aTsyReqHandle, aPackage.Des1n());
		case ESatMmsNotificationDownload:
			return MmsNotificationDownload(aTsyReqHandle, aPackage.Des1n());
		case ESatMmsTransferStatus:
			return MmsTransferStatus(aTsyReqHandle, aPackage.Des1n());		
		case ESatUssdDataDownload :
			return UssdDataDownload(aTsyReqHandle, aPackage.Des1n());
		default:
			return KErrNotSupported;
		}
	}

TInt CSatDSatTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
* CancelService is called by the server when it is "cleaning-up" any still outstanding
* asynchronous requests before closing a client's sub-session.
*
* CancelService is also called when individual cancel methods are called by the client,
* who wishes to cancel an outstanding asynchronous request.
*
* This will happen if a client closes its R-class handle without cancelling outstanding
* asynchronous requests.
*/
	{
	LOGTEXT(_L8("CSatDSatTsy: - CancelService called"));
	switch (aIpc)
		{
		case ESatNotifyProactiveSimSessionEnd:
			return NotifyProactiveSimSessionEndCancel(aTsyReqHandle);
		case ESatNotifyProactiveSimSessionStart:
			return NotifyProactiveSimSessionStartCancel(aTsyReqHandle);
		case ESatGetIcon:
			return GetIconCancel(aTsyReqHandle);
		case ESatGetImageInstance:
			return GetImageInstanceCancel(aTsyReqHandle);
		case ESatGetClut:
			return GetClutCancel(aTsyReqHandle);
		case ESatTerminalRsp:
			return TerminalRspCancel(aTsyReqHandle);
		case ESatGetMeSideSatProfile:
			return GetMeSideSatProfileCancel(aTsyReqHandle);
		case ESatMenuSelection:
			return MenuSelectionCancel(aTsyReqHandle);
		case ESatRefreshAllowed:
			return RefreshAllowedCancel(aTsyReqHandle);
		case ESatNotifyCallControlRequest:
			return NotifyCallControlRequestCancel(aTsyReqHandle);
		case ESatNotifyMoSmControlRequest:
			return NotifyMoSmControlRequestCancel(aTsyReqHandle);
		case ESatEventDownload:
			return EventDownloadCancel(aTsyReqHandle);
		case ESatNotifyDisplayTextPCmd:
			return NotifyDisplayTextPCmdCancel(aTsyReqHandle);
 		case ESatNotifyGetInkeyPCmd:
			return NotifyGetInkeyPCmdCancel(aTsyReqHandle);
		case ESatNotifyGetInputPCmd:
			return NotifyGetInputPCmdCancel(aTsyReqHandle);
		case ESatNotifyPlayTonePCmd:
			return NotifyPlayTonePCmdCancel(aTsyReqHandle);
		case ESatNotifySetUpMenuPCmd:
			return NotifySetUpMenuPCmdCancel(aTsyReqHandle);
		case ESatNotifySelectItemPCmd:
			return NotifySelectItemPCmdCancel(aTsyReqHandle);
		case ESatNotifySendSmPCmd:
			return NotifySendSmPCmdCancel(aTsyReqHandle);
		case ESatNotifySendSsPCmd:
			return NotifySendSsPCmdCancel(aTsyReqHandle);
		case ESatNotifySendUssdPCmd:
			return NotifySendUssdPCmdCancel(aTsyReqHandle);
		case ESatNotifySetUpCallPCmd:
			return NotifySetUpCallPCmdCancel(aTsyReqHandle);
		case ESatNotifyRefreshPCmd:
			return NotifyRefreshPCmdCancel(aTsyReqHandle);
		case ESatNotifySetUpEventListPCmd:
			return NotifySetUpEventListPCmdCancel(aTsyReqHandle);
		case ESatNotifySetUpIdleModeTextPCmd:
			return NotifySetUpIdleModeTextPCmdCancel(aTsyReqHandle);
		case ESatNotifySendDtmfPCmd:
			return NotifySendDtmfPCmdCancel(aTsyReqHandle);
		case ESatNotifyPerformCardApduPCmd:
			return NotifyPerformCardApduPCmdCancel(aTsyReqHandle);
		case ESatNotifyPowerOffCardPCmd:
			return NotifyPowerOffCardPCmdCancel(aTsyReqHandle);
		case ESatNotifyPowerOnCardPCmd:
			return NotifyPowerOnCardPCmdCancel(aTsyReqHandle);
		case ESatNotifyGetReaderStatusPCmd:
			return NotifyGetReaderStatusPCmdCancel(aTsyReqHandle);
		case ESatNotifyRunAtCommandPCmd:
			return NotifyRunAtCommandPCmdCancel(aTsyReqHandle);
		case ESatNotifyLanguageNotificationPCmd:
			return NotifyLanguageNotificationPCmdCancel(aTsyReqHandle);
		case ESatNotifyLaunchBrowserPCmd:
			return NotifyLaunchBrowserPCmdCancel(aTsyReqHandle);
		case ESatNotifyOpenChannelPCmd:
			return NotifyOpenChannelPCmdCancel(aTsyReqHandle);
		case ESatGetOpenChannelPCmd:
			return GetOpenChannelPCmdCancel(aTsyReqHandle);			
		case ESatNotifyCloseChannelPCmd:
			return NotifyCloseChannelPCmdCancel(aTsyReqHandle);
		case ESatNotifyReceiveDataPCmd:
			return NotifyReceiveDataPCmdCancel(aTsyReqHandle);
		case ESatNotifySendDataPCmd:
			return NotifySendDataPCmdCancel(aTsyReqHandle);
		case ESatNotifyGetChannelStatusPCmd:
			return NotifyGetChannelStatusPCmdCancel(aTsyReqHandle);
		case ESatNotifyServiceSearchPCmd:
			return NotifyServiceSearchPCmdCancel(aTsyReqHandle);
		case ESatNotifyGetServiceInfoPCmd:
			return NotifyGetServiceInfoPCmdCancel(aTsyReqHandle);
		case ESatNotifyDeclareServicePCmd:
			return NotifyDeclareServicePCmdCancel(aTsyReqHandle);
		case ESatGetProvisioningRefFile:
			return GetProvisioningRefFileCancel(aTsyReqHandle);
		case ESatSendMessageNoLogging:
			return SendMessageNoLoggingCancel(aTsyReqHandle);
		case ESatNotifyTimerExpiration:
			return NotifyTimerExpirationCancel(aTsyReqHandle);
		case ESatNotifySmsPpDownload:
			return NotifySmsPpDownloadCancel(aTsyReqHandle);
		case ESatNotifyCbDownload:
			return NotifyCbDownloadCancel(aTsyReqHandle);
		case ESatNotifyTimerMgmtPCmd:
			return NotifyTimerManagtPCmdCancel(aTsyReqHandle);
		case ESatNotifyLocalInfoPCmd:
			return NotifyLocalInfoPCmdCancel(aTsyReqHandle);
		case ESatNotifyPollingIntervalPCmd:
			return NotifyPollingIntervalPCmdCancel(aTsyReqHandle);
		case ESatNotifyPollingOffPCmd:
			return NotifyPollingOffPCmdCancel(aTsyReqHandle);
		case ESatNotifyTsyStateUpdated:
			return NotifyTsyStateUpdatedCancel(aTsyReqHandle);
		case ESatNotifyRefreshRequired:
			return NotifyRefreshRequiredCancel(aTsyReqHandle);
		case ESatNotifyRefreshRequiredParam:
			return NotifyRefreshRequiredCancel(aTsyReqHandle);
		case ESatNotifyRetrieveMultimediaMsgPCmd:
			return NotifyRetrieveMultimediaMsgPCmdCancel(aTsyReqHandle);
		case ESatNotifySubmitMultimediaMsgPCmd:
			return NotifySubmitMultimediaMsgPCmdCancel(aTsyReqHandle);
		case ESatNotifyDisplayMultimediaMsgPCmd:
			return NotifyDisplayMultimediaMsgPCmdCancel(aTsyReqHandle);
		case  ESatNotifySetFramesPCmd:
			return NotifySetFramesPCmdCancel(aTsyReqHandle);
		case  ESatNotifyGetFramesStatusPCmd:
			return NotifyGetFramesStatusPCmdCancel(aTsyReqHandle);
		case  ESatMmsNotificationDownload:
			return MmsNotificationDownloadCancel(aTsyReqHandle);
		case ESatMmsTransferStatus:
			return MmsTransferStatusCancel(aTsyReqHandle);
		case ESatUssdDataDownload:
			return UssdDataDownloadCancel(aTsyReqHandle);
		default:
			return KErrGeneral; 
		} 
	}


/***********************************************************************************/
//
// The following methods are called from ExtFunc and/or CancelService.
// Each of these will process a TSY request or cancel a TSY request
// Here, example values are returned or checked within this dummy TSY in order to ensure
// that the integrity of the data passed to/from client is maintained
//
/***********************************************************************************/

TInt CSatDSatTsy::GetIcon(const TTsyReqHandle aTsyReqHandle, TUint8* aRecordNumber, RSat::TIcon* aIconEf)
	{
	if((iGetIcon==0)||(iGetIcon==1))
		{
		iGetIcon++;
		LOGTEXT(_L8("CSatDSatTsy::GetIcon called"));
		switch(*aRecordNumber)
			{
			case 1:
				*aIconEf=DSATTSY_ICON_EF1;
				break;
			case 2:
				*aIconEf=DSATTSY_ICON_EF2;
				break;
			default:
				*aIconEf=DSATTSY_ICON_EF1;
				break;
			}
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::GetIconCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::GetIconCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;	
	}

TInt CSatDSatTsy::GetImageInstance(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDes8* aInstance)
/**
* The first time that this method is called, iGetImageInstance=0
* The request completes - ReqCompleted is called. One of the test cases for 
* aInstanceNumber can be tested.
*
* The second time that this method is called, iGetImageInstance=1
* The request completes - ReqCompleted is called. Another one of the test cases
* for aInstanceNumber can be tested.
* 
* The third time that this method is called, iGetImageInstance=2
* The request does not complete.
*
* This mechanism prevents an infinite loop as each time a notification completes,
* it is reposted automatically by the server, which would in this test code create an
* infinite loop of posting, completing and reposting the notification.
*
* This mechanism also allows the test client to test the cancel method associated.
*
* If parameters become corrupted in either of the two tests then 'KErrCorrupt' 
* and an unknown image instance returned.
*/
	{
	LOGTEXT(_L8("CSatDSatTsy::GetImageInstance called"));
	
	TInt result=KErrNone;
	
	RSat::TInstanceInfoV3Pckg* paramsPckgV3 = (RSat::TInstanceInfoV3Pckg*)aCallParams;
	RSat::TInstanceInfoV3& paramsV3 = (*paramsPckgV3)();

	if((iGetImageInstance==0)||(iGetImageInstance==1))
		{
		iGetImageInstance++;
		
		if(paramsV3.ExtensionId()== RSat::KSatV3)
			{
			// check that all parameters are being passed correctly
			if( paramsV3.iInstanceNumber == DSATTSY_INSTANCE_NUMBER1 &&
				paramsV3.iLength==DSATTSY_INSTANCE_LENGTH1 && 
				paramsV3.iOffset==DSATTSY_INSTANCE_OFFSET1 &&	
				paramsV3.iCoding==DSATTSY_INSTANCE_CODING_BASIC &&
				paramsV3.iImgWidth==DSATTSY_INSTANCE_WIDTH_1 &&
				paramsV3.iImgHeight==DSATTSY_INSTANCE_HEIGHT_1 &&
				paramsV3.iFileId==DSATTSY_INSTANCE_FILE_ID_1 ) 
				{
				*aInstance=DSATTSY_IMG_INSTANCE1;
				}
			else if(paramsV3.iInstanceNumber == DSATTSY_INSTANCE_NUMBER2 &&
					paramsV3.iLength==DSATTSY_INSTANCE_LENGTH2 && 
					paramsV3.iOffset==DSATTSY_INSTANCE_OFFSET2 &&
					paramsV3.iCoding==DSATTSY_INSTANCE_CODING_COLOUR &&
					paramsV3.iImgWidth==DSATTSY_INSTANCE_WIDTH_2 &&
					paramsV3.iImgHeight==DSATTSY_INSTANCE_HEIGHT_2 &&
					paramsV3.iFileId==DSATTSY_INSTANCE_FILE_ID_2 )
							{
				*aInstance=DSATTSY_IMG_INSTANCE2;
				}
			else
				{
				// parameters have been corrupted
				*aInstance=DSATTSY_IMG_INSTANCE_UNKNOWN;
				result=KErrCorrupt;
				}
			}				
		ReqCompleted(aTsyReqHandle,result);
		}

	iTsyAsyncReqHandle = aTsyReqHandle;
	return result;
	}


TInt CSatDSatTsy::GetImageInstanceCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::GetImageInstanceCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;	
	}

TInt CSatDSatTsy::GetClut(const TTsyReqHandle aTsyReqHandle, RSat::TInstanceNumberAndOffset* aClutInfo, TDes8* aClut)
	{
	LOGTEXT(_L8("CSatDSatTsy::GetClut called"));

	if((aClutInfo->iInstanceNumber!=DSATTSY_INSTANCE_NUMBER1)
			||(aClutInfo->iOffset!=DSATTSY_CLUT_OFFSET))
				ReqCompleted(aTsyReqHandle,KErrCorrupt);

	if(iGetClut==0)
		{
		iGetClut++;
		*aClut= DSATTSY_CLUT1;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::GetClutCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::GetClutCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;	
	}

TInt CSatDSatTsy::DisplayTextTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TDisplayTextRspV1Pckg* aRspPckg = (RSat::TDisplayTextRspV1Pckg*)aRsp;
	RSat::TDisplayTextRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.PCmdNumber()!=DSATTSY_DISPLAY_TEXT_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iDisplayTextTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::GetInkeyTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TGetInkeyRspV1Pckg* aRspPckg = (RSat::TGetInkeyRspV1Pckg*)aRsp;
	RSat::TGetInkeyRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_TEXT_STRING)
		||(rspV1.iRspFormat!=DSATTSY_USER_RSP_FORMAT)
		||(rspV1.iAdditionalInfo!=DSATTSY_USER_TEXT_INPUT)
		||(rspV1.PCmdNumber()!=DSATTSY_GET_INKEY_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iGetInkeyTerminalRsp++)
		{
		if(rspV1.ExtensionId()==RSat::KSatV2)
			{
			RSat::TGetInkeyRspV2Pckg* aV2RspPckg = (RSat::TGetInkeyRspV2Pckg*)aRsp;
			RSat::TGetInkeyRspV2& rspV2 = (*aV2RspPckg)();
			
			if((rspV2.iDuration.iTimeUnit!=DSATTSY_TIME_UNIT)
				|| (rspV2.iDuration.iNumOfUnits!=DSATTSY_NUM_OF_UNITS))
				{
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else
			ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::GetInputTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TGetInputRspV1Pckg* aRspPckg = (RSat::TGetInputRspV1Pckg*)aRsp;
	RSat::TGetInputRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_TEXT_STRING)
		||(rspV1.iRspFormat!=DSATTSY_GET_INPUT_RSP_FORMAT)
		||(rspV1.iAdditionalInfo!=DSATTSY_USER_TEXT_INPUT)
		||(rspV1.PCmdNumber()!=DSATTSY_GET_INPUT_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iGetInputTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::PlayToneTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TPlayToneRspV1Pckg* aRspPckg = (RSat::TPlayToneRspV1Pckg*)aRsp;
	RSat::TPlayToneRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV1.PCmdNumber()!=DSATTSY_PLAY_TONE_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iPlayToneTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}	

TInt CSatDSatTsy::SetUpMenuRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSetUpMenuRspV1Pckg* aRspPckg = (RSat::TSetUpMenuRspV1Pckg*)aRsp;
	RSat::TSetUpMenuRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV1.PCmdNumber()!=DSATTSY_SET_UP_MENU_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iSetUpMenuTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::SelectItemTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSelectItemRspV1Pckg* aRspPckg = (RSat::TSelectItemRspV1Pckg*)aRsp;
	RSat::TSelectItemRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_ITEM_ID)
		||(rspV1.iAdditionalInfo!=DSATTSY_RSP_ITEM_ID)
		||(rspV1.PCmdNumber()!=DSATTSY_SELECT_ITEM_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iSelectItemTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::SendSmTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSendSmRspV1Pckg* aRspPckg = (RSat::TSendSmRspV1Pckg*)aRsp;
	RSat::TSendSmRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV1.PCmdNumber()!=DSATTSY_SEND_SM_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iSendSmTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::SendSSTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSendSsRspV1Pckg* aRspPckg = (RSat::TSendSsRspV1Pckg*)aRsp;
	RSat::TSendSsRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV1.PCmdNumber()!=DSATTSY_SEND_SS_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else 
		{
		RSat::TControlResult ccResult;
		rspV1.iCcRequestedAction.GetCcGeneralResult(ccResult);

		RSat::TCallControlType ccType;
		rspV1.iCcRequestedAction.GetCallControlType(ccType);

		RSat::TCallSetUpParams address;
		rspV1.iCcRequestedAction.GetCallSetUpDetails(address);

		RSat::TAlphaIdValidity validity;
		RSat::TAlphaId alphaId;
		rspV1.iCcRequestedAction.GetAlphaId(validity, alphaId);

		RSat::TBCRepeatIndicator indicator;
		rspV1.iCcRequestedAction.GetBCRepeatIndicator(indicator);

		if ( (ccResult!=DSATTSY_CC_RESULT) || (ccType!=RSat::ECcAddress) 
		|| (address.iCcp1!=DSATTSY_CCP1) 
		||(address.iSubAddress!=DSATTSY_SUBADDRESS)
		|| (address.iCcp2!=DSATTSY_CCP2) 
		|| (address.iAddress.iTypeOfNumber!=DSATTSY_TON)
		|| (address.iAddress.iNumberPlan!=DSATTSY_NPI) 
		|| (address.iAddress.iTelNumber!=DSATTSY_TEL_NUMBER) 
		|| (validity!=DSATTSY_NO_ALPHAID)
		|| (alphaId.iAlphaId!=DSATTSY_NULL_BUF)
		|| (alphaId.iStatus!=DSATTSY_ALPHAID_STATUS)
		|| (indicator != DSATTSY_BC_REPEAT_INDICATOR_SEQUENTIAL))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}

		else if((rspV1.iGeneralResult2!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType2!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo2!=DSATTSY_NULL_BUF))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		
		else if(!iSendSsTerminalRsp++)
			{
			if(rspV1.ExtensionId()==RSat::KSatV2)
				{
				RSat::TSendSsRspV2Pckg* aV2RspPckg = (RSat::TSendSsRspV2Pckg*)aRsp;
				RSat::TSendSsRspV2& rspV2 = (*aV2RspPckg)();
			
				if((rspV2.iUssdString.iUssdString != DSATTSY_DEFAULT_TEXT)
				|| (rspV2.iUssdString.iDcs != DSATTSY_DCS))	
					ReqCompleted(aTsyReqHandle,KErrCorrupt);
				else
					ReqCompleted(aTsyReqHandle,KErrNone);
				}
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::SendUssdTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSendUssdRspV1Pckg* aRspPckg = (RSat::TSendUssdRspV1Pckg*)aRsp;
	RSat::TSendUssdRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_TEXT_STRING)
		||(rspV1.iAdditionalInfo!=DSATTSY_NETWORK_TEXT_STRING)
		||(rspV1.PCmdNumber()!=DSATTSY_SEND_USSD_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		RSat::TControlResult ccResult;
		rspV1.iCcRequestedAction.GetCcGeneralResult(ccResult);
		RSat::TCallControlType ccType;
		rspV1.iCcRequestedAction.GetCallControlType(ccType);

		RSat::TCallSetUpParams address;
		rspV1.iCcRequestedAction.GetCallSetUpDetails(address);

		RSat::TAlphaIdValidity validity;
		RSat::TAlphaId alphaId;
		rspV1.iCcRequestedAction.GetAlphaId(validity, alphaId);

		RSat::TBCRepeatIndicator indicator;
		rspV1.iCcRequestedAction.GetBCRepeatIndicator(indicator);

		if ( (ccResult!=DSATTSY_CC_RESULT) || (ccType!=RSat::ECcAddress) 
			|| (address.iCcp1!=DSATTSY_CCP1) 
			||(address.iSubAddress!=DSATTSY_SUBADDRESS)
			|| (address.iCcp2!=DSATTSY_CCP2) 
			|| (address.iAddress.iTypeOfNumber!=DSATTSY_TON)
			|| (address.iAddress.iNumberPlan!=DSATTSY_NPI) 
			|| (address.iAddress.iTelNumber!=DSATTSY_TEL_NUMBER) 
			|| (validity!=DSATTSY_NO_ALPHAID)
			|| (alphaId.iAlphaId!=DSATTSY_NULL_BUF)
			|| (alphaId.iStatus!=DSATTSY_ALPHAID_STATUS
			|| indicator != DSATTSY_BC_REPEAT_INDICATOR_SEQUENTIAL))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}

		else if((rspV1.iGeneralResult2!=DSATTSY_PCMD_RESULT_SUCCESS)
			||(rspV1.iInfoType2!=DSATTSY_PCMD_RSP_NO_INFO)
			||(rspV1.iAdditionalInfo2!=DSATTSY_NULL_BUF))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}

		else if(!iSendUssdTerminalRsp++)
			ReqCompleted(aTsyReqHandle,KErrNone);
		}
		iTsyAsyncReqHandle = aTsyReqHandle;
		return KErrNone;
	}

TInt CSatDSatTsy::SetUpCallTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSetUpCallRspV1Pckg* aRspPckg = (RSat::TSetUpCallRspV1Pckg*)aRsp;
	RSat::TSetUpCallRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV1.PCmdNumber()!=DSATTSY_SET_UP_CALL_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else
		{
		if(rspV1.ExtensionId()==RSat::KSatV2)
			{
			RSat::TSetUpCallRspV2Pckg* aRspV2Pckg = (RSat::TSetUpCallRspV2Pckg*)aRsp;
			RSat::TSetUpCallRspV2&     rspV2      = (*aRspV2Pckg)();

			if((rspV2.iUssdString.iUssdString != DSATTSY_DEFAULT_TEXT)
			|| (rspV2.iUssdString.iDcs != DSATTSY_DCS))	
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else
			{
			RSat::TControlResult ccResult;
			rspV1.iCcRequestedAction.GetCcGeneralResult(ccResult);

			RSat::TCallControlType ccType;
			rspV1.iCcRequestedAction.GetCallControlType(ccType);

			RSat::TCallSetUpParams address;
			rspV1.iCcRequestedAction.GetCallSetUpDetails(address);

			RSat::TAlphaIdValidity validity;
			RSat::TAlphaId alphaId;
			rspV1.iCcRequestedAction.GetAlphaId(validity, alphaId);

			RSat::TBCRepeatIndicator indicator;
			rspV1.iCcRequestedAction.GetBCRepeatIndicator(indicator);

			if ( (ccResult!=DSATTSY_CC_RESULT) || (ccType!=RSat::ECcAddress) 
				|| (address.iCcp1!=DSATTSY_CCP1) 
				|| (address.iSubAddress!=DSATTSY_SUBADDRESS)
				|| (address.iCcp2!=DSATTSY_CCP2) 
				|| (address.iAddress.iTypeOfNumber!=DSATTSY_TON)
				|| (address.iAddress.iNumberPlan!=DSATTSY_NPI) 
				|| (address.iAddress.iTelNumber!=DSATTSY_TEL_NUMBER) 
				|| (validity!=DSATTSY_NO_ALPHAID)
				|| (alphaId.iAlphaId!=DSATTSY_NULL_BUF)
				|| (alphaId.iStatus!=DSATTSY_ALPHAID_STATUS)
				|| (indicator != DSATTSY_BC_REPEAT_INDICATOR_ALTERNATE))
				{
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}

			else if((rspV1.iGeneralResult2!=DSATTSY_PCMD_RESULT_SUCCESS)
				||(rspV1.iInfoType2!=DSATTSY_PCMD_RSP_NO_INFO)
				||(rspV1.iAdditionalInfo2!=DSATTSY_NULL_BUF))
				{
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}
			else if(!iSetUpCallTerminalRsp++)
				ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}

		iTsyAsyncReqHandle = aTsyReqHandle;
		return KErrNone;
	}

TInt CSatDSatTsy::RefreshTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TRefreshRspV1Pckg* aRspPckg = (RSat::TRefreshRspV1Pckg*)aRsp;
	RSat::TRefreshRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV1.PCmdNumber()!=DSATTSY_REFRESH_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iRefreshTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::SetUpEventListTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSetUpEventListRspV1Pckg* aRspPckg = (RSat::TSetUpEventListRspV1Pckg*)aRsp;
	RSat::TSetUpEventListRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV1.PCmdNumber()!=DSATTSY_SET_UP_EVENT_LIST_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iSetUpEventListTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::SetUpIdleModeTextTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSetUpIdleModeTextRspV1Pckg* aRspPckg = (RSat::TSetUpIdleModeTextRspV1Pckg*)aRsp;
	RSat::TSetUpIdleModeTextRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV1.PCmdNumber()!=DSATTSY_SET_UP_IDLE_MODE_TEXT_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iSetUpIdleModeTextTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::SendDtmfTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSendDtmfRspV1Pckg* aRspPckg = (RSat::TSendDtmfRspV1Pckg*)aRsp;
	RSat::TSendDtmfRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV1.PCmdNumber()!=DSATTSY_SEND_DTMF_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iSendDtmfTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::PerformCardApduTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TPerformCardApduRspV2Pckg* aV2RspPckg = (RSat::TPerformCardApduRspV2Pckg*)aRsp;
	RSat::TPerformCardApduRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_PERFORM_CARD_APDU_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iPerformCardApduTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::PowerOffCardTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TPowerOffCardRspV2Pckg* aV2RspPckg = (RSat::TPowerOffCardRspV2Pckg*)aRsp;
	RSat::TPowerOffCardRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_POWER_OFF_CARD_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iPowerOffCardTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::PowerOnCardTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TPowerOnCardRspV2Pckg* aV2RspPckg = (RSat::TPowerOnCardRspV2Pckg*)aRsp;
	RSat::TPowerOnCardRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_POWER_ON_CARD_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iPowerOnCardTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::GetReaderStatusTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TGetReaderStatusRspV2Pckg* aV2RspPckg = (RSat::TGetReaderStatusRspV2Pckg*)aRsp;
	RSat::TGetReaderStatusRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_GET_READER_STATUS_NUMBER)
		||(rspV2.iMode!=DSATTSY_GET_READER_STATUS_MODE))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iGetReaderStatusTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::RunAtCommandTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TRunAtCommandRspV2Pckg* aV2RspPckg = (RSat::TRunAtCommandRspV2Pckg*)aRsp;
	RSat::TRunAtCommandRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_RUN_AT_COMMAND_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iRunAtCommandTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::LanguageNotificationTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TLanguageNotificationRspV2Pckg* aV2RspPckg = (RSat::TLanguageNotificationRspV2Pckg*)aRsp;
	RSat::TLanguageNotificationRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_LANGUAGE_NOTIFICATION_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iLanguageNotificationTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::LaunchBrowserTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TLaunchBrowserRspV2Pckg* aV2RspPckg = (RSat::TLaunchBrowserRspV2Pckg*)aRsp;
	RSat::TLaunchBrowserRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_LAUNCH_BROWSER_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iLaunchBrowserTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::OpenChannelTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TOpenChannelRspV2Pckg* aV2RspPckg = (RSat::TOpenChannelRspV2Pckg*)aRsp;
	RSat::TOpenChannelRspV2& rspV2 = (*aV2RspPckg)();
			
	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_OPEN_CHANNEL_NUMBER)
		||(rspV2.iBearer.iType!=DSATTSY_CSD_BEARER_TYPE)
		||(rspV2.iBearer.iParams!=DSATTSY_BEARER_PARAMS)
		||(rspV2.iBufferSize!=DSATTSY_BUFFER_SIZE))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iOpenChannelTerminalRsp++)
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::CloseChannelTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TCloseChannelRspV2Pckg* aV2RspPckg = (RSat::TCloseChannelRspV2Pckg*)aRsp;
	RSat::TCloseChannelRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_CLOSE_CHANNEL_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iCloseChannelTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::ReceiveDataTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TReceiveDataRspV2Pckg* aV2RspPckg = (RSat::TReceiveDataRspV2Pckg*)aRsp;
	RSat::TReceiveDataRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_RECEIVE_DATA_NUMBER)
		||(rspV2.iChannelDataLength!=DSATTSY_DATA_LENGTH))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iReceiveDataTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::SendDataTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TSendDataRspV2Pckg* aV2RspPckg = (RSat::TSendDataRspV2Pckg*)aRsp;
	RSat::TSendDataRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_SEND_DATA_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iSendDataTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::GetChannelStatusTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TGetChannelStatusRspV2Pckg* aV2RspPckg = (RSat::TGetChannelStatusRspV2Pckg*)aRsp;
	RSat::TGetChannelStatusRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_GET_CHANNEL_STATUS_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iGetChannelStatusTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::GetServiceInfoTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TGetServiceInfoRspV2Pckg* aV2RspPckg = (RSat::TGetServiceInfoRspV2Pckg*)aRsp;
	RSat::TGetServiceInfoRspV2& rspV2 = (*aV2RspPckg)();
	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_GET_SERVICE_INFO_NUMBER)
		||(rspV2.iRecordInfo.iBearerId!=DSATTSY_BEARER_NOT_PRESENT)
		||(rspV2.iRecordInfo.iServiceId!=DSATTSY_NULL_BEARER_ID)
		||(rspV2.iRecordInfo.iRecord!=DSATTSY_NULL_NARROW_BUF))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iGetServiceInfoTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::DeclareServiceTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TDeclareServiceRspV2Pckg* aV2RspPckg = (RSat::TDeclareServiceRspV2Pckg*)aRsp;
	RSat::TDeclareServiceRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_DECLARE_SERVICE_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iDeclareServiceTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::ServiceSearchTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TServiceSearchRspV2Pckg* aV2RspPckg = (RSat::TServiceSearchRspV2Pckg*)aRsp;
	RSat::TServiceSearchRspV2& rspV2 = (*aV2RspPckg)();

	if((rspV2.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV2.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV2.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV2.PCmdNumber()!=DSATTSY_SERVICE_SEARCH_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iServiceSearchTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
TInt CSatDSatTsy::TimerMgmtTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TTimerMgmtRspV3Pckg* aV3RspPckg = (RSat::TTimerMgmtRspV3Pckg*)aRsp;
	RSat::TTimerMgmtRspV3& rspV3 = (*aV3RspPckg)();

	if((rspV3.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV3.iInfoType!=DSATTSY_INFO_TYPE)
		||(rspV3.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV3.PCmdNumber()!=DSATTSY_TIMER_MNGT_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iTimerMgmtTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::LocalInfoTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TLocalInfoRspV3Pckg* aV3RspPckg = (RSat::TLocalInfoRspV3Pckg*)aRsp;
	RSat::TLocalInfoRspV3& rspV3 = (*aV3RspPckg)();

	if((rspV3.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV3.iInfoType!=DSATTSY_INFO_TYPE)
		||(rspV3.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV3.PCmdNumber()!=DSATTSY_LOCAL_INFO_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iLocalInfoTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	

TInt CSatDSatTsy::PollingIntvTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TPollingIntervalRspV3Pckg* aV3RspPckg = (RSat::TPollingIntervalRspV3Pckg*)aRsp;
	RSat::TPollingIntervalRspV3& rspV3 = (*aV3RspPckg)();

	if((rspV3.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV3.iInfoType!=DSATTSY_INFO_TYPE)
		||(rspV3.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV3.PCmdNumber()!=DSATTSY_POLLING_INTERVAL_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iPollingIntervalTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::PollingOffTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp)
	{
	RSat::TPollingOffRspV3Pckg* aV3RspPckg = (RSat::TPollingOffRspV3Pckg*)aRsp;
	RSat::TPollingOffRspV3& rspV3 = (*aV3RspPckg)();

	if((rspV3.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV3.iInfoType!=DSATTSY_INFO_TYPE)
		||(rspV3.iAdditionalInfo!=DSATTSY_NULL_BUF)
		||(rspV3.PCmdNumber()!=DSATTSY_POLLING_OFF_NUMBER))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iPollingOffTerminalRsp++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::RetrieveMultimediaMsgTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iRetrieveMultimediaMsgTerminalRsp++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyRetrieveMultimediaMsgPCmd called"));
		RSat::TRetrieveMultimediaMessageRspV6Pckg* retrieveMmRspV6PCmdPckg = (RSat::TRetrieveMultimediaMessageRspV6Pckg*)aPCmd;	
		RSat::TRetrieveMultimediaMessageRspV6& retrieveMmRspV6 = (*retrieveMmRspV6PCmdPckg)();
		
		if((retrieveMmRspV6.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)	||
		    (retrieveMmRspV6.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO) ||
		    (retrieveMmRspV6.PCmdNumber()!=DSATTSY_RETRIEVE_MULTIMEDIA_MESSAGE))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}


TInt CSatDSatTsy::SubmitMultimediaMsgTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iSubmitMultimediaMsgTerminalRsp++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySubmitMultimediaMsgPCmd called"));
		RSat::TSubmitMultimediaMessageRspV6Pckg* submitMMRspV6PCmdPckg = (RSat::TSubmitMultimediaMessageRspV6Pckg*)aPCmd;	
		RSat::TSubmitMultimediaMessageRspV6& submitMMRspV6 = (*submitMMRspV6PCmdPckg)();
		
		if((submitMMRspV6.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)	||
		    (submitMMRspV6.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO) ||
		    (submitMMRspV6.PCmdNumber()!=DSATTSY_SUBMIT_MULTIMEDIA_MESSAGE))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
TInt CSatDSatTsy::DisplayMultimediaMsgTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iDisplayMultimediaMsgTerminalRsp++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyDisplayMultimediaMsgPCmd called"));
		RSat::TDisplayMultimediaMessageRspV6Pckg* displayMMRspV6PCmdPckg = (RSat::TDisplayMultimediaMessageRspV6Pckg*)aPCmd;	
		RSat::TDisplayMultimediaMessageRspV6& displayMMRspV6 = (*displayMMRspV6PCmdPckg)();
		
		if((displayMMRspV6.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)	||
		    (displayMMRspV6.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO) ||
		    (displayMMRspV6.PCmdNumber()!=DSATTSY_DISPLAY_MULTIMEDIA_MESSAGE))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}
	
TInt CSatDSatTsy::SetFramesTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iSetFramesTerminalRsp++)
		{
		LOGTEXT(_L8("CSatDSatTsy::SetFramesTerminalRsp called"));
		RSat::TSetFramesRspV6Pckg* setFramesRspV6PCmdPckg = (RSat::TSetFramesRspV6Pckg*)aPCmd;	
		RSat::TSetFramesRspV6& setFramesRspV6 = (*setFramesRspV6PCmdPckg)();
		
		if((setFramesRspV6.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)	||
		    (setFramesRspV6.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO) ||
		    (setFramesRspV6.PCmdNumber()!=DSATTSY_SET_FRAMES) ||
		    (setFramesRspV6.iFramesInformation.iFrameId!=DSATTSY_FRAMES_INFORMATION_FRAMEID) ||
		    (setFramesRspV6.iFramesInformation.iFrameList!=DSATTSY_FRAMES_INFORMATION_FRAME_LIST ))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);	
			}
		else
			{
			ReqCompleted(aTsyReqHandle,KErrNone);	
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}
	
TInt CSatDSatTsy::GetFramesStatusTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iGetFramesStatusTerminalRsp++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyGetFramesStatusPCmd called"));
		RSat::TGetFramesStatusRspV6Pckg* getFramesStatusRspV6PCmdPckg = (RSat::TGetFramesStatusRspV6Pckg*)aPCmd;	
		RSat::TGetFramesStatusRspV6& getFramesStatusRspV6 = (*getFramesStatusRspV6PCmdPckg)();	
		
		if((getFramesStatusRspV6.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(getFramesStatusRspV6.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(getFramesStatusRspV6.PCmdNumber()!=DSATTSY_GET_FRAMES_STATUS) 
		||(getFramesStatusRspV6.iFramesInformation.iFrameId!=DSATTSY_FRAMES_INFORMATION_FRAMEID) 
		||(getFramesStatusRspV6.iFramesInformation.iFrameList!=DSATTSY_FRAMES_INFORMATION_FRAME_LIST ))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else 
			{
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}
	


TInt CSatDSatTsy::TerminalRsp(const TTsyReqHandle aTsyReqHandle, RSat::TPCmd* aPCmd, TDes8* aRsp)
	{
	LOGTEXT(_L8("CSatDSatTsy::TerminalRsp called"));
	switch (*aPCmd)
		{
		case RSat::EDisplayText:
			return DisplayTextTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EGetInkey:
			return GetInkeyTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EGetInput:
			return GetInputTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EPlayTone:
			return PlayToneTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESetUpMenu:
			return SetUpMenuRsp(aTsyReqHandle, aRsp);
		case RSat::ESelectItem:
			return SelectItemTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESendSm:
			return SendSmTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESendSs:
			return SendSSTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESendUssd:
			return SendUssdTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESetUpCall:
			return SetUpCallTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ERefresh:
			return RefreshTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESetUpEventList:
			return SetUpEventListTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESetUpIdleModeText:
			return SetUpIdleModeTextTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESendDtmf:
			return SendDtmfTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EPerformCardApdu:
			return PerformCardApduTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EPowerOffCard:
			return PowerOffCardTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EPowerOnCard:
			return PowerOnCardTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EGetReaderStatus:
			return GetReaderStatusTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ERunAtCommand:
			return RunAtCommandTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ELanguageNotification:
			return LanguageNotificationTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ELaunchBrowser:
			return LaunchBrowserTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EOpenChannelCs:
		case RSat::EOpenChannelGprs:
		case RSat::EOpenChannelLocal:
		case RSat::EOpenChannelUiccServerMode:
			return OpenChannelTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ECloseChannel:
			return CloseChannelTerminalRsp(aTsyReqHandle, aRsp);			
		case RSat::EReceiveData:
			return ReceiveDataTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESendData:
			return SendDataTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EGetChannelStatus:
			return GetChannelStatusTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EDeclareService:
			return DeclareServiceTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EServiceSearch:
			return ServiceSearchTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EGetServiceInformation:
			return GetServiceInfoTerminalRsp(aTsyReqHandle,aRsp);		
		case RSat::ETimerMngt:
			return TimerMgmtTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ELocalInfo:
			return LocalInfoTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EPollingInterval:
			return PollingIntvTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EPollingOff:
			return PollingOffTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ERetrieveMultimediaMsg:
			return RetrieveMultimediaMsgTerminalRsp(aTsyReqHandle,aRsp);		
		case RSat::ESubmitMultimediaMsg:
			return SubmitMultimediaMsgTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EDisplayMultimediaMsg:
			return DisplayMultimediaMsgTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::ESetFrames:
			return SetFramesTerminalRsp(aTsyReqHandle, aRsp);
		case RSat::EGetFramesStatus:
			return GetFramesStatusTerminalRsp(aTsyReqHandle, aRsp);
		default:
			return KErrGeneral; 
			}
	}

TInt CSatDSatTsy::TerminalRspCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::TerminalRspCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CSatDSatTsy::GetMeSideSatProfile(const TTsyReqHandle aTsyReqHandle, TDes8* aMeSimSatProfile)
	{
	// First call by test code is RSat::GetMeSideSatProfile() with V2 param.
	// Second call is RSat::GetMeSideSatProfile() followed by Cancel
	// Third call is RSat::GetMeSideSatProfile() with V5 param.
	if((iGetMeSideSatProfile == 0) || (iGetMeSideSatProfile == 2))
		{
		LOGTEXT(_L8("CSatDSatTsy::GetMeSideSatProfile called"));
		RSat::TSatProfileV1Pckg* aProfilePckg = (RSat::TSatProfileV1Pckg*)aMeSimSatProfile;
		RSat::TSatProfileV1& profileV1 = (*aProfilePckg)();

		profileV1.iSatProfileByte1=0x01;
		profileV1.iSatProfileByte2=0x02;
		profileV1.iSatProfileByte3=0x03;
		profileV1.iSatProfileByte4=0x04;
		profileV1.iSatProfileByte5=0x05;
		profileV1.iSatProfileByte6=0x06;
		profileV1.iSatProfileByte7=0x07;
		profileV1.iSatProfileByte8=0x08;
		profileV1.iSatProfileByte9=0x09;

		if ((profileV1.ExtensionId() == RSat::KSatV2) || 
			(profileV1.ExtensionId() == RSat::KSatV5) || 
			(profileV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TSatProfileV2Pckg* v2ProfilePckg = (RSat::TSatProfileV2Pckg*)aMeSimSatProfile;
			RSat::TSatProfileV2& profileV2 = (*v2ProfilePckg)();

			profileV2.iSatProfileByte10=0x0A;
			profileV2.iSatProfileByte11=0x0B;
			profileV2.iSatProfileByte12=0x0C;
			profileV2.iSatProfileByte13=0x0D;
			profileV2.iSatProfileByte14=0x0E;
			profileV2.iSatProfileByte15=0x0F;
			profileV2.iSatProfileByte16=0x10;
			profileV2.iSatProfileByte17=0x20;
			profileV2.iSatProfileByte18=0x30;
			profileV2.iNumOfChannels=DSATTSY_NUMBER_OF_CHANNELS;
			profileV2.iScreenHeight=DSATTSY_SCREEN_HEIGHT;
			profileV2.iScreenWidth=DSATTSY_SCREEN_WIDTH;
			profileV2.iTiaEiaProtoVersion=DSATTSY_PROTO_VERSION;
			profileV2.iWidthReduction=DSATTSY_WIDTH_REDUCTION;
			}
			
		if ((profileV1.ExtensionId() == RSat::KSatV5) || 
			(profileV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TSatProfileV5Pckg* profileV5Pckg = (RSat::TSatProfileV5Pckg*)aMeSimSatProfile;
			RSat::TSatProfileV5& profileV5 = (*profileV5Pckg)();
			
			// Set profileV5.iSatProfileByte28 to:
			// (KCapsTextAttributeAlignmentLeft | KCapsTextAttributeAlignmentCentre | 
			// KCapsTextAttributeAlignmentRight | KCapsTextAttributeFontSizeNormal | 
			// KCapsTextAttributeFontSizeLarge | KCapsTextAttributeFontSizeSmall)
			profileV5.iSatProfileByte28 = 0x3F;
			// Set profileV5.iSatProfileByte29 to
			// (KCapsTextAttributeStyleNormal | KCapsTextAttributeStyleBold | 
			// KCapsTextAttributeStyleItalic | KCapsTextAttributeStyleUnderline | 
			// KCapsTextAttributeStyleStrikethrough | KCapsTextAttributeStyleForeground | 
			// KCapsTextAttributeStyleBackground)
			profileV5.iSatProfileByte29 = 0x7F;
			}
		if (profileV1.ExtensionId() == RSat::KSatV6) 
			{
			RSat::TSatProfileV6Pckg* profileV6Pckg = (RSat::TSatProfileV6Pckg*)aMeSimSatProfile;
			RSat::TSatProfileV6& profileV6 = (*profileV6Pckg)();
			
			// Set profileV6.iSatProfileByte21 to:
			//(KCapsWML  | KCapsXHTML |
			// KCapsHTML | KCapsCHTML )	
			profileV6.iSatProfileByte21 = 0x0F;
			// Set profileV6.iSatProfileByte22 to:
			//(KCapsProvideLocalInfoUTRAN | KCapsProvideLocalInfoBatteryState |
			// KCapsPlayThemedAndMelodyTone | KCapsSetUpCallMultimediaCall |
			// KCapsRefreshGBA	| KCapsRetrieveMultimediaMessage |
			// KCapsSubmitMultimediaMessage | KCapsDisplayMultimediaMesage)
			profileV6.iSatProfileByte22 = 0xFF;
			// Set profileV6.iSatProfileByte23 to:
			//(KCapsSetFrames  | KCapsGetFramesStatus  |
			// KCapsMmsNotificationDownload | KCapsProvideLocalInfoMEID |
			// KCapsProvideLocalInfoNmrUTRAN | KCapsUssdDataDownload)
            profileV6.iSatProfileByte23 = 0xE7;
			// Set profileV6.iSatProfileByte24 to:
			//(KCapsMaxNumFramesBit1 | KCapsMaxNumFramesBit2 |
			// KCapsMaxNumFramesBit3 | KCapsMaxNumFramesBit4)
            profileV6.iSatProfileByte24 = 0x0F;
			// Set profileV6.iSatProfileByte25 to:
			// (KCapsBrowsingStatusEvent | KCapsMmsTransferStatusEvent |
			// KCapsFrameParametersChangeEvent)
			profileV6.iSatProfileByte25 = 0x07;	
			}
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iGetMeSideSatProfile++;
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::GetMeSideSatProfileCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::GetMeSideSatProfileCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}
	
TInt CSatDSatTsy::ClientSatProfileIndication(const TTsyReqHandle aTsyReqHandle, TDes8* aClientSatProfile)
	{
	
	// First call by test code is with version 2 parameter
	// Second call is with version 5 parameter
	if (iClientSatProfileIndication<2)
		{	
		LOGTEXT(_L8("CSatDSatTsy::ClientSatProfileIndication called"));
		
		RSat::TSatProfileV1Pckg* aClientSatProfilePckg = (RSat::TSatProfileV1Pckg*)aClientSatProfile;
		RSat::TSatProfileV1& satProfileV1 = (*aClientSatProfilePckg)();
		
		// Check version 1 parameters
		if ((satProfileV1.iSatProfileByte1!=0x01)
			||(satProfileV1.iSatProfileByte2!=0x02)
			||(satProfileV1.iSatProfileByte3!=0x03)
			||(satProfileV1.iSatProfileByte4!=0x04)
			||(satProfileV1.iSatProfileByte5!=0x05)
			||(satProfileV1.iSatProfileByte6!=0x06)
			||(satProfileV1.iSatProfileByte7!=0x07)
			||(satProfileV1.iSatProfileByte8!=0x08)
			||(satProfileV1.iSatProfileByte9!=0x09))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		
		// Check version 2 parameters
		if( (satProfileV1.ExtensionId()==RSat::KSatV2) ||
			(satProfileV1.ExtensionId()==RSat::KSatV5) ||
			(satProfileV1.ExtensionId()==RSat::KSatV6))
			{
			RSat::TSatProfileV2Pckg* aV2ClientSatProfilePckg = (RSat::TSatProfileV2Pckg*)aClientSatProfile;
			RSat::TSatProfileV2& satProfileV2 = (*aV2ClientSatProfilePckg)();
			
			if ((satProfileV2.iSatProfileByte10!=0x0A)
				||(satProfileV2.iSatProfileByte11!=0x0B)
				||(satProfileV2.iSatProfileByte12!=0x0C)
				||(satProfileV2.iSatProfileByte13!=0x0D)
				||(satProfileV2.iSatProfileByte14!=0x0E)
				||(satProfileV2.iSatProfileByte15!=0x0F)
				||(satProfileV2.iSatProfileByte16!=0x10)
				||(satProfileV2.iSatProfileByte17!=0x20)
				||(satProfileV2.iSatProfileByte18!=0x30)
				||(satProfileV2.iNumOfChannels!=DSATTSY_NUMBER_OF_CHANNELS)
				||(satProfileV2.iScreenHeight!=DSATTSY_SCREEN_HEIGHT)
				||(satProfileV2.iScreenWidth!=DSATTSY_SCREEN_WIDTH)
				||(satProfileV2.iTiaEiaProtoVersion!=DSATTSY_PROTO_VERSION)
				||(satProfileV2.iWidthReduction!=DSATTSY_WIDTH_REDUCTION))
				{
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}
			}
		
		// Check version 5 parameters	
		if ((satProfileV1.ExtensionId() == RSat::KSatV5) ||
		    (satProfileV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TSatProfileV5Pckg* clientSatProfilePckgV5 = (RSat::TSatProfileV5Pckg*) aClientSatProfile;
			RSat::TSatProfileV5& satProfileV5 = (*clientSatProfilePckgV5)();
			
			if ((satProfileV5.iSatProfileByte28 != 0x3F) ||
				(satProfileV5.iSatProfileByte29 != 0x7F))
				{
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
				}
			}
		
		// Check version 6 parameters	
		if (satProfileV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TSatProfileV6Pckg* clientSatProfilePckgV6 = (RSat::TSatProfileV6Pckg*) aClientSatProfile;
			RSat::TSatProfileV6& satProfileV6 = (*clientSatProfilePckgV6)();
			
			if ((satProfileV6.iSatProfileByte21 != 0x0F) ||
				(satProfileV6.iSatProfileByte22 != 0xFF) ||
				(satProfileV6.iSatProfileByte23 != 0xE7) ||
				(satProfileV6.iSatProfileByte24 != 0x0F) ||
				(satProfileV6.iSatProfileByte25 != 0x07))
				{
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
				}
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iClientSatProfileIndication++;	
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::MenuSelection(const TTsyReqHandle aTsyReqHandle, TDes8* aSelection)
	{
	LOGTEXT(_L8("CSatDSatTsy::MenuSelection called"));
	RSat::TMenuSelectionV1Pckg* aSelectionPckg = (RSat::TMenuSelectionV1Pckg*)aSelection;
	RSat::TMenuSelectionV1& menuSelectionV1 = (*aSelectionPckg)();

	if ((menuSelectionV1.iItemId!=DSATTSY_ITEM_ID)
		||(menuSelectionV1.iHelp!=DSATTSY_HELP_REQUESTED))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		if(!iMenuSelection++)
			ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}

TInt CSatDSatTsy::MenuSelectionCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::MenuSelectionCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyCallControlRequest(const TTsyReqHandle aTsyReqHandle, TDes8* aCallControlDetails)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyCallControlRequest called"));
	if(!iNotifyCallControlRequest++)
	{
		RSat::TAlphaId nullBuf;
		nullBuf.iAlphaId=DSATTSY_NULL_BUF;
		nullBuf.iStatus=DSATTSY_ALPHAID_STATUS;
		if(iNotifyCallControlRequestTestNumber==0)
			{
			RSat::TCallControlV1Pckg* cmdPckg = (RSat::TCallControlV1Pckg*)aCallControlDetails;
			RSat::TCallControlV1& cmdV1 = (*cmdPckg)();
	
			RSat::TCallSetUpParams aParams;
			aParams.iCcp1					=DSATTSY_CCP1;
			aParams.iSubAddress				=DSATTSY_SUBADDRESS;
			aParams.iCcp2					=DSATTSY_CCP2;
			aParams.iAddress.iTypeOfNumber	=DSATTSY_TON;
			aParams.iAddress.iNumberPlan	=DSATTSY_NPI;
			aParams.iAddress.iTelNumber		=DSATTSY_TEL_NUMBER;
	
			cmdV1.SetCcGeneralResult(DSATTSY_CC_RESULT);

			cmdV1.SetAlphaId(DSATTSY_NO_ALPHAID, nullBuf);
			cmdV1.SetCallSetUpDetails(aParams);
			cmdV1.SetBCRepeatIndicator(RSat::EBCSequentialMode);

			iNotifyCallControlRequestTestNumber++;

			if (cmdV1.ExtensionId() == RSat::KSatV2)
				{
				RSat::TCallControlV2Pckg* aV2PCmdPckg = (RSat::TCallControlV2Pckg*)aCallControlDetails;
				RSat::TCallControlV2&     callControlV2 = (*aV2PCmdPckg)();

				callControlV2.SetCallName(DSATTSY_CALL_NAME);
				callControlV2.SetActionOriginator(DSATTSY_ACTION_ORIGINATOR);
				}
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (iNotifyCallControlRequestTestNumber==1)
			{
			RSat::TCallControlV1Pckg* cmdPckg = (RSat::TCallControlV1Pckg*)aCallControlDetails;
			RSat::TCallControlV1& cmdV1 = (*cmdPckg)();

			RSat::TSsString ssString;
			ssString.iTypeOfNumber = DSATTSY_TON;
			ssString.iNumberPlan = DSATTSY_NPI;
			ssString.iSsString = DSATTSY_SS_STRING;
			
			cmdV1.SetCcGeneralResult(DSATTSY_CC_RESULT);
			cmdV1.SetAlphaId(DSATTSY_NO_ALPHAID, nullBuf);
			cmdV1.SetBCRepeatIndicator(RSat::EBCSequentialMode);
			cmdV1.SetSendSsDetails(ssString);

			iNotifyCallControlRequestTestNumber++;

			if (cmdV1.ExtensionId() == RSat::KSatV2)
				{
				RSat::TCallControlV2Pckg* aV2PCmdPckg = (RSat::TCallControlV2Pckg*)aCallControlDetails;
				RSat::TCallControlV2&     callControlV2 = (*aV2PCmdPckg)();

				callControlV2.SetCallName(DSATTSY_CALL_NAME);
				callControlV2.SetActionOriginator(DSATTSY_ACTION_ORIGINATOR);
				}
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (iNotifyCallControlRequestTestNumber==2)
			{
			RSat::TCallControlV1Pckg* cmdPckg = (RSat::TCallControlV1Pckg*)aCallControlDetails;
			RSat::TCallControlV1& cmdV1 = (*cmdPckg)();

			RSat::TUssdString ussdString;
			ussdString.iDcs=DSATTSY_DCS;
			ussdString.iUssdString=DSATTSY_USSD_STRING;
	
			cmdV1.SetCcGeneralResult(DSATTSY_CC_RESULT);
			cmdV1.SetAlphaId(DSATTSY_NO_ALPHAID, nullBuf);
			cmdV1.SetBCRepeatIndicator(RSat::EBCSequentialMode);
			cmdV1.SetSendUssdDetails(ussdString);

			iNotifyCallControlRequestTestNumber++;

			if (cmdV1.ExtensionId() == RSat::KSatV2)
				{
				RSat::TCallControlV2Pckg* aV2PCmdPckg = (RSat::TCallControlV2Pckg*)aCallControlDetails;
				RSat::TCallControlV2&     callControlV2 = (*aV2PCmdPckg)();

				callControlV2.SetCallName(DSATTSY_CALL_NAME);
				callControlV2.SetActionOriginator(DSATTSY_ACTION_ORIGINATOR);
				}
			ReqCompleted(aTsyReqHandle,KErrNone);
			}			
		else if (iNotifyCallControlRequestTestNumber==3)
			{
			RSat::TCallControlV5Pckg* cmdPckg = (RSat::TCallControlV5Pckg*)aCallControlDetails;
			RSat::TCallControlV5& cmd = (*cmdPckg)();
			
			cmd.SetCcGeneralResult(DSATTSY_CC_RESULT);
			cmd.SetAlphaId(DSATTSY_NO_ALPHAID, nullBuf);			
			cmd.SetBCRepeatIndicator(DSATTSY_BC_REPEAT_INDICATOR_FALLBACK);
			TPdpParameters  pdpParams(DSATTSY_PDP_PARAMETERS);			
			cmd.SetPdpParameters(pdpParams);
			
			iNotifyCallControlRequestTestNumber++;
		
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (iNotifyCallControlRequestTestNumber==4)
			{
			RSat::TCallControlV5Pckg* cmdPckg = (RSat::TCallControlV5Pckg*)aCallControlDetails;
			RSat::TCallControlV5& cmd = (*cmdPckg)();
			
			cmd.SetCcGeneralResult(DSATTSY_CC_RESULT);
			cmd.SetAlphaId(DSATTSY_NO_ALPHAID, nullBuf);		
			TPdpParameters  pdpParams(DSATTSY_PDP_PARAMETERS2);			
			cmd.SetPdpParameters(pdpParams);			
			cmd.SetBCRepeatIndicator(DSATTSY_BC_REPEAT_INDICATOR_SERVICECHANGEANDFALLBACK);
			
			iNotifyCallControlRequestTestNumber++;
		
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (iNotifyCallControlRequestTestNumber==5)
			{
			RSat::TCallControlV6Pckg* cmdPckg = (RSat::TCallControlV6Pckg*)aCallControlDetails;
			RSat::TCallControlV6& cmd = (*cmdPckg)();
			
			cmd.SetCcGeneralResult(DSATTSY_CC_RESULT);
			TPdpParameters  pdpParams(DSATTSY_PDP_PARAMETERS2);			
			cmd.SetPdpParameters(pdpParams);			
			cmd.SetBCRepeatIndicator(DSATTSY_BC_REPEAT_INDICATOR_SERVICECHANGEANDFALLBACK);
			cmd.SetCallParamOrigin(DSATTSY_CC_CALL_PARAM_ORIGIN);
			
			iNotifyCallControlRequestTestNumber++;
		
			ReqCompleted(aTsyReqHandle,KErrNone);			
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyCallControlRequestCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyCallControlCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyCallControlRequest=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyMoSmControlRequest(const TTsyReqHandle aTsyReqHandle, TDes8* aMoSmDetails)
	{
	if(!iNotifyMoSmControlRequest++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyMoSmControlRequest called"));
		RSat::TMoSmControlV1Pckg* cmdPckg = (RSat::TMoSmControlV1Pckg*)aMoSmDetails;
		RSat::TMoSmControlV1& cmdV1 = (*cmdPckg)();

		cmdV1.iRpAddress.iTypeOfNumber	=DSATTSY_TON;
		cmdV1.iRpAddress.iNumberPlan	=DSATTSY_NPI;
		cmdV1.iRpAddress.iTelNumber		=DSATTSY_TEL_NUMBER;
		cmdV1.iTpAddress.iTypeOfNumber	=DSATTSY_TON;
		cmdV1.iTpAddress.iNumberPlan	=DSATTSY_NPI;
		cmdV1.iTpAddress.iTelNumber		=DSATTSY_TEL_NUMBER;
		cmdV1.iResult					=DSATTSY_CONTROL_RESULT;
		cmdV1.iAlphaId.iAlphaId			=DSATTSY_ALPHA_ID1;
		cmdV1.iAlphaId.iStatus			=DSATTSY_ALPHA_ID1_STATUS;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyMoSmControlRequestCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyMoSmControlRequestCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CSatDSatTsy::EventDownload(const TTsyReqHandle aTsyReqHandle, RSat::TEventList* aSingleEvent, TDes8* aEventInfo)
	{
	LOGTEXT(_L8("CSatDSatTsy::Event Download called"));

	RSat::TEventDownloadBaseV2Pckg* eventBaseV2Pckg = (RSat::TEventDownloadBaseV2Pckg*)aEventInfo;
	RSat::TEventDownloadBaseV2&     eventBaseV2     = (*eventBaseV2Pckg)();

	if (!iEventDownload++)
		{
		switch( *aSingleEvent )
			{
		case RSat::KUserActivity:			// Allow KuserActivity to fall through to KidleScreenAvailable
		case RSat::KIdleScreenAvailable:
			// Don't expect any data for these events
			if (eventBaseV2.iDataAvailable)				
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				ReqCompleted(aTsyReqHandle, KErrNone);
			break;

		case RSat::KCardReaderStatus:
			// Expect TCardReaderStatusEventV2
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TCardReaderStatusEventV2Pckg* cardReaderEventV2Pckg = (RSat::TCardReaderStatusEventV2Pckg *)aEventInfo;
				RSat::TCardReaderStatusEventV2&     cardReaderEventV2     = (*cardReaderEventV2Pckg)();

				if (cardReaderEventV2.iCardReaderStatus != DSATTSY_EVENT_CARD_READER_STATUS)
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;

		case RSat::KLanguageSelection:
			// Expect TLanguageSelectionEventV2
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TLanguageSelectionEventV2Pckg* languageV2Pckg = (RSat::TLanguageSelectionEventV2Pckg *)aEventInfo;
				RSat::TLanguageSelectionEventV2&     languageV2     = (*languageV2Pckg)();

				if (languageV2.iLanguage != DSATTSY_EVENT_LANGUAGE_SELECTION)
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;

		case RSat::KBrowserTermination:
			// Expect TBrowserTerminationEventV2
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TBrowserTerminationEventV2Pckg* terminationV2Pckg = (RSat::TBrowserTerminationEventV2Pckg *)aEventInfo;
				RSat::TBrowserTerminationEventV2&     terminationV2     = (*terminationV2Pckg)();

				if (terminationV2.iCause != DSATTSY_EVENT_BROWSER_TERMINATION_CAUSE)
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;

		case RSat::KDataAvailable:
			// Expect TDataAvailableEventV2
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TDataAvailableEventV2Pckg* dataV2Pckg = (RSat::TDataAvailableEventV2Pckg *)aEventInfo;
				RSat::TDataAvailableEventV2&     dataV2     = (*dataV2Pckg)();

				if ((dataV2.iStatus != DSATTSY_EVENT_DATA_AVAILABLE_STATUS)
				 || (dataV2.iLength != DSATTSY_EVENT_DATA_AVAILABLE_LENGTH))
					{
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
					}
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;

		case RSat::KChannelStatus:
			// Expect TChannelStatusEventV2
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TChannelStatusEventV2Pckg* channelV2Pckg = (RSat::TChannelStatusEventV2Pckg *)aEventInfo;
				RSat::TChannelStatusEventV2&     channelV2     = (*channelV2Pckg)();

				if (channelV2.iStatus != DSATTSY_EVENT_CHANNEL_STATUS)
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;

		case RSat::KDisplayParamsChanges:
			// Expect TDisplayParamsEventV2
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TDisplayParamsEventV2Pckg* displayV2Pckg = (RSat::TDisplayParamsEventV2Pckg *)aEventInfo;
				RSat::TDisplayParamsEventV2&     displayV2     = (*displayV2Pckg)();

				if ((displayV2.iSatProfileByte14 != DSATTSY_EVENT_DISPLAY_PROF14)
				 || (displayV2.iScreenHeight     != DSATTSY_SCREEN_HEIGHT)
				 || (displayV2.iSatProfileByte15 != DSATTSY_EVENT_DISPLAY_PROF15)
				 || (displayV2.iScreenWidth      != DSATTSY_SCREEN_WIDTH)
				 || (displayV2.iSatProfileByte16 != DSATTSY_EVENT_DISPLAY_PROF16)
				 || (displayV2.iWidthReduction   != DSATTSY_WIDTH_REDUCTION))
					{
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
					}
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;

		case RSat::KLocalConnection:
			// Expect TLocalConnectionEventV2
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TLocalConnectionEventV2Pckg* localV2Pckg = (RSat::TLocalConnectionEventV2Pckg *)aEventInfo;
				RSat::TLocalConnectionEventV2&     localV2     = (*localV2Pckg)();

				if ((localV2.iRecord.iBearerId  != DSATTSY_EVENT_LOCAL_BEARER_ID)
				 || (localV2.iRecord.iServiceId != DSATTSY_EVENT_LOCAL_SERVICE_ID)
				 || (localV2.iRecord.iRecord	!= DSATTSY_EVENT_LOCAL_SERVICE_RECORD)
				 || (localV2.iAddress.iCoding   != DSATTSY_EVENT_LOCAL_ADDR_CODING)
				 || (localV2.iAddress.iAddress  != DSATTSY_EVENT_LOCAL_ADDRESS))
					{
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
					}
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;
			
		case RSat::KNetworkSearchModeChange:
			// Expect TNetworkSearchModeChangeV6
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TNetworkSearchModeChangeEventV6Pckg* networkSearchV6Pckg = (RSat::TNetworkSearchModeChangeEventV6Pckg *)aEventInfo;
				RSat::TNetworkSearchModeChangeEventV6&     networkSearchV6     = (*networkSearchV6Pckg)();

				if (networkSearchV6.iNetworkSearchMode  != DSATTSY_NETWORK_SEARCH_MODE1)
					{
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
					}
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;
	
		case RSat::KBrowsingStatusChange:
			// Expect TBrowsingStatusEventV6
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TBrowsingStatusEventV6Pckg* browsingStatusEventV6Pckg = (RSat::TBrowsingStatusEventV6Pckg *)aEventInfo;
				RSat::TBrowsingStatusEventV6&     browsingStatusEventV6     = (*browsingStatusEventV6Pckg)();

				if (browsingStatusEventV6.iBrowserStatus  != DSATTSY_BROWSING_STATUS1)
					{
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
					}
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;
		
		case RSat::KFramesInformationChange:
			// Expect TFramesInformationChangeV6
			if (!eventBaseV2.iDataAvailable)
				ReqCompleted(aTsyReqHandle, KErrCorrupt);
			else
				{
				RSat::TFramesInformationChangedEventV6Pckg* framesInfoChangedEventV6Pckg = (RSat::TFramesInformationChangedEventV6Pckg *)aEventInfo;
				RSat::TFramesInformationChangedEventV6&     framesInfoChangedEventV6     = (*framesInfoChangedEventV6Pckg)();

				if ((framesInfoChangedEventV6.iFramesInformation.iFrameId  != DSATTSY_FRAMES_INFORMATION_FRAMEID )
				 || (framesInfoChangedEventV6.iFramesInformation.iFrameList != DSATTSY_FRAMES_INFORMATION_FRAME_LIST ))
					{
					ReqCompleted(aTsyReqHandle, KErrCorrupt);
					}
				else
					ReqCompleted(aTsyReqHandle, KErrNone);
				}
			break;
					
		default:
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		}

	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::EventDownloadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::EventDownloadCancel called"));

	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);

	iEventDownload = 0;

	return KErrNone;
	}


TInt CSatDSatTsy::NotifyDisplayTextPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyDisplayTextPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyDisplayTextPCmd called"));
		RSat::TDisplayTextV1Pckg* aPCmdPckg = (RSat::TDisplayTextV1Pckg*)aPCmd;
		RSat::TDisplayTextV1& displayTextV1 = (*aPCmdPckg)();
		
		displayTextV1.SetPCmdNumber(DSATTSY_DISPLAY_TEXT_NUMBER);
		displayTextV1.iPriority				=DSATTSY_TEXT_PRIORITY;
		displayTextV1.iClearScreenTrigger	=DSATTSY_CLEAR_SCREEN_TRIGGER;
		displayTextV1.iText					=DSATTSY_TEXT_TO_DISPLAY;
		displayTextV1.iIconId.iIdentifier	=DSATTSY_ICON_ID1;
		displayTextV1.iIconId.iQualifier	=DSATTSY_ICON_QUALIFIER1;
		displayTextV1.iImmediateRsp			=DSATTSY_IMMEDIATE_RSP;

		if ((displayTextV1.ExtensionId() == RSat::KSatV2) ||
			(displayTextV1.ExtensionId() == RSat::KSatV5) || (displayTextV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TDisplayTextV2Pckg* aV2PCmdPckg = (RSat::TDisplayTextV2Pckg*)aPCmd;
			RSat::TDisplayTextV2&   displayTextV2 = (*aV2PCmdPckg)();

			displayTextV2.iDuration.iTimeUnit=DSATTSY_TIME_UNIT;
			displayTextV2.iDuration.iNumOfUnits=DSATTSY_NUM_OF_UNITS;
			}
			
		if ((displayTextV1.ExtensionId() == RSat::KSatV5) || (displayTextV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TDisplayTextV5Pckg* aV5PCmdPckg = (RSat::TDisplayTextV5Pckg*)aPCmd;
			RSat::TDisplayTextV5&   displayTextV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			displayTextV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			displayTextV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}
		
		if (displayTextV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TDisplayTextV6Pckg* displayTextV6PCmdPckg = (RSat::TDisplayTextV6Pckg*)aPCmd;
			RSat::TDisplayTextV6&   displayTextV6 = (*displayTextV6PCmdPckg)();
			
			displayTextV6.iFrameId=DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}	
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
		
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyDisplayTextPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyDisplayTextPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyDisplayTextPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetInkeyPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyGetInkeyPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyGetInkeyPCmd called"));
		RSat::TGetInkeyV1Pckg* aPCmdPckg = (RSat::TGetInkeyV1Pckg*)aPCmd;
		RSat::TGetInkeyV1& getInkeyV1 = (*aPCmdPckg)();

		getInkeyV1.SetPCmdNumber(DSATTSY_GET_INKEY_NUMBER);
		
		getInkeyV1.iRspFormat			=DSATTSY_USER_RSP_FORMAT;
		getInkeyV1.iText				=DSATTSY_TEXT_TO_DISPLAY;
		getInkeyV1.iIconId.iIdentifier	=DSATTSY_ICON_ID1;
		getInkeyV1.iIconId.iQualifier	=DSATTSY_ICON_QUALIFIER1;
		getInkeyV1.iHelp				=DSATTSY_HELP;

		if ((getInkeyV1.ExtensionId() == RSat::KSatV2) ||
			(getInkeyV1.ExtensionId() == RSat::KSatV5) || 
			(getInkeyV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TGetInkeyV2Pckg* aV2PCmdPckg = (RSat::TGetInkeyV2Pckg*)aPCmd;
			RSat::TGetInkeyV2&     getInkeyV2  = (*aV2PCmdPckg)();

			getInkeyV2.iDuration.iTimeUnit=DSATTSY_TIME_UNIT;
			getInkeyV2.iDuration.iNumOfUnits=DSATTSY_NUM_OF_UNITS;
			getInkeyV2.iMode=DSATTSY_GET_INKEY_RSP_MODE;
			}
			
		if ((getInkeyV1.ExtensionId() == RSat::KSatV5) || 
			(getInkeyV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TGetInkeyV5Pckg* aV5PCmdPckg = (RSat::TGetInkeyV5Pckg*)aPCmd;
			RSat::TGetInkeyV5&   getInkeyV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			getInkeyV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			getInkeyV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);			
			}

		if (getInkeyV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TGetInkeyV6Pckg* getInkeyV6PCmdPckg = (RSat::TGetInkeyV6Pckg*)aPCmd;
			RSat::TGetInkeyV6&   getInkeyV6 = (*getInkeyV6PCmdPckg)();
			
			getInkeyV6.iFrameId=DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;	
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
		
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetInkeyPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyGetInkeyPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyGetInkeyPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetInputPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyGetInputPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyGetInputPCmd called"));
		RSat::TGetInputV1Pckg* aPCmdPckg = (RSat::TGetInputV1Pckg*)aPCmd;
		RSat::TGetInputV1& getInputV1 = (*aPCmdPckg)();

		getInputV1.SetPCmdNumber(DSATTSY_GET_INPUT_NUMBER);
		
		getInputV1.iInputDisplayOption		=DSATTSY_INPUT_DISPLAY_OPTION;
		getInputV1.iRspFormat				=DSATTSY_GET_INPUT_RSP_FORMAT;
		getInputV1.iText					=DSATTSY_TEXT_TO_DISPLAY;
		getInputV1.iRspLength.iMinRspLength	=DSATTSY_MIN_RSP_LENGTH;
		getInputV1.iRspLength.iMaxRspLength	=DSATTSY_MAX_RSP_LENGTH;
		getInputV1.iDefaultText				=DSATTSY_DEFAULT_TEXT;
		getInputV1.iIconId.iIdentifier		=DSATTSY_ICON_ID1;
		getInputV1.iIconId.iQualifier		=DSATTSY_ICON_QUALIFIER1;
		getInputV1.iHelp					=DSATTSY_HELP;

		if ((getInputV1.ExtensionId() == RSat::KSatV5) ||
			(getInputV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TGetInputV5Pckg* aV5PCmdPckg = (RSat::TGetInputV5Pckg*)aPCmd;
			RSat::TGetInputV5&   getInputV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			getInputV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			getInputV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}

		if (getInputV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TGetInputV6Pckg* getInputV6PCmdPckg = (RSat::TGetInputV6Pckg*)aPCmd;
			RSat::TGetInputV6&   getInputV6 = (*getInputV6PCmdPckg)();
			
			getInputV6.iFrameId=DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1 ;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}

TInt CSatDSatTsy::NotifyGetInputPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyGetInputPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyGetInputPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyPlayTonePCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyPlayTonePCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyPlayTonePCmd called"));
		RSat::TPlayToneV1Pckg* aPCmdPckg = (RSat::TPlayToneV1Pckg*)aPCmd;
		RSat::TPlayToneV1& playToneV1 = (*aPCmdPckg)();
		
		playToneV1.SetPCmdNumber(DSATTSY_PLAY_TONE_NUMBER);

		playToneV1.iAlphaId.iAlphaId		=DSATTSY_ALPHA_ID1;
		playToneV1.iAlphaId.iStatus			=DSATTSY_ALPHA_ID1_STATUS;
		playToneV1.iTone					=DSATTSY_TONE;
		playToneV1.iDuration.iTimeUnit		=DSATTSY_TIME_UNIT;
		playToneV1.iDuration.iNumOfUnits	=DSATTSY_NUM_OF_UNITS;

		if	((playToneV1.ExtensionId() == RSat::KSatV2) ||
			(playToneV1.ExtensionId() == RSat::KSatV5) || (playToneV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TPlayToneV2Pckg* aV2PCmdPckg = (RSat::TPlayToneV2Pckg*)aPCmd;
			RSat::TPlayToneV2& playToneV2 = (*aV2PCmdPckg)();

			playToneV2.iIconId.iIdentifier=DSATTSY_ICON_ID1;
			playToneV2.iIconId.iQualifier=DSATTSY_ICON_QUALIFIER1;
			}
			
		if ((playToneV1.ExtensionId() == RSat::KSatV5) ||
		   (playToneV1.ExtensionId() == RSat::KSatV6))	
			{
			RSat::TPlayToneV5Pckg* aV5PCmdPckg = (RSat::TPlayToneV5Pckg*)aPCmd;
			RSat::TPlayToneV5&   playToneV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			playToneV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			playToneV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);			
			}

		if (playToneV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TPlayToneV6Pckg* playToneV6PCmdPckg = (RSat::TPlayToneV6Pckg*)aPCmd;
			RSat::TPlayToneV6&   playToneV6 = (*playToneV6PCmdPckg)();
			
			playToneV6.iFrameId=DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1 ;
			playToneV6.iTone=DSATTSY_TONEV6;			
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyPlayTonePCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyPlayTonePCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyPlayTonePCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySetUpMenuPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	RSat::TItem item1, item2, item3, item4;
	TUint action1=0x01, action2=0x02, action3=0x03, action4=0x04;
	TUint iconId1=0x01, iconId2=0x02, iconId3=0x03, iconId4=0x04;
	
	RSat::TTextAttribute textAttribute1, textAttribute2, textAttribute3, textAttribute4;

	LOGTEXT(_L8("CSatDSatTsy::NotifySetUpMenuPCmd called"));
	RSat::TSetUpMenuV1Pckg* aPCmdPckg = (RSat::TSetUpMenuV1Pckg*)aPCmd;
	RSat::TSetUpMenuV1& setUpMenuV1 = (*aPCmdPckg)();

	item1.iItemId=0x01;
	item1.iItemString=DSATTSY_ITEM1;
	item2.iItemId=0x02;
	item2.iItemString=DSATTSY_ITEM2;
	item3.iItemId=0x03;
	item3.iItemString=DSATTSY_ITEM3;
	item4.iItemId=0x04;	
	item4.iItemString=DSATTSY_ITEM4;
	
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

	setUpMenuV1.iAlphaId.iAlphaId	=DSATTSY_ALPHA_ID1;
	setUpMenuV1.iAlphaId.iStatus	=DSATTSY_ALPHA_ID1_STATUS;
	setUpMenuV1.iHelp				=DSATTSY_HELP;
	setUpMenuV1.iIconId.iIdentifier	=DSATTSY_ICON_ID1;
	setUpMenuV1.iIconId.iQualifier	=DSATTSY_ICON_QUALIFIER1;
	setUpMenuV1.iIconListQualifier	=DSATTSY_ICON_QUALIFIER2;

	setUpMenuV1.SetPCmdNumber(DSATTSY_SET_UP_MENU_NUMBER);

	if(	(setUpMenuV1.ExtensionId() == RSat::KSatV2) ||
		(setUpMenuV1.ExtensionId() == RSat::KSatV5))
		{
		RSat::TSetUpMenuV2Pckg* aV2PCmdPckg = (RSat::TSetUpMenuV2Pckg*)aPCmd;
		RSat::TSetUpMenuV2& setUpMenuV2 = (*aV2PCmdPckg)();

		setUpMenuV2.iPreference=DSATTSY_SELECTION_PREFERENCE;
		}

	if(!iNotifySetUpMenuPCmd++)
		{
		switch (iNotifySetUpMenuTestNumber)
			{
		case 0:
			setUpMenuV1.AddItem(item1);
			setUpMenuV1.AddItem(item2);
			setUpMenuV1.AddItem(item3);

			iNotifySetUpMenuTestNumber ++;
			ReqCompleted(aTsyReqHandle,KErrNone);
			
			break;
		case 1:
			setUpMenuV1.AddItem(item1, action1);
			setUpMenuV1.AddItem(item2, action2);
			setUpMenuV1.AddItem(item3, action3);

			iNotifySetUpMenuTestNumber ++;
			ReqCompleted(aTsyReqHandle,KErrNone);
			
			break;		
		case 2:
			setUpMenuV1.AddItemIcon(item1, iconId1);
			setUpMenuV1.AddItemIcon(item2, iconId2);
			setUpMenuV1.AddItemIcon(item3, iconId3);

			iNotifySetUpMenuTestNumber ++;
			ReqCompleted(aTsyReqHandle,KErrNone);
			
			break;
		case 3:
			setUpMenuV1.AddItem(item1, action1, iconId1);
			setUpMenuV1.AddItem(item2, action2, iconId2);
			setUpMenuV1.AddItem(item3, action3, iconId3);
			setUpMenuV1.AddItem(item4);
			
			iNotifySetUpMenuTestNumber ++;
			ReqCompleted(aTsyReqHandle,KErrNone);
			
			break;
		case 4:
			if(setUpMenuV1.ExtensionId() == RSat::KSatV5)
				{
				RSat::TSetUpMenuV5Pckg* aV5PCmdPckg = (RSat::TSetUpMenuV5Pckg*)aPCmd;
				RSat::TSetUpMenuV5& setUpMenuV5 = (*aV5PCmdPckg)();		
				
				setUpMenuV5.AddItem(item1, textAttribute1);
				setUpMenuV5.AddItem(item2, textAttribute2);
				setUpMenuV5.AddItem(item3, textAttribute3);
				setUpMenuV5.AddItem(item4, textAttribute4);
				
				setUpMenuV5.iTextAttribute.iStatus = textAttribute1.iStatus;
				setUpMenuV5.iTextAttribute.iTextAttributeData = textAttribute1.iTextAttributeData;
				
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrNone);				
				}
			else
				{
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}
				
			break;
		case 5:
			if(setUpMenuV1.ExtensionId() == RSat::KSatV5)
				{
				RSat::TSetUpMenuV5Pckg* aV5PCmdPckg = (RSat::TSetUpMenuV5Pckg*)aPCmd;
				RSat::TSetUpMenuV5& setUpMenuV5 = (*aV5PCmdPckg)();		
				
				setUpMenuV5.AddItem(item1, textAttribute1, action1);
				setUpMenuV5.AddItem(item2, textAttribute2, action2);
				setUpMenuV5.AddItem(item3, textAttribute3, action3);
				setUpMenuV5.AddItem(item4, textAttribute4, action4);
				
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrNone);				
				}
			else
				{
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}
				
			break;		
		case 6:
			if(setUpMenuV1.ExtensionId() == RSat::KSatV5)
				{
				RSat::TSetUpMenuV5Pckg* aV5PCmdPckg = (RSat::TSetUpMenuV5Pckg*)aPCmd;
				RSat::TSetUpMenuV5& setUpMenuV5 = (*aV5PCmdPckg)();		

				setUpMenuV5.AddItemIcon(item1, textAttribute1, iconId1);
				setUpMenuV5.AddItemIcon(item2, textAttribute2, iconId2);
				setUpMenuV5.AddItemIcon(item3, textAttribute3, iconId3);
				setUpMenuV5.AddItemIcon(item4, textAttribute4, iconId4);
				
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrNone);				
				}
			else
				{
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}
					
			break;
		case 7:
			if(setUpMenuV1.ExtensionId() == RSat::KSatV5)
				{
				RSat::TSetUpMenuV5Pckg* aV5PCmdPckg = (RSat::TSetUpMenuV5Pckg*)aPCmd;
				RSat::TSetUpMenuV5& setUpMenuV5 = (*aV5PCmdPckg)();		
				
				setUpMenuV5.AddItem(item1, textAttribute1, action1, iconId1);
				setUpMenuV5.AddItem(item2, textAttribute2, action2, iconId2);
				setUpMenuV5.AddItem(item3, textAttribute3, action3, iconId3);
				setUpMenuV5.AddItem(item4, textAttribute4, action4, iconId4);
				
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrNone);				
				}
			else
				{
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}
			break;
		case 8:
			if(setUpMenuV1.ExtensionId() == RSat::KSatV5)
				{
				RSat::TSetUpMenuV5Pckg* aV5PCmdPckg = (RSat::TSetUpMenuV5Pckg*)aPCmd;
				RSat::TSetUpMenuV5& setUpMenuV5 = (*aV5PCmdPckg)();		
				
				setUpMenuV5.AddItem(item1);
				setUpMenuV5.AddItem(item2);
				setUpMenuV5.AddItem(item3);
				setUpMenuV5.AddItem(item4);
				
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrNone);				
				}
			else
				{
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}			
			break;
		case 9:
			if(setUpMenuV1.ExtensionId() == RSat::KSatV5)
				{
				RSat::TSetUpMenuV5Pckg* aV5PCmdPckg = (RSat::TSetUpMenuV5Pckg*)aPCmd;
				RSat::TSetUpMenuV5& setUpMenuV5 = (*aV5PCmdPckg)();		
				
				setUpMenuV5.AddItem(item1, action1);
				setUpMenuV5.AddItem(item2, action2);
				setUpMenuV5.AddItem(item3, action3);
				setUpMenuV5.AddItem(item4, action4);
				
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrNone);				
				}
			else
				{
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}			
			break;
		case 10:
			if(setUpMenuV1.ExtensionId() == RSat::KSatV5)
				{
				RSat::TSetUpMenuV5Pckg* aV5PCmdPckg = (RSat::TSetUpMenuV5Pckg*)aPCmd;
				RSat::TSetUpMenuV5& setUpMenuV5 = (*aV5PCmdPckg)();		
				
				setUpMenuV1.AddItemIcon(item1, iconId1);
				setUpMenuV1.AddItemIcon(item2, iconId2);
				setUpMenuV1.AddItemIcon(item3, iconId3);
				setUpMenuV1.AddItemIcon(item4, iconId4);
				
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrNone);				
				}
			else
				{
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}			
			break;
		case 11:
			if(setUpMenuV1.ExtensionId() == RSat::KSatV5)
				{
				RSat::TSetUpMenuV5Pckg* aV5PCmdPckg = (RSat::TSetUpMenuV5Pckg*)aPCmd;
				RSat::TSetUpMenuV5& setUpMenuV5 = (*aV5PCmdPckg)();		
				
				setUpMenuV5.AddItem(item1, action1, iconId1);
				setUpMenuV5.AddItem(item2, action2, iconId2);
				setUpMenuV5.AddItem(item3, action3, iconId3);
				setUpMenuV5.AddItem(item4, action4, iconId4);
				
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrNone);				
				}
			else
				{
				iNotifySetUpMenuTestNumber ++;
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				}			
			break;								
		case 12:
			iNotifySetUpMenuTestNumber ++;
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;		
		default:
			break;
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySetUpMenuPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySetUpMenuPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySetUpMenuPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySelectItemPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySelectItemPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySelectItemPCmd called"));
		RSat::TSelectItemV1Pckg* aPCmdPckg = (RSat::TSelectItemV1Pckg*)aPCmd;
		RSat::TSelectItemV1& selectItemV1 = (*aPCmdPckg)();
		
		TUint action1=0x01, action2=0x02, action3=0x03;
		RSat::TItem item1, item2, item3;
		TUint iconId1=0x01, iconId2=0x02, iconId3=0x03;

		item1.iItemId=0x01;
		item1.iItemString=DSATTSY_ITEM1;
		item2.iItemId=0x02;
		item2.iItemString=DSATTSY_ITEM2;
		item3.iItemId=0x03;
		item3.iItemString=DSATTSY_ITEM3;

		selectItemV1.SetPCmdNumber(DSATTSY_SELECT_ITEM_NUMBER);

		selectItemV1.iPresentationType	=DSATTSY_PRESENTATION_TYPE;
		selectItemV1.iHelp				=DSATTSY_HELP;
		selectItemV1.iAlphaId.iAlphaId	=DSATTSY_ALPHA_ID1;
		selectItemV1.iAlphaId.iStatus	=DSATTSY_ALPHA_ID1_STATUS;
		selectItemV1.iDefaultItemId		=DSATTSY_DEFAULT_ITEM;
		selectItemV1.iIconId.iIdentifier=DSATTSY_ICON_ID1;
		selectItemV1.iIconId.iQualifier	=DSATTSY_ICON_QUALIFIER1;
		selectItemV1.iIconListQualifier	=DSATTSY_ICON_QUALIFIER2;
		
		if ((selectItemV1.ExtensionId() == RSat::KSatV1) ||
			(selectItemV1.ExtensionId() == RSat::KSatV2))
			{
			selectItemV1.AddItem(item1, action1, iconId1);
			selectItemV1.AddItem(item2, action2, iconId2);
			selectItemV1.AddItem(item3, action3, iconId3);
			}
	
		if ((selectItemV1.ExtensionId() == RSat::KSatV2) ||
			(selectItemV1.ExtensionId() == RSat::KSatV5) || (selectItemV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TSelectItemV2Pckg* aV2PCmdPckg = (RSat::TSelectItemV2Pckg*)aPCmd;
			RSat::TSelectItemV2& selectItemV2 = (*aV2PCmdPckg)();

			selectItemV2.iPreference=DSATTSY_SELECTION_PREFERENCE;
			}

		if ((selectItemV1.ExtensionId() == RSat::KSatV5) ||
			(selectItemV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TSelectItemV5Pckg* aV5PCmdPckg = (RSat::TSelectItemV5Pckg*)aPCmd;
			RSat::TSelectItemV5& selectItemV5 = (*aV5PCmdPckg)();
			
			RSat::TTextAttribute textAttribute1, textAttribute2, textAttribute3;
			
			textAttribute1.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			textAttribute1.iTextAttributeData.Append(textAttributeData, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
	
			textAttribute2.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS2;
			const TUint8 textAttributeData2[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA2;
			textAttribute2.iTextAttributeData.Append(textAttributeData2, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
	
			textAttribute3.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS3;
			const TUint8 textAttributeData3[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA3;
			textAttribute3.iTextAttributeData.Append(textAttributeData3, DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			
			selectItemV5.AddItem(item1, textAttribute1, action1, iconId1);
			selectItemV5.AddItem(item2, textAttribute2, action2, iconId2);
			selectItemV5.AddItem(item3, textAttribute3, action3, iconId3);
			
			selectItemV5.iTextAttribute = textAttribute1;			
			}

		if (selectItemV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TSelectItemV6Pckg* selectItemV6PCmdPckg = (RSat::TSelectItemV6Pckg*)aPCmd;
			RSat::TSelectItemV6& selectItemV6 = (*selectItemV6PCmdPckg)();
			
			selectItemV6.iFrameId=DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySelectItemPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySelectItemPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySelectItemPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendSmPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySendSmPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySendSmPCmd called"));
		RSat::TSendSmV1Pckg* aPCmdPckg = (RSat::TSendSmV1Pckg*)aPCmd;
		RSat::TSendSmV1& sendSmV1 = (*aPCmdPckg)();

		sendSmV1.SetPCmdNumber(DSATTSY_SEND_SM_NUMBER);
		
		sendSmV1.iAlphaId.iAlphaId		=DSATTSY_ALPHA_ID1;
		sendSmV1.iAlphaId.iStatus		=DSATTSY_ALPHA_ID1_STATUS;
		sendSmV1.iAddress.iTypeOfNumber	=DSATTSY_TON;
		sendSmV1.iAddress.iNumberPlan	=DSATTSY_NPI;
		sendSmV1.iAddress.iTelNumber	=DSATTSY_TEL_NUMBER;
		sendSmV1.iSmsTpdu				=DSATTSY_SMS_TPDU; 
		sendSmV1.iIconId.iIdentifier	=DSATTSY_ICON_ID1;
		sendSmV1.iIconId.iQualifier		=DSATTSY_ICON_QUALIFIER1;

		if ((sendSmV1.ExtensionId() == RSat::KSatV5)  ||
			(sendSmV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TSendSmV5Pckg* aV5PCmdPckg = (RSat::TSendSmV5Pckg*)aPCmd;
			RSat::TSendSmV5&   sendSmV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			sendSmV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			sendSmV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}		

		if (sendSmV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TSendSmV6Pckg* sendSmV6PCmdPckg = (RSat::TSendSmV6Pckg*)aPCmd;
			RSat::TSendSmV6&   sendSmV6 = (*sendSmV6PCmdPckg)();
			
			sendSmV6.iFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendSmPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySendSmPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySendSmPCmd=0;
	return KErrNone;
	}


TInt CSatDSatTsy::SendMessageNoLogging(const TTsyReqHandle aTsyReqHandle, const TDesC8* aMsg, TUint16* aMsgRef)
	{
	LOGTEXT(_L8("CSatDSatTsy::SendMessageNoLogging called"));
	RSat::TSatSmsV1Pckg* aMsgPckg = (RSat::TSatSmsV1Pckg*)aMsg;
	RSat::TSatSmsV1& msgV1 = (*aMsgPckg)();

	*aMsgRef=DSATTSY_SMS_REF;

	if ((msgV1.iServiceCenter.iNumberPlan!=DSATTSY_NPI)
		||(msgV1.iServiceCenter.iTypeOfNumber!=DSATTSY_TON)
		||(msgV1.iServiceCenter.iTelNumber!=DSATTSY_TEL_NUMBER)
		||(msgV1.iBuf!=DSATTSY_SMS_TPDU))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		if(!iSendMessageNoLogging++)
			ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}

TInt CSatDSatTsy::SendMessageNoLoggingCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::SendMessageNoLoggingCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendSsPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySendSsPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySendSsPCmd called"));
		RSat::TSendSsV1Pckg* aPCmdPckg = (RSat::TSendSsV1Pckg*)aPCmd;
		RSat::TSendSsV1& sendSsV1 = (*aPCmdPckg)();

		sendSsV1.SetPCmdNumber(DSATTSY_SEND_SS_NUMBER);
		
		sendSsV1.iAlphaId.iAlphaId			=DSATTSY_ALPHA_ID1;
		sendSsV1.iAlphaId.iStatus			=DSATTSY_ALPHA_ID1_STATUS;
		sendSsV1.iSsString.iTypeOfNumber	=DSATTSY_TON;
		sendSsV1.iSsString.iNumberPlan		=DSATTSY_NPI;
		sendSsV1.iSsString.iSsString		=DSATTSY_SS_STRING;
		sendSsV1.iIconId.iIdentifier		=DSATTSY_ICON_ID1;
		sendSsV1.iIconId.iQualifier			=DSATTSY_ICON_QUALIFIER1;

		if (sendSsV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TSendSsV6Pckg* sendSsV6PCmdPckg = (RSat::TSendSsV6Pckg*)aPCmd;
			RSat::TSendSsV6&   sendSsV6 = (*sendSsV6PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			sendSsV6.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			sendSsV6.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendSsPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySendSsPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySendSsPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendUssdPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySendUssdPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySendUssdPCmd called"));
		RSat::TSendUssdV1Pckg* aPCmdPckg = (RSat::TSendUssdV1Pckg*)aPCmd;
		RSat::TSendUssdV1& sendUssdV1 = (*aPCmdPckg)();
		
		sendUssdV1.SetPCmdNumber(DSATTSY_SEND_USSD_NUMBER);

		sendUssdV1.iAlphaId.iAlphaId		=DSATTSY_ALPHA_ID1;
		sendUssdV1.iAlphaId.iStatus			=DSATTSY_ALPHA_ID1_STATUS;
		sendUssdV1.iUssdString.iDcs			=DSATTSY_DCS;
		sendUssdV1.iUssdString.iUssdString	=DSATTSY_USSD_STRING;
		sendUssdV1.iIconId.iIdentifier		=DSATTSY_ICON_ID1;
		sendUssdV1.iIconId.iQualifier		=DSATTSY_ICON_QUALIFIER1;

		if (sendUssdV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TSendUssdV6Pckg* sendUssdV6PCmdPckg = (RSat::TSendUssdV6Pckg*)aPCmd;
			RSat::TSendUssdV6&   sendUssdV6 = (*sendUssdV6PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			sendUssdV6.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			sendUssdV6.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendUssdPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySendUssdPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySendUssdPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySetUpCallPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySetUpCallPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySetUpCallPCmd called"));
		RSat::TSetUpCallV1Pckg* aPCmdPckg = (RSat::TSetUpCallV1Pckg*)aPCmd;
		RSat::TSetUpCallV1& setUpCallV1 = (*aPCmdPckg)();

		setUpCallV1.SetPCmdNumber(DSATTSY_SET_UP_CALL_NUMBER);
		
		setUpCallV1.iType								=DSATTSY_SET_UP_CALL_TYPE;
		setUpCallV1.iAlphaIdConfirmationPhase.iAlphaId	=DSATTSY_ALPHA_ID1;
		setUpCallV1.iAlphaIdConfirmationPhase.iStatus	=DSATTSY_ALPHA_ID1_STATUS;
		setUpCallV1.iIconIdConfirmationPhase.iIdentifier=DSATTSY_ICON_ID1;
		setUpCallV1.iIconIdConfirmationPhase.iQualifier	=DSATTSY_ICON_QUALIFIER1;
		setUpCallV1.iAddress.iTypeOfNumber				=DSATTSY_TON;
		setUpCallV1.iAddress.iNumberPlan				=DSATTSY_NPI;
		setUpCallV1.iAddress.iTelNumber					=DSATTSY_TEL_NUMBER;
		setUpCallV1.iCapabilityConfigParams				=DSATTSY_CCP1;
		setUpCallV1.iSubAddress							=DSATTSY_SUBADDRESS;
		setUpCallV1.iDuration.iTimeUnit					=DSATTSY_TIME_UNIT;
		setUpCallV1.iDuration.iNumOfUnits				=DSATTSY_NUM_OF_UNITS;
		setUpCallV1.iAlphaIdCallSetUpPhase.iAlphaId		=DSATTSY_ALPHA_ID2;		
		setUpCallV1.iAlphaIdCallSetUpPhase.iStatus		=DSATTSY_ALPHA_ID2_STATUS;		
		setUpCallV1.iIconIdCallSetUpPhase.iIdentifier	=DSATTSY_ICON_ID2;
		setUpCallV1.iIconIdCallSetUpPhase.iQualifier	=DSATTSY_ICON_QUALIFIER2;

		if ((setUpCallV1.ExtensionId() == RSat::KSatV5)  ||
		   (setUpCallV1.ExtensionId() == RSat::KSatV6))
			{
			RSat::TSetUpCallV5Pckg* aV5PCmdPckg = (RSat::TSetUpCallV5Pckg*)aPCmd;
			RSat::TSetUpCallV5&   setUpCallV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			setUpCallV5.iTextAttributeConfirmationPhase.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			setUpCallV5.iTextAttributeConfirmationPhase.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			setUpCallV5.iTextAttributeCallSetUpPhase.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			setUpCallV5.iTextAttributeCallSetUpPhase.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}

		if (setUpCallV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TSetUpCallV6Pckg* setUpCallV6PCmdPckg = (RSat::TSetUpCallV6Pckg*)aPCmd;
			RSat::TSetUpCallV6&   setUpCallV6 = (*setUpCallV6PCmdPckg)();
			
			setUpCallV6.iFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1 ;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}

TInt CSatDSatTsy::NotifySetUpCallPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySetUpCallPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySetUpCallPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyRefreshPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyRefreshPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyRefreshPCmd called"));
		RSat::TRefreshV1Pckg* aPCmdPckg = (RSat::TRefreshV1Pckg*)aPCmd;
		RSat::TRefreshV1& refreshV1 = (*aPCmdPckg)();
		
		refreshV1.SetPCmdNumber(DSATTSY_REFRESH_NUMBER);

		refreshV1.iType	= DSATTSY_REFRESH_TYPE;
 		refreshV1.iFileList.Append(DSATTSY_REFRESH_ADN_EF);
 		refreshV1.iFileList.Append(DSATTSY_REFRESH_FDN_EF);


		if (refreshV1.ExtensionId()==RSat::KSatV2)
			{
			RSat::TRefreshV2Pckg* aV2PCmdPckg = (RSat::TRefreshV2Pckg*)aPCmd;
			RSat::TRefreshV2& refreshV2 = (*aV2PCmdPckg)();
			
			refreshV2.iAid=DSATTSY_AID;
			}

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}

TInt CSatDSatTsy::NotifyRefreshPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyRefreshPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyRefreshPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySetUpEventListPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySetUpEventListPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySetUpEventListPCmd called"));
		RSat::TSetUpEventListV1Pckg* aPCmdPckg = (RSat::TSetUpEventListV1Pckg*)aPCmd;
		RSat::TSetUpEventListV1& setUpEventListV1 = (*aPCmdPckg)();
		
		setUpEventListV1.SetPCmdNumber(DSATTSY_SET_UP_EVENT_LIST_NUMBER);

		setUpEventListV1.iType		=DSATTSY_SET_UP_EVENT_LIST_TYPE;
		setUpEventListV1.iEvents	=DSATTSY_EVENTS;

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySetUpEventListPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySetUpEventListPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySetUpEventListPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySetUpIdleModeTextPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySetUpIdleModeTextPCmd++)
		{
 		// First Request: set idle text mode to max length Unicode string
 		if (iSetUpIdleModeTextCommandCount++ == 0)
 			{		
			LOGTEXT(_L8("CSatDSatTsy::NotifySetUpIdleModeTextPCmd called"));
			RSat::TSetUpIdleModeTextV1Pckg* aPCmdPckg = (RSat::TSetUpIdleModeTextV1Pckg*)aPCmd;
			RSat::TSetUpIdleModeTextV1& setUpIdleModeTextV1 = (*aPCmdPckg)();
			
			setUpIdleModeTextV1.SetPCmdNumber(DSATTSY_SET_UP_IDLE_MODE_TEXT_NUMBER);

			setUpIdleModeTextV1.iType				=DSATTSY_IDLE_MODE_TXT_TYPE;
			setUpIdleModeTextV1.iText				=DSATTSY_IDLE_MODE_TEXT;
			setUpIdleModeTextV1.iIconId.iIdentifier	=DSATTSY_ICON_ID1;
			setUpIdleModeTextV1.iIconId.iQualifier	=DSATTSY_ICON_QUALIFIER1;

			setUpIdleModeTextV1.iCodingScheme		=DSATTSY_TEXT_ENCODING_SCHEME;
			
			if ((setUpIdleModeTextV1.ExtensionId() == RSat::KSatV5)||
 			   (setUpIdleModeTextV1.ExtensionId() == RSat::KSatV6))
				{
				RSat::TSetUpIdleModeTextV5Pckg* aV5PCmdPckg = (RSat::TSetUpIdleModeTextV5Pckg*)aPCmd;
				RSat::TSetUpIdleModeTextV5& setUpIdleModeTextV5 = (*aV5PCmdPckg)();
				
				const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
				
				setUpIdleModeTextV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
				setUpIdleModeTextV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
				}			

			if (setUpIdleModeTextV1.ExtensionId() == RSat::KSatV6)
				{
				RSat::TSetUpIdleModeTextV6Pckg* setUpIdleModeTextV6PCmdPckg = (RSat::TSetUpIdleModeTextV6Pckg*)aPCmd;
				RSat::TSetUpIdleModeTextV6& setUpIdleModeTextV6 = (*setUpIdleModeTextV6PCmdPckg)();
				
				setUpIdleModeTextV6.iFrameId=DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
				}
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else // second request: set idle text mode to max length 8-bit string
			{
			// reset command count
			iSetUpIdleModeTextCommandCount = 0;
			
			LOGTEXT(_L8("CSatDSatTsy::NotifySetUpIdleModeTextPCmd called"));
			RSat::TSetUpIdleModeTextV1Pckg* aPCmdPckg = (RSat::TSetUpIdleModeTextV1Pckg*)aPCmd;
			RSat::TSetUpIdleModeTextV1& setUpIdleModeTextV1 = (*aPCmdPckg)();
			
			setUpIdleModeTextV1.SetPCmdNumber(DSATTSY_SET_UP_IDLE_MODE_TEXT_NUMBER);
   
			setUpIdleModeTextV1.iType				=DSATTSY_IDLE_MODE_TXT_TYPE;
			CnvUtfConverter::ConvertToUnicodeFromUtf8(setUpIdleModeTextV1.iText,DSATTSY_IDLE_MODE_TEXT_8BIT);
 			setUpIdleModeTextV1.iIconId.iIdentifier	=DSATTSY_ICON_ID1;
 			setUpIdleModeTextV1.iIconId.iQualifier	=DSATTSY_ICON_QUALIFIER1;
 			
 			setUpIdleModeTextV1.iCodingScheme		=DSATTSY_TEXT_ENCODING_SCHEME_8;
 			
 			if ((setUpIdleModeTextV1.ExtensionId() == RSat::KSatV5)  ||
 			   (setUpIdleModeTextV1.ExtensionId() == RSat::KSatV6))
				{
				RSat::TSetUpIdleModeTextV5Pckg* aV5PCmdPckg = (RSat::TSetUpIdleModeTextV5Pckg*)aPCmd;
				RSat::TSetUpIdleModeTextV5& setUpIdleModeTextV5 = (*aV5PCmdPckg)();
				
				const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
				
				setUpIdleModeTextV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
				setUpIdleModeTextV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
				}
			
			if (setUpIdleModeTextV1.ExtensionId() == RSat::KSatV6)
				{
				RSat::TSetUpIdleModeTextV6Pckg* setUpIdleModeTextV6PCmdPckg = (RSat::TSetUpIdleModeTextV6Pckg*)aPCmd;
				RSat::TSetUpIdleModeTextV6& setUpIdleModeTextV6 = (*setUpIdleModeTextV6PCmdPckg)();
				
				setUpIdleModeTextV6.iFrameId=DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
				}
					
 			ReqCompleted(aTsyReqHandle,KErrNone);		
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySetUpIdleModeTextPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySetUpIdleModeTextPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySetUpIdleModeTextPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendDtmfPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySendDtmfPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySendDtmfPCmd called"));
		RSat::TSendDtmfV1Pckg* aPCmdPckg = (RSat::TSendDtmfV1Pckg*)aPCmd;
		RSat::TSendDtmfV1& sendDtmfV1 = (*aPCmdPckg)();
		
		sendDtmfV1.SetPCmdNumber(DSATTSY_SEND_DTMF_NUMBER);

		sendDtmfV1.iAlphaId.iAlphaId	=DSATTSY_ALPHA_ID1;
		sendDtmfV1.iAlphaId.iStatus		=DSATTSY_ALPHA_ID1_STATUS;
		sendDtmfV1.iDtmfString			=DSATTSY_DTMF_STRING;
		sendDtmfV1.iIconId.iIdentifier	=DSATTSY_ICON_ID1;
		sendDtmfV1.iIconId.iQualifier	=DSATTSY_ICON_QUALIFIER1;	

		if ((sendDtmfV1.ExtensionId() == RSat::KSatV5) ||
		    (sendDtmfV1.ExtensionId() == RSat::KSatV6))	
			{
			RSat::TSendDtmfV5Pckg* aV5PCmdPckg = (RSat::TSendDtmfV5Pckg*)aPCmd;
			RSat::TSendDtmfV5&   sendDtmfV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			sendDtmfV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			sendDtmfV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}

		if (sendDtmfV1.ExtensionId() == RSat::KSatV6)
			{
			RSat::TSendDtmfV6Pckg* sendDtmfV6PCmdPckg = (RSat::TSendDtmfV6Pckg*)aPCmd;
			RSat::TSendDtmfV6&   sendDtmfV6 = (*sendDtmfV6PCmdPckg)();
			
			sendDtmfV6.iFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendDtmfPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySendDtmfPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySendDtmfPCmd=0;
	return KErrNone;
	}

//
// New Typhoon and Jetstream API
//

TInt CSatDSatTsy::UsatClientReadyIndication(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::UsatClientReadyIndication called"));
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyPerformCardApduPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyPerformCardApduPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyPerformCardApduPCmd called"));
		RSat::TPerformCardApduV2Pckg* aV2PCmdPckg = (RSat::TPerformCardApduV2Pckg*)aPCmd;
		RSat::TPerformCardApduV2& performCardApduV2 = (*aV2PCmdPckg)();
		
		performCardApduV2.SetPCmdNumber(DSATTSY_PERFORM_CARD_APDU_NUMBER);

		performCardApduV2.iDestination=DSATTSY_CARD_READER_1;
		performCardApduV2.iCApdu.iCla=DSATTSY_CLA;
		performCardApduV2.iCApdu.iIns=DSATTSY_INS;
		performCardApduV2.iCApdu.iLc=DSATTSY_LC;
		performCardApduV2.iCApdu.iLe=DSATTSY_LE;
		performCardApduV2.iCApdu.iP1=DSATTSY_P1;
		performCardApduV2.iCApdu.iP2=DSATTSY_P2;
		performCardApduV2.iCApdu.iData=DSATTSY_CAPDU;

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyPerformCardApduPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyPerformCardApduPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyPerformCardApduPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyPowerOffCardPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyPowerOffCardPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyPowerOffCardPCmd called"));
		RSat::TPowerOffCardV2Pckg* aV2PCmdPckg = (RSat::TPowerOffCardV2Pckg*)aPCmd;
		RSat::TPowerOffCardV2& powerOffCardV2 = (*aV2PCmdPckg)();
		
		powerOffCardV2.SetPCmdNumber(DSATTSY_POWER_OFF_CARD_NUMBER);
		powerOffCardV2.iDestination=DSATTSY_CARD_READER_1;

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyPowerOffCardPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyPowerOffCardPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyPowerOffCardPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyPowerOnCardPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyPowerOnCardPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyPowerOnCardPCmd called"));
		RSat::TPowerOnCardV2Pckg* aV2PCmdPckg = (RSat::TPowerOnCardV2Pckg*)aPCmd;
		RSat::TPowerOnCardV2& powerOnCardV2 = (*aV2PCmdPckg)();
		
		powerOnCardV2.SetPCmdNumber(DSATTSY_POWER_ON_CARD_NUMBER);
		powerOnCardV2.iDestination=DSATTSY_CARD_READER_1;
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyPowerOnCardPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyPowerOnCardPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyPowerOnCardPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetReaderStatusPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyGetReaderStatusPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyGetReaderStatusPCmd called"));
		RSat::TGetReaderStatusV2Pckg* aV2PCmdPckg = (RSat::TGetReaderStatusV2Pckg*)aPCmd;
		RSat::TGetReaderStatusV2& getReaderStatusV2 = (*aV2PCmdPckg)();
		
		getReaderStatusV2.SetPCmdNumber(DSATTSY_GET_READER_STATUS_NUMBER);
		getReaderStatusV2.iDestination=DSATTSY_CARD_READER_1;
		getReaderStatusV2.iMode=DSATTSY_GET_READER_STATUS_MODE;
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetReaderStatusPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyGetReaderStatusPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyGetReaderStatusPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyRunAtCommandPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyRunAtCommandPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyRunAtCommandPCmd called"));
		RSat::TRunAtCommandV2Pckg* aV2PCmdPckg = (RSat::TRunAtCommandV2Pckg*)aPCmd;
		RSat::TRunAtCommandV2& runAtCmdV2 = (*aV2PCmdPckg)();
		
		runAtCmdV2.SetPCmdNumber(DSATTSY_RUN_AT_COMMAND_NUMBER);
		runAtCmdV2.iAlphaId.iStatus=DSATTSY_ALPHA_ID1_STATUS;
		runAtCmdV2.iAlphaId.iAlphaId=DSATTSY_ALPHA_ID1;
		runAtCmdV2.iAtCommand=DSATTSY_AT_COMMAND;
		runAtCmdV2.iIconId.iIdentifier=DSATTSY_ICON_ID1;
		runAtCmdV2.iIconId.iQualifier=DSATTSY_ICON_QUALIFIER1;
		
		if ((runAtCmdV2.ExtensionId() == RSat::KSatV5) ||
		    (runAtCmdV2.ExtensionId() == RSat::KSatV6))	
			{
			RSat::TRunAtCommandV5Pckg* aV5PCmdPckg = (RSat::TRunAtCommandV5Pckg*)aPCmd;
			RSat::TRunAtCommandV5&   runAtCmdV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			runAtCmdV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			runAtCmdV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}

		if (runAtCmdV2.ExtensionId() == RSat::KSatV6)
			{
			RSat::TRunAtCommandV6Pckg* runAtCmdV6PCmdPckg = (RSat::TRunAtCommandV6Pckg*)aPCmd;
			RSat::TRunAtCommandV6&   runAtCmdV6 = (*runAtCmdV6PCmdPckg)();
			
			runAtCmdV6.iFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyRunAtCommandPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyRunAtCommandPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyRunAtCommandPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyLanguageNotificationPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyLanguageNotificationPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyLanguageNotificationPCmd called"));
		RSat::TLanguageNotificationV2Pckg* aV2PCmdPckg = (RSat::TLanguageNotificationV2Pckg*)aPCmd;
		RSat::TLanguageNotificationV2& languageNotificationV2 = (*aV2PCmdPckg)();
		
		languageNotificationV2.SetPCmdNumber(DSATTSY_LANGUAGE_NOTIFICATION_NUMBER);
		languageNotificationV2.iNotificationType=DSATTSY_SPECIFIC_LANGUAGE;
		languageNotificationV2.iLanguage=DSATTSY_FRENCH;

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyLanguageNotificationPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyLanguageNotificationPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyLanguageNotificationPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::GetProvisioningRefFile(const TTsyReqHandle aTsyReqHandle, const RSat::TProvisioningFileRef* aFileRef, TDes8* aFile)
/**
* GetProviosioningRefFile is called to ask the ICC to return a file name 
* that corresponds to a given file reference.
*
* This test code can only return 2 files - it distinguishes between them based on
* the length of the file reference passed. Any 1 byte ref will return file1, any
* 128 byte ref will return file2. Any other length will return "not found".
*/
	{
	if(!iGetProvisioningRefFile++)
		{
		LOGTEXT(_L8("CSatDSatTsy::GetProvisioningRefFile called"));

		// We only understand 2 references (at the moment) - use the simple minded approach and select by length

		TInt len = aFileRef->Length();

		switch (len)
			{
		case 1:
			*aFile = DSATTSY_FILE1;
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;

		case 128:
			*aFile = DSATTSY_FILE2;
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;

		default:
			ReqCompleted(aTsyReqHandle,KErrNotFound);
			break;
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::GetProvisioningRefFileCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::GetProvisioningRefFileCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);

	iGetProvisioningRefFile = 0;

	return KErrNone;
	}

TInt CSatDSatTsy::NotifyLaunchBrowserPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyLaunchBrowserPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyLaunchBrowserPCmd called"));
		RSat::TLaunchBrowserV2Pckg* aV2PCmdPckg = (RSat::TLaunchBrowserV2Pckg*)aPCmd;
		RSat::TLaunchBrowserV2& launchBrowserV2 = (*aV2PCmdPckg)();
		
		launchBrowserV2.SetPCmdNumber(DSATTSY_LAUNCH_BROWSER_NUMBER);

		launchBrowserV2.iBrowserSel			= DSATTSY_BROWSER_SELECTION_MODE;
		launchBrowserV2.iBrowserId			= DSATTSY_BROWSER_ID;
		launchBrowserV2.iUrl				= DSATTSY_URL;
		launchBrowserV2.iBearerList			= DSATTSY_BEARER_LIST;
		launchBrowserV2.iText				= DSATTSY_PROXY_NAME;
		launchBrowserV2.iAlphaId.iStatus	= DSATTSY_ALPHA_ID1_STATUS;
		launchBrowserV2.iAlphaId.iAlphaId	= DSATTSY_ALPHA_ID1;
		launchBrowserV2.iIconId.iIdentifier	= DSATTSY_ICON_ID1;
		launchBrowserV2.iIconId.iQualifier	= DSATTSY_ICON_QUALIFIER1;

		if ((launchBrowserV2.ExtensionId() == RSat::KSatV5) || (launchBrowserV2.ExtensionId() == RSat::KSatV6))
			{
			RSat::TLaunchBrowserV5Pckg* aV5PCmdPckg = (RSat::TLaunchBrowserV5Pckg*)aPCmd;
			RSat::TLaunchBrowserV5&   launchBrowserV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			launchBrowserV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			launchBrowserV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}

		if (launchBrowserV2.ExtensionId() == RSat::KSatV6)
			{
			RSat::TLaunchBrowserV6Pckg* launchBrowserV6PCmdPckg = (RSat::TLaunchBrowserV6Pckg*)aPCmd;
			RSat::TLaunchBrowserV6&   launchBrowserV6 = (*launchBrowserV6PCmdPckg)();
			
			launchBrowserV6.iFrameId=DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		RSat::TProvisioningFileRef	file;

		switch(iNotifyLaunchBrowserCount++)
			{
		case 0:	// Return 1 file ref of 1 byte length
			launchBrowserV2.ResetFileRef();
			file = DSATTSY_FILE_REF_DATA_1;
			launchBrowserV2.AddFileRef( file );
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;

		case 1:	// Return 1 file ref of 128 byte length
			launchBrowserV2.ResetFileRef();
			file = DSATTSY_FILE_REF_DATA_128;
			launchBrowserV2.AddFileRef( file );
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;

		case 2:	// Return 2 files refs of 1 byte length (each)
			launchBrowserV2.ResetFileRef();
			file = DSATTSY_FILE_REF_DATA_1;
			launchBrowserV2.AddFileRef( file );
			launchBrowserV2.AddFileRef( file );
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;

		case 3:	// Partial failure, try to add 2 file refs of 128 bytes each,
				// Second add should fail with KErrOverflow as combined length
				// is bigger than buffer 
				// - actually returns 1 file ref of 128 bytes unless unexpected result from add
			{
				launchBrowserV2.ResetFileRef();
				file = DSATTSY_FILE_REF_DATA_128;
				launchBrowserV2.AddFileRef( file );
				TInt status = launchBrowserV2.AddFileRef( file );	// Expect to fail on this one
				if (status != KErrOverflow)
					ReqCompleted(aTsyReqHandle, status);
				else
					ReqCompleted(aTsyReqHandle,KErrNone);
			}
			break;

		case 4:	 // Fall through to default case from case 4
		default: // Return 2 file refs, 1st=128 bytes long, 2nd=1 byte long
			iNotifyLaunchBrowserCount = 0;		// Reset selection count - next time through will use case 0

			launchBrowserV2.ResetFileRef();
			file = DSATTSY_FILE_REF_DATA_128;
			launchBrowserV2.AddFileRef( file );
			file = DSATTSY_FILE_REF_DATA_1;
			launchBrowserV2.AddFileRef( file );
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyLaunchBrowserPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyLaunchBrowserPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyLaunchBrowserPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyOpenChannelPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
/**
* If this function is called with PCmd type of RSat::EAnyBearer then the code will
* return a type of RSat::ECsBearer. Subsequent calls will return the sequence
* RSat::EGprsBearer, RSat::ELocalLinksBearer, RSat::EPCmdTypeNotSet. The
* sequence then restarts with RSat::ECsBearer.
*/
	{
	if(!iNotifyOpenChannelPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyOpenChannelPCmd called"));
		RSat::TOpenChannelBaseV2Pckg* aV2PCmdPckg = (RSat::TOpenChannelBaseV2Pckg*)aPCmd;
		RSat::TOpenChannelBaseV2& openChannelBaseV2 = (*aV2PCmdPckg)();
		
		openChannelBaseV2.SetPCmdNumber(DSATTSY_OPEN_CHANNEL_NUMBER);

		openChannelBaseV2.iLinkEst=DSATTSY_ON_DEMAND_ESTABLISHMENT;
		openChannelBaseV2.iReconnectionMode=DSATTSY_AUTO_RECONNECTION;
		openChannelBaseV2.iAlphaId.iStatus=DSATTSY_ALPHA_ID1_STATUS;
		openChannelBaseV2.iAlphaId.iAlphaId=DSATTSY_ALPHA_ID1;
		openChannelBaseV2.iIconId.iIdentifier=DSATTSY_ICON_ID1;
		openChannelBaseV2.iIconId.iQualifier=DSATTSY_ICON_QUALIFIER1;
		openChannelBaseV2.iBearer.iType=DSATTSY_CSD_BEARER_TYPE;
		openChannelBaseV2.iBearer.iParams=DSATTSY_BEARER_PARAMS;
		openChannelBaseV2.iBufferSize=DSATTSY_BUFFER_SIZE;
		openChannelBaseV2.iSimMeInterface.iPrtNumber=DSATTSY_PORT_1;
		openChannelBaseV2.iSimMeInterface.iTransportProto=DSATTSY_TCP_PROTO;
		openChannelBaseV2.iDestinationAddress.iType=DSATTSY_IPV4_TYPE;
		openChannelBaseV2.iDestinationAddress.iAddress=DSATTSY_ADDRESS;

		switch (openChannelBaseV2.iPCmdType)
			{
		case RSat::EPCmdTypeNotSet:
			ReqCompleted(aTsyReqHandle,KErrArgument);
			break;
		case RSat::ECsBearer:
			{
			RSat::TOpenCsChannelV2Pckg* aV2CsPCmdPckg = (RSat::TOpenCsChannelV2Pckg*)aPCmd;
			RSat::TOpenCsChannelV2& openCsChannelV2 = (*aV2CsPCmdPckg)();

			openCsChannelV2.iAddress.iTypeOfNumber=DSATTSY_TON;
			openCsChannelV2.iAddress.iNumberPlan=DSATTSY_NPI;
			openCsChannelV2.iAddress.iTelNumber=DSATTSY_TEL_NUMBER;
			openCsChannelV2.iSubAddress=DSATTSY_SUBADDRESS;
			openCsChannelV2.iDuration1.iTimeUnit=DSATTSY_TIME_UNIT;
			openCsChannelV2.iDuration1.iNumOfUnits=DSATTSY_NUM_OF_UNITS;
			openCsChannelV2.iDuration2.iTimeUnit=DSATTSY_TIME_UNIT;
			openCsChannelV2.iDuration2.iNumOfUnits=DSATTSY_TIME_UNIT;
			openCsChannelV2.iLocalAddress.iType=DSATTSY_IPV4_TYPE;
			openCsChannelV2.iLocalAddress.iAddress=DSATTSY_ADDRESS;
			openCsChannelV2.iUserLogin=DSATTSY_LOGIN;
			openCsChannelV2.iUserPassword=DSATTSY_PASSWORD;

			if ((openCsChannelV2.ExtensionId() == RSat::KSatV5)
				|| (openCsChannelV2.ExtensionId() == RSat::KSatV6))
				{
				RSat::TOpenCsChannelV5Pckg* aV5PCmdPckg = (RSat::TOpenCsChannelV5Pckg*)aPCmd;
				RSat::TOpenCsChannelV5&   openCsChannelV5 = (*aV5PCmdPckg)();
				
				const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
				
				openCsChannelV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
				openCsChannelV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
				}
		
			if (openCsChannelV2.ExtensionId() == RSat::KSatV6)
				{
				RSat::TOpenCsChannelV6Pckg* csV6PCmdPckg = (RSat::TOpenCsChannelV6Pckg*)aPCmd;
				RSat::TOpenCsChannelV6&   openCsChannelV6 = (*csV6PCmdPckg)();
				
				openCsChannelV6.iFrameId = DSATTSYV6_FRAME_IDENTIFIER1;
				}
				
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;
			}
		case RSat::EGprsBearer:
			{
			RSat::TOpenGprsChannelV2Pckg* aV2GprsPCmdPckg = (RSat::TOpenGprsChannelV2Pckg*)aPCmd;
			RSat::TOpenGprsChannelV2& openGprsChannelV2 = (*aV2GprsPCmdPckg)();
			
			openGprsChannelV2.iAccessName=DSATTSY_ACCESS_NAME;
			openGprsChannelV2.iLocalAddress.iType=DSATTSY_IPV4_TYPE;
			openGprsChannelV2.iLocalAddress.iAddress=DSATTSY_ADDRESS;
			
			if ((openGprsChannelV2.ExtensionId() == RSat::KSatV4) 
			|| (openGprsChannelV2.ExtensionId() == RSat::KSatV5)
			|| (openGprsChannelV2.ExtensionId() == RSat::KSatV6))
				{
				RSat::TOpenGprsChannelV4Pckg* aV4GprsPCmdPckg = reinterpret_cast<RSat::TOpenGprsChannelV4Pckg*>(aPCmd);
				RSat::TOpenGprsChannelV4& openGprsChannelV4 = (*aV4GprsPCmdPckg)();
				
				openGprsChannelV4.iUserLogin=DSATTSY_LOGIN;
				openGprsChannelV4.iUserPassword=DSATTSY_PASSWORD;
				}
				
			if ((openGprsChannelV2.ExtensionId() == RSat::KSatV5)
			   || (openGprsChannelV2.ExtensionId() == RSat::KSatV6))
				{
				RSat::TOpenGprsChannelV5Pckg* aV5PCmdPckg = (RSat::TOpenGprsChannelV5Pckg*)aPCmd;
				RSat::TOpenGprsChannelV5&   openGprsChannelV5 = (*aV5PCmdPckg)();
			
				const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
				openGprsChannelV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
				openGprsChannelV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
				}
				
			if (openGprsChannelV2.ExtensionId() == RSat::KSatV6)
				{
				RSat::TOpenGprsChannelV6Pckg* gprsV6PCmdPckg = (RSat::TOpenGprsChannelV6Pckg*)aPCmd;
				RSat::TOpenGprsChannelV6&   openGprsChannelV6 = (*gprsV6PCmdPckg)();
			
				openGprsChannelV6.iFrameId = DSATTSYV6_FRAME_IDENTIFIER1;
				openGprsChannelV6.iBearer.iType=DSATTSYV6_GPRS_BEARER_TYPE1;
				}
				
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;
			}
		case RSat::ELocalLinksBearer:
			{
			RSat::TOpenLocalLinksChannelV2Pckg* aV2LocalPCmdPckg = (RSat::TOpenLocalLinksChannelV2Pckg*)aPCmd;
			RSat::TOpenLocalLinksChannelV2& openLocalChannelV2 = (*aV2LocalPCmdPckg)();

			openLocalChannelV2.iDuration1.iTimeUnit=DSATTSY_TIME_UNIT;
			openLocalChannelV2.iDuration1.iNumOfUnits=DSATTSY_TIME_UNIT;
			openLocalChannelV2.iDuration2.iTimeUnit=DSATTSY_TIME_UNIT;
			openLocalChannelV2.iDuration2.iNumOfUnits=DSATTSY_TIME_UNIT;
			openLocalChannelV2.iRemoteAddress.iCoding=DSATTSY_CODING;
			openLocalChannelV2.iRemoteAddress.iAddress=DSATTSY_ADDRESS;
			openLocalChannelV2.iUserPassword=DSATTSY_PASSWORD;

			if ((openLocalChannelV2.ExtensionId() == RSat::KSatV5)
				|| (openLocalChannelV2.ExtensionId() == RSat::KSatV6))
				{
				RSat::TOpenLocalLinksChannelV5Pckg* aV5PCmdPckg = (RSat::TOpenLocalLinksChannelV5Pckg*)aPCmd;
				RSat::TOpenLocalLinksChannelV5&   openLocalChannelV5 = (*aV5PCmdPckg)();
			
				const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
				openLocalChannelV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
				openLocalChannelV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);				
				}

			if (openLocalChannelV2.ExtensionId() == RSat::KSatV6)
				{
				RSat::TOpenLocalLinksChannelV6Pckg* localV6PCmdPckg = (RSat::TOpenLocalLinksChannelV6Pckg*)aPCmd;
				RSat::TOpenLocalLinksChannelV6&   openLocalChannelV6 = (*localV6PCmdPckg)();
		
				openLocalChannelV6.iFrameId = DSATTSYV6_FRAME_IDENTIFIER1;				
				}
				
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;
			}
		case RSat::EUiccServerMode:
			{
			RSat::TOpenUiccServerModeChannelV7Pckg* aV7PCmdPckg = (RSat::TOpenUiccServerModeChannelV7Pckg*)aPCmd;
			RSat::TOpenUiccServerModeChannelV7& uiccServerModeChannelV7 = (*aV7PCmdPckg)();
			
			uiccServerModeChannelV7.iPCmdType = DSATTSY_UICC_SERVER_MODE;
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			uiccServerModeChannelV7.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			uiccServerModeChannelV7.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);

			uiccServerModeChannelV7.iFrameId=DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			
			ReqCompleted(aTsyReqHandle,KErrNone);
			break;
			}
		case RSat::EAnyBearer:
			{
			// Monitor all kind of open channel PCmd

			// Select type of channel PCmd that has 'opened' 

			switch(iNotifyAnyBearerCount++)
				{
			case 0:
					{
					// This iteration set bearer type to "CS"

					RSat::TOpenCsChannelV2Pckg* aV2CsPCmdPckg = (RSat::TOpenCsChannelV2Pckg*)aPCmd;
					RSat::TOpenCsChannelV2& openCsChannelV2 = (*aV2CsPCmdPckg)();

					openCsChannelV2.iPCmdType = RSat::ECsBearer;

					ReqCompleted(aTsyReqHandle,KErrNone);
					break;
					}
			case 1:
					{
					// This iteration set bearer type to "GPRS"

					RSat::TOpenGprsChannelV2Pckg* aV2GprsPCmdPckg = (RSat::TOpenGprsChannelV2Pckg*)aPCmd;
					RSat::TOpenGprsChannelV2& openGprsChannelV2 = (*aV2GprsPCmdPckg)();
					
					openGprsChannelV2.iPCmdType	= RSat::EGprsBearer;

					ReqCompleted(aTsyReqHandle,KErrNone);
					break;
					}
			case 2:
					{
					// This iteration set bearer type to "Local Link"

					RSat::TOpenLocalLinksChannelV2Pckg* aV2LocalPCmdPckg = (RSat::TOpenLocalLinksChannelV2Pckg*)aPCmd;
					RSat::TOpenLocalLinksChannelV2& openLocalChannelV2 = (*aV2LocalPCmdPckg)();

					openLocalChannelV2.iPCmdType = RSat::ELocalLinksBearer;

					ReqCompleted(aTsyReqHandle, KErrNone);

					break;
					}
			default:
					{
					// Next iteration bearer will be "CS"

					iNotifyAnyBearerCount = 0;

					// Current iteration will return a failure ("type not set") in base channel

					openChannelBaseV2.iPCmdType = RSat::EPCmdTypeNotSet;

					ReqCompleted(aTsyReqHandle,KErrNone);
					}
				}
			}
		default:
			ReqCompleted(aTsyReqHandle,KErrArgument);
			}
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
TInt CSatDSatTsy::GetOpenChannelPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	/**
	* This function should be called with PCmd type set to RSat::ECsBearer, 
	* RSat::EGprsBearer or RSat::ELocalLinksBearer.
	*/
	LOGTEXT(_L8("CSatDSatTsy::GetOpenChannelPCmd called"));
	RSat::TOpenChannelBaseV2Pckg* aV2PCmdPckg = (RSat::TOpenChannelBaseV2Pckg*)aPCmd;
	RSat::TOpenChannelBaseV2& openChannelV2 = (*aV2PCmdPckg)();

	switch (openChannelV2.iPCmdType)
		{
	case RSat::EPCmdTypeNotSet:
		ReqCompleted(aTsyReqHandle,KErrArgument);
		break;
	case RSat::ECsBearer:
		{
		RSat::TOpenCsChannelV2Pckg* aV2CsPCmdPckg = (RSat::TOpenCsChannelV2Pckg*)aPCmd;
		RSat::TOpenCsChannelV2& openCsChannelV2 = (*aV2CsPCmdPckg)();

		openCsChannelV2.iAddress.iTypeOfNumber=DSATTSY_TON;
		openCsChannelV2.iAddress.iNumberPlan=DSATTSY_NPI;
		openCsChannelV2.iAddress.iTelNumber=DSATTSY_TEL_NUMBER;
		openCsChannelV2.iSubAddress=DSATTSY_SUBADDRESS;
		openCsChannelV2.iDuration1.iTimeUnit=DSATTSY_TIME_UNIT;
		openCsChannelV2.iDuration1.iNumOfUnits=DSATTSY_NUM_OF_UNITS;
		openCsChannelV2.iDuration2.iTimeUnit=DSATTSY_TIME_UNIT;
		openCsChannelV2.iDuration2.iNumOfUnits=DSATTSY_TIME_UNIT;
		openCsChannelV2.iLocalAddress.iType=DSATTSY_IPV4_TYPE;
		openCsChannelV2.iLocalAddress.iAddress=DSATTSY_ADDRESS;
		openCsChannelV2.iUserLogin=DSATTSY_LOGIN;
		openCsChannelV2.iUserPassword=DSATTSY_PASSWORD;

		ReqCompleted(aTsyReqHandle,KErrNone);
		break;
		}
	case RSat::EGprsBearer:
		{
		RSat::TOpenGprsChannelV2Pckg* aV2GprsPCmdPckg = (RSat::TOpenGprsChannelV2Pckg*)aPCmd;
		RSat::TOpenGprsChannelV2& openGprsChannelV2 = (*aV2GprsPCmdPckg)();
		
		openGprsChannelV2.iAccessName=DSATTSY_ACCESS_NAME;
		openGprsChannelV2.iLocalAddress.iType=DSATTSY_IPV4_TYPE;
		openGprsChannelV2.iLocalAddress.iAddress=DSATTSY_ADDRESS;
		
		if (openGprsChannelV2.ExtensionId() == RSat::KSatV4)
			{
			RSat::TOpenGprsChannelV4Pckg* aV4GprsPCmdPckg = reinterpret_cast<RSat::TOpenGprsChannelV4Pckg*>(aPCmd);
			RSat::TOpenGprsChannelV4& openGprsChannelV4 = (*aV4GprsPCmdPckg)();
			
			openGprsChannelV4.iUserLogin=DSATTSY_LOGIN;
			openGprsChannelV4.iUserPassword=DSATTSY_PASSWORD;
			}

		ReqCompleted(aTsyReqHandle,KErrNone);
		break;
		}
	case RSat::ELocalLinksBearer:
		{
		RSat::TOpenLocalLinksChannelV2Pckg* aV2LocalPCmdPckg = (RSat::TOpenLocalLinksChannelV2Pckg*)aPCmd;
		RSat::TOpenLocalLinksChannelV2& openLocalChannelV2 = (*aV2LocalPCmdPckg)();

		openLocalChannelV2.iDuration1.iTimeUnit=DSATTSY_TIME_UNIT;
		openLocalChannelV2.iDuration1.iNumOfUnits=DSATTSY_TIME_UNIT;
		openLocalChannelV2.iDuration2.iTimeUnit=DSATTSY_TIME_UNIT;
		openLocalChannelV2.iDuration2.iNumOfUnits=DSATTSY_TIME_UNIT;
		openLocalChannelV2.iRemoteAddress.iCoding=DSATTSY_CODING;
		openLocalChannelV2.iRemoteAddress.iAddress=DSATTSY_ADDRESS;
		openLocalChannelV2.iUserPassword=DSATTSY_PASSWORD;

		ReqCompleted(aTsyReqHandle,KErrNone);
		break;
		}
	case RSat::EAnyBearer:
		{
		ReqCompleted(aTsyReqHandle,KErrArgument);
		break;
		}
	default:
		ReqCompleted(aTsyReqHandle,KErrArgument);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyOpenChannelPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyOpenChannelPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyOpenChannelPCmd=0;
	return KErrNone;
	}
	
TInt CSatDSatTsy::GetOpenChannelPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::GetOpenChannelPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}	

TInt CSatDSatTsy::NotifyCloseChannelPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyCloseChannelPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyCloseChannelPCmd called"));
		RSat::TCloseChannelV2Pckg* aV2PCmdPckg = (RSat::TCloseChannelV2Pckg*)aPCmd;
		RSat::TCloseChannelV2& closeChannelV2 = (*aV2PCmdPckg)();
		
		closeChannelV2.SetPCmdNumber(DSATTSY_CLOSE_CHANNEL_NUMBER);

		closeChannelV2.iDestination=DSATTSY_CARD_READER_1;
		closeChannelV2.iAlphaId.iStatus=DSATTSY_ALPHA_ID1_STATUS;
		closeChannelV2.iAlphaId.iAlphaId=DSATTSY_ALPHA_ID1;
		closeChannelV2.iIconId.iIdentifier=DSATTSY_ICON_ID1;
		closeChannelV2.iIconId.iQualifier=DSATTSY_ICON_QUALIFIER1;

		if ((closeChannelV2.ExtensionId() == RSat::KSatV5) ||
			(closeChannelV2.ExtensionId() == RSat::KSatV6))
			{
			RSat::TCloseChannelV5Pckg* aV5PCmdPckg = (RSat::TCloseChannelV5Pckg*)aPCmd;
			RSat::TCloseChannelV5&   closeChannelV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			closeChannelV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			closeChannelV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}

		if (closeChannelV2.ExtensionId() == RSat::KSatV6)
			{
			RSat::TCloseChannelV6Pckg* closeChannelV6PCmdPckg = (RSat::TCloseChannelV6Pckg*)aPCmd;
			RSat::TCloseChannelV6&   closeChannelV6 = (*closeChannelV6PCmdPckg)();
			
			closeChannelV6.iFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyCloseChannelPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyCloseChannelPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyCloseChannelPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyReceiveDataPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyReceiveDataPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyReceiveDataPCmd called"));
		RSat::TReceiveDataV2Pckg* aV2PCmdPckg = (RSat::TReceiveDataV2Pckg*)aPCmd;
		RSat::TReceiveDataV2& receiveDataV2 = (*aV2PCmdPckg)();
		
		receiveDataV2.SetPCmdNumber(DSATTSY_RECEIVE_DATA_NUMBER);

		receiveDataV2.iDestination=DSATTSY_CARD_READER_1;
		receiveDataV2.iAlphaId.iStatus=DSATTSY_ALPHA_ID1_STATUS;
		receiveDataV2.iAlphaId.iAlphaId=DSATTSY_ALPHA_ID1;
		receiveDataV2.iIconId.iIdentifier=DSATTSY_ICON_ID1;
		receiveDataV2.iIconId.iQualifier=DSATTSY_ICON_QUALIFIER1;
		receiveDataV2.iChannelDataLength=DSATTSY_DATA_LENGTH;

		if ((receiveDataV2.ExtensionId() == RSat::KSatV5) ||
			(receiveDataV2.ExtensionId() == RSat::KSatV6))
			{
			RSat::TReceiveDataV5Pckg* aV5PCmdPckg = (RSat::TReceiveDataV5Pckg*)aPCmd;
			RSat::TReceiveDataV5&   receiveDataV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			receiveDataV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			receiveDataV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}		

		if (receiveDataV2.ExtensionId() == RSat::KSatV6)
			{
			RSat::TReceiveDataV6Pckg* receiveDataV6PCmdPckg = (RSat::TReceiveDataV6Pckg*)aPCmd;
			RSat::TReceiveDataV6&   receiveDataV6 = (*receiveDataV6PCmdPckg)();
			
			receiveDataV6.iFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyReceiveDataPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyReceiveDataPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyReceiveDataPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendDataPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySendDataPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySendDataPCmd called"));
		RSat::TSendDataV2Pckg* aV2PCmdPckg = (RSat::TSendDataV2Pckg*)aPCmd;
		RSat::TSendDataV2& sendDataV2 = (*aV2PCmdPckg)();
		
		sendDataV2.SetPCmdNumber(DSATTSY_SEND_DATA_NUMBER);

		sendDataV2.iDestination=DSATTSY_CARD_READER_1;
		sendDataV2.iMode=DSATTSY_SEND_DATA_MODE;
		sendDataV2.iAlphaId.iStatus=DSATTSY_ALPHA_ID1_STATUS;
		sendDataV2.iAlphaId.iAlphaId=DSATTSY_ALPHA_ID1;
		sendDataV2.iIconId.iIdentifier=DSATTSY_ICON_ID1;
		sendDataV2.iIconId.iQualifier=DSATTSY_ICON_QUALIFIER1;
		sendDataV2.iChannelData=DSATTSY_CHANNEL_DATA;

		if ((sendDataV2.ExtensionId() == RSat::KSatV5) ||
			(sendDataV2.ExtensionId() == RSat::KSatV6))
			{
			RSat::TSendDataV5Pckg* aV5PCmdPckg = (RSat::TSendDataV5Pckg*)aPCmd;
			RSat::TSendDataV5&   sendDataV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			sendDataV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			sendDataV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}

		if (sendDataV2.ExtensionId() == RSat::KSatV6)
			{
			RSat::TSendDataV6Pckg* sendDataV6PCmdPckg = (RSat::TSendDataV6Pckg*)aPCmd;
			RSat::TSendDataV6&   sendDataV6 = (*sendDataV6PCmdPckg)();
			
			sendDataV6.iFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySendDataPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySendDataPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySendDataPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetChannelStatusPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyGetChannelStatusPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyGetChannelStatusPCmd called"));
		RSat::TGetChannelStatusV2Pckg* aV2PCmdPckg = (RSat::TGetChannelStatusV2Pckg*)aPCmd;
		RSat::TGetChannelStatusV2& getStatusV2 = (*aV2PCmdPckg)();
		
		getStatusV2.SetPCmdNumber(DSATTSY_GET_CHANNEL_STATUS_NUMBER);

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetChannelStatusPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyGetChannelStatusPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyGetChannelStatusPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyServiceSearchPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyServiceSearchPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyServiceSearchPCmd called"));
		RSat::TServiceSearchV2Pckg* aV2PCmdPckg = (RSat::TServiceSearchV2Pckg*)aPCmd;
		RSat::TServiceSearchV2& serviceSearchV2 = (*aV2PCmdPckg)();
		
		serviceSearchV2.SetPCmdNumber(DSATTSY_SERVICE_SEARCH_NUMBER);

		serviceSearchV2.iAlphaId.iStatus=DSATTSY_ALPHA_ID1_STATUS;
		serviceSearchV2.iAlphaId.iAlphaId=DSATTSY_ALPHA_ID1;
		serviceSearchV2.iIconId.iIdentifier=DSATTSY_ICON_ID1;
		serviceSearchV2.iIconId.iQualifier=DSATTSY_ICON_QUALIFIER1;
		serviceSearchV2.iSearch.iBearerId=DSATTSY_BEARER_TECH_INDEPENDENT;
		serviceSearchV2.iSearch.iSearchParam=DSATTSY_SERVICE_SEARCH_PARAMS;
		serviceSearchV2.iFilter.iBearerId=DSATTSY_BEARER_TECH_INDEPENDENT;
		serviceSearchV2.iFilter.iDeviceFilter=DSATTSY_FILTER_INFO;

		if ((serviceSearchV2.ExtensionId() == RSat::KSatV5) ||
			(serviceSearchV2.ExtensionId() == RSat::KSatV6))
			{
			RSat::TServiceSearchV5Pckg* aV5PCmdPckg = (RSat::TServiceSearchV5Pckg*)aPCmd;
			RSat::TServiceSearchV5&   serviceSearchV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			serviceSearchV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			serviceSearchV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}

		if (serviceSearchV2.ExtensionId() == RSat::KSatV6)
			{
			RSat::TServiceSearchV6Pckg* serviceSearchV6PCmdPckg = (RSat::TServiceSearchV6Pckg*)aPCmd;
			RSat::TServiceSearchV6&   serviceSearchV6 = (*serviceSearchV6PCmdPckg)();
			
			serviceSearchV6.iFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyServiceSearchPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyServiceSearchPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyServiceSearchPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetServiceInfoPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyGetServiceInfoPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyGetServiceInfoPCmd called"));
		RSat::TGetServiceInfoV2Pckg* aV2PCmdPckg = (RSat::TGetServiceInfoV2Pckg*)aPCmd;
		RSat::TGetServiceInfoV2& getServiceInfoV2 = (*aV2PCmdPckg)();
		
		getServiceInfoV2.SetPCmdNumber(DSATTSY_GET_SERVICE_INFO_NUMBER);

		getServiceInfoV2.iAlphaId.iStatus=DSATTSY_ALPHA_ID1_STATUS;
		getServiceInfoV2.iAlphaId.iAlphaId=DSATTSY_ALPHA_ID1;
		getServiceInfoV2.iIconId.iIdentifier=DSATTSY_ICON_ID1;
		getServiceInfoV2.iIconId.iQualifier=DSATTSY_ICON_QUALIFIER1;
		getServiceInfoV2.iAttributeInfo.iBearerId=DSATTSY_BEARER_TECH_INDEPENDENT;
		getServiceInfoV2.iAttributeInfo.iAttributeInfo=DSATTSY_ATTRIBUTE_INFO;

		if ((getServiceInfoV2.ExtensionId() == RSat::KSatV5) ||
			(getServiceInfoV2.ExtensionId() == RSat::KSatV6))
			{
			RSat::TGetServiceInfoV5Pckg* aV5PCmdPckg = (RSat::TGetServiceInfoV5Pckg*)aPCmd;
			RSat::TGetServiceInfoV5&   getServiceInfoV5 = (*aV5PCmdPckg)();
			
			const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
			
			getServiceInfoV5.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
			getServiceInfoV5.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
			}

		if (getServiceInfoV2.ExtensionId() == RSat::KSatV6)
			{
			RSat::TGetServiceInfoV6Pckg* getServiceInfoV6PCmdPckg = (RSat::TGetServiceInfoV6Pckg*)aPCmd;
			RSat::TGetServiceInfoV6&   getServiceInfoV6 = (*getServiceInfoV6PCmdPckg)();
			
			getServiceInfoV6.iFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
			}
			
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetServiceInfoPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyGetServiceInfoPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyGetServiceInfoPCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyDeclareServicePCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyDeclareServicePCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyDeclareServicePCmd called"));
		RSat::TDeclareServiceV2Pckg* aV2PCmdPckg = (RSat::TDeclareServiceV2Pckg*)aPCmd;
		RSat::TDeclareServiceV2& declareServiceV2 = (*aV2PCmdPckg)();
		
		declareServiceV2.SetPCmdNumber(DSATTSY_DECLARE_SERVICE_NUMBER);

		declareServiceV2.iType=DSATTSY_DECLARE_SERVICE_TYPE;
		declareServiceV2.iServiceRecord.iBearerId=DSATTSY_BEARER_NOT_PRESENT;
		declareServiceV2.iServiceRecord.iServiceId=DSATTSY_NULL_BEARER_ID;
		declareServiceV2.iServiceRecord.iRecord=DSATTSY_NULL_NARROW_BUF;
		declareServiceV2.iInterface.iPrtNumber=DSATTSY_PORT_1;
		declareServiceV2.iInterface.iTransportProto=DSATTSY_TCP_PROTO;

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyDeclareServicePCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyDeclareServicePCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyDeclareServicePCmd=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyProactiveSimSessionStart(const TTsyReqHandle aTsyReqHandle)
	{
	if(!iNotifyProactiveSimSessionStart++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyProactiveSimSessionStart called"));
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
TInt CSatDSatTsy::NotifyProactiveSimSessionStartCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyProactiveSimSessionStartCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyProactiveSimSessionStart=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyProactiveSimSessionEnd(const TTsyReqHandle aTsyReqHandle)
	{
	if(!iNotifyProactiveSimSessionEnd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyProactiveSimSessionEnd called"));
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyProactiveSimSessionEndCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyProactiveSimSessionEndCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyProactiveSimSessionEnd=0;
	return KErrNone;
	}
	
TInt CSatDSatTsy::NotifyTimerExpiration(const TTsyReqHandle aTsyReqHandle, TDes8* aTimerDetails)
	{
	if(!iNotifyTimerExpiration++)
		{
		LOGTEXT(_L8("CSatDSatTsy::Notify Timer expiration called"));
		RSat::TTimerExpirationV3Pckg* cmdPckg = (RSat::TTimerExpirationV3Pckg*)aTimerDetails;
		RSat::TTimerExpirationV3& cmdV3 = (*cmdPckg)();	
		
		cmdV3.iTimerId = RSat::KTimer1;
		cmdV3.iTimerValue.iTimeUnit = RSat::ESeconds;
		cmdV3.iTimerValue.iNumOfUnits = 10;
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyTimerExpirationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyTimerExpirationCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyTimerExpiration=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySmsPpDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aSmsPpDownloadDetails)
	{
	if(!iNotifySmsPpDownload++)
		{
		LOGTEXT(_L8("CSatDSatTsy::Notify Sms Pp Download called"));
		RSat::TSmsPpDownloadV3Pckg* cmdPckg = (RSat::TSmsPpDownloadV3Pckg*)aSmsPpDownloadDetails;
		RSat::TSmsPpDownloadV3& cmdV3 = (*cmdPckg)();	
		
		cmdV3.iDeviceId=RSat::KEarPiece;
		cmdV3.iRpAddress.iTypeOfNumber	=DSATTSY_TON;
		cmdV3.iRpAddress.iNumberPlan	=DSATTSY_NPI;
		cmdV3.iRpAddress.iTelNumber		=DSATTSY_TEL_NUMBER;
		cmdV3.iPdu=DSATTSY_SMS_TPDU; 
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyTsyStateUpdated(const TTsyReqHandle aTsyReqHandle,RSat::TPCmd* aPCmd)
	{
	if(!iNotifyTsyStateUpdated++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyTsyStateUpdated called"));
		*aPCmd = DSATTSY_SENDSS;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
TInt CSatDSatTsy::NotifyTsyStateUpdatedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyTsyStateUpdatedCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}	

TInt CSatDSatTsy::NotifyRefreshRequired(const TTsyReqHandle aTsyReqHandle)
	{
	if(!iNotifyRefreshRequired++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyRefreshRequired called"));

		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}
	
TInt CSatDSatTsy::NotifyRefreshRequired(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyRefreshRequired++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyRefreshRequired called (with param)"));

		RSat::TRefreshV1Pckg* refreshPckg = (RSat::TRefreshV1Pckg*)aPCmd;
		RSat::TRefreshV1& refreshV1 = (*refreshPckg)();
		
		refreshV1.SetPCmdNumber(DSATTSY_REFRESH_NUMBER);

		refreshV1.iType	= DSATTSY_REFRESH_TYPE;
 		refreshV1.iFileList.Append(DSATTSY_REFRESH_FDN_EF);
 		refreshV1.iFileList.Append(DSATTSY_REFRESH_ADN_EF);

		if (refreshV1.ExtensionId()==RSat::KSatV2)
			{
			RSat::TRefreshV2Pckg* aV2PCmdPckg = (RSat::TRefreshV2Pckg*)aPCmd;
			RSat::TRefreshV2& refreshV2 = (*aV2PCmdPckg)();
			
			refreshV2.iAid=DSATTSY_AID;
			}

		ReqCompleted(aTsyReqHandle,KErrNone);
		}		
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}

TInt CSatDSatTsy::NotifyRefreshRequiredCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyRefreshRequiredCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyRefreshRequired=0;
	return KErrNone;
	}

TInt CSatDSatTsy::RefreshAllowed(const TTsyReqHandle aTsyReqHandle, TDes8* aRefreshAllowedRsp)
	{
	RSat::TRefreshRspV1Pckg* aRspPckg = (RSat::TRefreshRspV1Pckg*)aRefreshAllowedRsp;
	RSat::TRefreshRspV1& rspV1 = (*aRspPckg)();

	if((rspV1.iGeneralResult!=DSATTSY_PCMD_RESULT_SUCCESS)
		||(rspV1.iInfoType!=DSATTSY_PCMD_RSP_NO_INFO)
		||(rspV1.iAdditionalInfo!=DSATTSY_NULL_BUF))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iRefreshAllowed++)
		ReqCompleted(aTsyReqHandle,KErrNone);
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::RefreshAllowedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::RefreshAllowedCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyRefreshRequired=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifySmsPpDownloadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySmsPpDownloadCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySmsPpDownload=0;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyCbDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aCbDownloadDetails)
	{
	if(!iNotifyCbDownload++)
		{
		LOGTEXT(_L8("CSatDSatTsy::Notify CB Download called"));
		RSat::TCbDownloadV3Pckg* cmdPckg = (RSat::TCbDownloadV3Pckg*)aCbDownloadDetails;
		RSat::TCbDownloadV3& cmdV3 = (*cmdPckg)();		
		
		cmdV3.iDeviceId=RSat::KEarPiece;
		cmdV3.iPdu= DSATTSY_CB_PDU; 
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
TInt CSatDSatTsy::NotifyCbDownloadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyCbDownloadCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyCbDownload=0;
	return KErrNone;
	}
	
TInt CSatDSatTsy::NotifyTimerManagtPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyTimerManagtPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyTimerManagtPCmd called"));
		RSat::TTimerMgmtV3Pckg* aV3PCmdPckg = (RSat::TTimerMgmtV3Pckg*)aPCmd;
		RSat::TTimerMgmtV3& timerMngtV3 = (*aV3PCmdPckg)();
		
		timerMngtV3.iTimerId = RSat::KTimer1;
		timerMngtV3.iTimerValue.iTimeUnit = RSat::ESeconds;
		timerMngtV3.iTimerValue.iNumOfUnits = 10;
		timerMngtV3.iOperation = RSat::EStartTimer;
				
		ReqCompleted(aTsyReqHandle,KErrNone);	
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyTimerManagtPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyTmerManagtPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyTimerManagtPCmd=0;
	return KErrNone;
	}
	
TInt CSatDSatTsy::NotifyLocalInfoPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyLocalInfoPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyLocalInfoPCmd called"));
		RSat::TLocalInfoV3Pckg* aV3PCmdPckg = (RSat::TLocalInfoV3Pckg*)aPCmd;	
		RSat::TLocalInfoV3& localInfoV3 = (*aV3PCmdPckg)();
	
		localInfoV3.iDevideId = RSat::KKeypad;
		localInfoV3.iInfoType = RSat::KProvideLocalInfo;
		if(localInfoV3.ExtensionId()== RSat::KSatV6)
			{
			RSat::TLocalInfoV6Pckg* localInfoV6PCmdPckg = (RSat::TLocalInfoV6Pckg*)aPCmd;	
			RSat::TLocalInfoV6& localInfoV6 = (*localInfoV6PCmdPckg)();
			localInfoV6.iUtranMeasurementQualifier=DSATTSYV6_LOCAL_INFOV6;
			localInfoV6.iInfoType = RSat::KProvideLocalInfoSearchMode;
			}
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyLocalInfoPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyLocalInfoPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyLocalInfoPCmd=0;
	return KErrNone;
	}
	
TInt CSatDSatTsy::NotifyPollingIntervalPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyPollingIntervalPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyPollingIntervalPCmd called"));
		RSat::TPollingIntervalV3Pckg* aV3PCmdPckg = (RSat::TPollingIntervalV3Pckg*)aPCmd;	
		RSat::TPollingIntervalV3& pollingIntvV3 = (*aV3PCmdPckg)();
		
		pollingIntvV3.iIntv.iTimeUnit = RSat::ESeconds;
		pollingIntvV3.iIntv.iNumOfUnits = DSATTSY_NUM_OF_UNITS;
		pollingIntvV3.iDestination=RSat::KEarPiece;
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
	
TInt CSatDSatTsy::NotifyPollingIntervalPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	
	LOGTEXT(_L8("CSatDSatTsy::NotifyPollingIntervalPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyPollingIntervalPCmd=0;
	return KErrNone;
	}
	
TInt CSatDSatTsy::NotifyPollingOffPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyPollingOffPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyPollingOffPCmd called"));
		RSat::TPollingOffV3Pckg* aV3PCmdPckg = (RSat::TPollingOffV3Pckg*)aPCmd;	
		RSat::TPollingOffV3& pollingIntvV3 = (*aV3PCmdPckg)();
		
		pollingIntvV3.iIntv.iTimeUnit = RSat::ESeconds;
		pollingIntvV3.iIntv.iNumOfUnits = 20;
		pollingIntvV3.iDestination=RSat::KNetwork;
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyPollingOffPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	
	LOGTEXT(_L8("CSatDSatTsy::NotifyPollingOffPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyPollingOffPCmd=0;
	return KErrNone;
	}



TInt CSatDSatTsy::NotifyRetrieveMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd) 
	{
	if(!iNotifyRetrieveMultimediaMsgPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyRetrieveMultimediaMsgPCmd called"));
		RSat::TRetrieveMultimediaMessageV6Pckg* retrieveMmV6PCmdPckg = (RSat::TRetrieveMultimediaMessageV6Pckg*)aPCmd;	
		RSat::TRetrieveMultimediaMessageV6& retrieveMMV6 = (*retrieveMmV6PCmdPckg)();
		
		const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
		retrieveMMV6.SetPCmdNumber(DSATTSY_RETRIEVE_MULTIMEDIA_MESSAGE);
		retrieveMMV6.iDestination = DSATTSYV6_DEVICE_ID1;
		retrieveMMV6.iAlphaId.iAlphaId= DSATTSYV6_ALPHAID1;
		retrieveMMV6.iAlphaId.iStatus = DSATTSYV6_ALPHAID_STATUS2;
		retrieveMMV6.iIconId.iIdentifier = DSATTSYV6_ICON_ID1;
		retrieveMMV6.iIconId.iQualifier =  DSATTSYV6_ICON_QUALIFIER3;
		retrieveMMV6.iMultimediaContentId = DSATTSYV6_MULTIMEDIA_CONTENT_ID1;
		retrieveMMV6.iMultimediaMessageRef = DSATTSYV6_MULTIMEDIA_MESSAGE_REF1;
		retrieveMMV6.iMultimediaMessageId = DSATTSYV6_MULTIMEDIA_MESSAGE_ID1;
		retrieveMMV6.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
		retrieveMMV6.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyRetrieveMultimediaMsgPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyRetrieveMultimediaMsgPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyRetrieveMultimediaMsgPCmd=0;
	return KErrNone;
	}



TInt CSatDSatTsy::NotifySubmitMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySubmitMultimediaMsgPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySubmitMultimediaMsgPCmd called"));
		RSat::TSubmitMultimediaMessageV6Pckg* submitMMV6PCmdPckg = (RSat::TSubmitMultimediaMessageV6Pckg*)aPCmd;	
		RSat::TSubmitMultimediaMessageV6& submitMMV6 = (*submitMMV6PCmdPckg)();
		
		const TUint8 textAttributeData[DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH] = DSATTSY_TEXT_ATTRIBUTE_DATA;
		submitMMV6.SetPCmdNumber(DSATTSY_SUBMIT_MULTIMEDIA_MESSAGE);
		submitMMV6.iDestination = DSATTSYV6_DEVICE_ID1;
		submitMMV6.iAlphaId.iAlphaId= DSATTSYV6_ALPHAID1;
		submitMMV6.iAlphaId.iStatus = DSATTSYV6_ALPHAID_STATUS2;
		submitMMV6.iIconId.iIdentifier = DSATTSYV6_ICON_ID1;
		submitMMV6.iIconId.iQualifier =  DSATTSYV6_ICON_QUALIFIER3;
		submitMMV6.iSubmissionFile = DSATTSYV6_SUBMISSION_FILE1;
		submitMMV6.iTextAttribute.iStatus = DSATTSY_TEXT_ATTRIBUTE_STATUS;
		submitMMV6.iMultimediaMessageId = DSATTSYV6_MULTIMEDIA_MESSAGE_ID1;
		submitMMV6.iTextAttribute.iTextAttributeData.Append(textAttributeData,DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH);
		
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}
	
TInt CSatDSatTsy::NotifySubmitMultimediaMsgPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifySubmitMultimediaMsgPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySubmitMultimediaMsgPCmd=0;
	return KErrNone;	
	}
	


TInt CSatDSatTsy::NotifyDisplayMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyDisplayMultimediaMsgPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyDisplayMultimediaMsgPCmd called"));
		RSat::TDisplayMultimediaMessageV6Pckg* displayMMV6PCmdPckg = (RSat::TDisplayMultimediaMessageV6Pckg*)aPCmd;	
		RSat::TDisplayMultimediaMessageV6& displayMMV6 = (*displayMMV6PCmdPckg)();	
		
		displayMMV6.SetPCmdNumber(DSATTSY_DISPLAY_MULTIMEDIA_MESSAGE);
		displayMMV6.iDestination = DSATTSYV6_DEVICE_ID3;
		displayMMV6.iDisplayPriority = DSATTSYV6_DISPLAY_PRIORITY1;
		displayMMV6.iSubmissionFile = DSATTSYV6_SUBMISSION_FILE1;
		displayMMV6.iMultimediaMessageId = DSATTSYV6_MULTIMEDIA_MESSAGE_ID1;
		displayMMV6.iImmediateRsp = DSATTSYV6_IMMEDIATE_RESPONSE1;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyDisplayMultimediaMsgPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyDisplayMultimediaMsgPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyDisplayMultimediaMsgPCmd=0;
	return KErrNone;	
	}


TInt CSatDSatTsy::NotifySetFramesPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifySetFramesPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifySetFramesPCmd called"));
		RSat::TSetFramesV6Pckg* setFramesV6PCmdPckg = (RSat::TSetFramesV6Pckg*)aPCmd;	
		RSat::TSetFramesV6& setFramesV6 = (*setFramesV6PCmdPckg)();	
		
		setFramesV6.SetPCmdNumber(DSATTSY_SET_FRAMES);
		setFramesV6.iDestination = DSATTSYV6_DEVICE_ID1;
        setFramesV6.iDefaultFrameId = DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1;
        setFramesV6.iFrameId = DSATTSYV6_FRAME_IDENTIFIER1;
        setFramesV6.iFrameLayout.iFramesLayout = DSATTSYV6_FRAME_LAYOUT_FORMAT1;
        setFramesV6.iFrameLayout.iFrameLayoutBuf = DSATTSYV6_FRAME_LAYOUT1;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone; 
	}
	
TInt CSatDSatTsy::NotifySetFramesPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::SetFramesPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifySetFramesPCmd=0;
	return KErrNone;	
	}
	

TInt CSatDSatTsy::NotifyGetFramesStatusPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	if(!iNotifyGetFramesStatusPCmd++)
		{
		LOGTEXT(_L8("CSatDSatTsy::NotifyGetFramesStatusPCmd called"));
		RSat::TGetFramesStatusV6Pckg* getFramesStatusV6PCmdPckg = (RSat::TGetFramesStatusV6Pckg*)aPCmd;	
		RSat::TGetFramesStatusV6& getFramesStatusV6 = (*getFramesStatusV6PCmdPckg)();	
		
		getFramesStatusV6.SetPCmdNumber(DSATTSY_GET_FRAMES_STATUS);
		getFramesStatusV6.iDestination = DSATTSYV6_DEVICE_ID1;
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::NotifyGetFramesStatusPCmdCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::NotifyGetFramesStatusPCmdCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iNotifyGetFramesStatusPCmd=0;
	return KErrNone;	
	}
	


TInt CSatDSatTsy::MmsNotificationDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	LOGTEXT(_L8("CSatDSatTsy::MmsNotificationDownload called"));
	RSat::TMmsNotificationDownloadV6Pckg* mMSNotificationDownloadV6PCmdPckg = (RSat::TMmsNotificationDownloadV6Pckg*)aPCmd;	
	RSat::TMmsNotificationDownloadV6& mMSNotificationDownloadV6 = (*mMSNotificationDownloadV6PCmdPckg)();	
		
	if((mMSNotificationDownloadV6.iDeviceId != DSATTSYV6_DEVICE_ID1) || 
	   (mMSNotificationDownloadV6.iLastEnvelope != DSATTSYV6_LAST_ENVELOPE1) || 
	   (mMSNotificationDownloadV6.iMMSNotification != DSATTSYV6_MMS_NOTIFICATION1))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);			
		}
	else if(!iMmsNotificationDownload++)
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;	
	}

TInt CSatDSatTsy::MmsNotificationDownloadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::MmsNotificationDownloadCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iMmsNotificationDownload=0;
	return KErrNone;	
	}



TInt CSatDSatTsy::MmsTransferStatus(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd)
	{
	LOGTEXT(_L8("CSatDSatTsy::MmsTransferStatus called"));
	RSat::TMmsTransferStatusV6Pckg* mMSTransferStatusV6PCmdPckg = (RSat::TMmsTransferStatusV6Pckg*)aPCmd;	
	RSat::TMmsTransferStatusV6& mMSTransferStatusV6 = (*mMSTransferStatusV6PCmdPckg)();	
		
	if((mMSTransferStatusV6.iDeviceId != DSATTSYV6_DEVICE_ID3) || 
	   (mMSTransferStatusV6.iMultimediaMessageId !=DSATTSYV6_MULTIMEDIA_MESSAGE_ID1) || 
	   (mMSTransferStatusV6.iMultimediaMessageStatus !=DSATTSYV6_MULTIMEDIA_MESSAGE_STATUS))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	else if(!iMmsTransferStatus++)
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}

	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CSatDSatTsy::MmsTransferStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::MMTransferStatusCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iMmsTransferStatus=0;
	return KErrNone;	
	}

	

TInt CSatDSatTsy::UssdDataDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aUssdDataDownload)
	{
	LOGTEXT(_L8("CSatDSatTsy::UssdDataDownload called"));
	RSat::TUssdDataDownloadV6Pckg* ussdDataDownloadPckg = (RSat::TUssdDataDownloadV6Pckg*)aUssdDataDownload;
	RSat::TUssdDataDownloadV6& ussdDataDownloadV6 = (*ussdDataDownloadPckg)();	
		
	if((ussdDataDownloadV6.iDeviceId != DSATTSYV6_DEVICE_ID1) || 
	   (ussdDataDownloadV6.iUssdString.iUssdString != DSATTSY_USSD_STRING))
		{
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
		}
	else if(!iUssdDataDownload++)
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}

	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}	

TInt CSatDSatTsy::UssdDataDownloadCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSatDSatTsy::UssdDataDownloadCancel called"));
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	iUssdDataDownload=0;
	return KErrNone;	
	}



TSecurityPolicy CSatDSatTsy::GetRequiredPlatSecCaps(const TInt aIpc)
	{
	switch(aIpc)
		{
	case ETestCustomIPC1:
		return TSecurityPolicy(ECapabilityNetworkServices);
	default:
		//For any non-Custom IPCs return a policy that will always cause the capability check to pass
		return TSecurityPolicy(TSecurityPolicy::EAlwaysPass);
		}
	}	

