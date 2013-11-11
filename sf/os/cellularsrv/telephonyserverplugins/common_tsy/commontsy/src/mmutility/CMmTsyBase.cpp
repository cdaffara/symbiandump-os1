// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CMmTsyBase.h"

// ======== MEMBER FUNCTIONS ========

CMmTsyBase::~CMmTsyBase()
    {
    }

CMmTsyBase::CMmTsyBase()
    {
    }

// ---------------------------------------------------------------------------
// CMmTsyBase::Init
// Initialisation method that is called from ETel Server.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmTsyBase::Init()
    {
    TInt iMax = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();

    for ( TInt i = 0; iMax > i; i++ )
        {
        if( NULL != subTsyPtr[i] )
            {
            subTsyPtr[i]->Init();
            }
        }
    return;
    }

// ---------------------------------------------------------------------------
// CMmTsyBase::Init
// RegisterNotification is called when the server recognises
// that this notification is being posted for the first time on this sub-
// session object. It enables the TSY to "turn on" any regular notification
// messages that it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmTsyBase::RegisterNotification(    
    const TInt aIpc )  
    {
    TInt ret = KErrNotSupported;
    TInt iMax = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();

    for ( TInt i = 0; iMax > i; i++ )
        {
        if ( ( NULL != subTsyPtr[i] ) 
             && ( EFalse != subTsyPtr[i]->SupportingIPC( aIpc ) ) )
            {
            ret = subTsyPtr[i]->RegisterNotification( aIpc );
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmTsyBase::OpenNewObjectByNameL
// Creates new CTelObject object and returns a pointer to it. Not supported
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmTsyBase::OpenNewObjectByNameL(    
    const TDesC&  //aName   
        )
    {
    //Not supported
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmTsyBase::OpenNewObjectL
// Opens a new Object by newname (not supported by CUSTOM)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmTsyBase::OpenNewObjectL( 
    TDes& /*aNewName*/ )
    {
    User::Leave(KErrNotSupported);
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmTsyBase::DeregisterNotification
// DeregisterNotification is called when the server recognises that this
// notification will not be posted again because the last client to have
// a handle on this sub-session object has just closed the handle. It
// enables the TSY to "turn off" any regular notification messages that
// it may receive from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmTsyBase::DeregisterNotification(    
    const TInt aIpc ) 
    {
    TInt ret = KErrNotSupported;
    TInt iMax = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();

    for ( TInt i = 0; iMax > i; i++ )
        {
        if ( ( NULL != subTsyPtr[i] ) 
             && ( EFalse != subTsyPtr[i]->SupportingIPC( aIpc ) ) )
            {
            ret = subTsyPtr[i]->DeregisterNotification( aIpc );
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmTsyBase::DoBaseExtFuncL
// Dispatches extension function requests. At the beginning of this method,
// packaged pointers are unpacked. Then the correct function is selected
// using the IPC number.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmTsyBase::DoBaseExtFuncL(
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage ) 
    {
    TInt ret = KErrNotSupported;
    TInt iMax = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();

    for ( TInt i = 0; iMax > i; i++ )
        {
        if ( ( NULL != subTsyPtr[i] ) 
             && ( EFalse != subTsyPtr[i]->SupportingIPC( aIpc ) ) )
            {
            ret = subTsyPtr[i]->DoExtFuncL( aTsyReqHandle, aIpc, aPackage );
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmTsyBase::ReqModeL
// Returns request mode for given IPC. If the IPC number belongs to some of
// the Custom API functions.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmTsyBase::ReqModeL(
    const TInt aIpc ) 
    {
    CTelObject::TReqMode ret=0;
    TInt i = 0;
    TInt iMax = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();

    for ( i = 0; iMax > i; i++ )
        {
        if ( ( NULL != subTsyPtr[i] ) 
             && ( EFalse != subTsyPtr[i]->SupportingIPC( aIpc ) ) )
            {
            ret = subTsyPtr[i]->ReqModeL( aIpc );
            break;
            }
        }
    if ( i >= iMax )
        {
        User::Leave(KErrNotSupported);
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmTsyBase::NumberOfSlotsL
// Returns number of slots to be used for given IPC.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmTsyBase::NumberOfSlotsL(
    const TInt aIpc ) 
    {
    TInt numberOfSlots = 0;
    TInt i = 0;
    TInt iMax = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();

    for ( i = 0; iMax > i; i++ )
        {
        if  ( ( NULL != subTsyPtr[i] ) 
              && ( EFalse != subTsyPtr[i]->SupportingIPC( aIpc ) ) )
            {
            numberOfSlots = subTsyPtr[i]->NumberOfSlotsL( aIpc );
            break;
            }
        }
    if ( i >= iMax )
        {
        User::Leave(KErrNotSupported);
        }
    return numberOfSlots;
    }
    
// ---------------------------------------------------------------------------
// CMmTsyBase::CancelService
// When the clients close their subsessions (eg. by calling RPhone::Close),
// they may not have cancelled all their outstanding asynchronous requests
// before closing. It is up to the ETel server to clean up in this situation,
// so the server will find the list of outstanding requests related to that
// sub-session object and pass these outstanding IPC request numbers, one at
// a time, to the CancelService function in the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmTsyBase::CancelService(
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle )  
    {
    TInt ret = KErrNone;
    TInt i = 0;
    TInt iMax = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();

    for ( i = 0; iMax > i; i++ )
        {
        if ( ( NULL != subTsyPtr[i] ) 
             && ( EFalse != subTsyPtr[i]->SupportingIPC( aIpc ) ) )
            {
            ret = subTsyPtr[i]->CancelService( aIpc, aTsyReqHandle );
            break;
            }
        }
    if ( i >= iMax )
        {
        ret = KErrGeneral;
        }
    return ret;
    }

#ifdef REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmTsyBase::Complete
// Completes the request due timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmTsyBase::Complete(
    TInt aReqHandleType, 
    TInt aError, 
    TInt aIPC ) 
    {
    TInt max = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();

    for ( TInt i = 0; max > i; i++ )
        {
        //check which custom object handles this IPC
        if ( ( NULL != subTsyPtr[i] ) 
               && ( EFalse != subTsyPtr[i]->SupportingIPC( aIPC ) ) )
            {
            //call complete method of the right custom object
            subTsyPtr[i]->Complete( aReqHandleType, aError );
            i = max;
            }
        }
    }

#endif

// ---------------------------------------------------------------------------
// CMmTsyBase::ResetReqHandle
// Resets the request handle in the right req handle store
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmTsyBase::ResetReqHandle(const TTsyReqHandle aTsyReqHandle, const TInt aIpc)
    {
    TInt max = GetMaxNumberOfSubsystems();
    CMmSubTsyBase** subTsyPtr = GetSubsystemArrayPtr();
    TBool ret = EFalse;
    
    for ( TInt i = 0 ; max > i; i++ )
        {
         //check which custom object handles this IPC
        if ( ( NULL != subTsyPtr[i] ) 
        && ( EFalse != subTsyPtr[i]->SupportingIPC( aIpc ) ) )
               {
               //call reset handle method of the right custom object
               ret = subTsyPtr[i]->ResetReqHandle( aTsyReqHandle, aIpc );
               i = max;
               }
        }
    return ret;
    }


//  End of File

