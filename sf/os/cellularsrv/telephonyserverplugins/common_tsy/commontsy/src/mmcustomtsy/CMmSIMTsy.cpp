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
#include "CMmSIMTsyTraces.h"
#endif

#include "CMmSIMTsy.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmSIMTsy::CMmSIMTsy():
    iReqHandleType(ESIMTsyReqHandleUnknown)
    {
    }

void CMmSIMTsy::ConstructL(
    CMmCustomTsy* aMmCustomTsy )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_CONSTRUCTL_1, "TSY: CMmSIMTsy::ConstructL");
    iMmCustomTsy = aMmCustomTsy;

    // Create req handle store
#ifdef REQHANDLE_TIMER
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( iMmCustomTsy,
        iMmCustomTsy->iMmPhoneTsy, ESIMRequestTypeMaxNumOfRequests,
        iSIMReqHandles );
#else
   iTsyReqHandleStore = CMmTsyReqHandleStore::NewL(
        ESIMRequestTypeMaxNumOfRequests, iSIMReqHandles );
#endif // REQHANDLE_TIMER

    // By default VIAG parameters are not requested. This means that
    // when "SIM ready" indication is received, TSY will not check
    // if it needs to read VIAG parameters or not.
    iViagParamsReadRequested = EFalse;

    // Initialize pointer to NULL.
    iCurrentlyRetrievedCache = NULL;

    // create the o2 HomeZone dynamic cache
    // IGNORE if this fails due to Out-of-Memory problem
    TInt leaveCode;
    TRAP( leaveCode, iViagDynamicCache = new ( ELeave )
        CArrayFixFlat<RMmCustomAPI::TViagCacheRecordContent>( 
        ( KPhEngMaxViagHomeZones * KPhEngMaxCacheId ) ); );
    if ( leaveCode )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_CONSTRUCTL_2, "TSY: CMmSIMTsy::ConstructL: Could not construct o2 HomeZone dynamic cache, error=%{TSymbianErrorCodes}", leaveCode);
        iViagDynamicCache = NULL;
        }
    else
        {
        // initialise
        RMmCustomAPI::TViagCacheRecordContent zero;
        zero.iLac = 0xFFFF;
        zero.iCellId = 0xFFFF;
        iViagDynamicCache->AppendL( zero, ( KPhEngMaxViagHomeZones * 
            KPhEngMaxCacheId ) );
        iViagHomeZoneCacheReady = EFalse;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_CONSTRUCTL_3, "TSY: CMmSIMTsy::ConstructL: o2 HomeZone dynamic cache array created.");
        }
    }

CMmSIMTsy* CMmSIMTsy::NewL(
    CMmCustomTsy* aMmCustomTsy )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_NEWL_1, "TSY: CMmSIMTsy::NewL");
    CMmSIMTsy* self = new (ELeave) CMmSIMTsy();

    CleanupStack::PushL( self );
    self->ConstructL( aMmCustomTsy );
    CleanupStack::Pop( self );

    return self;
    }

CMmSIMTsy::~CMmSIMTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_DTOR_1, "TSY: CMmSIMTsy::~CMmSIMTsy()");
    // delete all CB ID related
    ResetAndDestroySimCbTopics();

    delete iTsyReqHandleStore;
    delete iViagDynamicCache; 
    delete iCurrentlyRetrievedCache; 
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::Init
// Initialisation method that is called from ETel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::Init()
    {
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::SupportingIPC
// Tells whether the object supports given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmSIMTsy::SupportingIPC(
    TInt  aIpc )
    {
    TInt ret( EFalse );

    switch ( aIpc )
        {
        case EReadViagHomeZoneParamsIPC:
        case EReadViagHomeZoneCacheIPC:
        case EWriteViagHomeZoneCacheIPC:
        case EWriteViagHomeZoneUHZIUESettingsIPC:
        case ECustomStartSimCbTopicBrowsingIPC:
        case ECustomGetNextSimCbTopicIPC:
        case ECustomDeleteSimCbTopicIPC:
            ret = ETrue;
            break;
        default:
            ret = EFalse;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::DoExtFuncL
// Dispatches extension function requests.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_DOEXTFUNCL_1, "TSY: CMmSIMTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, aTsyReqHandle, (TUint)this);
    TInt ret( KErrGeneral );

    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==ESIMTsyReqHandleUnknown, User::Invariant());

    switch ( aIpc )
        {
        case EReadViagHomeZoneParamsIPC:
            ret = ReadViagHomeZoneParamsL(
                REINTERPRET_CAST( RMmCustomAPI::TViagParams*, aPackage.Ptr1() 
                ), REINTERPRET_CAST( RMmCustomAPI::TViagElement*,
                aPackage.Ptr2() ) );
            break;
        case EReadViagHomeZoneCacheIPC:
            ret = ReadDynamicViagHomeZoneCacheL( aTsyReqHandle, 
                REINTERPRET_CAST( RMmCustomAPI::TViagCacheRecordId*, 
                aPackage.Ptr1() ), REINTERPRET_CAST(
                RMmCustomAPI::TViagCacheRecordContent*, aPackage.Ptr2() ) );
            break;
        case EWriteViagHomeZoneCacheIPC:
            ret = WriteViagHomeZoneCacheL(
                REINTERPRET_CAST( RMmCustomAPI::TViagCacheRecordId*, 
                    aPackage.Ptr1() ), REINTERPRET_CAST( 
                    RMmCustomAPI::TViagCacheRecordContent*, aPackage.Ptr2() )
                    );
            break;
        case ECustomStartSimCbTopicBrowsingIPC:
            ret = StartSimCbTopicBrowsingL( aTsyReqHandle );
            break;
        case ECustomGetNextSimCbTopicIPC:
            ret = GetNextSimCbTopic( aTsyReqHandle, REINTERPRET_CAST( 
                  RMmCustomAPI::TSimCbTopic*, aPackage.Ptr1()) );
            break;
        case ECustomDeleteSimCbTopicIPC:
            ret = DeleteSimCbTopicL( aTsyReqHandle, REINTERPRET_CAST(TUint*, 
                aPackage.Ptr1() ) );
            break;
        case EWriteViagHomeZoneUHZIUESettingsIPC:
            ret = WriteViagHomeZoneUHZIUESettingsL( REINTERPRET_CAST(
                RMmCustomAPI::TViagUHZIUESettings*, aPackage.Ptr1() ) );
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

    // save request handle
    if ( ESIMTsyReqHandleUnknown != iReqHandleType )
        {
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, aTsyReqHandle );
#endif // REQHANDLE_TIMER
        // We've finished with this value now. Clear it so it doesn't leak
        //  up to any other instances of this method down the call stack
        iReqHandleType = ESIMTsyReqHandleUnknown;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_DOEXTFUNCL_2, "<-- TSY: CMmSIMTsy::DoExtFuncL, error = %{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::ReqModeL
// Returns request mode for given IPC. Leaves with error code KErrNotSupported
// if the IPC number does not belong to some of the SIM API functions.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmSIMTsy::ReqModeL(
    const TInt aIpc )
    {
    CTelObject::TReqMode ret = 0;

    switch ( aIpc )
        {
        case EReadViagHomeZoneParamsIPC:
        case EReadViagHomeZoneCacheIPC:
        case EWriteViagHomeZoneCacheIPC:
        case EWriteViagHomeZoneUHZIUESettingsIPC:
        case ECustomStartSimCbTopicBrowsingIPC:
        case ECustomGetNextSimCbTopicIPC:
        case ECustomDeleteSimCbTopicIPC:
            ret = 0;    // flow control in tsy
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC. Leaves with error code
// KErrNotSupported if the IPC number does not belong to some of the SIM
// API functions.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    TInt numberOfSlots( KMmCustomDefaultSlots );

    switch ( aIpc )
        {
        case EReadViagHomeZoneParamsIPC:
        case EReadViagHomeZoneCacheIPC:
        case EWriteViagHomeZoneCacheIPC:
        case EWriteViagHomeZoneUHZIUESettingsIPC:
        case ECustomStartSimCbTopicBrowsingIPC:
        case ECustomGetNextSimCbTopicIPC:
        case ECustomDeleteSimCbTopicIPC:
           numberOfSlots = KMmCustomDefaultSlots;
           break;
        default:
            // Unknown or invalid IPC
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::CancelService
// When the clients close their sub-sessions (eg. by calling RPhone::Close),
// they may not have cancelled all their outstanding asynchronous requests
// before closing. It is up to the ETel server to clean up in this situation,
// so the server will find the list of outstanding requests related to that
// sub-session object and pass these outstanding IPC request numbers, one at
// a time, to the CancelService function in the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::CancelService(
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle)
    {
    TInt ret( KErrGeneral );

    switch ( aIpc )
        {
        case EReadViagHomeZoneParamsIPC:
            ret = ReadViagHomeZoneParamsCancel();
            break;
        case EReadViagHomeZoneCacheIPC:
            ret = ReadViagHomeZoneCacheCancel( aTsyReqHandle );
            break;
        case EWriteViagHomeZoneCacheIPC:
            ret = WriteViagHomeZoneCacheCancel();
            break;
        case EWriteViagHomeZoneUHZIUESettingsIPC:
            ret = WriteViagHomeZoneUHZIUESettingsCancel();
            break;
        case ECustomGetNextSimCbTopicIPC:
        case ECustomDeleteSimCbTopicIPC:
            // operation cannot be cancelled on DOS side
            ret = KErrNone;
            break;
        default:
            ret = KErrGeneral;
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::RegisterNotification
// Called when the server recognises this notification being posted for
// the first time on this sub-session object. It enables the TSY to "turn on"
// any regular notification messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::RegisterNotification(
    const TInt )
    {
    // No supported notifications in CMmSimTsy
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::DeregisterNotification
// Called when the server recognises that this notification will not be posted
// again because the last client to have a handle on this sub-session object
// has just closed the handle. It enables the TSY to "turn off" any regular
// notification messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::DeregisterNotification(
    const TInt )
    {
    // No supported notifications in CMmSimTsy
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::ReadViagHomeZoneParamsL
// Reads VIAG Home Zone Parameters.
// If it has been requested before SIM is ready, the request is saved and will
// be done later. See methods CheckViagHomezoneParams and
// CMmPhoneTsy::CompleteNotifySimStatusReady.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::ReadViagHomeZoneParamsL(
    RMmCustomAPI::TViagParams* aViagParameters, 
    RMmCustomAPI::TViagElement* aViagElements )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READVIAGHOMEZONEPARAMSL_1, "TSY: CMmSIMTsy::ReadViagHomeZoneParamsL");
    TInt ret( KErrNone);

    if ( ESIMTsyReqHandleUnknown != iTsyReqHandleStore->GetTsyReqHandle(
        ESIMRequestTypeReadParams ) )
        {
        return KErrServerBusy;
        }

    // Count is already checked in CustomAPI
    iViagParams = aViagParameters;
    iViagElems = aViagElements;

    // If SIM is ready, then request VIAG homezone parameters.
    // If not ready, store pointers and send request when the
    // "SIM ready" indication is received.
    if ( iMmCustomTsy->Phone()->NosBootState()->iSIMReady )
        {
        // send request to DOS (no parameters)
        ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
            EReadViagHomeZoneParamsIPC );
        }
    else
        {
        // SIM is not yet ready. TSY should send read request when SIM has
        // indicated that it is ready. See CheckViagHomezoneParams and
        // CMmPhoneTsy::CompleteNotifySimStatusReady.
        iViagParamsReadRequested = ETrue;
        }

    // If message is sent or we are waiting for adaptation indication SIM 
    // being ready. Then store handle.
    if ( KErrNone == ret )
        {
        iReqHandleType = ESIMRequestTypeReadParams;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::CheckViagHomezoneParamsL
// Reads VIAG Home Zone Parameters when it has been requested before
// SIM is ready.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::CheckViagHomezoneParamsL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_CHECKVIAGHOMEZONEPARAMSL_1, "TSY: CMmSIMTsy::CheckViagHomezoneParamsL");
    // Check if the request is pending
    if ( ESIMTsyReqHandleUnknown != iTsyReqHandleStore->GetTsyReqHandle(
        ESIMRequestTypeReadParams )  && iViagParamsReadRequested )
        {
        // Ensure that we wont flood SIM with mupltiple VIAG params
        // read requests, set flag to EFalse.
        iViagParamsReadRequested = EFalse;

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_CHECKVIAGHOMEZONEPARAMSL_2, "TSY: CMmSIMTsy::CheckViagHomezoneParamsL - Sending request" );
        // send request to DOS (no parameters)
        // Void return value, there is nothing to be done if request fails.
        (void) iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
            EReadViagHomeZoneParamsIPC );
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_CHECKVIAGHOMEZONEPARAMSL_3, "TSY: CMmSIMTsy::CheckViagHomezoneParamsL - VIAG homezone params not requested");
        }

    StartDynamicCachingL();
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::CompleteReadViagHomeZoneParamsResp
// Completes a ReadViagHomeZoneParams request (which was issued either by
// ReadViagHomeZoneParams or by CheckViagHomezoneParams).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::CompleteReadViagHomeZoneParamsResp(
    RMmCustomAPI::TViagParams* aParams, 
    RMmCustomAPI::TViagElements* aElems, 
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONEPARAMSRESP_1, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneParamsResp - aError: %{TSymbianErrorCodes}", aError );
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ESIMRequestTypeReadParams );
    if ( ESIMTsyReqHandleUnknown != reqHandle )
        {
        if ( aError == KErrNone && iViagParams && iViagElems )
            {
            *iViagParams = *aParams;
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONEPARAMSRESP_2, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneParamsResp - iSmsC: %S", iViagParams->iSmsC );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONEPARAMSRESP_3, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneParamsResp - iScp: %S",  iViagParams->iScp );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONEPARAMSRESP_4, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneParamsResp - iSubscribedZoneAndVersion: %d", iViagParams->iSubscribedZoneAndVersion );

            for ( TUint8 i = 0; i < RMmCustomAPI::KViagElementCount; i++ )
                {
                iViagElems[ i ].iCoordinates.iX = aElems->At(i).iCoordinates.iX;
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONEPARAMSRESP_5, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneParamsResp - Element: %d, iX: %d", (TUint)i, aElems->At(i).iCoordinates.iX );

                iViagElems[ i ].iCoordinates.iY = aElems->At(i).iCoordinates.iY;
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONEPARAMSRESP_6, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneParamsResp - Element: %d, iY: %d", (TUint)i, aElems->At(i).iCoordinates.iY );

                iViagElems[ i ].iCoordinates.iR2 = aElems->
                    At(i).iCoordinates.iR2;
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONEPARAMSRESP_7, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneParamsResp - Element: %d, iR2: %d", (TUint)i, aElems->At(i).iCoordinates.iR2 );

                iViagElems[ i ].iCoordinates.iZoneId = aElems->
                    At(i).iCoordinates.iZoneId;
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONEPARAMSRESP_8, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneParamsResp - Element: %d, iZoneId: %d", i, aElems->At(i).iCoordinates.iZoneId );

                iViagElems[ i ].iName.Copy( aElems->At(i).iName );
                iViagElems[ i ].iActiveFlag = aElems->At(i).iActiveFlag;
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONEPARAMSRESP_9, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneParamsResp - Element: %d, iActiveFlag: %d", i, aElems->At(i).iActiveFlag );
                }
            }
        iMmCustomTsy->ReqCompleted( reqHandle, aError );
        }
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::ReadViagHomeZoneParamsCancel
// Cancels reading VIAG Home Zone parameters.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::ReadViagHomeZoneParamsCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READVIAGHOMEZONEPARAMSCANCEL_1, "TSY: CMmSIMTsy::ReadViagHomeZoneParamsCancel");
    // reset the pointer to client space
    iViagParams = NULL;
    iViagElems = NULL;

    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ESIMRequestTypeReadParams );

    if ( ESIMTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel
    	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::ReadViagHomeZoneCacheL
// Reads VIAG Home Zone Cache.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::ReadViagHomeZoneCacheL(
    RMmCustomAPI::TViagCacheRecordId* aViagRecordId,
    RMmCustomAPI::TViagCacheRecordContent* aViagRecordContent )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READVIAGHOMEZONECACHEL_1, "TSY: CMmSIMTsy::ReadViagHomeZoneCacheL");
    TInt ret( KErrGeneral );

    if ( ESIMTsyReqHandleUnknown != iTsyReqHandleStore->GetTsyReqHandle(
        ESIMRequestTypeReadCache ) )
        {
        return KErrServerBusy;
        }

    iViagReadCacheRecord = aViagRecordContent;

    // send request to DOS
    // packed parameter: RMmCustomAPI::TViagCacheRecordId
    CMmDataPackage dataPackage;
    dataPackage.PackData( aViagRecordId );
    ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
        EReadViagHomeZoneCacheIPC, &dataPackage );

    if ( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = ESIMRequestTypeReadCache;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::CompleteReadViagHomeZoneCacheRespL
// Completes reading VIAG Home Zone Cache.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::CompleteReadViagHomeZoneCacheRespL(
    RMmCustomAPI::TViagCacheRecordContent* aViagRecord,
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONECACHERESPL_1, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneCacheRespL");
    iLastViagHomeZoneCacheError = aError;
    if (iCurrentlyRetrievedCache == NULL)
        {
        // Got an unexpected update... ignore!
        return;
        }
    if ( KErrNone == aError )
        {
        // response OK
        if ( iViagDynamicCache )
            {
            // save in dynamic cache
            // some arithmetics:
            //     cache ids range 1..4
            //     record ids range 0..20
            //     array indexes range 0..83
            // ==> A[c][r] = A[((c-1)*rMax)+r]
            TInt arrayIndex =
                ( ( iCurrentlyRetrievedCache->iCacheId - 1 ) * 21 ) +
                iCurrentlyRetrievedCache->iRecordId;
            iViagDynamicCache->At( arrayIndex ) = *aViagRecord;
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONECACHERESPL_2, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneCacheRespL:Entry at cacheId=%d, recordId=%d read.", iCurrentlyRetrievedCache->iCacheId,iCurrentlyRetrievedCache->iRecordId);

            TRAP_IGNORE(
                // loop. loop four caches times 21 entries!
            if ( 20 > iCurrentlyRetrievedCache->iRecordId )
                {
                iCurrentlyRetrievedCache->iRecordId++;
                // send request to DOS
                // packed parameter: RMmCustomAPI::TViagCacheRecordId
                CMmDataPackage dataPackage;
                dataPackage.PackData( iCurrentlyRetrievedCache );
                iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
                    EReadViagHomeZoneCacheIPC, &dataPackage );
                }
            else if ( 4 > iCurrentlyRetrievedCache->iCacheId )
                {
                // next cache
                iCurrentlyRetrievedCache->iCacheId++;
                iCurrentlyRetrievedCache->iRecordId = 0;
                // send request to DOS
                // packed parameter: RMmCustomAPI::TViagCacheRecordId
                CMmDataPackage dataPackage;
                dataPackage.PackData( iCurrentlyRetrievedCache );
                iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
                    EReadViagHomeZoneCacheIPC, &dataPackage );
                }
            else
                {
                // caching done
                delete iCurrentlyRetrievedCache;
                iCurrentlyRetrievedCache = NULL;
                iViagHomeZoneCacheReady = ETrue;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONECACHERESPL_3, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneCacheRespL:Caching completed successfully!");

                if( 0 < iReadViagHomeZoneCacheRequests.Count() )
                    {
                    
                    for ( TInt i=0; i <iReadViagHomeZoneCacheRequests.Count() ; i++ )
                        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONECACHERESPL_4, "TSY: CMmSIMTsy::CompleteReadDynamicViagHomeZoneCacheRespL: for i:%d", i);
                        TReadViagHomeZoneCacheRequest* req = 
		  			    iReadViagHomeZoneCacheRequests[ i ];
		  			    
                        // copy data from dynamic cache to client
                        // some arithmetics:
                        //     cache ids range 1..4
                        //     record ids range 0..20
                        //     array indexes range 0..83
                        // ==> A[c][r] = A[((c-1)*rMax)+r]
                        TInt arrayIndex = ( ( req->iViagRecordId->iCacheId - 1 ) * 21 ) +
                        req->iViagRecordId->iRecordId;//iCacheId;  
                        
                        //set value in client side
		                *(req->iViagRecordContent) =iViagDynamicCache->At( arrayIndex );
			            //reset the return pointer
		                iMmCustomTsy->ReqCompleted( req->iReqHandle, KErrNone );
                        }
                    iReadViagHomeZoneCacheRequests.ResetAndDestroy();
                        
                    }
                }
            );
            }
        }
        else
            {
            // caching aborted
        delete iCurrentlyRetrievedCache;
        iCurrentlyRetrievedCache = NULL;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONECACHERESPL_5, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneCacheRespL:There was a problem reading cache values from SIM, error=%{TSymbianErrorCodes}", aError);
            //iViagHomeZoneCacheReady = ETrue;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONECACHERESPL_6, "TSY: CMmSIMTsy::CompleteReadViagHomeZoneCacheRespL: Caching completed unsuccessfully!");

            for (TInt i = 0; i < iReadViagHomeZoneCacheRequests.Count(); i++)
                {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEREADVIAGHOMEZONECACHERESPL_7, "TSY: CMmSIMTsy::CompleteReadDynamicViagHomeZoneCacheRespL: for i:%d", i);
                TReadViagHomeZoneCacheRequest* req =
                        iReadViagHomeZoneCacheRequests[i];
                iMmCustomTsy->ReqCompleted(req->iReqHandle, aError);
                }
            iReadViagHomeZoneCacheRequests.ResetAndDestroy();

            }
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::ReadViagHomeZoneCacheCancel
// Cancels reading VIAG Home Zone cache.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::ReadViagHomeZoneCacheCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READVIAGHOMEZONECACHECANCEL_1, "TSY: CMmSIMTsy::ReadViagHomeZoneCacheCancel");
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READVIAGHOMEZONECACHECANCEL_2, "TSY: CMmSIMTsy::ReadViagHomeZoneCacheCancel - Handle:%d",aTsyReqHandle);
    // Reset the pointer to client space
    iViagReadCacheRecord = NULL;

    // Check if request handle already exists
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        ESIMRequestTypeReadCache );

	if (aTsyReqHandle == reqHandle ) 
		{
	    // Reset the request handle. Returns the deleted req handle
        reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
            ESIMRequestTypeReadCache );
        if ( ESIMTsyReqHandleUnknown != reqHandle )
        	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READVIAGHOMEZONECACHECANCEL_3, "TSY: CMmSIMTsy::ReadViagHomeZoneCacheCancel - Completed with Cancel Handle:%d",reqHandle);
			// Complete request with cancel
			iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
        	}
        }
	else
	    {		
        if( 0 < iReadViagHomeZoneCacheRequests.Count() )
            {
            // Search the request handle in the array
            for ( TInt i=0; i < iReadViagHomeZoneCacheRequests.Count(); i++ )
                {
                TReadViagHomeZoneCacheRequest* req = 
                iReadViagHomeZoneCacheRequests[ i ];
                
                if (aTsyReqHandle == req->iReqHandle)
                	{
                	// Complete the request if the request handle is found
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READVIAGHOMEZONECACHECANCEL_4, "TSY: CMmSIMTsy::ReadViagHomeZoneCacheCancel - Complete with Cancel from array Handle:%d Id=%d ", req->iReqHandle,i);
                    iMmCustomTsy->ReqCompleted( req->iReqHandle, KErrCancel );
	                delete iReadViagHomeZoneCacheRequests[ i ];
	                iReadViagHomeZoneCacheRequests.Remove(i);
                    break;                 	
                    }
                }
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::ReadDynamicViagHomeZoneCacheL
// Reads Dynamic VIAG Home Zone cache.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::ReadDynamicViagHomeZoneCacheL(
    const TTsyReqHandle aTsyReqHandle,
    RMmCustomAPI::TViagCacheRecordId* aViagRecordId,
    RMmCustomAPI::TViagCacheRecordContent* aViagRecordContent )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READDYNAMICVIAGHOMEZONECACHEL_1, "TSY: CMmSIMTsy::ReadDynamicViagHomeZoneCache:Is ViagHomeZoneCache Ready =%d", iViagHomeZoneCacheReady);
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READDYNAMICVIAGHOMEZONECACHEL_2, "TSY: CMmSIMTsy::ReadDynamicViagHomeZoneCache iCacheId %d iRecordId %d",aViagRecordId->iCacheId,aViagRecordId->iRecordId );
    TInt err = KErrNone;
	//  KPhEngMaxViagHomeZones(4) * KPhEngMaxCacheId(21)
	if ( 1 > aViagRecordId->iCacheId || 4 < aViagRecordId->iCacheId || 
		 0 > aViagRecordId->iRecordId || 20 < aViagRecordId->iRecordId )
		{
		err = KErrArgument;	
		}
    else if ( iViagDynamicCache && iViagHomeZoneCacheReady )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READDYNAMICVIAGHOMEZONECACHEL_3, "TSY: CMmSIMTsy::ReadDynamicViagHomeZoneCache: else if : Cache is ready, Reading cache possible");
        // copy data from dynamic cache to client
        // some arithmetics:
        //     cache ids range 1..4
        //     record ids range 0..20
        //     array indexes range 0..83
        // ==> A[c][r] = A[((c-1)*rMax)+r]
        TInt arrayIndex = ( ( aViagRecordId->iCacheId - 1 ) * 21 ) +
            aViagRecordId->iRecordId;
        *aViagRecordContent = iViagDynamicCache->At( arrayIndex );
        }
    else if ( !iViagHomeZoneCacheReady )
        {
 OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_READDYNAMICVIAGHOMEZONECACHEL_4, "TSY: CMmSIMTsy::ReadDynamicViagHomeZoneCache: else if : Cache is NOT ready, Reading cache NOT possible");
        //The request is already in processing because of previous request
        //Complete request with status value informing the client about
		TReadViagHomeZoneCacheRequest* req = 
                              new (ELeave) TReadViagHomeZoneCacheRequest();
		req->iReqHandle = aTsyReqHandle;
		req->iViagRecordId = aViagRecordId;
		req->iViagRecordContent = aViagRecordContent;
		iReadViagHomeZoneCacheRequests.AppendL( req );
		if(iLastViagHomeZoneCacheError != KErrNone)
		    {
            // Got an error last time, ask again
            StartDynamicCachingL();
		    }
        }

    // complete to client
    if ( iViagHomeZoneCacheReady )
        {
        iMmCustomTsy->ReqCompleted( aTsyReqHandle, err );
        return KErrNone;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::WriteViagHomeZoneCacheL
// Writes VIAG Home Zone Cache.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::WriteViagHomeZoneCacheL(
    RMmCustomAPI::TViagCacheRecordId* aViagRecordId,
    RMmCustomAPI::TViagCacheRecordContent* aViagRecordContent )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_WRITEVIAGHOMEZONECACHEL_1, "TSY: CCMmSIMTsy::WriteViagHomeZoneCacheL iCacheId %d iRecordId %d",aViagRecordId->iCacheId,aViagRecordId->iRecordId );
    TInt ret( KErrGeneral );

    if ( ESIMTsyReqHandleUnknown != iTsyReqHandleStore->GetTsyReqHandle(
        ESIMRequestTypeWriteCache ) )
        {
        return KErrServerBusy;
        }
	if ( 1 > aViagRecordId->iCacheId || 4 < aViagRecordId->iCacheId || 
		 0 > aViagRecordId->iRecordId || 20 < aViagRecordId->iRecordId )
		{
		return KErrArgument;	
		}
    else if ( iViagDynamicCache )
        {
        // copy data from client to dynamic cache
        // some arithmetics:
        //     cache ids range 1..4
        //     record ids range 0..20
        //     array indexes range 0..83
        // ==> A[c][r] = A[((c-1)*rMax)+r]
        TInt arrayIndex = ( ( aViagRecordId->iCacheId - 1 ) * 21 ) +
            aViagRecordId->iRecordId;
        iViagDynamicCache->At( arrayIndex ) = *aViagRecordContent;
        }

    // send request to DOS
    // packed parameters: RMmCustomAPI::TViagCacheRecordId and
    // RMmCustomAPI::TViagCacheRecordContent
    CMmDataPackage dataPackage;
    dataPackage.PackData( aViagRecordId, aViagRecordContent );
    ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
        EWriteViagHomeZoneCacheIPC, &dataPackage );

    if( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = ESIMRequestTypeWriteCache;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::CompleteWriteViagHomeZoneCacheResp
// Completes writing VIAG Home Zone Cache.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::CompleteWriteViagHomeZoneCacheResp(
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEWRITEVIAGHOMEZONECACHERESP_1, "TSY: CMmSIMTsy::CompleteWriteViagHomeZoneCacheResp");
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ESIMRequestTypeWriteCache );

    if ( ESIMTsyReqHandleUnknown != reqHandle )
        {
        iMmCustomTsy->ReqCompleted( reqHandle, aError );
        }
        
    return;
    }

// ----------------------------------------------------------------------------
// CMmSIMTsy::CompleteWriteViagHomeZoneUHZIUESettingsResp
// This method copletes the VIAG Home Zone UHZIUE settings write request.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMmSIMTsy::CompleteWriteViagHomeZoneUHZIUESettingsResp(
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEWRITEVIAGHOMEZONEUHZIUESETTINGSRESP_1, "TSY: CMmSIMTsy::CompleteWriteViagHomeZoneUHZIUESettingsResp error = %{TSymbianErrorCodes}",aError);
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ESIMRequestTypeWriteSettings );

    if ( ESIMTsyReqHandleUnknown != reqHandle )
        {
        iMmCustomTsy->ReqCompleted( reqHandle, aError );
        }
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::WriteViagHomeZoneCacheCancel
// Cancels writing VIAG Home Zone cache.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::WriteViagHomeZoneCacheCancel()
    {
    // reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ESIMRequestTypeWriteCache );

    if ( ESIMTsyReqHandleUnknown != reqHandle )
    	{
    	// complete with cancel
    	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::StartDynamicCachingL
// Start dynamic caching
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::StartDynamicCachingL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_STARTDYNAMICCACHINGL_1, "TSY: CMmSIMTsy::StartDynamicCachingL");
    iLastViagHomeZoneCacheError = KErrNone;
	// Make sure there are no ongoing dynamic caching requests.. 
	if( iCurrentlyRetrievedCache ) 
		{ 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_STARTDYNAMICCACHINGL_2, "TSY: CMmSIMTsy::StartDynamicCachingL - iCurrentlyRetrievedCache already exists - no new request done");
		return; 
		} 

    TInt trapError;
    TRAP( trapError, iCurrentlyRetrievedCache = new (ELeave) 
        RMmCustomAPI::TViagCacheRecordId(); );
    if ( trapError )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_STARTDYNAMICCACHINGL_3, "TSY: CMmSIMTsy::StartDynamicCachingL:Could not start caching, error=%{TSymbianErrorCodes}", trapError);
        return;
    }
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_STARTDYNAMICCACHINGL_4, "TSY: CMmSIMTsy::StartDynamicCachingL:Starting caching now.");
    iCurrentlyRetrievedCache->iCacheId = 1;  // caches range from 1 to 4
    iCurrentlyRetrievedCache->iRecordId = 0; // records range from 0 to 20
    // send request to DOS
    // packed parameter: RMmCustomAPI::TViagCacheRecordId
    CMmDataPackage dataPackage;
    dataPackage.PackData( iCurrentlyRetrievedCache );
    iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
        EReadViagHomeZoneCacheIPC, &dataPackage );

    }

// ----------------------------------------------------------------------------
// CMmSIMTsy::ReadViagHomeZoneUHZIUESettingsL
// This method writes specified VIAG Home Zone UHZIUE settings to SIM.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CMmSIMTsy::WriteViagHomeZoneUHZIUESettingsL(
    RMmCustomAPI::TViagUHZIUESettings* aSettings )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_WRITEVIAGHOMEZONEUHZIUESETTINGSL_1, "TSY: CMmSIMTsy::WriteViagHomeZoneUHZIUESettingsL");
    TInt ret( KErrGeneral );

    if ( ESIMTsyReqHandleUnknown != iTsyReqHandleStore->GetTsyReqHandle(
        ESIMRequestTypeWriteSettings ) )
        {
        return KErrServerBusy;
        }

    CMmDataPackage dataPackage;
    dataPackage.PackData( aSettings );
    ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
        EWriteViagHomeZoneUHZIUESettingsIPC, &dataPackage );

    if( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = ESIMRequestTypeWriteSettings;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::WriteViagHomeZoneUHZIUESettingsCancel
// Cancels writing request of VIAG Home Zone UHZIUE settings.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::WriteViagHomeZoneUHZIUESettingsCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_WRITEVIAGHOMEZONEUHZIUESETTINGSCANCEL_1, "TSY: CMmSIMTsy::WriteViagHomeZoneUHZIUESettingsCancel");
    // check if handle is not in use.
    TTsyReqHandle scFileHandle = 
        iTsyReqHandleStore->GetTsyReqHandle( ESIMRequestTypeWriteSettings );

    if( ESIMTsyReqHandleUnknown != scFileHandle )
        {
        // reset the req handle
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            ESIMRequestTypeWriteSettings );

        if ( ESIMTsyReqHandleUnknown != reqHandle )
        	{
        	// complete with cancel
        	iMmCustomTsy->ReqCompleted( reqHandle, KErrCancel );
        	}
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::StartSimCbTopicBrowsingL
// This method reads Cell Broadcast messages from SIM, and caches them in
// iSimCbTopics. No data will be copied to client, this is done via
// GetNextSimCbTopic.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::StartSimCbTopicBrowsingL(
    const TTsyReqHandle /*aTsyReqHandle */)
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_STARTSIMCBTOPICBROWSINGL_1, "TSY: CMmSIMTsy::StartSimCbTopicBrowsingL");
    // reset variables
    ResetAndDestroySimCbTopics();
    iGetNextSimCbTopicIndex = KNoCbIds;
    TInt ret( KErrNone );

    if ( ESIMTsyReqHandleUnknown != iTsyReqHandleStore->GetTsyReqHandle(
         ESIMRequestTypeStartSimCbTopicBrowsing ) ||
         ESIMTsyReqHandleUnknown != iTsyReqHandleStore->GetTsyReqHandle(
         ESIMRequestTypeDeleteSimCbTopic ) )
        {
        // this request cannot be processed if either reading or deleting
        // CB topics is currently pending
        ret = KErrServerBusy;
        }
    else
        {
        // send request to DOS (no packed parameters).
        ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
            ECustomStartSimCbTopicBrowsingIPC );
        if ( KErrNone == ret )
            {
            // store the request handle
            iReqHandleType = ESIMRequestTypeStartSimCbTopicBrowsing;
            }
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::CompleteStartSimCbTopicBrowsing
// This method completes the StartSimCbTopicBrowsing request. No data will be
// copied to client, this is done via GetNextSimCbTopic.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::CompleteStartSimCbTopicBrowsing(
    CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* aTopics, 
    TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETESTARTSIMCBTOPICBROWSING_1, "TSY: CMmSIMTsy::CompleteStartSimCbTopicBrowsing");
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ESIMRequestTypeStartSimCbTopicBrowsing );

    if ( ESIMTsyReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aError && aTopics )
            {
            // Request succeeded. Replace old CB topic array by the new one.
            ResetAndDestroySimCbTopics();
            iGetNextSimCbTopicIndex = 0;
            TRAP_IGNORE( iSimCbTopics = new ( ELeave ) 
                CArrayFixFlat<RMmCustomAPI::TSimCbTopic>( aTopics->Count() );
            // copy entries
            for ( int i = 0; i < aTopics->Count(); i++ )
                {
                iSimCbTopics->AppendL( aTopics->At( i ) );
                }
                );
            }
        else
            {
            // Request failed. Destroy CB topics array
            ResetAndDestroySimCbTopics();
            iGetNextSimCbTopicIndex = KNoCbIds;
            }

        // complete request
        iMmCustomTsy->ReqCompleted( reqHandle, aError );
        }
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::GetNextSimCbTopic
// This method get's the next id from local array and put's it to aSimCbTopic
// which is a pointer to client side structure.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::GetNextSimCbTopic(
    TTsyReqHandle aTsyReqHandle, 
    RMmCustomAPI::TSimCbTopic *aSimCbTopic )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_GETNEXTSIMCBTOPIC_1, "TSY: CMmSIMTsy::GetNextSimCbTopic");
    // initialize ret with KErrNotFound
    TInt ret ( KErrNotFound );

    if( ( iGetNextSimCbTopicIndex >= 0 ) && iSimCbTopics )
        {
        if( iSimCbTopics->Count() > iGetNextSimCbTopicIndex )
            {
            // read the next id and put it to client side pointer
            *aSimCbTopic = ( iSimCbTopics->At( iGetNextSimCbTopicIndex ) );
            iGetNextSimCbTopicIndex++;
            ret = KErrNone;
            }
        else
            {
            // set index to -1
            iGetNextSimCbTopicIndex = KNoCbIds;
            }
        }

    // complete request
    iMmCustomTsy->ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::DeleteSimCbTopicL
// Delete a CB topic stored on SIM.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSIMTsy::DeleteSimCbTopicL(
    const TTsyReqHandle /*aTsyReqHandle */, 
    TUint* aSimCbTopicNumber )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_DELETESIMCBTOPICL_1, "TSY: CMmSIMTsy::DeleteSimCbTopicL");
    TInt ret( KErrNone );
    TBool topicInSimMemoryDelete ( EFalse );

    // Check that neither browsing nor deleting request is pending
    if ( ESIMTsyReqHandleUnknown != iTsyReqHandleStore->GetTsyReqHandle(
         ESIMRequestTypeDeleteSimCbTopic ) ||
         ESIMTsyReqHandleUnknown != iTsyReqHandleStore->GetTsyReqHandle(
         ESIMRequestTypeStartSimCbTopicBrowsing ) )
        {
        ret = KErrServerBusy;
        }

    // Find the topic id in cached list
    if ( ( KErrNone == ret ) && iSimCbTopics )
    	{
    	iSimCbTopicIndexToBeDeleted = KErrNotFound;
        for ( int i = 0; i < iSimCbTopics->Count(); i++ )
            {
            if ( iSimCbTopics->At( i ).iNumber == *aSimCbTopicNumber )
                {
                // Store index of entry to be deleted
                iSimCbTopicIndexToBeDeleted = i;
                ret = KErrNone;
                topicInSimMemoryDelete = ETrue;
                break;
                }
            }
        }

    // Send delete request to DOS
    if ( KErrNone == ret )
        {
        // Send delete request to DOS
        // Packed parameter: TUint with topic id
        CMmDataPackage dataPackage;
        dataPackage.PackData( aSimCbTopicNumber, &topicInSimMemoryDelete );
        ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL(
            ECustomDeleteSimCbTopicIPC, &dataPackage );
        }

    if ( KErrNone == ret )
        {
        // Store the request handle
        iReqHandleType = ESIMRequestTypeDeleteSimCbTopic;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::CompleteDeleteSimCbTopic
// Completes DeleteSimCbTopic request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::CompleteDeleteSimCbTopic(
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETEDELETESIMCBTOPIC_1, "TSY: CMmSIMTsy::CompleteDeleteSimCbTopic- aError: %{TSymbianErrorCodes}", aError);
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        ESIMRequestTypeDeleteSimCbTopic );

    if ( ESIMTsyReqHandleUnknown != reqHandle )
        {

    // if delete was successful, the cached array has to be updated
    if ( KErrNone == aError )
        {
        // Delete entry from array
        if ( iSimCbTopics && ( iSimCbTopicIndexToBeDeleted != KErrNotFound ) )
        	{
        	iSimCbTopics->Delete( iSimCbTopicIndexToBeDeleted );
        	iSimCbTopics->Compress();
        	}

        // Reset index, it could go crazy otherwise
        iGetNextSimCbTopicIndex = 0;

        // Reset index of topic to be deleted
        iSimCbTopicIndexToBeDeleted = KErrNotFound;
        }

        iMmCustomTsy->ReqCompleted( reqHandle, aError );
        }
    }


// ---------------------------------------------------------------------------
// CMmSIMTsy::ResetAndDestroySimCbTopics
// Destroys the CB message id array
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::ResetAndDestroySimCbTopics()
    {
    if( iSimCbTopics )
        {
        iSimCbTopics->Reset();
        delete iSimCbTopics;
        iSimCbTopics = NULL;
        }
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmSIMTsy::SetTypeOfResponse
// Sets the type of response for a given handle. Automatic mode includes an
// automatic response in case of non response from the DOS in a specified time.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSIMTsy::SetTypeOfResponse(
    const TInt aReqHandleType, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut( 0 );
    TInt ipc( 0 );

    switch ( aReqHandleType )
        {
        case ESIMRequestTypeReadParams:
            timeOut = KMmViagHomeZoneReadParamsTimeOut;
            ipc = EReadViagHomeZoneParamsIPC;
            break;
        case ESIMRequestTypeReadCache:
            timeOut = KMmViagHomeZoneReadCacheTimeOut;
            ipc = EReadViagHomeZoneCacheIPC;
            break;
        case ESIMRequestTypeWriteCache:
            timeOut = KMmViagHomeZoneWriteCacheTimeOut;
            ipc = EWriteViagHomeZoneCacheIPC;
            break;
        case ESIMRequestTypeWriteSettings:
            timeOut = KMmViagHomeZoneWriteSettingsTimeOut;
            ipc = EWriteViagHomeZoneUHZIUESettingsIPC;
            break;
        case ESIMRequestTypeStartSimCbTopicBrowsing:
            timeOut = KMmStartSimCbTopicBrowsingTimeOut;
            ipc = ECustomStartSimCbTopicBrowsingIPC;
            break;
        case ESIMRequestTypeDeleteSimCbTopic:
        	timeOut = KMmDeleteSimCbTopicTimeOut;
        	ipc = ECustomDeleteSimCbTopicIPC;
        	break;
        default:
            // Does not use timer
            iTsyReqHandleStore->SetTsyReqHandle(
                aReqHandleType, aTsyReqHandle );
            break;
        }

    if ( 0 < timeOut )
        {
        // The timeout parameter is given in seconds.
        iTsyReqHandleStore->SetTsyReqHandle( aReqHandleType, aTsyReqHandle,
            timeOut, ipc );
        }
    }

// ---------------------------------------------------------------------------
// CMmSIMTsy::Complete
// Completes the request due the timer expiration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
void CMmSIMTsy::Complete(
    TInt aReqHandleType, 
    TInt aError )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMTSY_COMPLETE_1,  "CustomTSY: CMmSIMTsy::Complete.\n\t ReqHandleType:%d \n\t Error:%{TSymbianErrorCodes}\n", aReqHandleType, aError );

    // All possible TSY req handle types are listed in the
    // switch case below.
    switch ( aReqHandleType )
        {
        case ESIMRequestTypeReadParams:
            CompleteReadViagHomeZoneParamsResp( NULL, NULL, aError );
            break;
        case ESIMRequestTypeReadCache:
            TRAP_IGNORE( 
            	CompleteReadViagHomeZoneCacheRespL( NULL, aError ););
            break;
        case ESIMRequestTypeWriteCache:
            CompleteWriteViagHomeZoneCacheResp( aError );
            break;
        case ESIMRequestTypeWriteSettings:
            CompleteWriteViagHomeZoneUHZIUESettingsResp( aError );
            break;
        case ESIMRequestTypeStartSimCbTopicBrowsing:
            CompleteStartSimCbTopicBrowsing( NULL, aError );
            break;
        default:
            iMmCustomTsy->ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle(
                aReqHandleType ), aError );
            break;
        }
    }
#endif // REQHANDLE_TIMER

//  End of File
