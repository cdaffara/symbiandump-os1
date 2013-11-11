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
#include "cmmenstoretsyTraces.h"
#endif

#include "cmmenstoretsy.h"
#include "cmmtsyreqhandlestore.h"
#include "CMmPrivateUtility.h"
#include "MmTsy_numberOfSlots.h"
#include "MmTsy_timeoutdefs.h"
#include "cmmmessagemanagerbase.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>

// ======== MEMBER FUNCTIONS ========

CMmENStoreTsy::CMmENStoreTsy():
    iReqHandleType(EMultimodeENStoreReqHandleUnknown)
    {
    }

void CMmENStoreTsy::ConstructL()
    {
#ifdef REQHANDLE_TIMER
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this,
        iMmPhone,
        KMmTsyNumOfENStoreRequests,
        iStorageReqHandles );
#else
    // Create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        KMmTsyNumOfENStoreRequests,
        iStorageReqHandles );

#endif

    // Create GSM ENStore objects
    iMmENStoreGsmExt = static_cast<CMmENStoreExtInterface*>(
        CMmENStoreGsmExt::NewL( this ) );

    // Set mode GSM
    iMmENStoreExtInterface = iMmENStoreGsmExt;

    // Construct ReadAllArray
    iENStoreReadAllArray =
        new ( ELeave ) CArrayPtrFlat<CListReadAllAttempt>( 1 );

    //register conference call tsy in the message manager
    iMmPhone->MessageManager()->RegisterTsyObject( 
        CMmMessageManagerBase::EENStoreTsy, this );
    }

CMmENStoreTsy* CMmENStoreTsy::NewL(
    CMmPhoneTsy* aMmPhone )
    {
    // Create EN store
    CMmENStoreTsy* aMmENStoreTsy = new( ELeave ) CMmENStoreTsy();
    CleanupClosePushL( *aMmENStoreTsy );

    // Set MM Phone and phonet sender pointers
    aMmENStoreTsy->iMmPhone = aMmPhone;

    // Call construct
    aMmENStoreTsy->ConstructL();
    CleanupStack::Pop();

    return aMmENStoreTsy;
    }

CMmENStoreTsy::~CMmENStoreTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMENSTORETSY_DTOR_1, "TSY: CMmENStoreTsy::~CMmENStoreTsy");

    if (iMmPhone )
        {
        // deregister tsy object from message manager
        iMmPhone->MessageManager()->DeregisterTsyObject( this );
        }

    // Delete TSY request handle store if it exist
    if ( NULL != iTsyReqHandleStore )
        {
        delete iTsyReqHandleStore;
        iTsyReqHandleStore = NULL;
        }

    // Delete ENStoreGsmExt
    delete iMmENStoreGsmExt;
    iMmENStoreGsmExt = NULL;

    // Delete ENStoreReadAll array
    iENStoreReadAllArray->ResetAndDestroy();
    delete iENStoreReadAllArray;
    iENStoreReadAllArray = NULL;

    iReadENStoreEntryPtr = 0;
    iENStoreInfoChanged = 0;
    iNotifyENStoreEventPtr = 0;
    iNotifyENStoreEventIndexPtr = 0;
    iReadAllBufSizePtr = 0;
    iMmPhone = 0;
    iMmENStoreExtInterface = 0;
    iTsyReqHandleStore = 0;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::Init
// Initialisation method that is called from ETel Server.
// This is an initialization method for the class functionality.
// Generally initization method inits e.g. class attributes.
// In this case, this method is not needed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmENStoreTsy::Init()
    {
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::OpenNewObjectByNameL
// This method opens extendable sub-sessions.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmENStoreTsy::OpenNewObjectByNameL(
    const TDesC& )
    {
    // Not supported
    User::Leave( KErrNotSupported );
    //lint -e{527} "unreachable code"
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::OpenNewObjectL
// This method opens extendable sub-sessions.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmENStoreTsy::OpenNewObjectL(
    TDes& )
    {
    // Not supported
    User::Leave( KErrNotSupported );
    //lint -e{527} "unreachable code"
    
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::ReqModeL
// To ask what type of flow control is used for the IPC number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmENStoreTsy::ReqModeL(
    const TInt aIpc )
    {
    CTelObject::TReqMode ret = 0;

    // Match the request IPC
    switch ( aIpc )
        {
        // Non Flow controlled services
        case EMobilePhoneStoreReadAllPhase2:
            break;
        // Flow Controlled Services
        case EMobilePhoneStoreRead:
        case EMobilePhoneStoreWrite:
        case EMobilePhoneStoreDelete:
        case EMobilePhoneStoreDeleteAll:
        case EMobilePhoneStoreReadAllPhase1:
        case EMobilePhoneStoreGetInfo:
            ret = KReqModeFlowControlObeyed;
            break;
        // Multiple Completion Services with Immediate Server Repost
        // (Usually Notifications)
        case EMobilePhoneStoreNotifyStoreEvent:
            ret = KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
            break;
        // Cancel Requests
        // Listed here just for reference
        // case EMobilePhoneStoreGetInfoCancel:
        // case EMobilePhoneStoreReadCancel:
        // case EMobilePhoneStoreWriteCancel:
        // case EMobilePhoneStoreDeleteCancel:
        // case EMobilePhoneStoreDeleteAllCancel:
        // case EMobilePhoneStoreReadAllCancel:
        // case EMobilePhoneStoreNotifyStoreEventCancel:
        default:
            User::Leave( KErrNotSupported );
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::RegisterNotification
// Enables the TSY to "turn on" any regular notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::RegisterNotification(
    const TInt aIpc )
    {
    // Initalize ret value
    TInt ret( KErrNotSupported );

    // Match the request IPC
    switch ( aIpc )
        {
        // Not supported, but for possible future use
        case EMobilePhoneStoreNotifyStoreEvent:
            ret = KErrNone;
            break;

        default:
            // Unknown or invalid IPC.
            // ret already initialized KErrNotSupported,
            // do nothing
            break;
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::DeregisterNotification
// Enables the TSY to "turn off" any regular notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::DeregisterNotification(
    const TInt aIpc )
    {
    // Initalize ret value
    TInt ret( KErrNotSupported );

    // Match the request IPC
    switch ( aIpc )
        {
        // Not supported, but for possible future use
        case EMobilePhoneStoreNotifyStoreEvent:
            ret = KErrNone;
            break;

        default:
            // Unknown or invalid IPC.
            // ret already initialized KErrNotSupported,
            // do nothing
            break;
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::NumberOfSlotsL
// To ask the TSY how big buffer it wants.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    // Initialize number of slots to one
    TInt numberOfSlots = 1;

    // Match IPC
    switch ( aIpc )
        {
        // Not supported, but for possible future use
        case EMobilePhoneStoreNotifyStoreEvent:
            numberOfSlots = KMmENStoreNotifyStoreEventSlots;
            break;

        default:
            // Unknown or invalid EN storage IPC
            User::Leave( KErrNotSupported );
            break;
        }
        
    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::ExtFunc
// Handling of extended requests.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::ExtFunc(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
    // Initialize ret and trap error
    TInt ret = KErrNone;
    TInt trapError = KErrNone;

    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==EMultimodeENStoreReqHandleUnknown, User::Invariant());

    // Trap and call function that dispatches function requests
    TRAP ( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ); );

    // Check was there an error
    if ( trapError != KErrNone )
        {
        // Complete with error if trap error
		// Reset request handle to indicate the request is no longer ongoing
        iTsyReqHandleStore->FindAndResetTsyReqHandle( aTsyReqHandle );
        ReqCompleted( aTsyReqHandle, trapError );
        }
    else if ( ret != KErrNone )
        {
        // Complete with error if return value error
        ReqCompleted( aTsyReqHandle, ret );
        }
    else if ( EMultimodeENStoreReqHandleUnknown != iReqHandleType )
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
        iReqHandleType = EMultimodeENStoreReqHandleUnknown;
        return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::DoExtFuncL
// Dispatches extension function requests.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMENSTORETSY_DOEXTFUNCL_1, "TSY: CMmENStoreTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    // Initialize return value
    TInt ret( KErrNotSupported );

    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();

    switch ( aIpc )
        {
        case EMobilePhoneStoreRead:
            ret = Read( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EMobilePhoneStoreGetInfo:
            ret = GetInfo(
                REINTERPRET_CAST( RMobilePhoneStore::TMobilePhoneStoreInfoV1*,
                dataPtr ) );
            break;
        case EMobilePhoneStoreReadAllPhase1:
            ret = ReadAllPhase1( aTsyReqHandle,
                REINTERPRET_CAST( RMobilePhone::TClientId*, dataPtr ),
                REINTERPRET_CAST( TInt*, dataPtr2 ) );
            break;
        case EMobilePhoneStoreReadAllPhase2:
            ret = ReadAllPhase2( aTsyReqHandle,
                REINTERPRET_CAST( RMobilePhone::TClientId*, dataPtr ),
                aPackage.Des2n() );
            break;

        // Not supported functions completes here
        // EMobilePhoneStoreWrite
        // EMobilePhoneStoreDelete
        // EMobilePhoneStoreDeleteAll
        // EMobilePhoneStoreNotifyStoreEvent
        default:
            // Do nothing, ret already set KErrNotSupported
            break;
        }
        
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMENSTORETSY_DOEXTFUNCL_2, "TSY: CMmENStoreTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::CancelService
// To cancel any extension requests.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::CancelService(
    const TInt aIpc,
    const TTsyReqHandle aTsyReqHandle )
    {
    // Initialize return value
    TInt ret( KErrGeneral );

    // Match cancel request using IPC
    switch ( aIpc )
        {
        case EMobilePhoneStoreGetInfo:
            ret = GetInfoCancel( aTsyReqHandle );
            break;
        case EMobilePhoneStoreRead:
            ret = ReadCancel( aTsyReqHandle );
            break;
        case EMobilePhoneStoreReadAllPhase1:
        case EMobilePhoneStoreReadAllPhase2:
            ret = ReadAllCancel( aTsyReqHandle );
            break;
        // Not possible to call cancel functions below because
        // request function is not supported, and returns immediately.
        // these functions will never even called. Added here just to
        // possible future purpose.
        // case EMobilePhoneStoreWrite:
        // case EMobilePhoneStoreDelete:
        // case EMobilePhoneStoreDeleteAll:
        // case EMobilePhoneStoreNotifyStoreEvent:
        default:
            // Do nothing, ret initialized KErrGeneral
            break;
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::Read
// Read one entry from EN store.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::Read(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aEntry )
    {
    TInt ret( KErrArgument );

    // Unpack entry parameters
    RMobileENStore::TMobileENEntryV1Pckg* entryPckg = REINTERPRET_CAST
        ( RMobileENStore::TMobileENEntryV1Pckg*, aEntry );
    RMobileENStore::TMobileENEntryV1& entry = ( *entryPckg )();

    // Check that client uses valid location index
    if (0 < entry.iIndex && sizeof( RMobileENStore::TMobileENEntryV1 ) <= aEntry->MaxLength())
        {
        // Forward request to GSM Extension
        ret = iMmENStoreExtInterface->Read( 
            EMmTsyENStoreReadIPC, entry.iIndex );        
        }

    // ISI message construction failed or phonet sender returned error
    if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret ); 
        }
    else
        {
        // Save tsy req handle type
        iReqHandleType = EMultimodeENStoreRead;
        
        // Set internal EN entry pointer
        iReadENStoreEntryPtr = aEntry;        
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::CompleteRead
// Complete read one entry from EN store
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmENStoreTsy::CompleteRead(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeENStoreRead );

    // Check that request handle was valid
    if ( EMultimodeENStoreReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aResult )
            {
            // Response information
            TENStoreResponse* enStoreResp = NULL;
            
            //unpack data
            if ( aDataPackage )
                {
                aDataPackage->UnPackData( &enStoreResp );
                }

            // Get entry
            RMobileENStore::TMobileENEntryV1Pckg* entryPckg =
                REINTERPRET_CAST( RMobileENStore::TMobileENEntryV1Pckg*,
                iReadENStoreEntryPtr );
            RMobileENStore::TMobileENEntryV1& entry = ( *entryPckg )();

            // Fill TMobileENEntryV1 entry
            // OBS! iCountryCode, iIdentity
            // iAlphaId cannot be filled, no info from SIM Server
            entry.iNetworkSpecific = EFalse;
            entry.iMode = RMobilePhone::ENetworkModeUnknown;
            
            if ( &enStoreResp )
                {
                entry.iNumber.Copy( enStoreResp->iECCNumber );
                entry.iIndex = enStoreResp->iLocation;
                }

            //Complete request to success
            ReqCompleted( reqHandle, KErrNone );
            }
        else
            {
            // Complete with error
            ReqCompleted( reqHandle, aResult );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::ReadCancel
// Cancel read one entry from EN store
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::ReadCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeENStoreRead );

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::GetInfo
// Get EN store info (capabilities)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::GetInfo(
    RMobilePhoneStore::TMobilePhoneStoreInfoV1* aInfo )
    {
    // Initialize ret value
    TInt ret( KErrNone );
    
    // if aInfo is not correct type, complete request with KErrArgument
    if( RMobilePhoneStore::KETelMobilePhoneStoreV1 != aInfo->ExtensionId() )
        {
        ret = KErrArgument;
        }

    else
        {
        // Set get info internal pointer
        iENStoreInfoChanged = aInfo;

        // Request to GSM extension, get used entries,
        // -1 as a index param-> specified location param not needed
        ret = iMmENStoreExtInterface->Read( EMmTsyENStoreGetInfoIPC, -1 );

        if ( ret == KErrNone )
            {
            iReqHandleType = EMultimodeENStoreGetInfo;
            }
        
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::CompleteGetInfo
// Complete get EN store info (capabilities)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmENStoreTsy::CompleteGetInfo(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeENStoreGetInfo );

    // Request handle was valid
    if ( EMultimodeENStoreReqHandleUnknown != reqHandle )
        {
        // No errors
        if ( KErrNone == aResult )
            {
            // The number of used entries
            TInt usedEntries( 0 );
            
            // Unpack data
            if ( aDataPackage )
                {
                aDataPackage->UnPackData( usedEntries );
                }
            
            // The name of the store
            iENStoreInfoChanged->iName = KETelEmergencyNumberStore;

            // The type of entry in the phone-side store
            iENStoreInfoChanged->iType =
                RMobilePhoneStore::EEmergencyNumberStore;

            // Indicates the capabilities of the store
            // EN store can be only read one entry at time or whole store.
            // Write and delete is prohibited.
            iENStoreInfoChanged->iCaps = KENStoreCaps;

            // Indicates the current number of used entries
            iENStoreInfoChanged->iUsedEntries = usedEntries;

            // Indicates the total numbetr of entries, GSM 11.11
            iENStoreInfoChanged->iTotalEntries = KEccStorageNumberMaxCount;
            }

        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::GetInfoCancel
// Cancel get EN store info (capabilities)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::GetInfoCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodeENStoreGetInfo );

    // Complete with KErrCancel
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::ReadAllPhase1
// Read all EN store entries, first phase
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::ReadAllPhase1(
    const TTsyReqHandle aTsyReqHandle,
    const RMobilePhone::TClientId* aId,
    TInt* aBufSize )
    {
    // Initialize ret value
    TInt ret( KErrNone );

    // Set buffer size and client ID
    iReadAllBufSizePtr = aBufSize;
    iReadAllId = *aId;

    // Request to GSM extension
    ret = iMmENStoreExtInterface->Read( EMmTsyENStoreReadAllPhase1IPC, -1 );

    // error returned
    if ( KErrNone != ret )
        {
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        // Save tsy req handle type
        iReqHandleType = EMultimodeENStoreReadAll;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::CompleteReadAllPhase1L
// Read all EN store entries, complete first phase
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::CompleteReadAllPhase1L(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    // EN number array
    CArrayPtrFlat<TENStoreResponse>* ECCArray = NULL;
    //unpack data
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeENStoreReadAll );

    // Check that the request is still ongoing
    if ( EMultimodeENStoreReqHandleUnknown != reqHandle )
        {
        // Check aError is KErrNone and aECCArray contains entries
        if ( KErrNone == aResult )
            {
            aDataPackage->UnPackData( ECCArray );

            // Read the list, store its content and then return size of this
            // buffer to client
            CMobilePhoneENList* list = CMobilePhoneENList::NewL();
            CleanupStack::PushL( list );

            // Create an entry variable to fill ENStore list
            RMobileENStore::TMobileENEntryV1 entry;

            //Reset iCountryCode, iIdentity and iAlphaId. We can't provide
            //this information
            entry.iCountryCode.Zero();
            entry.iIdentity.Zero();
            entry.iAlphaId.Zero();

            entry.iNetworkSpecific = ETrue;
            entry.iMode = RMobilePhone::ENetworkModeGsm;

            // Loop until TSY's internal ENStorage is empty
            while ( !( 0 == ECCArray->Count() ) )
                {
                // Used entry is always deleted at the end of this loop, and
                // then compressed. Therefore first item always red.
                TENStoreResponse* enStoreMsg = ECCArray->At( 0 );

                entry.iNumber.SetLength( 0 ); //Set length to 0 before copying
                entry.iNumber.Copy( enStoreMsg->iECCNumber );
                entry.iIndex = enStoreMsg->iLocation;

                // Add to ENStore list
                list->AddEntryL( entry );

                // Delete internal list member
                delete enStoreMsg;
                ECCArray->Delete( 0 );
                ECCArray->Compress();
                }

            // Store the streamed list and the client ID
            CListReadAllAttempt* read =
                CListReadAllAttempt::NewL( &iReadAllId );
            CleanupStack::PushL( read );

            // Set list
            read->iListBuf = list->StoreLC();
            CleanupStack::Pop(); // Pop the CBufFlat allocated by StoreLC

            // Append list EN entry array
            iENStoreReadAllArray->AppendL( read );
            CleanupStack::Pop( read ); // Pop the CListReadAllAttempt

            // Return the CBufFlat’s size to client
            *iReadAllBufSizePtr = ( read->iListBuf )->Size();

            // Complete first phase of list retrieval
            ReqCompleted( reqHandle, KErrNone );
            CleanupStack::PopAndDestroy( list ); // Pop & destroy list
            }
        // PMM store doesn't contain emergency numbers
        else
            {
            ReqCompleted( reqHandle, aResult );
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::ReadAllPhase2
// Read all EN store entries, second phase
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::ReadAllPhase2(
    TTsyReqHandle aTsyReqHandle,
    const RMobilePhone::TClientId* aId,
    TDes8* aBuffer )
    {
    // Initialize ret value
    TInt ret( KErrCorrupt );

    // Set read pointer to NULL
    CListReadAllAttempt* read = NULL;

    // Find the read attempt from this client
    for ( TInt i = 0; ( i < iENStoreReadAllArray->Count() ) &&
            ( KErrCorrupt == ret ); ++i )
        {
        // Get and set item from array
        // This is not deleted later on within this function if session
        // handle and subsession handle does not match.
        // Someone else 'owns' this data.
        read = iENStoreReadAllArray->At( i );

        // Check that session and client are rigth ones
        if ( ( read->iClient.iSessionHandle    == aId->iSessionHandle ) &&
            (  read->iClient.iSubSessionHandle == aId->iSubSessionHandle ) )
            {
            // Set pointer to list buffer
            TPtr8 bufPtr( ( read->iListBuf )->Ptr( 0 ) );
            // Copy the streamed list to the client
            aBuffer->Copy( bufPtr );

            // Delete read and item from internal array
            delete read;
            iENStoreReadAllArray->Delete( i );

            // Complete request
            ReqCompleted( aTsyReqHandle, KErrNone );
            // Set ret to KErrNone so that loop can exit
            ret = KErrNone;
            }
        }

    // Check ret value
    if ( KErrCorrupt == ret )
        {
        // Doesn't found the matching client for read all phase 1
        iENStoreReadAllArray->ResetAndDestroy();
        ReqCompleted( aTsyReqHandle, ret );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::ReadAllCancel
// Cancel read all EN store entries
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreTsy::ReadAllCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // Reset req handle. Returns the deleted req handle
    iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeENStoreReadAll );

    // Remove the read all attempt
    CListReadAllAttempt* read = NULL;

    // Check that read all array does not contain any data.
    // If it does, delete it.
    // NOTE: functions request mode "Flow control obeyed" currently blocks
    // request so that only one client can request data at the time.
    // Therefore, loop-functionality is not needed.
    // Just check that is there
    if ( iENStoreReadAllArray->Count() > 0x0 )
        {
        read = iENStoreReadAllArray->At( 0 );
        delete read;
        iENStoreReadAllArray->Delete( 0 );
        }

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmENStoreTsy::SetTypeOfResponse
// Gets the type of response for a given Handle.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmENStoreTsy::SetTypeOfResponse(
    const TInt aReqHandleType,
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut( 0 );

    //example switch
    switch ( aReqHandleType )
        {
        case EMultimodeENStoreRead:
            timeOut = KMmENStoreReadTimeOut;
            break;
        case EMultimodeENStoreGetInfo:
            timeOut = KMmENGetInfoTimeOut;
            break;

        //Must not use timer:
        // - all notifications
            //case EMultimodeENStoreNotifyStoreEvent:
        // - 2nd phase methods (when two phase lists are used)
            //case EMultimodeENStoreReadAll
        default:
            //does not use timer
            iTsyReqHandleStore->SetTsyReqHandle(
                aReqHandleType,
                aTsyReqHandle );
            break;
        }

    if ( timeOut > 0 )
        {
        //the timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle(
            aReqHandleType,
            aTsyReqHandle,
            timeOut );
        }
    }

// ---------------------------------------------------------------------------
// CMmENStoreTsy::Complete
// Completes the request due timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmENStoreTsy::Complete(
    TInt aReqHandleType,
    TInt aError )
    {
    //All possible TSY req handle types are listed in the
    //switch case below.
    switch( aReqHandleType )
        {
        case EMultimodeENStoreRead:
            CompleteRead( aError, NULL );
            break;
        case EMultimodeENStoreGetInfo:
            CompleteGetInfo( aError, NULL );
            break;

        //Can't use timer:
        // - all notifications
            //case EMultimodeENStoreNotifyStoreEvent:
        // - 2nd phase methods (when two phase lists are used)
            //case EMultimodeENStoreReadAll
        default:
            ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle(
                            aReqHandleType ), aError );
            break;
        }
    }
#endif // REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmENStoreTsy::Phone
// Returns pointer to CMmPhoneTsy object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneTsy* CMmENStoreTsy::Phone()
    {
    return iMmPhone;
    }

//  End of File
