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
* Description:  An implementation class for responding to Mediator Service commands.
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    "MediatorCommandResponderBody.h"
#include    "MediatorServerClient.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

CMediatorCommandResponderBody::CMediatorCommandResponderBody( MMediatorCommandObserver* aObserver )
    : CActive( EPriorityStandard ),
      iObserver( aObserver ),
      iCategoryBuffer( iCategory ), 
      iCommandBuffer( iCommand ),
      iCommandDataPtr( NULL, 0 ),
      iDestroyed( NULL )
    {
    }

void CMediatorCommandResponderBody::ConstructL()
    {
    CActiveScheduler::Add( this );
    User::LeaveIfError( iMediatorServer.Connect() );
    ResetDataBufferL( KMaxCommandData );
    }

CMediatorCommandResponderBody* CMediatorCommandResponderBody::NewL(
                MMediatorCommandObserver* aObserver )
    {
    CMediatorCommandResponderBody* self = new( ELeave ) CMediatorCommandResponderBody( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
CMediatorCommandResponderBody::~CMediatorCommandResponderBody()
    {
    Cancel();
    iMediatorServer.Close();
    delete iCommandData;
    
    if ( iDestroyed ) // RunL is being executed
        {
        *iDestroyed = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::DoCancel
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorCommandResponderBody::DoCancel()
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorCommandResponderBody::DoCancel\n")));
    iMediatorServer.Cancel();    
    }



// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::RunL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorCommandResponderBody::RunL()
    {
    TRACE(Print(_L("[Mediator Server]\t CMediatorCommandResponderBody::RunL status %d\n"), iStatus.Int() ));
    
    if ( iObserver )
        {
        
        // Set the flag to the member variable that is updated by the destructor 
        // in case this instance is destroyed by observer callback.
        // Otherwise an attempt to manipulate member data after destruction will cause a panic.
        TBool destroyed = EFalse;
                
        // Check if there is an error --> Cancel command
        if ( iStatus < 0 )
            {
            ERROR_TRACE(Print(_L("[Mediator] CMediatorCommandResponderBody::RunL: iStatus=%d\n"), iStatus.Int() ) );
            iDestroyed = &destroyed;
            iObserver->CancelMediatorCommand( iCategory.iDomain,
                                              iCategory.iCategory, 
                                              iCommand.iCommandId );
            }
        else    // Everything should be ok
            {
            // Check the parameter data size. If bigger than expected, fetch it synchronously
            TInt dataSize = iStatus.Int();
            if ( dataSize > iCommandDataPtr.MaxLength() )
                {
                // Reset data buffer for bigger size
                ResetDataBufferL( dataSize );
                
                // Fetch data from Mediator
                iMediatorServer.FetchParameterData( iCommandDataPtr );
                }
            
            iDestroyed = &destroyed; // can't set earlier because leaving functions are called
                
            // Issue callback to client. 
            // For some reason error handling can't be done in RunError because server stops forwarding commands
            TRAPD( err, iObserver->MediatorCommandL( iCategory.iDomain,
                                         iCategory.iCategory, 
                                         iCommand.iCommandId,
                                         iCommand.iVersion, 
                                         *iCommandData ) );
            
            if ( err != KErrNone ) // Errors only are propagated back to command initiator
                {
                
                ERROR_TRACE(Print(_L("[Mediator] CMediatorCommandResponderBody::RunL: err=%d\n"), err ) );
                
                if ( !destroyed )
                    {
                    iMediatorServer.IssueResponse ( iCategory.iDomain,
                                                    iCategory.iCategory,
                                                    iCommand.iCommandId,
                                                    err,
                                                    KNullDesC8 );    
                    }
                }
            }
        
        if ( destroyed ) // instance does not exist any longer, don't proceed to command receiving
            {
            return;
            }
        
        iDestroyed = NULL; // set to NULL, because variable goes out of scope soon            
        
        }
    
    
    
    // Continue command receiving
    // Wait for next commands                                      
    StartCommandReceiving();           
    
    }
	
// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::RunError
// This will be called when there is a leave from ResetDataBufferL()
// 
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CMediatorCommandResponderBody::RunError( TInt aError )
#else
TInt CMediatorCommandResponderBody::RunError( TInt /*aError*/ )
#endif //_DEBUG
	{
	ERROR_TRACE(Print(_L("[Mediator Server]\t CMediatorCommandResponderBody::RunError called with Error %d\n"), aError )); 
	//Ignore the error
	return KErrNone;
	}
    
// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::RegisterCommandL
// Register a command category.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMediatorCommandResponderBody::RegisterCommand( 
                                        TUid aDomain,
                                        TUid aCategory, 
                                        const RCommandList& aCommands )
    {
    LOG(_L("[Mediator Server]\t CMediatorCommandResponderBody::RegisterCommand list"));
    // Send the registration
    TInt status = iMediatorServer.RegisterCommandList( aDomain, aCategory, aCommands );
    
    if ( status == KErrNone )
        {
        // Wait for incoming commands
        StartCommandReceiving();                        
        }
    else
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorCommandResponderBody::RegisterCommand: status=%d\n"), status ) );
        }
                
    return status;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::RegisterCommandL
// Register a command.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
TInt CMediatorCommandResponderBody::RegisterCommand( TUid aDomain, 
                                                     TUid aCategory, 
                                                     TInt aCommandId, 
                                                     TVersion aVersion,
                                                     TCapabilitySet aCaps, 
                                                     TInt aTimeOut )
    {
    LOG(_L("[Mediator Server]\t CMediatorCommandResponderBody::RegisterCommand"));
    
    // Create a list to contain one command
    RCommandList commandList;
    TCommand newCommand;
    newCommand.iCommandId   = aCommandId;
    newCommand.iVersion     = aVersion;
    newCommand.iCaps        = aCaps;
    newCommand.iTimeout     = aTimeOut;
    TInt error = commandList.Append( newCommand );
        
    if ( error == KErrNone )
        {
        // Register command
        error = iMediatorServer.RegisterCommandList( aDomain,
                                                  aCategory,
                                                  commandList );
        if ( error == KErrNone ) 
            {
            // Wait for incoming commands
            StartCommandReceiving();    
            }
        else
            {
            ERROR_TRACE(Print(_L("[Mediator] CMediatorCommandResponderBody::RegisterCommand (ln%d): error=%d\n"), __LINE__, error ) );
            }
        }
    else
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorCommandResponderBody::RegisterCommand (ln%d): error=%d\n"), __LINE__, error ) );    
        }        
    
    
            
    commandList.Reset();
    commandList.Close();
    
    return error;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::UnregisterCommand
// Unregister a command list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
TInt CMediatorCommandResponderBody::UnregisterCommand( 
                                            TUid aDomain, 
                                            TUid aCategory,
                                            const RCommandList& aCommands )
    {
    LOG(_L("[Mediator Server]\t CMediatorCommandResponderBody::UnregisterCommand list"));
    
    return iMediatorServer.UnregisterCommandList( aDomain, aCategory, aCommands );
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::UnregisterCommand
// Unregister a command.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
TInt CMediatorCommandResponderBody::UnregisterCommand( 
                                         TUid aDomain, 
                                         TUid aCategory, 
                                         TInt aCommandId )
    {
    LOG(_L("[Mediator Server]\t CMediatorCommandResponderBody::UnregisterCommand"));
        
    // Create a list to contain one command
    RCommandList commandList;
    TCommand command;
    command.iCommandId   = aCommandId;
    TInt error = commandList.Append( command );
        
    if ( !error )
        {
        // Unregister command
        error = iMediatorServer.UnregisterCommandList( aDomain,
                                                       aCategory,
                                                       commandList );
        }
    else
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorCommandResponderBody::UnregisterCommand: error=%d\n"), error ) );
        }
                
    commandList.Reset();
    commandList.Close();
    
    return error;
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::IssueResponseL
// Issue a response to a command.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
TInt CMediatorCommandResponderBody::IssueResponse( TUid aDomain,
                                                   TUid aCategory, 
                                                   TInt aCommandId,
                                                   TInt aStatus, 
                                                   const TDesC8& aData )
    {
    LOG(_L("[Mediator Server]\t CMediatorCommandResponderBody::IssueResponse"));
    return iMediatorServer.IssueResponse( aDomain,
                                          aCategory,
                                          aCommandId,
                                          aStatus,
                                          aData );
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::StartCommandReceiving
// Starts to receive commands asynchronously
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMediatorCommandResponderBody::StartCommandReceiving()
    {
    if ( !IsActive() )
        {
        iMediatorServer.ReceiveCommands( iStatus,
                                         iCategoryBuffer,
                                         iCommandBuffer,
                                         iCommandDataPtr );
        SetActive();   
        }         
    }

// -----------------------------------------------------------------------------
// CMediatorCommandResponderBody::ResetDataBufferL
// Starts to receive commands asynchronously
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMediatorCommandResponderBody::ResetDataBufferL( TInt aSize )
    {
    if ( iCommandData )
        {
        delete iCommandData;
        iCommandData = NULL;
        }
    iCommandData = HBufC8::NewL( aSize );
    iCommandDataPtr.Set( iCommandData->Des() );
       
    }

//  End of File  
