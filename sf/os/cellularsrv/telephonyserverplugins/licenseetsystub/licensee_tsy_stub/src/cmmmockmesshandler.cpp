// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmmockmesshandlerTraces.h"
#endif

#include "cmmmockmesshandler.h"
#include "cmmmessagerouter.h"
#include "cmmcustomstubext.h"
#include <pcktcs.h>
#include <ctsy/rmmcustomapi.h>

// callback timer interval
const TInt KTimeInterval = 1000000; 
const TUint KTmpPckgSize = 15; 

// ======== MEMBER FUNCTIONS ========

CMmMockMessHandler::CMmMockMessHandler()
    {  
    }

void CMmMockMessHandler::ConstructL( CMmMessageRouter* aMessageRouter )
    {
    iMessageRouter = aMessageRouter;

    if ( !iTimer )
        {
        iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
 OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMOCKMESSHANDLER_CONSTRUCTL_1, "TSY: CMmMockMessHandler::ConstructL: timer created" );
        }
    }

CMmMockMessHandler* CMmMockMessHandler::NewL( 
    CMmMessageRouter* aMessageRouter )
    {
    CMmMockMessHandler* self = new( ELeave ) CMmMockMessHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aMessageRouter );
    CleanupStack::Pop();
    return self;
    }

CMmMockMessHandler::~CMmMockMessHandler()
    {
    delete iTimer;
    iTimer = NULL;
    }

// ---------------------------------------------------------------------------
// CMmMockMessHandler::ExtFuncL
// Handles stub requests
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmMockMessHandler::ExtFuncL( 
    TInt aIpc, 
    const CMmDataPackage* /*aDataPackage*/)
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMOCKMESSHANDLER_EXTFUNCL_1, "TSY: CMmMockMessHandler::ExtFuncL. IPC: %d", aIpc );
    
    // callback indicators
    TInt ret( KErrNotSupported );
    TBool complete( EFalse );
    
    // datapackage for complete
    CMmDataPackage data;
    CMmDataPackage* package;
    package = NULL;
    TBuf8<KTmpPckgSize> temp;
    TUint8 routingActivated = 0x0;
    TInt mode( 0 );
    
    switch ( aIpc )
        {
        // return KErrNone without complete    
        case EMobilePhoneGetWaitingStatusPhase1:
        case EMobilePhoneGetALSLine:  
        case EMobilePhoneGetCallForwardingStatusPhase1:
        case EMobilePhoneGetLockInfo:
        case EMobilePhoneSetLockSetting:
        case EMobilePhoneGetBarringStatusPhase1:   
        case EMobileConferenceCallNotifyConferenceEvent:
        case EMobileConferenceCallNotifyCapsChangeCancel:
        case EMobileConferenceCallNotifyConferenceStatusChangeCancel:
        case EMobileConferenceCallNotifyConferenceEventCancel:
        case EMobileConferenceCallNotifyCapsChange:  
        case EMmTsyPhoneBookStoreWriteIPC:
        case EMmTsyONStoreDeleteIPC:   
        case EMmTsyPhoneBookStoreInitIPC:
        case EMmTsyPhoneBookStoreGetInfoIPC:
        case EMmTsyONStoreReadIPC:           
        case EMmTsyONStoreNotifyStoreEventIPC:            
        case EMobilePhoneStoreGetInfo:
        case EMobileSmsMessagingAckSmsStored:
        case EMobileSmsMessagingNackSmsStored:
        case EMobilePhoneStoreDelete:
        case EMobilePhoneStoreDeleteAll:
        case EMobilePhoneStoreRead: 
        case EMobilePhoneStoreWrite:
        case EMobileBroadcastMessagingReceiveMessage:
        case EMobileUssdMessagingSendMessage:
        case EMobileSmsMessagingGetMessageStoreInfo: 
        case EMobileSmsMessagingGetSmspListPhase1:
        case EMobilePhoneStoreReadAllPhase1:
        case EPacketNotifyDynamicCapsChange:
        case EPacketNotifyContextAdded:
        case EPacketNotifyStatusChange:
        case EPacketContextAddPacketFilter:
            ret = KErrNone;
            break;
        
        // return KErrNone and complete         
        case EMobilePhoneSetALSLine:
        case EMobilePhoneSelectNetwork:  
        case EMmTsyBootNotifySimStatusReadyIPC:      
        case EMobilePhoneSetCallWaitingStatus:
        case EMobilePhoneStopDTMFTone:
        case EMobilePhoneSetNetworkSelectionSetting:
        case EMobilePhoneSetCallForwardingStatus:
        case EMobilePhoneSetCallBarringStatus:
        case EMobilePhoneSendNetworkServiceRequest:
        case EMobilePhoneChangeSecurityCode:
        case EMobilePhoneAbortSecurityCode:
        case EMobilePhoneSetSSPassword:
        case EMobilePhoneSelectNetworkCancel:
        case EMobilePhoneGetDetectedNetworksCancel:
        case EMobileCallDialEmergencyCall:
        case EMobileCallSwap:
        case EMobileCallHold:
        case EMobileCallResume:
        case EMobileCallGoOneToOne:
        case EMobileCallTransfer:
        case EEtelCallAnswer:
        case EEtelCallHangUp:
        case EMobileCallSetDynamicHscsdParams:  
        case EMobileConferenceCallAddCall:
        case EMobileConferenceCallCreateConference:
        case EMobileConferenceCallSwap:
        case EMobileConferenceCallHangUp:
        case EMobileConferenceCallNotifyConferenceStatusChange: 
        case EMmTsyPhoneBookStoreCacheIPC:
        case EMmTsyPhoneBookStoreDeleteIPC:
        case EMmTsyPhoneBookStoreDeleteAllIPC:
        case EMmTsyONStoreWriteIPC: 
        case EMmTsyONStoreStoreAllIPC:
        case EMmTsyONStoreDeleteAllIPC:  
        case EMobileSmsMessagingStoreSmspList:
        case EMobileBroadcastMessagingReceiveMessageCancel:  
        case EMobileSmsMessagingSetMoSmsBearer:
        case ECustomTerminateCallIPC:
        case ECustomNetWakeupIPC:
        case ECustomSetAlsBlockedIPC:
        case ECustomClearCallBlackListIPC:
        case ECustomResetNetServerIPC:
        case ECustomSimLockActivateIPC:
        case ECustomSimLockDeActivateIPC:
        case ECustomDeleteSimCbTopicIPC:           
        case ECustomCheckSecurityCodeIPC:
        case ECustomCheckSecurityCodeCancelIPC:
        case ECustomDisablePhoneLockIPC: 
        case EPacketSetAttachMode:
        case EPacketSetDefaultContextParams:
            complete = ETrue;
            ret = KErrNone;
            break;
        
        // return KErrNotSupported and complete     
        case EMobilePhoneSendDTMFTones:
        case EMobilePhoneSendDTMFTonesCancel:
        case EMobilePhoneGetIdentityServiceStatus:
        case ECustomStartSimCbTopicBrowsingIPC:
            ret = KErrNotSupported;
            complete = ETrue;
            break; 
        
        // return KErrNotSupported    
        case EMobilePhoneGetNetworkRegistrationStatus:
        case EMobilePhoneGetPhoneStoreInfo:
        case EMmTsySimGetICCType:
        case EMmTsySecurityGetSimActivePinStateIPC:
        case EMobilePhoneGetDetectedNetworksV2Phase1:    
        case EMobilePhoneGetCipheringIndicatorStatus:
        case EMobilePhoneContinueDTMFStringSending:
        case EMobilePhoneStartDTMFTone:
        case EMobilePhoneGetServiceTable:
        case EMobilePhoneVerifySecurityCode:          
        case EMobileCallActivateCCBS:
        case EMobilePhoneAcceptCCBSRecall:
        case EMobilePhoneClearBlacklist:
        case EMobileCallDeflect:  
        case EMobileConferenceCallCreateConferenceCancel:
        case EMobileConferenceCallAddCallCancel:
        case EMobileConferenceCallSwapCancel:
        case EMobileConferenceCallHangUpCancel:  
        case EMobileConferenceCallGetCaps:
        case EMobileConferenceCallEnumerateCalls:   
        case EMmTsyENStoreReadAllPhase1IPC:
        case EMmTsyENStoreReadIPC: 
        case EMmTsyENStoreGetInfoIPC:
        case EMmTsyONStoreGetInfoIPC: 
        case EMobileSmsMessagingSendMessage:
        case EMmTsySmsSendSatMessage:   
        case EMobileSmsMessagingResumeSmsReception:    
        case ECustomCheckAlsPpSupportIPC:   
        case ECustomGetCipheringInfoIPC:
        case ECustomGetAlsBlockedIPC:
        case ECustomCheckEmergencyNumberIPC:
        case ECustomGetOperatorNameIPC:
        case ECustomCheckTwoDigitDialSupportIPC:    
        case ECustomGetNetworkProviderNameIPC:
        case ECustomReadSimFileIPC:
        case ECustomGetLifeTimeIPC:
        case ECustomSendAPDUReqIPC:
        case ECustomIsBlockedIPC:   
        case EPacketAttach:
        case EPacketDetach:
        case EPacketGetNtwkRegStatus:
        case EPacketSetPrefBearer:
        case EPacketRejectActivationRequest:    
        case EPacketContextGetDataVolumeTransferred:
        case EPacketContextInitialiseContext:             
        case EPacketContextDelete:
        case EPacketContextSetConfig:
        case EPacketContextActivate:
        case EPacketContextDeactivate:
        case EPacketContextModifyActiveContext:
        case EPacketQoSSetProfileParams:
            ret = KErrNotSupported;
            break; 
        
        // return KErrNotReady
        case EMobileConferenceCallGetMobileCallInfo:
            ret = KErrNotReady;
            break;
            
        // return KErrNotFound
        case EMmTsyPhoneBookStoreReadIPC:
            ret = KErrNotFound;
            break; 
            
            
        case EPacketInitProxiesIPC:
            GetHomeNetworkInfo();
            ret = KErrNone;
            break;
        case EPacketGetAttachMode:
            complete  = ETrue;
            mode = 1;          // 1 = When needed
            data.PackData( &mode );
            package = &data;
            ret = KErrNone;
            break;  
        case EMobilePhoneGetPhoneId:
            iTimer->Cancel();
            iTimer->Start( KTimeInterval, KTimeInterval, 
                TCallBack( TimerCallback, this ) );    
            ret = KErrNone;
            break;       
        case EMobilePhoneGetSubscriberId:
            iTimer->Cancel();
            iTimer->Start( KTimeInterval, KTimeInterval, 
                TCallBack( TimerCallback, this ) );    
            ret = KErrNone;
            break;
        case EMobilePhoneGetHomeNetwork:
            complete = ETrue;
            data.PackData( &iMyNetworkInfo );
            package = &data;
            ret = KErrNone;
            break; 
        case EMmTsyPhoneGetPin1DisableSupportedIPC:
            complete = ETrue;
            data.PackData ( &complete ); //Needed value for packet is ETrue
            package = &data;
            ret = KErrNone;
            break;    
        case EMmTsyActivateSmsRouting:
            routingActivated = 0x1;    //routing activated      
            data.PackData( &routingActivated );
            package = &data;
            ret = KErrNone;
            complete = ETrue;
            break;
            
        default:
            ret = KErrNotSupported;
            break;
        }

    if( complete )
        {
        if( NULL != package )
            {
            iMessageRouter->MessageManager()->Complete( aIpc, package, ret );
            }
         else
             {
             iMessageRouter->MessageManager()->Complete( aIpc, ret );
             }

        return KErrNone; 
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmMockMessHandler::GetHomeNetwork
// Creates and sends default values of Home network
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CMmMockMessHandler::GetHomeNetwork()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMOCKMESSHANDLER_GETHOMENETWORK_1, "TSY: CMmMockMessHandler::GetHomeNetwork.");
    
    iMyNetworkInfo.iMode = RMobilePhone::ENetworkModeGsm,
    iMyNetworkInfo.iStatus = RMobilePhone::ENetworkStatusCurrent,
    iMyNetworkInfo.iBandInfo = RMobilePhone::EBandUnknown;
    _LIT(KCountryCode, "358");
    iMyNetworkInfo.iCountryCode = KCountryCode;
    _LIT(KCdmaSID, "");
    iMyNetworkInfo.iCdmaSID = KCdmaSID;
    _LIT(KAnalogSID, "");
    iMyNetworkInfo.iAnalogSID = KAnalogSID;
    _LIT(KNetworkId, "NMC");
    iMyNetworkInfo.iNetworkId = KNetworkId;
    _LIT(KDisplayTag, "?");
    iMyNetworkInfo.iDisplayTag = KDisplayTag;
    _LIT(KShortName, "stub");
    iMyNetworkInfo.iShortName = KShortName;
    _LIT(KLongName, "stubtsy");
    iMyNetworkInfo.iLongName = KLongName;
    }

// ---------------------------------------------------------------------------
// CMmMockMessHandler::GetHomeNetworkInfo
// Requests for GetHomeNetwork
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CMmMockMessHandler::GetHomeNetworkInfo()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMOCKMESSHANDLER_GETHOMENETWORKINFO_1, "TSY: CMmMockMessHandler::GetHomeNetworkInfo.");
    // call for completion 
    TRAP_IGNORE( ExtFuncL( EMobilePhoneGetHomeNetwork, NULL ); );
    }

// ---------------------------------------------------------------------------
// CMmMockMessHandler::TimerCallback
// Compeltes IMEI and IMSI code to CommonTSY
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CMmMockMessHandler::TimerCallback( TAny* aThis )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMOCKMESSHANDLER_TIMERCALLBACK_1, "TSY: CMmMockMessHandler::TimerCallback.");

    // cancel the callback timer
    ( ( CMmMockMessHandler* )aThis )->iTimer->Cancel();
    
    // create datapackage
    CMmDataPackage data;
    CMmDataPackage* package;
    package = NULL;
    
    // create dummy phone id            
    TBuf8<KTmpPckgSize> tmp;
    _LIT8( phoneid,"000000000000000" );            
    tmp.Copy( phoneid );
    data.PackData( &tmp );
    package = &data; 
    
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMOCKMESSHANDLER_TIMERCALLBACK_2, "TSY: CMmMockMessHandler::TimerCallback: IPC EMobilePhoneGetPhoneId  Phone Id : %s", phoneid );
    ( ( CMmMockMessHandler* ) aThis )->iMessageRouter->MessageManager()->
        Complete( EMobilePhoneGetPhoneId, package, KErrNone );    
    
    //Create dummy IMSI
    _LIT8( imsi,"000000000000000" );
    TBuf8<KTmpPckgSize> temp;
    temp.Copy( imsi );
    data.PackData( &temp );
    package = &data;
    
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMOCKMESSHANDLER_TIMERCALLBACK_3, "TSY:CMmMockMessHandler::TimerCallback: IPC EMobilePhoneGetSubscriberId  Dummy imsi : %s", imsi );
    ( ( CMmMockMessHandler* ) aThis )->iMessageRouter->MessageManager()->
        Complete( EMobilePhoneGetSubscriberId, package, KErrNone );
    
    return ETrue;
    }

//  End of File
