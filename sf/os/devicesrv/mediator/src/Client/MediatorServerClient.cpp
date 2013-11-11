/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client API implementation for the Mediator Server
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <s32mem.h>
#include "MediatorServerClient.h"
#include "MediatorCommon.h"
#include "Debug.h"


// CONSTANTS
const TInt KServerRetries = 2;


// ============================ MEMBER FUNCTIONS ===============================

RMediatorServer::RMediatorServer()
	{
	}

// -----------------------------------------------------------------------------
// RMediatorServer::Connect
// Creates a new client-server -session with MediatorServer. Starts it up if not
// started already.
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::Connect()
	{
	// Check that there's no connection existing already
	if ( Handle() )
	    {
	    return KErrAlreadyExists;
	    }
	
    TInt tryLoop = 0;
    TInt error = 0;
    // Try to create session to server. If server is not started, try to start it.
    for ( tryLoop = 0; tryLoop < KServerRetries; tryLoop++ )
        {
        error = CreateSession( KMediatorServerName, Version() );   
        
        TRACE(Print(_L("[Mediator Server]\t Session created with status %d\n"), error));

        if ( error != KErrNotFound &&  error != KErrServerTerminated )
            {
            if ( error != KErrNone )
                {
                ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::Connect(ln%d): error=%d\n"), __LINE__, error ) );    
                }
            
            return error;
            }

        error = StartServer();
        
        if ( error != KErrNone && error != KErrAlreadyExists )
            {
            ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::Connect(ln%d): error=%d\n"), __LINE__, error ) );
            return error;
            }
            
        }
    return error;
	}
	
// -----------------------------------------------------------------------------
// StartServer
// Creates a new process for the server and starts it up.
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::StartServer()
    {
	TInt result( KErrNone );
	// create server - if one of this name does not already exist
	TFindProcess findServerProcess( KMediatorServerProcessName );
	TFullName name;
	if ( findServerProcess.Next( name ) != KErrNone ) // we don't exist already
		{
		TRequestStatus status( KRequestPending );
		RProcess server;
		// Create the server process
		result = server.Create( KMediatorServerExe, KNullDesC );		
		if( result != KErrNone )
			{
			ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::StartServer: result=%d\n"), result ) );
			return result;
			}

		// Process created successfully
		server.Rendezvous( status );
		server.Resume(); // start it going
		
		// Wait until the completion of the server creation
		// server signals us when it's up
		User::WaitForRequest( status );
		if( status != KErrNone )
			{
            ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::StartServer: status=%d\n"), status.Int() ) );
			server.Close();
			return status.Int();
			}
			
		// Server created successfully
		server.Close(); // we can close the handle to server process now
		}
    return result;
    }
	

// -----------------------------------------------------------------------------
// RMediatorServer::Close
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorServer::Close()
	{
	// Base call
	RSessionBase::Close(); 
	}

// -----------------------------------------------------------------------------
// RMediatorServer::Version
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TVersion RMediatorServer::Version(void) const
	{
	return( TVersion( KMediatorServerMajor, 
	                  KMediatorServerMinor, 
	                  KMediatorServerBuild ));
	}

// -----------------------------------------------------------------------------
// RMediatorServer::Cancel
// Cancels ongoing command in Mediator
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorServer::Cancel( )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::Cancel\n")); 
    
    //  Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
                                         
    // Send the command (ignore return value)                                
    SendReceive( ECancelAll, TIpcArgs() );
    }

// ============================ REGISTRATION COMMANDS ==========================

// -----------------------------------------------------------------------------
// RMediatorServer::RegisterEventL
// Registers event list to MediatorServer 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::RegisterEventList( TUid aDomain,  
                                         TUid aCategory,
                                         const REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::RegisterEventL list\n")); 
    
    //  Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    TInt status = KErrNone;
    
    // Check that there's something to register                                           
    if ( aEvents.Count() > 0 )
        { 
        // Fill category
        TMediatorCategory category;
        category.iDomain = aDomain;
        category.iCategory = aCategory;
        TMediatorCategoryBuffer categoryBuffer( category );
        
        // Create event array write pointer
        TInt arraySize = ( sizeof(MediatorService::TEvent) ) * aEvents.Count();
        TPtr8 arrayPtr( (TUint8*)&aEvents[0], arraySize, arraySize );

        // Send the command to server
        status =  SendReceive( ERegisterEventList, 
                               TIpcArgs( &categoryBuffer, 
                                         &arrayPtr ) );                                        
        }
    return status;
    }


// -----------------------------------------------------------------------------
// RMediatorServer::RegisterCommandListL
// Registers command list to MediatorServer 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::RegisterCommandList( TUid aDomain,  
                                           TUid aCategory,
                                           const RCommandList& aCommands )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::RegisterEventL list\n")); 
    
    //  Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));                                        
    TInt status = KErrNone;
    
    // Check that there's something to register                                           
    if ( aCommands.Count() )
        {
        // Fill category
        TMediatorCategory category;
        category.iDomain = aDomain;
        category.iCategory = aCategory;
        TMediatorCategoryBuffer categoryBuffer( category );
        
        // Create command array write pointer
        TInt arraySize = ( sizeof(MediatorService::TCommand) ) * aCommands.Count();
        TPtr8 arrayPtr( (TUint8*)&aCommands[0], arraySize, arraySize );

        // Send the command to server
        status =  SendReceive( ERegisterCommandList, 
                               TIpcArgs( &categoryBuffer, 
                                         &arrayPtr ) );   
        }
    
    return status;
    }
   
// -----------------------------------------------------------------------------
// RMediatorServer::UnregisterEventL
// Unregisters event list.
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::UnregisterEventList( TUid aDomain,  
                                           TUid aCategory,
                                           const REventList& aEvents )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::UnregisterEventListL\n")));  
    
    // Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    TInt status = KErrNone;
    
    // Check that there's something to unregister                                           
    if ( aEvents.Count() )
        {
        // Fill category
        TMediatorCategory category;
        category.iDomain = aDomain;
        category.iCategory = aCategory;
        TMediatorCategoryBuffer categoryBuffer( category );
        
        // Create event array write pointer
        TInt arraySize = ( sizeof(MediatorService::TEvent) ) * aEvents.Count();
        TPtr8 arrayPtr( (TUint8*)&aEvents[0], arraySize, arraySize );

        // Send the command to server
        status = SendReceive( EUnregisterEventList, 
                                         TIpcArgs( &categoryBuffer, 
                                                   &arrayPtr ) );                                                 
        }
    return status;
    }

// -----------------------------------------------------------------------------
// RMediatorServer::UnregisterCommandL
// Unregisters command list.
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::UnregisterCommandList( TUid aDomain, 
                                             TUid aCategory,
                                             const RCommandList& aCommands )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::UnregisterCommandList\n")));  
    
    // Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    TInt status = KErrNone;
    
    // Check that there's something to unregister                                           
    if ( aCommands.Count() )
        {
        /// Fill category
        TMediatorCategory category;
        category.iDomain = aDomain;
        category.iCategory = aCategory;
        TMediatorCategoryBuffer categoryBuffer( category );
        
        // Create command array write pointer
        TInt arraySize = ( sizeof(MediatorService::TCommand) ) * aCommands.Count();
        TPtr8 arrayPtr( (TUint8*)&aCommands[0], arraySize, arraySize );
        
        // Send the command to server
        status = SendReceive( EUnregisterCommandList, 
                              TIpcArgs( &categoryBuffer, 
                                        &arrayPtr ) );
                                                
        }
    return status;
    }

// -----------------------------------------------------------------------------
// RMediatorServer::RaiseEventL
// Raise event to Mediator Server.
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::RaiseEvent( TUid aDomain,  
                                  TUid aCategory,
                                  TInt aEventId,
                                  TVersion aVersion,
                                  const TDesC8& aData )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::RaiseEvent\n")));  
    
    // Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    // Create a structures for event data
    TMediatorCategory category;
    category.iDomain = aDomain;
    category.iCategory = aCategory;
    TMediatorCategoryBuffer categoryBuffer( category );
        
    TEvent event;
    event.iEventId = aEventId;
    event.iVersion = aVersion;
    TEventBuffer eventBuffer( event );
        
    // Send the command to server
    return SendReceive( ERaiseEvent, 
                          TIpcArgs( &categoryBuffer, 
                                    &eventBuffer, 
                                    &aData ) );
        
    }

// -----------------------------------------------------------------------------
// RMediatorServer::SubscribeEvent
// Subscribes a list of events
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::SubscribeEventList( TUid aDomain,  
                                          TUid aCategory,
                                          const REventList& aEvents )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::SubscribeEventList\n")));  
    
    // Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    TInt status = KErrNone;
    
    // Check that there's something to unregister                                           
    if ( aEvents.Count() )
        {
        // Fill category
        TMediatorCategory category;
        category.iDomain = aDomain;
        category.iCategory = aCategory;
        TMediatorCategoryBuffer categoryBuffer( category );
        
        // Create event array write pointer
        TInt arraySize = ( sizeof(MediatorService::TEvent) ) * aEvents.Count();
        TPtr8 arrayPtr( (TUint8*)&aEvents[0], arraySize, arraySize );

        // Send the command to server
        status = SendReceive( ESubscribeEventList, 
                               TIpcArgs( &categoryBuffer, 
                                         &arrayPtr ) );                                              
        }
    return status;                                   
    }

// -----------------------------------------------------------------------------
// RMediatorServer::UnsubscribeEventListL
// Unsubscribes a list of events.
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::UnsubscribeEventList( TUid aDomain,  
                                            TUid aCategory,
                                            const REventList& aEvents )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::UnsubscribeEventList\n")));  
    
    // Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    TInt status = KErrNone;
    
    // Check that there's something to unregister                                           
    if ( aEvents.Count() )
        {
        // Fill category
        TMediatorCategory category;
        category.iDomain = aDomain;
        category.iCategory = aCategory;
        TMediatorCategoryBuffer categoryBuffer( category );
        
        // Create event array write pointer
        TInt arraySize = ( sizeof(MediatorService::TEvent) ) * aEvents.Count();
        TPtr8 arrayPtr( (TUint8*)&aEvents[0], arraySize, arraySize );

        // Send the command to server
        status = SendReceive( EUnsubscribeEventList, 
                               TIpcArgs( &categoryBuffer, 
                                         &arrayPtr ) );                                              
        }
    return status;      
    }

// -----------------------------------------------------------------------------
// RMediatorServer::IssueCommand
// Synchronous function to send command request to Mediator Server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::IssueCommand( TUid aDomain,
                                    TUid aCategory, 
                                    TInt aCommandId,
                                    TVersion aVersion, 
                                    const TDesC8& aData )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::IssueCommand\n"))); 
    
    //  Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    // Fill category
    TMediatorCategory category;
    category.iDomain = aDomain;
    category.iCategory = aCategory;
    TMediatorCategoryBuffer categoryBuffer( category );
        
    // And command
    TCommand command;
    command.iCommandId = aCommandId;
    command.iVersion = aVersion;
    //Time out is initiatilized to 0 as its not used on the server side.  
    //this is just to supress the tool warnings.
    command.iTimeout = 0;
    TCommandBuffer commandBuffer( command );
        
    // Send the command                                 
    return SendReceive( EIssueCommand, 
                        TIpcArgs( &categoryBuffer, 
                                  &commandBuffer, 
                                  &aData ) );
    
    }
    
// -----------------------------------------------------------------------------
// RMediatorServer::IssueResponse
// Synchronous function to send command request to Mediator Server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::IssueResponse( TUid aDomain, 
                                     TUid aCategory, 
                                     TInt aCommandId,
                                     TInt aStatus,
                                     const TDesC8& aData )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::IssueResponse\n")); 
    
    //  Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    // Fill category
    TMediatorCategory category;
    category.iDomain = aDomain;
    category.iCategory = aCategory;
    TMediatorCategoryBuffer categoryBuffer( category );
        
    // And command
    TCommand command;
    command.iCommandId = aCommandId;
    //All the remaining parameters of TCommand are initiatilized to 0 as its not used on the server side.  
    //this is just to suppress the tool warnings.
    command.iVersion = TVersion(0,0,0);
	command.iCaps.SetEmpty();
    command.iTimeout = 0;

    TCommandBuffer commandBuffer( command );
        
    // Status
    TPckgBuf<TInt> statusBuffer( aStatus );
        
    // Send the command                                 
    return SendReceive( EIssueResponse, 
                        TIpcArgs( &categoryBuffer, 
                                  &commandBuffer, 
                                  &statusBuffer,
                                  &aData ) );
    }

// -----------------------------------------------------------------------------
// RMediatorServer::CancelCommand
// Cancels ongoing command in Mediator
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::CancelCommand( TUid aDomain, 
                                     TUid aCategory, 
                                     TInt aCommandId )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::CancelCommand\n")); 
    
    //  Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    // Fill category
    TMediatorCategory category;
    category.iDomain = aDomain;
    category.iCategory = aCategory;
    TMediatorCategoryBuffer categoryBuffer( category );
        
    // And command
    TCommand command;
    command.iCommandId = aCommandId;
    //All the remaining parameters of TCommand are initiatilized to 0 as its not used on the server side.  
    //this is just to suppress the tool warnings.
	command.iVersion = TVersion(0,0,0);
	command.iCaps.SetEmpty();
    command.iTimeout = 0;

    TCommandBuffer commandBuffer( command );
        
    // Send the command                                 
    return SendReceive( ECancelCommand, 
                        TIpcArgs( &categoryBuffer, 
                                  &commandBuffer ) );
    }


// -----------------------------------------------------------------------------
// RMediatorServer::ReceiveCommands
// Asynchronous function to send command request to Mediator Server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                
void RMediatorServer::ReceiveCommands( TRequestStatus& aStatus,
                                       TMediatorCategoryRetBuffer& aCategoryBuffer,
                                       TCommandRetBuffer& aCommandBuffer,
                                       TPtr8& aCommandData )
    {
    SendReceive( EWaitForCommand, TIpcArgs( &aCategoryBuffer, 
                                            &aCommandBuffer,
                                            &aCommandData ), aStatus );
    }

// -----------------------------------------------------------------------------
// RMediatorServer::GetDomains
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::GetDomains( RDomainList& aDomains )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::GetDomains\n")); 
    
        // Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    // Cleanup 
    aDomains.Reset();
    
    TInt err;
    // Reserve default sized array for domain items
    if ( ( err = aDomains.Reserve( KMediatorMaxDomainCount ) ) != KErrNone )
        {
        ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::GetDomains(ln%d): err=%d\n"), __LINE__, err ) );
        return err;
        }
    
    // Append empty items to array to reserver space
    for ( TInt index = 0; index < KMediatorMaxDomainCount; index++ )
        {
        TUid dummy = TUid::Uid(0);
        aDomains.Append( dummy );
        }
        
    // Define data passing pointers
    TInt arraySize = (sizeof(TUid) ) * KMediatorMaxDomainCount; 
    TPtr8 arrayPtr( (TUint8*)&aDomains[0], arraySize );
    
    // Send the command to server
    TInt status =  SendReceive( EGetDomains, 
                                TIpcArgs( &arrayPtr ) );
    
    // If no errors, Mediator Server completes the message with count
    if ( status > KMediatorMaxDomainCount )
        {
        // Reset array, otherwise Append will not work correctly.
        // Reserve will perform Reset anyway, so omitting this would just complicate the code below
        aDomains.Reset();
        
        // We have too much data coming --> need to fetch more.
        if ( ( err = aDomains.Reserve ( status ) ) != KErrNone )
            {
            ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::GetDomains(ln%d): err=%d\n"), __LINE__, err ) );
            return err;
            }
        
        // Append empty items to array to reserver space
        for ( TInt index = 0; index < status; index++ )
            {
            TUid dummy = TUid::Uid(0);
            aDomains.Append( dummy );
            }
        
        TInt bigArraySize = (sizeof(TUid) ) * status;
        TPtr8 bigArrayPtr( (TUint8*)&aDomains[0], bigArraySize );
        status =  SendReceive( EGetDomains, 
                               TIpcArgs( &bigArrayPtr ) );
        }
        
    // Remove the unneeded items from the array
    for ( TInt index = aDomains.Count() - 1; index >= status && index >= 0 ; index-- )
        {
        aDomains.Remove( index );
        }
    
    if ( status > 0 )
        {
        // No errors
        status = KErrNone;
        }
    return status;
    }


// -----------------------------------------------------------------------------
// RMediatorServer::GetCategories
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::GetCategories( TUid aDomain, RCategoryList& aCategories )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::GetCategories\n")); 
    
        // Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    // Create a structures for fetch data
    TMediatorCategory category;
    category.iDomain    = aDomain;
    category.iCategory  = TUid::Uid(0); // Not used
    TMediatorCategoryBuffer categoryBuffer( category );
    
    // cleanup
    aCategories.Reset();
    
    TInt err;
    // Reserve default sized array for category items
    if ( ( err = aCategories.Reserve( KMediatorMaxCategoryCount ) ) != KErrNone )
        {
        ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::GetCategories(ln%d): err=%d\n"), __LINE__, err ) );
        return err;
        }
    
    // Append empty items to array to reserver space
    for ( TInt index = 0; index < KMediatorMaxCategoryCount; index++ )
        {
        TUid dummy = TUid::Uid(0);
        aCategories.Append( dummy );
        }
    
    // Define data passing pointers
    TInt arraySize = (sizeof(TUid) ) * KMediatorMaxCategoryCount; 
    TPtr8 arrayPtr( (TUint8*)&aCategories[0], arraySize );
    
    // Send the command to server
    TInt status = SendReceive( EGetCategories, 
                               TIpcArgs( &categoryBuffer, 
                                         &arrayPtr ) ); 
                                  
    // If no errors, Mediator Server completes the message with count
    if ( status > KMediatorMaxCategoryCount )
        {
        // We have too much data coming --> need to fetch more.
        // Cleanup categories
        aCategories.Reset();
        
        if ( ( err = aCategories.Reserve( status ) ) != KErrNone )
            {
            ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::GetCategories(ln%d): err=%d\n"), __LINE__, err ) );
            return err;
            }
        
        // Append empty items to array to reserver space
        for ( TInt index = 0; index < status; index++ )
            {
            TUid dummy = TUid::Uid(0);
            aCategories.Append( dummy );
            }
        TInt bigArraySize = (sizeof(TUid) ) * status;
        TPtr8 bigArrayPtr( (TUint8*)&aCategories[0], bigArraySize );
        status =  SendReceive( EGetCategories, 
                               TIpcArgs( &categoryBuffer, 
                                         &bigArrayPtr ) );
        }
    
    // Remove the unneeded items from the array
    for ( TInt index = aCategories.Count() - 1; index >= status && index >= 0 ; index-- )
        {
        aCategories.Remove( index );
        }
    
    if ( status > 0 )
        {
        // No errors
        status = KErrNone;
        }
    return status;                                   
    }

// -----------------------------------------------------------------------------
// RMediatorServer::GetEvents
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::GetEvents( TUid aDomain, 
                                 TUid aCategory,
                                 REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::GetEvents\n")); 
    
        // Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    // Cleanup 
    aEvents.Reset();
    
    // Create a structures for fetch data
    TMediatorCategory category;
    category.iDomain    = aDomain;
    category.iCategory  = aCategory;
    TMediatorCategoryBuffer categoryBuffer( category );
    
    TInt err;
    // Reserve default sized array for event items
    if ( ( err = aEvents.Reserve( KMediatorMaxEventCount ) ) != KErrNone )
        {
        ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::GetEvents(ln%d): err=%d\n"), __LINE__, err ) );
        return err;
        }
    
    // Append empty items to array to reserver space
    for ( TInt index = 0; index < KMediatorMaxEventCount; index++ )
        {
        TEvent emptyEvent;
        aEvents.Append( emptyEvent );
        }
    
    // Define data return pointers
    TInt arraySize = ( sizeof(MediatorService::TEvent) ) * KMediatorMaxEventCount; 
    TPtr8 arrayPtr( (TUint8*)&aEvents[0], arraySize );
    
    // Send the command to server
    TInt status = SendReceive( EGetEvents, 
                               TIpcArgs( &categoryBuffer, 
                                         &arrayPtr ) );
                                  
    // If no errors, Mediator Server completes the message with count
    if ( status > KMediatorMaxEventCount )
        {
        // We have too much data coming --> need to fetch more.
        // Cleanup events
        aEvents.Reset();
        
        // Reserve default sized array for event items
        if ( ( err = aEvents.Reserve( status ) ) != KErrNone )
            {
            ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::GetEvents(ln%d): err=%d\n"), __LINE__, err ) );
            return err;
            }
        
        // Append empty items to array to reserver space
        for ( TInt index = 0; index < status; index++ )
            {
            TEvent emptyEvent;
            aEvents.Append( emptyEvent );
            }
        // Update pointers
        TInt bigArraySize = ( sizeof(MediatorService::TEvent) ) * status;
        TPtr8 bigArrayPtr( (TUint8*)&aEvents[0], bigArraySize );
        status =  SendReceive( EGetEvents, 
                               TIpcArgs( &categoryBuffer, 
                                         &bigArrayPtr ) );
        }

    // Remove the unneeded items from the array
    for ( TInt index = aEvents.Count() - 1; index >= status && index >= 0 ; index-- )
        {
        aEvents.Remove( index );
        }
    
    if ( status > 0 )
        {
        // No errors
        status = KErrNone;
        }
    return status;                                                   
    }

// -----------------------------------------------------------------------------
// RMediatorServer::GetCommands
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::GetCommands( TUid aDomain, 
                                   TUid aCategory,
                                   RCommandList& aCommands )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::GetCommands\n")); 
    
        // Check that session is open.
    __ASSERT_DEBUG( Handle(), User::Panic( KMediatorServerPanic, 
                                           EMediatorClientNoSessionActive ));
    
    // Create a structures for fetch data
    TMediatorCategory category;
    category.iDomain    = aDomain;
    category.iCategory  = aCategory;
    TMediatorCategoryBuffer categoryBuffer( category );
    
    // Cleanup 
    aCommands.Reset();
    
    TInt err;
    // Reserve default sized array for command items
    if ( ( err = aCommands.Reserve( KMediatorMaxCommandCount ) ) != KErrNone )
        {
        ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::GetCommands(ln%d): err=%d\n"), __LINE__, err ) );
        return err;
        }
    
    // Append empty items to array to reserver space
    for ( TInt index = 0; index < KMediatorMaxCommandCount; index++ )
        {
        TCommand emptyCommand;
        aCommands.Append( emptyCommand );
        }
    
     // Define data return pointers
    TInt arraySize = ( sizeof(MediatorService::TCommand) ) * KMediatorMaxCommandCount; 
    TPtr8 arrayPtr( (TUint8*)&aCommands[0], arraySize );
    
    // Send the command to server
    TInt status = SendReceive( EGetCommands, 
                               TIpcArgs( &categoryBuffer, 
                                         &arrayPtr ) );
                                  
    // If no errors, Mediator Server completes the message with count
    if ( status > KMediatorMaxCommandCount )
        {
        // We have too much data coming --> need to fetch more.
        // Cleanup commands
        aCommands.Reset();
        
        if ( ( err = aCommands.Reserve( status ) ) != KErrNone )
            {
            ERROR_TRACE(Print(_L("[Mediator] RMediatorServer::GetCommands(ln%d): err=%d\n"), __LINE__, err ) );
            return err;
            }
            
        // Append empty items to array to reserver space
        for ( TInt index = 0; index < status; index++ )
            {
            TCommand emptyCommand;
            aCommands.Append( emptyCommand );
            }
        // Update pointers
        TInt bigArraySize = ( sizeof(MediatorService::TCommand) ) * status;
        TPtr8 bigArrayPtr( (TUint8*)&aCommands[0], bigArraySize );
        status =  SendReceive( EGetCommands, 
                               TIpcArgs( &categoryBuffer, 
                                         &bigArrayPtr ) );
        }
    // Remove the unneeded items from the array
    for ( TInt index = aCommands.Count() - 1; index >= status && index >= 0 ; index-- )
        {
        aCommands.Remove( index );
        }
    
    if ( status > 0 )
        {
        // No errors
        status = KErrNone;
        }
    return status;                                                
    }

// -----------------------------------------------------------------------------
// RMediatorServer::ReceiveEvents
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorServer::ReceiveEvents( TRequestStatus& aStatus,
                                     TMediatorCategoryRetBuffer& aCategoryBuffer,
                                     TEventRetBuffer& aEventBuffer, 
                                     TPtr8& aEventData )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::ReceiveEvents\n"))); 
    SendReceive( EWaitForEvent, TIpcArgs( &aCategoryBuffer, 
                                          &aEventBuffer, 
                                          &aEventData ), aStatus );
    }

// -----------------------------------------------------------------------------
// RMediatorServer::ReceiveNotifications
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//               
void RMediatorServer::ReceiveNotifications( TRequestStatus& aStatus,
                                            TMediatorCategoryRetBuffer& aCategoryBuffer,
                                            TNotificationTypeRetBuffer& aTypeBuffer,
                                            TPtr8& aEventArrayPtr, 
                                            TPtr8& aCommandArrayPtr )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::ReceiveNotifications\n")));
    SendReceive( EWaitForNotifications, TIpcArgs( &aCategoryBuffer, 
                                                  &aTypeBuffer,
                                                  &aEventArrayPtr,
                                                  &aCommandArrayPtr ), aStatus );
    }

// -----------------------------------------------------------------------------
// RMediatorServer::ReceiveEvents
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RMediatorServer::CancelNotifications( )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::CancelNotifications\n"))); 
    return SendReceive( ECancelNotifications, TIpcArgs() );
    }

// -----------------------------------------------------------------------------
// RMediatorServer::WaitForCommandResponse
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void RMediatorServer::WaitForCommandResponse( 
                        TRequestStatus& aStatus,
                        TMediatorCategoryRetBuffer& aCategoryBuffer,
                        TCommandRetBuffer& aCommandBuffer, 
                        TPtr8& aCommandData,
                        TPckg<TInt>& aStatusBuffer  )
    {
    TRACE(Print(_L("[Mediator Server]\t RMediatorServer::WaitForCommandResponse\n"))); 
    SendReceive( EWaitForCommandResponse, TIpcArgs( &aCategoryBuffer, 
                                                    &aCommandBuffer, 
                                                    &aCommandData,
                                                    &aStatusBuffer ), aStatus );
    }
    
// -----------------------------------------------------------------------------
// RMediatorServer::FetchParameterData
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                 
TInt RMediatorServer::FetchParameterData( TPtr8& aParameterData )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::FetchParameterData\n")); 
    return SendReceive( EFetchParameterData, TIpcArgs( &aParameterData ) );
    }

// -----------------------------------------------------------------------------
// RMediatorServer::FetchNotificationEventList
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                 
TInt RMediatorServer::FetchNotificationEventList( TPtr8& aEventArrayPtr )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::FetchNotificationEventList\n")); 
    return SendReceive( EFetchNotificationEventList, TIpcArgs( &aEventArrayPtr ) );
    }
    
// -----------------------------------------------------------------------------
// RMediatorServer::FetchNotificationCommandList
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                 
TInt RMediatorServer::FetchNotificationCommandList( TPtr8& aCommandArrayPtr )
    {
    LOG(_L("[Mediator Server]\t RMediatorServer::FetchNotificationEventList\n")); 
    return SendReceive( EFetchNotificationCommandList, TIpcArgs( &aCommandArrayPtr ) );
    }

// End of file
