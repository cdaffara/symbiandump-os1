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
// Dummy Sat TSY classes.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __SATTSY_H
#define __SATTSY_H

#include <e32base.h>
#include <et_phone.h>
#include "Etelsat.h"
#include "coretsy.h"
#include "../tetel/Tetelcs.h"


/**
 * Dummy SAT TSY class
 */
class CSatDSatTsy : public CSubSessionExtDummyBase
	{
public:
	static CSatDSatTsy* NewL(CPhoneFactoryDummyBase* aFac);
	CSatDSatTsy(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CSatDSatTsy();

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TSecurityPolicy GetRequiredPlatSecCaps(const TInt aIpc);

private:
	TInt NotifyProactiveSimSessionEnd(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyProactiveSimSessionEndCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetIcon(const TTsyReqHandle aTsyReqHandle, TUint8* aRecordNumber, RSat::TIcon* aIconEf);
	TInt GetIconCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetImageInstance(const TTsyReqHandle aTsyReqHandle, TDesC8* aCallParams, TDes8* aInstance);
	TInt GetImageInstanceCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetClut(const TTsyReqHandle aTsyReqHandle, RSat::TInstanceNumberAndOffset* aClutInfo, TDes8* aClut);
	TInt GetClutCancel(const TTsyReqHandle aTsyReqHandle);
	TInt TerminalRsp(const TTsyReqHandle aTsyReqHandle, RSat::TPCmd* aPCmd, TDes8* aRsp);
	TInt TerminalRspCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetMeSideSatProfile(const TTsyReqHandle aTsyReqHandle, TDes8* aMeSimSatProfile);
	TInt GetMeSideSatProfileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt ClientSatProfileIndication(const TTsyReqHandle aTsyReqHandle, TDes8* aClientSatProfile);
	TInt MenuSelection(const TTsyReqHandle aTsyReqHandle, TDes8* aSelection);
	TInt MenuSelectionCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCallControlRequest(const TTsyReqHandle aTsyReqHandle, TDes8* aCallControlDetails);
	TInt NotifyCallControlRequestCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyMoSmControlRequest(const TTsyReqHandle aTsyReqHandle, TDes8* aMoSmDetails);
	TInt NotifyMoSmControlRequestCancel(const TTsyReqHandle aTsyReqHandle);
	TInt EventDownload(const TTsyReqHandle aTsyReqHandle, RSat::TEventList* aSingleEvent, TDes8* aEventInfo);	
	TInt EventDownloadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyDisplayTextPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyDisplayTextPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyGetInkeyPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyGetInkeyPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyGetInputPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyGetInputPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyPlayTonePCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyPlayTonePCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySetUpMenuPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySetUpMenuPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySelectItemPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySelectItemPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySendSmPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySendSmPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySendSsPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySendSsPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySendUssdPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySendUssdPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySetUpCallPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySetUpCallPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyRefreshPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyRefreshPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySetUpEventListPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySetUpEventListPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySetUpIdleModeTextPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySetUpIdleModeTextPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySendDtmfPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySendDtmfPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SendMessageNoLogging(const TTsyReqHandle aTsyReqHandle, const TDesC8* aMsg, TUint16* aMsgRef);
	TInt SendMessageNoLoggingCancel(const TTsyReqHandle aTsyReqHandle);


	// New 7.0s Methods
	TInt UsatClientReadyIndication(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyPerformCardApduPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyPerformCardApduPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyPowerOffCardPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyPowerOffCardPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyPowerOnCardPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyPowerOnCardPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyGetReaderStatusPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyGetReaderStatusPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyRunAtCommandPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyRunAtCommandPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyLanguageNotificationPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyLanguageNotificationPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetProvisioningRefFile(const TTsyReqHandle aTsyReqHandle,const RSat::TProvisioningFileRef* aFileRef, TDes8* aFile);
	TInt GetProvisioningRefFileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyLaunchBrowserPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyLaunchBrowserPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyOpenChannelPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyOpenChannelPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCloseChannelPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyCloseChannelPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyReceiveDataPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyReceiveDataPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySendDataPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySendDataPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyGetChannelStatusPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyGetChannelStatusPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyServiceSearchPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyServiceSearchPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyGetServiceInfoPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyGetServiceInfoPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyDeclareServicePCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyDeclareServicePCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyTsyStateUpdated(const TTsyReqHandle aTsyReqHandle,RSat::TPCmd* aPCmd);
	TInt NotifyTsyStateUpdatedCancel(const TTsyReqHandle aTsyReqHandle);

	TInt DisplayTextTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt GetInkeyTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt GetInputTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt PlayToneTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SetUpMenuRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SelectItemTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SendSmTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SendSSTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SendUssdTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SetUpCallTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt RefreshTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SetUpEventListTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SetUpIdleModeTextTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SendDtmfTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt PerformCardApduTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt PowerOffCardTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt PowerOnCardTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt GetReaderStatusTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt RunAtCommandTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt LanguageNotificationTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt LaunchBrowserTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt OpenChannelTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt CloseChannelTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt ReceiveDataTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SendDataTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt GetChannelStatusTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt ServiceSearchTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt GetServiceInfoTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt DeclareServiceTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt TimerMgmtTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt LocalInfoTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt PollingIntvTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt PollingOffTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	
	//New 8.0 Methods
	
	TInt NotifyProactiveSimSessionStart(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyProactiveSimSessionStartCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyTimerExpiration(const TTsyReqHandle aTsyReqHandle, TDes8* aTimerDetails);
	TInt NotifyTimerExpirationCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySmsPpDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aSmsPpDownloadDetails);
	TInt NotifySmsPpDownloadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCbDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aCbDownloadDetails);
	TInt NotifyCbDownloadCancel(const TTsyReqHandle aTsyReqHandle);	
	TInt NotifyTimerManagtPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyTimerManagtPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyLocalInfoPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyPollingIntervalPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyPollingOffPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyLocalInfoPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyPollingIntervalPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyPollingOffPCmdCancel(const TTsyReqHandle aTsyReqHandle);

	// New 8.1 Methods

	TInt NotifyRefreshRequired(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyRefreshRequiredCancel(const TTsyReqHandle aTsyReqHandle);
	TInt RefreshAllowed(const TTsyReqHandle aTsyReqHandle, TDes8* aRefreshAllowedRsp);
	TInt RefreshAllowedCancel(const TTsyReqHandle aTsyReqHandle);

	// New 9.1 Methods

	TInt NotifyRefreshRequired(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	
	// New 9.2 Methods
	
	TInt GetOpenChannelPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt GetOpenChannelPCmdCancel(const TTsyReqHandle aTsyReqHandle);

    // New 9.4 Methods
    
    TInt NotifyRetrieveMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
    TInt NotifySubmitMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
    TInt NotifyDisplayMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
    TInt NotifySetFramesPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
    TInt NotifyGetFramesStatusPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
    TInt UssdDataDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
    TInt MmsNotificationDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aMmsNotificationDownload);
  	TInt MmsTransferStatus(const TTsyReqHandle aTsyReqHandle, TDes8* aMmTransferStatus);
	TInt NotifyRetrieveMultimediaMsgPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySubmitMultimediaMsgPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyDisplayMultimediaMsgPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySetFramesPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyGetFramesStatusPCmdCancel(const TTsyReqHandle aTsyReqHandle);
    TInt UssdDataDownloadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt MmsNotificationDownloadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt MmsTransferStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt RetrieveMultimediaMsgTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SubmitMultimediaMsgTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt DisplayMultimediaMsgTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SetFramesTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt GetFramesStatusTerminalRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
private:
	TTsyReqHandle iTsyAsyncReqHandle;
	TInt iNotifyProactiveSimSessionEnd;
	TInt iNotifyProactiveSimSessionStart;
	TInt iNotifyCallControlRequest;
	TInt iNotifyMoSmControlRequest;
	TInt iNotifyDisplayTextPCmd;
	TInt iNotifyGetInkeyPCmd;
	TInt iNotifyGetInputPCmd;
	TInt iNotifyPlayTonePCmd;
	TInt iNotifySetUpMenuPCmd;
	TInt iNotifySelectItemPCmd;
	TInt iNotifySendSmPCmd;
	TInt iNotifySendSsPCmd;
	TInt iNotifySendUssdPCmd;
	TInt iNotifySetUpCallPCmd;
	TInt iNotifyRefreshPCmd;
	TInt iNotifySetUpEventListPCmd;
	TInt iNotifySetUpIdleModeTextPCmd;
	TInt iNotifySendDtmfPCmd;
	TInt iGetIcon;
	TInt iGetImageInstance;
	TInt iGetClut;
	TInt iDisplayTextTerminalRsp;
	TInt iGetInkeyTerminalRsp;
	TInt iGetInputTerminalRsp;
	TInt iPlayToneTerminalRsp;
	TInt iSetUpMenuTerminalRsp;
	TInt iSelectItemTerminalRsp;
	TInt iSendSmTerminalRsp;
	TInt iSendSsTerminalRsp;
	TInt iSendUssdTerminalRsp;
	TInt iSetUpCallTerminalRsp;
	TInt iRefreshTerminalRsp;
	TInt iSetUpEventListTerminalRsp;
	TInt iSetUpIdleModeTextTerminalRsp;
	TInt iSendDtmfTerminalRsp;
	TInt iGetMeSideSatProfile;
	TInt iClientSatProfileIndication;
	TInt iMenuSelection;
	TInt iEventDownload;
	TInt iSendMessageNoLogging;
		
	TInt iNotifySetUpMenuTestNumber;
	TInt iNotifyCallControlRequestTestNumber;
	TInt iNotifyOpenChannelTestNumber;

	// 7.0s API
	TInt iNotifyPerformCardApduPCmd;
	TInt iNotifyPowerOffCardPCmd;
	TInt iNotifyPowerOnCardPCmd;
	TInt iNotifyGetReaderStatusPCmd;
	TInt iNotifyRunAtCommandPCmd;
	TInt iNotifyLanguageNotificationPCmd;
	TInt iGetProvisioningRefFile;
	TInt iNotifyOpenChannelPCmd;
	TInt iNotifyAnyBearerCount;
	TInt iNotifyCloseChannelPCmd;
	TInt iNotifyReceiveDataPCmd;
	TInt iNotifySendDataPCmd;
	TInt iNotifyGetChannelStatusPCmd;
	TInt iNotifyServiceSearchPCmd;
	TInt iNotifyGetServiceInfoPCmd;
	TInt iNotifyDeclareServicePCmd;
	TInt iNotifyLaunchBrowserPCmd;
	TInt iNotifyLaunchBrowserCount;
	TInt iPerformCardApduTerminalRsp;
	TInt iPowerOffCardTerminalRsp;
	TInt iPowerOnCardTerminalRsp;
	TInt iGetReaderStatusTerminalRsp;
	TInt iRunAtCommandTerminalRsp;
	TInt iLanguageNotificationTerminalRsp;
	TInt iOpenChannelTerminalRsp;
	TInt iCloseChannelTerminalRsp;
	TInt iReceiveDataTerminalRsp;
	TInt iSendDataTerminalRsp;
	TInt iGetChannelStatusTerminalRsp;
	TInt iServiceSearchTerminalRsp;
	TInt iGetServiceInfoTerminalRsp;
	TInt iDeclareServiceTerminalRsp;
	TInt iLaunchBrowserTerminalRsp;
	// 8.0
	TInt iNotifyTimerExpiration;
	TInt iNotifySmsPpDownload;
	TInt iNotifyCbDownload;
	TInt iNotifyTimerManagtPCmd;
	TInt iNotifyLocalInfoPCmd;
	TInt iNotifyPollingIntervalPCmd;
	TInt iNotifyPollingOffPCmd;
	TInt iTimerMgmtTerminalRsp;
	TInt iLocalInfoTerminalRsp;
	TInt iPollingIntervalTerminalRsp;
	TInt iPollingOffTerminalRsp;
	TInt iNotifyTsyStateUpdated;
	// 8.1
	TInt iNotifyRefreshRequired;
	TInt iRefreshAllowed;
	// 9.1 - this variable is used to make two different requests
	// on idle mode text notification.  First request will return
	// 16-bit unicode text, and second request will return 16-bit
	// text copied from an 8-bit literal.
	TInt iSetUpIdleModeTextCommandCount;
	//9.4
	TInt iNotifyRetrieveMultimediaMsgPCmd;
	TInt iNotifySubmitMultimediaMsgPCmd;
	TInt iNotifyDisplayMultimediaMsgPCmd;
	TInt iNotifySetFramesPCmd;
	TInt iNotifyGetFramesStatusPCmd;
	TInt iRetrieveMultimediaMsgTerminalRsp;
	TInt iSubmitMultimediaMsgTerminalRsp;
	TInt iDisplayMultimediaMsgTerminalRsp;
	TInt iSetFramesTerminalRsp;
	TInt iGetFramesStatusTerminalRsp;
	TInt iMmsNotificationDownload;
	TInt iMmsTransferStatus;
	TInt iUssdDataDownload;	
	};

/**************************************************************************/
//
//	CPhoneDSatTsy
//
/**************************************************************************/

class CPhoneDSatTsy : public CPhoneDummyBase
	{
public:
	static CPhoneDSatTsy* NewL(CPhoneFactoryDummyBase* aFac);
	CPhoneDSatTsy(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CPhoneDSatTsy();
public:
	virtual void Init();
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
private:
	static CPhoneDSatTsy* This(TAny* aPtr);
	};
	

/**************************************************************************/
//
//	CPhoneFactoryDSatTsy
//
/**************************************************************************/

class CPhoneFactoryDSatTsy: public CPhoneFactoryDummyBase
	{
public:
	static CPhoneFactoryDSatTsy* NewL();
	CPhoneFactoryDSatTsy();
	~CPhoneFactoryDSatTsy();
	virtual CPhoneBase* NewPhoneL(const TDesC& aName);
	virtual TBool IsSupported(const TInt aMixin);
	};



#endif
