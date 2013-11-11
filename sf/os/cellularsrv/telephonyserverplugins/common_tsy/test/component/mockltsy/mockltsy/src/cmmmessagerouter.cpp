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
//

#include "cmmmessagerouter.h"
#include "mmmmesshandlerbase.h"
#include "cmockphonemesshandler.h"
#include "cmockcallmesshandler.h"
#include "cmockphonebookstoremesshandler.h"
#include "cmockpcktservicemesshandler.h"
#include "cmockpcktcontextmesshandler.h"
#include "cmockpcktqosmesshandler.h"
#include "CMmCustomMessHandlerGsmExt.h"
#include "cmocksmsmesshandler.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "pcktcs.h" // for packet data IPCs

#include "cmockltsyengine.h"
#include "cmocksyserver.h"
#include "rmockltsy.h"


CMmMessageRouter* CMmMessageRouter::NewL(MmMessageManagerCallback& aMessageManagerCallback)
    {
    CMmMessageRouter* const self = new(ELeave) CMmMessageRouter(aMessageManagerCallback);
    CleanupStack::PushL( self );
    self->ConstructL();   
    CleanupStack::Pop();
    return self;
    }

CMmMessageRouter::CMmMessageRouter(MmMessageManagerCallback& aMessageManagerCallback)
	: iMessageManagerCallback(aMessageManagerCallback)
    {
    }

void CMmMessageRouter::ConstructL()
    {
	iMockEngine = CMockLtsyEngine::NewL(this);
	iMockServer = CMockSYServer::NewL(KMockLtsyServerName, CActive::EPriorityHigh, *iMockEngine);

    InitL();
    InitModulesL(); 
    }

CMmMessageRouter::~CMmMessageRouter()
    {
    delete iMockServer;
    delete iMockEngine;
   
    delete  iPhoneMessHandler;
    delete  iCallMessHandler;
    delete  iPhoneBookStoreMessHandler;
    delete  iPcktServiceMessHandler;
    delete  iPcktContextMessHandler;
    delete  iPcktQoSMessHandler;
    delete  iSmsMessHandler;
    
    delete iMmCustomMessHandlerGsmExt;
    }

/**
First initialization phase
*/
void CMmMessageRouter::InitL()
    {
    }

/**
Second initialization phase
*/
void CMmMessageRouter::InitModulesL()
    {
    iPhoneMessHandler = CMockPhoneMessHandler::NewL( this ); 
    iCallMessHandler =  CMockCallMessHandler::NewL( this );
    iPhoneBookStoreMessHandler = CMockPhoneBookStoreMessHandler::NewL( this );  
    iPcktContextMessHandler = CMockPcktContextMessHandler::NewL( this );
    iPcktQoSMessHandler =  CMockPcktQoSMessHandler::NewL( this);
    iPcktServiceMessHandler = CMockPcktServiceMessHandler::NewL(this);
    iMmCustomMessHandlerGsmExt = CMmCustomMessHandlerGsmExt::NewL( this ); 
    iSmsMessHandler = CMockSmsMessHandler::NewL( this );
    }


/**
 * Part of the MMessageRouter interface which is part of the LTSY plugin
 * interface.
 * 
 * This function is the entry point for a request from the CTSY into the
 * LTSY.
 * 
 * @param aIpc The IPC of the request.
 * 
 * @param aDataPackage The data package containing the parameters of the
 * request.
 * 
 * @return Error code to propagate back up to the CTSY relating to making
 * this request. Returns KErrNone on success, KErrNotSupported if the LTSY 
 * does not supported this request (it could not find a message handler to 
 * handle this request).
 */
TInt CMmMessageRouter::ExtFuncL(TInt aIpc, const CMmDataPackage* aDataPackage)
	{
	MMmMessHandlerBase* messageHandler = RouteRequest( aIpc );
    if ( NULL == messageHandler )
        {
		return KErrNotSupported;
		}
	else
		{
		return messageHandler->ExtFuncL( aIpc, aDataPackage ); 
		}
	}


/**
Returns pointer to the message handler for this request, based on IPC number
*/
MMockMessHandlerBase* CMmMessageRouter::RouteRequest(const TInt aIpc ) 
    {
    MMockMessHandlerBase* messHandler = NULL;

    switch( aIpc )
        {
        case EMobileCallDialEmergencyCall:
        case EEtelCallAnswer:
        case EEtelCallDial:
        case EEtelCallHangUp:
        case EMobileCallHold:
        case EMobileCallResume:
        case EMobileCallSwap:
        case EMobileCallGoOneToOne:
        case EMobileCallTransfer:
        case EMobileCallDeflect:
        case EEtelLineNotifyIncomingCall:
        case EMobileCallAnswerISV:
   		case EMobileCallDialISV:
        case EMobileCallDialCallControl:
        case EEtelCallLoanDataPort:
        case EEtelCallRecoverDataPort:

        // Conference call functionality
        case EMobileConferenceCallCreateConference:
        case EMobileConferenceCallAddCall:
        case EMobileConferenceCallSwap:
        case EMobileConferenceCallHangUp:
        case EMobileConferenceCallNotifyConferenceEvent:
        case EMobileConferenceCallNotifyConferenceStatusChange:

        //CSD fearures
        case EMobileCallSetDynamicHscsdParams:
        
        // Completion 
        case EMobileCallNotifyMobileCallStatusChange:
        case EMobileCallGetMobileCallInfo:
        case EEtelCallNotifyStatusChange:
        case EMobileCallNotifyCallEvent:
		case EEtelLineNotifyCallAdded:
        case EMobileCallNotifyMobileDataCallCapsChange:
        case EEtelCallGetBearerServiceInfo:
        case EMobileCallNotifyHscsdInfoChange:
        case EMobileCallDialNoFdnCheck:
        case EMobileCallNotifyPrivacyConfirmation:
        case EMobileCallReceiveUUI:
            messHandler = iCallMessHandler;
            break;

        //ENStore
        case EMmTsyENStoreGetInfoIPC: 
        case EMmTsyENStoreReadIPC: 
        case EMmTsyENStoreReadAllPhase1IPC:
        
        //OnStore
        case EMmTsyONStoreGetInfoIPC: 
        case EMmTsyONStoreReadIPC: 
        case EMmTsyONStoreWriteIPC:
        case EMmTsyONStoreDeleteIPC:
        case EMmTsyONStoreDeleteAllIPC:
        case EMmTsyONStoreReadAllPhase1IPC: 
        case EMmTsyONStoreStoreAllIPC:
        case EMmTsyONStoreWriteSizeIPC:
        case EMmTsyONStoreWriteEntryIPC:
        case EMmTsyONStoreReadSizeIPC:
        case EMmTsyONStoreReadEntryIPC:
   
        //PhoneBookStore
        case EMmTsyPhoneBookStoreReadIPC:
        case EMmTsyPhoneBookStoreWriteIPC:
        case EMmTsyPhoneBookStoreGetInfoIPC:
        case EMmTsyPhoneBookStoreDeleteIPC:
        case EMmTsyPhoneBookStoreDeleteAllIPC:
        case EMmTsyPhoneBookStoreCacheIPC:
        case EMmTsyPhoneBookStoreInitIPC:
        case EMmTsyPhoneBookStoreResetCacheIPC:
        case EMmTsyPhoneBookStoreFdnInfoIPC:
            messHandler = iPhoneBookStoreMessHandler;
            break;
        case EPacketAttach:
        case EPacketDetach:
        case EPacketGetNtwkRegStatus:
        case EPacketSetAttachMode:
        case EPacketRejectActivationRequest:
        case EPacketGetAttachMode:
        case EPacketSetPrefBearer:
        case EPacketSetDefaultContextParams:
        case EPacketGetDefaultContextParams:
        case EPacketNotifyContextAdded:
        case EPacketNotifyDynamicCapsChange:
        case EPacketNotifyStatusChange:
        case EPacketInitProxiesIPC:
        case EPacketNotifyChangeOfNtwkRegStatus:
        case EPacketNotifyMSClassChange:
        case EPacketNotifyContextActivationRequested:
        case EPacketNotifyAttachModeChange:
        case EPacketNotifyReleaseModeChange:
        case EPacketNotifyTransferCapsIPC:
        case EMmTsyNotifyEGprsInfoChangeIPC:
        case EPacketUpdateMbmsMonitorServiceList:
        case EPacketGetMbmsNetworkServiceStatus:
        case EPacketNotifyMbmsNetworkServiceStatusChange:
        case EPacketGetStaticCaps:
        case EPacketEnumerateMbmsActiveServiceList:
        case EPacketEnumerateMbmsMonitorServiceList:
        case EPacketNotifyMbmsServiceAvailabilityChange:
            messHandler = iPcktServiceMessHandler;
            break;
        case EPacketContextInitialiseContext:
        case EPacketContextSetConfig: 
        case EPacketContextActivate:
        case EPacketContextDelete: 
        case EPacketContextGetDataVolumeTransferred:
        case EPacketContextDeactivate:
        case EPacketContextAddPacketFilter:
        case EPacketContextRemovePacketFilter:
        case EPacketContextModifyActiveContext:
        case EPacketContextNotifyStatusChange:
        case EPacketContextNotifyConnectionSpeedChange:
        case EPacketContextNotifyConfigChanged:
        case EPacketAddMediaAuthorization:
        case EPacketContextNotifyDataTransferred:
        case EPacketContextUpdateMbmsSessionList:
        case ECtsyPacketMbmsInitialiseContextReq:
        case ECtsyPacketMbmsInitialiseContextComp:
        case ECtsyPacketMbmsContextNotifyConfigChanged:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange: 
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
            messHandler = iPcktContextMessHandler;
            break;
        case EPacketQoSSetProfileParams:
        case EPacketQoSNotifyProfileChanged:
        case EPacketQoSNotifyProfileChangedCancel:
        case EPacketQoSGetProfileCaps:
            messHandler = iPcktQoSMessHandler;
            break;

        // Custom API functionality
        case ECustomCancelUssdSessionIPC:
        case ECustomClearCallBlackListIPC:
        case ECustomCheckEmergencyNumberIPC:
        case ECustomGetLifeTimeIPC:
        case ECustomTerminateCallIPC:
        case ECustomGetAlsBlockedIPC:
        case ECustomSetAlsBlockedIPC:
        case ECustomCheckAlsPpSupportIPC:
        case ECustomCheckTwoDigitDialSupportIPC:
        case ECustomGetCipheringInfoIPC:
        case ECustomNetWakeupIPC:
        case ECustomGetOperatorNameIPC:
        case ECustomResetNetServerIPC:
        case ECustomGetNetworkProviderNameIPC:
        case ECustomReadSimFileIPC:
        case ECustomNotifyDtmfEventIPC:
        case ECustomNotifyCellInfoChangeIPC:
        case ECustomGetCellInfoIPC:
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
        case ECustomGetIccCallForwardingStatusIPC:
        case ECustomNotifyHSxPAStatusIPC:
        case ECustomWriteHSxPAStatusIPC:
        case ECustomReadHSxPAStatusIPC:
        case ECustomNotifyRauEventIPC:

        case ECustomSimLockActivateIPC:
        case ECustomSimLockDeActivateIPC:
        case ECustomSendAPDUReqIPC:
        case ECustomSendAPDUReqV2IPC:    
        case ECustomStartSimCbTopicBrowsingIPC:
        case ECustomDeleteSimCbTopicIPC:
        case ECustomSetSimMessageStatusReadIPC:
        case ECustomGetSimAuthenticationDataIPC:
        case ECustomNotifySimCardStatusIPC:
        case ECustomGetSimCardReaderStatusIPC:
        case ECustomGetATRIPC:
        case ECustomSimWarmResetIPC:
        case ECustomPowerSimOffIPC:
        case ECustomPowerSimOnIPC:
        case ECustomGetCurrentSystemNetworkModesIPC:
        case ECustomSetSystemNetworkModeIPC:
        case ECustomGetSystemNetworkModesIPC:
        case ECustomNotifyNetworkConnectionFailureIPC:
        case ECustomSatRefreshCompleteNotificationIPC:
        case ECustomNotifySsNetworkEventIPC:
        case ECustomGetProgrammableOperatorLogoIPC:
        case ECustomNotifyPndCacheReadyIPC:
        case ECustomNotifySsRequestCompleteIPC:
        case ECustomSsAdditionalInfoNotificationIPC:
        case ECustomGetBandSelectionIPC:
        case ECustomSetBandSelectionIPC:
        case ECustomGetServiceTableSupportbyApplicationIPC:

        // Security (Custom) functionality
        case ECustomIsBlockedIPC:
        case ECustomCheckSecurityCodeIPC:
        case ECustomCheckSecurityCodeCancelIPC:
        case ECustomDisablePhoneLockIPC:
        case ECustomSetAlwaysOnMode:
        case ECustomSetDriveModeIPC:
        case ECustomNotifyAlsBlockedChangedIPC:
        case ECustomNotifyCipheringInfoChangeIPC:
        case ECustomNotifyNSPSStatusIPC:
        case EReadViagHomeZoneParamsIPC:
        case EReadViagHomeZoneCacheIPC:
        case EWriteViagHomeZoneCacheIPC:
        case EWriteViagHomeZoneUHZIUESettingsIPC:
        case ECustomGetDiagnosticOctetsIPC:
        case ECustomGetRemoteAlertingToneStatusIPC:
        case EMmTsySecurityGetSimActivePinStateIPC:
            messHandler = iMmCustomMessHandlerGsmExt;
            break;
        
        // SMS functionality            
        case EMmTsyActivateSmsRouting:
        case EMobileSmsMessagingGetMessageStoreInfo:
        case EMobileSmsMessagingReceiveMessage:
        case EMobileSmsMessagingSendMessage:
        case EMobileSmsMessagingSendMessageNoFdnCheck:
        case EMobileSmsMessagingResumeSmsReception:
        case EMobileSmsMessagingGetSmspListPhase1:
        case EMobileSmsMessagingStoreSmspList:
        case EMobileSmsMessagingAckSmsStored:
        case EMobileSmsMessagingNackSmsStored:
        case EMobileSmsMessagingSetMoSmsBearer:
        case EMobilePhoneStoreGetInfo:
        case EMobilePhoneStoreDelete: 
        case EMobilePhoneStoreDeleteAll:
        case EMobilePhoneStoreRead:
        case EMobilePhoneStoreReadAllPhase1:
        case EMobilePhoneStoreWrite:
        case EMmTsySmsSendSatMessage:
        case EMobileUssdMessagingSendMessage:
        case EMobileBroadcastMessagingReceiveMessage:
        case EMobileBroadcastMessagingReceiveMessageCancel:
        case EMobileBroadcastMessagingSetFilterSetting:
        case EMobileBroadcastMessagingSetLanguageFilter:
        case EMobileBroadcastMessagingStoreIdList:
        case EMobileBroadcastMessagingGetIdListPhase1:
        case EMmTsyGsmBroadcastNotifyMessageReceived:
        case EMmTsyWcdmaBroadcastNotifyMessageReceived:
        case EMobileSmsMessagingNotifyReceiveModeChange:
        case EMobileUssdMessagingSendRelease:
        case EMobileUssdMessagingReceiveMessage:
        case EMobileUssdMessagingNotifyNetworkRelease:
        case EMobileUssdMessagingSendMessageNoFdnCheck:
            messHandler = iSmsMessHandler;
            break;
            
        // Miscellaneous Phone features
        // case EMobilePhoneGetBatteryInfo:
        case EMobilePhoneGetPhoneStoreInfo:
        case EMobilePhoneGetPhoneId:
        case EMobilePhoneGetSubscriberId:
        case EMobilePhoneGetALSLine:
        case EMobilePhoneSetALSLine:
        case EMobilePhoneGetCustomerServiceProfile:
        case EMobilePhoneGetServiceTable:
        case EMmTsyPhoneGetPin1DisableSupportedIPC:
       	case EMobilePhoneNotifyBatteryInfoChange:
        case EMobilePhoneGetFdnStatus:
        case EMobilePhoneSetFdnSetting:
        case EMobilePhoneNotifyFdnStatusChange:
        case EMobilePhoneGetCurrentActiveUSimApplication:        

        // DTMF functionality
        case EMobilePhoneStartDTMFTone:
        case EMobilePhoneStopDTMFTone:
        case EMobilePhoneSendDTMFTones:
        case EMobilePhoneSendDTMFTonesCancel:
        case EMobilePhoneContinueDTMFStringSending:
        // case EMobilePhoneReadDTMFTones:       not supported
        // case EMobilePhoneReadDTMFTonesCancel: not supported
        // case EMobilePhoneGetDTMFCaps:                  no DOS call required
        // case EMobilePhoneNotifyDTMFCapsChange:         no DOS call required
        // case EMobilePhoneNotifyDTMFCapsChangeCancel:   no DOS call required
        case EMobilePhoneNotifyDTMFEvent:              // no DOS call required
        // case EMobilePhoneNotifyDTMFEventCancel:        no DOS call required
        case EMobilePhoneNotifyStopInDTMFString:       // no DOS call required
        // case EMobilePhoneNotifyStopInDTMFStringCancel: no DOS call required

        // Supplementary Services functionality
        case EMobilePhoneGetCallForwardingStatusPhase1:
        case EMobilePhoneSetCallForwardingStatus:
        case EMobilePhoneGetBarringStatusPhase1:
        case EMobilePhoneSetCallBarringStatus:
        case EMobilePhoneSetSSPassword:
        case EMobilePhoneGetWaitingStatusPhase1:
        case EMobilePhoneSetCallWaitingStatus:
        case EMobilePhoneNotifyCallWaitingStatusChange:
        case EMobilePhoneGetIdentityServiceStatus:
        case EMobilePhoneSendNetworkServiceRequest:
        case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
        case ECtsyPhoneSendNetworkServiceRequestInd:	
        	
        case EMobilePhoneNotifyCallBarringStatusChange:    // no DOS call required
        // case EMobilePhoneGetCallForwardingStatusPhase2:    no DOS call required
        case EMobilePhoneNotifyCallForwardingStatusChange: // no DOS call required
        case EMobilePhoneNotifyCallForwardingActive:       // no DOS call required
        // case EMobilePhoneGetBarringStatusPhase2:           no DOS call required

        // Security and SIM
        case EMobilePhoneGetLockInfo:
        case EMobilePhoneNotifyLockInfoChange:
        case EMobilePhoneGetSecurityCodeInfo:
        case EMobilePhoneNotifySecurityCodeInfoChange:
        
        case EMobilePhoneChangeSecurityCode:
        case EMobilePhoneAbortSecurityCode:
        case EMobilePhoneVerifySecurityCode:
        case EMobilePhoneSetLockSetting:
        case EMmTsyBootNotifySimStatusReadyIPC:
        case EMobilePhoneNotifySecurityEvent:
        case EMmTsySimGetICCType:
        case EMobilePhoneSetNetworkSelectionSetting:
        case EMobilePhoneSelectNetwork:
        case EMobilePhoneGetDetectedNetworksV2Phase1:
        case EMobilePhoneSelectNetworkCancel:
        case EMobilePhoneGetDetectedNetworksCancel:
        case EMobilePhoneGetNetworkRegistrationStatus:
        case EMobilePhoneGetHomeNetwork:
        case EMobilePhoneGetNetworkSecurityLevel:
        case EMobilePhoneGetCipheringIndicatorStatus:
        case EMobilePhoneNotifySecurityCapsChange:
        case EMmTsySimRefreshRegisterIPC:
        
        // APNControl
        case EMobilePhoneGetAPNname: 
        case EMobilePhoneEnumerateAPNEntries:
        case EMobilePhoneAppendAPNName:
        case EMobilePhoneDeleteAPNName:
        case EMobilePhoneNotifyAPNListChanged:
        case EMobilePhoneGetAPNControlListServiceStatus:
        case EMobilePhoneSetAPNControlListServiceStatus:  
        case EMobilePhoneNotifyAPNControlListServiceStatusChange:      
        
        // Cell Info
        case ECtsyPhoneCellInfoComp:
        case ECtsyPhoneCellInfoIndComp:
        case ECtsyPhoneCellInfoReq:
        case ECtsyPhoneCellInfoIndReq:
        	
        // Completion 
        case EMobilePhoneNotifySignalStrengthChange:
        case EMmTsyBootNotifyModemStatusReadyIPC:
        case EMobilePhoneNotifyNetworkSecurityLevelChange:
        case EMobilePhoneAuthorizationInfoPhase1:
        case EMmTsyBootGetRFStatusIPC:
        case EMobilePhoneGetMailboxNumbers:
        case EMobilePhoneNotifyNetworkRegistrationStatusChange:
        case EMmTsyBootNotifySecurityReadyIPC:
        case EMobilePhoneIMSAuthenticate:
        case EMobilePhoneGetIccMessageWaitingIndicators:
        case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
        case EMobilePhoneSetIccMessageWaitingIndicators:
        case EMobilePhoneNotifyNITZInfoChange:
        case EMobilePhoneNotifyCellInfoChange:
        case EMobilePhoneNotifyCurrentNetworkChange:
        case EMobilePhoneNotifyModeChange:
        case EMobilePhoneGetServiceProviderName:
        case EMobilePhoneNotifyNetworkSelectionSettingChange:
        case EEtelPhoneNotifyModemDetected:
        case EMmTsySimRefreshNowIPC:
        case EETelLineCapsChangeNotification:
        case EEtelPhoneInitialise:
		case EMobilePhoneNotifyCallServiceCapsChange:
		case EMmTsyUpdateLifeTimeIPC:                     		
		case ECtsyPhoneTerminateAllCallsReq:
		case ECtsyPhoneTerminateAllCallsComp:
        case ECtsyPhoneTerminateActiveCallsReq:
        case ECtsyPhoneTerminateActiveCallsComp:
        case EMobilePhoneNotifyIccAccessCapsChange:

            messHandler = iPhoneMessHandler;
            break;
       
        default:
            messHandler = NULL;
            break;
        }

    return messHandler;
    }


