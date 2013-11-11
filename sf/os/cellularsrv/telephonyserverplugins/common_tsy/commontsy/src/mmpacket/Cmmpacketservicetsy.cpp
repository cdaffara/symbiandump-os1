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
#include "CmmpacketservicetsyTraces.h"
#endif

#include "Cmmpacketservicetsy.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "MmTsy_timeoutdefs.h"
#include "MmTsy_numberOfSlots.h"
#include "CMmCommonStaticUtility.h"
#include "Cmmpacketcontextlist.h"
#include "cmmpacketservicegsmwcdmaext.h"
#include "cmmcalllist.h"
#include "cmmcalltsy.h"
#include "CMmCustomTsy.h"
#include "CMmPacketTsy.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

#include "cmmmbmscontextlist.h"
#include "cmmmbmsmonitoredlist.h"

// ======== MEMBER FUNCTIONS ========

CMmPacketServiceTsy::CMmPacketServiceTsy():
    iReqHandleType(EMultimodePacketServiceReqHandleUnknown)
    {
    }

CMmPacketServiceTsy* CMmPacketServiceTsy::NewL(
        CMmPhoneTsy* const aMmPhone,           
        CMmMessageManagerBase* const aMessageManager,
        MLtsyFactoryBase* aFactory  ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NEWL_1, "TSY: CMmPacketServiceTsy::NewL" );
    CMmPacketServiceTsy* const packetService =
        new ( ELeave ) CMmPacketServiceTsy();

    CleanupClosePushL( *packetService );
    packetService->iMmPhone = aMmPhone;
	packetService->iMessageManager = aMessageManager;
    packetService->ConstructL( aFactory );
    CleanupStack::Pop();

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NEWL_2, "TSY: CMmPacketServiceTsy::NewL." );
    return packetService;

    }

void CMmPacketServiceTsy::ConstructL( MLtsyFactoryBase* aFactory )
    {

    InitModulesL( aFactory );

    InitStoresAndListsL();

	iMessageManager->RegisterTsyObject( CMmMessageManagerBase::EPacketDataTsy,
        this  );

    // Current Network Mode is initialized to be EMSClassUnknown
    iCurrentMode = RMobilePhone::ENetworkModeGsm;

    }

CMmPacketServiceTsy::~CMmPacketServiceTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_DTOR_1, "TSY: CMmPacketServiceTsy::~CMmPacketServiceTsy" );

    if ( iMmPhone )
        {
        // deregister tsy object from message manager
        iMmPhone->MessageManager()->DeregisterTsyObject(this);
        
        // Reset packet data session pointer in phone
        iMmPhone->ResetPacketDataSession();
        }

    // Delete CMmTsyReqHandleStore object
    delete iTsyReqHandleStore;
    // Delete CMmPacketContextList object
    delete iPacketContextList;
    // Delete CMmPacketServiceGsmWcdmaExt object
    delete iMmPacketServiceGsmWcdmaExt;
    if(iMBMSContextList != NULL)
    	{
    	// Delete CMmMBMSContextList object
    	delete iMBMSContextList;
    	}
    if(iGetMbmsMonitoredServiceLists != NULL)
    	{
    	// Delete CArrayPtrFlat object
    	delete iGetMbmsMonitoredServiceLists;
    	}    

    if ( NULL != iDialUpContext )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_DTOR_2, "TSY: CMmPacketServiceTsy::~CMmPacketServiceTsy: Close Dial-up Object." );

        iDialUpContext->Close();
        }

    if ( NULL != iSecondaryDialUpContext )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_DTOR_3, "TSY: CMmPacketServiceTsy::~CMmPacketServiceTsy: Close Secondary Dial-up Object." );

        iSecondaryDialUpContext->Close();
        }
        
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_DTOR_4, "TSY: CMmPacketServiceTsy::~CMmPacketServiceTsy: Closing iMutex Object." );
    iMutex.Close();

#ifdef USING_CTSY_DISPATCHER
    //CTSY defect (memory leak) this maybe allocated, but it is never deleted.
    //no need for NULL check as C++ guarantees that operator delete checks its argument for null-ness.
    delete iMBMSMonitoredList;
    iMBMSMonitoredList = NULL;
#endif //USING_CTSY_DISPATCHER    
    }
    

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::InitModulesL
// Initialises extension modules for CMmPacketServiceTsy.
// if GSM flag is defined
// allocate memory for CMmPacketServiceGsmWcdmaExt object
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::InitModulesL( MLtsyFactoryBase* aFactory )
    {
    // Create Packet Service Gsm extension
    iMmPacketServiceGsmWcdmaExt =
        CMmPacketServiceGsmWcdmaExt::NewL( this, aFactory );
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::InitStoresAndListsL
// Initialises needed stores and lists.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::InitStoresAndListsL()
    {

#ifdef REQHANDLE_TIMER
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this, iMmPhone,
        ENumOfMultimodePacketServiceRequests, iPacketServiceReqHandles );
#else
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL(
        ENumOfMultimodePacketServiceRequests, iPacketServiceReqHandles );

#endif // REQHANDLE_TIMER

    // Create Packet Context List
    iPacketContextList = CMmPacketContextList::NewL( this );

    // create mbms context list 
    iMBMSContextList = CMmMBMSContextList::NewL();
    
    // create MBMS monitored service list
    iGetMbmsMonitoredServiceLists = new CArrayPtrFlat<CListReadAllAttempt>(1);
    
            
    // Set pointer to externally created dial-up context to be NULL
    iDialUpContext = NULL;
    
    // Set pointer to externally created dial-up context to be NULL
    iSecondaryDialUpContext = NULL;

    // Set phone not attached to the network
    iInternalSavedData.iServiceStatus = RPacketService::EStatusUnattached;

    // Current Mobile Station Class is initialized to be EMSClassUnknown
    iInternalSavedData.iMsClass = RPacketService::EMSClassUnknown;

    // If Gpds is suspended this flag is ETrue, otherwise EFalse
    iSuspended = EFalse;

    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::Init
// Initialisation method that is called from ETel Server.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::Init()
    {
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CustomTsy
// Returns a pointer to Custom TSY object.
// return custom tsy owned by phone tsy
//----------------------------------------------------------------------------
//
CMmCustomTsy* CMmPacketServiceTsy::CustomTsy() const
    {
    // Return custom tsy
    return iMmPhone->GetCustomTsy();
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::ActivePacketServiceExtension
// Returns a pointer to the active packet service extension.
// The packet data extension implements functionality that is specific for
// selected mode. There should always be some extension active.
//----------------------------------------------------------------------------
//
CMmPacketServiceGsmWcdmaExt* CMmPacketServiceTsy::ActivePacketServiceExtension()
    {
    // Return active packet service extension
    return iMmPacketServiceGsmWcdmaExt;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::TsyReqHandleStore
// Returns a pointer to the tsy request handle store.
// return tsy request handle store
//----------------------------------------------------------------------------
//
CMmTsyReqHandleStore* CMmPacketServiceTsy::TsyReqHandleStore()
    {
    // Return tsy request handle store
    return iTsyReqHandleStore;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::IsContextStatusActive
// Checks if context status is active from service point of view
//----------------------------------------------------------------------------
//
TBool CMmPacketServiceTsy::IsContextStatusActive(
    const RPacketContext::TContextStatus aContextStatus  ) const
    {

    if ( ( RPacketContext::EStatusActive == aContextStatus ) ||
         ( RPacketContext::EStatusDeactivating == aContextStatus ) )
        {
        // Context is active from the service point of view
        return ETrue;
        }

    return EFalse;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::UpdateAttachedPacketStatus
// Sets service status to active or attached and completes status changed
// notification to the client if needed.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::UpdateAttachedPacketStatus(
    const TBool aIsActiveContext ) //is context active
    {
    TBool isActive( aIsActiveContext );
    
    // If external context exist and there are no active internal context
    if ( iDialUpContext != NULL && isActive == EFalse )
        {
        // If external context is active then active context found
        isActive = IsContextStatusActive( iDialUpContext->ContextStatus() );
        } 

    // If external context exist and there are no active internal context
    if ( iSecondaryDialUpContext != NULL && isActive == EFalse )
        {
        // If external context is active then active context found
        isActive = IsContextStatusActive( 
            iSecondaryDialUpContext->ContextStatus() );
        }

    if ( isActive && ( RPacketService::EStatusActive !=
         iInternalSavedData.iServiceStatus ) )
        {
        // Complete status change notification to the client and
        // set service status EStatusActive
        CMmPacketServiceTsy::CompleteNotifyStatusChange(
		    RPacketService::EStatusActive, KErrNone );

        }
    else if ( !isActive && ( RPacketService::EStatusAttached !=
              iInternalSavedData.iServiceStatus ) )
        {

        // Complete status change notification to the client and
        // set service status EStatusAttached
		CMmPacketServiceTsy::CompleteNotifyStatusChange(
		    RPacketService::EStatusAttached, KErrNone );

        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_UPDATEATTACHEDPACKETSTATUS_1, "TSY: CMmPacketServiceTsy::UpdateAttachedPacketStatus. Service Status: %d", iInternalSavedData.iServiceStatus );
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompletePacketStatusChanged
// Called from active extension to notify service that transfer status
// is changed.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompletePacketStatusChanged(
    CMmDataPackage* aDataPackage,
    TInt aErrorCode )
    {
	RPacketService::TStatus packetStatus;
	TBool isresumed;
	TBool isActiveContext( EFalse );

	//Pack data
	aDataPackage->UnPackData( packetStatus, isresumed  );

    // Get number of created contexts
    const TInt numberOfContext( iPacketContextList->NumberOfContexts() );

    if ( isresumed )
        {
        // If current status is suspended
        if ( iSuspended )
            {

            CMmPacketContextTsy* packetContext;
            CMmMBMSContextTsy* mbmsContext;

            // check pdp context's
            for ( TInt i = 0; i < numberOfContext; i++ )
                {
                // Get context by index from context list
                packetContext = iPacketContextList->PacketContextByIndex( i );

                if ( NULL != packetContext )
                    {
                    // Inform context that service resumed from suspend
                    packetContext->ContextResumed();
                    }
                }
                
            // check mbms context's
            for ( TInt i = 0; i < iMBMSContextList->NumberOfContexts(); i++ )
                {
                // Get context by index from context list
                mbmsContext = iMBMSContextList->ContextByIndex( i );

                if ( NULL != mbmsContext )
                    {
                    // Inform context that service resumed from suspend
                    mbmsContext->ContextResumed();
                    }
                }                

            if ( iDialUpContext != NULL )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEPACKETSTATUSCHANGED_1, "TSY: CMmPacketServiceTsy::CompletePacketStatusChanged.  DialUp Context Resumed" );

                iDialUpContext->ContextResumed();
                }

            if ( iSecondaryDialUpContext != NULL )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEPACKETSTATUSCHANGED_2, "TSY: CMmPacketServiceTsy::CompletePacketStatusChanged. Secondary DialUp Context Resumed" );

                iSecondaryDialUpContext->ContextResumed();
                }

            // Get service status before suspend
            packetStatus = iInternalSavedData.iServiceStatusBeforeSuspend;
            // Current status is not suspended
            iSuspended = EFalse;

            }
        else
            {
            // Resumed without suspend, continue with current status
            packetStatus = iInternalSavedData.iServiceStatus;
            }
        }

    // If current status is not suspended or
    // current status is suspended and new status is unattached
    if ( !iSuspended || ( RPacketService::EStatusUnattached == packetStatus ) )
        {
        switch ( packetStatus )
            {
            case RPacketService::EStatusUnattached:
                if ( iSuspended )
                    {
                    // If previous status was suspended it is not valid 
                    // anymore
                    iSuspended = EFalse;
                    }

                // Complete status change notification with EStatusUnattached
                CMmPacketServiceTsy::CompleteNotifyStatusChange(
				    RPacketService::EStatusUnattached, aErrorCode );

                break;
            case RPacketService::EStatusAttached:
            case RPacketService::EStatusActive:
                CMmPacketContextTsy* packetContext;

                for ( TInt i = 0; i < numberOfContext; i++ )
                    {
                    packetContext =
                        iPacketContextList->PacketContextByIndex( i );

                    // If no active context found yet
                    if ( NULL != packetContext )
                        {
                        // Get context status
                        RPacketContext::TContextStatus contextStatus =
                            packetContext->ContextStatus();

                        // Check if context is active ( from service point 
                        // of view )
                        isActiveContext =
                            IsContextStatusActive( contextStatus );
                        }
                    if ( isActiveContext )
                        {
                        // Active context found, break
                        i = numberOfContext;
                        }
                    }
                    
                // no active pdp context's, check if any mbms context is active
                if( !isActiveContext )
                    {
                    CMmMBMSContextTsy* mbmsContext;
                    
                    // check mbms context's
                    for ( TInt i = 0; i < iMBMSContextList->NumberOfContexts(); i++ )
                        {
                        // Get context by index from context list
                        mbmsContext = iMBMSContextList->ContextByIndex( i );

                        if ( NULL != mbmsContext )
                            {
                            // Get context status
                            RPacketContext::TContextStatus contextStatus =
                                mbmsContext->ContextStatus();

                            // Check if context is active ( from service point 
                            // of view )
                            isActiveContext =
                                IsContextStatusActive( contextStatus );
                            }
                        if ( isActiveContext )
                            {
                            // Active context found, break
                            i = iMBMSContextList->NumberOfContexts();                                                   
                            }  
                        }
                    }

                // Update service status
                UpdateAttachedPacketStatus( isActiveContext );

                break;
            case RPacketService::EStatusSuspended:
                if ( !iSuspended && ( RPacketService::EStatusUnattached !=
                     iInternalSavedData.iServiceStatus ) )
                    {
                    // Save status before suspend internally
                    iInternalSavedData.iServiceStatusBeforeSuspend =
                        iInternalSavedData.iServiceStatus;

                    // Set iSuspended to be ETrue
                    iSuspended = ETrue;

                    CMmPacketContextTsy* packetContext;

                    for ( TInt i = 0; i < numberOfContext; i++ )
                        {
                        // Get next context
                        packetContext =
                            iPacketContextList->PacketContextByIndex( i );

                        if ( NULL != packetContext )
                            {
                            // Inform context that service is suspend
                            packetContext->ContextSuspended();
                            }
                        }
                    CMmMBMSContextTsy* mbmsContext;
                    
                    // check mbms context's
                    for ( TInt i = 0; i < iMBMSContextList->NumberOfContexts(); i++ )
                        {
                        // Get context by index from context list
                        mbmsContext = iMBMSContextList->ContextByIndex( i );

                        if ( NULL != mbmsContext )
                            {
                            // Inform context that service is suspend
                            mbmsContext->ContextSuspended();
                            }
                        }                       

                    if ( iDialUpContext != NULL )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEPACKETSTATUSCHANGED_3, "TSY: CMmPacketServiceTsy::CompletePacketStatusChanged.  DialUp Context Suspended" );
                        iDialUpContext->ContextSuspended();
                        }

                    if ( iSecondaryDialUpContext != NULL )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEPACKETSTATUSCHANGED_4, "TSY: CMmPacketServiceTsy::CompletePacketStatusChanged.  DialUp Context Suspended" );
                        iSecondaryDialUpContext->ContextSuspended();
                        }

                    // Complete status change notification with
                    // EStatusSuspended
                    CMmPacketServiceTsy::CompleteNotifyStatusChange(
                        RPacketService::EStatusSuspended, aErrorCode );

                    }

                break;
            default:
                break;
            }
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEPACKETSTATUSCHANGED_5, "TSY: CMmPacketServiceTsy::CompletePacketStatusChanged. Service Status: %d", iInternalSavedData.iServiceStatus );
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::ContextStatusChanged
// Context status is changed from the Service point of view.
// Called from CMmPacketContextTsy to notify service that context status is
// changed to inactive, activate or deleted state. These states indicate that
// context is changed to inactive or active state from the Packet Service
// point of view.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::ContextStatusChanged(
    const RPacketContext::TContextStatus aContextStatus )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_CONTEXTSTATUSCHANGED_1, "TSY: CMmPacketServiceTsy::ContextStatusChanged. Context Status: %d", aContextStatus );

    if ( !iSuspended && ( RPacketService::EStatusUnattached !=
         iInternalSavedData.iServiceStatus ) )
        {

        TBool isActiveContext( EFalse );

        if ( ( RPacketContext::EStatusInactive == aContextStatus ) ||
             ( RPacketContext::EStatusDeleted == aContextStatus ) )
            {
            const TInt numberOfContext(
                iPacketContextList->NumberOfContexts() );

            RPacketContext::TContextStatus contextStatus;

            for ( TInt i = 0; i < numberOfContext; i++ )
                {
                TInt ret( iPacketContextList->GetContextStatusByIndex(
                    i, contextStatus ) );

                if ( KErrNone == ret )
                    {
                    isActiveContext = IsContextStatusActive( contextStatus );

                    if ( isActiveContext )
                        {
                        // Active context found, break
                        i = numberOfContext;
                        }
                    }
                }
            }

        else if ( RPacketContext::EStatusActive == aContextStatus )
            {

            isActiveContext = ETrue;

            }

        UpdateAttachedPacketStatus( isActiveContext );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::PreferredBearerChanged
// Preferred bearer has been changed save new
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::PreferredBearerChanged(
    CMmDataPackage* aPackage )      
    {
	RPacketService::TPreferredBearer preferredBearer;

	aPackage->UnPackData( preferredBearer );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_PREFERREDBEARERCHANGED_1, "TSY: CMmPacketServiceTsy::PreferredBearerChanged. PreferredBearer: %d", preferredBearer );

    iInternalSavedData.iPreferredBearer = preferredBearer;

    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::Attach
// Starts asynchronous attach request to the network.
// This function will cause the phone to attempt an attach to the packet
// network, if it is not already attached.  If it is, the error
// KErrAlreadyExists should be returned.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::AttachL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_ATTACHL_1, "TSY: CMmPacketServiceTsy::AttachL." );

    TInt ret( KErrAlreadyExists );

    if ( RPacketService::EStatusUnattached ==
         iInternalSavedData.iServiceStatus )
        {

        const TTsyReqHandle attachHandle( iTsyReqHandleStore->GetTsyReqHandle(
            EMultimodePacketServiceAttach ) );

        if ( EMultimodePacketServiceReqHandleUnknown != attachHandle )
            {
            ret = KErrServerBusy;
            }
        else
            {
            // Attach
            ret = iMmPacketServiceGsmWcdmaExt->AttachL();

            if ( KErrNone == ret )
                {
                // Set request handle
                iReqHandleType = EMultimodePacketServiceAttach;
                }
            else
                {
                // Sending to the phonet failed for some reason
                ret = CMmCommonStaticUtility::EpocErrorCode(
                    KErrGeneral, KErrGsmMMNetworkFailure );
                }
            }
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteAttach
// Completes asynchronous attach request.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteAttach(
    const TInt aError )  //error value
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEATTACH_1, "TSY: CMmPacketServiceTsy::CompleteAttach. Error Value: %{TSymbianErrorCodes}", aError );

    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceAttach ) );

    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        CMmPacketServiceTsy::ReqCompleted( reqHandle, aError );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::Detach
// Starts asynchronous detach request to the network.
// This function will cause the phone to attempt to detach from the packet
// network, if it is not already detached. If it is, the error
// KErrAlreadyExists should be returned. If at least one context is active the
// error KErrInUse should be returned.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::DetachL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_DETACHL_1, "TSY: CMmPacketServiceTsy::DetachL." );

    TInt ret( KErrAlreadyExists );

    if ( RPacketService::EStatusUnattached !=
        iInternalSavedData.iServiceStatus )
        {

        // Get number of created contexts
        const TInt numberOfContext( iPacketContextList->NumberOfContexts() );
        RPacketContext::TContextStatus contextStatus;

        for ( TInt i = 0; i < numberOfContext; i++ )
            {
            ret = iPacketContextList->GetContextStatusByIndex(
                i, contextStatus );

            if ( KErrNone == ret )
                {
                if ( IsContextStatusActive( contextStatus ) )
                    {
                    // Atleast one of the contexts is active.
                    ret = KErrInUse;
                    i= numberOfContext;
                    }
                }
            }

        // If not KErrInUse, no active contexts found and detach can be done
        if ( KErrInUse != ret )
            {

            const TTsyReqHandle reqHandle( iTsyReqHandleStore->
                GetTsyReqHandle( EMultimodePacketServiceDetach ) );

            if ( EMultimodePacketServiceReqHandleUnknown == reqHandle )
                {
                // Detach
                ret = iMmPacketServiceGsmWcdmaExt->DetachL();

                if ( KErrNone == ret )
                    {
                    // Set request handle
                    iReqHandleType = EMultimodePacketServiceDetach;
                    }
                else
                    {
                    // Sending to the phonet failed for some reason
                    ret = CMmCommonStaticUtility::EpocErrorCode(
                        KErrGeneral, KErrGsmMMNetworkFailure );
                    }
                }
            else
                {
                // The request is already in processing because of previous
                // request. Complete request with KErrServerBusy informing the
                // client about the situation.
                ret = KErrServerBusy;
                }
            }
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteDetachL
// Completes asynchronous detach request.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteDetachL(
    const TInt aError ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEDETACHL_1, "TSY: CMmPacketServiceTsy::CompleteDetachL. Error Value: %{TSymbianErrorCodes}", aError );

    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceDetach ) );

    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        CMmPacketServiceTsy::ReqCompleted( reqHandle, aError );
        }
    if ( RPacketService::EAttachWhenPossible == iAttachMode )
        {
        SetAttachModeL( &iAttachMode );
        iReqHandleType = EMultimodePacketServiceReqHandleUnknown;
        }
    }
    
//----------------------------------------------------------------------------
// CMmPacketServiceTsy::EnumerateContexts
// Enumerates contexts.
// This function will retrieve both the number of opened contexts in the TSY,
// the aCount parameter containing this value on completion of the function,
// and the maximum possible number of contexts, passed back in the aMaxAllowed
// parameter. Note that this is not the maximum number of simultaneously
// active contexts, but the total number allowed.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::EnumerateContexts(
    TInt* const aCount,    
    TInt* const aMaxAllowed ) 
    {
    *aCount = iPacketContextList->NumberOfContexts();
    *aMaxAllowed = KMmMaxNumberOfContexts;

OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_ENUMERATECONTEXTS_1, "TSY: CMmPacketServiceTsy::EnumerateContexts. Count: %d Max Allowed: %d", *aCount, *aMaxAllowed );
    // Complete the request with KErrNone
    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetAttachMode
// Retrieves the current mode of GPRS Attach operation.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetAttachModeL(
    RPacketService::TAttachMode* const aMode ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETATTACHMODEL_1, "TSY: CMmPacketServiceTsy::GetAttachModeL. Mode:%d", *aMode );

    TInt ret( KErrServerBusy );

    const TTsyReqHandle reqHandle( iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodePacketServiceGetAttachMode ) );

    if ( EMultimodePacketServiceReqHandleUnknown == reqHandle )
        {
        // Get attach mode
        ret = iMmPacketServiceGsmWcdmaExt->GetAttachModeL();

        if ( KErrNone == ret )
            {
            // Save pointer to aMode internally
            iTemporarySavedData.iGetAttachMode = aMode;
            // Set req handle
            iReqHandleType = EMultimodePacketServiceGetAttachMode;
            }
        }
    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteGetAttachMode
// Completes asynchronous attach request.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteGetAttachMode(
    const RPacketService::TAttachMode aAttachMode, TInt aResult ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEGETATTACHMODE_1, "TSY: CMmPacketServiceTsy::CompleteGetAttachMode. Attach Mode: %d", aAttachMode );

    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceGetAttachMode ) );

    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {
        *iTemporarySavedData.iGetAttachMode = aAttachMode;
        iTemporarySavedData.iGetAttachMode = NULL;

        // Complete the client request
        CMmPacketServiceTsy::ReqCompleted( reqHandle, aResult );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetContextInfo
// Gets Context info of the context defined by aIndex.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetContextInfo(
    const TInt* const aIndex,                 
    RPacketService::TContextInfo* const aInfo ) 
    {
    TInt ret( KErrNone );

    // Call ContextStatusByIndex() to set context status to aInfo->iStatus
    ret = iPacketContextList->GetContextStatusByIndex(
        *aIndex, aInfo->iStatus );

    if ( KErrNone == ret )
        {
        // Call GetContextNameByIndex() to set context name to aInfo->iName
        ret = iPacketContextList->GetContextNameByIndex(
            *aIndex, aInfo->iName );

OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETCONTEXTINFO_1, "TSY: CMmPacketServiceTsy::GetContextInfo. Context Status: %d Context Name: %S", aInfo->iStatus, aInfo->iName );
        }

    // Complete request with ret
    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, ret );
    
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetDynamicCaps
// Returns the dynamic capabilities of the phone.
// call active extension to get dynamic capabilities to aCaps
// complete request with KErrNone
// return KErrNone
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetDynamicCaps(
    RPacketService::TDynamicCapsFlags* const aCaps ) 
    {
    // Call DynamicCaps() from extension
    iMmPacketServiceGsmWcdmaExt->GetDynamicCaps( aCaps );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETDYNAMICCAPS_1, "TSY: CMmPacketServiceTsy::GetDynamicCaps." );

    // Complete the request with KErrNone
    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetMSClass
// Gets current and maximum value of the Mobile Station Class.
// This function will retrieve both the current class and the highest class of
// the mobile station. The parameters aCurrentClass and aMaxClass may take one
// of the following values which are ordered in terms of class from highest to
// lowest:
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetMSClass(
    RPacketService::TMSClass* const aCurrentClass, 
    RPacketService::TMSClass* const aMaxClass )    
    {
    *aCurrentClass = iInternalSavedData.iMsClass;
    *aMaxClass = KMaxMsClass;
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETMSCLASS_1, "TSY: CMmPacketServiceTsy::GetMSClass. Current Class: %d Max Class: %d", *aCurrentClass, *aMaxClass );

    // Complete the request with KErrNone
    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetNtwkRegStatus
// Gets network registration status.
// This function allows the client to retrieve the current registration status
// of the phone on the packet network as defined by the enum
// TRegistrationStatus.
//------------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetNtwkRegStatusL(
    RPacketService::TRegistrationStatus* const aRegistrationStatus ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETNTWKREGSTATUSL_1, "TSY: CMmPacketServiceTsy::GetNtwkRegStatusL. Tsy Req Handle: %d", iTsyReqHandle );

    const TTsyReqHandle regHandle( iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodePacketServiceGetNtwkRegStatus ) );

    TInt ret( KErrServerBusy );

    if ( EMultimodePacketServiceReqHandleUnknown == regHandle )
        {

        // Get network registration status
        ret = iMmPacketServiceGsmWcdmaExt->GetNtwkRegStatusL();

        if ( KErrNone == ret )
            {
            iRegistrationStatus = aRegistrationStatus;
            // Set request handle type
            iReqHandleType = EMultimodePacketServiceGetNtwkRegStatus;
            }
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteGetNtwkRegStatus
// Completes get network registration status request.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteGetNtwkRegStatus(
    const RPacketService::TRegistrationStatus aRegistrationStatus,
    const TInt aError )                             
    {
    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceGetNtwkRegStatus ) );

    // If request handle ok
    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {

        if ( KErrNone == aError )
            {
            *iRegistrationStatus = aRegistrationStatus;
            iRegistrationStatus = NULL;
            }
        // Complete the client request
        CMmPacketServiceTsy::ReqCompleted( reqHandle, aError );

        }
    else
        {
        // Request was sent during initialisation from active extension
        CMmPacketServiceTsy::CompleteNotifyChangeOfNtwkRegStatus(
												aRegistrationStatus );
        }
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEGETNTWKREGSTATUS_1, "TSY: CMmPacketServiceTsy::CompleteGetNtwkRegStatus. TSY Req Handle: %d Registration Status: %d", (TInt)reqHandle, aRegistrationStatus );
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetPreferredBearer
// Returns preferred bearer.
// This function will retrieve the currently selected preferred bearer to
// attach to when in Class C operation. The TSY should cache this value, so
// the function should return quickly meaning there is no need for an
// asynchronous version.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetPreferredBearer(
    RPacketService::TPreferredBearer* const aBearer )   
    {
    *aBearer = iInternalSavedData.iPreferredBearer;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETPREFERREDBEARER_1, "TSY: CMmPacketServiceTsy::GetPreferredBearer. Preferred Bearer: %d", *aBearer );

    // Complete the request with KErrNone
    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetStaticCaps
// Retrieves those capabilities of the TSY that are constant.
// call active extension to set static capabilities to aCaps
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetStaticCaps(
    TUint* const aCaps,                       
    const RPacketContext::TProtocolType* const aPdpType) 
    {
    TInt ret = KErrNone;
    
    switch (*aPdpType)
        {
        case RPacketContext::EPdpTypeIPv4:
        case RPacketContext::EPdpTypeIPv6:
        case RPacketContext::EPdpTypeX25:
        case RPacketContext::EPdpTypePPP:
        case RPacketContext::EPdpTypeCDPD:
            iMmPacketServiceGsmWcdmaExt->GetStaticCaps( aCaps );
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETSTATICCAPS_1, "TSY: CMmPacketServiceTsy::GetStaticCaps." );
            
            // Complete the request with KErrNone
            CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, ret );
            break;
        default:
            ret = KErrArgument;
            break;
        }
    
    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetStatus
// Returns current status of the packet service.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetStatus(
    RPacketService::TStatus* const aPacketStatus )   
    {
    *aPacketStatus = iInternalSavedData.iServiceStatus;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETSTATUS_1, "TSY: CMmPacketServiceTsy::GetStatus. Service Status: %d", *aPacketStatus );

    // Complete the request with KErrNone
    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::NotifyChangeOfNtwkRegStatus
// Requests notification of network registration status change.
// Allows a client to be notified whenever a change in the status of the
// packet network registration is detected.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::NotifyChangeOfNtwkRegStatus(
    RPacketService::TRegistrationStatus* const aRegistrationStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NOTIFYCHANGEOFNTWKREGSTATUS_1, "TSY: CMmPacketServiceTsy::NotifyChangeOfNtwkRegStatus." );

    // Set request handle and save pointer to aRegistrationStatus
    iNotifyDataPointers.iChangeOfNtwkRegStatus = aRegistrationStatus;
    iReqHandleType =  EMultimodePacketServiceNotifyNtwkRegStatusChange;

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteNotifyChangeOfNtwkRegStatus
// Completes notify change of ntwk registration status to client.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteNotifyChangeOfNtwkRegStatus(
    const RPacketService::TRegistrationStatus aRegistrationStatus )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYCHANGEOFNTWKREGSTATUS_1, "TSY: CMmPacketServiceTsy::CompleteNotifyChangeOfNtwkRegStatus. Current reg Status: %d New reg Status: %d", iInternalSavedData.iRegistrationStatus, aRegistrationStatus );

    // If registration status changed
    if ( aRegistrationStatus != iInternalSavedData.iRegistrationStatus )
        {
        // Save new status internally
        iInternalSavedData.iRegistrationStatus = aRegistrationStatus;

        // Reset request handle. Returns the deleted request handle
        const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePacketServiceNotifyNtwkRegStatusChange ) );

        // If notification is requested by client
        if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
            {
            // Set aRegistrationStatus to the etel side memory
            *iNotifyDataPointers.iChangeOfNtwkRegStatus = aRegistrationStatus;
            iNotifyDataPointers.iChangeOfNtwkRegStatus = NULL;

            // Complete request with KErrNone
            CMmPacketServiceTsy::ReqCompleted( reqHandle, KErrNone );
            }
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteNotifyChangeOfNtwkRegStatus
// Completes notify change of ntwk registration status to client.
// Handles request coming from MessHandler
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteNotifyChangeOfNtwkRegStatus(
    CMmDataPackage* aDataPackage, TInt aResult )		
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYCHANGEOFNTWKREGSTATUS1_1, "TSY: CMmPacketServiceTsy::CompleteNotifyChangeOfNtwkRegStatus.");

	//Unpack necessary data from datapackage
	RPacketService::TRegistrationStatus registrationStatus;
	aDataPackage->UnPackData( registrationStatus );

    // If registration status changed
    if ( registrationStatus != iInternalSavedData.iRegistrationStatus )
        {

        // Save new status internally
        iInternalSavedData.iRegistrationStatus = registrationStatus;

        // Reset request handle. Returns the deleted request handle
        const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePacketServiceNotifyNtwkRegStatusChange ) );

        // If notification is requested by client
        if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
            {

            // Set registrationStatus to the etel side memory
            *iNotifyDataPointers.iChangeOfNtwkRegStatus = registrationStatus;
            iNotifyDataPointers.iChangeOfNtwkRegStatus = NULL;

            // Complete the client request
            CMmPacketServiceTsy::ReqCompleted( reqHandle, aResult );

            }
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::NotifyContextActivationRequested
// Requests notification of network requests context activation.
// This function will complete when the phone receives a request from the
// packet network to activate a PDP context. Depending on the type of
// network, this request may contain the PDP type requested, the PDP address
// for this context, and possibly the Access Point Name (APN) of the gateway
// to connect to.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::NotifyContextActivationRequested(
    TPacketDataConfigBase* const aContextParameters ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NOTIFYCONTEXTACTIVATIONREQUESTED_1, "TSY: CMmPacketServiceTsy::NotifyContextActivationRequested." );

    TInt ret = KErrNone;

    // If class of aContextParameters is TContextConfigGPRS
    if ( TPacketDataConfigBase::KConfigGPRS ==
         aContextParameters->ExtensionId() )
        {
        // Cast aContextParameters from TPacketDataConfigBase to
        // TContextConfigGPRS and save internally
        iNotifyDataPointers.iContextActivationRequested =
            ( RPacketContext::TContextConfigGPRS* ) aContextParameters;

        // Set request handle
        iReqHandleType =
            EMultimodePacketServiceNotifyContextActivationRequested;
        }
    // Else if class of aContextParameters is TContextConfigR99_R4
    else if ( TPacketDataConfigBase::KConfigRel99Rel4 ==
         aContextParameters->ExtensionId() )
        {
        // Cast aContextParameters from TPacketDataConfigBase to
        // TContextConfigR99_R4 and save internally
        iNotifyDataPointers.iContextActivationRequested99 =
            ( RPacketContext::TContextConfigR99_R4* ) aContextParameters;

        // Set request handle
        iReqHandleType =
            EMultimodePacketServiceNotifyContextActivationRequested99;
        }
    // Else if class of aContextParameters is TContextConfigR5
    else if ( TPacketDataConfigBase::KConfigRel5 ==
         aContextParameters->ExtensionId() )
        {
        // Cast aContextParameters from TPacketDataConfigBase to
        // TContextConfig_R5 and save internally
        iNotifyDataPointers.iContextActivationRequested5 =
            ( RPacketContext::TContextConfig_R5* ) aContextParameters;

        // Set request handle
        iReqHandleType =
            EMultimodePacketServiceNotifyContextActivationRequested5;           
        }                
    // Else class of aContextParameters is not supported
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteNotifyContextActivationRequested
// Completes context activation requested notification to client.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteNotifyContextActivationRequested(
    CMmDataPackage* aDataPackage )		//contains initial data
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYCONTEXTACTIVATIONREQUESTED_1, "TSY: CMmPacketServiceTsy::CompleteNotifyContextActivationRequested.");

	//Check if there is client request
	if ( EMultimodePacketServiceReqHandleUnknown != 
			CMmPacketServiceTsy::TsyReqHandleStore()->GetTsyReqHandle( 
				CMmPacketServiceTsy::
					EMultimodePacketServiceNotifyContextActivationRequested ) ||
		 EMultimodePacketServiceReqHandleUnknown != 
		  	CMmPacketServiceTsy::TsyReqHandleStore()->
              	GetTsyReqHandle( CMmPacketServiceTsy::
					EMultimodePacketServiceNotifyContextActivationRequested99 ) ||
		 EMultimodePacketServiceReqHandleUnknown != 
		 	CMmPacketServiceTsy::TsyReqHandleStore()->
              	GetTsyReqHandle( CMmPacketServiceTsy::
					EMultimodePacketServiceNotifyContextActivationRequested5 ) )						
		{
		//ETel API client may call RPacketService::NotifyContextActivationRequested
		//method using one of following parameter class: 
		//RPacketContext::TContextConfigGPRS, RPacketContext::TContextConfigR99_R4
		//or RPacketContext::TContextConfig_R5. LTSY may complete 
		//NotifyContextActivationRequested request using one of these parameter 
		//classes. CTSY has to complete RPacketService::NotifyContextActivationRequested
		//requests in all situations.
		
		//Copy data temporary from aDataPackage to 
		//RPacketContext::TContextConfig_R5
		
		RPacketContext::TContextConfig_R5 config_R5;
		TBool config_R5Filled = ETrue;
		
		//Before copying initialize GPRS specific fields
		TUint iPdpCompression = 0;
		RPacketContext::TAnonymousAccess iAnonymousAccessReqd = 
			RPacketContext::ENotApplicable;
		
		//Before copying reset R99_R4 and R5 specific fields
		config_R5.iPFI = RPacketContext::EBestEffort;																		
    	config_R5.iPdpHeaderCompression = RPacketContext::ENoHeaderCompression;     	    
    	config_R5.iPdpDataCompression = RPacketContext::ENoDataCompression;			
		
	
		TPacketDataConfigBase configBase;
		aDataPackage->UnPackData( configBase );
		if ( configBase.ExtensionId() == TPacketDataConfigBase::KConfigGPRS )
			{		
	    	RPacketContext::TContextConfigGPRS configGPRS;
	    	aDataPackage->UnPackData( configGPRS );
	    	
			config_R5.iPdpType =configGPRS.iPdpType;			
			config_R5.iAccessPointName = configGPRS.iAccessPointName;							
			config_R5.iPdpAddress = configGPRS.iPdpAddress;
			iPdpCompression = configGPRS.iPdpCompression;
			iAnonymousAccessReqd = configGPRS.iAnonymousAccessReqd;
			config_R5.iUseEdge = configGPRS.iUseEdge;
			config_R5.iProtocolConfigOption.iAuthInfo = 
				configGPRS.iProtocolConfigOption.iAuthInfo;
			config_R5.iProtocolConfigOption.iChallenge = 
				configGPRS.iProtocolConfigOption.iChallenge;
			config_R5.iProtocolConfigOption.iResponse = 
				configGPRS.iProtocolConfigOption.iResponse;
			config_R5.iProtocolConfigOption.iId = 
				configGPRS.iProtocolConfigOption.iId;
			config_R5.iProtocolConfigOption.iDnsAddresses = 
				configGPRS.iProtocolConfigOption.iDnsAddresses;
			config_R5.iProtocolConfigOption.iMiscBuffer = 
				configGPRS.iProtocolConfigOption.iMiscBuffer;				
			config_R5.iNWIContext = configGPRS.iNWIContext;
			}
		else if ( configBase.ExtensionId() == TPacketDataConfigBase::KConfigRel99Rel4 )
			{	
	    	RPacketContext::TContextConfigR99_R4 config_R99_R4;
	    	aDataPackage->UnPackData( config_R99_R4 );			
			
			config_R5.iPdpType = config_R99_R4.iPdpType;			
			config_R5.iAccessPointName = config_R99_R4.iAccessPointName;							
			config_R5.iPdpAddress = config_R99_R4.iPdpAddress;
			config_R5.iUseEdge = config_R99_R4.iUseEdge;
			config_R5.iProtocolConfigOption.iAuthInfo =
				config_R99_R4.iProtocolConfigOption.iAuthInfo;
			config_R5.iProtocolConfigOption.iChallenge =
				config_R99_R4.iProtocolConfigOption.iChallenge;
			config_R5.iProtocolConfigOption.iResponse =
				config_R99_R4.iProtocolConfigOption.iResponse;
			config_R5.iProtocolConfigOption.iId =
				config_R99_R4.iProtocolConfigOption.iId;
			config_R5.iProtocolConfigOption.iDnsAddresses =
				config_R99_R4.iProtocolConfigOption.iDnsAddresses;
			config_R5.iProtocolConfigOption.iMiscBuffer =
				config_R99_R4.iProtocolConfigOption.iMiscBuffer;								
			config_R5.iNWIContext = config_R99_R4.iNWIContext;
			config_R5.iPFI = config_R99_R4.iPFI;	
			}
		else if ( configBase.ExtensionId() == TPacketDataConfigBase::KConfigRel5 )
			{			
			//Just unpack to config_R5
	    	aDataPackage->UnPackData( config_R5 );
			}
		else
			{				
			//Unknown parameter class.
			config_R5Filled = EFalse;
			}
			
		//Copy data to clients and complete client requests
		
		TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
       		EMultimodePacketServiceNotifyContextActivationRequested );	
		if ( config_R5Filled &&
		     EMultimodePacketServiceReqHandleUnknown != reqHandle )
			{
			// Deliver R97/98 parameters to the ETel Packet API client
			iNotifyDataPointers.iContextActivationRequested->iPdpType =
				config_R5.iPdpType;			
			iNotifyDataPointers.iContextActivationRequested->iAccessPointName =
				config_R5.iAccessPointName;							
			iNotifyDataPointers.iContextActivationRequested->iPdpAddress =
				config_R5.iPdpAddress;
			iNotifyDataPointers.iContextActivationRequested->iPdpCompression =
				iPdpCompression;
			iNotifyDataPointers.iContextActivationRequested->iAnonymousAccessReqd =
				iAnonymousAccessReqd;
			iNotifyDataPointers.iContextActivationRequested->iUseEdge =
				config_R5.iUseEdge;
			iNotifyDataPointers.iContextActivationRequested->iProtocolConfigOption.iAuthInfo =
				config_R5.iProtocolConfigOption.iAuthInfo;
			iNotifyDataPointers.iContextActivationRequested->iProtocolConfigOption.iChallenge =
				config_R5.iProtocolConfigOption.iChallenge;
			iNotifyDataPointers.iContextActivationRequested->iProtocolConfigOption.iResponse =
				config_R5.iProtocolConfigOption.iResponse;
			iNotifyDataPointers.iContextActivationRequested->iProtocolConfigOption.iId =
				config_R5.iProtocolConfigOption.iId;
			iNotifyDataPointers.iContextActivationRequested->iProtocolConfigOption.iDnsAddresses =
				config_R5.iProtocolConfigOption.iDnsAddresses;
			iNotifyDataPointers.iContextActivationRequested->iProtocolConfigOption.iMiscBuffer =
				config_R5.iProtocolConfigOption.iMiscBuffer;				
			iNotifyDataPointers.iContextActivationRequested->iNWIContext =
				config_R5.iNWIContext;
			iNotifyDataPointers.iContextActivationRequested = NULL;
		
			// Complete request with KErrNone
			CMmPacketServiceTsy::ReqCompleted( reqHandle, KErrNone );
		    }		

		reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
       		EMultimodePacketServiceNotifyContextActivationRequested99 );	
		if ( config_R5Filled &&
		     EMultimodePacketServiceReqHandleUnknown != reqHandle )
			{		
			// Deliver R99_R4 parameters to the ETel Packet API client
			iNotifyDataPointers.iContextActivationRequested99->iPdpType =
				config_R5.iPdpType;			
			iNotifyDataPointers.iContextActivationRequested99->iAccessPointName =
				config_R5.iAccessPointName;							
			iNotifyDataPointers.iContextActivationRequested99->iPdpAddress =
				config_R5.iPdpAddress;
			iNotifyDataPointers.iContextActivationRequested99->iUseEdge =
				config_R5.iUseEdge;
			iNotifyDataPointers.iContextActivationRequested99->iProtocolConfigOption.iAuthInfo =
				config_R5.iProtocolConfigOption.iAuthInfo;
			iNotifyDataPointers.iContextActivationRequested99->iProtocolConfigOption.iChallenge =
				config_R5.iProtocolConfigOption.iChallenge;
			iNotifyDataPointers.iContextActivationRequested99->iProtocolConfigOption.iResponse =
				config_R5.iProtocolConfigOption.iResponse;
			iNotifyDataPointers.iContextActivationRequested99->iProtocolConfigOption.iId =
				config_R5.iProtocolConfigOption.iId;
			iNotifyDataPointers.iContextActivationRequested99->iProtocolConfigOption.iDnsAddresses =
				config_R5.iProtocolConfigOption.iDnsAddresses;
			iNotifyDataPointers.iContextActivationRequested99->iProtocolConfigOption.iMiscBuffer =
				config_R5.iProtocolConfigOption.iMiscBuffer;								
			iNotifyDataPointers.iContextActivationRequested99->iNWIContext =
				config_R5.iNWIContext;
			iNotifyDataPointers.iContextActivationRequested99->iPFI =
				config_R5.iPFI;	
			iNotifyDataPointers.iContextActivationRequested99 = NULL;	

			// Complete request with KErrNone
			CMmPacketServiceTsy::ReqCompleted( reqHandle, KErrNone );
			}
		
		reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
       		EMultimodePacketServiceNotifyContextActivationRequested5 );	
		if ( config_R5Filled &&
		     EMultimodePacketServiceReqHandleUnknown != reqHandle )
			{	
			// Deliver R5 parameters to the ETel Packet API client
			iNotifyDataPointers.iContextActivationRequested5->iPdpType =
				config_R5.iPdpType;			
			iNotifyDataPointers.iContextActivationRequested5->iAccessPointName =
				config_R5.iAccessPointName;							
			iNotifyDataPointers.iContextActivationRequested5->iPdpAddress =
				config_R5.iPdpAddress;
			iNotifyDataPointers.iContextActivationRequested5->iUseEdge =
				config_R5.iUseEdge;
			iNotifyDataPointers.iContextActivationRequested5->iProtocolConfigOption.iAuthInfo =
				config_R5.iProtocolConfigOption.iAuthInfo;
			iNotifyDataPointers.iContextActivationRequested5->iProtocolConfigOption.iChallenge =
				config_R5.iProtocolConfigOption.iChallenge;
			iNotifyDataPointers.iContextActivationRequested5->iProtocolConfigOption.iResponse =
				config_R5.iProtocolConfigOption.iResponse;
			iNotifyDataPointers.iContextActivationRequested5->iProtocolConfigOption.iId =
				config_R5.iProtocolConfigOption.iId;
			iNotifyDataPointers.iContextActivationRequested5->iProtocolConfigOption.iDnsAddresses =
				config_R5.iProtocolConfigOption.iDnsAddresses;
			iNotifyDataPointers.iContextActivationRequested5->iProtocolConfigOption.iMiscBuffer =
				config_R5.iProtocolConfigOption.iMiscBuffer;								
			iNotifyDataPointers.iContextActivationRequested5->iNWIContext =
				config_R5.iNWIContext;
			iNotifyDataPointers.iContextActivationRequested5->iPFI =
				config_R5.iPFI;																		
	    	iNotifyDataPointers.iContextActivationRequested5->iPdpHeaderCompression =
	    	    config_R5.iPdpHeaderCompression;      	    
	    	iNotifyDataPointers.iContextActivationRequested5->iPdpDataCompression =
	            config_R5.iPdpDataCompression;
	       	iNotifyDataPointers.iContextActivationRequested5 = NULL;
	            
	        // Complete request with KErrNone
	        CMmPacketServiceTsy::ReqCompleted( reqHandle, KErrNone );
			}
		}
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::NotifyContextAdded
// Requests notification of context has been added.
// Allows a client to be notified whenever a new context has been created. It
// should be noted that once the notification completes, the client must then
// re-post the notification if they wish to continue receiving further
// notifications.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::NotifyContextAdded(
    const TDataPackage& aPackage )   
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NOTIFYCONTEXTADDED_1, "TSY: CMmPacketServiceTsy::NotifyContextAdded." );

	TDes* const context = aPackage.Des1u();

    // Save request handle type and context Name
    iReqHandleType = EMultimodePacketServiceNotifyContextAdded;
    iNotifyDataPointers.iContextAdded = context;

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteNotifyContextAdded
// Completes the context added notification to client.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteNotifyContextAdded(
    CMmDataPackage* aPackage )  
    {
    TInfoName tempHostCID;
    tempHostCID.Zero();
    
	//Variables needed for unpacking data

	TInfoName contextName;
	contextName.Zero();
	TInt result ( 0 );

    TInt trapError( KErrNone );
    TInt ret( KErrNone );

	aPackage->UnPackData( contextName, result  );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYCONTEXTADDED_1, "TSY: CMmPacketServiceTsy::CompleteNotifyContextAdded." );

	if ( KErrNone != result )
        {
        //if one Dial-up context is created
        if ( iDialUpContext )
            {
            // External context, service has no control to the context.
            // Context name forced to External2.
            contextName.Copy( KStringExternal2 );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYCONTEXTADDED_2, "TSY: CMmPacketServiceTsy::CompleteNotifyContextAdded :Create Secondary Dial-up Object." );
        
            // Create object for externally created dial-up context
            TRAP( trapError, iSecondaryDialUpContext = CMmPacketContextTsy::NewL(
                this, tempHostCID, contextName ); );
            }

        else  
            {
            // External context, service has no control to the context.
            // Context name forced to External.
            contextName.Copy( KStringExternal );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYCONTEXTADDED_3, "TSY: CMmPacketServiceTsy::CompleteNotifyContextAdded :Create Dial-up Object." );
        
            // Create object for externally created dial-up context
            TRAP( trapError, iDialUpContext = CMmPacketContextTsy::NewL(
                this, tempHostCID, contextName ); );
            }

        if ( KErrNone == trapError )
            {
            if ( contextName.Compare( KStringExternal2 ) == 0 )
                {
                ret = iPacketContextList->AddObject(
                    iSecondaryDialUpContext, 0x00, 0x00, 
                    contextName );
                }
            else if ( contextName.Compare( KStringExternal ) == 0 )
                {
                ret = iPacketContextList->AddObject( iDialUpContext,
                    0x00, 0x00, contextName );
                }

            if( KErrNone != ret )
                {

                if ( contextName.Compare( KStringExternal2 ) == 0 )
                    {
                    iSecondaryDialUpContext->Close();
                    }
                else if ( contextName.Compare( KStringExternal ) == 0 )
                    {
                    iDialUpContext->Close();
                    }
                }

	        if ( KErrNone != ret || KErrNone != trapError )
	            {
	            if ( contextName.Compare( KStringExternal2 ) == 0 )
	                {
	                iSecondaryDialUpContext = NULL;
	                }
	            else if ( contextName.Compare( KStringExternal ) == 0 )
	              	{
	                iDialUpContext = NULL;
	                }
	           	}
	     	}
        
        //Check if dial-up context is created, and set default params
        if ( contextName.Compare( KStringExternal2 ) == 0 )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYCONTEXTADDED_4, "TSY: CMmPacketServiceTsy::CompleteNotifyContextAdded :Reset Secondary Dial-up Object." );
            // Reset secondary dial-up context
            if(iSecondaryDialUpContext)
            	{
            	iSecondaryDialUpContext->ResetDialUpContext();
            	}
			
            }
        else if ( contextName.Compare( KStringExternal ) == 0 )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYCONTEXTADDED_5, "TSY: CMmPacketServiceTsy::CompleteNotifyContextAdded :Reset Dial-up Object." );
            // Reset dial-up context
            if(iDialUpContext)
            	{
            	iDialUpContext->ResetDialUpContext();
            	}
            }
        }

    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceNotifyContextAdded ) );

    // Check that request handle is valid
    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {
        // Add context name to the etel side memory
        *iNotifyDataPointers.iContextAdded = contextName;
        iNotifyDataPointers.iContextAdded = NULL;
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYCONTEXTADDED_6, "TSY: CMmPacketServiceTsy::CompleteNotifyContextAdded. Context Name: %S", contextName );

        // Complete request with KErrNone
        CMmPacketServiceTsy::ReqCompleted( reqHandle, KErrNone );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::NotifyDynamicCapsChange
// Requests notification of dynamic capabilities change.
// This function will complete when the dynamic packet data capabilities
// change; that is, either some action has become possible and/or some action
// is no longer possible. A common usage of the dynamic capabilities functions
// would be for GetDynamicCaps() to be called first to retrieve the current
// set of capabilities, and then the NotifyDynamicCapsChange() request to be
// posted to be kept informed of any changes.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::NotifyDynamicCapsChange(
    RPacketService::TDynamicCapsFlags* const aCaps ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NOTIFYDYNAMICCAPSCHANGE_1, "TSY: CMmPacketServiceTsy::NotifyDynamicCapsChange." );

    // Save pointer to aCaps
    iNotifyDataPointers.iDynamicCapsChange = aCaps;

    // Set request handle type
    iReqHandleType = EMultimodePacketServiceNotifyDynamicCapsChange;

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteNotifyDynamicCapsChange
// Completes dynamic capabilities notification to client.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteNotifyDynamicCapsChange(
    const RPacketService::TDynamicCapsFlags aCaps ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYDYNAMICCAPSCHANGE_1, "TSY: CMmPacketServiceTsy::CompleteNotifyDynamicCapsChange." );

    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceNotifyDynamicCapsChange ) );

    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {
        *iNotifyDataPointers.iDynamicCapsChange = aCaps;
        iNotifyDataPointers.iDynamicCapsChange = NULL;

        CMmPacketServiceTsy::ReqCompleted( reqHandle, KErrNone );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::NotifyMSClassChange
// Requests notification of mobile station class change.
// This function will complete when there is a change in the current GPRS
// class of the mobile station. The new class is passed back in the aNewClass
// parameter.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::NotifyMSClassChange(
    RPacketService::TMSClass* const aNewClass ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NOTIFYMSCLASSCHANGE_1, "TSY: CMmPacketServiceTsy::NotifyMSClassChange." );

    // Save aNewClass internally
    iNotifyDataPointers.iMSClassChange = aNewClass;

    iReqHandleType = EMultimodePacketServiceNotifyMSClassChange;

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteNotifyMSClassChange
// Completes mobile station class notification to client.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteNotifyMSClassChange(
    CMmDataPackage* aDataPackage )				
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYMSCLASSCHANGE_1, "TSY: CMmPacketServiceTsy::CompleteNotifyMSClassChange." );

	RPacketService::TMSClass newClass;
	aDataPackage->UnPackData( newClass );

    if ( newClass != iInternalSavedData.iMsClass )
        {
        // Save new ms class internally
        iInternalSavedData.iMsClass = newClass;

        // Reset request handle. Returns the deleted request handle
        const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePacketServiceNotifyMSClassChange ) );

        if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
            {
            // Save changed MS Class to ETel side memory
            *iNotifyDataPointers.iMSClassChange = newClass;
            iNotifyDataPointers.iMSClassChange = NULL;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYMSCLASSCHANGE_2, "TSY: CMmPacketServiceTsy::CompleteNotifyMSClassChange. New Ms Class: %d", newClass );
            // Complete request with KErrNone
            CMmPacketServiceTsy::ReqCompleted( reqHandle, KErrNone );
            }
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::NotifyStatusChange
// informs client that service status change.
// Allows a client to be notified of a change in the status
// of the connection to the packet service. The new status is
// passed back to the client via aPacketStatus parameter.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::NotifyStatusChange(
    RPacketService::TStatus* const aPacketStatus ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NOTIFYSTATUSCHANGE_1, "TSY: CMmPacketServiceTsy::NotifyStatusChange." );

    // Save aPacketStatus to internal memory
    iNotifyDataPointers.iStatusChange = aPacketStatus;

    // Set request handle type
    iReqHandleType = EMultimodePacketServiceNotifyStatusChange;

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteNotifyStatusChange
// Completes service status notification to client.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteNotifyStatusChange(
    const RPacketService::TStatus& aPacketStatus,
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYSTATUSCHANGE_1, "TSY: CMmPacketServiceTsy::CompleteNotifyStatusChange." );
    if ( aPacketStatus != iInternalSavedData.iServiceStatus )
        {
        iInternalSavedData.iServiceStatus = aPacketStatus;

        // Reset req handle. Returns the deleted req handle
        const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePacketServiceNotifyStatusChange ) );

        if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
            {
            *iNotifyDataPointers.iStatusChange = aPacketStatus;
            iNotifyDataPointers.iStatusChange = NULL;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYSTATUSCHANGE_2, "TSY: CMmPacketServiceTsy::CompleteNotifyStatusChange. Service Status:%d", aPacketStatus );
            CMmPacketServiceTsy::ReqCompleted( reqHandle, aErrorCode );
            }
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::RejectActivationRequest
// Reject activation request from the network.
// This function will cause the phone to reject the network's request for a
// PDP context activation. Note, there is no corresponding "accept" message
// for the phone to send. To accept, the phone should simply begin to activate
// a PDP context with the appropriate IP address before the network's timer
// expires. On GPRS, this timer is between 5 and 8 seconds, but may
// effectively be between around 25 and 40 seconds as the specifications state
// that the activation request should be re-sent up to five times.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::RejectActivationRequestL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_REJECTACTIVATIONREQUESTL_1, "TSY: CMmPacketServiceTsy::RejectActivationRequestL." );

    const TTsyReqHandle tsyRegHandle( iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodePacketServiceRejectActivationRequest ) );

    TInt ret( KErrServerBusy );

    if ( EMultimodePacketServiceReqHandleUnknown == tsyRegHandle )
        {
		// Reject network activation
		ret = iMessageManager->HandleRequestL( 
		    EPacketRejectActivationRequest );

        if ( KErrNone == ret )
            {
            // Set request handle
            iReqHandleType = EMultimodePacketServiceRejectActivationRequest;
            }
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteRejectActivationRequest
// Completes asynchronous reject activation request.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteRejectActivationRequest()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEREJECTACTIVATIONREQUEST_1, "TSY: CMmPacketServiceTsy::CompleteRejectActivationRequest." );

    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceRejectActivationRequest ) );

    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        CMmPacketServiceTsy::ReqCompleted( reqHandle, KErrNone );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::SetAttachMode
// Sets attach mode given in parameter to the server.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::SetAttachModeL(
    const RPacketService::TAttachMode* aMode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_SETATTACHMODEL_1, "TSY: CMmPacketServiceTsy::SetAttachModeL." );

    iAttachMode = *aMode;

    TInt ret( KErrServerBusy );
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodePacketServiceSetAttachMode ) );

    if ( EMultimodePacketServiceReqHandleUnknown == reqHandle )
        {
 		//Set attach mode
		ret = iMmPacketServiceGsmWcdmaExt->SetAttachModeL( *aMode );

		if ( KErrNone == ret )
            {
            // Set req handle
            iReqHandleType = EMultimodePacketServiceSetAttachMode;
            }
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteSetAttachMode
// Completes the Set Attach Mode request.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteSetAttachMode(
    const TInt aError )  //error value
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETESETATTACHMODE_1, "TSY: CMmPacketServiceTsy::CompleteSetAttachMode. Error Value: %{TSymbianErrorCodes}", aError );

    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceSetAttachMode ) );

    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        CMmPacketServiceTsy::ReqCompleted( reqHandle, aError );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::SetPreferredBearer
// Sets preferred bearer given in parameter to the GPDS server.
// This function will set the preferred bearer for the MT to default to when
// dropping from Class B operation to Class C, for instance when the MT moves
// into a Routing Area of cells or new GPRS network which operates in Network
// Mode III. Class C operation requires that the MT is attached to either GSM
// or GPRS, not both.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::SetPreferredBearerL(
    const TDataPackage& aPackage )		
    {
	TAny* const dataPtr = aPackage.Ptr1();

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_SETPREFERREDBEARERL_1, "TSY: CMmPacketServiceTsy::SetPreferredBearerL. Preferred Bearer: %d", &dataPtr );

    const TTsyReqHandle reqHandle( iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodePacketServiceSetPreferredBearer ) );

    TInt ret( KErrServerBusy );

    if ( EMultimodePacketServiceReqHandleUnknown == reqHandle )
        {
        RPacketService::TPreferredBearer* abearer = REINTERPRET_CAST(
            RPacketService::TPreferredBearer*, dataPtr );

        // Set preferred beareraPackage
        ret = iMmPacketServiceGsmWcdmaExt->SetPreferredBearerL( aPackage );

        if ( KErrNone == ret )
            {
            iTemporarySavedData.iPreferredBearer = *abearer;

            // Set request handle type
            iReqHandleType = EMultimodePacketServiceSetPreferredBearer;
            }
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteSetPreferredBearer
// Completes the Set Preferred Bearer request.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteSetPreferredBearer(
    const TInt aError )  //epoc error code
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETESETPREFERREDBEARER_1, "TSY: CMmPacketServiceTsy::CompleteSetPreferredBearer. Error: %{TSymbianErrorCodes}", aError );

    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceSetPreferredBearer ) );

    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aError )
            {
            iInternalSavedData.iPreferredBearer =
                iTemporarySavedData.iPreferredBearer;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETESETPREFERREDBEARER_2, "TSY: CMmPacketServiceTsy::CompleteSetPreferredBearer. Preferred Bearer: %d", iInternalSavedData.iPreferredBearer );
            }

        // Complete the client request
        CMmPacketServiceTsy::ReqCompleted( reqHandle, aError );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::ExtFunc
// ExtFunc is called by the ETel when it has request for the TSY.
// For TSY to process request a request handle, request type and request data
// are passed to the TSY.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::ExtFunc(
    const TTsyReqHandle aTsyReqHandle,  
    const TInt aIpc,                    
    const TDataPackage& aPackage )     
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_EXTFUNC_1, "TSY: CMmPacketServiceTsy::ExtFunc. IPC: %{TIPCNamesList}", aIpc );

    TInt ret( KErrNone );
    TInt trapError( KErrNone );

    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==EMultimodePacketServiceReqHandleUnknown, User::Invariant());
    
    // Set tsy request handle
    iTsyReqHandle = aTsyReqHandle;

    // Call DoExtFuncL()
    TRAP( trapError, ret = DoExtFuncL( aIpc, aPackage ); );

    if ( KErrNone != trapError )
        {
		//reset request handle to indicate the request is no longer ongoing
        iTsyReqHandleStore->FindAndResetTsyReqHandle(aTsyReqHandle);
        CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, trapError );
        }
    else if ( KErrNone != ret )
        {
        CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, ret );
        }
    else if ( EMultimodePacketServiceReqHandleUnknown != iReqHandleType )
        {
        // Save request handle type

#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( iReqHandleType );
#else
        iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, iTsyReqHandle );
#endif // REQHANDLE_TIMER
        }
    // We've finished with this value now. Clear it so it doesn't leak
    //  up to any other instances of this method down the call stack
    iReqHandleType = EMultimodePacketServiceReqHandleUnknown;
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::DoExtFuncL
// Calls correct method to handle ETel server request.
// unpack packaged parameters
// call correct method implemention using the IPC number and return the
// return value back to the caller.
// in case IPC number is not understood by switch, KErrNotSupported will
// be returned.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::DoExtFuncL(
    const TInt aIpc,                
    const TDataPackage& aPackage )  
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_DOEXTFUNCL_1, "TSY: CMmPacketServiceTsy::DoExtFuncL. IPC: %{TIPCNamesList} TSY Req Handle:%d Object:0x%08x", (TUint)aIpc, (TInt)iTsyReqHandle , (TUint)this);

    TInt ret( KErrNotSupported );

    TAny* const dataPtr = aPackage.Ptr1();
    TAny* const dataPtr2 = aPackage.Ptr2();

    switch ( aIpc )
        {
        case EPacketNotifyContextAdded:
			ret = NotifyContextAdded( aPackage );
            break;
        case EPacketAttach:
            ret = AttachL();
            break;
        case EPacketDetach:
            ret = DetachL();
            break;
        case EPacketGetStatus:
            ret = GetStatus(
                REINTERPRET_CAST( RPacketService::TStatus*, dataPtr ) );
            break;
        case EPacketNotifyStatusChange:
            ret = NotifyStatusChange(
                REINTERPRET_CAST( RPacketService::TStatus*, dataPtr ) );
            break;
        case EPacketNotifyContextActivationRequested:
            ret = NotifyContextActivationRequested(
                REINTERPRET_CAST( TPacketDataConfigBase*, dataPtr ) );
            break;
        case EPacketRejectActivationRequest:
            ret = RejectActivationRequestL();
            break;
        case EPacketEnumerateContexts:
            ret = EnumerateContexts(
                REINTERPRET_CAST( TInt*, dataPtr ),
                REINTERPRET_CAST( TInt*, dataPtr2 ) );
            break;
        case EPacketGetContextInfo:
            ret = GetContextInfo(
                REINTERPRET_CAST( TInt*, dataPtr ),
                REINTERPRET_CAST( RPacketService::TContextInfo*, dataPtr2 ) );
            break;
        case EPacketGetNtwkRegStatus:
            ret = GetNtwkRegStatusL(
                REINTERPRET_CAST( RPacketService::TRegistrationStatus*,
                    dataPtr ) );
            break;
        case EPacketNotifyChangeOfNtwkRegStatus:
            ret = NotifyChangeOfNtwkRegStatus(
                REINTERPRET_CAST( RPacketService::TRegistrationStatus*,
                    dataPtr ) );
            break;
        case EPacketGetMSClass:
            ret = GetMSClass(
                REINTERPRET_CAST( RPacketService::TMSClass*, dataPtr ),
                REINTERPRET_CAST( RPacketService::TMSClass*, dataPtr2 ) );
            break;
        case EPacketNotifyMSClassChange:
            ret = NotifyMSClassChange(
                REINTERPRET_CAST( RPacketService::TMSClass*, dataPtr ) );
            break;
        case EPacketSetPrefBearer:
			ret = SetPreferredBearerL( aPackage );
            break;
        case EPacketGetPrefBearer:
            ret = GetPreferredBearer( REINTERPRET_CAST(
                RPacketService::TPreferredBearer*, dataPtr ) );
            break;
        case EPacketGetStaticCaps:
            ret = GetStaticCaps(
                REINTERPRET_CAST( TUint*, dataPtr ),
                REINTERPRET_CAST( RPacketContext::TProtocolType*, dataPtr2 ) );
            break;
        case EPacketGetDynamicCaps:
            ret = GetDynamicCaps( REINTERPRET_CAST(
                RPacketService::TDynamicCapsFlags*, dataPtr ) );
            break;
        case EPacketNotifyDynamicCapsChange:
            ret = NotifyDynamicCapsChange( REINTERPRET_CAST(
                RPacketService::TDynamicCapsFlags*, dataPtr ) );
            break;
        case EPacketSetAttachMode:
            ret = SetAttachModeL(
                REINTERPRET_CAST( RPacketService::TAttachMode*, dataPtr ) );
            break;
        case EPacketGetAttachMode:
            ret = GetAttachModeL(
                REINTERPRET_CAST( RPacketService::TAttachMode*, dataPtr ) );
            break;
        case EPacketPrepareOpenSecondary:
            ret = PrepareOpenSecondary( aPackage.Des1u() );
            break;
        case EPacketEnumerateNifs:
            ret = EnumerateNifs( REINTERPRET_CAST ( TInt*, dataPtr ) );
            break;
        case EPacketGetNifInfo:
        	// Check that dataPtr2 is not null
        	if (dataPtr2)
        		{
        		// Get the TDes8 version of the parameter and find its MaxLength()
        		TInt parameterLength = aPackage.Des2n()->MaxLength();
        		
        		// Create a temporary object of the expected type and find the expected MaxLength()
        		RPacketService::TNifInfoV2 temp;
        		RPacketService::TNifInfoV2Pckg tempPckg(temp);
        		TInt expectedLength = tempPckg.MaxLength();

				// Compare the expected length with the actual length of the sent parameter
	        	if ( parameterLength == expectedLength )
	        		{
		            ret = GetNifInfo( REINTERPRET_CAST( TInt*, dataPtr ),
	    	            REINTERPRET_CAST( RPacketService::TNifInfoV2*, dataPtr2 ) );
	        		}
	        	else
	        		{
	        		ret = KErrArgument;
	        		}
        		}
        	else
        		{
        		ret = KErrArgument;
        		}	
            break;
        case EPacketEnumerateContextsInNif:
            ret = EnumerateContextsInNif( aPackage.Des2u(),
                REINTERPRET_CAST( TInt*, dataPtr ) );
            break;
        case EPacketGetContextNameInNif:
            ret = GetContextNameInNif(
                REINTERPRET_CAST( RPacketService::TContextNameInNif*,
                    dataPtr ), aPackage.Des2u() );
            break;
        case EPacketSetDefaultContextParams:
			ret = SetDefaultContextParamsL( aPackage );
            break;
        case EPacketGetDefaultContextParams:
            ret = GetDefaultContextParams(
                REINTERPRET_CAST( TPacketDataConfigBase*, dataPtr ) );
            break;
        case EPacketNotifyMbmsNetworkServiceStatusChange:
        	ret = NotifyMbmsNetworkServiceStatusChange( 
        		reinterpret_cast<TMbmsNetworkServiceStatus*> (dataPtr ) );
            break;
        case EPacketNotifyMbmsServiceAvailabilityChange:
        	ret = NotifyMbmsServiceAvailabilityChange();
            break;
        case EPacketGetMbmsNetworkServiceStatus:
        	ret = GetMbmsNetworkServiceStatus(
        	    *reinterpret_cast<TBool*> (dataPtr ),
        	    reinterpret_cast<TMbmsNetworkServiceStatus*> (dataPtr2 ) );
        	break; 
        case EPacketEnumerateMbmsMonitorServiceList:
            ret = EnumerateMonitorServiceList(
                *reinterpret_cast<TInt*> ( dataPtr ),
                *reinterpret_cast<TInt*> ( dataPtr2 ) );
            break;
        case EPacketEnumerateMbmsActiveServiceList:
            ret = EnumerateMbmsActiveServiceList(
                *reinterpret_cast<TInt*> ( dataPtr),
                *reinterpret_cast<TInt*>(dataPtr2) );
            break;
        case EPacketUpdateMbmsMonitorServiceList:
			TMbmsAction action;
			action = *reinterpret_cast< TMbmsAction*> (dataPtr );
        	if(dataPtr2)
				{
				ret = UpdateMbmsMonitorServiceListL(
               		action,
                      aPackage.Des2n() );
				}
			else
				{
				ret = UpdateMbmsMonitorServiceListL(
					action, NULL );
				}                            
            break;
		case EPacketGetMbmsMonitoredServicesPhase1:
            ret = GetMbmsMonitoredServicesPhase1L(
                reinterpret_cast<RMobilePhone::TClientId*> (dataPtr),
                *reinterpret_cast<TUint*>(dataPtr2 ) );
            break;
        case EPacketGetMbmsMonitoredServicesPhase2:
            ret = GetMbmsMonitoredServicesPhase2(
                reinterpret_cast<RMobilePhone::TClientId*>( dataPtr ),
                aPackage.Des2n());
            break;           
        //case EPacketSetMSClass:
        default:
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_DOEXTFUNCL_2, "<-- TSY: CMmPacketServiceTsy::DoExtFuncL, error = %{TSymbianErrorCodes}", ret);
    return ret;
    }



//----------------------------------------------------------------------------
// CMmPacketServiceTsy::ReqModeL
// Returns request mode for given IPC.
// leave if given IPC is not supported
// return request mode for IPC number given in parameter
//----------------------------------------------------------------------------
//
CTelObject::TReqMode CMmPacketServiceTsy::ReqModeL(
    const TInt aIpc ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_REQMODEL_1, "TSY: CMmPacketServiceTsy::ReqModeL. IPC: %{TIPCNamesList}", aIpc );

    CTelObject::TReqMode reqMode( 0 );
    switch ( aIpc )
        {
        // All disabled TSYs wishing to implement their own buffering 
        // algorithm will place all requests in this category.

        // Get methods that do not use DOS and return immediately.
        // Other methods that return immediately.
        // Methods that can propably take a long time flow control not 
        // required.
        case EPacketGetNtwkRegStatus:
        case EPacketGetDynamicCaps:
        case EPacketGetStatus:
        case EPacketEnumerateContexts:
        case EPacketGetContextInfo:
        case EPacketGetAttachMode:
        case EPacketGetStaticCaps:
        case EPacketRejectActivationRequest:
        case EPacketSetAttachMode:
        case EPacketAttach:
        case EPacketDetach:
        case EPacketSetPrefBearer:
        case EPacketGetPrefBearer:
        case EPacketPrepareOpenSecondary:
        case EPacketEnumerateNifs:
        case EPacketGetNifInfo:
        case EPacketEnumerateContextsInNif:
        case EPacketGetContextNameInNif:
        case EPacketSetDefaultContextParams:
        case EPacketGetDefaultContextParams:
        case EPacketUpdateMbmsMonitorServiceList:
        case EPacketEnumerateMbmsMonitorServiceList:
        case EPacketGetMbmsNetworkServiceStatus:
        case EPacketEnumerateMbmsActiveServiceList:        
        case EPacketGetMbmsMonitoredServicesPhase1:
        case EPacketGetMbmsMonitoredServicesPhase2:
            break;

        // Commands that change the state of the phone.
        case EPacketGetMSClass:
            reqMode = KReqModeFlowControlObeyed;
            break;

        // Requests that notify a client about a change of state.
        case EPacketNotifyContextAdded:
        case EPacketNotifyStatusChange:
        case EPacketNotifyChangeOfNtwkRegStatus:
        case EPacketNotifyContextActivationRequested:
        case EPacketNotifyDynamicCapsChange:
        case EPacketNotifyMSClassChange:
        case EPacketNotifyMbmsNetworkServiceStatusChange:
            reqMode = KReqModeMultipleCompletionEnabled |
                KReqModeRePostImmediately;
        case EPacketNotifyMbmsServiceAvailabilityChange:       
            break;

        // Following requests are not supported
        //case EPacketSetMSClass:
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return reqMode;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC.
// return number of slots to be used for IPC number given in parameter
// leave with KErrNotSupported if invalid IPC
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::NumberOfSlotsL(
    const TInt aIpc ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NUMBEROFSLOTSL_1, "TSY: CMmPacketServiceTsy::NumberOfSlotsL. IPC: %{TIPCNamesList}", aIpc );

    TInt numberOfSlots( 0 );
    switch ( aIpc )
        {

        case EPacketNotifyStatusChange:
            numberOfSlots = KMmPacketServiceNotifyStatusChangeSlots;
            break;
        case EPacketNotifyDynamicCapsChange:
            numberOfSlots = KMmPacketServiceNotifyDynamicCapsChangeSlots;
            break;
        case EPacketNotifyContextAdded:
            numberOfSlots = KMmPacketServiceNotifyContextAddedSlots;
            break;
        case EPacketNotifyContextActivationRequested:
            numberOfSlots =
                KMmPacketServiceNotifyContextActivationRequestedSlots;
            break;
        case EPacketNotifyChangeOfNtwkRegStatus:
            numberOfSlots = KMmPacketServiceNotifyChangeOfNtwkRegStatusSlots;
            break;
        case EPacketNotifyMSClassChange:
            numberOfSlots = KMmPacketServiceNotifyMSClassChangeSlots;
            break;
        case EPacketNotifyMbmsNetworkServiceStatusChange:
            numberOfSlots = KMmPacketServiceNotifyMBMSServiceStatusSlots;
            break;
        default:
            // Unknown or invalid IPC
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CancelService
// When the clients close their sub-sessions (eg. by calling RPhone::Close),
// they may not have cancelled all their outstanding asynchronous requests
// before closing. It is up to the ETel server to clean up in this situation,
// so the server will find the list of outstanding requests related to that
// sub-session object and pass these outstanding IPC request numbers, one at a
// time, to the CancelService function in the TSY.
// This method is also called when client initiates normal cancel to the
// request.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::CancelService(
    const TInt aIpc,                    
    const TTsyReqHandle aTsyReqHandle ) 
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_CANCELSERVICE_1, "TSY: CMmPacketServiceTsy::CancelService. IPC: %{TIPCNamesList} Tsy Req Handle:%d", aIpc, aTsyReqHandle);

    TInt ret( KErrNone );
    
    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==EMultimodePacketServiceReqHandleUnknown, User::Invariant());

    // When the clients close their sub-sessions (eg. by calling
    // RPacketService::Close), they may not have cancelled all their
    // outstanding asynchronous requests before closing.  It is up to the ETel
    // server to clean up in this situation, so the server will find the list
    // of outstanding requests related to that sub-session object and pass
    // these outstanding IPC request numbers, one at a time, to the
    // CancelService method in the TSY.

    switch ( aIpc )
        {
        case EPacketNotifyContextAdded:
            iNotifyDataPointers.iContextAdded = NULL;
            iReqHandleType = EMultimodePacketServiceNotifyContextAdded;
            break;
        case EPacketNotifyStatusChange:
            iNotifyDataPointers.iStatusChange = NULL;
            iReqHandleType = EMultimodePacketServiceNotifyStatusChange;
            break;
        case EPacketNotifyContextActivationRequested:
            if ( aTsyReqHandle == iTsyReqHandleStore->GetTsyReqHandle(
                EMultimodePacketServiceNotifyContextActivationRequested ) )
                {
                iNotifyDataPointers.iContextActivationRequested = NULL;
                iReqHandleType =
                    EMultimodePacketServiceNotifyContextActivationRequested;
                }
            else if ( aTsyReqHandle == iTsyReqHandleStore->GetTsyReqHandle(
                EMultimodePacketServiceNotifyContextActivationRequested99 ) )
                {
                iNotifyDataPointers.iContextActivationRequested99 = NULL;
                iReqHandleType =
                    EMultimodePacketServiceNotifyContextActivationRequested99;
                }
            else if ( aTsyReqHandle == iTsyReqHandleStore->GetTsyReqHandle(
                EMultimodePacketServiceNotifyContextActivationRequested5 ) )
                {
                iNotifyDataPointers.iContextActivationRequested5 = NULL;
                iReqHandleType =
                    EMultimodePacketServiceNotifyContextActivationRequested5;
                }                
            break;
        case EPacketNotifyChangeOfNtwkRegStatus:
            iNotifyDataPointers.iChangeOfNtwkRegStatus = NULL;
            iReqHandleType = EMultimodePacketServiceNotifyNtwkRegStatusChange;
            break;
        case EPacketNotifyMSClassChange:
            iNotifyDataPointers.iMSClassChange = NULL;
            iReqHandleType = EMultimodePacketServiceNotifyMSClassChange;
            break;
        case EPacketNotifyDynamicCapsChange:
            iNotifyDataPointers.iDynamicCapsChange = NULL;
            iReqHandleType = EMultimodePacketServiceNotifyDynamicCapsChange;
            break;
        case EPacketNotifyMbmsNetworkServiceStatusChange:
        	iNotifyDataPointers.iMBMSStatusChange = NULL;
        	iReqHandleType = EMultimodePacketServiceNotifyMBMSStatusChange;
        	break;
        case EPacketNotifyMbmsServiceAvailabilityChange:
         	iReqHandleType = EMultimodePacketServiceNotifyMbmsServiceAvailabilityChange;
        	break;       
        case EPacketGetMbmsNetworkServiceStatus:
         	iReqHandleType = EMultimodePacketServiceGetMBMSStatus;
        	break;       
        case EPacketGetMbmsMonitoredServicesPhase1:
        case EPacketGetMbmsMonitoredServicesPhase2:
        	GetMbmsMonitoredServicesCancel( aTsyReqHandle );
        	break;
        case EPacketUpdateMbmsMonitorServiceList:
        	UpdateMbmsMonitorServiceListCancel(aTsyReqHandle);	
        	break;
        // TSY has started a request and it is not possible to then
        // cancel this request. The best thing for the TSY to do in this case
        // is to proceed as though the Cancel never happened. The server's 
        // call to the TSY cancel function will return synchronously.
        case EPacketAttach:
        case EPacketDetach:
        case EPacketGetStatus:
        case EPacketRejectActivationRequest:
        case EPacketEnumerateContexts:
        case EPacketGetContextInfo:
        case EPacketGetNtwkRegStatus:
        case EPacketGetMSClass:
        case EPacketSetPrefBearer:
        case EPacketGetPrefBearer:
        case EPacketGetStaticCaps:
        case EPacketGetDynamicCaps:
        case EPacketSetAttachMode:
        case EPacketGetAttachMode:
        case EPacketEnumerateNifs:
        case EPacketGetNifInfo:
        case EPacketEnumerateContextsInNif:
        case EPacketGetContextNameInNif:
        case EPacketSetDefaultContextParams:
        case EPacketGetDefaultContextParams:
        case EPacketEnumerateMbmsMonitorServiceList:
        case EPacketEnumerateMbmsActiveServiceList:
            break;

        // Cancel methods that are not supported.
        //case EPacketSetMSClass:
        default:
            ret = KErrNotSupported;
            break;
        }

    if ( EMultimodePacketServiceReqHandleUnknown != iReqHandleType )
        {
        // Reset tsy request handle
        iTsyReqHandleStore->ResetTsyReqHandle( iReqHandleType );

        // We've finished with this value now. Clear it so it doesn't leak
        //  up to any other instances of this method down the call stack
        iReqHandleType = EMultimodePacketServiceReqHandleUnknown;

        // Complete request with KErrCancel
        CMmPacketServiceTsy::ReqCompleted( aTsyReqHandle, KErrCancel );
        }

    return ret;
    }

//----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::RegisterNotification
// Register notification.
// Returns KErrNone to ETel server in case of known notification request type.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::RegisterNotification(
    const TInt aIpc ) 
	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_REGISTERNOTIFICATION_1, "TSY: CMmPacketServiceTsy::RegisterNotification. IPC: %{TIPCNamesList}", aIpc );

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EPacketNotifyContextAdded:
        case EPacketNotifyStatusChange:
        case EPacketNotifyContextActivationRequested:
        case EPacketNotifyChangeOfNtwkRegStatus:
        case EPacketNotifyMSClassChange:
        case EPacketNotifyDynamicCapsChange:
        case EPacketNotifyMbmsNetworkServiceStatusChange:
        case EPacketNotifyMbmsServiceAvailabilityChange:
            break;
        default:
            // Not supported
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::DeregisterNotification
// Deregister notification.
// Returns KErrNone to ETel server in case of known notification request type.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::DeregisterNotification(
    const TInt aIpc ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_DEREGISTERNOTIFICATION_1, "TSY: CMmPacketServiceTsy::DeregisterNotification. IPC: %{TIPCNamesList}", aIpc );

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EPacketNotifyContextAdded:
        case EPacketNotifyStatusChange:
        case EPacketNotifyContextActivationRequested:
        case EPacketNotifyChangeOfNtwkRegStatus:
        case EPacketNotifyMSClassChange:
        case EPacketNotifyDynamicCapsChange:
        case EPacketNotifyMbmsNetworkServiceStatusChange:
        case EPacketNotifyMbmsServiceAvailabilityChange:
            break;
        default:
            // Not supported
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }
    
  

//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::GetMbmsNetworkServiceStatus
// Requests Get MBMS network service status 
//-----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetMbmsNetworkServiceStatus(
		TBool aAttemptAttach, 
		TMbmsNetworkServiceStatus* aMBMSServiceStatus )
{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETMBMSNETWORKSERVICESTATUS_1, "TSY: CMmPacketServiceTsy:: TMbmsNetworkServiceStatus = %d", aMBMSServiceStatus );

   const TTsyReqHandle regHandle( iTsyReqHandleStore->GetTsyReqHandle(
   EMultimodePacketServiceGetMBMSStatus ) );
   
   TInt ret( KErrServerBusy ); 
   if ( !iMbmsStatusCached )
    	{
    	if ( EMultimodePacketServiceReqHandleUnknown == regHandle )  
    		{
    		// Save pointer to aMBMSServiceStatus
    		iTemporarySavedData.iMbmsStatus = aMBMSServiceStatus;  
    		    			    
	    	// Get MBMS network service status
	    	CMmDataPackage dataPackage;
	    	dataPackage.PackData( &aAttemptAttach );
			TRAPD(err, ret = iMessageManager->HandleRequestL( EPacketGetMbmsNetworkServiceStatus, &dataPackage));
			if(err != KErrNone)
				{
				ret = err;
				}
			}
			if ( KErrNone == ret )
   				{
   	 			// Set request handle type
	    		iReqHandleType = EMultimodePacketServiceGetMBMSStatus;	
   				}
    		
    	}
    else // iMbmsStatusCached == ETrue
    	{
    	
		*aMBMSServiceStatus = iInternalSavedData.iMbmsStatus;
		
   		// Complete the request with KErrNone
    	CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );

    	ret = KErrNone;
    
   		}
   		
 	return ret;	
}
	
//-----------------------------------------------------------------------------
// void CMmPacketServiceTsy::CompleteGetMbmsNetworkServiceStatus
// Completes Get MBMS network service status request 
//-----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteGetMbmsNetworkServiceStatus( 
	CMmDataPackage* aDataPackage, TInt aError )
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEGETMBMSNETWORKSERVICESTATUS_1, "TSY: CMmPacketServiceTsy::CompleteGetMbmsNetworkServiceStatus.");

	//Update the cache
	TMbmsNetworkServiceStatus mbmsNetworkServiceStatus = EMbmsSupportUnknown;
	if ( KErrNone == aError )
		{
		//Unpack necessary data from datapackage
		if ( aDataPackage )
			{
			aDataPackage->UnPackData( mbmsNetworkServiceStatus );
			}
            
		// Change MBMS cache indicator value
		iMbmsStatusCached = ETrue;
		iInternalSavedData.iMbmsStatus = mbmsNetworkServiceStatus;
		}

	// Reset request handle. Returns the deleted request handle
	const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
	EMultimodePacketServiceGetMBMSStatus ) );

   // If request handle ok
    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
    	{
    	*iTemporarySavedData.iMbmsStatus = mbmsNetworkServiceStatus;
        // Complete the client request
        CMmPacketServiceTsy::ReqCompleted( reqHandle, aError );
        }
	}
	
//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::NotifyMbmsNetworkServiceStatusChange
// Requests notification of MBMS network service status change.
// Allows a client to be notified whenever a change in the status of the
// MBMS network service is detected.
//-----------------------------------------------------------------------------
//

TInt CMmPacketServiceTsy::NotifyMbmsNetworkServiceStatusChange(
	TMbmsNetworkServiceStatus* aMBMSServiceStatus )
	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NOTIFYMBMSNETWORKSERVICESTATUSCHANGE_1, "TSY: CMmPacketServiceTsy::NotifyMbmsNetworkServiceStatusChange TMbmsNetworkServiceStatus = %d", aMBMSServiceStatus );
	
	// Set request handle and save pointer to aMBMSServiceStatus
    iNotifyDataPointers.iMBMSStatusChange = aMBMSServiceStatus;
    iReqHandleType =  EMultimodePacketServiceNotifyMBMSStatusChange;

    return KErrNone;
	}
	
//-----------------------------------------------------------------------------
// Void CMmPacketServiceTsy::NotifyMbmsNetworkServiceStatusChange
// Completes notification of MBMS network service status request 
//-----------------------------------------------------------------------------
//	
void CMmPacketServiceTsy::CompleteNotifyMbmsNetworkServiceStatusChange( 
	CMmDataPackage* aDataPackage, TInt aResult )
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYMBMSNETWORKSERVICESTATUSCHANGE_1, "TSY: CMmPacketServiceTsy::NotifyMbmsNetworkServiceStatusChange.");
	        
	TInt result = aResult;        
	if (aResult == KErrNone)
		{	        
		//Unpack necessary data from datapackage
		TMbmsNetworkServiceStatus newMbmsNetworkServiceStatus;
		aDataPackage->UnPackData( newMbmsNetworkServiceStatus );
	
		// If Mbms network service status changed
		if ( newMbmsNetworkServiceStatus != iInternalSavedData.iMbmsStatus )
        	{

        	// Save new status internally
        	iInternalSavedData.iMbmsStatus = newMbmsNetworkServiceStatus;
        	iMbmsStatusCached = ETrue;
        	
        	// Reset request handle. Returns the deleted request handle
			const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
	        EMultimodePacketServiceNotifyMBMSStatusChange ) );
	        	
	        // If request handle ok
	    	if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
	        	{
           		// Set MbmsNetworkServiceStatus to the etel side memory
           		*iNotifyDataPointers.iMBMSStatusChange = newMbmsNetworkServiceStatus;
           		iNotifyDataPointers.iMBMSStatusChange = NULL;

	       		// Complete the client request
	       		CMmPacketServiceTsy::ReqCompleted( reqHandle, result );

        		}
       	 	} 
       	 		
		}
	else // result is != KErrNone
		{
		// Reset request handle. Returns the deleted request handle
		const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
	    EMultimodePacketServiceNotifyMBMSStatusChange ) );
	   	// Complete the client request
	    CMmPacketServiceTsy::ReqCompleted( reqHandle, result );
		}
	}
	
//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::NotifyMbmsServiceAvailabilityChange
// Requests notification of MBMS service availability status change.
// Allows a client to be notified whenever a change in the status of the
// MBMS network service is detected.
//-----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::NotifyMbmsServiceAvailabilityChange()
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_NOTIFYMBMSSERVICEAVAILABILITYCHANGE_1, "TSY: CMmPacketServiceTsy::NotifyMbmsServiceAvailabilityChange.");
	
	// Set request handle
    iReqHandleType =  EMultimodePacketServiceNotifyMbmsServiceAvailabilityChange;

    return KErrNone;
	}
	
//-----------------------------------------------------------------------------
// Void CMmPacketServiceTsy::CompleteNotifyMbmsServiceAvailabilityChange
// Completes notification of MBMS service availability status request 
//-----------------------------------------------------------------------------
//	
void CMmPacketServiceTsy::CompleteNotifyMbmsServiceAvailabilityChangeL(CMmDataPackage* aDataPackage, TInt aResult)
	{
	// Reset request handle. Returns the deleted request handle
	const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
	        EMultimodePacketServiceNotifyMbmsServiceAvailabilityChange ) );
		
	// If request handle ok
	if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
		{
#ifdef USING_CTSY_DISPATCHER
		//three defects, 
		//1) no need to allocate a list all we need is a pointer and we unpack over this
		//2) PopAndDestroy at the end could cause issues, in the iMBMSMonitoredList null case
		//3) if completed with an Error,this will cause subsequent  notifys to have no effect as
		//the request handle has been removed from the store. Here as before the request is completed 
		//with aResult on aDataPackage or iMBMSMonitoredList being null.
		if((aResult == KErrNone) && aDataPackage && iMBMSMonitoredList)
			{
			CPcktMbmsMonitoredServiceList* updatedMonitorList = NULL;
			//Unpack necessary data from datapackage
			aDataPackage->UnPackData(&updatedMonitorList);
			iMBMSMonitoredList->SetServicesAvailabilityL(updatedMonitorList);
			}
		
		// Complete the client request
 	   	CMmPacketServiceTsy::ReqCompleted( reqHandle, aResult );
				
#else
		if (aResult == KErrNone)
			{
			if (aDataPackage)
				{
				if(iMBMSMonitoredList != NULL)
					{
					// Create new list for changes
					CPcktMbmsMonitoredServiceList* updatedMonitorList = CPcktMbmsMonitoredServiceList::NewL();
					CleanupStack::PushL(updatedMonitorList);
				
					//Unpack necessary data from datapackage
					aDataPackage->UnPackData( &updatedMonitorList );
					iMBMSMonitoredList->SetServicesAvailabilityL(updatedMonitorList);
					}
				// Complete the client request
		 	   	CMmPacketServiceTsy::ReqCompleted( reqHandle, aResult );
	 	  	 	CleanupStack::PopAndDestroy();		 	   	
	 	   		}
	 	   	else
	 	   		{
				CMmPacketServiceTsy::ReqCompleted( reqHandle, aResult );	
	 	   		}
			}
#endif //USING_CTSY_DISPATCHER
		}
	}					

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::EnumerateMonitorServiceList
// Enumrates monitored service list
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::EnumerateMonitorServiceList( TInt& aCount, TInt& aMaxAllowed )
    {
		if (iMBMSMonitoredList != NULL)
			{
			aCount = iMBMSMonitoredList->NumberOfMonitoredList(  );	
			}
		else
			{
			aCount = 0;
			}
    	aMaxAllowed = iMaxMonitoredServices;
    	
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_ENUMERATEMONITORSERVICELIST_1, "TSY: CMmPacketServiceTsy::EnumerateMonitorServiceList." );
      		
       	// Complete the request with KErrNone
    	CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );	  

    return KErrNone;
    } 

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteMaxMonitoredServicesChange
// Enumrates monitored service list
//----------------------------------------------------------------------------
//    
void CMmPacketServiceTsy::CompleteMaxMonitoredServicesChange( TInt aMaxMonitorValue )
	{
	iMaxMonitoredServices = aMaxMonitorValue;
	}
//----------------------------------------------------------------------------
// CMmPacketServiceTsy::UpdateMbmsMonitorServiceListL
// Request to update monitored service list
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::UpdateMbmsMonitorServiceListL( TMbmsAction aAction,   
             TDes8* aList )
    { 
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->GetTsyReqHandle(
   	EMultimodePacketServiceUpdateMBMSMonitorServiceList ) );
    
    TInt ret( KErrServerBusy ); 
   
	if ( EMultimodePacketServiceReqHandleUnknown == reqHandle )  
		{ 
		if (iMBMSMonitoredList == NULL)
			{
			// create MBMS monitored service list
	 		iMBMSMonitoredList = CMmMBMSMonitoredList::NewL( this, iMaxMonitoredServices );	
			}
		iActionType = aAction;
		// Save changes of Monitored service list and action
		ret = iMBMSMonitoredList->SaveUpdateValuesToTempL(aList, aAction); 
		
		// temp list was ok
		if( KErrNone == ret )
			{
			// Get correct data packet and send it to LTSY
			ret = iMBMSMonitoredList->PackDataL();	
			
			if ( KErrNone == ret )
				{
				// Set request handle type
				iReqHandleType = EMultimodePacketServiceUpdateMBMSMonitorServiceList;	
				}
			}
		else if( KErrAlreadyExists == ret )
			{
			// there were no new entries client tried to supply. Complete with KErrNone
			// to client to think updating was ok
			CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );
			ret = KErrNone;
			}
		else if( KErrNotFound == ret )
			{
			// there were no new entries client tried to supply. Complete with KErrNotfound
			CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, ret );
			ret = KErrNone;
			}
		}	
	
 	return ret;	
    
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteUpdateMbmsMonitorServiceListL
// Completes update monitored service list request
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteUpdateMbmsMonitorServiceList( 
    CMmDataPackage* aDataPackage, 
    const TInt aResult)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEUPDATEMBMSMONITORSERVICELIST_1,  "TSY: CMmPacketServiceTsy::CompleteUpdateMbmsMonitorServiceListL." );

	TInt result= aResult;
	TInt err(KErrNone);
	// Reset request handle. Returns the deleted request handle
	const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
	        EMultimodePacketServiceUpdateMBMSMonitorServiceList ) );
	        
	if( aResult == KErrNone )
		{
		TRAP( err, result = iMBMSMonitoredList->ProcessEntriesL( NULL, iActionType ) );
		if ( !err )
		    TRAP( err, CompleteNotifyMbmsServiceAvailabilityChangeL( NULL, aResult ) );		
		}
	//there were problem managing entries
	else if( (aResult == KErrMbmsImpreciseServiceEntries) && (aDataPackage != NULL) )
		{
		CPcktMbmsMonitoredServiceList* failedMonitorEntries = NULL;
		aDataPackage->UnPackData( &failedMonitorEntries ); 
		// there were a problems with some entries, 
		// Check failed entries exists and add succesful entries to main list
		if( failedMonitorEntries != NULL )
			{
			TRAP( err, iMBMSMonitoredList->ProcessEntriesL( failedMonitorEntries, iActionType ) );
			}
		else // There was a general error, don't add anything to main list
			{
			TRAP( err, iMBMSMonitoredList->ResetTempListL() );
			}
		}
	 if( EMultimodePacketServiceReqHandleUnknown != reqHandle )
		 {
         // If there was an error due to a function leaving, complete to the client with that error.
         if (err)
             {
             result = err;
             }
		 //complete with error to client
		 CMmPacketServiceTsy::ReqCompleted( reqHandle, result );
		 }
	    
	}
	
//----------------------------------------------------------------------------
// CMmPacketServiceTsy::UpdateMbmsMonitorServiceListCancel
// Completes update monitored service list request
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::UpdateMbmsMonitorServiceListCancel( TTsyReqHandle aTsyReqHandle )	
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_UPDATEMBMSMONITORSERVICELISTCANCEL_1,  "TSY: CMmPacketServiceTsy::UpdateMbmsMonitorServiceListCancel ReqHandle %d", aTsyReqHandle );
	
	if ( aTsyReqHandle != EMultimodePacketServiceReqHandleUnknown )
        {
        // Reset tsy request handle
        iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePacketServiceUpdateMBMSMonitorServiceList );

        // Complete request to client with KErrCancel
        ReqCompleted( aTsyReqHandle, KErrCancel );
        }	
        
    return KErrNone;
	}



//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetMbmsMonitoredServicesPhase1L
// Returns size of the monitored service list.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetMbmsMonitoredServicesPhase1L(RMobilePhone::TClientId* aClient,  TUint& aBufSize)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETMBMSMONITOREDSERVICESPHASE1L_1, "TSY: CMmPacketServiceTsy::GetMbmsMonitoredServicesPhase1L called.");
	// Initialize ret value
	TInt ret( KErrNone );
	TInt leaveCode( KErrNone );
	

	if( !iMBMSMonitoredList )
		{
		// No current monitor servece list exist. Complete error.
		ReqCompleted( iTsyReqHandle, KErrNotFound );
		}
	else
		{
		// Get current size of monitor service list
		TRAP( leaveCode, ret = GetSizeOfMonitoredServiceListL( aClient, aBufSize ) );
		if (leaveCode == KErrNone)
			{
			leaveCode = ret;
			}
		if ( leaveCode != KErrNone )
			{
			ReqCompleted( iTsyReqHandle, leaveCode );
			}
		}
	
	return leaveCode;
	}

// ---------------------------------------------------------------------------
// CMmMBMSMonitoredList::GetSizeOfMonitoredServiceList
// This function returns size of monitored servicelist.
// ---------------------------------------------------------------------------
//  
TInt CMmPacketServiceTsy::GetSizeOfMonitoredServiceListL(RMobilePhone::TClientId* aClient,  TUint& aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETSIZEOFMONITOREDSERVICELISTL_1, "TSY: CMmPacketServiceTsy::GetSizeOfMonitoredServiceList called.");
	// create new list
	CPcktMbmsMonitoredServiceList* currentList=CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(currentList);
	RPacketService::TMbmsServiceAvailabilityV1 entry;
	
	if (!iMBMSMonitoredList)
		{
		// create MBMS monitored service list
 		iMBMSMonitoredList = CMmMBMSMonitoredList::NewL( this, iMaxMonitoredServices );	
		}
	
	// fill TMbmsServiceAvailabilityV1 information
	for (TInt i=0; i < iMBMSMonitoredList->ServiceList()->Enumerate(); i++)
		{
		
		entry = iMBMSMonitoredList->ServiceList()->GetEntryL(i);
		// Add the entry into the list, at the next empty location
		currentList->AddEntryL(entry);
		}
	// Store the streamed list and the client ID
	CListReadAllAttempt* read = CListReadAllAttempt::NewL( aClient );
	CleanupStack::PushL(read);

	read->iListBuf = currentList->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC; iListBuf will be destroy by read destructor 
	
	iGetMbmsMonitoredServiceLists->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt; the read will be destroy in phase 2
	
    // return the CBufFlat's size to client
	aBufSize=(read->iListBuf)->Size();
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETSIZEOFMONITOREDSERVICELISTL_2, "TSY: CMmPacketServiceTsy::GetSizeOfMonitoredServiceList: size=%d",aBufSize);
    // Complete first phase of list retrieval
    ReqCompleted( iTsyReqHandle, KErrNone );
    CleanupStack::PopAndDestroy( currentList ); // Pop & destroy list
    return KErrNone;	
	}
			
//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetMbmsMonitoredServicesPhase2
// Returns monitored service list.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetMbmsMonitoredServicesPhase2(RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	// Initialize ret value
    TInt ret( KErrNotFound );
    
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETMBMSMONITOREDSERVICESPHASE2_1, "TSY: CMmPacketServiceTsy::GetMbmsMonitoredServicesPhase2 called");
	CListReadAllAttempt* read=NULL;
	
	// Find the get Mbms monitored services from this client
	for (TInt i=0; i<iGetMbmsMonitoredServiceLists->Count(); ++i)
		{
		// Check that session and client are rigth ones
		read = iGetMbmsMonitoredServiceLists->At(i);
		
		if ((read->iClient.iSessionHandle == aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle == aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			
	  	
	  		if ( bufPtr.Length() <= aBuf->Length() )
		  		{
		  		// Copy the streamed list to the client
				// Set pointer to list buffer
				aBuf->Copy( bufPtr );
				ret = KErrNone;
				}
			else
				{
				ret = KErrArgument;
				}
			
			// Delete read and item from internal array
			delete read;
			iGetMbmsMonitoredServiceLists->Delete(i);
			
			// Complete request
			ReqCompleted(iTsyReqHandle, ret);
			
            return ret;
			}
		}
		
    // Doesn't found the matching client for read all phase 1
    return ret;	
	}
	
//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::GetMbmsMonitoredServicesPhase1LCancel
// Cancels Set Always On request
//-----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetMbmsMonitoredServicesCancel( TTsyReqHandle aTsyReqHandle )
	{
	
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETMBMSMONITOREDSERVICESCANCEL_1, "TSY: CMmPacketServiceTsy::GetMbmsMonitoredServicesPhase1LCancel called");
    // Remove the read all attempt
    CListReadAllAttempt* read = NULL;

    // Check that read all array does not contain any data.
    // If it does, delete it.
    // NOTE: functions request mode "Flow control obeyed" currently blocks
    // request so that only one client can request data at the time.
    // Therefore, loop-functionality is not needed.
    if ( iGetMbmsMonitoredServiceLists->Count() > 0 )
        {
        read = iGetMbmsMonitoredServiceLists->At( 0 );
        delete read;
        iGetMbmsMonitoredServiceLists->Delete( 0 );
        }

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;	
	}	
	
//----------------------------------------------------------------------------
// CMmPacketServiceTsy::MBMSContextList
// Returns pointer to MBMS context list.
//----------------------------------------------------------------------------
//
CMmMBMSContextList* CMmPacketServiceTsy::MBMSContextList()
    {
    return iMBMSContextList;
    }
       

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::OpenNewObjectL
// Creates new Context object and returns a pointer to it.
//----------------------------------------------------------------------------
//
CTelObject* CMmPacketServiceTsy::OpenNewObjectL(
    TDes& aNewName )  
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTL_1, "TSY: CMmPacketServiceTsy::OpenNewObjectL. %S", aNewName );
    CTelObject* newContext = NULL;
    TUint8 proxyId( 0 );    
    TInt ret( KErrNone );
             
    if(0 == aNewName.Compare(KMBMSContextName))
        {
        ret = iMBMSContextList->GenerateNewContextName( aNewName, proxyId, iMaxActiveServices );

        if( KErrNone != ret )
            {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTL_2, "TSY: CMmPacketServiceTsy::OpenNewObjectL. Mbms Leaves with: %{TSymbianErrorCodes}", ret );
            User::Leave( ret );            
            }
            
        CMmMBMSContextTsy* context = CMmMBMSContextTsy::NewL( this, aNewName, proxyId ); 
        ret = iMBMSContextList->AddObject( context );
        if( KErrNone != ret )
            {
            // Created context couldn't save.
            // Call Close() to context to remove it completely
            context->Close();
            context = NULL;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTL_3, "TSY: CMmPacketServiceTsy::OpenNewObjectL. Mbms Leaves  with: %{TSymbianErrorCodes}", ret );
            User::Leave( ret );
            }
        else
            {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTL_4, "TSY: CMmPacketServiceTsy::OpenNewObjectL. New MBMS Context Name: %S Proxy Id: %d",aNewName, proxyId );
            newContext = context;            
            }        
        }
    else
        {            
        ret = iPacketContextList->GenerateNewContextName( aNewName, proxyId );

        if ( KErrNone != ret )
            {
            // Reason to leave is that maximum number of Contexts already exists.
            // Leaving from here requires that TSY is configurated with max number
            // of context supported by this phone ( value KMmMaxNumberOfContexts )
            ret = CMmCommonStaticUtility::EpocErrorCode(
                KErrOverflow, KErrUmtsMaxNumOfContextExceededByPhone );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTL_5, "TSY: CMmPacketServiceTsy::OpenNewObjectL. Leaves with: %{TSymbianErrorCodes}", ret );
            User::Leave( ret );
            }
        CMmPacketContextTsy* context = CMmPacketContextTsy::NewL( this, iHostCID, aNewName, proxyId );

    	// reset cotext name 
    	iHostCID.Zero();
        ret = iPacketContextList->AddObject( context, proxyId, 0x00, aNewName );
        if( KErrNone != ret )
            {
            // Created context couldn't save.
            // Call Close() to context to remove it completely
            context->Close();
            context = NULL;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTL_6, "TSY: CMmPacketServiceTsy::OpenNewObjectL. Leaves with: %{TSymbianErrorCodes}", ret );
            User::Leave( ret );
            }
        else
            {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTL_7, "TSY: CMmPacketServiceTsy::OpenNewObjectL. New Context Name: %S Proxy Id: %d",aNewName, proxyId );
            newContext = context;            
            }
        }               
    return newContext;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::OpenNewObjectByNameL
// Returns context object defined in parameter aName.
//----------------------------------------------------------------------------
//
CTelObject* CMmPacketServiceTsy::OpenNewObjectByNameL(
    const TDesC& aName )   
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTBYNAMEL_1, "TSY: CMmPacketServiceTsy::OpenNewObjectByNameL. Context Name: %S", aName );

    // length of object's name cannot be over 16 chars
    if ( 16 < aName.Length() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTBYNAMEL_2, "TSY: CMmPacketServiceTsy::OpenNewObjectByNameL. Leaves with: KErrOverflow");
        User::Leave( KErrOverflow  );
        }

    const TInfoName mmPacketContextName( aName );
    CMmPacketContextTsy* mmPacketContext = NULL;

    if ( mmPacketContextName.Compare( KStringExternal ) == 0 )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTBYNAMEL_3, "TSY: CMmPacketServiceTsy::OpenNewObjectByNameL :Open Existing Dial-up Object." );
        mmPacketContext = iDialUpContext;
        }
    else if ( mmPacketContextName.Compare( KStringExternal2 ) == 0 )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTBYNAMEL_4, "TSY: CMmPacketServiceTsy::OpenNewObjectByNameL :Open Existing Secondary Dial-up Object." );
        mmPacketContext = iSecondaryDialUpContext;
        }
    else
        {
        mmPacketContext =
            iPacketContextList->PacketContextByName( &mmPacketContextName );
        }

    // If not found, Leaves
    if ( NULL == mmPacketContext )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_OPENNEWOBJECTBYNAMEL_5, "TSY: CMmPacketServiceTsy::OpenNewObjectByNameL. Leaves with: KErrNotFound");
        User::Leave( KErrNotFound );
        }

    return mmPacketContext;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::PacketContextList
// Returns pointer to the context list object.
//----------------------------------------------------------------------------
//
CMmPacketContextList* CMmPacketServiceTsy::PacketContextList()
    {
    return iPacketContextList;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::ServiceStatus
// Returns current status of Packet Service.
//----------------------------------------------------------------------------
//
RPacketService::TStatus CMmPacketServiceTsy::ServiceStatus() const
    {
	return iInternalSavedData.iServiceStatus;
    }
//----------------------------------------------------------------------------
// CMmPacketServiceTsy::DialUpContext
// Returns externally created dial-up context.
//----------------------------------------------------------------------------
//
CMmPacketContextTsy* CMmPacketServiceTsy::DialUpContext(
    const TInfoName aContextName )
    {

    CMmPacketContextTsy* mmpacketContext = NULL;

    if ( aContextName.Compare( KStringExternal ) == 0 )
        {
        mmpacketContext = iDialUpContext;
        }

    else if ( aContextName.Compare( KStringExternal2 ) == 0 )
        {
        mmpacketContext =  iSecondaryDialUpContext;
        }

    return mmpacketContext;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::ResetPointerToDialUpContex
// Resets pointer to dial-up context.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::ResetPointerToDialUpContext(
    const TInfoName aContextName )
    {

    if ( aContextName.Compare( KStringExternal ) == 0 )
        {
        iDialUpContext = NULL;
        }

    else if ( aContextName.Compare( KStringExternal2 ) == 0 )
        {
        iSecondaryDialUpContext = NULL;
        }
    }

#ifdef REQHANDLE_TIMER

//---------------------------------------------------------------------------
// CMmPacketServiceTsy::PhoneTsy
// Returns pointer to the phone tsy object.
//---------------------------------------------------------------------------
//
CMmPhoneTsy* CMmPacketServiceTsy::PhoneTsy()
    {
    return iMmPhone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::SetTypeOfResponse
// Sets the type of response for a given Handle.
// Automatic mode includes an automatic response in case of non response from
// the DOS in a specified time.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::SetTypeOfResponse(
    const TInt aReqHandleType ) 
    {

    TInt timeOut( 0 );

    switch ( aReqHandleType )
        {
        case EMultimodePacketServiceAttach:
            timeOut = KMmPacketServiceAttachTimeOut;
            break;
        case EMultimodePacketServiceDetach:
            timeOut = KMmPacketServiceDetachTimeOut;
            break;
        case EMultimodePacketServiceGetNtwkRegStatus:
            timeOut = KMmPacketServiceGetNtwkRegStatusTimeOut;
            break;
        case EMultimodePacketServiceSetPreferredBearer:
            timeOut = KMmPacketServiceSetPreferredBearerTimeOut;
            break;
        case EMultimodePacketServiceRejectActivationRequest:
            timeOut = KMmPacketServiceRejectActivationRequest;
            break;
        case EMultimodePacketServiceSetAttachMode:
            timeOut = KMmPacketServiceSetAttachModeTimeOut;
            break;
        case EMultimodePacketServiceGetAttachMode:
            timeOut = KMmPacketServiceGetAttachModeTimeOut;
            break;
        case EMultimodePacketServiceSetDefaultContextParams:
            timeOut = KMmPacketSetDefaultContextParamsTimeOut;
            break;
        case EMultimodePacketServiceGetMBMSStatus:
            timeOut = KMmPacketServiceGetMbmsNetworkServiceStatusTimeOut;
            break;
        case EMultimodePacketServiceUpdateMBMSMonitorServiceList:
            timeOut = KMmPacketServiceUpdateMbmsMonitorServiceListTimeOut;
            break;            
        // Can't use timer:
        //  - all notifications
        //case EMultimodePacketServiceNotifyNtwkRegStatusChange:
        //case EMultimodePacketServiceNotifyContextActivationRequested:
        //case EMultimodePacketServiceNotifyContextAdded:
        //case EMultimodePacketServiceNotifyDynamicCapsChange:
        //case EMultimodePacketServiceNotifyMSClassChange:
        //case EMultimodePacketServiceNotifyStatusChange:

        default:
            // Does not use timer
            iTsyReqHandleStore->SetTsyReqHandle(
                aReqHandleType, iTsyReqHandle );
            break;
        }

    if ( 0 < timeOut )
        {
        // The timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle(
            aReqHandleType, iTsyReqHandle, timeOut );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::Complete
// Completes the request due timer expiration
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::Complete(
    const CTelObject* const aObject, 
    const TInt aReqHandleType,      
    const TInt aError )              
    {

    if ( this == aObject )
        {
        // Call service's Complete()
        Complete( aReqHandleType, aError );
        }
    else if ( aObject == iDialUpContext && NULL != iDialUpContext )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETE_1, "TSY: CMmPacketServiceTsy::OpenNewObjectByNameL :Complete Dial-up Object Request." );

        iDialUpContext->Complete( aReqHandleType, aError );
        }
    else if ( aObject == iSecondaryDialUpContext 
              && NULL != iSecondaryDialUpContext )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETE_2, "TSY: CMmPacketServiceTsy::OpenNewObjectByNameL :Complete Dial-up Object Request." );

        iSecondaryDialUpContext->Complete( aReqHandleType, aError );

        }
    else
        {

        CMmPacketContextTsy* context;

        const TInt numberOfContexts( iPacketContextList->NumberOfContexts() );

        // Check if the object is a Context object
        for ( TInt i = 0; i < numberOfContexts; i++ )
            {

            // Get context object
            context = iPacketContextList->PacketContextByIndex( i );
            
            if (context == NULL)
            	{
            	continue;
            	}
            // Check if the current context is the object that we are
            // trying to find
            if ( context == aObject )
                {
                // Call context's Complete() method
                context->Complete( aReqHandleType, aError );
                // Object found, break
                return ;
                }

            else if ( context->PacketQoSTsy() == aObject )
                {
                // Call qos's Complete() method
                context->PacketQoSTsy()->Complete( aReqHandleType, aError );

                // Object found, break
                return ;
                }
            }  
           
        CMmMBMSContextTsy* mbms;
        
        const TInt numberOfMbms( iMBMSContextList->GetNumberOfObjects() );
        
        for( TInt i = 0; i < numberOfMbms; i++ )
            {
            mbms = MBMSContextList( )->ContextByIndex( i );
            
            // Check if the current context is the object that we are
            // trying to find
            if ( mbms == aObject )
                {
                // Call context's Complete() method
                mbms->Complete( aReqHandleType, aError );
                // Object found, break
                return ;
                }            
            }          
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::Complete
// Completes the request due timer expiration.
//----------------------------------------------------------------------------

void CMmPacketServiceTsy::Complete(
    const TInt aReqHandleType,    
    const TInt aError )            
    {
    // All possible TSY req handle types are listed in the
    // switch case below.
    switch ( aReqHandleType )
        {

        case EMultimodePacketServiceAttach:
            CMmPacketServiceTsy::CompleteAttach( aError );
            break;
        case EMultimodePacketServiceDetach:
            TRAP_IGNORE( CMmPacketServiceTsy::CompleteDetachL( aError ));
            break;
        case EMultimodePacketServiceGetNtwkRegStatus:
            CMmPacketServiceTsy::CompleteGetNtwkRegStatus( 
                RPacketService::EUnknown, aError );
            break;
        case EMultimodePacketServiceSetPreferredBearer:
            CMmPacketServiceTsy::CompleteSetPreferredBearer( aError );
            break;
        case EMultimodePacketServiceSetAttachMode:
            CMmPacketServiceTsy::CompleteSetAttachMode( aError );
            break;
        case EMultimodePacketServiceSetDefaultContextParams:
            CMmPacketServiceTsy::CompleteSetDefaultContextParams( aError );
            break;
        case EMultimodePacketServiceGetMBMSStatus:
            CMmPacketServiceTsy::CompleteGetMbmsNetworkServiceStatus( NULL, aError );
            break;   
        case EMultimodePacketServiceEnumerateMbmsActiveServices:
            CMmPacketServiceTsy::CompleteEnumerateMbmsActiveServiceList( NULL, aError );
            break;
        case EMultimodePacketServiceUpdateMBMSMonitorServiceList:
           CMmPacketServiceTsy::CompleteUpdateMbmsMonitorServiceList( NULL, aError );
           break;

        // Can't use timer:
        //  - all notifications
        //case EMultimodePacketServiceNotifyNtwkRegStatusChange:
        //case EMultimodePacketServiceNotifyContextActivationRequested:
        //case EMultimodePacketServiceNotifyContextAdded:
        //case EMultimodePacketServiceNotifyDynamicCapsChange:
        //case EMultimodePacketServiceNotifyMSClassChange:
        //case EMultimodePacketServiceNotifyStatusChange:

        default:
            CMmPacketServiceTsy::ReqCompleted( iTsyReqHandleStore->
				ResetTsyReqHandle( aReqHandleType ), aError );
            break;
        }
    }
#endif // REQHANDLE_TIMER


#ifdef TF_LOGGING_ENABLED


//----------------------------------------------------------------------------
// CMmPacketServiceTsy::ReqCompleted
// Overloads CTelObject::ReqCompleted for logging purposes.
// Prints the aTsyReqHandle and aError variable in the log file and then
// calls CTelObject::ReqCompleted.
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::ReqCompleted(
    const TTsyReqHandle aTsyReqHandle,  
    const TInt aError )                   
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_REQCOMPLETED_1, "TSY: CMmPacketServiceTsy::ReqCompleted. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", aTsyReqHandle, aError, this);

    CTelObject::ReqCompleted( aTsyReqHandle, aError );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_REQCOMPLETED_2, "<-- TSY: CMmPacketServiceTsy::ReqCompleted");
    }

#endif // TF_LOGGING_ENABLED


//----------------------------------------------------------------------------
// CMmPacketServiceTsy::PrepareOpenSecondary
// Prepares to open secondary from given primary context
// Next Context that client wants to open is secondary context. Secondary
// context copies configuration from context given by client
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::PrepareOpenSecondary(
    TDes16* aPrimaryContextName )  
    {

    if ( aPrimaryContextName->Length() <= iHostCID.MaxLength() )
        {
        // Check if primary context exists; return KErrArgument if it doesn't
        TInfoName primaryContextName = *aPrimaryContextName;
        CMmPacketContextTsy* mmPacketContextTsy = iPacketContextList->PacketContextByName(&primaryContextName );

        if( NULL == mmPacketContextTsy )
            {
            return KErrArgument;        
            }

        iHostCID = *aPrimaryContextName;
        // Complete request with ret
        CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );
        return KErrNone;
        }

    return KErrArgument;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::EnumerateNifs
// Method returns number of network interfaces
// call method EnumerateNifs from CMmPacketContextList
// complete request to client
// return KErrNone
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::EnumerateNifs(
    TInt* aCount )   
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_ENUMERATENIFS_1, "TSY: CMmPacketServiceTsy::EnumerateNifs." );

    iPacketContextList->EnumerateNifs( aCount );

    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetNifInfo
// Return information about network interface
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetNifInfo(
    TInt* aIndex,                            
    RPacketService::TNifInfoV2* aNifInfoV2 ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETNIFINFO_1, "TSY: CMmPacketServiceTsy::GetNifInfo." );

    TInt ret = iPacketContextList->GetNifInfo( aIndex, aNifInfoV2 );

    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, ret );

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::EnumerateContextsInNif
// Returns number of contexts in Nif
// get existingContextName from parameter aExistingContextName
// call method EnumerateContextsInNif from CMmPacketContextList
// complete request to client
// return KErrNone
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::EnumerateContextsInNif(
    TDesC* aExistingContextName,    
    TInt* aCount )                   
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_ENUMERATECONTEXTSINNIF_1, "TSY: CMmPacketServiceTsy::EnumerateContextsInNif." );

	TInt ret = KErrNone;
	if (aExistingContextName->Length() > KMaxInfoName)
		{
		ret = KErrArgument;
		}
	else
		{		
	    TInfoName existingContextName = *aExistingContextName;

	    iPacketContextList->EnumerateContextsInNif( existingContextName, aCount );

	    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );
		}

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetContextNameInNif
// Returns name of context in network interface
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetContextNameInNif(
    RPacketService::TContextNameInNif* aContextNameInNif,
    TDes* aContextName )                
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETCONTEXTNAMEINNIF_1, "TSY: CMmPacketServiceTsy::GetContextNameInNif." );

    TInt ret = iPacketContextList->GetContextNameInNif( aContextNameInNif,
        aContextName );

    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, ret );

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::SetDefaultContextParams
// Sets default context parameters
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::SetDefaultContextParamsL(
    const TDataPackage& aPackage )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_SETDEFAULTCONTEXTPARAMSL_1, "TSY: CMmPacketServiceTsy::SetDefaultContextParamsL." );

    TInt ret(KErrServerBusy);

    const TTsyReqHandle reqHandle( iTsyReqHandleStore->GetTsyReqHandle(
    EMultimodePacketServiceSetDefaultContextParams ) );

    if ( EMultimodePacketServiceReqHandleUnknown == reqHandle )
        {
		ret = iMmPacketServiceGsmWcdmaExt->
            SetDefaultContextParamsL( aPackage );

        if ( KErrNone == ret )
            {
            // Set req handle
            iReqHandleType = EMultimodePacketServiceSetDefaultContextParams;
            }
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteSetDefaultContextParams
// Completes call SetDefaultContextParameters
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteSetDefaultContextParams(
    TInt aCause )    
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETESETDEFAULTCONTEXTPARAMS_1, "TSY: CMmPacketServiceTsy::CompleteSetDefaultContextParams. aCause: %{TSymbianErrorCodes}", aCause );

    // Reset request handle. Returns the deleted request handle
    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePacketServiceSetDefaultContextParams ) );

    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
        {
        // Complete the client request
        CMmPacketServiceTsy::ReqCompleted( reqHandle, aCause );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GetDefaultContextParams
// Gets default context parameters
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::GetDefaultContextParams(
    TPacketDataConfigBase* aContextConfig )      
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GETDEFAULTCONTEXTPARAMS_1, "TSY: CMmPacketServiceTsy::GetDefaultContextParams." );

    TInt ret = iMmPacketServiceGsmWcdmaExt->GetDefaultContextParams(
        aContextConfig );

    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, ret );

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::IsActivationAllowed
// Returns true if Packet Data is currently allowed
//----------------------------------------------------------------------------
//
TBool CMmPacketServiceTsy::IsActivationAllowed()
    {
    TBool isAllowed = ETrue;

    if ( RMobilePhone::ENetworkModeGsm == iCurrentMode &&
        RPacketService::EMSClassDualMode != iInternalSavedData.iMsClass )
        {
        //Check if the object is a Call object
        for( TInt i = 0; i < iMmPhone->CallList()->GetNumberOfObjects() ; i++ )
            {
            //Get call object
            CMmCallTsy* call = iMmPhone->CallList()->GetMmCallByIndex( i );
            //check if the current call object exist
            if( call != NULL )
                {
                //call the complete method
                RCall::TStatus status = call->Status();

                if ( RCall::EStatusUnknown != status &&
                     RCall::EStatusIdle != status )
                    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_ISACTIVATIONALLOWED_1, "TSY: CMmPacketServiceTsy::IsActivationAllowed. CS call active, Context Activation not allowed" );
                    isAllowed = EFalse;
                    i = iMmPhone->CallList()->GetNumberOfObjects();
                    }
                }
            }
        }

    return isAllowed;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::MessageManagerBase
// returns pointer to CMmMessageManager
//----------------------------------------------------------------------------
//
CMmMessageManagerBase* CMmPacketServiceTsy::MessageManager()
	{
	return iMessageManager;
	}

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteNotifyEGprsInfoChange
// Completes notification t
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteNotifyEGprsInfoChange(
    CMmDataPackage* aDataPackage )		
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENOTIFYEGPRSINFOCHANGE_1, "TSY: CMmPacketServiceTsy::CompleteNotifyEGprsInfoChange." );

	//Unpack data
	TBool lastEdgeGprsSupport;
	aDataPackage->UnPackData( lastEdgeGprsSupport );

	iMmPacketServiceGsmWcdmaExt->SetEdgeGprsSupport( lastEdgeGprsSupport );

	CMmCustomTsy* customTsy = CMmPacketServiceTsy::CustomTsy();

        if ( NULL != customTsy )
            {
            CMmPacketTsy* packetTsy = customTsy->PacketTsy();

            if ( NULL != packetTsy )
                {
                packetTsy->
                    CompleteNotifyEGprsInfoChange( lastEdgeGprsSupport );
                }
            }
	}

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::QosTsy
// Gets pointer to right qos by context name given as parameter.
//----------------------------------------------------------------------------
//
CMmPacketQoSTsy* CMmPacketServiceTsy::QosTsy(
    TInfoName aMmPacketContextName )
	{
	CMmPacketQoSTsy* packetQoSTsy = NULL;

	CMmPacketContextTsy* mmPacketContextTsy = iPacketContextList->
		PacketContextByName( &aMmPacketContextName );

	if ( NULL != mmPacketContextTsy )
		{
		packetQoSTsy = mmPacketContextTsy->PacketQoSTsy();
		}

	if ( NULL != packetQoSTsy )
		{
		return packetQoSTsy;
		}

	else
		{
		return NULL;
		}
	}

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::CompleteNetworkModeChange
// Completes Network Mode Change
//----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteNetworkModeChange(
    CMmDataPackage* aDataPackage )  
    {
    RMobilePhone::TMobilePhoneNetworkMode ntwkMode( 
        RMobilePhone::ENetworkModeUnknown );
    //Unpack data
    aDataPackage->UnPackData( ntwkMode );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETENETWORKMODECHANGE_1, "TSY: CMmPacketServiceTsy::CompleteNetworkModeChange. ntwkMode: %d", ntwkMode );
    iCurrentMode = ntwkMode;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceTsy::Context
// Accessory method to get right context.
//----------------------------------------------------------------------------
//
CMmPacketContextTsy* CMmPacketServiceTsy::ContextTsy(
    TInfoName& contextName )
    {
    CMmPacketContextTsy* mmPacketContextTsy = iPacketContextList->
        PacketContextByName( &contextName );

    if( NULL == mmPacketContextTsy )
        {
        mmPacketContextTsy = iMBMSContextList->ContextByName( contextName );        
        }
    return mmPacketContextTsy;
    }
    
//----------------------------------------------------------------------------
// CMmPacketServiceTsy::GlobalKernelObjectHandle
// Returns unnamed mutex handle owned by TSY 
//----------------------------------------------------------------------------
//
RHandleBase* CMmPacketServiceTsy::GlobalKernelObjectHandle()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GLOBALKERNELOBJECTHANDLE_1, "TSY: CMmPacketServiceTsy::GlobalKernelObjectHandle" );

    if ( KNullHandle == iMutex.Handle() )
        {
        TInt ret = iMutex.CreateLocal( EOwnerProcess );
    
        if ( KErrNone == ret )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GLOBALKERNELOBJECTHANDLE_2, "TSY: CMmPacketServiceTsy::GlobalKernelObjectHandle - Mutex created" );
            }
        else
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GLOBALKERNELOBJECTHANDLE_3, "TSY: CMmPacketServiceTsy::GlobalKernelObjectHandle - Mutex creation failed" );
            }                  
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_GLOBALKERNELOBJECTHANDLE_4, "TSY: CMmPacketServiceTsy::GlobalKernelObjectHandle - Mutex creation skipped" );
        }
         
    return &iMutex;
    }

//-----------------------------------------------------------------------------
// CMmPacketServiceTsy::SetAlwaysOnL
// Requests Set Always On with mode given as parameter aMode
//-----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::SetAlwaysOnL(	
    TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TSetAlwaysOnMode aMode )
	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_SETALWAYSONL_1, "TSY: CMmPacketServiceTsy::SetAlwaysOn Mode = %d", aMode );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_SETALWAYSONL_2,  "TSY: CMmPacketServiceTsy::SetAlwaysOn ReqHandle %d", aTsyReqHandle );
	TInt ret( KErrServerBusy );
	
	//Check if request is allready pending
	TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodePacketSetAlwaysOn );
	
	if( EMultimodePacketServiceReqHandleUnknown == reqHandle )
		{
        //Pack 
		CMmDataPackage data;
		data.PackData( &aMode );
		
		iTsyReqHandleStore->SetTsyReqHandle( 
            EMultimodePacketSetAlwaysOn, aTsyReqHandle );
		
		ret = iMessageManager->HandleRequestL( ECustomSetAlwaysOnMode, &data );
		
		if ( KErrNone != ret)
			{
			iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePacketSetAlwaysOn );
			}
		}
	
	return ret;
	}

//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::CompleteSetAlwaysOn
// Completes Set Always On request 
//-----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteSetAlwaysOn( 
    TInt aError )
	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETESETALWAYSON_1,  "TSY: CMmPacketServiceTsy::CompleteSetAlwaysOn Error %{TSymbianErrorCodes}", aError );

    const TTsyReqHandle reqHandle( iTsyReqHandleStore->ResetTsyReqHandle(
            								  EMultimodePacketSetAlwaysOn ) );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETESETALWAYSON_2,  "TSY: CMmPacketServiceTsy::CompleteSetAlwaysOn ReqHandle %d", reqHandle );

    if ( EMultimodePacketServiceReqHandleUnknown != reqHandle )
    	{
    	CMmCustomTsy* customTsy = CMmPacketServiceTsy::CustomTsy();

        if ( NULL != customTsy )
            {
            CMmPacketTsy* packetTsy = customTsy->PacketTsy();

            if ( NULL != packetTsy )
                {
				packetTsy->CompleteSetAlwaysOn( reqHandle, aError );
                }
            }	
    	}
	}

//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::CancelSetAlwaysOn
// Cancels Set Always On request
//-----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CancelSetAlwaysOn( 
    TTsyReqHandle aTsyReqHandle )
	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_CANCELSETALWAYSON_1,  "TSY: CMmPacketServiceTsy::CancelSetAlwaysOn ReqHandle %d", aTsyReqHandle );
	
	if ( aTsyReqHandle != EMultimodePacketServiceReqHandleUnknown )
        {
        // Reset tsy request handle
        iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePacketSetAlwaysOn );

        // Complete request to client with KErrCancel
        ReqCompleted( aTsyReqHandle, KErrCancel );
        }
	}

//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::EnumerateActiveServiceList
// Enumerates all active services
//-----------------------------------------------------------------------------
//
TInt CMmPacketServiceTsy::EnumerateMbmsActiveServiceList( TInt &aCount, TInt &aMaxAllowed )
    {   
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_ENUMERATEMBMSACTIVESERVICELIST_1,  "TSY: CMmPacketServiceTsy::EnumerateMbmsActiveServiceList. max: %d", aMaxAllowed );
     
    aCount = EnumerateMbmsActiveServices();
    aMaxAllowed = iMaxActiveServices;
    
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_ENUMERATEMBMSACTIVESERVICELIST_2,  "TSY: CMmPacketServiceTsy::EnumerateActiveServiceList Count: %d, Max allowed %d", aCount, aMaxAllowed );
    // Complete the request with KErrNone
    CMmPacketServiceTsy::ReqCompleted( iTsyReqHandle, KErrNone );
    return KErrNone;    
    }
    
//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::EnumerateMbmsActiveServices
// Enumerates all active services
//-----------------------------------------------------------------------------
//   
TInt CMmPacketServiceTsy::EnumerateMbmsActiveServices()
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_ENUMERATEMBMSACTIVESERVICES_1,  "TSY: CMmPacketServiceTsy::EnumerateMbmsActiveService. max: %d", iMaxActiveServices );
    TInt ret( 0 );    
    
    for( TInt i = 0; i< iMBMSContextList->NumberOfContexts(); i++ )
        {
        CMmMBMSContextTsy* context = iMBMSContextList->ContextByIndex( i );
        
        if( RPacketContext::EStatusActive == context->ContextStatus() ||
            RPacketContext::EStatusReceiving == context->ContextStatus() ||
            RPacketContext::EStatusSuspended == context->ContextStatus() ||
            RPacketContext::EStatusDeactivating == context->ContextStatus() )
            {
            TInt count = context->SessionCount();
            if(  0 == count)
	            {
	            ret +=1;	
	            }
	        else
		        {
	            ret += count;	        	
		        }

            }        
        }    
        
    return ret;
    }
    
//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::CompleteEnumerateMbmsActiveServiceList
// Saves the maximum value for active services
//-----------------------------------------------------------------------------
//
void CMmPacketServiceTsy::CompleteEnumerateMbmsActiveServiceList(        
    CMmDataPackage* aDataPackage,
    const TInt aResult)
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICETSY_COMPLETEENUMERATEMBMSACTIVESERVICELIST_1,  "TSY: CMmPacketServiceTsy::CompleteEnumerateMbmsActiveServiceList. Error: %{TSymbianErrorCodes}", aResult );
           
    if( KErrNone == aResult && NULL != aDataPackage )
        {
        aDataPackage->UnPackData( iMaxActiveServices );
        }       
    }
    
//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::MaximumActiveServices
// Maximum number of active services
//-----------------------------------------------------------------------------
//    
TInt CMmPacketServiceTsy::MaximumActiveServices()
    {
    return iMaxActiveServices;
    }

    
//-----------------------------------------------------------------------------
// TBool CMmPacketServiceTsy::ResetReqHandle
// Resets request handle in the req handle store
//-----------------------------------------------------------------------------
// 
TBool CMmPacketServiceTsy::ResetReqHandle( const TTsyReqHandle aTsyReqHandle )
    {
    return iTsyReqHandleStore->FindAndResetTsyReqHandle( aTsyReqHandle );
    }

//  End of File

