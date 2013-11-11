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



// INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsupplservtsyTraces.h"
#endif

#include "cmmsupplservtsy.h"

// include internal Tsy classes
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/ctsydomaincrkeys.h>
#include <ctsy/rmmcustomapi.h>
#include "ctsyprivatecrpkeys.h"
#include <ctsy/serviceapi/gsmerror.h>

// ======== MEMBER FUNCTIONS ========

CMmSupplServTsy::CMmSupplServTsy()
    {
    }

void CMmSupplServTsy::ConstructL()
    {
    // Initialise SS capabilities
    iCallServiceCaps = KMultimodeTsyGsmCallServiceCaps;

    // Initialise Call Forwarding member variables
    iRetCFStatusSize = NULL;
    iRetCallForwardingCondition = NULL;
    iRetServiceGroup = NULL;
    iRetActiveType = NULL;
    iCFList = NULL;
    iCFClientId = NULL;
    iCBClientId = NULL;
    iCWClientId = NULL;
    iMwisCentRep = iMmPhone->GetMwisCenRep();
    iCfisCentRep = iMmPhone->GetCfisCenRep();
    iCFISPrivateCentRep = iMmPhone->GetCfisPRCenRep();
    iGetIccSupportAtBoot = ETrue;
    iMessageWaitingSupportedByIcc = ETrue; 
    // Initialize flag  
    iSsNoFdnCheckFlag = ESsNoFdnCheckUnknown; 
    }


CMmSupplServTsy* CMmSupplServTsy::NewL(
    CMmPhoneTsy* aPhoneTsy ) // Pointer to PhoneTsy
    {
    CMmSupplServTsy* self = new( ELeave ) CMmSupplServTsy();

    CleanupStack::PushL( self );    
    self->iMmPhone = aPhoneTsy;
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    

CMmSupplServTsy::~CMmSupplServTsy()
    {
    iMmPhone = NULL;
    iRetCallServiceCaps = NULL;
    iRetCFStatusSize = NULL;
    iRetCallForwardingCondition = NULL;
    iRetServiceGroup = NULL;
    iRetActiveType = NULL;
    iCFList = NULL;
    
    if ( iCFClientId )
        {
    	delete iCFClientId;
        iCFClientId = NULL;
        }
        
    iRetCBStatusSize = NULL;
    iRetCallBarringCondition = NULL;
    iCBList = NULL;
    
    if ( iCBClientId )
        {
        delete iCBClientId;
        iCBClientId = NULL;
        }
        
    iRetCWStatusSize = NULL;
    iRetNotifyCallWaitingStatus = NULL;
    iCWList = NULL;
    
    if ( iCWClientId )
        {
        delete iCWClientId;
        iCWClientId = NULL;	
        }
        
    iRetGetIdentityServiceStatus = NULL;
    iMwisCentRep = NULL;
    iRetSetMessageWaiting = NULL;
    iRetNotifyMessageWaiting = NULL; 
    iMwisInfoRequests.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::DoExtFuncL
// Supplementary Services-specific functionality of CMmPhoneTsy::DoExtFuncL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::DoExtFuncL( 
  	const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage ) 
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_DOEXTFUNCL_1, "TSY: CMmSupplServTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);
    TInt ret = KErrNone;
    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();

    switch ( aIpc )
        {
        // supported SS features
      	// Call Service Capabilities
        // Get Call Service Capabilities
        case EMobilePhoneGetCallServiceCaps:
            ret = GetCallServiceCaps( aTsyReqHandle, 
            reinterpret_cast<TUint32*>( dataPtr ) );
            break;
        // Notify Change of Call Service Capabilities
        case EMobilePhoneNotifyCallServiceCapsChange:
            ret = NotifyCallServiceCapsChange(  
            reinterpret_cast<TUint32*>( dataPtr ) );
            break;
      	//Call Forwarding
        //Get Call Forwarding Status and Information - Phase1
        case EMobilePhoneGetCallForwardingStatusPhase1:
            ret = GetCallForwardingStatusPhase1L( aTsyReqHandle, 
            reinterpret_cast<CRetrieveMobilePhoneCFList::TGetCallForwardingRequest*>
                ( dataPtr ), 
            reinterpret_cast< TInt*>( dataPtr2 ) );
            break;
        //Get Call Forwarding Status and Information - Phase2
        case EMobilePhoneGetCallForwardingStatusPhase2:
            ret = GetCallForwardingStatusPhase2( aTsyReqHandle ,
            reinterpret_cast<RMobilePhone::TClientId*>( dataPtr ), 
            aPackage.Des2n() );
            break;
        // Set Call Forwarding Status and Information
        case EMobilePhoneSetCallForwardingStatus:
            ret = SetCallForwardingStatusL( aTsyReqHandle, 
            reinterpret_cast<RMobilePhone::TMobilePhoneCFCondition*>
                ( dataPtr ),
            reinterpret_cast<const RMobilePhone::TMobilePhoneCFChangeV1*>
                ( dataPtr2 ) );
            break;
        // Notify Change of Call Forwarding Status or Information
        case EMobilePhoneNotifyCallForwardingStatusChange:
            ret = NotifyCallForwardingStatusChange(
            reinterpret_cast<RMobilePhone::TMobilePhoneCFCondition*>
                ( dataPtr ) );
            break;
        // Call Forwarding Active Notification
        case EMobilePhoneNotifyCallForwardingActive:
            ret = NotifyCallForwardingActive(
            reinterpret_cast<RMobilePhone::TMobileService*>( dataPtr ),
            reinterpret_cast<RMobilePhone::TMobilePhoneCFActive*>
                ( dataPtr2 ) );
            break;

        //Call Barring
        // Get Call Barring Status and Information - Phase1
        case EMobilePhoneGetBarringStatusPhase1:
            ret = GetCallBarringStatusPhase1L( aTsyReqHandle, 
            reinterpret_cast<CRetrieveMobilePhoneCBList::TGetCallBarringRequest*>
                ( dataPtr ),
            reinterpret_cast<TInt*>( dataPtr2 ) );
            break;            
        // Get Call Barring Status and Information - Phase2
        case EMobilePhoneGetBarringStatusPhase2:
            ret = GetCallBarringStatusPhase2( aTsyReqHandle, 
            reinterpret_cast<RMobilePhone::TClientId*>( dataPtr ),
            aPackage.Des2n() );
            break;
        // Set Call Barring Status and Information
        case EMobilePhoneSetCallBarringStatus:
            ret = SetCallBarringStatusL( aTsyReqHandle, 
            reinterpret_cast<RMobilePhone::TMobilePhoneCBCondition*>
                ( dataPtr ),
            reinterpret_cast<const RMobilePhone::TMobilePhoneCBChangeV1*>
                ( dataPtr2 ) );
            break;
        // Notify Change of Call Barring Status or Information
        case EMobilePhoneNotifyCallBarringStatusChange:
            ret = NotifyCallBarringStatusChange( 
            reinterpret_cast<RMobilePhone::TMobilePhoneCBCondition*>
                ( dataPtr ) );
            break;

        // Supplementary Services password change
        // Set SS password
        case EMobilePhoneSetSSPassword:
            ret = SetSSPasswordL( aTsyReqHandle, aPackage.Des1n(),
            reinterpret_cast<TInt*>( dataPtr2 ) );
            break;

      	// Call Waiting
        // Get Call Waiting Status - Phase1
        case EMobilePhoneGetWaitingStatusPhase1:
            ret = GetCallWaitingStatusPhase1L( aTsyReqHandle, 
            reinterpret_cast<CRetrieveMobilePhoneCWList::TGetCallWaitingRequest*>
                ( dataPtr ),
            reinterpret_cast<TInt*>( dataPtr2 ) );
            break;
 
        // Get Call Waiting Status - Phase2
        case EMobilePhoneGetWaitingStatusPhase2:
            ret = GetCallWaitingStatusPhase2( aTsyReqHandle, 
            reinterpret_cast<RMobilePhone::TClientId*>( dataPtr ),
            aPackage.Des2n() );
            break;

        // Set Call Waiting Status
        case EMobilePhoneSetCallWaitingStatus:
            ret = SetCallWaitingStatusL( aTsyReqHandle, 
            reinterpret_cast<RMobilePhone::TMobileService*>( dataPtr ),
            reinterpret_cast<RMobilePhone::TMobilePhoneServiceAction*>
                ( dataPtr2 ) );
            break;

        // Notify Change of Call Waiting Status
        case EMobilePhoneNotifyCallWaitingStatusChange:
            ret = NotifyCallWaitingStatusChange( aPackage.Des1n() );
            break;

      	// Identity Services
        // Get Status of an Identity Service 
        case EMobilePhoneGetIdentityServiceStatus:
            ret = GetIdentityServiceStatusL( aTsyReqHandle, 
            reinterpret_cast<RMobilePhone::TIdServiceAndLocation*>( dataPtr ),
            reinterpret_cast<RMobilePhone::TMobilePhoneIdServiceStatus*>
                ( dataPtr2 ) );
            break;

        // User's Access to Network Services
        case EMobilePhoneSendNetworkServiceRequest:
            {
            //Set flag value
            iSsNoFdnCheckFlag = ESsNoFdnCheckNotUsed;
            ret = SendNetworkServiceRequestL( 
                      aTsyReqHandle, aPackage.Des1u() );
            }
            break;

        // User's Access to Network Services with NoFdnCheck
        case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
            {
            //Set flag value
            iSsNoFdnCheckFlag = ESsNoFdnCheckUsed;
            ret = SendNetworkServiceRequestL( 
                      aTsyReqHandle, aPackage.Des1u() );                      
            }
            break;                    

        // Notify for Network Service Requests         
        case EMobilePhoneNotifySendNetworkServiceRequest:
            ret = NotifySendNetworkServiceRequest(
            		reinterpret_cast<const RMobilePhone::TMobilePhoneNotifySendSSOperation*>
            			( dataPtr ), aPackage.Des2n() );
            break;                    
          
        // Notify for All Network Service Requests         
        case EMobilePhoneNotifyAllSendNetworkServiceRequest:
            ret = NotifyAllSendNetworkServiceRequest(
             reinterpret_cast<RMobilePhone::TMobilePhoneNotifySendSSOperation*>
                ( dataPtr ), aPackage.Des2n() );
            break;                    
         
        // Get ICC message waiting indicators
        case EMobilePhoneGetIccMessageWaitingIndicators:
            ret = GetIccMessageWaitingIndicatorsL( aTsyReqHandle,
                REINTERPRET_CAST( 
                RMobilePhone::TMobilePhoneMessageWaitingV1*, dataPtr ) );
            break;
        
        // Set ICC message waiting indicators    
        case EMobilePhoneSetIccMessageWaitingIndicators:
            ret = SetIccMessageWaitingIndicatorsL( aTsyReqHandle,
                    aPackage.Des1n() );
            break;
        
        // Notify ICC message waiting indicators change
        case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
            ret = NotifyIccMessageWaitingIndicatorsChange( 
                REINTERPRET_CAST( 
                RMobilePhone::TMobilePhoneMessageWaitingV1*, dataPtr ) );
            break;       

        // not supported SS features
        case EMobilePhoneGetCallForwardingIndicator:
        case EMobilePhoneSetCallBarringPassword:
        case EMobilePhoneGetCompMethodName:
            ret = KErrNotSupported;
            break;

        // error case
        default:
            // this method should only be called for SS cases
            ret = KErrArgument;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_DOEXTFUNCL_2, "TSY: CMmSupplServTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CancelService
// Cancels SS requests.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::CancelService(    
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle ) 
    {
    TInt ret ( KErrNone );
    switch ( aIpc )
        {
        // Notifications
        case EMobilePhoneNotifyCallServiceCapsChange:
            ret = NotifyCallServiceCapsChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyCallForwardingStatusChange:
            ret = NotifyCallForwardingStatusChangeCancel( aTsyReqHandle );        
            break;
        case EMobilePhoneNotifyCallForwardingActive:
            ret = NotifyCallForwardingActiveCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyCallBarringStatusChange:
            ret = NotifyCallBarringStatusChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyCallWaitingStatusChange:
            ret = NotifyCallWaitingStatusChangeCancel( aTsyReqHandle );
            break;
        // Other requests
        case EMobilePhoneGetCallForwardingStatusPhase1:
        case EMobilePhoneGetCallForwardingStatusPhase2:
            ret = GetCallForwardingStatusCancel( aTsyReqHandle );   
            break;
        case EMobilePhoneSetCallForwardingStatus:
            ret = SetCallForwardingStatusCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetBarringStatusPhase1:
        case EMobilePhoneGetBarringStatusPhase2:
            ret = GetCallBarringStatusCancel( aTsyReqHandle );
            break;
        case EMobilePhoneSetCallBarringStatus:
            ret = SetCallBarringStatusCancel( aTsyReqHandle );
            break;
        case EMobilePhoneSetSSPassword:
            ret = SetSSPasswordCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetWaitingStatusPhase1:
        case EMobilePhoneGetWaitingStatusPhase2:
            ret = GetCallWaitingStatusCancel( aTsyReqHandle );
            break;
        case EMobilePhoneSetCallWaitingStatus:
            ret = SetCallWaitingStatusCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetIdentityServiceStatus:
            ret = GetIdentityServiceStatusCancel( aTsyReqHandle );
            break; 
        case EMobilePhoneSendNetworkServiceRequest: 
        case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
        // Ignore
            break;
        case EMobilePhoneGetIccMessageWaitingIndicators:
            ret = GetIccMessageWaitingIndicatorsCancel( aTsyReqHandle );
            break;
        case EMobilePhoneSetIccMessageWaitingIndicators:
            ret = SetIccMessageWaitingIndicatorsCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
            ret = NotifyIccMessageWaitingIndicatorChangeCancel( aTsyReqHandle );
            break; 
        case EMobilePhoneNotifySendNetworkServiceRequest:
        	ret = NotifySendNetworkServiceRequestCancel( aTsyReqHandle );
        	break;
        case EMobilePhoneNotifyAllSendNetworkServiceRequest:
            ret = NotifyAllSendNetworkServiceRequestCancel( aTsyReqHandle );
            break;  

        // Error case
        default:
            // This method should only be called for SS cases
            ret = KErrArgument; 
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::Complete
// Completes the request due timer expiration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::Complete(
    TInt aReqHandleType, 
    TInt aError ) 
    {
    switch( aReqHandleType )
        {
        case CMmPhoneTsy::EMultimodePhoneGetCallForwardingStatusPhase1:
            TRAP_IGNORE( CompleteGetCallForwardingStatusPhase1L( 
            	NULL, aError ););
            break;
        case CMmPhoneTsy::EMultimodePhoneSetCallForwardingStatus:
            CompleteSetCallForwardingStatus( aError );
            break;
        case CMmPhoneTsy::EMultimodePhoneGetCallBarringStatusPhase1:
            TRAP_IGNORE( CompleteGetCallBarringStatusPhase1L( 
            	NULL, aError ););
            break;
        case CMmPhoneTsy::EMultimodePhoneSetCallBarringStatus:
            CompleteSetCallBarringStatus( aError );
            break;
        case CMmPhoneTsy::EMultimodePhoneSetSSPassword: 
            CompleteSetSSPassword( aError );
            break;
        case CMmPhoneTsy::EMultimodePhoneGetCallWaitingStatusPhase1:
            TRAP_IGNORE( CompleteGetCallWaitingStatusPhase1L( 
            	NULL, aError ););
            break;
        case CMmPhoneTsy::EMultimodePhoneSetCallWaitingStatus:
            CompleteSetCallWaitingStatus( aError );
            break;
        case CMmPhoneTsy::EMultimodePhoneGetIdentityServiceStatus: 
            CompleteGetIdentityServiceStatus( 
                RMobilePhone::EIdServiceUnknown, aError );
            break;
        case CMmPhoneTsy::EMultimodePhoneSendNetworkServiceRequest:
            CompleteSendNetworkServiceRequest( aError );
            break;  
        case CMmPhoneTsy::EMultimodePhoneSendNetworkServiceRequestNoFdnCheck:
            CompleteSendNetworkServiceRequestNoFdnCheck( aError );
            break;                                
        case CMmPhoneTsy::EMultimodePhoneGetIccMessageWaitingIndicators:
            CompleteGetIccMessageWaitingIndicators( NULL, aError );
            break;
        case CMmPhoneTsy::EMultimodePhoneSetIccMessageWaitingIndicators:
            CompleteSetIccMessageWaitingIndicators( aError );
            break;
        default:
            // Ignore
            break;
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallServiceCaps
// Get Call Service Capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallServiceCaps(
    const TTsyReqHandle aTsyReqHandle, 
    TUint32* aCaps ) 
    {
    *aCaps = iCallServiceCaps;
    // Complete the request using the return value from extension
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallServiceCapsChange
// Notify Change of Call Service Capabilities.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallServiceCapsChange(
    TUint32* aCaps ) 
    {

    iRetCallServiceCaps = aCaps;
    iMmPhone->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyCallServiceCapsChange;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallServiceCapsChangeCancel
// Use this method to cancel a previously placed asynchronous 
// NotifyCallServiceCapsChange request.
// (other items were commented in a header). 
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallServiceCapsChangeCancel(
    TTsyReqHandle aTsyReqHandle )
    {
    //set pointer to NULL
    iRetCallServiceCaps = NULL;

    // Reset request handle and pointer
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallServiceCapsChange );

    // Complete the request and return KErrNone
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteNotifyCallServiceCapsChange
// Description: This method completes an outstanding 
// NotifyCallServiceCapsChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteNotifyCallServiceCapsChange(
		RMobilePhone::TMobilePhoneCallServiceCaps aCallServiceCaps ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallServiceCapsChange );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        *iRetCallServiceCaps = aCallServiceCaps;
        iRetCallServiceCaps = NULL; 
        iMmPhone->ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallForwardingStatusPhase1L
// First phase of fetching call forwarding status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallForwardingStatusPhase1L(
    const TTsyReqHandle aTsyReqHandle,  // Tsy request handle
    CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, 
    TInt* aBufSize ) // Buffer size
    {
    TInt ret = KErrNone;
    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    // Check some error cases first
    else if ( RMobilePhone::EInfoLocationCache == aReqData->iLocation )
        {
        // No cache implemented
        ret = KErrNotFound;
        }
    else if ( ( RMobilePhone::ECallForwardingAllCases == aReqData->iCondition ) 
              || ( RMobilePhone::ECallForwardingAllConditionalCases == 
                                                      aReqData->iCondition ) ) 
        {
        // The combined cases cannot be used for requesting the status,
        // only for activation and deactivation
        ret = KErrArgument;
        }
    else
        {
        // Forward the request to DOS
        CMmDataPackage package;
        package.PackData( &aReqData->iCondition, &aReqData->iServiceGroup );
        ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneGetCallForwardingStatusPhase1, &package );
        iSsTransactionOngoing = ETrue;
        }
    if ( ret != KErrNone )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        }
    else
        {
        // Save return pointer for list size
        iRetCFStatusSize = aBufSize;

        // Copy client id (session and subsession handle). it's used for
        // matching phase 1 and 2 of a request
        
        if ( iCFClientId )
            {
            delete iCFClientId;
            iCFClientId = NULL;
            }
            
        iCFClientId = new( ELeave ) RMobilePhone::TClientId( 
                                        aReqData->iClient );

        // Save req handle type
        iMmPhone->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneGetCallForwardingStatusPhase1;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallForwardingStatusPhase2
// Second phase of fetching call forwarding status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallForwardingStatusPhase2(
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TClientId const * aClient, 
    TDes8* aBuf ) 
    {
    TInt ret = KErrNone;
    // Check if client id matches between phase 1 and phase 2
    if ( ( iCFClientId->iSessionHandle == aClient->iSessionHandle ) &&
         ( iCFClientId->iSubSessionHandle == aClient->iSubSessionHandle ) )
        {
        // Copy the streamed list to the client
        aBuf->Copy( iCFList->Ptr( 0 ) );

        // Reset the client Id
        delete iCFClientId;
        iCFClientId = NULL;

        // Reset the list
        delete iCFList;
        iCFList = NULL;
        }
    else
        {
        ret = KErrNotFound;
        }

    iMmPhone->ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallForwardingStatusCancel
// This method cancels an outstanding GetCallForwardingStatus 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallForwardingStatusCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetCFStatusSize = NULL;
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneGetCallForwardingStatusPhase1 );
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
    iSsTransactionOngoing = EFalse;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteGetCallForwardingStatusPhase1L
// This method completes an outstanding GetCallForwardingStatusPhase1 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteGetCallForwardingStatusPhase1L(
    CMobilePhoneCFList* aResults, 
    TInt aErrorCode ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneGetCallForwardingStatusPhase1 );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            // If there's no error, we should have got some results ...
            if ( aResults && aResults->Enumerate() > 0 )
                {
                // Store the streamed list for phase 2
                iCFList = aResults->StoreLC();          
                CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC
                }
            if ( iCFList ) 
                {
                // Copy the size of the streamed list to client
                *iRetCFStatusSize = iCFList->Size();
                }
            else
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETEGETCALLFORWARDINGSTATUSPHASE1L_1, "TSY: CMmSupplServTsy::CompleteGetCallForwardingStatusPhase1L: No response received.");
                aErrorCode = KErrNotFound;
                }
            }
            
        iRetCFStatusSize = NULL;
        iMmPhone->ReqCompleted( reqHandle, aErrorCode );       
        iSsTransactionOngoing = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallForwardingStatusChange
// This request allows a client to be notified when the status 
// or associated information of any of the call forwarding conditions 
// applied to any of the basic service groups changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallForwardingStatusChange(
    RMobilePhone::TMobilePhoneCFCondition* aCondition ) 
    {
    iRetCallForwardingCondition = aCondition;
    // Save req handle type
    iMmPhone->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyCallForwardingStatusChange;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallForwardingStatusChangeCancel
// This method cancels an outstanding 
// NotifyCallForwardingStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallForwardingStatusChangeCancel(
     const TTsyReqHandle aTsyReqHandle ) // Tsy request handle
    {
    iRetCallForwardingCondition = NULL;
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallForwardingStatusChange );
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteNotifyCallForwardingStatusChange
// Description: This method completes an outstanding 
// NotifyCallForwardingStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteNotifyCallForwardingStatusChange(
    RMobilePhone::TMobilePhoneCFCondition aCondition ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallForwardingStatusChange );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        *iRetCallForwardingCondition = aCondition;
        iRetCallForwardingCondition = NULL; 
        iMmPhone->ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetCallForwardingStatusL
// This method sets the call forwarding status/information for 
// the call forwarding service specified by the aCondition parameter.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetCallForwardingStatusL(
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TMobilePhoneCFCondition* aCondition, 
    const RMobilePhone::TMobilePhoneCFChangeV1* aInfo ) 
    {
    TInt ret = KErrNone;
    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    else if ( ( RMobilePhone::EServiceActionActivate != aInfo->iAction ) &&
        ( RMobilePhone::EServiceActionDeactivate != aInfo->iAction ) &&
        ( RMobilePhone::EServiceActionRegister != aInfo->iAction ) &&
        ( RMobilePhone::EServiceActionErase != aInfo->iAction ) )
        {
        // Status can be set only by above actions
        ret = KErrArgument;
        }
    else
        {
        iCondition = aCondition;
        iInfo = aInfo;
        
        // Forward the request to DOS
        CMmDataPackage package;
        package.PackData( aCondition, 
            const_cast<RMobilePhone::TMobilePhoneCFChangeV1**>( &aInfo ) );
        ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneSetCallForwardingStatus, &package );
        iSsTransactionOngoing = ETrue;
        }
    if ( ret != KErrNone )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        }
    else
        {
        // Save req handle type
        iMmPhone->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneSetCallForwardingStatus;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteSetCallForwardingStatus
// This method completes an outstanding SetCallForwardingStatus 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteSetCallForwardingStatus(
    TInt aErrorCode ) // error code
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneSetCallForwardingStatus );
	
    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
		{ 	
	 	//Update Central Repository
	    if( KErrNone == aErrorCode )
	    	{
	    	iCFISPrivateCentRep->Set( KCtsyCallForwardingSupported, ETrue );	
	    	}
	    
	    iCfisCentRep->Set( KCtsyCallForwardingPhoneCFCondition, 
	        *iCondition );
	    iCfisCentRep->Set( KCtsyCallForwardingStatus, 
	        iInfo->iServiceGroup );
	    iCFISPrivateCentRep->Set( KCtsyCallForwardingAction, 
	        iInfo->iAction );
	    iCFISPrivateCentRep->Set( KEFCfisiTimeout, iInfo->iTimeout );
	    iCFISPrivateCentRep->Set( KCtsyCallForwardingNumberPlan, 
	        iInfo->iNumber.iNumberPlan );
	    iCfisCentRep->Set( KCtsyCallForwardingNumber, 
	        iInfo->iNumber.iTelNumber );        
	    iCFISPrivateCentRep->Set( KCtsyCallForwardingMspId, 
	        RMmCustomAPI::KProfileIdentityOne );
	    
	    iMmPhone->ReqCompleted( reqHandle, aErrorCode );
	    iSsTransactionOngoing = EFalse;
       }  
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetCallForwardingStatusCancel
// This method cancels setting of the call forwarding status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetCallForwardingStatusCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    TTsyReqHandle cancelSetCallForwardingStatus =
        iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneSetCallForwardingStatus );

    if ( !cancelSetCallForwardingStatus )
        {
        // Cancellation called although request is not going on
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETCALLFORWARDINGSTATUSCANCEL_1, "TSY: CMmSupplServTsy::SetCallForwardingStatusCancel - KErrServerBusy");
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
        }
    iSsTransactionOngoing = EFalse;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallForwardingActive
// Call Forwarding Active Notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallForwardingActive(
    RMobilePhone::TMobileService* aServiceGroup, 
    RMobilePhone::TMobilePhoneCFActive* aActiveType ) 
    {
    iRetServiceGroup = aServiceGroup;
    iRetActiveType = aActiveType;

    iMmPhone->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyCallForwardingActive;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallForwardingActiveCancel
// Use this method to cancel a previously placed asynchronous 
// NotifyCallForwardingActive request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallForwardingActiveCancel(
    TTsyReqHandle aTsyReqHandle ) 

    {
    iRetServiceGroup = NULL;
    iRetActiveType = NULL;
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallForwardingActive );

    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteNotifyCallForwardingActive
// Completes asynchronous CallForwardingActive notification 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteNotifyCallForwardingActive(
    RMobilePhone::TMobileService aServiceGroup, 
    RMobilePhone::TMobilePhoneCFActive aActiveType )                                            
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallForwardingActive );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {        
        *iRetServiceGroup = aServiceGroup;
        *iRetActiveType = aActiveType;
        iRetServiceGroup = NULL;
        iRetActiveType = NULL;
        iMmPhone->ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallBarringStatusPhase1L
// First phase of fetching call barring status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallBarringStatusPhase1L(
    const TTsyReqHandle aTsyReqHandle,  // Tsy request handle
    CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData,
    TInt* aBufSize ) // Buffer size pointer
    {
    TInt ret = KErrNone;
    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    // Check some error cases first
    else if ( RMobilePhone::EInfoLocationCache == aReqData->iLocation )
        {
        // No cache implemented
        ret = KErrNotFound;
        }
    else if ( ( RMobilePhone::EBarAllCases == aReqData->iCondition )
          || ( RMobilePhone::EBarAllOutgoingServices == aReqData->iCondition )
          || ( RMobilePhone::EBarAllIncomingServices == aReqData->iCondition ) )
        {
        // The combined cases cannot be used for requesting the status,
        // only for deactivation
        ret = KErrArgument;
        }
    else
        {
        // Forward the request to DOS
        CMmDataPackage package;
        package.PackData( &aReqData->iCondition );
        ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneGetBarringStatusPhase1, &package );
        iSsTransactionOngoing = ETrue;
        }
    if ( ret != KErrNone )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        }
    else
        {
        // Save return pointer for list size
        iRetCBStatusSize = aBufSize;

        // Copy client id (session and subsession handle). it's used  
        // for matching phase 1 and 2 of a request
        
        if ( iCBClientId )
            {
            delete iCBClientId;
            iCBClientId = NULL;
            }
            
        iCBClientId = new( ELeave ) RMobilePhone::TClientId( 
                                                      aReqData->iClient );

        // Save req handle type
        iMmPhone->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneGetCallBarringStatusPhase1;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallBarringStatusPhase2
// Second phase of fetching call barring status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallBarringStatusPhase2(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TClientId const* aClient, 
    TDes8* aBuf )   
    {
    TInt ret = KErrNone;
    // Check if client id matches between phase 1 and phase 2
    if ( ( iCBClientId->iSessionHandle == aClient->iSessionHandle ) &&
         ( iCBClientId->iSubSessionHandle == aClient->iSubSessionHandle ) )
        {
        // Copy the streamed list to the client
        aBuf->Copy( iCBList->Ptr( 0 ) );

        // Reset the client Id
        delete iCBClientId;
        iCBClientId = NULL;

        // Reset the list
        delete iCBList;
        iCBList = NULL;
        }
    else
        {
        ret = KErrNotFound;
        }

    iMmPhone->ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallBarringStatusPhase2
// This method cancels an outstanding GetCallBarringStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallBarringStatusCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    iRetCBStatusSize = NULL;

    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneGetCallBarringStatusPhase1 );

    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
    iSsTransactionOngoing = EFalse;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteGetCallBarringStatusPhase1L
// This method completes an outstanding GetCallBarringStatusPhase1 request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteGetCallBarringStatusPhase1L(
    CMobilePhoneCBList* aResults, 
    TInt aErrorCode ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneGetCallBarringStatusPhase1 );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            // If there's no error, we should have got some results ...
            if ( aResults && aResults->Enumerate() > 0 )
                {
                // Store the streamed list for phase 2
                iCBList = aResults->StoreLC();          
                CleanupStack::Pop(); // Pop the CBufFlat allocated by StoreLC
                }
            if ( iCBList ) 
                {
                // Copy the size of the streamed list to client
                *iRetCBStatusSize = iCBList->Size();
                }
            else
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETEGETCALLBARRINGSTATUSPHASE1L_1, "TSY: CMmSupplServTsy::CompleteGetCallBarringStatusPhase1L: No response received.");
                aErrorCode = KErrNotFound;
                }
            }
            
        iRetCBStatusSize = NULL;
        iMmPhone->ReqCompleted( reqHandle, aErrorCode );       
        iSsTransactionOngoing = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetCallBarringStatusL
// This method sets the call barring status for the call barring 
// condition specified by the aCondition parameter. The client can 
// be activating or deactivating the specified CB condition.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetCallBarringStatusL( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TMobilePhoneCBCondition* aCondition, 
    const RMobilePhone::TMobilePhoneCBChangeV1* aInfo ) 
    {
    TInt ret = KErrNone;

    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    else if ( KMmTsyMaxLengthOfSSPassword != aInfo->iPassword.Length() )
        {
        // Password must have full length
        ret = CMmCommonStaticUtility::EpocErrorCode( KErrGeneral,
            KErrGsmSSPasswordRegistrationFailure );
        }
    else if ( ( RMobilePhone::EServiceActionActivate != aInfo->iAction ) &&
        ( RMobilePhone::EServiceActionDeactivate != aInfo->iAction ) &&
        ( RMobilePhone::EServiceActionRegister != aInfo->iAction ) &&
        ( RMobilePhone::EServiceActionErase != aInfo->iAction ) )
        {
        // Status can be set only by above actions
        ret = KErrArgument;
        }
    else if ( ( ( RMobilePhone::EServiceActionDeactivate != aInfo->iAction ) 
        && ( RMobilePhone::EServiceActionErase != aInfo->iAction ) )
        && 
        ( ( RMobilePhone::EBarAllCases == *aCondition ) 
        || ( RMobilePhone::EBarAllOutgoingServices == *aCondition ) 
        || ( RMobilePhone::EBarAllIncomingServices == *aCondition ) ) )
        {
        // The combined conditions (all calls, all outgoing calls, 
        // all incoming calls) are legal only for deactivation and erasure
        ret = KErrArgument;
        }
    else
        {
        // Forward the request to DOS
        // packed parameters: a RMobilePhone::TMobilePhoneCBCondition 
        // and a RMobilePhone::TMobilePhoneCBChangeV1
        CMmDataPackage package;
        package.PackData( aCondition, 
            const_cast<RMobilePhone::TMobilePhoneCBChangeV1**>( &aInfo ) );
        ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobilePhoneSetCallBarringStatus, &package );
        iSsTransactionOngoing = ETrue;
        }
    if ( ret != KErrNone )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        }
    else
        {
        //Save req handle type
        iMmPhone->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneSetCallBarringStatus;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteSetCallBarringStatus
// This method completes an outstanding asynchronous 
// SetCallBarringStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteSetCallBarringStatus(
    TInt aErrorCode ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneSetCallBarringStatus );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        iMmPhone->ReqCompleted( reqHandle, aErrorCode );
        iSsTransactionOngoing = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetCallBarringStatusCancel
// This method cancels setting of the call barring status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetCallBarringStatusCancel(
    const TTsyReqHandle aTsyReqHandle ) //Request status
    {
    TTsyReqHandle cancelSetCallBarringStatus = iMmPhone->iTsyReqHandleStore->
        ResetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneSetCallBarringStatus );

    if ( !cancelSetCallBarringStatus )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETCALLBARRINGSTATUSCANCEL_1, "TSY: CMmPhoneTsy::SetCallBarringStatusCancel - KErrServerBusy");
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
        }
    iSsTransactionOngoing = EFalse;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallBarringStatusChange
// This request allows a client to be notified when the status 
// or associated information changes for any of the call barring conditions 
// as applied to any of the call service groups.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallBarringStatusChange(
    RMobilePhone::TMobilePhoneCBCondition* aCondition ) 
    {
    iRetCallBarringCondition = aCondition;
    // Save req handle type
    iMmPhone->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyCallBarringStatusChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallBarringStatusChangeCancel
// This method cancels an outstanding asynchronous 
// NotifyCallBarringStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallBarringStatusChangeCancel( 
    const TTsyReqHandle aTsyReqHandle ) // tsy request handle
    {
    iRetCallBarringCondition = NULL;
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallBarringStatusChange );
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteNotifyCallBarringStatusChange
// This method completes an outstanding asynchronous 
// NotifyCallBarringStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteNotifyCallBarringStatusChange(
    RMobilePhone::TMobilePhoneCBCondition aCondition ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallBarringStatusChange );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        *iRetCallBarringCondition = aCondition;
        iRetCallBarringCondition = NULL; 
        iMmPhone->ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetSSPasswordL
// This method allows the client to change the call barring 
// password or common SS password.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetSSPasswordL(
    const TTsyReqHandle aTsyReqHandle, // Tsy request handle
    TDes8* aPasswordPckg, // Password package
    TInt const* aSsService ) // SS service
    {
    TInt ret = KErrArgument;
    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    // Check if parameter aSsService contains one of the two valid service 
    // codes: According to 3GPP TS 22.030 V5.0.0 (2002-06), the only two 
    // valid cases are 
    // "Password for Barring Services" = 330 
    // "Common Password for All Appropriate Services" = 0
    else if ( ( 330 == *aSsService ) || ( 0 == *aSsService ) )
        {
        TPckg<RMobilePhone::TMobilePhonePasswordChangeV2>* ssPasswordPckg = 
            reinterpret_cast<TPckg<RMobilePhone::TMobilePhonePasswordChangeV2>*>
                ( aPasswordPckg );
        RMobilePhone::TMobilePhonePasswordChangeV2& ssPassword = 
            ( *ssPasswordPckg )();

        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETSSPASSWORDL_1, "TSY: CMmSupplServTsy::SetSSPassword - SS service: %d", *aSsService );

        OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETSSPASSWORDL_2, "TSY: CMmSupplServTsy::SetSSPassword - Old PW: %S", ssPassword.iOldPassword );

        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETSSPASSWORDL_3, "TSY: CMmSupplServTsy::SetSSPassword - New PW: %S, Verify PW: %S", ssPassword.iNewPassword, ssPassword.iVerifiedPassword );

        // Packed parameters: TMobilePhonePasswordChangeV2 and TUint16
        TUint16 ssServiceUnsigned = static_cast<TUint16>( *aSsService );
        CMmDataPackage dataPackage;
        dataPackage.PackData( &ssPassword, &ssServiceUnsigned );
        ret = iMmPhone->MessageManager()->HandleRequestL( 
                  EMobilePhoneSetSSPassword, &dataPackage );
        iSsTransactionOngoing = ETrue;
        }
    if ( KErrNone != ret ) 
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        }
    else
        {
        // Save req handle type
        iMmPhone->iReqHandleType = CMmPhoneTsy::EMultimodePhoneSetSSPassword;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteSetSSPassword
// This method completes an outstanding asynchronous SetSSPassword request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteSetSSPassword(
    TInt aErrorCode ) // Error code
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneSetSSPassword );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        iMmPhone->ReqCompleted( reqHandle, aErrorCode );
        iSsTransactionOngoing = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetSSPasswordCancel
// This method cancels setting of the SS password.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetSSPasswordCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    TTsyReqHandle cancelSetSSPasswordHandle = 
        iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneSetSSPassword );

    if ( !cancelSetSSPasswordHandle )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETSSPASSWORDCANCEL_1, "TSY: CMmSupplServTsy::SetSSPasswordCancel - KErrServerBusy");
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
        }
    iSsTransactionOngoing = EFalse;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallWaitingStatusPhase1L
// First phase of fetching call waiting status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallWaitingStatusPhase1L(
   	const TTsyReqHandle aTsyReqHandle, 
   	CRetrieveMobilePhoneCWList::TGetCallWaitingRequest const* aReqData, 
   	TInt* aBufSize ) 
    {
    TInt ret = KErrNone;
    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    else if ( RMobilePhone::EInfoLocationCache == aReqData->iLocation )
        {
        // Cache not supported
        ret = KErrNotFound;
        }
     else 
        {
        // Forward the request to DOS
        // no packed parameters
        ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneGetWaitingStatusPhase1 );
        iSsTransactionOngoing = ETrue;
        }

    if ( ret != KErrNone )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        }
    else
        {
        // Save return pointer for list size
        iRetCWStatusSize = aBufSize;

        // Copy client id (session and subsession handle). it's used for
        // matching phase 1 and 2 of a request
        
        if ( iCWClientId )
            {
            delete iCWClientId;
            iCWClientId = NULL;
            }
            
        iCWClientId = new( ELeave ) RMobilePhone::TClientId( 
                                                      aReqData->iClient );

        // Save req handle type
        iMmPhone->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneGetCallWaitingStatusPhase1;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallWaitingStatusPhase2
// Second phase of fetching call waiting status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallWaitingStatusPhase2(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TClientId const* aClient, 
    TDes8* aBuf ) 
    {
    TInt ret = KErrNone;
    // Check if client id matches between phase 1 and phase 2
    if ( ( iCWClientId->iSessionHandle == aClient->iSessionHandle ) &&
         ( iCWClientId->iSubSessionHandle == aClient->iSubSessionHandle ) )
        {
        // Copy the streamed list to the client
        aBuf->Copy( iCWList->Ptr( 0 ) );

        // Reset the client Id
        delete iCWClientId;
        iCWClientId = NULL;

        // Reset the list
        delete iCWList;
        iCWList = NULL;
        }
    else
        {
        ret = KErrNotFound;
        }

    iMmPhone->ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetCallWaitingStatusCancel
// This method cancels an outstanding GetCallWaitingStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetCallWaitingStatusCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    iRetCWStatusSize = NULL;

    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneGetCallWaitingStatusPhase1 );

    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
    iSsTransactionOngoing = EFalse;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteGetCallWaitingStatusPhase1L
// This method completes an outstanding GetCallWaitingStatusPhase1L request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteGetCallWaitingStatusPhase1L(
    CMobilePhoneCWList* aResults, 
    TInt aErrorCode ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneGetCallWaitingStatusPhase1 );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            // If there's no error, we should have got some results ...
            if ( aResults )
                {
                // Store the streamed list for phase 2
                iCWList = aResults->StoreLC();          
                CleanupStack::Pop(); // Pop the CBufFlat allocated by StoreLC
                }

            if ( iCWList ) 
                {
                // Copy the size of the streamed list to client
                *iRetCWStatusSize = iCWList->Size();
                }
            else
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETEGETCALLWAITINGSTATUSPHASE1L_1, "TSY: CMmSupplServTsy::CompleteGetCallWaitingStatusPhase1L: No response received.");
                aErrorCode = KErrNotFound;
                }
            }
            
        iRetCWStatusSize = NULL;
        iMmPhone->ReqCompleted( reqHandle, aErrorCode );
        iSsTransactionOngoing = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetCallWaitingStatusL
// This method sets the call waiting status. The client can be activating 
// or deactivating the CW service.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetCallWaitingStatusL(
    const TTsyReqHandle aTsyReqHandle,  
    RMobilePhone::TMobileService* aServiceGroup,     
    RMobilePhone::TMobilePhoneServiceAction* aAction ) 
    {
    TInt ret = KErrNone;
    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    else if ( ( RMobilePhone::EServiceActionActivate != *aAction ) &&
            ( RMobilePhone::EServiceActionDeactivate != *aAction ) )
        {
        // Status can be set only by above actions
        ret = KErrArgument;
        }
    else
        {
        // Forward the request to DOS
        // packed parameters: a TMobileService and a TMobilePhoneServiceAction
        CMmDataPackage package;
        package.PackData( aServiceGroup, aAction );
        ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneSetCallWaitingStatus, &package );
        iSsTransactionOngoing = ETrue;
        }

    if ( ret != KErrNone )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        }
    else
        {
        // Save req handle type
        iMmPhone->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneSetCallWaitingStatus;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteSetCallWaitingStatus
// This method completes an outstanding asynchronous SetCallWaitingStatus 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteSetCallWaitingStatus(
    TInt aErrorCode ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneSetCallWaitingStatus );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        iMmPhone->ReqCompleted( reqHandle, aErrorCode );
        iSsTransactionOngoing = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetCallWaitingStatusCancel
// This method cancels setting of the call waiting status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetCallWaitingStatusCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    TTsyReqHandle cancelSetCallWaitingStatusHandle = 
        iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneSetCallWaitingStatus );

    if ( !cancelSetCallWaitingStatusHandle )
        {
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about 
        //the situation.
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETCALLWAITINGSTATUSCANCEL_1, "TSY: CMmSupplServTsy::SetCallWaitingStatusCancel - KErrServerBusy");
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
        }
    iSsTransactionOngoing = EFalse;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallWaitingStatusChange
// This request allows a client to be notified when the status 
// changes for the call waiting service as applied to any of the call 
// service groups.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallWaitingStatusChange(
     TDes8* aCWStatus )   
    {
    TInt ret = KErrArgument;      
    
    if ( sizeof( RMobilePhone::TMobilePhoneCWInfoEntryV1 ) <= 
         aCWStatus->MaxLength() )
        {    
        ret = KErrNone;
        iRetNotifyCallWaitingStatus = aCWStatus;

        // Save req handle type
        iMmPhone->iReqHandleType = CMmPhoneTsy::EMultimodePhoneNotifyCallWaitingStatusChange;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyCallWaitingStatusChangeCancel
// This method cancels an outstanding asynchronous 
// NotifyCallWaitingStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyCallWaitingStatusChangeCancel( 
    const TTsyReqHandle aTsyReqHandle ) 
    {
    iRetNotifyCallWaitingStatus = NULL;
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallWaitingStatusChange );

    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteNotifyCallWaitingStatusChange
// This method cancels an outstanding asynchronous 
// NotifyCallWaitingStatusChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteNotifyCallWaitingStatusChange(
    RMobilePhone::TMobilePhoneCWInfoEntryV1 const* aChangedCwStatus )
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyCallWaitingStatusChange );

    if( ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle ) 
        && ( NULL != iRetNotifyCallWaitingStatus ) )
        {
        // Pack status information
        RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg* cwStatusPckg = 
            reinterpret_cast<RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg*>( 
            iRetNotifyCallWaitingStatus );
        RMobilePhone::TMobilePhoneCWInfoEntryV1& cwStatus = 
                                                    ( *cwStatusPckg )();
    
        cwStatus.iServiceGroup = aChangedCwStatus->iServiceGroup;
        cwStatus.iStatus = aChangedCwStatus->iStatus;

        iRetNotifyCallWaitingStatus = NULL; 
        iMmPhone->ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetIdentityServiceStatusL
// This method reads the current status of the Identity Service.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
TInt CMmSupplServTsy::GetIdentityServiceStatusL(
    const TTsyReqHandle aTsyReqHandle,      
    RMobilePhone::TIdServiceAndLocation* aServiceAndLocation, 
    RMobilePhone::TMobilePhoneIdServiceStatus* aStatus ) 
    {
    TInt ret = KErrNone;
    // Check some error cases first
    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    else if ( RMobilePhone::EInfoLocationCache == 
                                aServiceAndLocation->iLocation )
        {
        // No cache implemented
        ret = KErrNotFound;
        }
    else
        {
        // Forward the request to DOS
        // packed parameter: TMobilePhoneIdService (service code)
        CMmDataPackage package;
        package.PackData( &aServiceAndLocation->iService );
        ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneGetIdentityServiceStatus, &package );
        iSsTransactionOngoing = ETrue;
        }
    if ( ret != KErrNone )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        }
    else
        {
        // Save return pointer for list size
        iRetGetIdentityServiceStatus = aStatus;

        // Save req handle type
        iMmPhone->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneGetIdentityServiceStatus;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetIdentityServiceStatusCancel
// This method cancels an outstanding asynchronous GetIdentityServiceStatus 
// method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
TInt CMmSupplServTsy::GetIdentityServiceStatusCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetGetIdentityServiceStatus = NULL;

    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneGetIdentityServiceStatus );

    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
    iSsTransactionOngoing = EFalse;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteGetIdentityServiceStatus
// Completes the GetIdentityServiceStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
void CMmSupplServTsy::CompleteGetIdentityServiceStatus(
    RMobilePhone::TMobilePhoneIdServiceStatus aStatus, 
    TInt aErrorCode )
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneGetIdentityServiceStatus );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            // Copy the returned status to client
            *iRetGetIdentityServiceStatus = aStatus;
            }
            
        iRetGetIdentityServiceStatus = NULL;
        iMmPhone->ReqCompleted( reqHandle, aErrorCode );
        iSsTransactionOngoing = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SendNetworkServiceRequestL
// This method sends a network service request string ( e.g. a SS string 
// entered by the user ) directly to the serving network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
TInt CMmSupplServTsy::SendNetworkServiceRequestL(
    const TTsyReqHandle aTsyReqHandle, 
    const TDesC* aServiceString )
    {
    TInt ret = KErrNone;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SENDNETWORKSERVICEREQUESTL_1, "TSY: CMmSupplServTsy::SendNetworkServiceRequestL: iSsNoFdnCheckFlag: %d", iSsNoFdnCheckFlag);
    if ( iSsTransactionOngoing )
	    {
	    ret = KErrServerBusy;
	    }
    else
        {
        // Forward the request to DOS
        // packed parameter:TDesC* (service string)
        CMmDataPackage package;
        package.PackData( &aServiceString );
        
        if( iSsNoFdnCheckFlag == ESsNoFdnCheckUsed )
            {
            ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobilePhoneSendNetworkServiceRequestNoFdnCheck, &package );
            }
        if ( iSsNoFdnCheckFlag == ESsNoFdnCheckNotUsed )
            {
            ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobilePhoneSendNetworkServiceRequest, &package );
            }            
        
        iSsTransactionOngoing = ETrue;
        }

    if ( ret != KErrNone )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        iSsTransactionOngoing = EFalse;
        }
    else
        {
        if( iSsNoFdnCheckFlag == ESsNoFdnCheckUsed )
            {
            // Save tsy req handle type
            iMmPhone->iReqHandleType = CMmPhoneTsy::
                EMultimodePhoneSendNetworkServiceRequestNoFdnCheck;    
            }
        if ( iSsNoFdnCheckFlag == ESsNoFdnCheckNotUsed )
            {
            // Save tsy req handle type
            iMmPhone->iReqHandleType = CMmPhoneTsy::
                EMultimodePhoneSendNetworkServiceRequest;            
            }
        }
    
    return KErrNone; 
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteSendNetworkServiceRequest
// Completes an outstanding SendNetworkServiceRequest request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteSendNetworkServiceRequest(
    TInt aErrorCode ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneSendNetworkServiceRequest );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        iMmPhone->ReqCompleted( reqHandle, aErrorCode );
        iSsTransactionOngoing = EFalse;
        
        iSsNoFdnCheckFlag = ESsNoFdnCheckUnknown; 
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetIccMessageWaitingIndicatorsL
// Retrieves the set of message waiting indicators from the current ICC, if 
// the ICC doesn't support MWIS then get it from the CenRep. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetIccMessageWaitingIndicatorsL( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TMobilePhoneMessageWaitingV1* aMessageWaiting )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_GETICCMESSAGEWAITINGINDICATORSL_1, "TSY: CMmSupplServTsy::GetIccMessageWaitingIndicatorsL");
    
    // check if parameter is correct type of class
    if ( KETelExtMultimodeV1 != aMessageWaiting->ExtensionId() )
        {
		// complete request
		iMmPhone->ReqCompleted( aTsyReqHandle, KErrArgument ); 
        }
    
    else
        {
        // check that handle is not used for Set or Get
         if ( ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown == 
            iMmPhone->iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneGetIccMessageWaitingIndicators ) ) &&
            ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown == 
            iMmPhone->iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneSetIccMessageWaitingIndicators ) ) )
            {
            TInt ret( KErrNone );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_GETICCMESSAGEWAITINGINDICATORSL_2, "TSY: CMmSupplServTsy::GetIccMessageWaitingIndicatorsL, MWIS supported:%d", iMessageWaitingSupportedByIcc );

			if (iMmPhone->IsSIMReady())
				{
				if ( iMessageWaitingSupportedByIcc )
					{ 
			        
					// no packed parameters for DOS call 
					ret = iMmPhone->iMessageManager->HandleRequestL( 
					    EMobilePhoneGetIccMessageWaitingIndicators ); 
			
					if ( KErrNone != ret ) 
						{ 
						// complete request
						iMmPhone->ReqCompleted ( aTsyReqHandle, ret ); 
						} 
					else 
						{ 
						// save tsy req handle type 
						iMmPhone->iReqHandleType = CMmPhoneTsy::
						    EMultimodePhoneGetIccMessageWaitingIndicators; 
						// save request in queue for completion
						TMwisInfoRequest* req = new (ELeave) TMwisInfoRequest();
						req->iReqHandle = aTsyReqHandle;
						req->iMwisInfo = aMessageWaiting;
						iMwisInfoRequests.AppendL(req);
						}
					}
				else
				    {	      
				    // read the values from the central repository          
			        ret = ReadIccMessageWaitingIndicatorsFromCentRep( 
			            iMessageWaiting );
			        
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_GETICCMESSAGEWAITINGINDICATORSL_3, "TSY: CMmSupplServTsy::GetIccMessageWaitingIndicatorsL, Error during the reading from CentRep:%{TSymbianErrorCodes}", ret);
				
				   	if ( KErrNone == ret )
				   		{
				   		// copy default values to the client side 
						*aMessageWaiting = iMessageWaiting; 
						// complete request immediately
						iMmPhone->ReqCompleted ( aTsyReqHandle, ret ); 
				   		}
				    }
				}
			else
				{
				// save request in queue for completion
				TMwisInfoRequest* req = new (ELeave) TMwisInfoRequest();
				req->iReqHandle = aTsyReqHandle;
				req->iMwisInfo = aMessageWaiting;
				iMwisInfoRequests.AppendL(req);
				}
			}
			else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_GETICCMESSAGEWAITINGINDICATORSL_4, "TSY: CMmSupplServTsy::GetIccMessageWaitingIndicatorsL - save reqHandle to queue");
			// Save reqHandle to queue for completion
			TMwisInfoRequest* req = new (ELeave) TMwisInfoRequest();
			req->iReqHandle = aTsyReqHandle;
			req->iMwisInfo = aMessageWaiting;
			iMwisInfoRequests.AppendL(req);
			}
        }
    
    return KErrNone; 
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteGetIccMessageWaitingIndicators
// This method completes an outstanding GetIccMessageWaitingIndicators 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteGetIccMessageWaitingIndicators( 
    RMobilePhone::TMobilePhoneMessageWaitingV1* aMessageWaiting, 
    TInt aErrorCode )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETEGETICCMESSAGEWAITINGINDICATORS_1, "TSY: CMmSupplServTsy::CompleteGetIccMessageWaitingIndicators, bootup=%d, Error: %{TSymbianErrorCodes}", iGetIccSupportAtBoot, aErrorCode );
             
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetIccMessageWaitingIndicators );
        
    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( ( KErrNone == aErrorCode ) || 
            ( KErrCPHSInUseBySIM == aErrorCode ) )
            {
            SetIccSupportForMWIS( ETrue );
            
            // has information changed
            if ( IsMwisInfoChanged ( *aMessageWaiting ) )
                {  
                // complete notify; also copy information if changed
                CompleteNotifyIccMessageWaitingIndicatorChange( 
                    aMessageWaiting, aErrorCode );
                }
            else
                {
                // set the error code to none, in CPHS case
                aErrorCode = KErrNone; 
                }
            }
        else 
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETEGETICCMESSAGEWAITINGINDICATORS_2, "TSY: CMmSupplServTsy::CompleteGetIccMessageWaitingIndicators, NOT supported by SIM" );
            SetIccSupportForMWIS( EFalse );
            
            // read the data from the Central Repository
            aErrorCode = ReadIccMessageWaitingIndicatorsFromCentRep( 
                iMessageWaiting );           
            }
                 
        // Go through the request queue and complete same information to all reguests
        for ( TInt i = 0; i < iMwisInfoRequests.Count(); i++ )
        	{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETEGETICCMESSAGEWAITINGINDICATORS_3, "TSY: CMmSupplServTsy::CompleteGetIccMessageWaitingIndicators, Going throw request queue" );
        	TMwisInfoRequest* req = iMwisInfoRequests[ i ];
        	//set value in client side
        	*(req->iMwisInfo) = iMessageWaiting;
        	// Complete the client request
        	iMmPhone->ReqCompleted( req->iReqHandle, aErrorCode );
        	}
        //Destroy Array - all client request handled
        iMwisInfoRequests.ResetAndDestroy();
        }   
    else if ( iGetIccSupportAtBoot )
        {
        iGetIccSupportAtBoot = EFalse;
        
        // BOOT-UP case
        if ( KErrNone == aErrorCode )
            {
            // SIM supports EF-MWIS
            SetIccSupportForMWIS( ETrue );
            iMessageWaiting = *aMessageWaiting;
            aErrorCode = WriteIccMessageWaitingIndicatorsToCentRep( 
                iMessageWaiting );    
            }
        else if ( KErrCPHSInUseBySIM == aErrorCode )
            {
            SetIccSupportForMWIS( ETrue );
            // update only display status information
            iMessageWaiting.iDisplayStatus = aMessageWaiting->iDisplayStatus;
            aErrorCode = iMwisCentRep->Set( KCtsyMessageWaitingDisplayStatus, 
                aMessageWaiting->iDisplayStatus );
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETEGETICCMESSAGEWAITINGINDICATORS_4, "TSY: CMmSupplServTsy::CompleteGetIccMessageWaitingIndicators, CPHS bootup, status=%d, error=%{TSymbianErrorCodes} ", aMessageWaiting->iDisplayStatus, aErrorCode );
            }
        else
            {
            SetIccSupportForMWIS( EFalse );
            }
        } 
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::GetIccMessageWaitingIndicatorsCancel
// Use this method to cancel a previously placed asynchronous 
// GetIccMessageWaitingIndicators request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::GetIccMessageWaitingIndicatorsCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_GETICCMESSAGEWAITINGINDICATORSCANCEL_1, "TSY: CMmSupplServTsy::GetIccMessageWaitingIndicatorsCancel");
    
	//reset the req handle if only one req remains
	if( 1 == iMwisInfoRequests.Count() )
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_GETICCMESSAGEWAITINGINDICATORSCANCEL_2, "TSY: CMmSupplServTsy::GetIccMessageWaitingIndicatorsCancel - one req");
		TTsyReqHandle reqHandle = iMmPhone->
		iTsyReqHandleStore->ResetTsyReqHandle(
				CMmPhoneTsy::EMultimodePhoneGetIccMessageWaitingIndicators);
		}
	// qo through array if several requests and find correct reqhandle to cancel
	for ( TInt i = 0; i < iMwisInfoRequests.Count(); i++ )
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_GETICCMESSAGEWAITINGINDICATORSCANCEL_3, "TSY: CMmSupplServTsy::GetIccMessageWaitingIndicatorsCancel - queued requests");
		TMwisInfoRequest* req = iMwisInfoRequests[ i ];
		if( aTsyReqHandle == req->iReqHandle  )
			{
			iMmPhone->ReqCompleted( req->iReqHandle, KErrCancel );
			delete iMwisInfoRequests[ i ];
			iMwisInfoRequests.Remove(i);
			break;
			}
		}
    
    return KErrNone; 
    }   

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetIccMessageWaitingIndicatorsL
// Set the message waiting indicators on the current ICC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetIccMessageWaitingIndicatorsL( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aParams )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETICCMESSAGEWAITINGINDICATORSL_1, "TSY: CMmSupplServTsy::SetIccMessageWaitingIndicatorsL");
    
    TInt ret( KErrNone ); 

    if ( sizeof( RMobilePhone::TMobilePhoneMessageWaitingV1 ) > aParams->MaxLength() )
	  	{
	  	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETICCMESSAGEWAITINGINDICATORSL_2, "TSY: CMmSupplServTsy::SetIccMessageWaitingIndicatorsL bad size argument");
	  	// Complete the request with appropiate error        
        ret = KErrArgument;
	  	}
    
    // check if handle is not used for Set or Get
    else if ( ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown == iMmPhone->
            iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneSetIccMessageWaitingIndicators ) ) &&
            ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown == iMmPhone->
            iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneGetIccMessageWaitingIndicators ) ) )
        {
        
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETICCMESSAGEWAITINGINDICATORSL_3, "TSY: CMmSupplServTsy::SetIccMessageWaitingIndicatorsL, MWIS supported:%d",iMessageWaitingSupportedByIcc );
    
        RMobilePhone::TMobilePhoneMessageWaitingV1Pckg* paramsPckgV1 = 
            reinterpret_cast<RMobilePhone::TMobilePhoneMessageWaitingV1Pckg*>( aParams );
        RMobilePhone::TMobilePhoneMessageWaitingV1& messageWaiting = ( *paramsPckgV1 )(); 
       	
		if ( iMessageWaitingSupportedByIcc )    
			{
			// save the client pointer temporaryly, used again on complete
            iRetSetMessageWaiting = &messageWaiting;
			
            // packed parameter for DOS call: 
            // RMobilePhone::TMobilePhoneMessageWaitingV1
    		CMmDataPackage dataPackage;    			
    		dataPackage.PackData( &messageWaiting );
    		
    		// send EMultimodePhoneSetIccMessageWaitingIndicators to DOS
            ret = iMmPhone->iMessageManager->HandleRequestL( 
                EMobilePhoneSetIccMessageWaitingIndicators, 
                &dataPackage );

            if ( KErrNone == ret )
                {
                // save tsy req handle type
                iMmPhone->iReqHandleType = CMmPhoneTsy::
                    EMultimodePhoneSetIccMessageWaitingIndicators;
                }
            }  
        
        else
            {
            // complete notify; also copy information if changed
            CompleteNotifyIccMessageWaitingIndicatorChange( 
                &messageWaiting, ret );
            iMmPhone->ReqCompleted ( aTsyReqHandle, ret );
            ret = KErrNone;
            }  
		}
    
    else
        {
        // handle already in use
        ret = KErrServerBusy; 
        }
    
    return ret; 
    } 

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteSetIccMessageWaitingIndicators
// This method completes an outstanding SetIccMessageWaitingIndicators 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteSetIccMessageWaitingIndicators( 
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETESETICCMESSAGEWAITINGINDICATORS_1, "TSY: CMmSupplServTsy::CompleteSetIccMessageWaitingIndicators, Error: %{TSymbianErrorCodes}", aErrorCode );
   
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneSetIccMessageWaitingIndicators );
        
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {        
        if ( ( KErrNone == aErrorCode ) || 
            ( KErrCPHSInUseBySIM == aErrorCode ) )
            {
            // send reqest to LTSY also in future
            SetIccSupportForMWIS( ETrue );
            
            // map error value so that during the notify-complete
            // the central repository values are updated, in CPHS case
            aErrorCode = KErrNone; 
            
            // complete notify; also copy information if changed
            CompleteNotifyIccMessageWaitingIndicatorChange( 
                iRetSetMessageWaiting, aErrorCode );
            }
        else
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETESETICCMESSAGEWAITINGINDICATORS_2, "TSY: CMmSupplServTsy::CompleteSetIccMessageWaitingIndicators, not supported by SIM" );
            SetIccSupportForMWIS( EFalse );
            
            // reset error value, return possible cenrep error value
            aErrorCode = KErrNone; 
            // complete notify; also copy information if changed
            CompleteNotifyIccMessageWaitingIndicatorChange( 
                iRetSetMessageWaiting, aErrorCode );
            }
            
        // reset the client pointer
        iRetSetMessageWaiting = NULL;          
        // Complete the client request
        iMmPhone->ReqCompleted( reqHandle, aErrorCode ); 
        }     

    //It might be possible that at the time of serving the request for SetIccMessageWaitingIndicators()
    //CTSY receives the request for GetIccMessageWaitingIndicators().
    //In that case the request will be queued in request queue.
    // So go through the request queue and complete all the request.
    for ( TInt i = 0; i < iMwisInfoRequests.Count(); i++ )
    	{
    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETESETICCMESSAGEWAITINGINDICATORS_3, "TSY: CMmSupplServTsy::CompleteGetIccMessageWaitingIndicators, Going throw request queue" );
    	TMwisInfoRequest* req = iMwisInfoRequests[ i ];
    	//set value in client side
    	*(req->iMwisInfo) = iMessageWaiting;
    	// Complete the client request
    	iMmPhone->ReqCompleted( req->iReqHandle, KErrNone );
    	}
    //Destroy Array - all client request handled
    iMwisInfoRequests.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetIccMessageWaitingIndicatorsCancel
// Use this method to cancel a previously placed asynchronous 
// SetIccMessageWaitingIndicators request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::SetIccMessageWaitingIndicatorsCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETICCMESSAGEWAITINGINDICATORSCANCEL_1, "TSY: CMmSupplServTsy::SetIccMessageWaitingIndicatorsCancel");
    
    // As cancel is called, the client memory address pointed to is no longer 
    // valid -> NULL it
    iRetSetMessageWaiting = NULL; 
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneSetIccMessageWaitingIndicators );
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
    
    return KErrNone; 
    } 

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyIccMessageWaitingIndicatorsChange
// Notify when message waiting indicators changed on ICC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyIccMessageWaitingIndicatorsChange( 
    RMobilePhone::TMobilePhoneMessageWaitingV1* aMessageWaiting )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_NOTIFYICCMESSAGEWAITINGINDICATORSCHANGE_1, "TSY: CMmSupplServTsy::NotifyIccMessageWaitingIndicatorsChangeL");
    
    TInt ret( KErrNone );
    
    // check if parameter is correct type of class
    if ( KETelExtMultimodeV1 == aMessageWaiting->ExtensionId() )
        {
        iRetNotifyMessageWaiting = aMessageWaiting;
        iMmPhone->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneNotifyIccMessageWaitingIndicatorsChange;
        }
    
    else
        {
        ret = KErrArgument;
        }
    
    return ret;
    } 

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteNotifyIccMessageWaitingIndicatorChange
// This method completes an outstanding NotifyIccMessageWaitingIndicatorChange 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteNotifyIccMessageWaitingIndicatorChange(
    RMobilePhone::TMobilePhoneMessageWaitingV1* aMessageWaiting,
    TInt& aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETENOTIFYICCMESSAGEWAITINGINDICATORCHANGE_1, "TSY: CMmSupplServTsy::CompleteNotifyIccMessageWaitingIndicatorChange, error:%{TSymbianErrorCodes}", aErrorCode );
    
    if ( IsMwisInfoChanged( *aMessageWaiting ) )
        {
        if ( KErrNone == aErrorCode )
            {
            // copy the new value to the member variable
            iMessageWaiting = *aMessageWaiting;
            
            // update central repository
            aErrorCode = WriteIccMessageWaitingIndicatorsToCentRep( 
                iMessageWaiting );
            }
        // CPHS is used by the SIM, supports only display status indications
        else if ( KErrCPHSInUseBySIM == aErrorCode )
            {
            // change the error code; CPHS case only, not an error
            aErrorCode = KErrNone; 
            
            if ( aMessageWaiting->iDisplayStatus != 
                iMessageWaiting.iDisplayStatus )
                {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETENOTIFYICCMESSAGEWAITINGINDICATORCHANGE_2, "TSY: CMmSupplServTsy::CompleteNotifyIccMessageWaitingIndicatorChange, CPHS information changed status=%d ", aMessageWaiting->iDisplayStatus);
                // update only CPHS supported information
                iMessageWaiting.iDisplayStatus = 
                    aMessageWaiting->iDisplayStatus;
                 
                // update central repository, only display status
                aErrorCode = iMwisCentRep->Set( 
                    KCtsyMessageWaitingDisplayStatus, 
                    aMessageWaiting->iDisplayStatus );
                }
            }
        
        // reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->
            ResetTsyReqHandle( CMmPhoneTsy::
            EMultimodePhoneNotifyIccMessageWaitingIndicatorsChange );

        if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
            {            
            // copy the altered information to the client side
            *iRetNotifyMessageWaiting = iMessageWaiting;
            iRetNotifyMessageWaiting = NULL;
            
            // complete the client request
            iMmPhone->ReqCompleted( reqHandle, aErrorCode );
            }
        }     
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyIccMessageWaitingIndicatorChangeCancel
// Use this method to cancel a previously placed asynchronous 
// NotifyIccMessageWaitingIndicatorChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyIccMessageWaitingIndicatorChangeCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_NOTIFYICCMESSAGEWAITINGINDICATORCHANGECANCEL_1, "TSY: CMmSupplServTsy::NotifyIccMessageWaitingIndicatorChangeCancel");
    
    // Null the pointer to client-side data
    iRetNotifyMessageWaiting = NULL;  
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyIccMessageWaitingIndicatorsChange ); 
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
    
    return KErrNone; 
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetIccSupportForMWIS
// Use this method to set the ICC support for Message Waiting indicators.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::SetIccSupportForMWIS( TBool aSupport )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETICCSUPPORTFORMWIS_1, "TSY: CMmSupplServTsy::SetIccSupportForMWIS, support%d ", aSupport );
    
    iMessageWaitingSupportedByIcc = aSupport; 
    }
    
// ---------------------------------------------------------------------------
// CMmSupplServTsy::ReadIccMessageWaitingFromCentRep
// Read the EF-MWIS infromation from the central repository
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::ReadIccMessageWaitingIndicatorsFromCentRep( 
    RMobilePhone::TMobilePhoneMessageWaitingV1& aMessageWaiting )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_READICCMESSAGEWAITINGINDICATORSFROMCENTREP_1, "TSY: CMmSupplServTsy::ReadIccMessageWaitingIndicatorsFromCentRep");
    
    TInt retval( KErrNone );
    TInt tempVal( 0 );
    
    // Read values from the central repository and check that it is positive           
    retval = iMwisCentRep->Get( KCtsyMessageWaitingDisplayStatus, tempVal );
    ( tempVal > 0 ) ? 
        aMessageWaiting.iDisplayStatus = tempVal :
        aMessageWaiting.iDisplayStatus = 0;    
      
    retval = iMwisCentRep->Get( KCtsyMessageWaitingVoiceMailCount, 
        tempVal );
    ( tempVal > 0 ) ? 
        aMessageWaiting.iVoiceMsgs = tempVal :
        aMessageWaiting.iVoiceMsgs = 0;
    
    retval = iMwisCentRep->Get( KCtsyMessageWaitingAuxLineCount, tempVal );
    ( tempVal > 0 ) ? 
        aMessageWaiting.iAuxVoiceMsgs = tempVal :
        aMessageWaiting.iAuxVoiceMsgs = 0;
   
    retval = iMwisCentRep->Get( KCtsyMessageWaitingDataCount, tempVal );
    ( tempVal > 0 ) ? 
        aMessageWaiting.iDataMsgs = tempVal :
        aMessageWaiting.iDataMsgs = 0;
    
    retval = iMwisCentRep->Get( KCtsyMessageWaitingFaxCount, tempVal );
    ( tempVal > 0 ) ? 
        aMessageWaiting.iFaxMsgs = tempVal :
        aMessageWaiting.iFaxMsgs = 0;
     
    retval = iMwisCentRep->Get( KCtsyMessageWaitingEmailCount, tempVal );
    ( tempVal > 0 ) ? 
        aMessageWaiting.iEmailMsgs = tempVal :
        aMessageWaiting.iEmailMsgs = 0;

    retval = iMwisCentRep->Get( KCtsyMessageWaitingOtherMessageCount,
        tempVal );
    ( tempVal > 0 ) ? 
        aMessageWaiting.iOtherMsgs = tempVal :
        aMessageWaiting.iOtherMsgs = 0;
       
    return retval;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep
// Write the EF-MWIS infromation to the central repository
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep( 
    const RMobilePhone::TMobilePhoneMessageWaitingV1& aMessageWaiting )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_WRITEICCMESSAGEWAITINGINDICATORSTOCENTREP_1, "TSY: CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep");
    
    TInt retval( KErrNone );
	
	// Write new values to the central repository       
    retval = iMwisCentRep->Set( KCtsyMessageWaitingDisplayStatus, 
        aMessageWaiting.iDisplayStatus );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_WRITEICCMESSAGEWAITINGINDICATORSTOCENTREP_2, "TSY: CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep : iDisplayStatus:%u", aMessageWaiting.iDisplayStatus );
    retval = iMwisCentRep->Set( KCtsyMessageWaitingVoiceMailCount,
        aMessageWaiting.iVoiceMsgs );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_WRITEICCMESSAGEWAITINGINDICATORSTOCENTREP_3, "TSY: CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep : iVoiceMsgs:%u", aMessageWaiting.iVoiceMsgs );
    retval = iMwisCentRep->Set( KCtsyMessageWaitingAuxLineCount,
        aMessageWaiting.iAuxVoiceMsgs );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_WRITEICCMESSAGEWAITINGINDICATORSTOCENTREP_4, "TSY: CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep : iAuxVoiceMsgs:%u", aMessageWaiting.iAuxVoiceMsgs );
    retval = iMwisCentRep->Set( KCtsyMessageWaitingDataCount,
        aMessageWaiting.iDataMsgs );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_WRITEICCMESSAGEWAITINGINDICATORSTOCENTREP_5, "TSY: CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep : iDataMsgs:%u", aMessageWaiting.iDataMsgs );
    retval = iMwisCentRep->Set( KCtsyMessageWaitingFaxCount,
        aMessageWaiting.iFaxMsgs );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_WRITEICCMESSAGEWAITINGINDICATORSTOCENTREP_6, "TSY: CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep : iFaxMsgs:%u", aMessageWaiting.iFaxMsgs );
    retval = iMwisCentRep->Set( KCtsyMessageWaitingEmailCount,
        aMessageWaiting.iEmailMsgs );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_WRITEICCMESSAGEWAITINGINDICATORSTOCENTREP_7, "TSY: CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep : iEmailMsgs:%u", aMessageWaiting.iEmailMsgs );
    retval = iMwisCentRep->Set( KCtsyMessageWaitingOtherMessageCount,
        aMessageWaiting.iOtherMsgs );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_WRITEICCMESSAGEWAITINGINDICATORSTOCENTREP_8, "TSY: CMmSupplServTsy::WriteIccMessageWaitingIndicatorsToCentRep : iOtherMsgs:%u", aMessageWaiting.iOtherMsgs );

    return retval;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::IsMwisInfoChanged
// Return ETrue if the MWIS information has been changed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmSupplServTsy::IsMwisInfoChanged( 
    const RMobilePhone::TMobilePhoneMessageWaitingV1& aNewMessageWaiting )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_ISMWISINFOCHANGED_1, "TSY: CMmSupplServTsy::IsMwisInfoChanged");
    
    TBool retval( ETrue ); 
    
    if( aNewMessageWaiting.iDisplayStatus == iMessageWaiting.iDisplayStatus &&
        aNewMessageWaiting.iVoiceMsgs == iMessageWaiting.iVoiceMsgs &&
        aNewMessageWaiting.iAuxVoiceMsgs == iMessageWaiting.iAuxVoiceMsgs &&
        aNewMessageWaiting.iDataMsgs == iMessageWaiting.iDataMsgs &&
        aNewMessageWaiting.iFaxMsgs == iMessageWaiting.iFaxMsgs &&
        aNewMessageWaiting.iEmailMsgs == iMessageWaiting.iEmailMsgs &&
        aNewMessageWaiting.iOtherMsgs == iMessageWaiting.iOtherMsgs )
        {
        retval = EFalse; 
        }
        
    return retval; 
    }
// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteSendNetworkServiceRequestNoFdnCheck
// Completes an outstanding SendNetworkServiceRequest with no FDN check 
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteSendNetworkServiceRequestNoFdnCheck(
    TInt aErrorCode ) 
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneSendNetworkServiceRequestNoFdnCheck );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        iMmPhone->ReqCompleted( reqHandle, aErrorCode );
        iSsTransactionOngoing = EFalse;
        iSsNoFdnCheckFlag = ESsNoFdnCheckUnknown; 
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifySendNetworkServiceRequest
// This method activates notifying of a send network request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifySendNetworkServiceRequest(
    const RMobilePhone::TMobilePhoneNotifySendSSOperation* aOperation,
    TDes8* aSendSSRequestPckg )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_NOTIFYSENDNETWORKSERVICEREQUEST_1, "TSY: CMmSupplServTsy::NotifySendNetworkServiceRequest" );
 
	if(sizeof(RMobilePhone::TMobilePhoneSendSSRequestV3) > aSendSSRequestPckg->MaxLength())
		{
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_NOTIFYSENDNETWORKSERVICEREQUEST_2, "TSY: CMmNetTsy::NotifySendNetworkServiceRequest Bad size argument");
	    // Complete the request with appropiate error        
	    return KErrArgument;
		}

    TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3>* ssDataPckg = 
        reinterpret_cast<TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3>*>( aSendSSRequestPckg );

    RMobilePhone::TMobilePhoneSendSSRequestV3& ssData = ( *ssDataPckg )();   
    iInputSSOperation = *aOperation;
    
    //save pointer to client data
    iRetSSDataPtrIndRequest = &ssData;

 	// Save tsy req handle type
    iMmPhone->iReqHandleType = CMmPhoneTsy::EMultimodePhoneNotifySendNetworkServiceRequest;
        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteSendNetworkServiceRequestInd
// This method completes notify network service request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteSendNetworkServiceRequestInd(
    TInt aErrorCode, CMmDataPackage* aDataPackage  )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETESENDNETWORKSERVICEREQUESTIND_1, "TSY: CMmSupplServTsy::CompleteSendNetworkServiceRequestInd. Error: %{TSymbianErrorCodes}", aErrorCode );
  
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
		CMmPhoneTsy::EMultimodePhoneNotifySendNetworkServiceRequest );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // Check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // Unpack data to client side pointers
            aDataPackage->UnPackData ( *iRetSSDataPtrIndRequest );
        
            if (iRetSSDataPtrIndRequest->iOpCode == iInputSSOperation)
            	{
            	// Complete the client request   	
            	iMmPhone->ReqCompleted( reqHandle, KErrNone );
            	}
#ifdef USING_CTSY_DISPATCHER
            //CTSY defect, if a complete is called with a different op code, then the 
            //original request can never be completed as the request handle is taken
            //out of the store at the start of the function, here we simply put it back in
            //as a quick fix to minimise if defs.
            else
            	{
            	iMmPhone->iTsyReqHandleStore->SetTsyReqHandle(CMmPhoneTsy::EMultimodePhoneNotifySendNetworkServiceRequest,reqHandle);
            	}
#endif //USING_CTSY_DISPATCHER
            }
        else
            {
            // Complete error value
            iMmPhone->ReqCompleted( reqHandle, aErrorCode );
            } 
        }
    }

// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifySendNetworkRequestCancel()
// Cancels notify send network service request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifySendNetworkServiceRequestCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_NOTIFYSENDNETWORKSERVICEREQUESTCANCEL_1, "TSY: CMmSupplServTsy::NotifySendNetworkServiceRequestCancel");

    // Clear data pointers        
	iRetSSDataPtrIndRequest = NULL;
    
    // Reset the req handle
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifySendNetworkServiceRequest ); 
                                     
    // Complete request with cancel    
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
  
    return KErrNone; 
    }
  
// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyAllSendNetworkServiceRequest
// This method activates notifying of network release
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyAllSendNetworkServiceRequest(
    RMobilePhone::TMobilePhoneNotifySendSSOperation* aOperation,
    TDes8* aRequestComplete )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_NOTIFYALLSENDNETWORKSERVICEREQUEST_1, "TSY: CMmSupplServTsy::NotifyAllSendNetworkRequest" );
 
    TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3>* ssDataPckg = 
        reinterpret_cast<TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3>*>
        ( aRequestComplete );

    RMobilePhone::TMobilePhoneSendSSRequestV3& ssData = 
        ( *ssDataPckg )();
           
    //save pointer to client data
    iRetSSDataPtrAllRequest = &ssData;
    
    // aOperation type 
    iRetSSOperationPtr = aOperation;    
 
    // set req handle
    iMmPhone->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyAllSendNetworkServiceRequest;
   
    return KErrNone;
    }



// ---------------------------------------------------------------------------
// CMmSupplServTsy::CompleteNotifyAllSendNetworkServiceRequest
// This method completes notify network service request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::CompleteNotifyAllSendNetworkServiceRequest(
    TInt aErrorCode, CMmDataPackage* aDataPackage  )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_COMPLETENOTIFYALLSENDNETWORKSERVICEREQUEST_1, "TSY: CMmSupplServTsy::CompleteNotifyAllSendNetworkRequest. Error: %{TSymbianErrorCodes}", aErrorCode );
   
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
		CMmPhoneTsy::EMultimodePhoneNotifyAllSendNetworkServiceRequest );

    if( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // Check if response was ok
        if ( KErrNone == aErrorCode )
            {
            // Unpack data to client side pointers
            aDataPackage->UnPackData ( *iRetSSOperationPtr, *iRetSSDataPtrAllRequest );
        
            // Complete the client request
            iMmPhone->ReqCompleted( reqHandle, KErrNone );
            }
        else
            {
            // Complete error value
            iMmPhone->ReqCompleted( reqHandle, aErrorCode );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CMmSupplServTsy::NotifyAllSendNetworkRequestCancel()
// Cancels notify all send network service request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSupplServTsy::NotifyAllSendNetworkServiceRequestCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_NOTIFYALLSENDNETWORKSERVICEREQUESTCANCEL_1, "TSY: CMmSupplServTsy::NotifyAllSendNetworkServiceRequestCancel");

    // Clear data pointers        
    iRetSSOperationPtr = NULL;
    iRetSSDataPtrAllRequest = NULL;
    
    // Reset the req handle
    iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyAllSendNetworkServiceRequest ); 
                                     
    // Compelete request with cancel    
    iMmPhone->ReqCompleted( aTsyReqHandle, KErrCancel );
   
    return KErrNone; 
    }
    
// ---------------------------------------------------------------------------
// CMmSupplServTsy::SetIccMwisBootUpFlag
// Set new value for ICC MWIS bootup.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSupplServTsy::SetIccMwisBootUpFlag( TBool aBootUp )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSUPPLSERVTSY_SETICCMWISBOOTUPFLAG_1, "TSY: CMmSupplServTsy::SetIccMwisBootUpFlag, aBootUp=%d", aBootUp );

    iGetIccSupportAtBoot = aBootUp;
    }    
    
//  End of File  
