// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CMmMessageRouterProxyTraces.h"
#endif

#include "cmmmessagerouterproxy.h"
#include "cmmvoicecalltsy.h"
#include "cmmdatacalltsy.h"
#include "cmmcallextinterface.h"
#include "cmmconferencecalltsy.h"
#include "cmmconferencecallextinterface.h"
#include "cmmphonetsy.h"
#include "cmmlinetsy.h"
#include "cmmnettsy.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmonstoretsy.h"
#include "cmmenstoretsy.h"
#include "cmmphonebookstoretsy.h"
#include "cmmphoneextinterface.h"
#include "cmmdtmftsy.h"
#include "cmmlinelist.h"
#include "cmmcalllist.h"
#include "cmmpblist.h"
#include "cmmsupplservtsy.h"
#include "Cmmpacketservicetsy.h"
#include "cmmpacketservicegsmwcdmaext.h"
#include "cmmpacketqosgsmwcdmaext.h"
#include "Cmmpacketcontextlist.h"
#include "cmmmbmscontextlist.h"
#include "cmmmbmscontexttsy.h"
#include "CMmCustomTsy.h"
#include "CMmCustomGsmExt.h"
#include "cmmussdtsy.h"
#include "cmmbroadcasttsy.h"
#include "CMmSimLockTsy.h"
#include "CMmWimTsy.h"
#include "CMmSIMTsy.h"
#include "cmmsecuritytsy.h"
#include "cmmsmstsy.h"
#include "cmmsmsstoragetsy.h"
#include "CMmCustomSecurityTsy.h"
#include "ctsydelegates.h"
#include <satcs.h>


CMmMessageRouterProxy::CMmMessageRouterProxy()
    {
    }
    
 CMmMessageRouterProxy* CMmMessageRouterProxy::NewL( 
    CTsyDelegates& aTsyDelegates )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_CTOR_1, "TSY: CMmMessageRouterProxy::NewL." );
    CMmMessageRouterProxy* const routerproxy =
        new ( ELeave ) CMmMessageRouterProxy();
    CleanupStack::PushL( routerproxy );
	routerproxy->iTsyDelegates = &aTsyDelegates;
    routerproxy->ConstructL();
    CleanupStack::Pop();
    return routerproxy;	
    }
    
CMmMessageRouterProxy::~CMmMessageRouterProxy()
	{	
	}
	
void CMmMessageRouterProxy::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CMmMessageRouterProxy::Complete 
// Complete with a data package
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageRouterProxy::Complete( 
    TInt aIpc, 
    CMmDataPackage* aData, 
    TInt aResult )
	{
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_COMPLETE_1, "TSY: CMmMessageRouterProxy::Complete. IPC = %{TIPCNamesList}, result: %d", aIpc, aResult);
	RouteCompletion( aIpc, aData, aResult ); 
	}
	
// ---------------------------------------------------------------------------
// CMmMessageRouterProxy::Complete 
// Complete without a data package
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageRouterProxy::Complete( TInt aIpc, TInt aResult )
	{    
    CMmDataPackage emptyDataPackage;
	Complete( aIpc, &emptyDataPackage, aResult );
	}
	
// ---------------------------------------------------------------------------
// CMmMessageRouterProxy::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmMessageRouterProxy::RouteCompletion(
    TInt aIpc,
    CMmDataPackage* aDataPackage,
    TInt aResult )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETION_1, "CMmMessageRouterProxy::RouteCompletion. IPC:%{TIPCNamesList} Result:%d Object:0x%08x", (TUint)aIpc, aResult, (TUint)this);
    TRAP_IGNORE( RouteCompletionL( aIpc, aDataPackage, aResult ) );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETION_2, "<-- CMmMessageRouterProxy::RouteCompletion" );
    }

// ---------------------------------------------------------------------------
// CMmMessageRouterProxy::RouteCompletionL
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmMessageRouterProxy::RouteCompletionL(
    TInt aIpc,
    CMmDataPackage* aDataPackage,
    TInt aResult )
    {
    //Literals used in routing
    _LIT( KStringExternal,  "External"  );
    _LIT( KStringExternal2, "External2" );
    
    //Object to complete
    CBase* mmObject = NULL;
      
     //Get mmObject to complete to
     switch ( aIpc )
        {
        // Call Functionality
        case EMobileCallHold:
        case EEtelCallDial:
        case EEtelCallAnswer:
        case EEtelCallHangUp:
        case EMobileCallResume:
        case EMobileCallSwap:
        case EMobileCallTransfer:
        case EMobileCallGoOneToOne:
        case EMobileCallNotifyMobileCallStatusChange:
        case EMobileCallGetMobileCallInfo:
        case EMobileCallNotifyCallEvent:
        case EMobileConferenceCallGetMobileCallInfo:
        case EMobileCallDialISV:
        case EMobileCallAnswerISV:
        case EMobileCallDialEmergencyCall:
        case EMobileCallReceiveUUI:
        case EMobileCallDialNoFdnCheck: 
        // CSD functionality
        case EMobileCallSetDynamicHscsdParams:
        case EMobileCallNotifyMobileDataCallCapsChange:
        case EMobileCallNotifyHscsdInfoChange:
        case EEtelCallGetCallParams:
        case EEtelCallGetBearerServiceInfo:
        	{
        	TInt callId( -1 );
	        RMobilePhone::TMobileService callMode( 
	            RMobilePhone::EVoiceService );

	        const CCallDataPackage* callData = REINTERPRET_CAST( 
	            const CCallDataPackage*, aDataPackage );

	        callData->GetCallIdAndMode( callId, callMode );

	        //call object can be usually reasoned by Call ID
	        mmObject = REINTERPRET_CAST( CBase*, 
                iMmPhone->CallList()->GetMmCallById( callId ) );	
        	}
            break;
            
		// Line Functionality
        case EEtelLineNotifyCallAdded:
        case EEtelLineNotifyIncomingCall:
        	{
        	TInt callId( -1 );
	        RMobilePhone::TMobileService callMode( 
	            RMobilePhone::EVoiceService );
	            
	        const CCallDataPackage* callData = REINTERPRET_CAST( 
	            const CCallDataPackage*, aDataPackage );
	            
	        callData->GetCallIdAndMode( callId, callMode );

            //correct line object is determined using the call mode
            mmObject = REINTERPRET_CAST( CBase*,
                iMmPhone->LineList()->GetMmLineByMode( callMode ) );
        	}
            break;

        // Conference call functionality
        case EMobileConferenceCallCreateConference:
        case EMobileConferenceCallAddCall:
        case EMobileConferenceCallSwap:
        case EMobileConferenceCallHangUp:
        case EMobileConferenceCallNotifyConferenceStatusChange:
        case EMobileConferenceCallNotifyConferenceEvent:

        // case EMobileConferenceCallGetCaps:
        // case EMobileConferenceCallNotifyCapsChange:
        // case EMobileConferenceCallEnumerateCalls:
        // case EMobileConferenceCallGetConferenceStatus:

        // case EMobileConferenceCallNotifyCapsChangeCancel:
        // case EMobileConferenceCallCreateConferenceCancel:
        // case EMobileConferenceCallAddCallCancel:
        // case EMobileConferenceCallSwapCancel:
        // case EMobileConferenceCallHangUpCancel:
        // case EMobileConferenceCallNotifyConferenceStatusChangeCancel:
        // case EMobileConferenceCallNotifyConferenceEventCancel:
        	mmObject = iTsyDelegates->
                GetTsyObject( CMmMessageManagerBase::EConferenceCallTsy );
            break;
            
		// Phonebook functionality
        // case EMobilePhoneReadDTMFTones:       not supported
        // case EMobilePhoneReadDTMFTonesCancel: not supported
        // case EMobilePhoneStartDTMFTone:                no completion
        // case EMobilePhoneContinueDTMFStringSending:    no completion
        // case EMobilePhoneGetDTMFCaps:                  no DOS call required
        // case EMobilePhoneNotifyDTMFCapsChange:         no DOS call required
        // case EMobilePhoneNotifyDTMFCapsChangeCancel:   no DOS call required
        // case EMobilePhoneNotifyStopInDTMFStringCancel: no DOS call required
        // case EMobilePhoneNotifyDTMFEventCancel: 		  no DOS call required
        case EMobilePhoneNotifyDTMFEvent:
         	mmObject = iTsyDelegates->GetTsyObject( CMmMessageManagerBase::EDtmfTsy );
			break;
			
        case EMmTsyPhoneBookStoreInitIPC:
        case EMmTsyPhoneBookStoreCacheIPC:
        case EMmTsyPhoneBookStoreReadIPC:
        case EMmTsyPhoneBookStoreWriteIPC:
        case EMmTsyPhoneBookStoreGetInfoIPC:
        case EMmTsyPhoneBookStoreDeleteIPC:
        case EMmTsyPhoneBookStoreDeleteAllIPC:
        case EMmTsyPhoneBookStoreResetCacheIPC:
        	{
        	const CPhoneBookDataPackage* phonebookData = REINTERPRET_CAST( 
        	    const CPhoneBookDataPackage*, aDataPackage );

	        TName phoneBookName;
	        phonebookData->GetPhoneBookName( phoneBookName );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_1, "TSY: CMmMessageRouterProxy::RouteCompletion: PB Name: %S", phoneBookName);

	        for( TInt i = 0; i < iMmPhone->PBList()->GetNumberOfObjects(); i++ )
	            {
	            CMmPhoneBookStoreTsy* pbStore = iMmPhone->PBList()->
	                GetMmPBByIndex( i );

	            if ( 0 == pbStore->PhoneBookName()->CompareF( phoneBookName ) )
	                {
	                mmObject = static_cast<CBase*>( pbStore );
	                }
	            }
	        if ( NULL == mmObject )
	            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_2, "TSY: CMmMessageRouterProxy::RouteCompletion: PB object not found!");
				iMmPhone->SetPBInitActiveStatus( EFalse );
	            }
        	}
            break;
		// ENStore functionality
        case EMmTsyENStoreGetInfoIPC:
        case EMmTsyENStoreReadIPC:
        case EMmTsyENStoreReadAllPhase1IPC:
        	mmObject = iTsyDelegates->
                GetTsyObject( CMmMessageManagerBase::EENStoreTsy  );
            break;
            
		// ONStore functionality
        case EMmTsyONStoreGetInfoIPC:
        case EMmTsyONStoreReadIPC:
        case EMmTsyONStoreWriteIPC:
        case EMmTsyONStoreDeleteIPC:
        case EMmTsyONStoreDeleteAllIPC:
        case EMmTsyONStoreReadAllPhase1IPC:
        case EMmTsyONStoreStoreAllIPC:
		case EMmTsyONStoreReadEntryIPC:
		case EMmTsyONStoreReadSizeIPC:
		case EMmTsyONStoreWriteSizeIPC:
		case EMmTsyONStoreWriteEntryIPC:
        	mmObject = iTsyDelegates->GetTsyObject( 
        	    CMmMessageManagerBase::EONStoreTsy  );
            break;
		
		//Packet data functionality
        case EPacketAttach:
        case EPacketDetach:
        case EPacketGetNtwkRegStatus:
        case EPacketSetAttachMode:
        case EPacketRejectActivationRequest:
        case EPacketGetAttachMode:
        case EPacketSetDefaultContextParams:
        case EPacketNotifyContextAdded:
        case EPacketNotifyStatusChange:
        case EPacketNotifyDynamicCapsChange:
        case EPacketNotifyMSClassChange:
        case EPacketSetPrefBearer:
        case EPacketNotifyChangeOfNtwkRegStatus:
        case EMmTsyNotifyEGprsInfoChangeIPC: // dummy internal IPC
        case EPacketNotifyAttachModeChange:
        case EPacketNotifyContextActivationRequested:
        case EPacketContextInitialiseContext:
        case EPacketQoSSetProfileParams:
        case EPacketQoSNotifyProfileChanged:
        case EPacketNotifyReleaseModeChange:
        case EPacketContextSetConfig:
        case EPacketContextDelete:
        case EPacketContextNotifyDataTransferred:
        case EPacketContextActivate:
        case EPacketContextNotifyConfigChanged:
        case EPacketContextNotifyConnectionSpeedChange:
        case EPacketContextNotifyStatusChange:
        case EPacketContextDeactivate:
        case EPacketContextGetDataVolumeTransferred:
        case EPacketContextModifyActiveContext:
        case ECustomSetAlwaysOnMode:
        case EPacketNotifyTransferCapsIPC:
        case EPacketEnumerateMbmsActiveServiceList:
        case EPacketNotifyMbmsServiceAvailabilityChange:
        case EPacketUpdateMbmsMonitorServiceList:
        case EPacketGetMbmsNetworkServiceStatus:
        case EPacketNotifyMbmsNetworkServiceStatusChange:
        case EPacketContextUpdateMbmsSessionList:
        case EPacketGetStaticCaps:
        case EPacketEnumerateMbmsMonitorServiceList:
        case ECtsyPacketMbmsInitialiseContextComp:
        case ECtsyPacketMbmsContextNotifyConfigChanged:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        case EPacketContextNotifyConnectionInfoChange: 
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        	mmObject = REINTERPRET_CAST( 
                CBase*, iMmPhone->PacketDataSession() );
            break;

        // USSD functionality
        case EMobileUssdMessagingSendRelease:
        case EMobileUssdMessagingSendMessage:
        case EMobileUssdMessagingSendMessageNoFdnCheck:
        case EMobileUssdMessagingReceiveMessage:
        case EMobileUssdMessagingNotifyNetworkRelease:
        	mmObject = iTsyDelegates->GetTsyObject( 
        	    CMmMessageManagerBase::EUssdMessagingTsy );
            break;
            
		// Broadcast functionality
        case EMmTsyGsmBroadcastNotifyMessageReceived:
        case EMmTsyWcdmaBroadcastNotifyMessageReceived:
        case EMobileBroadcastMessagingReceiveMessage:
        case EMobileBroadcastMessagingReceiveMessageCancel:
        case EMobileBroadcastMessagingSetFilterSetting:
        case EMobileBroadcastMessagingSetLanguageFilter:
        case EMobileBroadcastMessagingStoreIdList:
        	mmObject = iTsyDelegates->GetTsyObject(  
        	    CMmMessageManagerBase::EBroadcastMessagingTsy );
			break;
			
		// SMS functionality
        case EMobileSmsMessagingGetMessageStoreInfo:
        case EMmTsyActivateSmsRouting:
        case EMmTsyDeactivateSmsRouting:
        case EMobileSmsMessagingReceiveMessage:
        case EMobileSmsMessagingSendMessage:
        case EMobileSmsMessagingSendMessageNoFdnCheck: 
        case EMmTsySmsSendSatMessage:
        case EMobileSmsMessagingResumeSmsReception:
        case EMobileSmsMessagingGetSmspListPhase1:
        case EMobileSmsMessagingStoreSmspList:
        case EMobileSmsMessagingAckSmsStored:
        case EMobileSmsMessagingNackSmsStored:
        case EMobileSmsMessagingSetMoSmsBearer:
        	mmObject = iTsyDelegates->GetTsyObject(  
        	    CMmMessageManagerBase::ESmsMessagingTsy );
            break;
            
		// Storage functionality
        case EMobilePhoneStoreGetInfo:
        case EMobilePhoneStoreDelete:
        case EMobilePhoneStoreDeleteAll:
        case EMobilePhoneStoreRead:
        case EMobilePhoneStoreWrite:
        case EMobilePhoneStoreReadAllPhase1:
        	mmObject = iTsyDelegates->GetTsyObject(  
        	    CMmMessageManagerBase::EPhoneStoreTsy );
            break;

        // Custom Tsy functionality
        case ECustomCancelUssdSessionIPC:
        case ECustomSsAdditionalInfoNotificationIPC:
        case ECustomNotifySsNetworkEventIPC:
        case ECustomNotifySsRequestCompleteIPC:
        case ECustomNotifyDtmfEventIPC:
        case ECustomGetDiagnosticOctetsIPC:
        case ECustomGetRemoteAlertingToneStatusIPC:
        case ECustomClearCallBlackListIPC:
        case ECustomCheckEmergencyNumberIPC:
        case ECustomGetLifeTimeIPC:
        case ECustomTerminateCallIPC:
        case ECustomGetAlsBlockedIPC:
        case ECustomSetAlsBlockedIPC:
        case ECustomCheckAlsPpSupportIPC:
        case ECustomCheckTwoDigitDialSupportIPC:
        case ECustomGetCipheringInfoIPC:
        case ECustomNotifyCipheringInfoChangeIPC:
        case ECustomNotifyNSPSStatusIPC:
        case ECustomNetWakeupIPC:
        case ECustomGetOperatorNameIPC:
        case ECustomResetNetServerIPC:
        case ECustomNotifyNetworkConnectionFailureIPC:
        case ECustomGetSystemNetworkModesIPC:
        case ECustomSetSystemNetworkModeIPC:
        case ECustomGetNetworkProviderNameIPC:
        case ECustomGetCurrentSystemNetworkModesIPC:
        case ECustomSatRefreshCompleteNotificationIPC:
        case ECustomReadSimFileIPC:
        case ECustomSimLockActivateIPC:
        case ECustomSimLockDeActivateIPC:
        case ECustomSendAPDUReqIPC:
        case ECustomSimWarmResetIPC:
        case ECustomGetATRIPC:
        case ECustomSendAPDUReqV2IPC:
        case ECustomGetSimCardReaderStatusIPC:
        case ECustomNotifySimCardStatusIPC:
        case ECustomPowerSimOnIPC:
        case ECustomPowerSimOffIPC:
        case ECustomStartSimCbTopicBrowsingIPC:
        case ECustomDeleteSimCbTopicIPC:
        case EReadViagHomeZoneParamsIPC:
        case EReadViagHomeZoneCacheIPC:
        case EWriteViagHomeZoneUHZIUESettingsIPC:
        case EWriteViagHomeZoneCacheIPC:
        case ECustomGetSimAuthenticationDataIPC:
        case ECustomSetDriveModeIPC:
        case ECustomNotifyRauEventIPC:
        case ECustomReadHSxPAStatusIPC:
        case ECustomWriteHSxPAStatusIPC:
        case ECustomNotifyHSxPAStatusIPC:
        case ECustomGetIccCallForwardingStatusIPC:
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
        case ECustomGetCellInfoIPC:
        case ECustomNotifyCellInfoChangeIPC:
        case ECustomGetBandSelectionIPC:
        case ECustomSetBandSelectionIPC:
        case ECustomNotifyRemoteAlertingToneStatusChangeIPC:
        case ECustomGetServiceTableSupportbyApplicationIPC:
        case ECustomGetProgrammableOperatorLogoIPC:
        	mmObject = iTsyDelegates->GetTsyObject( 
        	    CMmMessageManagerBase::ECustomTsy );
            break;

        // Security (Custom) functionality
        case ECustomIsBlockedIPC:
        case ECustomCheckSecurityCodeIPC:
        //case ECustomCheckEmergencyNumberIPC:
        //case ECustomSatRefreshCompleteNotificationIPC:
        case ECustomDisablePhoneLockIPC:
        case ECustomCheckSecurityCodeCancelIPC:
        case EMmTsySecurityGetSimActivePinStateIPC:
        	mmObject = iTsyDelegates->GetTsyObject( 
        	    CMmMessageManagerBase::ESecurityTsy );
            break;
        // This is a special case
        case EMobileCallNotifyPrivacyConfirmation:
            break; 
        default:
        	mmObject = iTsyDelegates->GetTsyObject( 
        	    CMmMessageManagerBase::EPhoneTsy );
            break;
        }
        
	// Route completion to right TSY 	
    if ( NULL != mmObject )
        {
        switch ( aIpc )
            {
            case EMobileCallHold:                
                static_cast<CMmVoiceCallTsy*>( mmObject )->
                    CompleteHold( aResult );
                break;
            case EMobileCallDialNoFdnCheck:
            	static_cast<CMmCallTsy*>( mmObject )->
                	CompleteDialNoFdn( aResult );
            	break;
            case EEtelCallDial:
            case EMobileCallDialISV:                
                static_cast<CMmCallTsy*>( mmObject )->
                    CompleteDial( aResult );
                break;                
            case EEtelCallAnswer:
            case EMobileCallAnswerISV:                
                static_cast<CMmCallTsy*>( mmObject )->
                    CompleteAnswerIncomingCall( aResult );
                break;                
            case EEtelCallHangUp:                
                static_cast<CMmCallTsy*>( mmObject )->
                    CompleteHangUp( aResult );
                break;                
            case EMobileCallResume:                
                static_cast<CMmVoiceCallTsy*>( mmObject )->
                    CompleteResume( aResult );
                break;                
            case EMobileCallSwap:                
                static_cast<CMmVoiceCallTsy*>( mmObject )->
                    CompleteSwap( aResult );
                break;                
            case EMobileCallTransfer:                
                static_cast<CMmVoiceCallTsy*>( mmObject )->
                    CompleteTransfer( aResult );
                break;                
            case EMobileCallGoOneToOne:                
                static_cast<CMmVoiceCallTsy*>( mmObject )->
                    CompleteGoOneToOne( aResult );
                break;                
            case EMobileCallNotifyMobileCallStatusChange:                
                static_cast<CMmCallTsy*>( mmObject )->
                    CompleteNotifyStatusChange( aResult, aDataPackage );
                // phone idicator status may change
                iMmPhone->CompleteNotifyIndicatorChange();
                break;                
            case EMobileCallGetMobileCallInfo:                
                static_cast<CMmCallTsy*>( mmObject )->
                    CompleteNotifyMobileCallInfoChange( aDataPackage );
                break;                
            case EMobileCallNotifyCallEvent:                
                static_cast<CMmCallTsy*>( mmObject )->
                    CompleteNotifyRemoteCallEvent( aDataPackage );
                break;
            case EMobileCallReceiveUUI:                
                static_cast<CMmVoiceCallTsy*>( mmObject )->
                    CompleteReceiveUUI( aDataPackage, aResult );
                break;
            case EMobileConferenceCallGetMobileCallInfo:                
                static_cast<CMmCallTsy*>( mmObject )->
                    CompleteNotifyMobileCallInfoChange( aDataPackage );
                break;                
            case EEtelLineNotifyCallAdded:                
                static_cast<CMmLineTsy*>( mmObject )->
                    CompleteNotifyAddBypassingCall( aDataPackage );
                break;                
            case EEtelLineNotifyIncomingCall:                
                static_cast<CMmLineTsy*>( mmObject )->
                    CompleteNotifyIncomingCall( aDataPackage );
                break;                
            case EMobilePhoneSetNetworkSelectionSetting:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteSetNetworkSelectionSetting( aResult );
                break;                
            case EMobilePhoneNotifyNetworkRegistrationStatusChange:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteNotifyNetworkRegistrationStatusChange( 
                    aDataPackage, aResult );
                // phone indicator status may change
                iMmPhone->CompleteNotifyIndicatorChange();
                break;                
            case EMobilePhoneNotifyModeChange:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteNotifyModeChange( aDataPackage, aResult );
                break;                
            case EMobilePhoneNotifyCurrentNetworkChange:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteNotifyCurrentNetworkChange( 
                    aDataPackage, aResult );
                break;                
            case EMobilePhoneNotifyNetworkSelectionSettingChange:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteNotifyNetworkSelectionSettingChange( 
                                                          aDataPackage );
                break;                
            case EMobilePhoneSelectNetwork:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteSelectNetwork( aResult );
                break;                
            case EMobilePhoneGetDetectedNetworksV2Phase1:
                {
                CMmNetTsy* netTsy = 
                    static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy();

                netTsy->ProcessGetDetectedNetworksPhase1L(  
                    aDataPackage, aResult );
                break;
                }
            case EMobilePhoneAuthorizationInfoPhase1:
                {
                CMmNetTsy* netTsy = 
                    static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy();                
                netTsy->ProcessGetAuthorizationInfoPhase1L(
                    aDataPackage, aResult );
                break;
                }
            case EMobilePhoneGetHomeNetwork:
                {
                CMmPhoneTsy* phoneTsy = 
                    static_cast<CMmPhoneTsy*>( mmObject );
                static_cast<CMmPhoneExtInterface*>( phoneTsy->
                    PhoneExtInterface() )->CompleteGetHomeNetwork( 
                    aDataPackage );
                break;
                }
			case EMmTsySimRefreshRegisterIPC:
            	{
            	CMmPhoneTsy* phoneTsy =
                	static_cast<CMmPhoneTsy*>( mmObject );
            	phoneTsy->CompleteSimRefreshRegister( aResult );
            	break;
            	}
            case EMobilePhoneGetDetectedNetworksCancel:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteGetDetectedNetworksCancel( aResult );
                break;
            case EMobilePhoneSelectNetworkCancel:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteSelectNetworkCancel( aResult );
                break;
            case EMobilePhoneNotifySignalStrengthChange:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteNotifySignalStrengthChange( aDataPackage );
                break;
            case EMobilePhoneNotifyNITZInfoChange:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteNotifyNITZInfoChange( aDataPackage );
                break;
            case ECtsyPhoneCellInfoComp:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteCellInfoReq( aDataPackage, aResult );
                break;            	            	
            case ECtsyPhoneCellInfoIndComp:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                	CompleteCellInfoIndReq( aDataPackage, aResult );
                break;            	
            case EMobilePhoneGetNetworkRegistrationStatus:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteGetNetworkRegistrationStatus( aResult );
                break;
            case EMobilePhoneGetCipheringIndicatorStatus:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteGetCipheringIndicatorStatus( 
                    aDataPackage, aResult );
                break;
            case EMobilePhoneNotifyNetworkSecurityLevelChange:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteNotifyNetworkSecurityLevelChange( 
                    aDataPackage );
                break;
            
      		case EMobilePhoneGetCurrentActiveUSimApplication:
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    CompleteGetCurrentActiveUSimApplication( 
                    aDataPackage, aResult );
                break;                                
#ifdef USING_CTSY_DISPATCHER
    		case ECtsyUpdateLifeTimeComp:
            	{
            	CMmPhoneTsy* phoneTsy =
                	static_cast<CMmPhoneTsy*>( mmObject );
            	phoneTsy->CompleteUpdateLifeTimer( aResult );
            	break;
            	}
#else
				case EMmTsyUpdateLifeTimeIPC:
            	{
            	CMmPhoneTsy* phoneTsy =
                	static_cast<CMmPhoneTsy*>( mmObject );
            	phoneTsy->CompleteUpdateLifeTimer( aResult );
            	break;
            	}                                 
#endif //USING_CTSY_DISPATCHER
      		case ECtsyPhoneTerminateAllCallsComp:
      			static_cast<CMmPhoneTsy*>( mmObject )->
      				CompleteTerminateAllCallsReq( aResult );
      			break;
            case ECtsyPhoneTerminateActiveCallsComp:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteTerminateActiveCallsReq( aResult );
                break;
                
            // Conference call functionality
            case EMobileConferenceCallCreateConference:                
                static_cast<CMmConferenceCallTsy*>( mmObject )->
                    CompleteCreateConference( aResult );
                break;                
            case EMobileConferenceCallAddCall:                
                static_cast<CMmConferenceCallTsy*>( mmObject )->
                    CompleteAddCall( aResult );
                break;                
            case EMobileConferenceCallSwap:                
                static_cast<CMmConferenceCallTsy*>( mmObject )->
                    CompleteSwap( aResult );
                break;                
            case EMobileConferenceCallHangUp:                
                static_cast<CMmConferenceCallTsy*>( mmObject )->
                    CompleteHangUp( aResult );
                break;                
            case EMobileConferenceCallNotifyConferenceStatusChange:                
                static_cast<CMmConferenceCallTsy*>( mmObject )->
                    CompleteNotifyConferenceStatusChange();
                break;                
            case EMobileConferenceCallNotifyConferenceEvent:
                static_cast<CMmConferenceCallTsy*>( mmObject )->
                    GetActiveConferenceCallExtension()->
                    CompleteNotifyConferenceEvent( aDataPackage );
                break;
            // case EMobileConferenceCallGetCaps:
            // case EMobileConferenceCallNotifyCapsChange:
            // case EMobileConferenceCallEnumerateCalls:
            // case EMobileConferenceCallGetConferenceStatus:

            // case EMobileConferenceCallNotifyCapsChangeCancel:
            // case EMobileConferenceCallCreateConferenceCancel:
            // case EMobileConferenceCallAddCallCancel:
            // case EMobileConferenceCallSwapCancel:
            // case EMobileConferenceCallHangUpCancel:
            // case EMobileConferenceCallNotifyConferenceStatusChangeCancel:
            // case EMobileConferenceCallNotifyConferenceEventCancel:

            // DTMF functionality
            case EMobilePhoneSendDTMFTones:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetDtmfTsy()->
                    CompleteSendDTMFTones( aResult );
                break;
#ifdef USING_CTSY_DISPATCHER
            case EMobilePhoneStartDTMFTone:
            	static_cast<CMmPhoneTsy*>( mmObject )->GetDtmfTsy()->
            		CompleteStartDTMFTone( aResult );
            	break;
            case EMobilePhoneStopDTMFTone:
            	static_cast<CMmPhoneTsy*>( mmObject )->GetDtmfTsy()->
            		CompleteStopDTMFTone( aResult );
            	break;
#else
            case EMobilePhoneStopDTMFTone:
#endif //USING_CTSY_DISPATCHER             	
            case EMobilePhoneSendDTMFTonesCancel:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetDtmfTsy()->
                    CompleteSendDTMFTonesCancel( aResult );
                break;                
            case EMobilePhoneNotifyStopInDTMFString:
                static_cast<CMmPhoneTsy*>( mmObject )->GetDtmfTsy()->
                    CompleteNotifyStopInDTMFString( aDataPackage );
                break;

            // case EMobilePhoneReadDTMFTones:       not supported
            // case EMobilePhoneReadDTMFTonesCancel: not supported
            // case EMobilePhoneStartDTMFTone:                no completion
            // case EMobilePhoneContinueDTMFStringSending:    no completion
            // case EMobilePhoneGetDTMFCaps:                  no DOS call required
            // case EMobilePhoneNotifyDTMFCapsChange:         no DOS call required
            // case EMobilePhoneNotifyDTMFCapsChangeCancel:   no DOS call required
            // case EMobilePhoneNotifyDTMFEventCancel:		  no DOS call required
            // case EMobilePhoneNotifyStopInDTMFStringCancel: no DOS call required

            case EMobilePhoneNotifyDTMFEvent: 				//no DOS call required
	           	// unpack result: a RMobilePhone::TMobilePhoneDTMFEvent
				TInt dtmfInfo;
				aDataPackage->UnPackData( dtmfInfo );
				static_cast<CMmDtmfTsy*>( mmObject )->CompleteNotifyDTMFEvent( dtmfInfo );
				break;
            case EMmTsyPhoneBookStoreInitIPC:                
                {
                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                    CompletePBStoreInitializationL( 
                    aResult, aDataPackage );
                    TBool done = EFalse;
                    
                    // NOTE: The statement above is redundant, as the loop below calls CompletePBStoreInitializationL
                    // on every phonebook store. However, we need to be certain that changing the order in which
                    // phonebook stores are initialised won't introduce side effects. (One future improvement)
                    for( TInt i = 0; i < iMmPhone->PBList()->GetNumberOfObjects(); i++ )
        	            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_3, "TSY: CMmMessageRouterProxy::RouteCompletion:EMmTsyPhoneBookStoreInitIPC for loop, check init statuses ");
        	            CMmPhoneBookStoreTsy* pbStore = iMmPhone->PBList()->
        	                GetMmPBByIndex( i );
                        done = pbStore->IsPBInitDone();
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_4, "TSY: CMmMessageRouterProxy::RouteCompletion: active: %u, done: %u",(TUint)iMmPhone->IsPBInitActive(), (TUint)done);
        	            if ( iMmPhone->IsPBInitActive() && !done ) 
        	                {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_5, "TSY: CMmMessageRouterProxy::RouteCompletion: complete also to: %S",*(pbStore->PhoneBookName()));
        	                mmObject = static_cast<CBase*>( pbStore );
        	                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                                CompletePBStoreInitializationL( 
                                    aResult, aDataPackage );
        	                }
        	            }
                iMmPhone->SetPBInitActiveStatus( EFalse );    
                }
                break;                
            case EMmTsyPhoneBookStoreCacheIPC:                
                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                    CompleteCachingL( aResult, aDataPackage );
                break;                
            case EMmTsyPhoneBookStoreReadIPC:                
                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                    CompleteReadL( aResult, aDataPackage );
                break;                
            case EMmTsyPhoneBookStoreWriteIPC:                
                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                    CompleteWriteL( aResult, aDataPackage );
                break;                
            case EMmTsyPhoneBookStoreGetInfoIPC:                
                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                    CompleteCountEntries( aResult, aDataPackage );
                break;                
            case EMmTsyPhoneBookStoreDeleteIPC:                
                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                    CompleteDeleteL( aResult, aDataPackage );
                break;                
            case EMmTsyPhoneBookStoreDeleteAllIPC:                
                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                    CompleteDeleteAll( aResult, aDataPackage );
                break;                
            case EMmTsyPhoneBookStoreResetCacheIPC:                
                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                    ResetCacheReady();
                static_cast<CMmPhoneBookStoreTsy*>( mmObject )->
                    ResetCache();
                break;                
            
            // CSD functionality
            case EEtelPhoneNotifyModemDetected:
                {
                RPhone::TStatus* modemStatus = NULL;
                //unpack the data
                aDataPackage->UnPackData( &modemStatus );
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteNotifyModemDetected( *modemStatus );
                break;
                }
            case EMobileCallSetDynamicHscsdParams:                
                static_cast<CMmDataCallTsy*>( mmObject )->
                    CompleteSetDynamicHscsdParams( aResult );
                break;                
            case EMobileCallNotifyMobileDataCallCapsChange:                
                static_cast<CMmDataCallTsy*>( mmObject )->
                    ActiveCallExtension()->
                    ReceivedMobileDataCallCaps( aDataPackage );
                break;                
            case EMobileCallNotifyHscsdInfoChange:                
                static_cast<CMmDataCallTsy*>( mmObject )->
                    ActiveCallExtension()->ReceivedHscsdInfo( 
                    aDataPackage );
                break;                
            case EEtelCallGetCallParams:                
                static_cast<CMmCallTsy*>( mmObject )->
                ActiveCallExtension()->ReceivedCallParams( aDataPackage );
                break;                
            case EEtelCallGetBearerServiceInfo:
                {
                RCall::TBearerService* bearerService = NULL;
                aDataPackage->UnPackData( &bearerService );
                static_cast<CMmCallTsy*>( mmObject )->
                    ActiveCallExtension()->ReceivedBearerServiceInfo( 
                    *bearerService );
                break;
                }
            case EMmTsyENStoreGetInfoIPC:                
                static_cast<CMmENStoreTsy*>( mmObject )->CompleteGetInfo(
                    aResult, aDataPackage );
                break;                
            case EMmTsyENStoreReadIPC:                
                static_cast<CMmENStoreTsy*>( mmObject )->CompleteRead(
                    aResult, aDataPackage );
                break;                
            case EMmTsyENStoreReadAllPhase1IPC:                
                static_cast<CMmENStoreTsy*>( mmObject )->
                        CompleteReadAllPhase1L( aResult, aDataPackage );
                break;                
            case EMmTsyONStoreGetInfoIPC:                
                static_cast<CMmONStoreTsy*>( mmObject )->CompleteGetInfo(
                    aResult, aDataPackage );
                break;                
            case EMmTsyONStoreReadIPC:                
                static_cast<CMmONStoreTsy*>( mmObject )->CompleteRead(
                    aResult, aDataPackage );
                break;                
            case EMmTsyONStoreWriteIPC:                
                static_cast<CMmONStoreTsy*>( mmObject )->CompleteWrite(
                    aResult, aDataPackage );
                break;                
            case EMmTsyONStoreDeleteIPC:                
                static_cast<CMmONStoreTsy*>( mmObject )->CompleteDelete(
                    aResult );
                break;                
            case EMmTsyONStoreDeleteAllIPC:                
                static_cast<CMmONStoreTsy*>( mmObject )->
                    CompleteDeleteAll( aResult );
                break;
            case EMmTsyONStoreReadAllPhase1IPC:
                static_cast<CMmONStoreTsy*>( mmObject )->
                    CompleteReadAllPhase1L();
                break;
            case EMmTsyONStoreReadEntryIPC:
   				STATIC_CAST( CMmONStoreTsy*, mmObject )->
                    InternalRetrieveONListReadEntryL( 
                    aResult, aDataPackage );
                break;
            case EMmTsyONStoreReadSizeIPC:
                STATIC_CAST( CMmONStoreTsy*, mmObject )->
                    InternalRetrieveONListReadSizeL( 
                    aResult, aDataPackage );
                break;
            case EMmTsyONStoreWriteSizeIPC:
                STATIC_CAST( CMmONStoreTsy*, mmObject )->
                    InternalStoreAllGetSizeL( aResult, aDataPackage );
                break;
            case EMmTsyONStoreWriteEntryIPC:
   				STATIC_CAST( CMmONStoreTsy*, mmObject )->
                    InternalStoreAllWriteEntryL( aResult );
                break;
            case EPacketAttach:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                ActivePacketServiceExtension()->CompleteAttach( 
                    aResult );
	            break;	            
	        case EPacketDetach:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                ActivePacketServiceExtension()->CompleteDetachL( 
                    aResult );
	            break;	            
	        case EPacketGetNtwkRegStatus:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                ActivePacketServiceExtension()->
                    CompleteGetNtwkRegStatus( aDataPackage, aResult );
	            break;	            
	            
	        case ECustomSetAlwaysOnMode:
            	STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
                    CompleteSetAlwaysOn( aResult );
            	break;	            
	        case EPacketSetAttachMode:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                ActivePacketServiceExtension()->CompleteSetAttachMode( 
	                aResult );
	            break;	            
	        case EPacketRejectActivationRequest:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                CompleteRejectActivationRequest();
	            break;	            
	        case EPacketGetAttachMode:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                ActivePacketServiceExtension()->CompleteGetAttachMode(
	                aDataPackage, aResult );
	            break;	            
	        case EPacketSetDefaultContextParams:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                ActivePacketServiceExtension()->
	                CompleteSetDefaultContextParams( aResult );
	            break;	            
	        case EPacketNotifyContextAdded:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                CompleteNotifyContextAdded( aDataPackage);
	            break;	            
	        case EPacketNotifyStatusChange:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                CompletePacketStatusChanged( aDataPackage, aResult );
	            break;	            
	        case EPacketNotifyDynamicCapsChange:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                ActivePacketServiceExtension()->
                    CompleteDynamicCapsChange( aDataPackage );
	            break;	            
            case EPacketNotifyTransferCapsIPC:                  
                STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
                    ActivePacketServiceExtension()->
                    CompleteTransferCapsChange( aDataPackage );
	            break;   	                	            
	        case EPacketNotifyMSClassChange:	          
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                CompleteNotifyMSClassChange( aDataPackage );
	            break;	            
	        case EPacketSetPrefBearer:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                ActivePacketServiceExtension()->
                    CompleteSetPreferredBearer( aResult );
	            break;	            
	        case EPacketNotifyChangeOfNtwkRegStatus:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                CompleteNotifyChangeOfNtwkRegStatus( aDataPackage, aResult );
	            break;	            
	        case EMmTsyNotifyEGprsInfoChangeIPC:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                CompleteNotifyEGprsInfoChange( aDataPackage );
	            break;	            
	        case EPacketNotifyAttachModeChange:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                PreferredBearerChanged( aDataPackage );
	            break;	            
	        case EPacketNotifyContextActivationRequested:	            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                CompleteNotifyContextActivationRequested( 
                        aDataPackage );
	            break;	            
            case EPacketNotifyMbmsNetworkServiceStatusChange:
            	static_cast<CMmPacketServiceTsy*>(mmObject)->
            	CompleteNotifyMbmsNetworkServiceStatusChange(aDataPackage, aResult);
            	break;
            case EPacketNotifyMbmsServiceAvailabilityChange:
            	static_cast<CMmPacketServiceTsy*>(mmObject)->
            	CompleteNotifyMbmsServiceAvailabilityChangeL(aDataPackage, aResult);
            	break;
            case EPacketGetMbmsNetworkServiceStatus:
            	static_cast<CMmPacketServiceTsy*>(mmObject)->
            	CompleteGetMbmsNetworkServiceStatus(aDataPackage, aResult);
            	break;
            case EPacketGetStaticCaps:
            	static_cast<CMmPacketServiceTsy*>(mmObject)->
            	ActivePacketServiceExtension()->CompleteGetStaticCaps(aDataPackage, aResult);
            	break;
            case EPacketEnumerateMbmsMonitorServiceList:
            	{
            	if (KErrNone == aResult)
                    { 
                    TInt maxMonitorValue;
                    aDataPackage->UnPackData( maxMonitorValue );
                    static_cast<CMmPacketServiceTsy*>(mmObject)->CompleteMaxMonitoredServicesChange( maxMonitorValue );
                    }	
            	}
            	break;	
            case EPacketUpdateMbmsMonitorServiceList: 
	            static_cast<CMmPacketServiceTsy*> ( mmObject )
	                ->CompleteUpdateMbmsMonitorServiceList( aDataPackage, 
	                    aResult );	            
                break;
	        case EPacketContextInitialiseContext:
	            {
	            TInfoName* contextName = NULL;
#ifdef USING_CTSY_DISPATCHER
	            TDesC* channelId = NULL;
	            aDataPackage->UnPackData(&contextName, &channelId);
	            __ASSERT_DEBUG(contextName,User::Invariant());
	            __ASSERT_DEBUG(channelId,User::Invariant());
#else
	            aDataPackage->UnPackData( &contextName);
#endif //USING_CTSY_DISPATCHER	            
	            
	            CMmPacketServiceTsy* serviceTsy = static_cast<CMmPacketServiceTsy*> (mmObject);
	                
	            CMmPacketContextTsy* mmPacketContextTsy = 
	                serviceTsy->ContextTsy( *contextName );
                    	            
	            if( NULL != mmPacketContextTsy && 
	            	mmPacketContextTsy->ContextType( ) == 
	            	CMmPacketContextTsy::EContextTypePacketContext )
	                {
	                CMmPacketContextGsmWcdmaExt* mmPacketContextExt =
	                    mmPacketContextTsy->PacketContextGsmWcdmaExt();
	                if( NULL != mmPacketContextExt )
	                    {
#ifdef USING_CTSY_DISPATCHER	                    
                        mmPacketContextExt->CompleteInitialiseContext(*channelId, aResult);
#else	                    
                            // The initialise context API will return the context name as the channelId.
                            mmPacketContextExt->CompleteInitialiseContext(aResult);
#endif //USING_CTSY_DISPATCHER                       
	                    }
	                }
	            break;
	            }
	            
	        case ECtsyPacketMbmsInitialiseContextComp:
	        	{
	        	TInfoName* contextName = NULL;
	        	RPacketContext::TDataChannelV2* dataChannelV2 = NULL;
	        	aDataPackage->UnPackData( &contextName, &dataChannelV2 );
	            CMmPacketServiceTsy* serviceTsy = static_cast<CMmPacketServiceTsy*> (mmObject);

	            CMmPacketContextTsy* mmPacketContextTsy = serviceTsy->ContextTsy( *contextName );
	        	
	        	CMmMBMSContextTsy* context = static_cast<CMmMBMSContextTsy*>  ( mmPacketContextTsy );
	        	
	        	if( NULL != context )
	        	   {
	        	   context->CompleteInitialiseContext( aResult, dataChannelV2 );	                    
	        	   }        		        
	        	break;
	        	}
	        	
	        case EPacketQoSSetProfileParams:
	            {
	            TInfoName mmPacketContextName;
	            aDataPackage->UnPackData( mmPacketContextName );

	            CMmPacketQoSTsy* mmPacketQosTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->QosTsy( 
	                mmPacketContextName );

	                if ( NULL != mmPacketQosTsy )
	                    {
	                    mmPacketQosTsy->
                            CompleteSetProfileParameters( aResult );
	                    }
	            break;
	            }
	        case EPacketQoSNotifyProfileChanged:
	            {
	            TInfoName mmPacketContextName;

	            aDataPackage->UnPackData( mmPacketContextName );

	            CMmPacketQoSTsy* mmPacketQosTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->QosTsy( 
	                mmPacketContextName );

	            if ( NULL != mmPacketQosTsy )
                    {
                    mmPacketQosTsy->PacketQoSGsmWcdmaExt()->
                        CompleteNotifyProfileChanged( aDataPackage );
                    }
	            break;
	            }
	        case EPacketNotifyReleaseModeChange:            
	            STATIC_CAST( CMmPacketServiceTsy*, mmObject )->
	                CompleteNetworkModeChange( aDataPackage );
	            break;            
	        case EPacketContextSetConfig:
	            {
	            TInfoName* contextName = NULL;
	            aDataPackage->UnPackData( &contextName );
	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                    ContextTsy( *contextName );
                    
	            if( NULL != mmPacketContextTsy && 
	            	mmPacketContextTsy->ContextType( ) == 
	            	CMmPacketContextTsy::EContextTypePacketContext )
	                {
    	            if( NULL != mmPacketContextTsy )
    	                {
    	                CMmPacketContextGsmWcdmaExt* mmPacketContextGsmWcdma =
    	                    mmPacketContextTsy->PacketContextGsmWcdmaExt();

    	                if( NULL != mmPacketContextGsmWcdma )
    	                    {
    	                    mmPacketContextGsmWcdma->
                                CompleteSetConfig( aResult);
    	                    }
    	                }
	                }
	            else
	                {	                
	                CMmMBMSContextTsy* context = static_cast<CMmMBMSContextTsy*> ( mmPacketContextTsy );
	                if( NULL != context )
	                    {
                        context->CompleteSetConfig( 
                        aResult );
                        }	                    	                
	                }
	            break;
	            }
            case EPacketEnumerateMbmsActiveServiceList:
                { 
                static_cast<CMmPacketServiceTsy*> ( mmObject )->
	                    CompleteEnumerateMbmsActiveServiceList( 
	                        aDataPackage, aResult );
                break;
                }
	            
            case EPacketContextUpdateMbmsSessionList:
                {
	            TInfoName* contextName = NULL;
	            aDataPackage->UnPackData( &contextName );
	            
	            CMmPacketContextTsy* mmPacketContextTsy = static_cast<CMmPacketServiceTsy*> ( mmObject )->
                    ContextTsy( *contextName );
                    
	            if( NULL != mmPacketContextTsy && 
	            		mmPacketContextTsy->ContextType( ) == 
	            		CMmPacketContextTsy::EContextTypeMBMS )
	                {
	                static_cast<CMmMBMSContextTsy*> 
	                    ( mmPacketContextTsy )->CompleteUpdateMbmsSessionList( aResult );
	                }
                break;
                }    
	        case EPacketContextDelete:
	            {
	            TInfoName* contextName = NULL;
	            aDataPackage->UnPackData( &contextName );
	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                    ContextTsy( *contextName );
	            
	            if( NULL != mmPacketContextTsy && 
	            	mmPacketContextTsy->ContextType( ) == 
	            	CMmPacketContextTsy::EContextTypePacketContext )
	                {
	                CMmPacketContextGsmWcdmaExt* mmPacketContextGsmWcdma =
	                    mmPacketContextTsy->PacketContextGsmWcdmaExt();

	                if( NULL != mmPacketContextGsmWcdma )
	                    {
	                    mmPacketContextGsmWcdma->CompleteDelete( aResult );
	                    }
	                }
	            else
	                {
	                CMmMBMSContextTsy* context = static_cast< 
	                    CMmMBMSContextTsy*>( mmPacketContextTsy );
	                if( NULL != context )
	                    {
                        context->CompleteDelete( 
                        aResult );
                        }	                	                
	                }
	            break;
	            }
	        case EPacketContextNotifyDataTransferred:
	            {
	            TInfoName* contextName = NULL;
	            RPacketContext::TDataVolume* dataVolume;
	            aDataPackage->UnPackData( &contextName, &dataVolume );
	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                    ContextTsy( *contextName );
	            if( NULL == mmPacketContextTsy && 
	                ( 0 == ( *contextName ).Compare( KStringExternal ) || 
	                0 == ( *contextName ).Compare( KStringExternal2 ) ) )
	                {
	                mmPacketContextTsy = STATIC_CAST(
	                    CMmPacketServiceTsy*, mmObject )->DialUpContext( 
	                    *contextName );
	                }
	            if( NULL != mmPacketContextTsy )
	                {
	                mmPacketContextTsy->
                        SetDataVolume( aResult, *dataVolume );
	                }
	            break;
	            }
	        case EPacketContextActivate:
	            {
	            TInfoName* contextName = NULL;
	            aDataPackage->UnPackData( &contextName);
	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                                                ContextTsy( *contextName );
                                                    	            
	            if( NULL == mmPacketContextTsy && 
	                ( 0 == (*contextName).Compare(KStringExternal) || 
	                0 == (*contextName).Compare(KStringExternal2) ) )
	                {
	                mmPacketContextTsy = STATIC_CAST(
	                    CMmPacketServiceTsy*, mmObject )->DialUpContext(
                        *contextName );
	                }
	            if( (mmPacketContextTsy != NULL) && (mmPacketContextTsy->ContextType( ) == 
	            	CMmPacketContextTsy::EContextTypePacketContext ))
	                {
	                CMmPacketContextGsmWcdmaExt* mmPacketContextGsmWcdma =
    	                    mmPacketContextTsy->PacketContextGsmWcdmaExt();
	                if( NULL != mmPacketContextGsmWcdma )
	                	{
	                	mmPacketContextGsmWcdma->CompleteActivate(aDataPackage, aResult );
	                	}
	                }
	            else if( (mmPacketContextTsy != NULL) && ( mmPacketContextTsy->ContextType( ) == 
	                			CMmPacketContextTsy::EContextTypeMBMS ))
	                    {	                    
    	                CMmMBMSContextTsy* mbmsContext = static_cast<
    	                    CMmMBMSContextTsy*>( mmPacketContextTsy );
    	                mbmsContext->CompleteActivate( aResult );	            	                    	                    
	                    }	                    
	            break;
	            }
	        case ECtsyPacketMbmsContextNotifyConfigChanged:
	        	{
	        	TInfoName* contextName = NULL;
	            aDataPackage->UnPackData( &contextName );

	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                                              ContextTsy( *contextName );                                              	                	                
	            if( (mmPacketContextTsy != NULL) && ( mmPacketContextTsy->ContextType( ) == 
	            	CMmPacketContextTsy::EContextTypeMBMS ) )
	            	{
	            	CMmMBMSContextTsy* mbmsContext = static_cast<CMmMBMSContextTsy*>( mmPacketContextTsy );
	            	mbmsContext->CompleteNotifyConfigChanged(aDataPackage,aResult);
	            	}
	            
	            break;
	           	}
	        
	        case EPacketContextNotifyConfigChanged:
	            {
	            TInfoName* contextName = NULL;
	            aDataPackage->UnPackData( &contextName );

	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                                              ContextTsy( *contextName );                                              	                	                
	            if( NULL == mmPacketContextTsy )
	                {
	                mmPacketContextTsy = STATIC_CAST(
	                    CMmPacketServiceTsy*, mmObject )->DialUpContext( 
	                    *contextName );
	                }
	            if( ( NULL != mmPacketContextTsy ) &&(mmPacketContextTsy->ContextType() == 
	            	CMmPacketContextTsy::EContextTypePacketContext ))
	                {
	                CMmPacketContextGsmWcdmaExt* mmPacketContextGsmWcdma = mmPacketContextTsy->PacketContextGsmWcdmaExt();
	                if( NULL != mmPacketContextGsmWcdma )
	                	{
	                	mmPacketContextGsmWcdma->CompleteActivatePhase2(
	                			aDataPackage, aResult );
	                	}
	                }
	            break;
	            }
	        case EPacketContextNotifyConnectionSpeedChange:
	            {
	            TInfoName* contextName = NULL;
	            TInt* connectionSpeed = NULL;
	            aDataPackage->UnPackData( &contextName, &connectionSpeed );

	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                    ContextTsy( *contextName );
	            
	            if( NULL == mmPacketContextTsy )
	                {
	                mmPacketContextTsy = STATIC_CAST(
	                    CMmPacketServiceTsy*, mmObject )->DialUpContext(
                        *contextName );
	                }
	            if( NULL != mmPacketContextTsy )
	                {
                    mmPacketContextTsy->
                        CompleteNotifyConnectionSpeedChange(
                        *connectionSpeed );
	                }
	            break;
	            }
	        case EPacketContextNotifyStatusChange:
	            {
	            TInfoName* contextName = NULL;
	            aDataPackage->UnPackData( &contextName );
	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
                    CMmPacketServiceTsy*, mmObject )->
                    ContextTsy( *contextName );
	                
	            if( NULL == mmPacketContextTsy && 
	                ( 0 == ( *contextName ).Compare( KStringExternal ) || 
	                0 == ( *contextName ).Compare( KStringExternal2 ) ) )
	                {
	                mmPacketContextTsy = STATIC_CAST(
	                    CMmPacketServiceTsy*, mmObject )->DialUpContext(
                        *contextName );
	                }
	     
	            if( NULL != mmPacketContextTsy )
	                {
	                // pdp context
    	            if( mmPacketContextTsy->ContextType( ) == 
    	            	CMmPacketContextTsy::EContextTypePacketContext )
    	                { 	                
    	                CMmPacketContextGsmWcdmaExt* mmPacketContextGsmWcdma =
    	                    mmPacketContextTsy->PacketContextGsmWcdmaExt();

    	                if( NULL != mmPacketContextGsmWcdma )
    	                    {
                          mmPacketContextGsmWcdma->CompleteNotifyStatusChange(
                                aDataPackage, aResult );
    	                    }
    	                }
    	            // mbms context
    	            else if( (mmPacketContextTsy != NULL) && (mmPacketContextTsy->ContextType( ) == 
    	            			CMmPacketContextTsy::EContextTypeMBMS ))
    	                    {	                    
    	                CMmMBMSContextTsy* mbmsContext = static_cast<
    	                    CMmMBMSContextTsy*>( mmPacketContextTsy );
    	                mbmsContext->CompleteNotifyStatusChange(
                                aDataPackage, aResult );            	                    	                    
	                    } 
	                }
	            break;
	            }
	        case EPacketContextDeactivate:
	            {
            	TInfoName* contextName = NULL;
            	
            	// only packed for MBMS in a particular fail scenario
            	RPacketMbmsContext::CMbmsSession* session = NULL;
         
	            if(aResult == KErrMbmsImpreciseServiceEntries)
	            	{
	            	// definitely an MBMS session
	            	aDataPackage->UnPackData( &contextName, &session );
	            	}
	            else
	            	{
	            	aDataPackage->UnPackData( &contextName );
	            	}
	            
	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                                                ContextTsy( *contextName );
                                             
	            if( NULL == mmPacketContextTsy )
	                {
	                mmPacketContextTsy = STATIC_CAST(
	                    CMmPacketServiceTsy*, mmObject )->DialUpContext(
                        *contextName );
	                }
	            if( (mmPacketContextTsy != NULL) &&(mmPacketContextTsy->ContextType() == 
	            	CMmPacketContextTsy::EContextTypePacketContext ))
	                {		            

    	            if( NULL != mmPacketContextTsy )
    	                {
    	                CMmPacketContextGsmWcdmaExt* mmPacketContextGsmWcdma =
    	                    mmPacketContextTsy->PacketContextGsmWcdmaExt();

    	                if( NULL != mmPacketContextGsmWcdma )
    	                    {
    	                    mmPacketContextGsmWcdma->
                                CompleteDeactivate(aResult);
    	                    }
    	                }
	                }
	                else if((mmPacketContextTsy != NULL) && (mmPacketContextTsy->ContextType( ) == 
	                	CMmPacketContextTsy::EContextTypeMBMS ))
	                    {
    	                CMmMBMSContextTsy* mbmsContext = static_cast<
    	                    CMmMBMSContextTsy*>( mmPacketContextTsy );
    	                mbmsContext->CompleteDeactivate(aDataPackage, aResult);	            	                    	                    
	                    }	                
	            break;
	            }
	        case EPacketContextGetDataVolumeTransferred:
	            {
	            TInfoName* contextName = NULL;
	            aDataPackage->UnPackData( &contextName );
	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                    ContextTsy( *contextName );

	            if( NULL == mmPacketContextTsy )
	                {
	                mmPacketContextTsy = STATIC_CAST(
	                    CMmPacketServiceTsy*, mmObject )->DialUpContext(
                        *contextName );
	                }

	            if( NULL != mmPacketContextTsy )
	                {
	                CMmPacketContextGsmWcdmaExt* mmPacketContextGsmWcdma =
	                    mmPacketContextTsy->PacketContextGsmWcdmaExt();

	                if( NULL != mmPacketContextGsmWcdma )
	                    {
                        mmPacketContextGsmWcdma->
                            CompleteGetDataVolumeTransferred( aDataPackage,
                            aResult );
	                    }
	                }
	            break;
	            }
	        case EPacketContextModifyActiveContext:
	            {
	            TInfoName* contextName = NULL;
	            aDataPackage->UnPackData( &contextName );
	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                    ContextTsy( *contextName );
	            
	            if( NULL != mmPacketContextTsy )
	                {
	                CMmPacketContextGsmWcdmaExt* mmPacketContextGsmWcdma =
	                    mmPacketContextTsy->PacketContextGsmWcdmaExt();

	                if( NULL != mmPacketContextGsmWcdma )
	                    {
	                    mmPacketContextGsmWcdma->
                        CompleteModifyActiveContext( 
                        aDataPackage, aResult );
	                    }
	                }
	            break;
	            }	            
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	        case EPacketContextNotifyConnectionInfoChange:
	            {
	            TInfoName* contextName = NULL;
	            TConnectionInfoBase* infoBase = NULL;
	            aDataPackage->UnPackData( &contextName, &infoBase );

	            CMmPacketContextTsy* mmPacketContextTsy = STATIC_CAST( 
	                CMmPacketServiceTsy*, mmObject )->
                    ContextTsy( *contextName );
	            
	            if( NULL != mmPacketContextTsy )
	                {
                    mmPacketContextTsy->
                        CompleteNotifyConnectionInfoChange( infoBase );
	                }
	            break;
	            }
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	            	            	            
            //USSD
            case EMobileUssdMessagingSendMessage:                
                static_cast<CMmUssdTsy*>( mmObject )->
                    CompleteSendMessage( aResult );
                break;                
            case EMobileUssdMessagingSendMessageNoFdnCheck:                
                static_cast<CMmUssdTsy*>( mmObject )->
                    CompleteSendMessageNoFdnCheck( aResult );
                break;                                
            case EMobileUssdMessagingReceiveMessage:                
                static_cast<CMmUssdTsy*>( mmObject )->
                    CompleteReceiveMessage( aResult, aDataPackage );
                break;                
            case EMobileUssdMessagingSendRelease:                
                static_cast<CMmUssdTsy*>( mmObject )->
                    CompleteSendRelease( aResult, aDataPackage );
                break; 
            case EMobileUssdMessagingNotifyNetworkRelease:                
                static_cast<CMmUssdTsy*>( mmObject )->
                    CompleteNotifyNetworkRelease( aResult, aDataPackage );
                break;                
            case EMmTsyGsmBroadcastNotifyMessageReceived:                
                static_cast<CMmBroadcastTsy*>( mmObject )->
                    CompleteReceiveMessageGsmCbs( aResult, aDataPackage );
                break;                
            case EMmTsyWcdmaBroadcastNotifyMessageReceived:                
                static_cast<CMmBroadcastTsy*>( mmObject )->
                  CompleteReceiveMessageWcdmaCbs( aResult, aDataPackage );
                break;                
            case EMobileBroadcastMessagingReceiveMessage:                
                static_cast<CMmBroadcastTsy*>( mmObject )->
                    InternalCompleteCbRoutingRequest( aResult );
                break;                
            case EMobileBroadcastMessagingReceiveMessageCancel:                
                static_cast<CMmBroadcastTsy*>( mmObject )->
                    CompleteReceiveMessageCancel( aResult );
                break;                
            case EMobileBroadcastMessagingSetFilterSetting:                
                static_cast<CMmBroadcastTsy*>( mmObject )->
                    CompleteSetFilterSetting( aResult );
                break;                
            case EMobileSmsMessagingGetMessageStoreInfo:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteGetMessageStoreInfo( aResult, aDataPackage );
                break;                
            case EMmTsyActivateSmsRouting:
            case EMmTsyDeactivateSmsRouting:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteActivateSmsRouting( aResult, aDataPackage );
                break;                
            case EMobileSmsMessagingReceiveMessage:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteReceiveMessage( aResult, aDataPackage );
                break;                
            case EMobileSmsMessagingSendMessage:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteSendMessage( aResult, aDataPackage );
                break;                
            case EMobileSmsMessagingSendMessageNoFdnCheck:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteSendMessageNoFdnCheck( aResult, aDataPackage );
                break;                
            case EMmTsySmsSendSatMessage:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteSendSatMessage( aResult );
                break;                
            case EMobileSmsMessagingResumeSmsReception:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteResumeSmsReception( aResult );
                break;                
            case EMobileSmsMessagingGetSmspListPhase1:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteReadAllSmspPhase1( aResult, aDataPackage );
                break;                
            case EMobileSmsMessagingStoreSmspList:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    InternalStoreSmspList( aResult );
                break;                
            case EMobileSmsMessagingAckSmsStored:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteAckSmsStored( aResult );
                break;                
            case EMobileSmsMessagingNackSmsStored:                
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteNackSmsStored( aResult );
                break;                
            case EMobileSmsMessagingSetMoSmsBearer:
                static_cast<CMmSmsTsy*>( mmObject )->
                    CompleteSetMoSmsBearer( aResult );
                break;
            case EMobilePhoneStoreGetInfo:                
                static_cast<CMmSmsStorageTsy*>( mmObject )->
                    CompleteGetInfo( aResult, aDataPackage );
                break;                
            case EMobilePhoneStoreDelete:                
                static_cast<CMmSmsStorageTsy*>( mmObject )->
                    CompleteDeleteSms( aResult );
                break;                
            case EMobilePhoneStoreDeleteAll:                
                static_cast<CMmSmsStorageTsy*>( mmObject )->
                    CompleteDeleteAllSms( aResult );
                break;                
            case EMobilePhoneStoreRead:               
                static_cast<CMmSmsStorageTsy*>( mmObject )->
            		CompleteReadSms( aResult, aDataPackage );
                break;                
            case EMobilePhoneStoreWrite:                
                static_cast<CMmSmsStorageTsy*>( mmObject )->
                    CompleteWriteSms( aResult, aDataPackage );
                break;                
            case EMobilePhoneStoreReadAllPhase1:
                {
                if ( iMmPhone->GetSmsTsy()->GetSmsStorageTsy() )
                    {
                    static_cast<CMmSmsStorageTsy*>( mmObject )->
                        RetrieveSmsListReadSmsL( aResult, aDataPackage );
                    }
                break;
                }
            
            // Supplementary Services functionality
            // Call Forwarding
            case EMobilePhoneGetCallForwardingStatusPhase1:
                {
                // unpack result: a CMobilePhoneCFList
                CMobilePhoneCFList* cfResults;
                aDataPackage->UnPackData( &cfResults );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteGetCallForwardingStatusPhase1L( 
                    cfResults, aResult );
                break;
                }
            case EMobilePhoneNotifyCallServiceCapsChange:                
                {
                // unpack result: a RMobilePhone::TMobilePhoneCallServiceCaps
                RMobilePhone::TMobilePhoneCallServiceCaps callServiceCaps;
                aDataPackage->UnPackData( callServiceCaps );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                  CompleteNotifyCallServiceCapsChange( callServiceCaps );
                break;
                }
            case EMobilePhoneSetCallForwardingStatus:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteSetCallForwardingStatus( aResult );
                break;                
            case EMobilePhoneNotifyCallForwardingStatusChange:
                {
                // unpack result: a RMobilePhone::TMobilePhoneCFCondition
                RMobilePhone::TMobilePhoneCFCondition cfCondition;
                aDataPackage->UnPackData( cfCondition );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                  CompleteNotifyCallForwardingStatusChange( cfCondition );
                break;
                }
            case EMobilePhoneNotifyCallForwardingActive:
                {
                // unpack results: a RMobilePhone::TMobileService
                // and a RMobilePhone::TMobilePhoneCFActive
                RMobilePhone::TMobileService serviceGroup;
                RMobilePhone::TMobilePhoneCFActive activeType;
                aDataPackage->UnPackData( serviceGroup, activeType );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteNotifyCallForwardingActive( 
                    serviceGroup, activeType );
                break;
                }
            // Call Barring
            case EMobilePhoneGetBarringStatusPhase1:
                {
                // unpack result: a CMobilePhoneCBList
                CMobilePhoneCBList* cbResults;
                aDataPackage->UnPackData( &cbResults );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                CompleteGetCallBarringStatusPhase1L( cbResults, aResult );
                break;
                }
            case EMobilePhoneSetCallBarringStatus:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteSetCallBarringStatus( aResult );
                break;                
            case EMobilePhoneNotifyCallBarringStatusChange:
                {
                // unpack result: a RMobilePhone::TMobilePhoneCBCondition
                RMobilePhone::TMobilePhoneCBCondition cbCondition;
                aDataPackage->UnPackData( cbCondition );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteNotifyCallBarringStatusChange( cbCondition );
                break;
                }
            // Supplementary Services password change
            case EMobilePhoneSetSSPassword:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteSetSSPassword( aResult );
                break;                
            // Call Waiting
            case EMobilePhoneGetWaitingStatusPhase1:
                {
                // unpack result: a CMobilePhoneCWList
                CMobilePhoneCWList* cwResults;
                aDataPackage->UnPackData( &cwResults );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                CompleteGetCallWaitingStatusPhase1L( cwResults, aResult );
                break;
                }
            case EMobilePhoneSetCallWaitingStatus:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteSetCallWaitingStatus( aResult );
                break;                
            case EMobilePhoneNotifyCallWaitingStatusChange:
                {
                // unpack result: RMobilePhone::TMobilePhoneCWInfoEntryV1
                RMobilePhone::TMobilePhoneCWInfoEntryV1* cwStatus;
                aDataPackage->UnPackData( &cwStatus );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteNotifyCallWaitingStatusChange( cwStatus );
                break;
                }
            // Identity EMobilePhoneGetIdentityServiceStatus
            case EMobilePhoneGetIdentityServiceStatus:
                {
                // if no error, then unpack result: a
                // RMobilePhone::TMobilePhoneIdServiceStatus
                RMobilePhone::TMobilePhoneIdServiceStatus status =
                    RMobilePhone::EIdServiceUnknown;
                if ( KErrNone == aResult )
                    {
                    aDataPackage->UnPackData( status );
                    }
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteGetIdentityServiceStatus( status, aResult );
                break;
                }
            // User's Access to Network Services
            case EMobilePhoneSendNetworkServiceRequest:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteSendNetworkServiceRequest( aResult );
                break;  
            case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
            	static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteSendNetworkServiceRequestNoFdnCheck( aResult );
                break;
            // Notify for Send Network Service Request
            case ECtsyPhoneSendNetworkServiceRequestInd:
            	static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
            		CompleteSendNetworkServiceRequestInd( aResult,aDataPackage);
            	break;
            // Notify for All Send Network Service Request
            case EMobilePhoneNotifyAllSendNetworkServiceRequest:
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                CompleteNotifyAllSendNetworkServiceRequest( aResult,
                    aDataPackage );
                break;
            // Get ICC message waiting indicators
            case EMobilePhoneGetIccMessageWaitingIndicators:
                {
                // packed param: RMobilePhone::TMobilePhoneMessageWaitingV1
                RMobilePhone::TMobilePhoneMessageWaitingV1* mmWaiting;
                aDataPackage->UnPackData( &mmWaiting );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteGetIccMessageWaitingIndicators( mmWaiting,
                    aResult );
                }
                break;
                
            // Set ICC message waiting indicators
            case EMobilePhoneSetIccMessageWaitingIndicators:                    
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteSetIccMessageWaitingIndicators( aResult );
                break;
                    
            // Notify ICC message waiting indicators change
            case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
                // packed param: RMobilePhone::TMobilePhoneMessageWaitingV1
                RMobilePhone::TMobilePhoneMessageWaitingV1* mmWaiting;
                aDataPackage->UnPackData( &mmWaiting );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSupplServTsy()->
                    CompleteNotifyIccMessageWaitingIndicatorChange(
                    mmWaiting, aResult );
                break;    

            // case EMobilePhoneGetCallForwardingStatusPhase2:          no DOS call required
            // case EMobilePhoneGetCallForwardingStatusCancel:          no DOS call required
            // case EMobilePhoneSetCallForwardingStatusCancel:          no DOS call required
            // case EMobilePhoneNotifyCallForwardingStatusChangeCancel: no DOS call required
            // case EMobilePhoneNotifyCallForwardingActiveCancel:       no DOS call required
            // case EMobilePhoneGetCallForwardingIndicator:             not supported
            // case EMobilePhoneGetCallForwardingIndicatorCancel:       not supported

            // Custom Tsy functionality
            case ECustomSsAdditionalInfoNotificationIPC:
                {
                // unpack pointer to RMmCustomAPI::TSsAdditionalInfo
                RMmCustomAPI::TSsAdditionalInfo* addInfo;
                aDataPackage->UnPackData( addInfo );

                static_cast<CMmCustomTsy*>( mmObject )->
                CompleteSsAdditionalInfoNotification( addInfo, aResult );
                break;
                }
            case ECustomNotifySsNetworkEventIPC:
                {
                // unpack result: a RMmCustomAPI::TSsTypeAndMode and
                // a RMmCustomAPI::TSsInfo
                RMmCustomAPI::TSsTypeAndMode ssTypeAndMode;
                RMmCustomAPI::TSsInfo ssInfo;
                aDataPackage->UnPackData( ssTypeAndMode, ssInfo );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifySsNetworkEvent( ssTypeAndMode, ssInfo, 
                    aResult );
                break;
                }
            case ECustomNotifySsRequestCompleteIPC:
                {
                // unpack result: a TInt with SS status
                TInt ssStatus;
                aDataPackage->UnPackData( ssStatus );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteSsRequestCompleteNotification( 
                    ssStatus, aResult );
                break;
                }
            case ECustomNotifyDtmfEventIPC:
                {
                // unpack result: a RMmCustomAPI::TDtmfInfo
                RMmCustomAPI::TDtmfInfo dtmfInfo;
                aDataPackage->UnPackData( dtmfInfo );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifyDtmfEvent( dtmfInfo, aResult );
                break;
                }
         	case ECustomNotifyRauEventIPC:
                {
                // unpack result
                RMmCustomAPI::TRauEventStatus rauEventStatus;
                aDataPackage->UnPackData( rauEventStatus );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifyRauEvent( rauEventStatus, aResult );
                break;
                }
            case ECustomGetDiagnosticOctetsIPC:
                {
                // unpack result: TInt (call ID) and TUint8 (diagnostics)
                TInt callId( -1 );
                TUint8 diags;
                aDataPackage->UnPackData( callId, diags );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetDiagnosticOctects( callId, diags );
                break;
                }
            case ECustomGetRemoteAlertingToneStatusIPC:
                {
                // unpack result: a TBool playRemoteAlertToneLocally
                TBool playRemoteAlertToneLocally;
                aDataPackage->UnPackData( playRemoteAlertToneLocally );
                // not completed by Custom Tsy, but by the GSM extension
                CMmCustomGsmExt* customGsmExt = 
                    static_cast<CMmCustomGsmExt*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetActiveExtensionPtr() );
                
                if ( customGsmExt )
                    {
                    customGsmExt->SetLocalRemoteAlertToneRequired(
                        playRemoteAlertToneLocally );
                    }
                break;
                }
            case ECustomClearCallBlackListIPC:                
                // no packed parameters
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteClearCallBlackList( aResult );
                break;                
            case ECustomCheckEmergencyNumberIPC:
                {
                // packed parameter: a ptr to TMobileTelNumber
                RMmCustomAPI::TMobileTelNumber* telNumber;
                aDataPackage->UnPackData( telNumber );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteCheckEmergencyNumber( telNumber, aResult );
                break;
                }
            case ECustomGetLifeTimeIPC:
                {
                // packed parameter: a RMmCustomAPI::TLifeTimeData
                RMmCustomAPI::TLifeTimeData lifeTime;
                aDataPackage->UnPackData( lifeTime );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetLifeTime( lifeTime, aResult );
                break;
                }
            case ECustomTerminateCallIPC:                
                // no packed parameters
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteTerminateCall( aResult );
                break;                
            case ECustomGetAlsBlockedIPC:
                {
                // packed parameter: TGetAlsBlockStatus (block status)
                RMmCustomAPI::TGetAlsBlockStatus blockStatus;
                aDataPackage->UnPackData( blockStatus );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetAlsBlocked( blockStatus, aResult );
                break;
                }
            case ECustomSetAlsBlockedIPC:                
                // no packed parameters
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteSetAlsBlocked( aResult );
                break;                
            case ECustomCheckAlsPpSupportIPC:
                {
                // packed parameter: RMmCustomAPI::TAlsSupport
                RMmCustomAPI::TAlsSupport alsSupport;
                aDataPackage->UnPackData( alsSupport );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetAlsPpSupport( alsSupport, aResult );
                break;
                }
            case ECustomCheckTwoDigitDialSupportIPC:
                {
                // packed parameter: RMmCustomAPI::TTwoDigitDialSupport
                RMmCustomAPI::TTwoDigitDialSupport twoDigitDialSupport;
                aDataPackage->UnPackData( twoDigitDialSupport );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteCheckTwoDigitDialSupport( twoDigitDialSupport,
                    aResult );
                break;
                }
            case ECustomGetCipheringInfoIPC:
                {
                // packed parameter:
                // TBool (ciphering indicator required/not required)
                TBool indReq;
                aDataPackage->UnPackData( indReq );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetCipheringInfo( indReq, aResult );
                break;
                }
            case ECustomNotifyCipheringInfoChangeIPC:
                {
                // packed parameters:
                // TBool (ciphering indicator required/not required)
                // TBool (ciphering on/off)
                TBool indReq;
                TBool ciStatus;
                aDataPackage->UnPackData( indReq, ciStatus );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifyCipheringInfoChange( indReq, ciStatus, 
                    aResult );
                break;
                }
            case ECustomNotifyNSPSStatusIPC:
                {
                // packed parameter: TBool nspsIsOn
                TBool nspsIsOn;
                aDataPackage->UnPackData( nspsIsOn );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifyNSPSStatus( nspsIsOn );
                break;
                }
            case ECustomNetWakeupIPC:                
                // no packed parameters
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNetWakeup( aResult );
                break;                
            case ECustomGetOperatorNameIPC:
                {
                // packed parameter: a RMmCustomAPI::TOperatorNameInfo
                RMmCustomAPI::TOperatorNameInfo operNameInfo;
                aDataPackage->UnPackData( operNameInfo );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetOperatorName( operNameInfo, aResult );
                break;
                }
            case EMobilePhoneIMSAuthenticate:                
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteImsAuthentication( aDataPackage, aResult );
                break;                
            case ECustomGetSimAuthenticationDataIPC:                
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteSimAuthentication( aDataPackage, aResult );
                break;                
            case ECustomResetNetServerIPC:                
                // no packed parameters
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteResetNetServer( aResult );
                break;                
            case ECustomNotifyNetworkConnectionFailureIPC:                
                // no packed parameters
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifyNetworkConnectionFailure();
                break;                
            case ECustomGetSystemNetworkModesIPC:
                {
                // packed parameters TUint32 (systemNetworkModes)
                TUint32 systemNetworkModes;
                aDataPackage->UnPackData( systemNetworkModes );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetSystemOrCurrentNetworkModes( 
                    systemNetworkModes, aResult );
                break;
                }
            case ECustomSetSystemNetworkModeIPC:                
                // no packed parameters)
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteSetSystemNetworkMode( aResult );
                break;                
            case ECustomGetNetworkProviderNameIPC:
                {
                // packed parameter: TDes* (network provider name)
                TDes* name;
                aDataPackage->UnPackData( name );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetNetworkOperatorName( name, aResult );
                break;
                }
            case ECustomGetCurrentSystemNetworkModesIPC:
                {
                TUint32 currentNetworkModes;
                aDataPackage->UnPackData( currentNetworkModes );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetCurrentSystemNetworkMode( 
                    currentNetworkModes, aResult );
                break;
                }
            case ECustomGetBandSelectionIPC:
                {
                // Packed parameters TBandSelection (systemNetworkBand). 
                // Initialize parameters. In otherwise compiler gives warning.
                // CompleteGetSystemNetworkBand does not handle parameters if 
                // aResult is not KErrNone.
                RMmCustomAPI::TBandSelection systemNetworkBand
                    = RMmCustomAPI::ENetworkBandAny;
                RMmCustomAPI::TNetworkModeCaps systemNetworkMode
                    = RMmCustomAPI::KCapsNetworkModeDual;
                if (KErrNone == aResult)
                	{
                aDataPackage->UnPackData( systemNetworkBand,
                    systemNetworkMode );
                	}
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetSystemNetworkBand( 
                    systemNetworkBand, systemNetworkMode, aResult );
                break;
                }
            case ECustomSetBandSelectionIPC:                
                // no packed parameters)
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteSetSystemNetworkBand( aResult );
                break;
            
            case ECustomSatRefreshCompleteNotificationIPC:                
                // no packed parameters
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteSatRefreshCompleteNotification();
                break;                
            case ECustomReadSimFileIPC:
                {
                // packed parameter: pointer to TDesC8 (SIM data)
                TDesC8* simData;
                aDataPackage->UnPackData( simData );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetSimFileInfo( simData, aResult );
                break;
                }
            case ECustomSimLockActivateIPC:
                {
                // no packed parameters
                CMmSimLockTsy* simLockTsy = static_cast<CMmSimLockTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetSimLockTsyPtr() );
                simLockTsy->CompleteActivateSimLock( aResult );
                break;
                }
            case ECustomSimLockDeActivateIPC:
                {
                // no packed parameters
                CMmSimLockTsy* simLockTsy = static_cast<CMmSimLockTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetSimLockTsyPtr() );
                simLockTsy->CompleteDeActivateSimLock( aResult );
                break;
                }
            case ECustomSendAPDUReqIPC:
                {
                CMmWimTsy* wimTsy = static_cast<CMmWimTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetWimTsyPtr() );
                wimTsy->CompleteSendAPDUReq( aDataPackage, aResult );
                break;
                }
            case ECustomSimWarmResetIPC:
                {
                // no packed parameters
                CMmWimTsy* wimTsy = static_cast<CMmWimTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetWimTsyPtr() );
                wimTsy->CompleteSimWarmReset( aResult );
                break;
                }
            case ECustomGetATRIPC:
                {
                // packed parameter: TPtrC8* atr
                TPtrC8* atr;
                aDataPackage->UnPackData( &atr );
                CMmWimTsy* wimTsy = static_cast<CMmWimTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetWimTsyPtr() );
                wimTsy->CompleteGetATR( *atr, aResult );
                break;
                }
            case ECustomGetSimCardReaderStatusIPC:
                {
                // packed parameter: TPtrC8* status
                TPtrC8* status;
                aDataPackage->UnPackData( &status );
                CMmWimTsy* wimTsy = static_cast<CMmWimTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetWimTsyPtr() );
                wimTsy->CompleteGetSimCardReaderStatus( 
                    *status, aResult );
                break;
                }
            case ECustomNotifySimCardStatusIPC:
                {
                // packed param: RMmCustomAPI::TSIMCardStatus returnStatus
                RMmCustomAPI::TSIMCardStatus returnStatus;
                aDataPackage->UnPackData( returnStatus );
                CMmWimTsy* wimTsy = static_cast<CMmWimTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetWimTsyPtr() );
                wimTsy->CompleteNotifySimCardStatus( returnStatus );
                break;
                }
            case ECustomSendAPDUReqV2IPC:
                {
                // packed parameter: TPtrC8* data
                TPtrC8* data;
                aDataPackage->UnPackData( &data );
                CMmWimTsy* wimTsy = static_cast<CMmWimTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetWimTsyPtr() );
                wimTsy->CompleteSendAPDUReqV2( *data, aResult );
                break;
                }
            case ECustomPowerSimOnIPC:
                {
                // no packed parameters
                CMmWimTsy* wimTsy = static_cast<CMmWimTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetWimTsyPtr() );
                wimTsy->CompletePowerSimOn( aResult );
                break;
                }
            case ECustomPowerSimOffIPC:
                {
                // no packed parameters
                CMmWimTsy* wimTsy = static_cast<CMmWimTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetWimTsyPtr() );
                wimTsy->CompletePowerSimOff( aResult );
                break;
                }
            case ECustomStartSimCbTopicBrowsingIPC:
                {
                // packed parameter: pointer to array with CB topics
                CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* 
                    cimCbTopics = NULL;
                if ( KErrNone == aResult )
                    {
                    // only unpack data in successful case
                    aDataPackage->UnPackData( cimCbTopics );
                    }
                CMmSIMTsy* simTsy = static_cast<CMmSIMTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetSIMTsyPtr() );
                simTsy->CompleteStartSimCbTopicBrowsing( 
                    cimCbTopics, aResult );
                break;
                }
            case ECustomDeleteSimCbTopicIPC:
                {
                // no packed parameters
                CMmSIMTsy* simTsy = static_cast<CMmSIMTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetSIMTsyPtr() );
                simTsy->CompleteDeleteSimCbTopic( aResult );
                break;
                }
                
            case ECustomGetServiceTableSupportbyApplicationIPC:
            	{
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetUSIMServiceSupport(aDataPackage, aResult );
                break;
            	
            	}
            	
            case EReadViagHomeZoneParamsIPC:
                {
                // packed parameter: RMmCustomAPI::TViagParams, pointer to 
                // RMmCustomAPI::TViagElements
                RMmCustomAPI::TViagParams viagParams;
                RMmCustomAPI::TViagElements* viagElems = NULL;
                if ( KErrNone == aResult )
                    {
                    // only unpack data in successful case
                    aDataPackage->UnPackData( viagParams, viagElems );
                    }
                CMmSIMTsy* simTsy = static_cast<CMmSIMTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                                                        GetSIMTsyPtr() );
                simTsy->CompleteReadViagHomeZoneParamsResp( &viagParams, 
                    viagElems, aResult );
                break;
                }
            case EReadViagHomeZoneCacheIPC:
                {
                // packed parameter: RMmCustomAPI::TViagCacheRecordContent
                RMmCustomAPI::TViagCacheRecordContent viagRecordContent;
                aDataPackage->UnPackData( viagRecordContent );
                CMmSIMTsy* simTsy = static_cast<CMmSIMTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetSIMTsyPtr() );
                simTsy->CompleteReadViagHomeZoneCacheRespL( 
                    &viagRecordContent, aResult );
                break;
                }
            case EWriteViagHomeZoneCacheIPC:
                {
                // no packed parameters
                CMmSIMTsy* simTsy = static_cast<CMmSIMTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetSIMTsyPtr() );
                simTsy->CompleteWriteViagHomeZoneCacheResp( aResult );
                break;
                }
            case EWriteViagHomeZoneUHZIUESettingsIPC:
                {
                // no packed parameters
                CMmSIMTsy* simTsy = static_cast<CMmSIMTsy*>(
                    static_cast<CMmCustomTsy*>( mmObject )->
                    GetSIMTsyPtr() );
                simTsy->CompleteWriteViagHomeZoneUHZIUESettingsResp( 
                    aResult );
                break;
                }
            case ECustomSetDriveModeIPC:                
                // no packed parameters
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteSetDriveMode( aResult );
                break;                
            
            // Security (Custom) functionality
            case ECustomIsBlockedIPC:
                {
                TBool isBlocked;
                aDataPackage->UnPackData ( isBlocked );
                static_cast<CMmCustomSecurityTsy*>( mmObject )->
                    CompleteIsBlocked( aResult, isBlocked );
                break;
                }
            case ECustomCheckSecurityCodeIPC:                
                static_cast<CMmCustomSecurityTsy*>( mmObject )->
                    CompleteCheckSecurityCode( aResult );
                break;                
            //case ECustomCheckEmergencyNumberIPC:
            //case ECustomSatRefreshCompleteNotificationIPC:
            case ECustomDisablePhoneLockIPC:                
                static_cast<CMmCustomSecurityTsy*>( mmObject )->
                    CompleteDisablePhoneLock( aResult);
                break;                
            case ECustomCheckSecurityCodeCancelIPC:                
                static_cast<CMmCustomSecurityTsy*>( mmObject )->
                    CompleteCheckSecurityCodeCancel();
                break;                
            case ECustomReadHSxPAStatusIPC:
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteReadHSxPAStatus( aDataPackage, aResult );
                break;
            case ECustomWriteHSxPAStatusIPC:
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteWriteHSxPAStatus( aResult );
                break;
            case ECustomNotifyHSxPAStatusIPC:
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifyHSxPAStatus( aDataPackage, aResult );
                break;
            case ECustomGetIccCallForwardingStatusIPC:
            	static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetIccCallForwardingStatus( aDataPackage, aResult );
                break;
            case ECustomNotifyIccCallForwardingStatusChangeIPC:
            	static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifyIccCallForwardingStatusChange( aDataPackage, aResult );
                break;
            case ECustomGetCellInfoIPC:
                {
                //unpack the data
                RMmCustomAPI::TMmCellInfo* cellInfo;
                aDataPackage->UnPackData( &cellInfo );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteGetCellInfo( cellInfo, aResult );
                break;
                }
            case ECustomNotifyCellInfoChangeIPC:
            	{
                //unpack the data
                RMmCustomAPI::TMmCellInfo* cellInfoChange;
                aDataPackage->UnPackData( &cellInfoChange );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifyCellInfoChange( cellInfoChange, aResult );
                break;
                }
        	case ECustomNotifyRemoteAlertingToneStatusChangeIPC:
                {
                // unpack result
                RMmCustomAPI::TRemoteAlertingToneStatus remoteAlertingToneStatus;
                aDataPackage->UnPackData( remoteAlertingToneStatus );
                static_cast<CMmCustomTsy*>( mmObject )->
                    CompleteNotifyRemoteAlertingToneStatusChange( 
                    remoteAlertingToneStatus, aResult );
                break;
                }
                
            case ECustomGetProgrammableOperatorLogoIPC:
            	{
            	RMmCustomAPI::TOperatorId* operatorId;
            	RMmCustomAPI::TOperatorLogo* operatorLogo;
            	aDataPackage->UnPackData(&operatorId, &operatorLogo);
				static_cast<CMmCustomTsy*>( mmObject )->CompleteGetProgrammableOperatorLogo(operatorId, operatorLogo, aResult);
				break;
            	}
                
                
        	// Security cases
            case EMobilePhoneGetLockInfo:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSecurityTsy()->
                    CompleteGetLockInfo( aDataPackage, aResult );
                break;                
            case EMobilePhoneChangeSecurityCode:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSecurityTsy()->
                    CompleteChangeSecurityCode( aResult );
                break;                
            case EMobilePhoneAbortSecurityCode:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSecurityTsy()->
                    CompleteAbortSecurityCode( aResult );
                break;                
            case EMobilePhoneVerifySecurityCode:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSecurityTsy()->
                    CompleteVerifySecurityCodeL( aResult );
                break;                
            case EMobilePhoneSetLockSetting:
                {
                //unpack the data
                RMobilePhone::TMobilePhoneLockStatus* status = NULL;
                RMobilePhone::TMobilePhoneLockSetting* setting = NULL;
                aDataPackage->UnPackData( &status, &setting );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSecurityTsy()->
                    CompleteSetLockSetting( aResult,*status, *setting );
                break;
                }
            case EMobilePhoneNotifySecurityEvent:
                {
                RMobilePhone::TMobilePhoneSecurityEvent* event = NULL;
                //unpack the data
                aDataPackage->UnPackData( &event );
                static_cast<CMmPhoneTsy*>( mmObject )->GetSecurityTsy()->
                    CompleteNotifySecurityEventL( *event, aResult );
                break;
                }
            case EMobilePhoneNotifyLockInfoChange:                
                static_cast<CMmPhoneTsy*>( mmObject )->GetSecurityTsy()->
                    CompleteNotifyLockInfoChange( aDataPackage, aResult);
                break;                
            case EMmTsySecurityGetSimActivePinStateIPC:                
                static_cast<CMmCustomSecurityTsy*>(
                    mmObject )->SetActivePin( aDataPackage );
                break;                
            case EMmTsySimGetICCType:                
                static_cast<CMmPhoneTsy*>( mmObject )->
                  PhoneExtInterface()->CompleteGetICCType( aDataPackage );
                break;                
            case EMmTsyBootNotifySimStatusReadyIPC: 
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteNotifySimStatusReadyL();
                break;                
            case EMmTsyBootNotifyModemStatusReadyIPC:                
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteNotifyModemStatusReady();
                break;                
            case EMmTsyBootNotifySecurityReadyIPC:                
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteNotifySecurityReady();
                break;   
            case EMobilePhoneNotifyIccAccessCapsChange:                
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteNotifyIccAccessCapsChange();
                break;              
            case EMmTsyBootGetRFStatusIPC:
                {
                TRfStateInfo* rfInfo = NULL;
                aDataPackage->UnPackData ( &rfInfo );
                CMmPhoneTsy* phoneTsy = 
                                  static_cast<CMmPhoneTsy*>( mmObject );
                phoneTsy->SetRfStateInfo( *rfInfo );
                // inform also SMS Tsy about RF state
                CMmSmsTsy* smsTsy = phoneTsy->SmsSession();
                if ( smsTsy )
                    {
                    if ( ERfsStateInfoInactive == *rfInfo )
                        {
                        smsTsy->SetOffline( ETrue );
                        }
                    else
                        {
                        smsTsy->SetOffline( EFalse );
                        }
                    }
                break;
                }
            case EMobilePhoneGetMailboxNumbers:
            	iMmPhone->CompleteGetMailboxNumbers( aDataPackage, aResult );
                break;            
            // Miscellaneous Phone functionality

            // case EMobilePhoneGetBatteryInfo: // handled in CommonTSY
            case EMobilePhoneNotifyBatteryInfoChange:
                static_cast<CMmPhoneTsy*>( mmObject )->
                  CompleteNotifyBatteryInfoChange(aDataPackage, aResult );
                //  phone indicator status may change
                iMmPhone->CompleteNotifyIndicatorChange();
                break;

            case EMobilePhoneGetPhoneId:
                {
                // packed parameter: TDes8 with serial number
                TBuf8<KSerialNumberLength> serialNumber;
                aDataPackage->UnPackData( serialNumber );
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetPhoneId( serialNumber, aResult );
                break;
                }

#ifdef USING_CTSY_DISPATCHER               
            case ECtsyGetPhoneIdV2Comp: 
                {
                // Ltsy returns packed parameter of RMobilePhone::TMobilePhoneIdentityV1
                RMobilePhone::TMobilePhoneIdentityV1* phoneId = NULL;
                aDataPackage->UnPackData(&phoneId);
                __ASSERT_DEBUG(phoneId, User::Invariant());

                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetPhoneId(*phoneId, aResult);
                break;
                }
#endif //USING_CTSY_DISPATCHER
             
            case EMobilePhoneGetSubscriberId:
                {
                // packed param: TBuf8<RMobilePhone::KIMSISize> with IMSI
                TBuf8<RMobilePhone::KIMSISize> imsi;
                aDataPackage->UnPackData( imsi );
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetSubscriberIdL( imsi, aResult );
                break;
                }
            case EMobilePhoneGetALSLine:
                {
                // packed parameter: RMobilePhone::TMobilePhoneALSLine
                RMobilePhone::TMobilePhoneALSLine alsStatus;
                aDataPackage->UnPackData ( alsStatus );

                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetAlsLine ( alsStatus, aResult );
                break;
                }
            case EMobilePhoneSetALSLine:                
                // no packed parameters
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteSetAlsLine ( aResult );
                break;                
            case EMobilePhoneGetCustomerServiceProfile:
                {
                // packed parameter: RMobilePhone::TMobilePhoneCspFileV1
                RMobilePhone::TMobilePhoneCspFileV1 cspFile;
                aDataPackage->UnPackData( cspFile );
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetCustomerServiceProfile( cspFile, aResult );
                break;
                }
            case EMobilePhoneGetPhoneStoreInfo:
                {
                //packed param: RMobilePhoneStore::TMobilePhoneStoreInfoV1
                RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
                aDataPackage->UnPackData( phoneStoreInfo );
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetPhoneStoreInfo( phoneStoreInfo, aResult );
                break;
                }
            case EMobilePhoneGetServiceTable:
                {
                //packed param: RMobilePhone::TMobilePhoneServiceTableV1
                RMobilePhone::TMobilePhoneServiceTableV1 table;
                aDataPackage->UnPackData( table );
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetServiceTableL ( table, aResult );
                break;
                }
            case EMmTsyPhoneGetPin1DisableSupportedIPC:
                {
                // packed parameter: TBool (supported or not)
                TBool supported = EFalse;
                aDataPackage->UnPackData( supported );
                if ( !supported )
                    {
                    static_cast<CMmPhoneTsy*>( mmObject )->
                          GetSecurityTsy()->SetPin1DisableNotSupported();
                    }
                break;
                }
            case EMobilePhoneSetFdnSetting :                
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteSetFdnSettingL( aResult );
                break;                
            case EMobilePhoneGetFdnStatus:                
                static_cast<CMmPhoneTsy*>( mmObject )-> 
                    CompleteGetFdnStatus( aDataPackage, aResult );
                break;                
            case EMobilePhoneGetServiceProviderName:                
                static_cast<CMmPhoneTsy*>( mmObject )->
                  CompleteGetServiceProviderName( aDataPackage, aResult );
                break;                
            case EMmTsySimRefreshNowIPC:
                {
                //packed param: TUint16(list of SIM files to be refreshed)
                TUint16 filesToRefresh = 0;
                aDataPackage->UnPackData( filesToRefresh );
                // Refresh notification received, start refresh
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteSimRefreshNowL( filesToRefresh );
                break;
                }
            case EMmTsyGetCallForwardingNumberIPC:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetCallForwardingNumber( 
                    aResult, aDataPackage );
                break;

            // Access point control list (ACL)
            case EMobilePhoneEnumerateAPNEntries:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteEnumerateAPNEntries( aDataPackage, aResult );
                break;
            case EMobilePhoneGetAPNname:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetAPNname( aDataPackage, aResult );
                break;
            case EMobilePhoneAppendAPNName:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteAppendAPNName( aResult );
                break;
            case EMobilePhoneDeleteAPNName:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteDeleteAPNName( aResult );
                break;
            case EMobilePhoneSetAPNControlListServiceStatus:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteSetAPNControlListServiceStatus( aResult );
                break;
            case EMobilePhoneGetAPNControlListServiceStatus:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteGetAPNControlListServiceStatus( aDataPackage, 
                    aResult );
                break;
            case EMobilePhoneNotifyAPNListChanged:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteNotifyAPNListChanged( aResult );
                break;
            case EMobilePhoneNotifyAPNControlListServiceStatusChange:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    CompleteNotifyAPNControlListServiceStatusChange( 
                    aDataPackage, aResult );
                break;
            case EMobilePhoneGetSecurityCodeInfo:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    GetSecurityTsy()->CompleteGetSecurityCodeInfo( 
                    aDataPackage, aResult );
                break;
            case EMobilePhoneNotifySecurityCodeInfoChange:
                static_cast<CMmPhoneTsy*>( mmObject )->
                    GetSecurityTsy()->CompleteNotifySecurityCodeInfoChange( 
                    aDataPackage, aResult );
                break;
            case EMmTsyPhoneBookStoreFdnInfoIPC:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_6, "TSY: CMmMessageRouterProxy::RouteCompletion: route to phonetsy EMmTsyPhoneBookStoreFdnInfoIPC");
                static_cast<CMmPhoneTsy*>( mmObject )->
                    SaveFdnInfoDetails( aResult, aDataPackage );
                break;
            default:
                break;
            }

        }
    else //special cases goes here
        {
        switch ( aIpc )
            {
            //this IPC has to be routed to
            //the NetTsy object as is an special case. 
            //We Force the routing.
            case EMobileCallNotifyPrivacyConfirmation:
                {
                mmObject =iTsyDelegates-> 
                      GetTsyObject(  CMmMessageManagerBase::EPhoneTsy );
                static_cast<CMmPhoneTsy*>( mmObject )->GetNetTsy()->
                    ReceivedCallPrivacyInformation( aDataPackage );
                break;
                }
            case EMobileCallGetMobileCallInfo:
                {
                // If call object is not found, this call object has just 
                // been initiated. Inform Line object instead.
                // (Get mobile call information is always completed before 
                // call status change)
                TInt callId( -1 );
                RMobilePhone::TMobileService callMode( 
                    RMobilePhone::EVoiceService );

                const CCallDataPackage* callData = 
                  reinterpret_cast<const CCallDataPackage*>(aDataPackage);
                callData->GetCallIdAndMode( callId, callMode );

                mmObject = REINTERPRET_CAST( CBase*, 
                      iMmPhone->LineList()->GetMmLineByMode( callMode ) );
                
                if ( NULL == mmObject )
                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_7, "TSY: CMmMessageRouterProxy::RouteCompletion: Call object not found for mobile Call Info!");
                    }
                else
                    {
                    // complete call added notification directly from here
                    static_cast<CMmLineTsy*>( mmObject )->
                        CompleteNotifyDiallingStatus( aDataPackage );
                    }
                break;
                }
            case EEtelCallDial:
            case EMobileCallDialISV:
            case EMobileCallDialNoFdnCheck:
                {
                //If data/video call creation fails (e.g when dialled 
                //number has not been in the FDN list), dial is completed 
                //when this call object has just been initiated 
                //(and therefore not found).
                TInt callId( -1 );
                RMobilePhone::TMobileService callMode( 
                    RMobilePhone::EServiceUnspecified );

                const CCallDataPackage* callData = 
                reinterpret_cast<const CCallDataPackage*>( aDataPackage );
                callData->GetCallIdAndMode( callId, callMode );

                if ( callId == 0 && ( 
                    callMode == RMobilePhone::ECircuitDataService || 
                    callMode == RMobilePhone::EVoiceService || 
                    callMode == RMobilePhone::EAuxVoiceService ) )
                    {
                    //check if data call exists
                    CMmDataCallTsy* mmCall = 
                      reinterpret_cast<CMmDataCallTsy*>(
                        iMmPhone->CallList()->
                            GetMmCallByMode( callMode ) );

                    if ( NULL == mmCall )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_8, "TSY: CMmMessageRouterProxy::RouteCompletion: Call object not found for Dial!");
                        }
                    else
                        {
                        //find object that requested dial
                        if ( ( mmCall->IsServiceLocallyRequested( 
                            CMmCallTsy::EMultimodeCallDial ) )
                            || ( mmCall->IsServiceLocallyRequested( 
                            CMmCallTsy::EMultimodeCallDialISV ) ) 
                            || (mmCall->IsServiceLocallyRequested( 
                            CMmCallTsy::EMultimodeMobileCallDialEmergencyCall ) ) 
                            || ( mmCall->IsServiceLocallyRequested( 
                            CMmCallTsy::
                            EMultimodeCallDialNoFdnCheck ) ) )
                            {
                            for( TInt i = 0; i < iMmPhone->CallList()->
                                GetNumberOfObjects(); i++ )
                                {
                                mmCall = reinterpret_cast<CMmDataCallTsy*>(
                                    iMmPhone->CallList()->
                                                    GetMmCallByIndex(i) );
                                                    
                                if ( ( mmCall->ServiceRequested( 
                                    CMmCallTsy::EMultimodeCallDial ) 
                                      && mmCall->CallMode() == callMode )
                                      || ( mmCall->ServiceRequested( 
                                      CMmCallTsy::EMultimodeCallDialISV ) 
                                    && mmCall->CallMode() == callMode ) )
                                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_9, "TSY: CMmMessageRouterProxy::RouteCompletion: Special case before CompleteDial");
                                    mmCall->CompleteDial( aResult );
                                    break;
                                    }
                                    
                                if  ( mmCall->ServiceRequested( 
                                    CMmCallTsy::EMultimodeCallDialNoFdnCheck ) 
                                    && mmCall->CallMode() == callMode )
                                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMMESSAGEROUTERPROXY_ROUTECOMPLETIONL_10, "TSY: CMmMessageRouterProxy::RouteCompletion: Special case before CompleteDialNoFdn");
                                    mmCall->CompleteDialNoFdn( aResult );
                                    break;
                                    }
                                }
                            }
                        }
                    }
                break;
                }
            default:
                break;
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CMmMessageRouterProxy::RegisterTsyObject
// Registers a Tsy object for request and notification completions
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//  
void CMmMessageRouterProxy::SetPhoneTsy(
    CMmPhoneTsy* aMmPhone )
    {
    iMmPhone = aMmPhone;
    }

//  End of File
