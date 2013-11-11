/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  An implementation for receiving command/event notifications and info.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorNotificationsBody.h"
#include    "MediatorServerClient.h"
#include    "Debug.h"


// CONSTANTS
const TInt KMediatorDefaultCommandCount = 10;
const TInt KMediatorDefaultEventCount = 10;

// ============================ MEMBER FUNCTIONS ===============================

CMediatorNotificationsBody::CMediatorNotificationsBody()
    : CActive( EPriorityNormal ), 
      iCommandArrayPtr(NULL, 0), 
      iEventArrayPtr(NULL, 0),
      iCategoryBuffer( iCategory ),
      iNotificationTypeBuffer( iNotificationType ),
      iDestroyed(NULL)
     
    {
    }

void CMediatorNotificationsBody::ConstructL()
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::ConstructL\n"));
    CActiveScheduler::Add( this );
    User::LeaveIfError( iMediatorServer.Connect() );
    }

CMediatorNotificationsBody* CMediatorNotificationsBody::NewL()
    {
    CMediatorNotificationsBody* self = new( ELeave ) CMediatorNotificationsBody;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorNotificationsBody::~CMediatorNotificationsBody()
    {
    Cancel();
    iMediatorServer.Close();
    iEventList.Close();
    iCommandList.Close();
    
    if ( iDestroyed ) // RunL is being executed
        {
        *iDestroyed = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::RunL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorNotificationsBody::RunL()
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorNotificationsBody::RunL status %d\n"), iStatus.Int() ));
    
    if ( iStatus < 0 )  // We have an error
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorNotificationsBody::RunL: iStatus=%d\n"), iStatus.Int() ) );
        
        // Cleanup and return
        iCommandList.Reset();
        iEventList.Reset();
        return;
        }
    
    // Set the flag to the member variable that is updated by the destructor 
    // in case this instance is destroyed by observer callback.
    // Otherwise an attempt to manipulate member data after destruction will cause a panic.
    TBool destroyed = EFalse;
    // iDestroyed is set individually in each case in order to prevent erros if a leave occurs
    
    TInt err = KErrNone;
    
    // Act according to notification type. NOTE! All the observer callback-calls are trapped because
    // leave would cause a possible access violation in destructor (iDestoryed pointing to an non-existing variable)
    switch( iNotificationType )
        {
        case EMediatorEventsRegistered:
            {
            LOG(_L("[Mediator Server]\t Events registered\n"));
            if ( iObserver )
                {
                TBool fetchEvents = iStatus.Int() > iEventList.Count() ? ETrue : EFalse;
                
                ResizeEventListL( iStatus.Int() );
                
                if ( fetchEvents )
                    {
                    FetchEventListL();
                    }
                
                iDestroyed = &destroyed;
                TRAP( err, iObserver->MediatorEventsAddedL( iCategory.iDomain,
                                                            iCategory.iCategory,
                                                            iEventList ) );
                }
            break;
            }
        case EMediatorCommandsRegistered:
            {
            LOG(_L("[Mediator Server]\t Commands registered\n"));
            if ( iObserver )
                {
                TBool fetchCommands = iStatus.Int() > iCommandList.Count() ? ETrue : EFalse;
                
                ResizeCommandListL( iStatus.Int() );
                
                if ( fetchCommands )
                    {
                    FetchCommandListL();
                    }
                
                iDestroyed = &destroyed;    
                TRAP( err, iObserver->MediatorCommandsAddedL( iCategory.iDomain,
                                                              iCategory.iCategory,
                                                              iCommandList ) );
                }
            break;
            }
        case EMediatorEventsUnregistered:
            {
            LOG(_L("[Mediator Server]\t Events unregistered\n"));
            if ( iObserver )
                {
                TBool fetchEvents = iStatus.Int() > iEventList.Count() ? ETrue : EFalse;
                
                ResizeEventListL( iStatus.Int() );
                
                if ( fetchEvents )
                    {
                    FetchEventListL();
                    }
                
                iDestroyed = &destroyed;    
                TRAP( err, iObserver->MediatorEventsRemovedL( iCategory.iDomain,
                                                              iCategory.iCategory,
                                                              iEventList ) );
                }
            break;
            }
        case EMediatorCommandsUnregistered:
            {
            LOG(_L("[Mediator Server]\t Commands unregistered\n"));
            if ( iObserver )
                {
                TBool fetchCommands = iStatus.Int() > iCommandList.Count() ? ETrue : EFalse;
                
                ResizeCommandListL( iStatus.Int() );
                
                if ( fetchCommands )
                    {
                    FetchCommandListL();
                    }
                
                iDestroyed = &destroyed;    
                TRAP( err, iObserver->MediatorCommandsRemovedL( iCategory.iDomain,
                                                                iCategory.iCategory,
                                                                iCommandList ) );
                }
            break;
            }
        case EMediatorCategoryUnregistered:
            {
            
            LOG(_L("[Mediator Server]\t Category unregistered\n"));
            if ( iObserver )
                {
                iDestroyed = &destroyed;
                TRAP( err, iObserver->MediatorCategoryRemovedL( iCategory.iDomain,
                                                                iCategory.iCategory ) );
                }
            break;
            }
        default:
            {
            LOG(_L("[Mediator Server]\t Unknown notification\n"));
            break;
            }
        }
    
    
    if ( err != KErrNone )
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorNotificationsBody::RunL: err=%d, iNotificationType=%d\n"), err,
                                                                                                             iNotificationType ) );
        if ( !destroyed )
            {
            iDestroyed = NULL;
            }
            
        User::Leave( err ); // leave now, if an observer callback caused an error
        }
    
    if ( !destroyed ) // client may delete instance in observer callback
        {
        iDestroyed = NULL; // set to NULL, because local variable goes out of scope soon
        // Continue receiving notifications
        ReceiveNotificationsL();    
        }
    
    }
        
// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::DoCancel
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorNotificationsBody::DoCancel()
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::DoCancel\n"));  
    iMediatorServer.Cancel(); 
    }

// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::RegisterNotificationObserver
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorNotificationsBody::RegisterNotificationObserver( 
            MMediatorNotifications* aObserver )
    {
    iObserver = aObserver;
    //return ReceiveNotifications();
    
    TRAPD ( err, ReceiveNotificationsL() );
    
    return err;
    }
            
// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::UnregisterNotificationObserver
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorNotificationsBody::UnregisterNotificationObserver()
    {
    iObserver = NULL;
    // Empty existing arrays
    if ( iCommandList.Count() > 0 )
        {
        iCommandList.Reset();
        }
    if ( iEventList.Count() > 0 )
        {
        iEventList.Reset();
        }
    // Cancel the show
    return iMediatorServer.CancelNotifications();
    }

// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::GetDomains
// Gets a list of registered domains
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorNotificationsBody::GetDomains( RDomainList& aDomains )
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::GetDomains\n"));
    return iMediatorServer.GetDomains( aDomains );
    }
    

// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::GetCategories
// Gets a list of registered domains
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorNotificationsBody::GetCategories( TUid aDomain, 
                                                RCategoryList& aCategories )
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::GetCategories\n"));
    return iMediatorServer.GetCategories( aDomain, aCategories );
    }

// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::GetEvents
// Gets a list of registered events withing the domain/category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorNotificationsBody::GetEvents( TUid aDomain, 
                                            TUid aCategory,
                                            REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::GetEvents\n"));
    return iMediatorServer.GetEvents( aDomain, aCategory, aEvents );
    }

// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::GetCommands
// Gets a list of registered commands withing the domain/category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorNotificationsBody::GetCommands( TUid aDomain, 
                                              TUid aCategory,
                                              RCommandList& aCommands )
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::GetCommands\n"));
    return iMediatorServer.GetCommands( aDomain, aCategory, aCommands );
    }


// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::ReceiveNotificationsL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CMediatorNotificationsBody::ReceiveNotificationsL()
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::ReceiveNotificationsL\n"));
    if ( IsActive() )
        {
        ERROR_LOG(_L("[Mediator] CMediatorNotificationsBody::ReceiveNotificationsL: User::Leave( KErrAlreadyExists )\n") );
        User::Leave( KErrAlreadyExists );
        }
        
    ResizeEventListL( KMediatorDefaultEventCount );
    
    ResizeCommandListL ( KMediatorDefaultCommandCount );
    
    // Define data return pointers
    TInt commandArraySize = ( sizeof(MediatorService::TCommand) ) * KMediatorDefaultCommandCount; 
    TPtr8 commandArrayPtr( (TUint8*)&iCommandList[0], commandArraySize );
    iCommandArrayPtr.Set( commandArrayPtr );
    
    TInt eventArraySize = ( sizeof(MediatorService::TEvent) ) * KMediatorDefaultEventCount; 
    TPtr8 eventArrayPtr( (TUint8*)&iEventList[0], eventArraySize );
    iEventArrayPtr.Set( eventArrayPtr );
    
    SetActive();
    
    iMediatorServer.ReceiveNotifications( iStatus,
                                          iCategoryBuffer, 
                                          iNotificationTypeBuffer,
                                          iEventArrayPtr,
                                          iCommandArrayPtr );
    }

// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::ResizeEventListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorNotificationsBody::ResizeEventListL( TInt aEventCount )
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::ResizeEventListL\n"));
    if ( iEventList.Count() > aEventCount ) // less events has been received than space allocated
        {
        // suppress list
        for ( TInt i = iEventList.Count() - 1; i >= aEventCount; i-- )
            {
            iEventList.Remove( i );
            }
        }
    else if ( iEventList.Count() < aEventCount )  // more events than initially/previously allocated
        {
        // reallocate event list
        iEventList.Reset();
        
        User::LeaveIfError( iEventList.Reserve( aEventCount ) );
        
        for ( TInt i = 0; i < aEventCount; i++ )
            {
            TEvent emptyEvent;
            iEventList.Append( emptyEvent );
            }    
        }
    }

// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::ResizeCommandListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorNotificationsBody::ResizeCommandListL( TInt aCommandCount )
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::ResizeCommandListL\n"));
    if ( iCommandList.Count() > aCommandCount ) // less commands has been received than space allocated
        {
        // suppress list
        for ( TInt i = iCommandList.Count() - 1; i >= aCommandCount; i-- )
            {
            iCommandList.Remove( i );
            }
        }
    else if ( iCommandList.Count() < aCommandCount )  // more commands than initially/previously allocated
        {
        // reallocate command list
        iCommandList.Reset();
        
        User::LeaveIfError( iCommandList.Reserve( aCommandCount) );
        
        for ( TInt i = 0; i < aCommandCount; i++ )
            {
            TCommand emptyCommand;
            iCommandList.Append( emptyCommand );
            }    
        }
    }

// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::FetchEventListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorNotificationsBody::FetchEventListL()
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::FetchEventListL\n"));
    TInt eventArraySize = ( sizeof(MediatorService::TEvent) ) * iEventList.Count(); 
    TPtr8 eventArrayPtr( (TUint8*)&iEventList[0], eventArraySize );
    iEventArrayPtr.Set( eventArrayPtr );
    User::LeaveIfError( iMediatorServer.FetchNotificationEventList( iEventArrayPtr ) );
    }
    
// -----------------------------------------------------------------------------
// CMediatorNotificationsBody::FetchCommandListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorNotificationsBody::FetchCommandListL()
    {
    LOG(_L("[Mediator Server]\t CMediatorNotificationsBody::FetchCommandListL\n"));
    TInt commandArraySize = ( sizeof(MediatorService::TCommand) ) * iCommandList.Count(); 
    TPtr8 commandArrayPtr( (TUint8*)&iCommandList[0], commandArraySize );
    iCommandArrayPtr.Set( commandArrayPtr );
    User::LeaveIfError( iMediatorServer.FetchNotificationCommandList( commandArrayPtr ) );
    }

// ----------------------------------------------------------------------------
// CStartupMediatorPluginSubscriber::RunError()
// ----------------------------------------------------------------------------
#ifdef _DEBUG
TInt CMediatorNotificationsBody::RunError( TInt aError )
#else
TInt CMediatorNotificationsBody::RunError( TInt /*aError*/ )
#endif //_DEBUG
    {
    ERROR_TRACE(Print(_L("[Mediator] CMediatorNotificationsBody::RunError: err=%d\n"), aError));

    // Return KErrNone to avoid panic
    return KErrNone;
    }

//  End of File  
