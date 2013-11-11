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
#include "CMmSimLockTsyTraces.h"
#endif

#include "CMmSimLockTsy.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmSimLockTsy::CMmSimLockTsy()
    {
    return;
    }

void CMmSimLockTsy::ConstructL(
    CMmCustomTsy* aMmCustomTsy )
    {
    iMmCustomTsy = aMmCustomTsy;

    // Create req handle store
#ifdef REQHANDLE_TIMER   
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( iMmCustomTsy,
        iMmCustomTsy->iMmPhoneTsy, ESimLockRequestTypeMaxNumOfRequests, 
        iSimLockReqHandles );
#else
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( 
        ESimLockRequestTypeMaxNumOfRequests, iSimLockReqHandles );
#endif // REQHANDLE_TIMER

    return;
    }

CMmSimLockTsy* CMmSimLockTsy::NewL(
    CMmCustomTsy* aMmCustomTsy )
    {
    CMmSimLockTsy* self = new (ELeave) CMmSimLockTsy();
    
    CleanupStack::PushL( self );
    self->ConstructL( aMmCustomTsy );
    CleanupStack::Pop();

    return self;
    }

CMmSimLockTsy::~CMmSimLockTsy()
    {
    delete iTsyReqHandleStore;
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::Init
// Initialisation method that is called from ETel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSimLockTsy::Init()
    {
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::SupportingIPC
// Tells whether the object supports given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmSimLockTsy::SupportingIPC(
    TInt aIpc )
    {
    TBool ret( EFalse );
    
    switch ( aIpc )
        {
        case ECustomSimLockActivateIPC:
        case ECustomSimLockDeActivateIPC:
            {
            ret = ETrue;
            break;
            }
        default:
            {
            ret = EFalse;
            break;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::DoExtFuncL
// Dispatches extension function requests. At the beginning of this method,
// packaged pointers are unpacked. Then the correct function is selected
// using the IPC number.
// If the IPC is supported, return the return value of corresponding function,
// else return KErrNotSupported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSimLockTsy::DoExtFuncL( 
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMLOCKTSY_DOEXTFUNCL_1, "TSY: CMmSimLockTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, aTsyReqHandle, (TUint)this);
    TInt ret(KErrNone);

    switch ( aIpc )
        {
        case ECustomSimLockActivateIPC:
            ret = ActivateSimLockL( aTsyReqHandle, 
                REINTERPRET_CAST( RMmCustomAPI::TSimLockPassword*,
                    aPackage.Ptr1() ),
                REINTERPRET_CAST( RMmCustomAPI::TLockNumber*,
                    aPackage.Ptr2() ) );
            break;
        case ECustomSimLockDeActivateIPC:   
            ret = DeActivateSimLockL( aTsyReqHandle, 
                REINTERPRET_CAST( RMmCustomAPI::TSimLockPassword*,
                    aPackage.Ptr1() ),
                REINTERPRET_CAST( RMmCustomAPI::TLockNumber*,
                    aPackage.Ptr2() ) );
            break;
        default:
            ret = KErrNotSupported;
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMLOCKTSY_DOEXTFUNCL_2, "TSY: CMmSimLockTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
	return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmSimLockTsy::ReqModeL
// Returns request mode for given IPC. Leaves with error code KErrNotSupported
// if the IPC number does not belong to some of the SimLock API functions. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmSimLockTsy::ReqModeL( 
    const TInt aIpc )
    {
    CTelObject::TReqMode ret = 0;

    switch ( aIpc )
        {
        case ECustomSimLockActivateIPC:
        case ECustomSimLockDeActivateIPC:
            ret = 0;    // flow control in tsy
            break;
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC. Leaves with error code 
// KErrNotSupported if the IPC number does not belong to some of the SimLock 
// API functions. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSimLockTsy::NumberOfSlotsL( 
    const TInt aIpc )
    {
    TInt numberOfSlots = KMmCustomDefaultSlots;

    switch ( aIpc )
        {
        case ECustomSimLockActivateIPC:
        case ECustomSimLockDeActivateIPC:
            numberOfSlots = KMmCustomSimLockSlots;
            break;
        default:
            // Unknown or invalid IPC
            User::Leave( KErrNotSupported );
            break;
        }
    
    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::CancelService
// When the clients close their sub-sessions (eg. by calling RPhone::Close),
// they may not have cancelled all their outstanding asynchronous requests
// before closing. It is up to the ETel server to clean up in this situation,
// so the server will find the list of outstanding requests related to that
// sub-session object and pass these outstanding IPC request numbers, one at
// a time, to the CancelService function in the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSimLockTsy::CancelService( 
    const TInt aIpc, 
    const TTsyReqHandle )
    {
    TInt ret( KErrGeneral );

    switch ( aIpc )
        {
        case ECustomSimLockActivateIPC:
        case ECustomSimLockDeActivateIPC:
            // request can't be cancelled on DOS side
            ret = KErrNone;
            break;
        default:
            return KErrGeneral; 
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::RegisterNotification
// Called when the server recognises this notification being posted for 
// the first time on this sub-session object. It enables the TSY to "turn on" 
// any regular notification messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSimLockTsy::RegisterNotification( 
    const TInt )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::DeregisterNotification
// Called when the server recognises that this notification will not be posted 
// again because the last client to have a handle on this sub-session object 
// has just closed the handle. It enables the TSY to "turn off" any regular 
// notification messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSimLockTsy::DeregisterNotification( 
    const TInt )
    {
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::ActivateSimLockL
// This method activates SimLock.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSimLockTsy::ActivateSimLockL( 
    const TTsyReqHandle aTsyReqHandle, 
    RMmCustomAPI::TSimLockPassword* aPassword, 
    RMmCustomAPI::TLockNumber* aLockNumber )
    {
    TInt ret( KErrArgument );

    if ( NULL != iTsyReqHandleStore->GetTsyReqHandle(
         ESimLockRequestTypeActivateSimLock ) )
        {
        return KErrServerBusy;
        }
    
    switch ( *aLockNumber )
        {
        case RMmCustomAPI::EOperator:
        case RMmCustomAPI::EGid1:
        case RMmCustomAPI::EGid2:
        case RMmCustomAPI::EImsi:
        case RMmCustomAPI::EOperator_Gid1:
        case RMmCustomAPI::EOperator_Gid2:
            iActivateSimLockNumber = aLockNumber;
            break;
        default:
            return KErrArgument;
        }
    iActivateSimLockPassword = aPassword;
    
    // Ask DOS to activate SimLock
    // packed parameters: RMmCustomAPI::TSimLockPassword and
    // RMmCustomAPI::TLockNumber
    CMmDataPackage dataPackage;
    dataPackage.PackData( aPassword, aLockNumber );
    ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL( 
        ECustomSimLockActivateIPC, &dataPackage );

    if ( KErrNone == ret )
        {
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( ESimLockRequestTypeActivateSimLock,
            aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle(
            ESimLockRequestTypeActivateSimLock, aTsyReqHandle );
#endif // REQHANDLE_TIMER
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::CompleteActivateSimLock
// Completes ActivateSimLock request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSimLockTsy::CompleteActivateSimLock( 
    TInt aError )
    {
    if ( NULL != iTsyReqHandleStore->GetTsyReqHandle( 
         ESimLockRequestTypeActivateSimLock ) )
        {
        iMmCustomTsy->ReqCompleted( iTsyReqHandleStore->
            ResetTsyReqHandle( ESimLockRequestTypeActivateSimLock ), aError );
        }
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::DeActivateSimLockL
// This method deactivates SimLock.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmSimLockTsy::DeActivateSimLockL( 
    const TTsyReqHandle aTsyReqHandle, 
    RMmCustomAPI::TSimLockPassword* aPassword, 
    RMmCustomAPI::TLockNumber* aLockNumber )
    {
    TInt ret( KErrArgument );

    if ( NULL != iTsyReqHandleStore->GetTsyReqHandle(
         ESimLockRequestTypeDeActivateSimLock ) )
        {
        return KErrServerBusy;
        }
    
    switch ( *aLockNumber )
        {
        case RMmCustomAPI::EOperator:
        case RMmCustomAPI::EGid1:
        case RMmCustomAPI::EGid2:
        case RMmCustomAPI::EImsi:
        case RMmCustomAPI::EOperator_Gid1:
        case RMmCustomAPI::EOperator_Gid2:
        case RMmCustomAPI::EGlobalUnlock:
            iDeActivateSimLockNumber = aLockNumber;
            break;
        default:
            return KErrArgument;
        }
    
    iDeActivateSimLockPassword = aPassword;
    
    // Ask DOS to deactivate SimLock
    // packed parameters: RMmCustomAPI::TSimLockPassword and
    // RMmCustomAPI::TLockNumber
    CMmDataPackage dataPackage;
    dataPackage.PackData( aPassword, aLockNumber );
    ret = iMmCustomTsy->Phone()->MessageManager()->HandleRequestL( 
        ECustomSimLockDeActivateIPC, &dataPackage );

    if ( KErrNone == ret )
        {
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( ESimLockRequestTypeDeActivateSimLock,
            aTsyReqHandle );
#else
        iTsyReqHandleStore->SetTsyReqHandle(
            ESimLockRequestTypeDeActivateSimLock, aTsyReqHandle );
#endif // REQHANDLE_TIMER
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmSimLockTsy::CompleteDeActivateSimLock
// Completes DeActivateSimLock request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSimLockTsy::CompleteDeActivateSimLock( 
    TInt aError )
    {
    if ( NULL != iTsyReqHandleStore->GetTsyReqHandle( 
        ESimLockRequestTypeDeActivateSimLock ) )
        {
        iMmCustomTsy->ReqCompleted( iTsyReqHandleStore->
            ResetTsyReqHandle( ESimLockRequestTypeDeActivateSimLock ),
            aError );
        }
    }

#ifdef REQHANDLE_TIMER
// ---------------------------------------------------------------------------
// CMmSimLockTsy::SetTypeOfResponse
// Sets the type of response for a given handle. Automatic mode includes an 
// automatic response in case of non response from the DOS in a specified time.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSimLockTsy::SetTypeOfResponse( 
    const TInt aReqHandleType, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut( 0 );
    TInt ipc( 0 );

    switch ( aReqHandleType )
        {
        case ESimLockRequestTypeActivateSimLock:
            timeOut = KMmActivateSimLockTimeOut;
            ipc = ECustomSimLockActivateIPC;
            break;
        case ESimLockRequestTypeDeActivateSimLock:
            timeOut = KMmDeactivateSimLockTimeOut;
            ipc = ECustomSimLockDeActivateIPC;
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
// CMmSimLockTsy::Complete
// Completes the request due the timer expiration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmSimLockTsy::Complete( 
    TInt aReqHandleType, 
    TInt aError )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSIMLOCKTSY_COMPLETE_1,  "CMmCustomTsy: CMmSimLockTsy::Complete.\n\t ReqHandleType:%d \n\t Error:%{TSymbianErrorCodes}\n", aReqHandleType, aError );

    // All possible TSY req handle types are listed in the
    // switch case below.
    switch ( aReqHandleType )
        {
        case ESimLockRequestTypeActivateSimLock:
            CompleteActivateSimLock( aError );
            break;
        case ESimLockRequestTypeDeActivateSimLock:
            CompleteDeActivateSimLock( aError );
            break;
        default:
            iMmCustomTsy->ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle( 
                aReqHandleType ), aError );
            break;
        }
    }
#endif // REQHANDLE_TIMER
    
//  End of File 
