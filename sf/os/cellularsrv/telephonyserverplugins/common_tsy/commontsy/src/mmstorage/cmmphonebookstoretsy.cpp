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
#include "cmmphonebookstoretsyTraces.h"
#endif

#include "cmmphonebookstoretsy.h"
#include "cmmpblist.h"
#include "cmmphonebookstoregsmext.h"
#include "CMmCustomTsy.h"
#include <ctsy/rmmcustomapi.h>
#include "cmmmessagemanagerbase.h"
#include "cmmtsyreqhandlestore.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/gsmerror.h>


// ======== MEMBER FUNCTIONS ========

CMmPhoneBookStoreTsy::CMmPhoneBookStoreTsy():
    iReqHandleType(EMultimodePhoneBookStoreReqHandleUnknown)
    {
    }

void CMmPhoneBookStoreTsy::ConstructL( const TDesC& aName )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CONSTRUCTL_1, "TSY: CMmPhoneBookStoreTsy::ConstructL - entered");
#ifdef REQHANDLE_TIMER
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this,
        iMmPhoneTsy,
        EMmTsyNumOfStorageRequests,
        iStorageReqHandles);
#else
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL(
        EMmTsyNumOfStorageRequests, iStorageReqHandles );

#endif

    iInitError = KErrNone;
    // Get mode of current extension.
    RMobilePhone::TMobilePhoneNetworkMode currentMode;
    iMmPhoneTsy->GetStaticExtensionMode( &currentMode );

    // Reset all CMmPhoneBookStoreTsy variables
    ResetVariables();


    // Create GSM ENStore objects
    iMmPhoneBookStoreExtInterface =
        static_cast<CMmPhoneBookStoreExtInterface*>
            (CMmPhoneBookStoreGsmExt::NewL( this ) );

    // Set phonebook type by name
    SetPhoneBookType( aName );
    // Set extension mode
    SetActivePBExtension( iMmPhoneBookStoreExtInterface );
    // Set phonebookrelated name and number max values
    SetMaxNameAndNumLenght();

OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CONSTRUCTL_2, "TSY: CMmPhoneBookStoreTsy::ConstructL - PB %S", iPhoneBookName);

    // Used entries is zero in the start
    iUsedEntries = 0;

    // Create bootState
    CMmPhoneTsy::TNosBootState* bootState = iMmPhoneTsy->NosBootState();

    //Create Read Store
    iReadReqStore = new ( ELeave ) TReadRequestStore();
    //Init
    iRetFillBufferedDes = KErrNone;
    iReadReqStore->iCaching = EFalse;
    iRequestedOnPBRead = EFalse;
    iIsPBInitCompleted = EFalse;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CONSTRUCTL_3, "TSY: CMmPhoneBookStoreTsy::ConstructL - iIsPhonebookInitialized: %u", (TUint)iStoreInfoData->iIsPhonebookInitialized);
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CONSTRUCTL_4, "TSY: CMmPhoneBookStoreTsy::ConstructL - iSIMReady: %u", (TUint)bootState->iSIMReady);
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CONSTRUCTL_5, "TSY: CMmPhoneBookStoreTsy::ConstructL - before initialization");
    if(bootState->iSIMReady)
        {
        SimIsReady();
        }
    else
        {
        iIsPBInitCompleted = EFalse;
        }
    }

CMmPhoneBookStoreTsy* CMmPhoneBookStoreTsy::NewL(
    CMmPhoneTsy* aMmPhone,
    const TDesC& aName )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_NEWL_1, "TSY: CMmPhoneBookStoreTsy::NewL - entered");
    
    CMmPhoneBookStoreTsy* mmPhoneBookStoreTsy =
        new ( ELeave ) CMmPhoneBookStoreTsy();

    CleanupClosePushL( *mmPhoneBookStoreTsy );

    mmPhoneBookStoreTsy->iMmPhoneTsy = aMmPhone;
    mmPhoneBookStoreTsy->iPhoneBookName = aName;
    mmPhoneBookStoreTsy->ConstructL( aName );

    CleanupStack::Pop( mmPhoneBookStoreTsy );

    return mmPhoneBookStoreTsy;
    }


CMmPhoneBookStoreTsy::~CMmPhoneBookStoreTsy()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DTOR_1, "TSY: CMmPhoneBookStoreTsy::~CMmPhoneBookStoreTsy - PB %S", iPhoneBookName);

    if ( iMmPhoneTsy )
        {
        CMmPhoneTsy::TNosBootState* bootState = iMmPhoneTsy->NosBootState();
        
        if ( iMmPhoneTsy->GetCustomTsy() )
            {
            RMmCustomAPI::TPndCacheStatus fdnCacheStatus = 
            iMmPhoneTsy->GetCustomTsy()->GetPhonebookCacheStatus( KFDNPhoneBook );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DTOR_2, "TSY: CMmPhoneBookStoreTsy::~CMmPhoneBookStoreTsy - FDN cachestatus %d", fdnCacheStatus);

            RMmCustomAPI::TPndCacheStatus adnCacheStatus =
            iMmPhoneTsy->GetCustomTsy()->GetPhonebookCacheStatus( KADNPhoneBook );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DTOR_3, "TSY: CMmPhoneBookStoreTsy::~CMmPhoneBookStoreTsy - ADN cachestatus %d", adnCacheStatus);

            // Check if the caching is active
            if ( bootState->iCachingActive )
                {
                // Check if the phonebook type is either ADN or FDN
                if ( KFDNPhoneBook == iPhoneBookType
                        || KADNPhoneBook == iPhoneBookType )
                    {
                    // Check if the cache status is either failed or not ready
                    if ( RMmCustomAPI::ECacheNotReady == fdnCacheStatus
                            || RMmCustomAPI::ECacheFailed == fdnCacheStatus
                            || RMmCustomAPI::ECacheNotReady == adnCacheStatus 
                            || RMmCustomAPI::ECacheFailed == adnCacheStatus )
                        {
                        //Create package
                        CPhoneBookDataPackage package;
                        // Set package data
                        package.SetPhoneBookName( iPhoneBookName );
                        TRAP_IGNORE( iMmPhoneTsy->MessageManager()->HandleRequestL( 
                                EMmTsyPhoneBookStoreCacheCancelIPC, &package ); );
                        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DTOR_4, "TSY: CMmPhoneBookStoreTsy::~CMmPhoneBookStoreTsy - CacheCancel for %d", iPhoneBookType);
                        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DTOR_5, "TSY: CMmPhoneBookStoreTsy::~CMmPhoneBookStoreTsy - CachingActive = EFalse");

                        bootState->iCachingActive = EFalse;
                        }
                    }

                if ( (( RMmCustomAPI::ECacheNotReady == fdnCacheStatus
                        || RMmCustomAPI::ECacheFailed == fdnCacheStatus ) && (KADNPhoneBook == iPhoneBookType) )
                        || (( RMmCustomAPI::ECacheNotReady == adnCacheStatus
                                || RMmCustomAPI::ECacheFailed == adnCacheStatus )) && (KFDNPhoneBook == iPhoneBookType) )  

                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DTOR_6, "TSY: CMmPhoneBookStoreTsy::~CMmPhoneBookStoreTsy - CachingActive = ETrue");
                    // Set the caching back to active
                    bootState->iCachingActive = ETrue;                 
                    }
                }
            }
        }
    if ( iTsyReqHandleStore )
        {
        delete iTsyReqHandleStore;
        iTsyReqHandleStore = NULL;
        }

    if ( iMmPhoneTsy )
        {
        // Remove this phonebook from the list of PB's
        iMmPhoneTsy->PBList()->RemoveObject( this );
        }

    // Delete PhoneBookStoreGsmExt
    if ( iMmPhoneBookStoreExtInterface )
        {
        delete iMmPhoneBookStoreExtInterface;
        iMmPhoneBookStoreExtInterface = NULL;
        }

    // Delete phonebook write entry
    if ( iWriteEntry )
        {
        delete iWriteEntry;
        iWriteEntry = NULL;
        }

    // Delete phonebook delete entry
    if ( iDeleteEntry )
        {
        delete iDeleteEntry;
        iDeleteEntry = NULL;
        }
    
    // Delete cache
    if ( iPBStoreCache )
        {
		/*
		 * When using the NON-DISPATCHER version, CTSY copies overthe contents of the LTSYs Phonebook Store Cache.
		 * This is becasue the LTSY Phonebook Store Cache is destroyed when the complete request of the phonebook
		 * store read cache is finished. CTSY needs this information to stay alive longer. The problem doesn't exist
		 * when using the dispatcher as this information is contained within it for the lifetime of the dispatcher
		 * or until it's destroyed but CTSY is aware of this.
		 */
#ifndef USING_CTSY_DISPATCHER
        iPBStoreCache->ResetAndDestroy();
        delete iPBStoreCache;
#endif //USING_CTSY_DISPATCHER
        iPBStoreCache = NULL;
        }
    
    //if new cache was requested on Read, Store must be deleted here
    if ( iReadReqStore )
        {
        delete iReadReqStore;
        iReadReqStore = NULL;
        }

    // Set cache ready status to Not Ready
    ResetCacheReady();
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::Phone
// Returns pointer to the Multimode Phone object. For internal use.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneTsy* CMmPhoneBookStoreTsy::Phone()
    {
    return iMmPhoneTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::Init
// Initialisation method that is called from ETel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::Init()
    {
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::OpenNewObjectByNameL
// This method is not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmPhoneBookStoreTsy::OpenNewObjectByNameL( const TDesC& )
    {
    User::Leave( KErrNotSupported );

    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::OpenNewObjectL
// This method is not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmPhoneBookStoreTsy::OpenNewObjectL( TDes& )
    {
    User::Leave( KErrNotSupported );

    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ReqModeL
// ReqModeL is called from the server's CTelObject::ReqAnalyserL
// in order to check the type of request it has.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmPhoneBookStoreTsy::ReqModeL(
    const TInt aIpc )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_REQMODEL_1, "TSY: CMmPhoneBookStoreTsy::ReqModeL IPC:%{TIPCNamesList}", aIpc);

    CTelObject::TReqMode ret = 0;
    switch ( aIpc )
        {
        // Flow Controlled Services
        case EMobilePhoneStoreDelete:
        case EMobilePhoneStoreDeleteAll:
            ret=KReqModeFlowControlObeyed;
            break;
        // Multiple Completion Services with Immediate Server Repost
        // (Usually Notifications)
        case EMobilePhoneStoreNotifyStoreEvent:
            ret = KReqModeMultipleCompletionEnabled |
                KReqModeRePostImmediately;
            break;

        // Methods that can propably take a long time and cannot therefore be
        // flow controlled. Solution: All these methods must check req handle
        // table before handling the request. In case that the request table
        // indicates that same method has been called and has not been
        // completed, the method should return KErrServerBusy.
        case EMobilePhoneBookStoreRead:
        case EMobilePhoneBookStoreWrite:
            ret = 0;
            break;
        // Methods that are using DOS but have own flow control system
        case EMobilePhoneStoreGetInfo:
            ret = 0;
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::RegisterNotification
// RegisterNotification is called when the server recognises
// that this notification is being posted for the first time on this
// sub-session object. It enables the TSY to "turn on" any regular
// notification messages that it may receive from DOS. Currently does not
// really do anything but returns KErrNone to ETel server in case of known
// notification request type.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::RegisterNotification(
    const TInt aIpc )
    {
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobilePhoneStoreNotifyStoreEvent:
            // KErrNone
            break;
        default:
            // Unknown or invalid IPC
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::DeregisterNotification
// DeregisterNotification is called when the server recognises
// that this notification will not be posted again because the last client
// to have a handle on this sub-session object has just closed the handle.
// It enables the TSY to "turn off" any regular notification messages that
// it may receive from DOS. Currently does not really do anything but returns
// KErrNone to ETel server in case of known notification request type.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::DeregisterNotification(
    const TInt aIpc )
    {
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobilePhoneStoreNotifyStoreEvent:
            // KErrNone
            break;
        default:
            // Unknown or invalid IPC
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::NumberOfSlotsL
// NumberOfSlotsL is called by the server when it is registering a
// new NOTIFICATION. It enables the TSY to tell the server
// how many buffer slots to allocate for "repost immediately" notifications
// that may trigger before clients collect them.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    TInt numberOfSlots = 1; // number of slots

    switch ( aIpc )
        {
        case EMobilePhoneStoreNotifyStoreEvent:
            numberOfSlots = KMmPhoneStoreNotifyStoreEventSlots;
            break;
        default:
            // Unknown or invalid Storage IPC
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CancelService
// CancelService is called by the server when it is
// "cleaning-up" any still outstanding asynchronous requests before closing
// a client's sub-session. This will happen if a client closes its R-class
// handle without cancelling outstanding asynchronous requests. Only
// Mobile API requests are handled here.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::CancelService(
    const TInt aIpc,
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CANCELSERVICE_1, "TSY: CMmPhoneBookStoreTsy::CancelService IPC:%{TIPCNamesList} Handle:%d", aIpc, aTsyReqHandle);

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobilePhoneStoreGetInfo:
            ret = GetInfoCancel( aTsyReqHandle );
            break;
        case EMobilePhoneBookStoreRead:
            ret =  ReadCancel( aTsyReqHandle );
            break;
        case EMobilePhoneStoreDelete:
        case EMobilePhoneBookStoreWrite:
        case EMobilePhoneStoreDeleteAll:
            // These request can't be cancelled, because no
            // functionality for cancelling. So everything
            // to do, is let the request continue and return KErrNone.
            ret = KErrNone;
            break;
        case EMobilePhoneStoreNotifyStoreEvent:
            ret =  NotifyStoreEventCancel( aTsyReqHandle );
            break;
        default:
            break;
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CANCELSERVICE_2, "TSY: CMmPhoneBookStoreTsy::CancelService return: %{TSymbianErrorCodes}", ret);

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ExtFunc
// Dispatches extension function requests. All functions that are not
// part of the core function set, are routed via the ExtFunc method.
// At the beginning of this method, packaged pointers are unpacked. Then
// the correct function is selected using the IPC number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::ExtFunc(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_EXTFUNC_1, "TSY: CMmPhoneBookStoreTsy::ExtFunc IPC:%{TIPCNamesList} Handle:%d", aIpc, aTsyReqHandle);

    TInt ret = KErrNone;
    TInt trapError = KErrNone;

    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==EMultimodePhoneBookStoreReqHandleUnknown, User::Invariant());


    TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ); );

    if ( KErrNone != trapError )
        {        
		// Reset request handle to indicate the request is no longer ongoing
        iTsyReqHandleStore->FindAndResetTsyReqHandle( aTsyReqHandle );
        ReqCompleted( aTsyReqHandle, trapError );
        }
    else if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else if ( EMultimodePhoneBookStoreReqHandleUnknown != iReqHandleType )
        {
        // Save request handle
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, aTsyReqHandle );
#endif
        }
    // We've finished with this value now. Clear it so it doesn't leak
    //  up to any other instances of this method down the call stack
    iReqHandleType = EMultimodePhoneBookStoreReqHandleUnknown;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::DoExtFuncL
// ExtFunc is called by the server when it has a "extended",
// i.e. non-core ETel request for the TSY. To process a request handle,
// request type and request data are passed to the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DOEXTFUNCL_1, "TSY: CMmPhoneBookStoreTsy::DoExtFuncL IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    TAny* dataPtr=aPackage.Ptr1();
    TAny* dataPtr2=aPackage.Ptr2();
    TInt result(KErrNotSupported);

    switch ( aIpc )
        {
        //
        // PhoneBook Storage Requests
        //
        case EMobilePhoneStoreRead:
        case EMobilePhoneStoreWrite:
            ReqCompleted( aTsyReqHandle, KErrNotSupported );
            result = KErrNone;
            break;
        case EMobilePhoneBookStoreRead:
            result = ReadL( aTsyReqHandle,
                REINTERPRET_CAST(
                RMobilePhoneBookStore::TPBIndexAndNumEntries*, dataPtr),
                aPackage.Des2n() );
            break;
        case EMobilePhoneBookStoreWrite:
            result = WriteL( aTsyReqHandle, aPackage.Des1n(),
                REINTERPRET_CAST( TInt*, dataPtr2 ) );
            break;
        case EMobilePhoneStoreDelete:
            result = DeleteL( aTsyReqHandle,
                REINTERPRET_CAST( TInt*, dataPtr ) );
            break;
        case EMobilePhoneStoreDeleteAll:
            result = DeleteAllL( aTsyReqHandle );
            break;
        case EMobilePhoneStoreGetInfo:
            result = GetInfoL( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EMobilePhoneStoreNotifyStoreEvent:
            result = NotifyStoreEvent( REINTERPRET_CAST(
                RMobilePhoneStore::TMobileStoreEvent*, dataPtr ),
                REINTERPRET_CAST( TInt*, dataPtr2 ) );
            break;
        default:
            result = KErrNotSupported;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DOEXTFUNCL_2, "TSY: CMmPhoneBookStoreTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", result);
    return result;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::PhoneBookName
// This method returns the phonebook name of the
// current storage object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TName* CMmPhoneBookStoreTsy::PhoneBookName()
    {
    return &iPhoneBookName;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::SetPhoneBookType
// This method sets the phonebook type by the name of the
// current storage object.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::SetPhoneBookType(
    const TDesC& aName )
    {
    if ( !aName.CompareF( KETelIccAdnPhoneBook ) )
        {
        iPhoneBookType = KADNPhoneBook; //ADN
        }
    else if ( !aName.CompareF( KETelIccBdnPhoneBook ) )
        {
        iPhoneBookType = KBDNPhoneBook; //BDN
        }
    else if ( !aName.CompareF( KETelIccSdnPhoneBook ) )
        {
        iPhoneBookType = KSDNPhoneBook; //SDN
        }
    else if ( !aName.CompareF( KETelIccFdnPhoneBook ) )
        {
        iPhoneBookType = KFDNPhoneBook; //FDN
        }
    else if ( !aName.CompareF( KETelIccVoiceMailBox ) )
        {
        iPhoneBookType = KVMBXPhoneBook; //VMBX
        }
    else if ( !aName.CompareF( KETelIccMbdnPhoneBook ) )
        {
        iPhoneBookType = KMBDNPhoneBook; //MBDN
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SETPHONEBOOKTYPE_1,  "TSY: CMmPhoneBookStoreTsy::SetPhoneBookType - Type: %d", iPhoneBookType);
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CompletePBStoreInitializationL
// Completes Phonebook initialization
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CompletePBStoreInitializationL(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEPBSTOREINITIALIZATIONL_1, "TSY: CMmPhoneBookStoreTsy::CompletePBStoreInitializationL - Result: %{TSymbianErrorCodes}",aResult );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEPBSTOREINITIALIZATIONL_2, "TSY: CMmPhoneBookStoreTsy::CompletePBStoreInitializationL - PhoneBookType: %u",iPhoneBookType );

    // If initialization made successfully
    // fill phonebook related static data
    if ( KErrNone == aResult )
        {
        // Response information
        CStorageInfoData* pbInitResp;
        //unpack data
        aDataPackage->UnPackData( &pbInitResp );

        // Set data got from phonebook initialization
        *iStoreInfoData = *pbInitResp;
        // Reset initialization value
        iStoreInfoData->iIsPhonebookInitialized = ETrue;
        iStoreInfoData->iIsPhonebookInitializeFailed = EFalse;
        iIsPBInitCompleted = ETrue;
        iInitError = aResult;
        SetMaxNameAndNumLenght();
        // Just in case here, if cache request has been come
        // before initialization is finished.
        CacheEntriesL();       
       }
    else
    //Initialisation has failed
        {
        CMmCustomTsy* mmCustom = iMmPhoneTsy->GetCustomTsy();
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEPBSTOREINITIALIZATIONL_3, "TSY: CMmPhoneBookStoreTsy::CompletePBStoreInitializationL has failed" );

        if ( NULL != mmCustom )
            {
            //complete
            mmCustom->CompleteNotifyPhoneBookCacheReady
                ( iPhoneBookName, KErrGeneral );

            //Update cache status with the right "cache value"
            mmCustom->UpdateCacheStatus( RMmCustomAPI::ECacheFailed,
                iPhoneBookName );

            // Even if PB initialization has failed, set the member variable
            // to ETrue. Certain processes like FDN cache does not start if
            // this is EFalse.
            iStoreInfoData->iIsPhonebookInitialized = ETrue;

            // Initializing should be tried again later
            iStoreInfoData->iIsPhonebookInitializeFailed = ETrue;
            }
        }
    
    
	if( iStoreInfoData->iIsPhonebookInitialized )
		{
		TTsyReqHandle getInfoHandle =
			iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePhoneStoreGetInfo );
		if ( EMultimodePhoneBookStoreReqHandleUnknown != getInfoHandle )
			{
			iReqHandleType = EMultimodePhoneBookStoreReqHandleUnknown;
			RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg info(*iPhoneBookInfoChanged);
			TInt result = GetInfoL(getInfoHandle, &info);
			if ( KErrNone != result )
				{
				ReqCompleted( getInfoHandle, result );
				}
				// Save request handle
			if ( EMultimodePhoneBookStoreReqHandleUnknown != iReqHandleType )
				{
#ifdef REQHANDLE_TIMER
				SetTypeOfResponse( iReqHandleType, getInfoHandle );
#else
				iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, getInfoHandle );
#endif
				iReqHandleType = EMultimodePhoneBookStoreReqHandleUnknown;
				}
			
			}
		}
	iMmPhoneTsy->PhoneBookStoreInitCompleteL(iInitError);

    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::SetMaxNameAndNumLenght
// Sets maximum name & number length for the current phonebook
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::SetMaxNameAndNumLenght()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SETMAXNAMEANDNUMLENGHT_1, "TSY: CMmPhoneBookStoreTsy::SetMaxNameAndNumLenght");
    switch ( iPhoneBookType )
        {
        case KADNPhoneBook:
            iMaxNumLen = iStoreInfoData->iADNNumberLengthMax;
            iMaxTextLen = iStoreInfoData->iADNTextLengthMax;
            iMultipleReadPosTo = iStoreInfoData->iADNNumOfEntries;
            break;
        case KFDNPhoneBook:
            iMaxNumLen = iStoreInfoData->iFDNNumberLengthMax;
            iMaxTextLen = iStoreInfoData->iFDNTextLengthMax;
            iMultipleReadPosTo = iStoreInfoData->iFDNNumOfEntries;
            break;
        case KSDNPhoneBook:
            iMaxNumLen = iStoreInfoData->iSDNNumberLengthMax;
            iMaxTextLen = iStoreInfoData->iSDNTextLengthMax;
            iMultipleReadPosTo = iStoreInfoData->iSDNNumOfEntries;
            break;
        case KVMBXPhoneBook:
            iMaxNumLen = iStoreInfoData->iVMBXNumberLengthMax;
            iMaxTextLen = iStoreInfoData->iVMBXTextLengthMax;
            iMultipleReadPosTo = iStoreInfoData->iVMBXNumOfEntries;
            break;
        case KMBDNPhoneBook:
            iMaxNumLen = iStoreInfoData->iMBDNNumberLengthMax;
            iMaxTextLen = iStoreInfoData->iMBDNTextLengthMax;
            iMultipleReadPosTo = iStoreInfoData->iMBDNNumOfEntries;
            break;
        case KBDNPhoneBook:
        default:
            iMaxNumLen = 0;
            iMaxTextLen = 0;
            iMultipleReadPosTo = 0;
            break;
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ReadL
// This method reads one or more entries from the
// current storage.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::ReadL(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhoneBookStore::TPBIndexAndNumEntries* aIndexAndEntries,
    TDes8* aPBData, TBool aContinuedRead )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_1, "TSY: CMmPhoneBookStoreTsy::ReadL");

    // Initialise ret to KErrServerBusy
    TInt ret = KErrServerBusy;

    // Check if request handle already exists
    TTsyReqHandle phoneBookStoreReadHandle =
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodePhoneBookStoreRead );
    
    // If phoneBookStoreReadHandle is already set, it means that a read request is in progress.
    // New (second simultaneous) read request is blocked and KErrServerBusy is returned.
    // When read is requested after unsuccessful sim refresh registration, ReadL function is called
    // as call back from ContinueReadL. In this case, phoneBookStoreReadHandle check should be ignored
    // since the handle will already be set. This is done by checking the value of aContinuedRead which
    // is set ETrue when ReadL is invoked from ContinueReadL.
    if ( !aContinuedRead && EMultimodePhoneBookStoreReqHandleUnknown != phoneBookStoreReadHandle )
        {
        // If request handle already exists return KErrServerBusy
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_2, "TSY: CMmPhoneBookStoreTsy::ReadL - Handle already exists");
        ReqCompleted( aTsyReqHandle, ret );
        }
    // check the buffer size
    else if ( sizeof(CPhoneBookStoreEntry) > aPBData->MaxLength() )
        {
        ReqCompleted( aTsyReqHandle, KErrArgument );
        }
    //Caching needs to be done only in case of ADN or FDN phonebook.
    else if( !iMmPhoneTsy->GetSimRefreshRegister() &&
        !( iReadReqStore->iCaching ) &&
        (KADNPhoneBook == iPhoneBookType || KFDNPhoneBook == iPhoneBookType))
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_3, "TSY: CMmPhoneBookStoreTsy::ReadL - SIM refresh registration failed Cache info can be corrupted - ");

        //Store Read req params for new request
        iReadReqStore->iReqHandle = aTsyReqHandle;
        iReadReqStore->iIndexAndEntries = aIndexAndEntries;
        iReadReqStore->iPBData = aPBData;
        iReadReqStore->iCaching = ETrue;

        //Start new cache
        ret = iMmPhoneTsy->StartNewCacheL( iPhoneBookType );
        iRequestedOnPBRead = ETrue;
        //Store req handle
        iTsyReqHandleStore->SetTsyReqHandle(
            EMultimodePhoneBookStoreRead, aTsyReqHandle );
        }

    else
        {
        if ( ( KADNPhoneBook == iPhoneBookType ||
                KFDNPhoneBook == iPhoneBookType ) &&
             ( !iStoreInfoData->iIsPhonebookInitialized || !iCacheReady ) )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_4, "TSY: CMmPhoneBookStoreTsy::ReadL - Initiliszation ongoing -> Read not possible");
            // If phonebook initialisation is not ready for AND and FDN phonebooks,
            // complete with KErrInUse. This is done becauce TSY can not handle
            // multible simultaneously request to certain phonebook.
            // (No dynamic transaktion Ids)
            ret = KErrInUse;
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            ret = KErrNone;
            TBool isOneEntryRequest = ETrue;
            iNumEntriesFilled = 0;
            iIndexAndEntries = aIndexAndEntries;
            TBool cacheRequest = EFalse;

            iIndexToRead = aIndexAndEntries->iIndex;

        // If user supplies index which is less than 0, complete with
        // KErrArgument each record are numbered from 0 to maximum
        // number of records.
        if ( ( 1 > iIndexToRead ) || ( 1 > aIndexAndEntries->iNumSlots ) )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_5, "TSY: CMmPhoneBookStoreTsy::ReadL - Index is less than 1 or aIndexAndEntries->iNumSlots is less then 1" );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_6, "TSY: CMmPhoneBookStoreTsy::ReadL - Index %d ", iIndexToRead);
            iIndexAndEntries = NULL;
            iIndexToRead = 0;
            ReqCompleted( aTsyReqHandle, KErrArgument );
            }
        else if ( KBDNPhoneBook != iPhoneBookType && 
                  aIndexAndEntries->iNumSlots > iMultipleReadPosTo  )
            {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_7, "TSY: CMmPhoneBookStoreTsy::ReadL - iPhoneBookType %u: ", iPhoneBookType);
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_8, "TSY: CMmPhoneBookStoreTsy::ReadL - iNumSlots: %d > iMultipleReadPosTo: %d", aIndexAndEntries->iNumSlots, iMultipleReadPosTo );
            iIndexAndEntries = NULL;
            iIndexToRead = 0;
            ReqCompleted( aTsyReqHandle, KErrNotFound );
            }
        else
            {
            iNumEntriesToRead = aIndexAndEntries->iNumSlots;

            if ( iNumEntriesToRead > 1 )
                {
                isOneEntryRequest = EFalse;
                }

            iBufferToRead = aPBData;

            // If not cache request
            if ( ( isOneEntryRequest ) || ( KBDNPhoneBook == iPhoneBookType
                || KSDNPhoneBook == iPhoneBookType
                || KVMBXPhoneBook == iPhoneBookType
                || KMBDNPhoneBook == iPhoneBookType ) )
                {
                ret = iMmPhoneBookStoreExtInterface->ReadL(
                EMmTsyPhoneBookStoreReadIPC, iPhoneBookName,
                aIndexAndEntries);
                }
            // If cache request, but cache is not yet ready
            else if ( ( NULL == iPBStoreCache ) || !iCacheReady )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_9, "TSY: CMmPhoneBookStoreTsy::ReadL - Cache is not yet ready" );
                ret = KErrNotReady;
                }
            // Set cache request for completion handling
            else
                {
                cacheRequest = ETrue;

                if ( ( iNumEntriesToRead ) > iMultipleReadPosTo )
                    {
                    ret = KErrArgument;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_10, "TSY: CMmPhoneBookStoreTsy::ReadL - KErrArgument" );
                    }
                }

            // Error when doing Read -request or error in parameters
            if ( KErrNone != ret )
                {
                // Null CTSYs pointer to LTSY owned array
				iPBStoreReadEntries = NULL;
                    
                ReqCompleted( aTsyReqHandle, ret );
                }
            // Set request type and handle type for non-cached request
            else if ( EFalse == cacheRequest )
                {
                // Save tsy req handle type
                // if not store at new cache request
                if( !iReadReqStore->iCaching )
                    {
                    iReqHandleType = EMultimodePhoneBookStoreRead;
                    }
                }
            // Handle cached request
            else
                {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_11, "TSY: CMmPhoneBookStoreTsy::ReadL - iIndexToRead %d ", iIndexToRead);
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_12, "TSY: CMmPhoneBookStoreTsy::ReadL - iNumSlots %d ", iNumEntriesToRead);
                ReadEntriesFromArrayL( EPBStoreCacheArray );
                aIndexAndEntries->iNumSlots = iNumEntriesFilled;
                //This condition will arise when client has sent a buffer insufficient to
                //hold even one entry. KErrArgument has to be returned in this case.
                if ( ( iRetFillBufferedDes == KErrOverflow ) && ( iNumEntriesFilled == 0 ) )
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_13, "TSY: CMmPhoneBookStoreTsy::ReadL - Insufficient buffer to hold even 1 entry." );
                    ret = KErrArgument;
                    }
                else if ( 0 == iNumEntriesFilled )
                    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READL_14, "TSY: CMmPhoneBookStoreTsy::ReadL - iNumEntriesFilled is 0 " );
                    ret = KErrNotFound;
                    }
                ReqCompleted( aTsyReqHandle, ret );
                }
            }
        }
        //New cache is done and read is done
        if( !(KErrOverflow == iRetFillBufferedDes ) )
            {
            //Reset caching info for Read req store
            iReadReqStore->iCaching = EFalse;
            }

        //If sim refresh registration is failed try to register
        if( !iMmPhoneTsy->GetSimRefreshRegister() )
            {
            iMmPhoneTsy->SimRefreshRegisterL();
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CompleteReadL
// This method completes read -request when a entry has been read
// from the (U)SIM
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CompleteReadL(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEREADL_1, "TSY: CMmPhoneBookStoreTsy::CompleteReadL - aResult: %{TSymbianErrorCodes}",aResult );

    TInt errCode = aResult;
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneBookStoreRead ); 

    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        // Fill buffered data with entry if no errors
        if ( KErrNone == aResult )
            {
			aDataPackage->UnPackData(iPBStoreReadEntries); 
            ReadEntriesFromArrayL( EPBStoreReadEntriesArray );
            //This condition will arise when client has sent a buffer insufficient to
            //hold even one entry. KErrArgument has to be returned in this case.
            if ( ( iRetFillBufferedDes == KErrOverflow ) && ( iNumEntriesFilled == 0 ) )
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEREADL_2, "TSY: CMmPhoneBookStoreTsy::CompleteReadL - Insufficient buffer to hold even 1 entry." );
                errCode = KErrArgument;
                }
            }
        iIndexAndEntries->iNumSlots = iNumEntriesFilled;
        }

    // Reset class variables for read request
    iIndexToRead = 0;
    iNumEntriesToRead = 0;
    iNumEntriesFilled = 0;
    iIndexAndEntries = NULL;
    iBufferToRead = NULL;

    // Complete only if request is not cancelled or handle is valid
    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, errCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CountEntriesL
// This method is part of GetInfo functionality, and it is used
// to count the num of used entries for SDN and VMBX.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::CountEntriesL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COUNTENTRIESL_1, "TSY: CMmPhoneBookStoreTsy::CountEntriesL called");

    // before phonebook requests, phonebook must be initialized
    TInt returnValue( KErrNone );
    returnValue = iMmPhoneBookStoreExtInterface->
        CountPhonebookEntriesL( EMmTsyPhoneBookStoreGetInfoIPC,
        iPhoneBookName );

    return returnValue;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CompleteCountEntries
// This method is part of GetInfo functionality, and it is used
// to complete count the num of used entries for SDN and VMBX.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CompleteCountEntries(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECOUNTENTRIES_1, "TSY: CMmPhoneBookStoreTsy::CompleteCountEntries - Result: %{TSymbianErrorCodes}", aResult );
    TInt numOfEntries( 0 );

    //reset request handle. Returns the deleted req handle.
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneStoreGetInfo );

    if ( KErrNone == aResult && EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        // Fill if no errors and reqHandle is valid
        aDataPackage->UnPackData( numOfEntries );
        iUsedEntries = numOfEntries;
        iPhoneBookInfoChanged->iUsedEntries = iUsedEntries;
        iIsUsedEntriesCounted = ETrue;
        }

    // Complete if request is not cancelled or handle is valid
    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CacheEntriesL
// This method activates phonebook entry caching.
// if phonebook initialization is not done, it is done first.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//


#ifndef USING_CTSY_DISPATCHER 


void CMmPhoneBookStoreTsy::CacheEntriesL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL_1, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - entered");
    // Get Phonebook related DLL -static data

    // Check if phonebook has been initialized successfully. Caching of ADN
    // entries does not work if phonebook initialize have been failed.
    if ( iStoreInfoData->iIsPhonebookInitialized &&
         ( EFalse == iStoreInfoData->iIsPhonebookInitializeFailed ||
           KADNPhoneBook != iPhoneBookType ) )
        {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL_2, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - Cache PB %S ", iPhoneBookName);

		TInt ret = KErrNone;
		CMmPhoneTsy::TNosBootState* bootState = iMmPhoneTsy->NosBootState();
		
              if ( !iCacheReady )
                {
                // Check if SIM is ready, and SMS has been cached already
                if ( ( KADNPhoneBook == iPhoneBookType ) ||
                     ( KFDNPhoneBook == iPhoneBookType ) &&
                     ( EFalse != bootState->iSIMReady ) )
                    {
                    // Cache must be reseted after the boot
                    if ( NULL != iPBStoreCache )
                        {
                        iPBStoreCache->ResetAndDestroy();
                        delete iPBStoreCache;
                        iPBStoreCache = NULL;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL_3, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - Old cache deleted");
                        }
                    
                    //Inform phonebook that the cache is not ready for refresh
                    ResetCacheReady();

                    bootState->iCachingActive = ETrue;
                    TRAP( ret, ReadToCacheL();)
                    }
                }
            else if ( iCacheReady )
                {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL_4, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - PB %S cache entries ready", iPhoneBookName);
                }
        if ( KErrNone != ret)
            {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL_5, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - PB %S caching failed!", iPhoneBookName);
            }
        }
    // Phonebook not initialized -> let's do it before caching
    else if ( ( !iStoreInfoData->iIsPhonebookInitialized
        || iStoreInfoData->iIsPhonebookInitializeFailed )
        && !iMmPhoneTsy->IsPBInitActive() )
        {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL_6, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - Starting initialization PB: %S", iPhoneBookName);
        // before phonebook requests, phonebook must be initialized
        iMmPhoneBookStoreExtInterface->InitPhonebook(
            EMmTsyPhoneBookStoreInitIPC, iPhoneBookName );
        iMmPhoneTsy->SetPBInitActiveStatus( ETrue );
        }
    }

#endif //USING_CTSY_DISPATCHER



// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CompleteCachingL
// This method completes caching from sim server to TSY
// After the caching cache status is updated.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CompleteCachingL(
    TInt aResult, CMmDataPackage* aDataPackage )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECACHINGL_1, "TSY: CMmPhoneBookStoreTsy::CompleteCachingL - PB %S Caching completed", iPhoneBookName);
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECACHINGL_2, "TSY: CMmPhoneBookStoreTsy::CompleteCachingL - Result: %{TSymbianErrorCodes}",aResult );
	
#ifdef USING_CTSY_DISPATCHER 		
	aDataPackage->UnPackData(iPBStoreCache);
#else
	CArrayPtrSeg<CPhoneBookStoreEntry>* entryData;
	aDataPackage->UnPackData(entryData);	
	CopyLtsyCacheToCtsyCacheL(entryData);
#endif
	
    CMmPhoneTsy::TNosBootState* bootState = iMmPhoneTsy->NosBootState();

    // If cache were successfully generated, update
    // the number of used entries
    if ( KErrNone == aResult )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECACHINGL_3, "TSY: CMmPhoneBookStoreTsy::CompleteCachingL - iPBStoreCache->Count: %d",iPBStoreCache->Count() );
        iUsedEntries = iPBStoreCache->Count();
        //Set the caching ready only if request is successfull.
        //when FDN is on, the ADN caching is not successful. Meanwhile the FDN
        //is on, the ADN list is not available, so this is not a problem.
        //When the user turns the FDN back to off the tsy will
        //cache the FDN list
        //again
        iCacheReady = ETrue;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECACHINGL_4, "TSY: CMmPhoneBookStoreTsy::CompleteCachingL - iUsedEntries: %d",iUsedEntries );

    bootState->iCachingActive = EFalse;

    CMmCustomTsy* mmCustom = iMmPhoneTsy->GetCustomTsy();

    // Check if client has requested caching before completing
    if ( NULL != mmCustom && !iMmPhoneTsy->IsRefreshOngoing() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECACHINGL_5, "TSY: CMmPhoneBookStoreTsy::CompleteCachingL - Client refresh req complete" );
        if ( KErrNone == aResult )
            {
            mmCustom->UpdateCacheStatus( RMmCustomAPI::ECacheReady,
                iPhoneBookName );

             mmCustom->CompleteNotifyPhoneBookCacheReady(
                iPhoneBookName, KErrNone );

              if( iRequestedOnPBRead )
                  {
                  ContinueReadL();
                  }
            }
        //in all other cases, RMmCustomAPI::ECacheFailed is used
        else
            {
            mmCustom->UpdateCacheStatus( RMmCustomAPI::ECacheFailed,
                iPhoneBookName );

            mmCustom->CompleteNotifyPhoneBookCacheReady(
                iPhoneBookName, KErrGeneral );
            }
        }
    // Otherwise caching has been originated from SIM and completion goes
    // through refresh
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECACHINGL_6, "TSY: CMmPhoneBookStoreTsy::CompleteCachingL - SIM refresh req complete" );

        // Check for which phonebook the caching must be completed
        if ( !iPhoneBookName.CompareF( KETelIccAdnPhoneBook ) )
            {
            iMmPhoneTsy->CompleteCacheSimL( KCacheAdn, aResult );
            }
        else if ( !iPhoneBookName.CompareF( KETelIccFdnPhoneBook ) )
            {
            iMmPhoneTsy->CompleteCacheSimL( KCacheFdn, aResult );
            }
        //NOTIFY THE PHONEBOOK that the cache is ready
        if ( NULL != mmCustom )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECACHINGL_7, "TSY: CMmPhoneBookStoreTsy::CompleteCachingL, mmCustom->UpdateCacheStatus" );
            if ( KErrNone == aResult )
                {
                mmCustom->UpdateCacheStatus(
                    RMmCustomAPI::ECacheReady, iPhoneBookName );
                }
            else
                {
                mmCustom->UpdateCacheStatus(
                    RMmCustomAPI::ECacheFailed, iPhoneBookName );
                }
            mmCustom->CompleteNotifyPhoneBookCacheReady(
                    iPhoneBookName, aResult );
            }
        }
    // Get current status of FDN and ADN
    if (mmCustom != NULL)
        {
        RMmCustomAPI::TPndCacheStatus fdnCacheStatus =
            mmCustom->GetPhonebookCacheStatus( KFDNPhoneBook );
        RMmCustomAPI::TPndCacheStatus adnCacheStatus =
            mmCustom->GetPhonebookCacheStatus( KADNPhoneBook );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECACHINGL_8, "TSY: CMmPhoneBookStoreTsy::CompleteCachingL - FDN cachestatus %d", fdnCacheStatus);
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETECACHINGL_9, "TSY: CMmPhoneBookStoreTsy::CompleteCachingL - ADN cachestatus %d", adnCacheStatus);

	    // Set caching back to active if either FDN or ADN is caching
        if ( RMmCustomAPI::ECacheNotReady == fdnCacheStatus 
    		|| RMmCustomAPI::ECacheNotReady == adnCacheStatus ) 
    	    {
    	    bootState->iCachingActive = ETrue;
    	    }
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ReadEntriesFromArrayL
// Fills client's multiple read request from cached entries or enty array
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::ReadEntriesFromArrayL(
    TPBArrayTypeToRead aArrayType )
    {
    CArrayPtrSeg<CPhoneBookStoreEntry>* ptrToArray = NULL;

    if ( EPBStoreCacheArray == aArrayType )
        {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READENTRIESFROMARRAYL_1, "TSY: CMmPhoneBookStoreTsy::ReadEntriesFromCache %S, iPBStoreCache", iPhoneBookName);
        ptrToArray = iPBStoreCache;
        }
    else if ( EPBStoreReadEntriesArray == aArrayType )
        {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READENTRIESFROMARRAYL_2, "TSY: CMmPhoneBookStoreTsy::ReadEntriesFromEntryArray %S, iPBStoreReadEntries", iPhoneBookName);
        ptrToArray = iPBStoreReadEntries;
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READENTRIESFROMARRAYL_3, "TSY: CMmPhoneBookStoreTsy::ReadEntriesFromEntryArray invalid aArrayType");
        User::Leave(KErrNotSupported);
        }

    CPhoneBookStoreEntry* cacheEntry = NULL;

    for ( TInt locationIndex = 0; locationIndex < ptrToArray->Count();
          locationIndex++ )
        {

        // Check if location matches into location required
        if ( ( ptrToArray->At( locationIndex ) )->iLocation >= iIndexToRead &&
            ( ptrToArray->At( locationIndex ) )->
            iLocation < iIndexToRead + iNumEntriesToRead )
            {
            cacheEntry = ptrToArray->At( locationIndex );

            TInt ret = FillBufferedDesL( cacheEntry );
            if ( KErrNone != ret )
                {
                locationIndex = ptrToArray->Count();
                }
            }

        cacheEntry = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ReadToCacheL
// Starts caching operation. Cache array is given as
// a one parameter that is filled by DOS -side.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::ReadToCacheL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READTOCACHEL_1, "TSY: CMmPhoneBookStoreTsy::ReadToCacheL");

    // Call ReadL from GSM extension
    iMmPhoneBookStoreExtInterface->ReadToCacheL(
          EMmTsyPhoneBookStoreCacheIPC,
          iPhoneBookName);
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::UpdateCacheL
// This method updates cache and it is used when
// Write- and Delete requests are succesfully completed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::UpdateCacheL(
    CPhoneBookStoreEntry* aEntry,
    TBool aDeleteEntry )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_1, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL");
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_2, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - iPBStoreCache->Count(): %d ", iPBStoreCache->Count() );

    if(aEntry == NULL)
        {
        if(aDeleteEntry == EFalse)
		   {
		   delete iWriteEntry;
           iWriteEntry = NULL;
		   }
		
		User::Leave(KErrArgument);
        }
    
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_3, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - aEntry->Location(): %u ", aEntry->iLocation );

    TInt result( KErrNotFound );
    TInt updateIndex( 0 );

    for ( TInt index = 0; index < iPBStoreCache->Count() &&
          result == KErrNotFound; index++ )
        {
        if ( ( iPBStoreCache->At( index ) )->iLocation == aEntry->iLocation )
            {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_4, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - iPBStoreCache->iLocation:  %u ", aEntry->iLocation);
            updateIndex =  index;
            result = KErrNone;
            }
        }

    // Case: <Add new entry into cache>
    if ( KErrNotFound == result )
        { 
        // Update write entry into cache       
        if ( aDeleteEntry == EFalse)
            {
            // Append new entry into cache
            // Sort array for PBClient
            iPBStoreCache->AppendL( aEntry );
            TKeyArrayPtr sortKey( _FOFF( CPhoneBookStoreEntry, iLocation ),
                     ECmpTUint16 );
            iPBStoreCache->Sort( sortKey );
            // Append the number or used entries
            iUsedEntries++;
            // For notification purpose
            iMobilePhoneBookStoreEvent = RMobilePhoneStore::KStoreEntryAdded;
            // Reset pointer to PB object. It is stored in iPBStoreCache array
            iWriteEntry = NULL;
            }     
        else
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_5, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL, Deleted entry is not in cache! ");
            }          
        }
    // Entry found
    else
        {
        // Case: <Change existing entry in cache>
        if ( aEntry->iText || aEntry->iNumber ||
            ( aEntry->iAnr && aEntry->iAnr->MdcaCount() > 0 ) ||
            ( aEntry->iEmail && aEntry->iEmail->MdcaCount() > 0 ) ||
            ( aEntry->iSne && aEntry->iSne->MdcaCount() > 0 ) )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_6, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - Change existing entry");
            // Every update/deletion of an existing phonebook entry or the
            // addition of a new phone book entry, incremements the
            // iChangeCounter.
            iPhoneBookInfo.iChangeCounter++;
            // Append updated entry to cache

            // Delete current entry
            CPhoneBookStoreEntry* entryToDelete =
                iPBStoreCache->At( updateIndex );
            if ( entryToDelete == iDeleteEntry )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_7, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - entryToDelete == iDeleteEntry");
                // reset iDeleteEntry member
                iDeleteEntry = NULL;
                }
            if ( entryToDelete == iWriteEntry )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_8, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - entryToDelete == iWriteEntry");
                // reset iWriteEntry member
                iWriteEntry = NULL;
                }
            delete entryToDelete;
            entryToDelete = NULL;
            iPBStoreCache->Delete( updateIndex );
            iPBStoreCache->InsertL( updateIndex, aEntry );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_9, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - updateIndex: %d", updateIndex);
            if ( aEntry == iWriteEntry )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_10, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - aEntry == iWriteEntry");
                // reset iWriteEntry member
                iWriteEntry = NULL;
                }
            // Compress cache
            iPBStoreCache->Compress();
            // Complete notify store event
            iMobilePhoneBookStoreEvent = (
                RMobilePhoneStore::KStoreEntryChanged );
            }
        // Case: <Delete entry from cache>
        else
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_11, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - Delete entry");
            // decrease iUsedEntries
            iUsedEntries--;

            // Every update/deletion of an existing phonebook entry or the
            // addition of a new phone book entry incremements the
            // iChangeCounter.
            iPhoneBookInfo.iChangeCounter++;
            CPhoneBookStoreEntry* entryToDelete =
                iPBStoreCache->At( updateIndex );
            if ( entryToDelete == iDeleteEntry )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_12, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - entryToDelete == iDeleteEntry");
                // reset iDeleteEntry member
                iDeleteEntry = NULL;
                }
            else
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_UPDATECACHEL_13, "TSY: CMmPhoneBookStoreTsy::UpdateCacheL - iWriteEntry = NULL");
                // reset iWriteEntry member
                iWriteEntry = NULL;
                }

            delete entryToDelete;
            entryToDelete = NULL;
            iPBStoreCache->Delete( updateIndex );
            // Compress cache
            iPBStoreCache->Compress();

            // Complete notify store event
            iMobilePhoneBookStoreEvent = (
                RMobilePhoneStore::KStoreEntryDeleted );

			if ( !iPBStoreCache->Count() )
				{
				// Store is empty
				// Complete notify store event
				iMobilePhoneBookStoreEvent = (
						RMobilePhoneStore::KStoreEmpty );
				}
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::FillBufferedDesL
// This method fills the buffered descriptor by client with
// the entry. Data to be filled must contain at least one valid field.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::FillBufferedDesL(
    CPhoneBookStoreEntry* aPhoneBookStoreResp )
    {
    TInt ret( KErrNone );

    // Create pbBuffer
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();

    CleanupStack::PushL(pbBuffer);
    
    // Set buffered descriptor
    pbBuffer->Set( iBufferToRead );

    // Initialize variable for while -loop
    TInt marked( 0 );
    // The number 6 is the number of the loops. Every number
    // has its own functionality within loop, beginging
    // from "AddNewEntry" to "Append Sne"
    while ( KErrNone == ret && 7 > marked )
        {
        // Append new entry
        if ( 0 == marked )
            {
            ret = pbBuffer->AddNewEntryTag();
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_FILLBUFFEREDDESL_1, "TSY: CMmPhoneBookStoreTsy::FillBufferedDesL - 0, ret: %{TSymbianErrorCodes} ", ret);
            }
        // Append Index
        else if ( 1 == marked )
            {
            ret = pbBuffer->PutTagAndValue(
                RMobilePhoneBookStore::ETagPBAdnIndex, (TUint16)(
                aPhoneBookStoreResp->iLocation ) );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_FILLBUFFEREDDESL_2, "TSY: CMmPhoneBookStoreTsy::FillBufferedDesL - 1, ret: %{TSymbianErrorCodes} ", ret);
            }
        // Append Name
        else if ( 2 == marked )
            {
            if ( aPhoneBookStoreResp->iText &&
                 aPhoneBookStoreResp->iText->Size() > 0 )
                {
                ret = pbBuffer->PutTagAndValue(
                    RMobilePhoneBookStore::ETagPBText,
                    aPhoneBookStoreResp->iText->Des() );
                }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_FILLBUFFEREDDESL_3, "TSY: CMmPhoneBookStoreTsy::FillBufferedDesL - 2, ret: %{TSymbianErrorCodes} ", ret);
            }
        // Append Number
        else if ( 3 == marked )
            {
            if ( aPhoneBookStoreResp->iNumber &&
                 aPhoneBookStoreResp->iNumber->Size() > 0 )
                {
                ret = pbBuffer->PutTagAndValue(
                    RMobilePhoneBookStore::ETagPBNumber,
                    aPhoneBookStoreResp->iNumber->Des() );
                }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_FILLBUFFEREDDESL_4, "TSY: CMmPhoneBookStoreTsy::FillBufferedDesL - 3, ret: %{TSymbianErrorCodes} ", ret);
            }
        // Append Email
        else if ( 4 == marked )
            {
            for ( TInt numEmails = 0;
                  numEmails < aPhoneBookStoreResp->iEmail->MdcaCount();
                  ++numEmails )
                {
                ret = pbBuffer->PutTagAndValue(
                    RMobilePhoneBookStore::ETagPBEmailAddress,
                    aPhoneBookStoreResp->iEmail->MdcaPoint( numEmails ) );
                }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_FILLBUFFEREDDESL_5, "TSY: CMmPhoneBookStoreTsy::FillBufferedDesL - 4, ret: %{TSymbianErrorCodes} ", ret);
            }
        // Append Sne
        else if ( 5 == marked )
            {
            for ( TInt numSnes = 0;
                  numSnes < aPhoneBookStoreResp->iSne->MdcaCount();
                  ++numSnes )
                {
                ret = pbBuffer->PutTagAndValue(
                    RMobilePhoneBookStore::ETagPBSecondName,
                    aPhoneBookStoreResp->iSne->MdcaPoint( numSnes ) );
                }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_FILLBUFFEREDDESL_6, "TSY: CMmPhoneBookStoreTsy::FillBufferedDesL - 5, ret: %{TSymbianErrorCodes} ", ret);
            }
        // Append Anr
        else if ( 6 == marked )
            {
            for ( TInt numAnrs = 0;
                numAnrs < aPhoneBookStoreResp->iAnr->MdcaCount();
                ++numAnrs )
                {
                pbBuffer->AddNewNumberTag();
                ret = pbBuffer->PutTagAndValue(
                    RMobilePhoneBookStore::ETagPBNumber,
                    aPhoneBookStoreResp->iAnr->MdcaPoint( numAnrs ) );
                }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_FILLBUFFEREDDESL_7, "TSY: CMmPhoneBookStoreTsy::FillBufferedDesL - 6, ret: %{TSymbianErrorCodes} ", ret);
            }

        marked++;
        }

    // If error occurs after the "new entry" has been filled, corrupted
    // entry will be destroyed from pbbuffer. If appending new tag causes
    // error, RemovePartialEntry -function will not be used, because nothing
    // is yet inside pbbuffer related to this entry.
    if ( KErrNone != ret && 1 < marked )
        {
        pbBuffer->RemovePartialEntry();
        }

    // Increase the class variable which holds
    // the number of entries filled to buffered data
    if ( KErrNone == ret && 4 <= marked )
        {
        iNumEntriesFilled++;
        }
    
    CleanupStack::PopAndDestroy(pbBuffer);

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_FILLBUFFEREDDESL_8, "TSY: CMmPhoneBookStoreTsy::FillBufferedDesL - returnvalue: %{TSymbianErrorCodes} ", ret);

    //If simrefresh registration is failed prevent new cache
    if( KErrOverflow == ret && !( iMmPhoneTsy->GetSimRefreshRegister() ) )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_FILLBUFFEREDDESL_9, "TSY: CMmPhoneBookStoreTsy::ReadL - Prevent new cache" );
        iReadReqStore->iCaching = ETrue;
        }

    iRetFillBufferedDes = ret;

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ReadCancel
// Cancels reading a entry from the Phonebook if the request is
// still outstanding.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::ReadCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_READCANCEL_1, "TSY: CMmPhoneBookStoreTsy::ReadCancel - Handle:%d", aTsyReqHandle );

    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneBookStoreRead );

    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        // Reset class variables
        iIndexToRead = 0;
        iNumEntriesToRead = 0;
        iNumEntriesFilled = 0;
        iIndexAndEntries = NULL;
        iBufferToRead = NULL;
        // Complete with KErrCancel
        ReqCompleted( aTsyReqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::WriteL
// This method writes the entry supplied within the entry
// parameter into the store, at an index specified by entry index. If the
// client specifies an index of -1, then the TSY will store the entry in the
// first free location and then return this location within an entry index
// when it will be completed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::WriteL(
    const TTsyReqHandle aTsyReqHandle,
    TDesC8* aPBData,
    TInt* aIndex )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_WRITEL_1, "TSY: CMmPhoneBookStoreTsy::WriteL - EtelIndex: %d",*aIndex );

    //SDN phonebook can't be updated by user.
    if (iPhoneBookType == KSDNPhoneBook)
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_WRITEL_2, "TSY: CMmPhoneBookStoreTsy::WriteL - SDN phonebook entry can't be updated");
        ReqCompleted( aTsyReqHandle, KErrAccessDenied );
        }
    else
        {
        // Check if request handle already exists
        TTsyReqHandle writePhoneBookStoreHandle =
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodePhoneBookStoreWrite );
    
        if ( EMultimodePhoneBookStoreReqHandleUnknown
            != writePhoneBookStoreHandle )
            {
            // If request handle already exists return KErrServerBusy
            ReqCompleted( aTsyReqHandle, KErrServerBusy );
            }
        else
            {
            // Save pointer to data
            iPBData = aPBData;
            // Save pointer to index
            iWriteIndexToReturn = aIndex;
    
            TInt ret( KErrArgument );
    
            // If user supplies index which is less than -1 or 0, complete with
            // KErrArgument
            if ( -1 > *aIndex || 0 == *aIndex )
                {
                iPBData = NULL;
                iWriteIndexToReturn = 0;
                }
            // Write by location at SIM card
            else
                {
                // Straight write-forwarding to SIM server,
                TInt index = *aIndex;
    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_WRITEL_3, "TSY: CMmPhoneBookStoreTsy::WriteL - Straight write-forwarding to SIM");
                ret = WriteByIndexL( index );
                }
    
            // Check error while processing write
            if ( KErrNone != ret )
                {
                iPBData = NULL;
                iWriteIndexToReturn = NULL;
                ReqCompleted( aTsyReqHandle, ret );
                }
            else
                {
                // Save tsy req handle type
                iReqHandleType = EMultimodePhoneBookStoreWrite;
                }
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::WriteByIndexL
// This method writes the entry to the (U)SIM, specified by the
// location index at SIM Card.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::WriteByIndexL(
    TInt aIndex )
    {
    TInt ret( KErrNone );

    // Separate PhoneBook data, get
    // - Name to write
    // - Number to write
    // - Email address
    // - SNE
    // - ANR/AAS
    // - GRP

	if ( iWriteEntry != NULL )
		{
		delete iWriteEntry;
		iWriteEntry = NULL;
		}

    // NOTE: when this entry is created, it is always
    // put on the cache in succesfull case.
    iWriteEntry = new( ELeave )CPhoneBookStoreEntry();
    iWriteEntry->ConstructL();

    ret = SeparatePBDataL( iPBData, iWriteEntry );

    if ( KErrNone == ret )
        {
        // Forward request to GSM extension
        ret = iMmPhoneBookStoreExtInterface->WriteL(
            EMmTsyPhoneBookStoreWriteIPC, // Ipc
            iPhoneBookName, // PhoneBook name
            aIndex, // Write index
            iWriteEntry );
        }

    if ( KErrNone != ret )
        {
        delete iWriteEntry;
        iWriteEntry = NULL;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CompleteWriteL
// This method completes writing an entry.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CompleteWriteL(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEWRITEL_1, "TSY: CMmPhoneBookStoreTsy::CompleteWriteL - Result: %{TSymbianErrorCodes}",aResult );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneBookStoreWrite );

    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        //Create a TPBEntryInfo struct
        TPBEntryInfo pbEntryInfo;

        //initialise variables of the structure to 0
        pbEntryInfo.iLocation = 0;
        pbEntryInfo.iMaxNumLength = 0;
       
        if ( ( KErrNone == aResult) || 
            ( KErrGsmSimServSneFull == aResult ) ||
            ( KErrGsmSimServAnrFull == aResult ) ||
            ( KErrGsmSimServEmailFull == aResult ) )
            {
            aDataPackage->UnPackData( pbEntryInfo );
            // For VMBX, set KStoreEntryAdded always event for a Write-request
            iMobilePhoneBookStoreEvent = RMobilePhoneStore::KStoreEntryAdded;

            // Update cache for ADN and FDN
            if (  ( KADNPhoneBook == iPhoneBookType )
                || ( KFDNPhoneBook == iPhoneBookType ) )
                {
                if ( KErrGsmSimServSneFull == aResult )
                    {
                     iWriteEntry->iSne->Reset();
                    }
                else if ( KErrGsmSimServAnrFull == aResult )
                    {
                    iWriteEntry->iAnr->Reset();
                    }
                else if ( KErrGsmSimServEmailFull == aResult )    
                    {
                    iWriteEntry->iEmail->Reset();  
                    } 
                iWriteEntry->iLocation =
                    static_cast<TUint16>( pbEntryInfo.iLocation );
                UpdateCacheL( iWriteEntry, EFalse );
                }

            if ( KADNPhoneBook == iPhoneBookType )
                {
                //Update Adn Max Num Length
                iStoreInfoData->iADNNumberLengthMax =
                    pbEntryInfo.iMaxNumLength;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEWRITEL_2, "TSY: CMmPhoneBookStoreTsy::CompleteWriteL: pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }
            else if ( KFDNPhoneBook == iPhoneBookType )
                {
                //Update Fdn Max Num Length
                iStoreInfoData->iFDNNumberLengthMax =
                    pbEntryInfo.iMaxNumLength;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEWRITEL_3, "TSY: CMmPhoneBookStoreTsy::CompleteWriteL:FDN pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }
            else if ( KVMBXPhoneBook == iPhoneBookType )
                {
                //Update VMBX Max Num Length
                if ( 0 < pbEntryInfo.iMaxNumLength )
                    {
                    iStoreInfoData->iVMBXNumberLengthMax =
                        pbEntryInfo.iMaxNumLength;
                    }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEWRITEL_4, "TSY: CMmPhoneBookStoreTsy::CompleteWriteL:VMBX pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }
            else if ( KMBDNPhoneBook == iPhoneBookType )
                {
                //Update MBDN Max Num Length
                if ( 0 < pbEntryInfo.iMaxNumLength )
                    {
                    iStoreInfoData->iMBDNNumberLengthMax =
                        pbEntryInfo.iMaxNumLength;
                    }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEWRITEL_5, "TSY: CMmPhoneBookStoreTsy::CompleteWriteL:MBDN pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }
            CompleteNotifyStoreEvent( pbEntryInfo.iLocation );
            }
        if ( ( KErrNone == aResult ) || 
            ( KErrGsmSimServSneFull == aResult ) ||
            ( KErrGsmSimServAnrFull == aResult ) ||
            ( KErrGsmSimServEmailFull == aResult ) && 
            ( -1 == *iWriteIndexToReturn ) )
            {
            *iWriteIndexToReturn = pbEntryInfo.iLocation;
            }

        ReqCompleted( reqHandle, aResult );
        }

    // Writing was done unsuccesfully and we could not
    // put iWriteEntry into cache, so we can delete it.
    if ( ( KErrNone != aResult ) && iWriteEntry ) 
        {
        if ( ( KErrGsmSimServSneFull== aResult ) || 
             ( KErrGsmSimServAnrFull == aResult ) || 
             ( KErrGsmSimServEmailFull == aResult ) )
            {
             // Do Nothing
            }
        else
            {
            delete iWriteEntry;
            iWriteEntry = NULL;                
            }
        }
    iPBData = NULL;
    iWriteIndexToReturn = NULL;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::SeparatePBDataL
// This fuction separates a byte stream formatted data, which
// is passed across the ETel API.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::SeparatePBDataL(
    TDesC8* aPBData,
    CPhoneBookStoreEntry* aWriteData )
    {
    TInt ret( KErrNone );
    TInt returnValue( KErrNone );
    TUint8 tmpTagValue;
    TUint8 tagValue;
    CPhoneBookBuffer::TPhBkTagType tagType;
    TUint8 newEntryTag;
    newEntryTag = RMobilePhoneBookStore::ETagPBNewEntry;
    TPtrC16 textPtr;
    TPtrC16 numPtr;
    TPtrC16 emailPtr;
    TPtrC16 snePtr;
    TPtrC16 anrPtr;

    // Create pbBuffer
    CPhoneBookBuffer* pbBuffer = new( ELeave ) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);
    
    // Set pointer to constant descriptor
    TPtr8 dataToSeparate( NULL, 0, 0 );
    dataToSeparate.Set( (TUint8*)aPBData->Ptr(),
        aPBData->Length(), aPBData->Length() );

    // Init and get entry info
    pbBuffer->Set( &dataToSeparate );
    pbBuffer->StartRead();
    ret = pbBuffer->GetTagAndType( tmpTagValue, tagType );

    // Ensure, that there is new entry containing data
    if ( KErrNone == ret && tmpTagValue == newEntryTag )
        {
        // Get the first value from TLV data
        ret = pbBuffer->GetTagAndType( tagValue, tagType );

        while ( KErrNone == ret )
            {
            switch ( tagValue )
                {
                // Get name
                case RMobilePhoneBookStore::ETagPBText:
                    ret = pbBuffer->GetValue( textPtr );
                    if ( iMaxTextLen < textPtr.Length() )
                        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SEPARATEPBDATAL_1, "TSY: CMmPhoneBookStoreTsy::SeparatePBDataL - Text length is more than KPBStoreTextSize " );
                        ret = CMmCommonStaticUtility::EpocErrorCode(
                            KErrTooBig, KErrGsm0707TextStringTooLong );
                        }
                    else
                        {
                        if ( 0 < textPtr.Length() )
                            {
                            aWriteData->iText =
                                HBufC::NewLC( textPtr.Length() );
                            aWriteData->iText->Des() = textPtr;
                            CleanupStack::Pop();
                            }
                        }
                    break;
                // Get number
                case RMobilePhoneBookStore::ETagPBNumber:
                    ret = pbBuffer->GetValue( numPtr );
                        aWriteData->iNumber = HBufC::NewLC( numPtr.Length() );
                        aWriteData->iNumber->Des() = numPtr;
                        CleanupStack::Pop();
                    break;
                // Skip index, it has been supplied as a parameter
                case RMobilePhoneBookStore::ETagPBAdnIndex:
                    pbBuffer->SkipValue( CPhoneBookBuffer::EPhBkTypeInt16 );
                    break;
                // Skip TON, not needed
                case RMobilePhoneBookStore::ETagPBTonNpi:
                    pbBuffer->SkipValue( CPhoneBookBuffer::EPhBkTypeInt8 );
                    break;
                // Get Email(s)
                case RMobilePhoneBookStore::ETagPBEmailAddress:
                    ret = pbBuffer->GetValue( emailPtr );
                    if ( iStoreInfoData-> 
                        iEmailTextLengthMax < emailPtr.Length() )
                        {
                        ret = KErrArgument;
                        }
                    else
                        {
                        aWriteData->iEmail->AppendL( emailPtr );
                        }
                    break;
                // Get Sne(s)
                case RMobilePhoneBookStore::ETagPBSecondName:
                    ret = pbBuffer->GetValue( snePtr );
                    if ( iStoreInfoData->iSNETextLengthMax < snePtr.Length() )
                        {
                        ret = KErrArgument;
                        }
                    else
                        {
                        aWriteData->iSne->AppendL( snePtr );
                        }
                    break;
                // Get Anr(s)
                case RMobilePhoneBookStore::ETagPBAnrStart:
                    {
                ret = pbBuffer->GetTagAndType( tagValue, tagType );
                switch ( tagValue )
                    {
                    case RMobilePhoneBookStore::ETagPBNumber:
                        ret = pbBuffer->GetValue( anrPtr );

                        if ( iStoreInfoData->iANRNumLengthMax <
                            anrPtr.Length() )
                            {
                            ret = CMmCommonStaticUtility::EpocErrorCode(
                                KErrTooBig, KErrGsm0707DialStringTooLong );
                            }
                        else
                            {
                            aWriteData->iAnr->AppendL( anrPtr );
                            }
                        break;
                    case RMobilePhoneBookStore::ETagPBText:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SEPARATEPBDATAL_2, "TSY: CMmPhoneBookStoreTsy::SeparatePBDataL - RMobilePhoneBookStore::ETagPBAnrStart ETagPBText" );
                        break;
                    case RMobilePhoneBookStore::ETagPBTonNpi:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SEPARATEPBDATAL_3, "TSY: CMmPhoneBookStoreTsy::SeparatePBDataL - RMobilePhoneBookStore::ETagPBAnrStart ETagPBTonNpi" );
                        break;
                    default:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SEPARATEPBDATAL_4, "TSY: CMmPhoneBookStoreTsy::SeparatePBDataL - RMobilePhoneBookStore::ETagPBAnrStart KErrArgument" );
                        ret = KErrArgument;
                        break;
                    }
                }
                break;
                // Unknown Tag, complete with KErrArgument
                default:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SEPARATEPBDATAL_5, "TSY: CMmPhoneBookStoreTsy::SeparatePBDataL - Unknown Tag " );
                    ret = KErrArgument;
                    break;
                }
            // Get the new field if no errors

            if ( KErrNone == ret )
                {
                ret = pbBuffer->GetTagAndType( tagValue, tagType );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SEPARATEPBDATAL_6, "TSY: CMmPhoneBookStoreTsy::SeparatePBDataL - RMobilePhoneBookStore::GetNewField tagValue: %d ", tagValue);
                }         
            }
        if ( KErrNotFound != ret )
            {
            returnValue = ret;
            }
        }
    // Bad descriptor, no new entry found
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SEPARATEPBDATAL_7, "TSY: CMmPhoneBookStoreTsy::SeparatePBDataL - Bad descriptor " );
        returnValue = KErrArgument;
        }

    CleanupStack::PopAndDestroy(pbBuffer);
    
    return returnValue;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::DeleteL
// This method deletes the entry in the slot specified by the
// index parameter from the store.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::DeleteL(
    const TTsyReqHandle aTsyReqHandle,
    TInt* aIndex )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DELETEL_1, "TSY: CMmPhoneBookStoreTsy::DeleteL - aIndex: %d",*aIndex );

    //SDN phonebook can't be updated by user.
    if (iPhoneBookType == KSDNPhoneBook)
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DELETEL_2, "TSY: CMmPhoneBookStoreTsy::DeleteL - SDN phonebook entry can't be updated");
        ReqCompleted( aTsyReqHandle, KErrAccessDenied );
        }
    else
        {
        TInt ret( KErrNone );
    
        // If user supplies index which is less than 0, complete with KErrArgument,
        // each record is numbered from 1 to 255 (or maximum number of records).->
        // /Multimode ETel API Definition Document 4.06/
        // NOTE! TSY does not take the responsibility, if the location number is
        // greater than total numbers of records in this PBStore.
        if ( 0 > *aIndex )
            {
            ReqCompleted( aTsyReqHandle, KErrArgument );
            }
        else
            {
			if ( iDeleteEntry == NULL )
				{
				delete iDeleteEntry;
				iDeleteEntry = NULL;
				}

            // These are done for updating cache
            iDeleteEntry = new ( ELeave ) CPhoneBookStoreEntry;
            iDeleteEntry->ConstructL();
            iDeleteEntry->iLocation = static_cast<TUint16>( *aIndex );
    
            // Forward Delete request to extension
            ret = iMmPhoneBookStoreExtInterface->DeleteL(
                    EMmTsyPhoneBookStoreDeleteIPC,
                    iPhoneBookName,
                    iDeleteEntry->iLocation );
    
            // Request returned error
            if ( KErrNone != ret )
                {
                delete iDeleteEntry;
                iDeleteEntry = NULL;
                ReqCompleted( aTsyReqHandle, ret );
                }
            else
                {
                // Save tsy req handle type
                iReqHandleType = EMultimodePhoneStoreDelete;
                }
            }
        }

    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CompleteDeleteL
// Completes deleting an entry from the (U)SIM
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CompleteDeleteL(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEDELETEL_1, "TSY: CMmPhoneBookStoreTsy::CompleteDeleteL - Result: %{TSymbianErrorCodes}",aResult );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneStoreDelete );

    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        // If aResult is KErrNone
        if ( KErrNone == aResult )
            {
            //Create a TPBEntryInfo struct
            TPBEntryInfo pbEntryInfo;

            //initialise variables of the structure to 0
            pbEntryInfo.iLocation = 0;
            pbEntryInfo.iMaxNumLength = 0;

            //Unpack data
            aDataPackage->UnPackData( pbEntryInfo );

            // For VMBX, set KStoreEntryDeleted(or KStoreEmpty) for a Delete-request
            iMobilePhoneBookStoreEvent = RMobilePhoneStore::KStoreEntryDeleted;
            if(iStoreInfoData->iVMBXNumOfEntries <= 0)
                {
                iMobilePhoneBookStoreEvent = RMobilePhoneStore::KStoreEmpty;
                }

            //Update cache only if iPhoneBookType is ADN or FDN
            if ( KADNPhoneBook == iPhoneBookType || 
                 KFDNPhoneBook == iPhoneBookType )
                 {
                 // Call UpdateCacheL with iDeleteEntry
                 UpdateCacheL( iDeleteEntry, ETrue );
                 }
                
            // Complete notify store event
            CompleteNotifyStoreEvent( iDeleteEntry->iLocation );

            if ( KADNPhoneBook == iPhoneBookType )
                {
                //Update Adn Max Num Length
                iStoreInfoData->iADNNumberLengthMax =
                    pbEntryInfo.iMaxNumLength;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEDELETEL_2, "TSY: CMmPhoneBookStoreTsy::CompleteDeleteL: pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }
            else if ( KFDNPhoneBook == iPhoneBookType )
                {
                //Update Fdn Max Num Length
                iStoreInfoData->iFDNNumberLengthMax =
                    pbEntryInfo.iMaxNumLength;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEDELETEL_3, "TSY: CMmPhoneBookStoreTsy::CompleteDeleteL:FDN pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }
            else if ( KVMBXPhoneBook == iPhoneBookType )
                {
                //Update VMBX Max Num Length
                if ( 0 < pbEntryInfo.iMaxNumLength )
                    {
                    iStoreInfoData->iVMBXNumberLengthMax =
                        pbEntryInfo.iMaxNumLength;
                    }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEDELETEL_4, "TSY: CMmPhoneBookStoreTsy::CompleteDeleteL:VMBX pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }
            }
        
#ifdef USING_CTSY_DISPATCHER
           
		//set the VMB phonebook maximum number length, from the code above it appears to be a defect as the
		//VMB phonebook maximum number length can never be set due to it being behind the guard that the phonebook
		//has to be either ADN or FDN
        if((aResult == KErrNone) && (iPhoneBookType == KVMBXPhoneBook))
        	{
            TPBEntryInfo* pbEntryInfo = NULL;
            aDataPackage->UnPackData(&pbEntryInfo);
            __ASSERT_DEBUG(pbEntryInfo, User::Invariant());
        	if(pbEntryInfo->iMaxNumLength >= 0)
        		{
            	iStoreInfoData->iVMBXNumberLengthMax = pbEntryInfo->iMaxNumLength;
        		}
        	}
#endif //USING_CTSY_DISPATCHER

        // Delete now unnecessary iDeleteEntry
        delete iDeleteEntry;
        iDeleteEntry = NULL;
            
        // Complete with error
        ReqCompleted( reqHandle, aResult );
        }
    else 
        {
        if ( iDeleteEntry )
            {
            delete iDeleteEntry;
            iDeleteEntry = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::DeleteAllL
// This method deletes all entries from the store by overwriting
// the number and the name fields within SIM.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::DeleteAllL(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DELETEALLL_1, "TSY: CMmPhoneBookStoreTsy::DeleteAllL - Handle:%d", aTsyReqHandle );

    //SDN phonebook can't be updated by user.
    if (iPhoneBookType == KSDNPhoneBook)
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_DELETEALLL_2, "TSY: CMmPhoneBookStoreTsy::DeleteAllL - SDN phonebook entry can't be updated");
        ReqCompleted( aTsyReqHandle, KErrAccessDenied );
        }
    else
        {
        TInt ret( KErrNone );
    
        // Forwarded function call
        ret = iMmPhoneBookStoreExtInterface->DeleteAllL(
                EMmTsyPhoneBookStoreDeleteAllIPC, iPhoneBookName );
    
        if ( KErrNone != ret )
            {
            // Complete with error
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save tsy req handle type
            iReqHandleType = EMultimodePhoneStoreDeleteAll;
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CompleteDeleteAll
// Completes delete all entries from the Phonebook store. If
// no error value occurred, store event can be set to KStoreEmpty.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CompleteDeleteAll(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEDELETEALL_1, "TSY: CMmPhoneBookStoreTsy::CompleteDeleteAll - Result: %{TSymbianErrorCodes}",aResult );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneStoreDeleteAll );

    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        // If deletion done succesfully, complete notify store event
        // that all entries has deleted at once.
        if ( KErrNone == aResult )
            {
            if ( ( KADNPhoneBook == iPhoneBookType )
                || ( KFDNPhoneBook == iPhoneBookType ) )
                {
                ResetCache();
                }

            //Create a TPBEntryInfo struct
            TPBEntryInfo pbEntryInfo;

            //initialise variables of the structure to 0
            pbEntryInfo.iLocation = 0;
            pbEntryInfo.iMaxNumLength = 0;

            //Unpack data
            aDataPackage->UnPackData( pbEntryInfo );

            if ( KADNPhoneBook == iPhoneBookType )
                {
                //Update Adn Max Num Length
                iStoreInfoData->iADNNumberLengthMax =
                    pbEntryInfo.iMaxNumLength;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEDELETEALL_2, "TSY: CMmPhoneBookStoreTsy::CompleteDeleteAll: pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }
            else if ( KFDNPhoneBook == iPhoneBookType )
                {
                //Update Fdn Max Num Length
                iStoreInfoData->iFDNNumberLengthMax =
                    pbEntryInfo.iMaxNumLength;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEDELETEALL_3, "TSY: CMmPhoneBookStoreTsy::CompleteDeleteAll:FDN pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }
            else if ( KVMBXPhoneBook == iPhoneBookType )
                {
                //Update VMBX Max Num Length
                if ( 0 < pbEntryInfo.iMaxNumLength )
                    {
                    iStoreInfoData->iVMBXNumberLengthMax =
                        pbEntryInfo.iMaxNumLength;
                    }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEDELETEALL_4, "TSY: CMmPhoneBookStoreTsy::CompleteDeleteAll:VMBX pbEntryInfo.iMaxNumLength:%d", pbEntryInfo.iMaxNumLength);
                }

            // Complete notify store event
            iMobilePhoneBookStoreEvent = RMobilePhoneStore::KStoreEmpty;
            CompleteNotifyStoreEvent( -1 ); // DeleteAll->index -1
            }

        // Complete with error
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::GetInfoL
// This method returns information about the phonebookstore.
// This information includes the store's name, the type of thing it stores,
// the total number of storage slots and the number of used slots. It does
// not give information about which slots are filled and which are empty.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::GetInfoL(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aInfo )
    {

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_1, "TSY: CMmPhoneBookStoreTsy::GetInfoL Handle:%d", aTsyReqHandle);

    TInt ret( KErrNone );
    TTsyReqHandle getInfoHandle =
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodePhoneStoreGetInfo );

    if ( 0 < getInfoHandle )
        {
        // The request is already in processing because of previous request
        // Complete request with status value informing the client about
        // the situation.
        ret = KErrServerBusy;
        }
    else if(iInitError != KErrNone)
        {
        // The phone book initilization failed
        return iInitError;
        }
	else if ( !(iStoreInfoData->iIsPhonebookInitialized) )
		{
		// The data is not ready yet, the store was not initialized...
        RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg* infoPckg = 
            reinterpret_cast<RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg*>( 
            aInfo );
        iPhoneBookInfoChanged = &( ( *infoPckg )() );    
		iReqHandleType = EMultimodePhoneStoreGetInfo;
		return KErrNone;
		}
    else if ( sizeof( RMobilePhoneBookStore::TMobilePhoneBookInfoV1 ) > 
              aInfo->MaxLength() )
        {
        // aInfo can not contain TMobilePhoneBookInfoV1 object.
        ret = KErrArgument;
        }
    else
        {
        RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg* infoPckg = 
            reinterpret_cast<RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg*>( 
            aInfo );
        iPhoneBookInfoChanged = &( ( *infoPckg )() );    
            
        TBool completeRequest( ETrue );

        // Reset info
        SetInfo();

        // BDN is not supported feature. Request will be completed later on 
        // this method with KErrNone and hard coded BDN storage information 
        // will be delivered to the client.
        if ( KBDNPhoneBook == iPhoneBookType )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_2, "TSY: CMmPhoneBookStoreTsy::GetInfoL - BDN" );
            }
        // If PB type is ADN, total entries and used entries already read
        else if ( KADNPhoneBook == iPhoneBookType )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_3, "TSY: CMmPhoneBookStoreTsy::GetInfoL - ADN" );
            ret = KErrNotReady;

            //if cache is ready, set information and get name length
            if ( EFalse != iCacheReady )
                {
                ret = KErrNone;

                // The maximum length for the number in a phonebook entry
                iPhoneBookInfoChanged->iMaxNumLength =
                    iStoreInfoData->iADNNumberLengthMax;

                // The maximum length for the text tag in a phonebook entry
                iPhoneBookInfoChanged->iMaxTextLength =
                    iStoreInfoData->iADNTextLengthMax;

                // The total number of entries
                iPhoneBookInfoChanged->iTotalEntries =
                    iStoreInfoData->iADNNumOfEntries;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_4, "TSY: CMmPhoneBookStoreTsy::GetInfoL - total entries: %d",iStoreInfoData->iADNNumOfEntries );

                // Check if Email, SNE & ANR Caps are supported + GRP

                if ( iStoreInfoData->iEmailNumOfEntriesPerEntry > 0 )
                    {
                    iPhoneBookInfoChanged->iCaps =
                        iPhoneBookInfoChanged->iCaps |
                        RMobilePhoneBookStore::KCapsEmailAddressUsed;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_5, "TSY: CMmPhoneBookStoreTsy::GetInfoL ADN -> Email supported" );
                    }
                if ( iStoreInfoData->iSNENumOfEntriesPerEntry > 0 )
                    {
                    iPhoneBookInfoChanged->iCaps =
                        iPhoneBookInfoChanged->iCaps |
                        RMobilePhoneBookStore::KCapsSecondNameUsed;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_6, "TSY: CMmPhoneBookStoreTsy::GetInfoL ADN -> SNE supported" );
                    }
                if ( iStoreInfoData->iANRNumOfEntriesPerEntry > 0 )
                    {
                    iPhoneBookInfoChanged->iCaps =
                        iPhoneBookInfoChanged->iCaps |
                        RMobilePhoneBookStore::KCapsAdditionalNumUsed;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_7, "TSY: CMmPhoneBookStoreTsy::GetInfoL ADN -> ANR supported" );
                    }
                }
            }
        // If PB type is FDN, total entries and used entries already read
        else if ( KFDNPhoneBook == iPhoneBookType )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_8, "TSY: CMmPhoneBookStoreTsy::GetInfoL - FDN" );
            ret = KErrNotReady;

            //if cache is ready, set information and get name length
            if ( EFalse != iCacheReady )
                {
                ret = KErrNone;

                // The maximum length for the number in a phonebook entry
                iPhoneBookInfoChanged->iMaxNumLength =
                    iStoreInfoData->iFDNNumberLengthMax;

                // The maximum length for the text tag in a phonebook entry
                iPhoneBookInfoChanged->iMaxTextLength =
                    iStoreInfoData->iFDNTextLengthMax;

                // The total number of entries
                iPhoneBookInfoChanged->iTotalEntries =
                    iStoreInfoData->iFDNNumOfEntries;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_9, "TSY: CMmPhoneBookStoreTsy::GetInfoL - total entries: %d",iStoreInfoData->iFDNNumOfEntries );

                // Set caps by phonebook name
                iPhoneBookInfoChanged->iCaps = KPBTypeFDNCaps;
                }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_10, "TSY: CMmPhoneBookStoreTsy::GetInfoL - iMaxNumLength: %d",iPhoneBookInfoChanged->iMaxNumLength );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_11, "TSY: CMmPhoneBookStoreTsy::GetInfoL - iMaxTextLength: %d",iPhoneBookInfoChanged->iMaxTextLength );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_12, "TSY: CMmPhoneBookStoreTsy::GetInfoL - iTotalEntries: %d",iPhoneBookInfoChanged->iTotalEntries );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_13, "TSY: CMmPhoneBookStoreTsy::GetInfoL - iUsedEntries: %d",iPhoneBookInfoChanged->iUsedEntries );
            }
        else if ( KSDNPhoneBook == iPhoneBookType )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_14, "TSY: CMmPhoneBookStoreTsy::GetInfoL - SDN" );
            ret = KErrNone;

            // The maximum length for the number in a phonebook entry
            iPhoneBookInfoChanged->iMaxNumLength =
                iStoreInfoData->iSDNNumberLengthMax;

            // The maximum length for the text tag in a phonebook entry
            iPhoneBookInfoChanged->iMaxTextLength =
                iStoreInfoData->iSDNTextLengthMax;

            // The total number of entries
            iPhoneBookInfoChanged->iTotalEntries =
                iStoreInfoData->iSDNNumOfEntries;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_15, "TSY: CMmPhoneBookStoreTsy::GetInfoL - total entries: %d",iStoreInfoData->iSDNNumOfEntries );

            if ( !iIsUsedEntriesCounted )
                {
                // Check first that SIM server is ready before doing this
                if ( !iIsUsedEntriesCounted &&
                     iMmPhoneTsy->NosBootState()->iSIMReady &&
                    iStoreInfoData->iIsPhonebookInitialized )
                    {
                    // GetInfo -request cannot be completed
                    // before used entries are counted
                    completeRequest = EFalse;
                    ret = CountEntriesL();
                    }
                else
                    {
                    iPhoneBookInfoChanged = NULL;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_16, "TSY: CMmPhoneBookStoreTsy::GetInfoL, not ready to perform CountEntriesL()");
                    ret = KErrNotReady;
                    }
                }
            }
        //VMBX need to read total and used entries
        else if ( KVMBXPhoneBook == iPhoneBookType )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_17, "TSY: CMmPhoneBookStoreTsy::GetInfoL - VMBX" );
            ret = KErrNone;

            // The maximum length for the number in a phonebook entry
            iPhoneBookInfoChanged->iMaxNumLength =
                iStoreInfoData->iVMBXNumberLengthMax;

            // The maximum length for the text tag in a phonebook entry
            iPhoneBookInfoChanged->iMaxTextLength =
                iStoreInfoData->iVMBXTextLengthMax;

            // The total number of entries
            iPhoneBookInfoChanged->iTotalEntries =
                iStoreInfoData->iVMBXNumOfEntries;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_18, "TSY: CMmPhoneBookStoreTsy::GetInfoL - total entries: %d",iStoreInfoData->iVMBXNumOfEntries );

            // Set caps by the store info data 
            //iPhoneBookInfoChanged->iCaps = iStoreInfoData->iVMBXCaps;
            iPhoneBookInfoChanged->iCaps = KPBTypeVMBXCaps;
            
            if ( !iIsUsedEntriesCounted )
                {
                completeRequest = EFalse;
                // Check first that SIM server is ready before doing this
                if ( !iIsUsedEntriesCounted &&
                     iMmPhoneTsy->NosBootState()->iSIMReady &&
                     iStoreInfoData->iIsPhonebookInitialized )
                    {
                    completeRequest = EFalse;
                    ret = CountEntriesL();
                    }
                else
                    {
                    iPhoneBookInfoChanged = NULL;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_19, "TSY: CMmPhoneBookStoreTsy::GetInfoL, not ready to perform CountEntriesL()");
                    ret = KErrNotReady;
                    }
                }
            }
         //MBDN
        else if ( KMBDNPhoneBook == iPhoneBookType )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_20, "TSY: CMmPhoneBookStoreTsy::GetInfoL - MBDN" );
            ret = KErrNone;

            // The maximum length for the number in a phonebook entry
            iPhoneBookInfoChanged->iMaxNumLength =
                iStoreInfoData->iMBDNNumberLengthMax;

            // The maximum length for the text tag in a phonebook entry
            iPhoneBookInfoChanged->iMaxTextLength =
                iStoreInfoData->iMBDNTextLengthMax;

            // The total number of entries
            iPhoneBookInfoChanged->iTotalEntries =
                iStoreInfoData->iMBDNNumOfEntries;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_21, "TSY: CMmPhoneBookStoreTsy::GetInfoL - total entries: %d",iPhoneBookInfoChanged->iTotalEntries );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_22, "TSY: CMmPhoneBookStoreTsy::GetInfoL - iMaxNumLength: %d",iPhoneBookInfoChanged->iMaxNumLength );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOL_23, "TSY: CMmPhoneBookStoreTsy::GetInfoL - iMaxTextLength: %d",iPhoneBookInfoChanged->iMaxTextLength );

            // Set caps by the store info data 
            iPhoneBookInfoChanged->iCaps = KPBTypeMBDNCaps;
            }
        if ( completeRequest )
            {
            // All data got, complete request
            ReqCompleted( aTsyReqHandle, ret );

            // Request completed here. Set ret to KErrNone. In otherwise request
            // will be completed second time in CMmPhoneBookStoreTsy::ExtFunc.   
            ret = KErrNone;
            }
        else if ( KErrNone == ret )
            {
            // Save tsy req handle type
            iReqHandleType = EMultimodePhoneStoreGetInfo;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CompleteGetInfo
// Completes GetInfo -request.
// GetInfo method needed info is got mainly in initial -phase.
// In case of SDN and VMBX used entries must be conted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CompleteGetInfo(
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETEGETINFO_1, "TSY: CMmPhoneBookStoreTsy::CompleteGetInfo - Result: %{TSymbianErrorCodes}", aError );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneStoreGetInfo );

    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        // Complete request with error
        ReqCompleted( reqHandle, aError );
        }

    // NULL class parameter
    iPhoneBookInfoChanged = NULL;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::SetInfo
// This method resets the info parameter from the client before
// the request is done to the extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::SetInfo()
    {
    // The name of the phone-side store
    iPhoneBookInfoChanged->iName = iPhoneBookName;

    // The maximum number of characters for the number in a phonebook entry
    iPhoneBookInfoChanged->iMaxNumLength = KNoInfoAvail;

    // The maximum number of characters for the text tag in a phonebook entry
    iPhoneBookInfoChanged->iMaxTextLength = KNoInfoAvail;

    // The memory location of this phonebook
    // BDN not supported, location set to unknown
    if ( !iPhoneBookName.CompareF( KETelIccBdnPhoneBook ) )
        {
        iPhoneBookInfoChanged->iLocation =
            RMobilePhoneBookStore::ELocationUnknown;
        }
    // Location SIM
    else
        {
        iPhoneBookInfoChanged->iLocation =
            RMobilePhoneBookStore::ELocationIccMemory;
        }

    // The type of entry in the phone-side store
    iPhoneBookInfoChanged->iType = RMobilePhoneStore::EPhoneBookStore;

    // Set change counter
    iPhoneBookInfoChanged->iChangeCounter = iPhoneBookInfo.iChangeCounter;

    // Set caps by phonebook name
    if ( !iPhoneBookName.CompareF( KETelIccAdnPhoneBook ) )
        {
        iPhoneBookInfoChanged->iCaps = KPBTypeADNCaps;
        }
    else if ( !iPhoneBookName.CompareF( KETelIccFdnPhoneBook ) )
        {
        iPhoneBookInfoChanged->iCaps = KPBTypeFDNCaps;
        }
    else if ( !iPhoneBookName.CompareF( KETelIccVoiceMailBox ) )
        {
        iPhoneBookInfoChanged->iCaps = KPBTypeVMBXCaps;
        }
    else if ( !iPhoneBookName.CompareF( KETelIccSdnPhoneBook ) )
        {
        iPhoneBookInfoChanged->iCaps = KPBTypeSDNCaps;
        }
    else if ( !iPhoneBookName.CompareF( KETelIccBdnPhoneBook ) )
        {
        iPhoneBookInfoChanged->iCaps = KPBTypeBDNCaps;
        }
    else if ( !iPhoneBookName.CompareF( KETelIccMbdnPhoneBook ) )
        {
        iPhoneBookInfoChanged->iCaps = KPBTypeMBDNCaps;
        }

    // Indicates the current number of entries that may be held in this store
    iPhoneBookInfoChanged->iUsedEntries = iUsedEntries;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SETINFO_1, "TSY: CMmPhoneBookStoreTsy::SetInfo - used entries: %d",iUsedEntries );

    // Indicates the total number of entries
    iPhoneBookInfoChanged->iTotalEntries = KNoInfoAvail;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::GetInfoCancel
// This method cancels get store information request if it
// is still outstanding.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::GetInfoCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETINFOCANCEL_1, "TSY: CMmPhoneBookStoreTsy::GetInfoCancel - Handle:%d", aTsyReqHandle );

    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneStoreGetInfo );

    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        // Reset class attribute
        iPhoneBookInfoChanged = NULL;
        // Complete with KErrCancel
        ReqCompleted( aTsyReqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::NotifyStoreEvent
// This method activates notifying of phonebookstore events.
//
// This method allows a client to be notified when one of the events
// defined in TMobilePhoneStoreEvent occurs to the store.  These events
// allow a client to know the overall status of a store (full, empty etc)
// and also when a slot within a store changes. A slot could change because
// another client or the phone has created, updated or deleted an entry.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::NotifyStoreEvent(
    RMobilePhoneStore::TMobileStoreEvent *aEvent,
    TInt* aIndex )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_NOTIFYSTOREEVENT_1, "TSY: CMmPhoneBookStoreTsy::NotifyStoreEvent - called" );

    TTsyReqHandle requestHandle = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodePhoneStoreNotifyStoreEvent );

    // If request is not running
    if ( 0 == requestHandle )
        {
        // Set variables to notify
        iNotifyPhoneBookStoreEventPtr = aEvent;
        iNotifyPhoneBookStoreEventIndexPtr = aIndex;

        // Save tsy req handle type
        iReqHandleType = EMultimodePhoneStoreNotifyStoreEvent;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::NotifyStoreEventCancel
// This method cancels notifying of phonebookstore events.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::NotifyStoreEventCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_NOTIFYSTOREEVENTCANCEL_1, "TSY: CMmPhoneBookStoreTsy::NotifyStoreEventCancel - called" );

    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneStoreNotifyStoreEvent );

    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        // Complete with cancel
        iNotifyPhoneBookStoreEventPtr = NULL;
        iNotifyPhoneBookStoreEventIndexPtr = NULL;
        ReqCompleted( aTsyReqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CompleteNotifyStoreEvent
// This method is called when PhoneBookStore related event happens
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CompleteNotifyStoreEvent(
    TInt aLocation )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETENOTIFYSTOREEVENT_1, "TSY: CMmPhoneBookStoreTsy::CompleteNotifyStoreEvent - Called" );

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                                EMultimodePhoneStoreNotifyStoreEvent );

    if ( EMultimodePhoneBookStoreReqHandleUnknown != reqHandle )
        {
        RMobilePhoneStore::TMobileStoreEvent* phoneBookStoreEvent =
            REINTERPRET_CAST( RMobilePhoneStore::TMobileStoreEvent*,
            iNotifyPhoneBookStoreEventPtr );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETENOTIFYSTOREEVENT_2, "TSY: CMmPhoneBookStoreTsy::CompleteNotifyStoreEvent - StoreEvent %8X ", (TUint)iMobilePhoneBookStoreEvent );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_COMPLETENOTIFYSTOREEVENT_3, "TSY: CMmPhoneBookStoreTsy::CompleteNotifyStoreEvent - location %d ", aLocation );
        // Fill notify information
        *phoneBookStoreEvent = iMobilePhoneBookStoreEvent;
        *iNotifyPhoneBookStoreEventIndexPtr = aLocation;

        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::SetActivePBExtension
// Sets pointer to active extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreTsy::SetActivePBExtension(
    CMmPhoneBookStoreExtInterface* aMmPBExt )
    {
    TInt ret = KErrGeneral;

    if ( aMmPBExt )
        {
        iMmPhoneBookStoreExtInterface = aMmPBExt;
        ret = KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ActivePBExtension
// Returns a pointer to the active extension.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreExtInterface* CMmPhoneBookStoreTsy::ActivePBExtension()
    {
    return iMmPhoneBookStoreExtInterface;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ResetVariables
// Reset all used variables.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::ResetVariables()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_RESETVARIABLES_1, "TSY: CMmPhoneBookStoreTsy::ResetVariables" );

    // Reset phonebooktype
    iPhoneBookType = 0;

    // Cache variables
    iPBStoreCache = NULL;
    iMultipleReadPosTo = 1000;
    iCacheReady = EFalse;

    // Read variables
    iNumEntriesToRead = 0;
    iBufferToRead = NULL;
    iIndexToRead = 0;
    iNumEntriesFilled = 0;
    iIndexAndEntries = NULL;
    iPBStoreReadEntries = NULL;

    // Write variables
    iPBData = NULL;
    iWriteIndexToReturn = NULL;

    // GetInfo variables
    iPhoneBookInfoChanged = NULL;

    // NotifyStoreEvent
    iNotifyPhoneBookStoreEventIndexPtr = NULL;
    iNotifyPhoneBookStoreEventPtr = NULL;

    // Initialization
    iStoreInfoData = iMmPhoneTsy->PhoneBookState();

    // Count SDN and VMBX used entries
    iIsUsedEntriesCounted = EFalse;

    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ResetCache
// Resets cache
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::ResetCache()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_RESETCACHE_1, "TSY: CMmPhoneBookStoreTsy::ResetCache" );

    // SDN and VMBX do not have cache. Skip reset if the cache pointer is null.
    if ( NULL != iPBStoreCache )
        {

        iPBStoreCache->ResetAndDestroy();
        iPhoneBookInfo.iChangeCounter++;
        iUsedEntries = 0;
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ResetCacheReady
// Resets iCacheReady flag
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::ResetCacheReady()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_RESETCACHEREADY_1, "TSY: CMmPhoneBookStoreTsy::ResetCacheReady" );
    iCacheReady = EFalse;
    CMmCustomTsy* mmCustom = NULL;

    if( iMmPhoneTsy )
        {
        mmCustom = iMmPhoneTsy->GetCustomTsy();
        }

    if ( NULL != mmCustom )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_RESETCACHEREADY_2, "TSY: CMmPhoneBookStoreTsy::ResetCacheReady, mmCustom->UpdateCacheStatus - not ready" );
        mmCustom->UpdateCacheStatus( RMmCustomAPI::ECacheNotReady,
            iPhoneBookName );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ResetInitialisationStatus
// Resets iIsPhonebookInitialized flag
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::ResetInitialisationStatus()
    {
    iStoreInfoData->iIsPhonebookInitialized = EFalse;
    iStoreInfoData->iIsPhonebookInitializeFailed = EFalse;
    }


#ifdef TF_LOGGING_ENABLED
// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ReqCompleted
// Overrides CTelObject::ReqCompleted for test logging purposes.
// It prints the aTsyReqHandle and aError variable in the log file and then
// calls CTelObject::ReqCompleted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::ReqCompleted(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aError )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_REQCOMPLETED_1, "TSY: CMmPhoneBookStoreTsy::ReqCompleted. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", aTsyReqHandle, aError, this);
    CTelObject::ReqCompleted( aTsyReqHandle, aError );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_REQCOMPLETED_2, "<-- TSY: CMmPhoneBookStoreTsy::ReqCompleted");
    }

#endif


#ifdef REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::SetTypeOfResponse
// Sets the type of response for a given Handle. Automatic
// mode includes an automatic response in case of no response
// from the DOS in a specified time
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::SetTypeOfResponse(
    const TInt aReqHandleType,
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SETTYPEOFRESPONSE_1, "TSY: CMmPhoneBookStoreTsy::SetTypeOfResponse - Handle:%d Type:%d", (TUint)aTsyReqHandle, aReqHandleType );

    TInt timeOut( 0 );

    //example switch
    switch ( aReqHandleType )
        {
        case EMultimodePhoneBookStoreRead:
            timeOut = KMmPBStoreReadTimeOut;
            break;
        case EMultimodePhoneBookStoreWrite:
            timeOut = KMmPBStoreWriteTimeOut;
            break;
        case EMultimodePhoneStoreDelete:
            timeOut = KMmPBDeleteTimeOut;
            break;
        case EMultimodePhoneStoreDeleteAll:
            timeOut = KMmPBDeleteAllTimeOut;
            break;
        case EMultimodePhoneStoreGetInfo:
            timeOut = KMmPBGetInfoTimeOut;
            break;

        //Must not use timer:
        // - all notifications
            //case EMultimodePhoneStoreNotifyStoreEvent:

        default:
            //does not use timer
            iTsyReqHandleStore->SetTsyReqHandle(
                aReqHandleType,
                aTsyReqHandle );
            break;
        }

    if ( 0 < timeOut )
        {
        //the timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle(
            aReqHandleType,
            aTsyReqHandle,
            timeOut );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::Complete
// Completes the request due timer expiration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::Complete(
    TInt aReqHandleType,
    TInt aError )
    {
    // Catch possible leave. If leave happens, there is nothing we can do.
    TRAP_IGNORE(
        //All possible TSY req handle types are listed in the
        //switch case below.
        switch( aReqHandleType )
            {
            case EMultimodePhoneBookStoreRead:
                CompleteReadL( aError, NULL );
                break;
            case EMultimodePhoneBookStoreWrite:
                CompleteWriteL( aError, NULL );
                break;
            case EMultimodePhoneStoreDelete:
                CompleteDeleteL( aError, NULL );
                break;
            case EMultimodePhoneStoreDeleteAll:
                CompleteDeleteAll( aError, NULL );
                break;
            case EMultimodePhoneStoreGetInfo:
                CompleteGetInfo( aError );
                break;

            //Can't use timer:
            // - all notifications
                //case EMultimodePhoneStoreNotifyStoreEvent:

            default:
                ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle(
                                aReqHandleType ), aError );
                break;
            }
        ); // end trap
    }

#endif

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::ContinueReadL
// Continues ReadL method
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::ContinueReadL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CONTINUEREADL_1, "TSY: CMmPhoneBookStoreTsy::ContinueReadL ");

    iRequestedOnPBRead = EFalse;
    //Reguest Read with stored parameters
    if( iReadReqStore )
        {
        ReadL(iReadReqStore->iReqHandle,iReadReqStore->iIndexAndEntries,
        iReadReqStore->iPBData, ETrue ); // ETrue means that Read is a continued-read.
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::IsPBInitDone
// Is PB initialization done.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TBool CMmPhoneBookStoreTsy::IsPBInitDone()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_ISPBINITDONE_1, "TSY: CMmPhoneBookStoreTsy::IsPBInitDone: - PB %S", iPhoneBookName);
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_ISPBINITDONE_2, "TSY: CMmPhoneBookStoreTsy::IsPBInitDone: - PB iIsPBInitCompleted = %d ", iIsPBInitCompleted );
    
    return iIsPBInitCompleted;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::GetPhonebookType
// Return phonebook type.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
// 
TUint8 CMmPhoneBookStoreTsy::GetPhonebookType()
 	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_GETPHONEBOOKTYPE_1,  "TSY: CMmPhoneBookStoreTsy::GetPhonebookType - iPhoneBookType: %d", iPhoneBookType);
 	return iPhoneBookType;
 	}

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CopyLtsyCacheToCtsyCacheL
// This method copies Ltsy Cache to Ctsy Cache
// This is only done when NOT using the dispatcher as the Ltsy creates the array and destroys
// the memory before CTSY is completely done with it. It now has to be copied over to CTSY domain
// to make sure the Ltsy does not delete before Ctsy is finished. This is not the case using Ctsy Dispatcher
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::CopyLtsyCacheToCtsyCacheL( CArrayPtrSeg<CPhoneBookStoreEntry>* aEntryData )
	{
	if (iPBStoreCache!=NULL)
		{
		iPBStoreCache->ResetAndDestroy();
		delete iPBStoreCache;
		iPBStoreCache = NULL;
		}
	if (aEntryData->Count() > 0)
		{
		iPBStoreCache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>(aEntryData->Count());
		
		for(TInt count=0; count < (aEntryData)->Count(); count++)
			{
			CPhoneBookStoreEntry* phoneBookStoreEntry = new( ELeave )CPhoneBookStoreEntry();
			CleanupStack::PushL( phoneBookStoreEntry );		
			phoneBookStoreEntry->ConstructL();
			
			CPhoneBookStoreEntry* pbsePtr = (*aEntryData)[count];
			
			phoneBookStoreEntry->iLocation = pbsePtr->iLocation;
			
			if((pbsePtr->iText)!=NULL)
				{
				phoneBookStoreEntry->iText = HBufC16::NewL((pbsePtr->iText)->Size());
				*(phoneBookStoreEntry->iText)=(*(pbsePtr->iText));
				}
			
			if((pbsePtr->iNumber)!=NULL)
				{
				phoneBookStoreEntry->iNumber = HBufC16::NewL((pbsePtr->iNumber)->Size());
				*(phoneBookStoreEntry->iNumber)=(*(pbsePtr->iNumber));
				}
			
			if((pbsePtr->iEmail)!=NULL)
				{
				CDesC16Array* email = pbsePtr->iEmail;
				for(TInt emailCount = 0; emailCount<(email->Count()); emailCount++)
					{
					(phoneBookStoreEntry->iEmail)->AppendL((*email)[emailCount]);
					}
				}		
			
			if((pbsePtr->iSne)!=NULL)
				{
				CDesC16Array* sne = pbsePtr->iSne;
				for(TInt sneCount = 0; sneCount<(sne->Count()); sneCount++)
					{
					(phoneBookStoreEntry->iSne)->AppendL((*sne)[sneCount]);
					}
				}		
			
			if((pbsePtr->iAnr)!=NULL)
				{
				CDesC16Array* anr = pbsePtr->iAnr;
				for(TInt anrCount = 0; anrCount<(anr->Count()); anrCount++)
					{
					(phoneBookStoreEntry->iAnr)->AppendL((*anr)[anrCount]);
					}	
				}
			
			iPBStoreCache->AppendL(phoneBookStoreEntry);
			CleanupStack::Pop( phoneBookStoreEntry );
			}
		}
	else
		{
		iPBStoreCache = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>(1);
		}
	}

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::SimIsReady()
// This method will be activate by the phone to notify the SIM is ready.
// ---------------------------------------------------------------------------
//
void CMmPhoneBookStoreTsy::SimIsReady()
    {
    
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SIMISREADY_1, "TSY: CMmPhoneBookStoreTsy::SimIsReady - iIsPhonebookInitialized: %u", (TUint)iStoreInfoData->iIsPhonebookInitialized);
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SIMISREADY_2, "TSY: CMmPhoneBookStoreTsy::SimIsReady - before initialization");
        // Check if phonebook has been initialized
        if ( !iMmPhoneTsy->IsPBInitActive()
            && ( !iStoreInfoData->iIsPhonebookInitialized )
            )
            {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SIMISREADY_3, "TSY: CMmPhoneBookStoreTsy::SimIsReady - starting initialization");

            // before phonebook requests, phonebook must be initialized
            iMmPhoneBookStoreExtInterface->InitPhonebook(
                EMmTsyPhoneBookStoreInitIPC, iPhoneBookName );

            iMmPhoneTsy->SetPBInitActiveStatus( ETrue );
            }
        else if (iMmPhoneTsy->IsPBInitActive())
            {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SIMISREADY_4, "TSY: CMmPhoneBookStoreTsy::SimIsReady - initialization active via other store, waiting for completion");
            iIsPBInitCompleted = EFalse;
            }
        else
            {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SIMISREADY_5, "TSY: CMmPhoneBookStoreTsy::SimIsReady - Initialization was done via other store");
            // Set initialization flag 
            iIsPBInitCompleted = ETrue;
            }
        
        if ( iStoreInfoData->iIsPhonebookInitialized
            && ( iPhoneBookType == KADNPhoneBook
            || iPhoneBookType == KFDNPhoneBook ) )
            {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_SIMISREADY_6, "TSY: CMmPhoneBookStoreTsy::SimIsReady - starting caching");
            // Call CacheEntries method to read entries from SIM.
            CacheEntriesL();
            }
    }


//  End of File
