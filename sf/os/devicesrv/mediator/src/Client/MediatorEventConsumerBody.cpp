/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  An implementation class for receiving Mediator events.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorEventConsumerBody.h"
#include    "MediatorServerClient.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

CMediatorEventConsumerBody::CMediatorEventConsumerBody( MMediatorEventObserver* aObserver )
    : CActive( EPriorityNormal ),
      iObserver( aObserver ), 
      iCategoryBuffer( iCategory ),
      iEventBuffer( iEvent ),
      iEventDataPtr( NULL, 0 ),
      iDestroyed ( NULL )
    {
    }

void CMediatorEventConsumerBody::ConstructL()
    {
    CActiveScheduler::Add( this );
    User::LeaveIfError( iMediatorServer.Connect() );
    ResetDataBufferL( KMaxEventData );  
    }

CMediatorEventConsumerBody* CMediatorEventConsumerBody::NewL(
                        MMediatorEventObserver* aObserver )
    {
    CMediatorEventConsumerBody* self = new( ELeave ) CMediatorEventConsumerBody( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorEventConsumerBody::~CMediatorEventConsumerBody()
    {
    Cancel();
    iMediatorServer.Close();
    delete iEventData;
    
    if ( iDestroyed ) // RunL is being executed
        {
        *iDestroyed = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumerBody::RunL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorEventConsumerBody::RunL()
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorEventConsumerBody::RunL status %d\n"), iStatus.Int() ));
    
    if ( iObserver && ( iStatus >= KErrNone ) )
        {
        // Check the parameter data size. If bigger than expected, fetch it synchronously
        TInt dataSize = iStatus.Int();
        if ( dataSize > iEventDataPtr.MaxLength() )
            {
            // Reset data buffer for bigger size
            ResetDataBufferL( dataSize );
            
            // Fetch data from Mediator
            iMediatorServer.FetchParameterData( iEventDataPtr );
            }
        
        // Set the flag to the member variable that is updated by the destructor 
        // in case this instance is destroyed by observer callback.
        // Otherwise an attempt to manipulate member data after destruction will cause a panic.
        TBool destroyed = EFalse;
        iDestroyed = &destroyed;
        
        // must be trapped, so that leave won't cause access violation (iDestroyed pointing to a non-existing variable)
        TRAPD( err, iObserver->MediatorEventL( iCategory.iDomain,
                                               iCategory.iCategory, 
                                               iEvent.iEventId, 
                                               *iEventData ) );
        
        if ( err != KErrNone )
            {
            ERROR_TRACE(Print(_L("[Mediator] CMediatorEventConsumerBody::RunL: err=%d\n"), err ) );
            if ( !destroyed )
                {
                iDestroyed = NULL;
                }
            User::Leave( err );
            }
        
        if ( destroyed )
            {
            return; // instance destroyed in MediatorEventL, don't proceed to event receiving
            }
        
        iDestroyed = NULL; // set member variable to NULL, because local variable goes out of scope            
        }
    StartEventReceiving();
    }
 
// -----------------------------------------------------------------------------
// CMediatorEventConsumerBody::RunError
// This will be called when there is a leave from ResetDataBufferL() and MediatorEventL() of RunL()
// 
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CMediatorEventConsumerBody::RunError( TInt aError )
#else
TInt CMediatorEventConsumerBody::RunError( TInt /*aError*/ )
#endif //_DEBUG
	{
	ERROR_TRACE(Print(_L("[Mediator Server]\t CMediatorEventConsumerBody::RunError called with Error %d\n"), aError ));
	//Ignore the error
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CMediatorEventConsumerBody::DoCancel
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorEventConsumerBody::DoCancel()
    {
    LOG(_L("[Mediator Server]\t CMediatorEventConsumerBody::DoCancel\n")); 
    iMediatorServer.Cancel();    
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumerBody::SubscribeEventL
// Subscribe to event category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorEventConsumerBody::SubscribeEvent( TUid aDomain,
                                                      TUid aCategory,
                                                      const REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t CMediatorEventConsumerBody::SubscribeEvent\n"));
    // Start to receive
    StartEventReceiving();
    // Subscribe
    return iMediatorServer.SubscribeEventList( aDomain, 
                                               aCategory,
                                               aEvents );
    }
    
// -----------------------------------------------------------------------------
// CMediatorEventConsumerBody::SubscribeEventL
// Subscribe to event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorEventConsumerBody::SubscribeEvent( TUid aDomain,
                                                      TUid aCategory, 
                                                      TInt aEventId,
                                                      TVersion aVersion )
    {
    LOG(_L("[Mediator Server]\t CMediatorEventConsumerBody::SubscribeEvent\n"));
    
    __UHEAP_MARK;
        
    // Create a list to contain one event
    REventList eventList;
    TEvent event;
    event.iEventId = aEventId;
    event.iVersion = aVersion;
    TInt error = eventList.Append( event );
        
    if ( !error )
        {
        // Start to receive
        StartEventReceiving();
        // Subscribe eventlist
        error = iMediatorServer.SubscribeEventList( aDomain,
                                                    aCategory,
                                                    eventList );
        }
    else
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorEventConsumerBody::SubscribeEvent: error=%d\n"), error ) );
        }
        
    // Clean up                                        
    eventList.Reset();
    
    __UHEAP_MARKEND;
    return error;
    }
    
// -----------------------------------------------------------------------------
// CMediatorEventConsumerBody::UnsubscribeEvent
// Unsubscibe an event category.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorEventConsumerBody::UnsubscribeEvent( TUid aDomain,
                                                        TUid aCategory,
                                                        const REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t CMediatorEventConsumerBody::UnsubscribeEvent list\n"));
    return iMediatorServer.UnsubscribeEventList( aDomain, aCategory, aEvents );
    }
    
// -----------------------------------------------------------------------------
// CMediatorEventConsumerBody::UnsubscribeEvent
// Unsubscibe an event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorEventConsumerBody::UnsubscribeEvent( TUid aDomain,
                                                        TUid aCategory, 
                                                        TInt aEventId )
    {
    LOG(_L("[Mediator Server]\t CMediatorEventConsumerBody::UnsubscribeEvent\n"));
    __UHEAP_MARK;
    
    // Create a list to contain one event
    REventList eventList;
    TEvent event;
    event.iEventId = aEventId;
    TInt error = eventList.Append( event );
        
    if ( !error )
        {
        // Unsubscribe eventlist
        error = iMediatorServer.UnsubscribeEventList( aDomain,
                                                      aCategory,
                                                      eventList );
        }
    else
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorEventConsumerBody::UnsubscribeEvent: error=%d\n"), error ) );
        }        
        
    // Clean up                                        
    eventList.Reset();
    
    __UHEAP_MARKEND;
    return error;
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumerBody::StartEventReceiving
// Starts to wait for events
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMediatorEventConsumerBody::StartEventReceiving()
    {
	if ( !IsActive() )
	    {
	    iMediatorServer.ReceiveEvents( iStatus, 
	                                   iCategoryBuffer, 
	                                   iEventBuffer, 
	                                   iEventDataPtr );
        SetActive();	                                   
	    }
    }

// -----------------------------------------------------------------------------
// CMediatorEventConsumerBody::ResetDataBufferL
// Starts to receive commands asynchronously
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMediatorEventConsumerBody::ResetDataBufferL( TInt aSize )
    {
    if ( iEventData )
        {
        delete iEventData;
        iEventData = NULL;
        }
    iEventData = HBufC8::NewL( aSize );
    iEventDataPtr.Set( iEventData->Des() );
       
    }
//  End of File  
