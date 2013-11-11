/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Session implementation class for Mediator Server
*
*/


// INCLUDE FILES
#include    <e32base.h>
#include    <s32mem.h>

#include    "MediatorServerSession.h"
#include    "MediatorCommon.h"
#include    "MediatorServer.h"
#include    "MediatorServiceDefs.h"
#include    "mediatorqueitem.h"
#include    "MediatorDebug.h"
#include    "Debug.h"
using namespace MediatorService;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorServerSession::CMediatorServerSession
// -----------------------------------------------------------------------------
//
CMediatorServerSession::CMediatorServerSession() :iNotificationsRegistered( EFalse ),
                                                  iNotificationQueue( _FOFF(CMediatorQueItem,iSlink) )
	{
	}

// -----------------------------------------------------------------------------
// CMediatorServerSession::NewL
// -----------------------------------------------------------------------------
//
CMediatorServerSession* CMediatorServerSession::NewL()
	{
	LOG(_L("[Mediator Server]\t CMediatorServerSession::NewL"));
	return new( ELeave ) CMediatorServerSession();
	}

// -----------------------------------------------------------------------------
// CMediatorServerSession::~CMediatorServerSession
// -----------------------------------------------------------------------------
//    
CMediatorServerSession::~CMediatorServerSession()
	{
	LOG(_L("[Mediator Server]\t CMediatorServerSession::~CMediatorServerSession"));

    // cancel all asynchronous waiting procedures
    CancelAll( KErrCancel );

    // cancel pending commands
    Server().CommandHandler().CancelCommands( this, this );

	// Unregister all subscriptions
	// Get handler
    CMediatorServerObjectHandler& objectHandler = Server().ObjectHandler();
    objectHandler.ClearRegistrations( this, this, this );
    
    iEventServiceList.ResetAndDestroy();
	iCommandServiceList.ResetAndDestroy();
	
	if ( iDataBuffer )
	    {
	    delete iDataBuffer;
	    }
	
	CleanNotificationQueue();
	}

// -----------------------------------------------------------------------------
// CMediatorServerSession::CleanNotificationQueue
// -----------------------------------------------------------------------------
// 
void CMediatorServerSession::CleanNotificationQueue()
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::CleanNotificationQueue"));
    
    // release memory allocated to notification queue items
    TSglQueIter<CMediatorQueItem> iter( iNotificationQueue );
    CMediatorQueItem* item;
    
    while ( ( item = iter++ ) != NULL)
        {
        iNotificationQueue.Remove( *item );
        delete item;
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::Server
// Returns a reference to server
// -----------------------------------------------------------------------------
//
CMediatorServer& CMediatorServerSession::Server()
    {
    return *static_cast<CMediatorServer*>(const_cast<CServer2*>(CSession2::Server()));
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::ServiceL
// Implements CSession2 -derived ServiceL -method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::ServiceL(const RMessage2& aMessage)
	{
	LOG(_L("[Mediator Server]\t CMediatorServerSession::ServiceL"));
	
	// If ServiceL leaves, default implementation in server framework
	// completes the RMessage2 with the leave code.
	DispatchMessageL( aMessage );
	}
	
// -----------------------------------------------------------------------------
// CMediatorServerSession::ServiceError
// Implements CSession2 -derived ServiceError -method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::ServiceError(const RMessage2& aMessage,TInt aError)
    {
    ERROR_TRACE(Print(_L("[Mediator] CMediatorServerSession::ServiceError: opcode=%d, aError=%d\n"), aMessage.Function(), aError ));
    
    RMediatorDebug::LogError(aMessage, aError);
    
    CSession2::ServiceError(aMessage, aError);
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::ReadCategoryL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
TMediatorCategory CMediatorServerSession::ReadCategoryL( 
                                        const RMessage2& aMessage,
                                        const TInt aMessageSlot )
    {
    TMediatorCategoryBuffer categoryBuffer;
    aMessage.ReadL( aMessageSlot, categoryBuffer );
    TMediatorCategory category = categoryBuffer();
    return category;
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::WriteCategoryL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
void CMediatorServerSession::WriteCategoryL( TUid aDomain,
                                             TUid aCategory,
                                             const RMessage2& aMessage,
                                             const TInt aMessageSlot  )
    {
    TMediatorCategory category;
    category.iDomain = aDomain;
    category.iCategory = aCategory;
    TMediatorCategoryRetBuffer categoryBuffer( category );
    aMessage.WriteL( aMessageSlot, categoryBuffer );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::WriteEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
void CMediatorServerSession::WriteEventL( TInt aEventId,
                                          const RMessage2& aMessage,
                                          const TInt aMessageSlot )
    {
    MediatorService::TEvent event;
    event.iEventId = aEventId;
    TEventRetBuffer buffer( event );
    aMessage.WriteL( aMessageSlot, buffer );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::WriteCommandL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
void CMediatorServerSession::WriteCommandL( TInt aCommandId,
                                            TVersion aVersion,
                                            const RMessage2& aMessage,
                                            const TInt aMessageSlot )
    {
    MediatorService::TCommand command;
    command.iCommandId  = aCommandId;
    command.iVersion    = aVersion;
    TCommandRetBuffer buffer( command );
    aMessage.WriteL( aMessageSlot, buffer );
    }
    
   
// -----------------------------------------------------------------------------
// CMediatorServerSession::ReadEventListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
REventList CMediatorServerSession::ReadEventListL( const RMessage2& aMessage,
                                                   const TInt aMessageSlot )
    {
     // Read event list information from Message
    REventList eventList;
    CleanupClosePushL( eventList );
    
    // Create input pointer for data
    TInt size = aMessage.GetDesMaxLength( aMessageSlot );
    TInt count = size / ( sizeof(MediatorService::TEvent) );
    
    if ( count <= 0 )
        {
        User::Leave( KErrArgument );
        }
    
    for ( TInt index = 0; index < count; index ++ )
        {
        TEvent event;
        eventList.AppendL( event );
        }
    TPtr8 arrayPtr( (TUint8*)&eventList[0], size );
    
    // Read data from message
    aMessage.ReadL( aMessageSlot, arrayPtr );
    
    CleanupStack::Pop( &eventList );
    return eventList;
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::EventListFitsToClientMemory
// -----------------------------------------------------------------------------
// 
TBool CMediatorServerSession::EventListFitsToClientMemory( const REventList& aEvents,
                                                           const RMessage2& aMessage,
                                                           const TInt aMessageSlot )
    {
    TBool retval( ETrue );
    
    if ( aEvents.Count() > 0 )
        {
        TInt arraySize( ( sizeof(MediatorService::TEvent) ) * aEvents.Count() );
    
        // Check that there's enough space before writing the answer
        TInt space( aMessage.GetDesMaxLength( aMessageSlot ) ); 
        
        retval = ( arraySize <= space );
        }
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::CommandListFitsToClientMemory
// -----------------------------------------------------------------------------
// 
TBool CMediatorServerSession::CommandListFitsToClientMemory( const RCommandList& aCommands,
                                                             const RMessage2& aMessage,
                                                             const TInt aMessageSlot )
    {
    TBool retval( ETrue );
    
    if ( aCommands.Count() > 0 )
        {
        TInt arraySize( ( sizeof(MediatorService::TCommand) ) * aCommands.Count() );
        
        // Check that there's enough space before writing the answer
        TInt space( aMessage.GetDesMaxLength( aMessageSlot ) ); 
                
        retval = ( arraySize <= space );
        }
    
    return retval;
    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::WriteEventListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
void CMediatorServerSession::WriteEventListL( const REventList& aEvents,
                                              const RMessage2& aMessage,
                                              const TInt aMessageSlot )
    {
    if ( aEvents.Count() > 0 )
        {
        TInt arraySize = ( sizeof(MediatorService::TEvent) ) * aEvents.Count();
        TInt space( aMessage.GetDesMaxLength( aMessageSlot ) );
        
        // the method assumes that there's enought space in client memory to accommadate the events
        __ASSERT_DEBUG( space >= arraySize, User::Invariant() );
        
        if ( arraySize > space )
            {
            arraySize = space; // ensure that we don't overflow client buffer in any circumstances
            }

        TPtr8 arrayPtr( (TUint8*)&aEvents[0], arraySize, arraySize );
        aMessage.WriteL( aMessageSlot, arrayPtr );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::ReadCommandListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
RCommandList CMediatorServerSession::ReadCommandListL( const RMessage2& aMessage,
                                                       const TInt aMessageSlot )
    {
     // Read command list information from Message
    RCommandList commandList;
    CleanupClosePushL( commandList );
    
    // Create input pointer for data
    TInt size = aMessage.GetDesMaxLength( aMessageSlot );
    TInt count = size / ( sizeof(MediatorService::TCommand) );
    for ( TInt index = 0; index < count; index ++ )
        {
        TCommand command;
        commandList.AppendL( command );
        }
    TPtr8 arrayPtr( (TUint8*)&commandList[0], size );
    
    // Read data from message
    aMessage.ReadL( aMessageSlot, arrayPtr );
    
    CleanupStack::Pop( &commandList );
    return commandList;
    }
   

// -----------------------------------------------------------------------------
// CMediatorServerSession::WriteCommandListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
void CMediatorServerSession::WriteCommandListL( const RCommandList& aCommands,
                                                const RMessage2& aMessage,
                                                const TInt aMessageSlot )
    {
    // Write the command list back to client
    if ( ( aCommands.Count() > 0 ) )
        {
        TInt arraySize( ( sizeof(MediatorService::TCommand) ) * aCommands.Count() );

        // Check that there's enough space before writing the answer
        TInt space( aMessage.GetDesMaxLength( aMessageSlot ) ); 
        
        // this method assumes that there's enough space in client side
        __ASSERT_DEBUG( arraySize <= space, User::Invariant() );
        
        if ( arraySize > space ) // overflow prevention
            {
            arraySize = space;
            }
        
        TPtr8 arrayPtr( (TUint8*)&aCommands[0], arraySize, arraySize );    
        aMessage.WriteL( aMessageSlot, arrayPtr );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::ReadDataL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
HBufC8* CMediatorServerSession::ReadDataLC( const RMessage2& aMessage,
                                            const TInt aMessageSlot )
    {
     // Then read parameter data from slot 2
    TInt dataSize = aMessage.GetDesLength( aMessageSlot );

    if ( dataSize < 0 )
        {
        User::Leave( KErrArgument );
        }

    HBufC8* data = HBufC8::NewLC( dataSize );
    TPtr8 ptr( data->Des() );
	aMessage.ReadL( aMessageSlot, ptr );
	return data;
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::WriteDataL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
void CMediatorServerSession::WriteDataL( const TDesC8& aData,
                                         const RMessage2& aMessage,
                                         const TInt aMessageSlot )
    {
    // Read max data length
    TInt space = aMessage.GetDesMaxLength( aMessageSlot );
    if ( space >= aData.Length() )
        {
        aMessage.WriteL( aMessageSlot, aData );
        }
    else    // Not enough space to write
        {
        // Store the data to temporary buffer --> it is fetched from client side
        // synchronously
        if ( iDataBuffer )
            {
            delete iDataBuffer;
            iDataBuffer = NULL;
            }
        iDataBuffer = aData.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::ReadEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
MediatorService::TEvent CMediatorServerSession::ReadEventL( 
                                    const RMessage2& aMessage,
                                    const TInt aMessageSlot )
    {
    TEventBuffer eventBuffer;
    aMessage.ReadL( aMessageSlot, eventBuffer );
    return eventBuffer();
    }
    
// -----------------------------------------------------------------------------
// CMediatorServerSession::ReadCommandL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
MediatorService::TCommand CMediatorServerSession::ReadCommandL( 
                                    const RMessage2& aMessage,
                                    const TInt aMessageSlot )
    {
    TCommandBuffer commandBuffer;
    aMessage.ReadL( aMessageSlot, commandBuffer );
    return commandBuffer();
    }
    
// -----------------------------------------------------------------------------
// CMediatorServerSession::DispatchMessageL
// Dispatches message according to its opcode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::DispatchMessageL( const RMessage2 &aMessage )
    {
	LOG(_L("[Mediator Server]\t CMediatorServerSession::DispatchMessageL"));
	
	RMediatorDebug::Log(aMessage, *this);
	
    switch ( aMessage.Function() )
        {
        case ERegisterEventList:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: ERegisterEventList"));
            RegisterEventListL( aMessage );
            break;
            }    
        case ERegisterCommandList:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: ERegisterCommand"));
            RegisterCommandListL( aMessage );
            break;
            }
        case EUnregisterEventList:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EUnregisterEventList"));
            UnregisterEventListL( aMessage );
            break;
            }
         case EUnregisterCommandList:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EUnregisterCommandList"));
            UnregisterCommandListL( aMessage );
            break;
            }
        case ESubscribeEventList:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: ESubscribeEvent"));
            SubscribeEventListL( aMessage );
            break;
            }
        case EUnsubscribeEventList:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EUnsubscribeEvent"));
            UnsubscribeEventListL( aMessage );
            break;
            }
        case ERaiseEvent:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: ERaiseEvent"));
            RaiseEventL( aMessage );
            break;
            }
        case EIssueCommand:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: IssueCommandL"));
            IssueCommandL( aMessage );
            break;
            }  
         case ECancelCommand:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: CancelCommandL"));
            CancelCommandL( aMessage );
            break;
            }
         case EIssueResponse:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EIssueResponse"));
            IssueResponseL( aMessage );
            break;
            }
        case EGetDomains:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EGetDomains"));
            GetDomainsL( aMessage );
            break;
            }
        case EGetCategories:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EGetCategories"));
            GetCategoriesL( aMessage );
            break;
            }
        case EGetCommands:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EGetCommands"));
            GetCommandsL( aMessage );
            break;
            }
        case EGetEvents:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EGetEvents"));
            GetEventsL( aMessage );
            break;
            }
        case EWaitForEvent:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EWaitForEvent"));
            WaitForEventsL( aMessage );
            break;
            }
        case EWaitForCommand:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EWaitForCommand"));
            WaitForCommandsL( aMessage );
            break;
            }
        case EWaitForNotifications:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EWaitForNotifications"));
            WaitForNotificationsL( aMessage );
            break;
            }
        case ECancelNotifications:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: ECancelNotifications"));
            CancelNotificationsL( aMessage );
            break;
            }
        case EWaitForCommandResponse:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EWaitForCommandResponse"));
            WaitForCommandResponseL( aMessage );
            break;
            }
        case EFetchParameterData:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EFetchParameterData"));
            FetchParameterDataL( aMessage );
            break;
            }
        case ECancelAll:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: ECancelAll"));
            CancelAll( aMessage );
            break;
            }
            
        case EFetchNotificationEventList:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EFetchNotificationEventList"));
            FetchNotificationEventListL( aMessage );
            break;    
            }
        
        case EFetchNotificationCommandList:
            {
            LOG(_L("[Mediator Server]\t DispatchMessageL: EFetchNotificationCommandList"));
            FetchNotificationCommandListL( aMessage );
            break;
            }
        default:
            {
            // Unknown opcode --> panic client
            ERROR_TRACE(Print(_L("[Mediator] CMediatorServerSession::DispatchMessageL: Unknown opcode=%d\n"), aMessage.Function()));
            CMediatorServer::PanicClient( aMessage, EMediatorClientBadRequest );
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::MediatorCommandL
// Callback from command handler when command is issued
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::MediatorCommandL( TUid aDomain,
                                               TUid aCategory, 
                                               TInt aCommandId,
                                               TVersion aVersion, 
                                               const TDesC8& aData )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::MediatorCommandL"));
    
    if ( !iCommandInitMessage.IsNull() )
        {
        // Write domain & category information
        WriteCategoryL( aDomain, aCategory, iCommandInitMessage, 0 );
        
        // Write event to slot 1
        WriteCommandL( aCommandId, aVersion, iCommandInitMessage, 1 );
        
        // Write data to slot 2
        WriteDataL( aData, iCommandInitMessage, 2 );
        
        // Complete message with data size
        iCommandInitMessage.Complete( aData.Length() );
        }
    else    // Append command to service list
        {
        // Create a new command object
        CCommand* newCommand = CCommand::NewL( aCommandId );
        CleanupStack::PushL( newCommand );
        newCommand->SetParameterDataL( aData );
        newCommand->SetDomain( aDomain );
        newCommand->SetCategory( aCategory );
        newCommand->SetVersion( aVersion );
        iCommandServiceList.AppendL( newCommand );  // Last to queue
        CleanupStack::Pop( newCommand );  // ownership is in the array
        TRACE(Print(_L("[Mediator Server]\t Session %d - command buffer %d\n"), this, iCommandServiceList.Count() ));
        }
    
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::CancelMediatorCommandL
// Callback from command handler when command processing is cancelled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::CancelMediatorCommandL( TUid aDomain,
                                                     TUid aCategory, 
                                                     TInt aCommandId )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::CancelMediatorCommand"));
    
    // Check the service list and remove the command from there if exists
    // At this point it has not been delivered to responder yet
    TBool found = EFalse;
    for ( TInt index = 0; index < iCommandServiceList.Count() && !found; index++ )
        {
        CCommand* commandPtr = iCommandServiceList[index];
        if ( commandPtr )
            {
            if ( commandPtr->Domain() == aDomain
                 && commandPtr->Category() == aCategory
                 && commandPtr->Id() == aCommandId 
                 && commandPtr->Status() == ECommandPending )
                {
                iCommandServiceList.Remove( index );
                delete commandPtr;
                commandPtr = NULL;
                found = ETrue;
                }
            }
        }
    
    // Write domain & category information to slot 0
    if ( iCommandInitMessage.Handle() && !found )
        {
        WriteCategoryL( aDomain, aCategory, iCommandInitMessage, 0 );
    
        // Write command id and version to slot 1
        TVersion version( 0, 0, 0 );
        WriteCommandL( aCommandId, version, iCommandInitMessage, 1 );
        
        // Status to slot 2
        TPckg<TInt> statusBuffer( KErrCancel );
        iCommandInitMessage.WriteL( 2, statusBuffer );
        
        // Write data to slot 3
        WriteDataL( KNullDesC8, iCommandInitMessage, 2 );
        
        // Complete message
        iCommandInitMessage.Complete( KErrCancel );
        }
    // The command hs been delivered, but the responder is busy or otherwise is not
    // yet ready to handle the command cancellation, add to the list with cancelled status
    else if ( !iCommandInitMessage.Handle() && !found )
        {
        // Create a new command object
        CCommand* newCommand = CCommand::NewL( aCommandId );
        CleanupStack::PushL( newCommand );
        newCommand->SetDomain( aDomain );
        newCommand->SetCategory( aCategory );
        newCommand->SetStatus( ECommandCanceled );
        iCommandServiceList.AppendL( newCommand );  // Last to queue
        CleanupStack::Pop( newCommand );  // ownership is in the array
        }
            

    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::CancelMediatorCommandL
// Callback from command handler when command processing is cancelled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::MediatorCommandTimeoutL( TUid aDomain,
                                                      TUid aCategory, 
                                                      TInt aCommandId )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::MediatorCommandTimeoutL"));
    
    // Check the service list and remove the command from there if exists
    // At this point it has not been delivered to responder yet
    TBool found = EFalse;
    for ( TInt index = 0; index < iCommandServiceList.Count() && !found; index++ )
        {
        CCommand* commandPtr = iCommandServiceList[index];
        if ( commandPtr )
            {
            if ( commandPtr->Domain() == aDomain
                 && commandPtr->Category() == aCategory
                 && commandPtr->Id() == aCommandId
                 && commandPtr->Status() == ECommandPending ) // only pending commands are completed with timeout
                {
                iCommandServiceList.Remove( index );
                delete commandPtr;
                commandPtr = NULL;
                found = ETrue;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::CommandResponseL
// Callback from command handler when command processing is cancelled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::CommandResponseL( TUid aDomain,
                                               TUid aCategory, 
                                               TInt aCommandId,
                                               TInt aStatus, 
                                               const TDesC8& aData )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::CommandResponseL"));
    
    if ( !iCommandResponseMessage.IsNull() )
        {
        // Write domain & category information
        WriteCategoryL( aDomain, aCategory, iCommandResponseMessage, 0 );
        
        // Write command to slot 1
        TVersion version( 0, 0, 0 );
        WriteCommandL( aCommandId, version, iCommandResponseMessage, 1 );
        
        // Write data to slot 2
        WriteDataL( aData, iCommandResponseMessage, 2 );
        
        // Status to slot 3
        TPckg<TInt> statusBuffer( aStatus );
        iCommandResponseMessage.WriteL( 3, statusBuffer );
        
        // Complete message with data size
        iCommandResponseMessage.Complete( aData.Length() );
        }
    }
    
// -----------------------------------------------------------------------------
// CMediatorServerSession::MediatorEventL
// Callback from event handler when event occurs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::MediatorEventL( TUid aDomain,
                                             TUid aCategory, 
                                             TInt aEventId, 
                                             const TDesC8& aData )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::MediatorEventL"));   
    if ( !iEventNotificationMessage.IsNull() )
        {
        // Write domain & category information
        WriteCategoryL( aDomain, aCategory, iEventNotificationMessage, 0 );
        
        // Write event to slot 1
        WriteEventL( aEventId, iEventNotificationMessage, 1 );
        
        // Write data to slot 2
        WriteDataL( aData, iEventNotificationMessage, 2 );
        
        // Complete message with data size
        iEventNotificationMessage.Complete( aData.Length() );
        }
    else    // Append event to service list
        {
        // Create a new event object
        CEvent* newEvent = CEvent::NewL( aEventId );
        CleanupStack::PushL( newEvent );
        newEvent->SetParameterDataL( aData );
        newEvent->SetDomain( aDomain );
        newEvent->SetCategory( aCategory );
        iEventServiceList.AppendL( newEvent );  // Last to queue
        CleanupStack::Pop( newEvent );  // ownership is in the array
        TRACE(Print(_L("[Mediator Server]\t Session %d - event buffer %d\n"),this ,iEventServiceList.Count() ));
        }
    
    }
    
// -----------------------------------------------------------------------------
// CMediatorServerSession::MediatorEventsAddedL
// Callback from event handler when an event is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::MediatorEventsAddedL( TUid aDomain, 
                                                   TUid aCategory, 
                                                   const REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::MediatorEventsAddedL"));
    
    HandleMediatorEventsChangedL( EMediatorEventsRegistered, aDomain, aCategory, aEvents );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::HandleMediatorEventsChangedL
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::HandleMediatorEventsChangedL( TMediatorNotificationType aNotificationType,
                                                         const TUid& aDomain, 
                                                         const TUid& aCategory, 
                                                         const REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::HandleMediatorEventsChangedL"));
    
    if ( !iNotificationMessage.IsNull() )
        {
        TBool eventsPassed( ETrue );
        
        DoMediatorEventsChangedL( aNotificationType, aDomain, aCategory, aEvents, eventsPassed );
        
        if ( !eventsPassed )
            {
            // store events for next EFetchNotificationEventList
            CMediatorQueItem* eventsBuffer = CEventListQueItem::NewL( aNotificationType, aDomain, aCategory, aEvents );
            iNotificationQueue.AddFirst( *eventsBuffer );
            }
        }
    else if ( iNotificationsRegistered )
        {
        LOG(_L("[Mediator Server]\t CMediatorServerSession::HandleMediatorEventsChangedL: received events buffered"));
        
        // store events for next EWaitForNotifications
        CMediatorQueItem* eventsBuffer = CEventListQueItem::NewL( aNotificationType, aDomain, aCategory, aEvents );
        iNotificationQueue.AddLast( *eventsBuffer );
        }
    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::DoMediatorEventsChangedL
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::DoMediatorEventsChangedL( TMediatorNotificationType aNotificationType,
                                                     const TUid& aDomain, 
                                                     const TUid& aCategory, 
                                                     const REventList& aEvents,
                                                     TBool& aEventsPassedToClient )
    {
    TRACE( Print( _L("[Mediator Server]\t CMediatorServerSession::DoMediatorEventsChangedL: D=0x%x C=0x%x, count=%d"),
                    aDomain.iUid, aCategory.iUid, aEvents.Count() ) );
    
    // Write category information to message slot 0
    WriteCategoryL( aDomain, aCategory, iNotificationMessage, 0 );
    
    // Write notification type to slot 1
    TNotificationTypeRetBuffer typeBuffer( aNotificationType );
    iNotificationMessage.WriteL( 1, typeBuffer );
    
    // Write event list
    if ( EventListFitsToClientMemory( aEvents, iNotificationMessage, 2 ) )
        {
        WriteEventListL( aEvents, iNotificationMessage, 2 );
        aEventsPassedToClient = ETrue;
        }
    else
        {
        aEventsPassedToClient = EFalse;
        }
    
    // Complete message
    iNotificationMessage.Complete( aEvents.Count() );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::PurgeNextBufferedNotificationL
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::PurgeNextBufferedNotificationL()
    {
    if ( !iNotificationQueue.IsEmpty() )
         {
         TBool completed( ETrue );
         CMediatorQueItem* notification = iNotificationQueue.First();
         
         TRACE( Print( _L("[Mediator Server]\t CMediatorServerSession::PurgeNextBufferedNotificationL: type=%d"),
                       notification->NofiticationType() ) );
         // remove now, notification will be pushed back to queue if cannot be delivered with the current client request
         iNotificationQueue.Remove( *notification ); 
         CleanupStack::PushL( notification );
         
         switch ( notification->NofiticationType() )
             {
             case EMediatorEventsRegistered:
             case EMediatorEventsUnregistered:    
                 DoMediatorEventsChangedL( notification->NofiticationType(),
                                           notification->Domain(),
                                           notification->Category(),
                                           *(notification->Events()),
                                           completed ); 
                 break;

             case EMediatorCommandsRegistered:
             case EMediatorCommandsUnregistered:
                 DoMediatorCommandsChangedL( notification->NofiticationType(),
                                             notification->Domain(),
                                             notification->Category(),
                                             *(notification->Commands()),
                                             completed );
                 break;
                 
             case EMediatorCategoryUnregistered:
                 DoMediatorCategoryRemovedL( notification->Domain(), 
                                             notification->Category() );    
                 break;
             
             default:
                 __ASSERT_DEBUG( EFalse, User::Invariant() );
             }
         
         if ( completed )
             {
             // no longer needed, cleanup
             CleanupStack::PopAndDestroy( notification );
             }
         else // could not be completed without further client request
             {
             iNotificationQueue.AddFirst( *notification );
             CleanupStack::Pop( notification );
             }

         }
    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::MediatorCommandsAddedL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::MediatorCommandsAddedL( TUid aDomain, 
                                                     TUid aCategory,
                                                     const RCommandList& aCommands )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::MediatorCommandsAddedL"));
    
    HandleMediatorCommandsChangedL( EMediatorCommandsRegistered, aDomain, aCategory, aCommands);
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::HandleMediatorCommandsChangedL
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::HandleMediatorCommandsChangedL( TMediatorNotificationType aNotificationType,
                                                             const TUid& aDomain, 
                                                             const TUid& aCategory, 
                                                             const RCommandList& aCommands )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::HandleMediatorCommandsChangedL"));
    if ( !iNotificationMessage.IsNull() )
        {
        TBool commandListPassed( ETrue );
               
        DoMediatorCommandsChangedL( aNotificationType, aDomain, aCategory, aCommands, commandListPassed );
       
        if ( !commandListPassed )
            {
            // store commands for next EFetchNotificationCommandList
            CMediatorQueItem* commandsItem = CCommandListQueItem::NewL( aNotificationType, aDomain, aCategory, aCommands );
            iNotificationQueue.AddFirst( *commandsItem );
            }
        }
    else if ( iNotificationsRegistered )
        {
        LOG(_L("[Mediator Server]\t CMediatorServerSession::HandleMediatorCommandsChangedL: received commands buffered"));
       
        // store commands for next EWaitForNotifications
        CMediatorQueItem* commandsItem = CCommandListQueItem::NewL( aNotificationType, aDomain, aCategory, aCommands );
        iNotificationQueue.AddLast( *commandsItem );
       }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::DoMediatorCommandsChangedL
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::DoMediatorCommandsChangedL( TMediatorNotificationType aNotificationType, 
                                                         const TUid& aDomain, 
                                                         const TUid& aCategory, 
                                                         const RCommandList& aCommands,
                                                         TBool& aCommandsPassedToClient )
    {
    TRACE( Print( _L("[Mediator Server]\t CMediatorServerSession::DoMediatorCommandsChangedL: D=0x%x, C=0x%x, count=%d" ),
                  aDomain.iUid, aCategory.iUid, aCommands.Count() ) );
    
    // Write category information to message slot 0
    WriteCategoryL( aDomain, aCategory, iNotificationMessage, 0 );
    
    // Write notification type to slot 1
    TMediatorNotificationType notificationType = aNotificationType;
    TNotificationTypeRetBuffer typeBuffer( notificationType );
    iNotificationMessage.WriteL( 1, typeBuffer );
    
    // Write command list to slot 3
    if ( CommandListFitsToClientMemory( aCommands, iNotificationMessage, 3 ) )
        {
        WriteCommandListL( aCommands, iNotificationMessage, 3 );
        aCommandsPassedToClient = ETrue;
        }
    else
        {
        aCommandsPassedToClient = EFalse;
        }
    
    // Complete message
    iNotificationMessage.Complete( aCommands.Count() );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::DoMediatorCategoryRemovedL
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::DoMediatorCategoryRemovedL( const TUid& aDomain, 
                                                         const TUid& aCategory )
    {
    TRACE( Print( _L("[Mediator Server]\t CMediatorServerSession::DoMediatorCategoryRemovedL: D=0x%x, C=0x%x"),
           aDomain.iUid, aCategory.iUid ) );
    
    // Write category information to message slot 0
    WriteCategoryL( aDomain, aCategory, iNotificationMessage, 0 );
        
    // Write notification type to slot 1
    TMediatorNotificationType notificationType = EMediatorCategoryUnregistered;
    TNotificationTypeRetBuffer typeBuffer( notificationType );
    iNotificationMessage.WriteL( 1, typeBuffer );
        
    // Complete message
    iNotificationMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::MediatorEventL
// Callback from event handler when event occurs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::MediatorCategoryRemovedL( TUid aDomain, TUid aCategory )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::MediatorCategoryRemovedL"));
    
    if ( !iNotificationMessage.IsNull() )
        {
        DoMediatorCategoryRemovedL( aDomain, aCategory );
        }
    else if ( iNotificationsRegistered )
        {
        LOG(_L("[Mediator Server]\t CMediatorServerSession::MediatorCategoryRemovedL: buffering"));
            
        // store category removal for next EWaitForNotifications
        CMediatorQueItem* notification = CCategoryQueItem::NewL( aDomain, aCategory );
        iNotificationQueue.AddLast( *notification );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::MediatorEventL
// Callback from event handler when event occurs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::MediatorEventsRemovedL( TUid aDomain, 
                                                     TUid aCategory,
                                                     const REventList& aEvents )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::MediatorEventsRemovedL"));
    
    HandleMediatorEventsChangedL( EMediatorEventsUnregistered, aDomain, aCategory, aEvents );
    }
                                     
// -----------------------------------------------------------------------------
// CMediatorServerSession::MediatorCommandsRemovedL
// Callback from event handler when event occurs
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::MediatorCommandsRemovedL( TUid aDomain, 
                                                       TUid aCategory,
                                                       const RCommandList& aCommands )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::MediatorCommandsRemovedL"));
    
    HandleMediatorCommandsChangedL( EMediatorCommandsUnregistered, aDomain, aCategory, aCommands );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::RegisterEventListL
// Helper method to register event list. Uses Event handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::RegisterEventListL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::RegisterEventListL"));
    
    // Read domain & category
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
    // Read event list
    REventList eventList = ReadEventListL( aMessage, 1 );
    CleanupClosePushL( eventList );
    
    RMediatorDebug::LogData(aMessage, *this, eventList);
    
    // Get handler
    CMediatorServerEventHandler& eventHandler = Server().EventHandler();
    
    // Register events
    eventHandler.RegisterEventListL( category, eventList, aMessage.SecureId() );
    
    // Cleanup
    CleanupStack::PopAndDestroy( &eventList );
    
    // Complete message in case of no errors
    aMessage.Complete( KErrNone );
    
    RMediatorDebug::LogStatus();
    }

    
// -----------------------------------------------------------------------------
// CMediatorServerSession::RegisterCommandListL
// Helper function to register command. Uses Command Handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::RegisterCommandListL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::RegisterCommandListL"));

    // Read domain & category
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
     // Read command list information from Message
    RCommandList commandList = ReadCommandListL( aMessage, 1 );
    CleanupClosePushL( commandList );
    
    RMediatorDebug::LogData(aMessage, *this, commandList);
    
    // Get handler
    CMediatorServerCommandHandler& commandHandler = Server().CommandHandler();
    
    // Register commands
    commandHandler.RegisterCommandListL( category, 
                                         commandList, 
                                         aMessage.SecureId(),
                                         this );
    
    // cleanup
    CleanupStack::PopAndDestroy( &commandList );
    
    // Complete message in case of no errors
    aMessage.Complete( KErrNone );

    RMediatorDebug::LogStatus();
    }
    
// -----------------------------------------------------------------------------
// CMediatorServerSession::UnregisterEventListL
// Helper method to unregister event(s). Uses Event handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::UnregisterEventListL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::UnregisterEventListL"));
    
    // Read domain & category
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
     // Read event list information from Message
    REventList eventList = ReadEventListL( aMessage, 1 );
    CleanupClosePushL( eventList );
    
    RMediatorDebug::LogData(aMessage, *this, eventList);
    
    // Get handler
    CMediatorServerEventHandler& eventHandler = Server().EventHandler();
    
    // Register events
    eventHandler.UnregisterEventListL( category, eventList, aMessage.SecureId() );
    
    // cleanup
    CleanupStack::PopAndDestroy( &eventList );

    // Complete message in case of no errors
    aMessage.Complete( KErrNone );

    RMediatorDebug::LogStatus();
    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::UnregisterCommandListL
// Helper method to unregister command(s). Uses Command handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::UnregisterCommandListL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::UnregisterCommandListL"));
    
    // Read domain & category
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
    // Read command list information from Message
    RCommandList commandList = ReadCommandListL( aMessage, 1 );
    CleanupClosePushL( commandList );
    
    RMediatorDebug::LogData(aMessage, *this, commandList);
    
    // Get handler
    CMediatorServerCommandHandler& commandHandler = Server().CommandHandler();
    
    // Unregister commands
    commandHandler.UnregisterCommandListL( category, commandList, aMessage.SecureId() );
    
    // cleanup
    CleanupStack::PopAndDestroy( &commandList );

    // Complete message in case of no errors
    aMessage.Complete( KErrNone );
    
    RMediatorDebug::LogStatus();    
    }



// -----------------------------------------------------------------------------
// CMediatorServerSession::SubscribeEventL
// Helper method to subscribes to event(s). Uses Event handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::SubscribeEventListL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::SubscribeEventListL"));
  
    // Read domain & category information
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
     // Read event list information from Message
    REventList eventList = ReadEventListL( aMessage, 1 );
    CleanupClosePushL( eventList );
    
    RMediatorDebug::LogData(aMessage, *this, eventList);
    
    // Get handler
    CMediatorServerEventHandler& eventHandler = Server().EventHandler();
    
    // Subscribe to events
    TSecurityInfo messageCaps( aMessage );
    eventHandler.SubscribeEventListL( category, eventList, messageCaps.iCaps, this );
    
    // Cleanup 
    CleanupStack::PopAndDestroy( &eventList );
    
    // Complete message in case of no errors
    aMessage.Complete( KErrNone ); 
    
    RMediatorDebug::LogStatus();
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::UnsubscribeEventL
// Helper method to subscribes to event(s). Uses Event handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::UnsubscribeEventListL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::UnsubscribeEventListL"));
    
    // Read domain & category
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
    // Read event list from slot 1
    REventList eventList = ReadEventListL( aMessage, 1 );
    CleanupClosePushL( eventList );
    
    RMediatorDebug::LogData(aMessage, *this, eventList);
    
    // Get handler
    CMediatorServerEventHandler& eventHandler = Server().EventHandler();
    
    // Unsubscribe to events
    eventHandler.UnsubscribeEventListL( category, eventList, this );
    
    // Cleanup
    CleanupStack::PopAndDestroy( &eventList );
    // Complete message in case of no errors
    aMessage.Complete( KErrNone );

    RMediatorDebug::LogStatus();
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::GetDomainsL
// Helper method return registered events. Uses Object handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::GetDomainsL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::GetDomainsL"));
    
    // Construct array to hold registered domains
    RDomainList domainList;
    CleanupClosePushL( domainList );
    
    // Get handler
    CMediatorServerObjectHandler& objectHandler = Server().ObjectHandler();
    
    // Get the list of events
    objectHandler.GetDomainsL( domainList );

    RMediatorDebug::LogData(aMessage, *this, domainList);
    
    // Write the information back to client
    // 2 is for key information
    TInt arraySize = ( sizeof(TUid) ) * domainList.Count();
    
    // Check that there's enough space before writing the answer
    TInt space = aMessage.GetDesMaxLength(0);   // Slot 0
    if ( ( domainList.Count() > 0 ) && ( arraySize <= space  ) )
        {
        TPtr8 arrayPtr( (TUint8*)&domainList[0], arraySize, arraySize );
        aMessage.WriteL( 0, arrayPtr );
        }
    
    // Complete message in case of no errors with domain count
    aMessage.Complete( domainList.Count() );
    
    // Clean up
    CleanupStack::PopAndDestroy( &domainList );

    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::GetCategoriesL
// Helper method return registered events. Uses Object handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::GetCategoriesL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::GetCategoriesL"));
    
    // Read domain information
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
    // Construct array to hold registered categories
    RCategoryList categoryList;
    CleanupClosePushL( categoryList );
    
    // Get handler
    CMediatorServerObjectHandler& objectHandler = Server().ObjectHandler();
    
    // Get the list of events
    objectHandler.GetCategoriesL( category, categoryList );
    
    RMediatorDebug::LogData(aMessage, *this, categoryList);
    
    // Write the information back to client
    // 2 is for key information
    TInt arraySize = ( sizeof(TUid) ) * categoryList.Count();
    
    // Check that there's enough space before writing the answer
    TInt space = aMessage.GetDesMaxLength(1);   // Slot 1
    if ( ( categoryList.Count() > 0 ) && ( arraySize <= space  ) )
        {
        TPtr8 arrayPtr( (TUint8*)&categoryList[0], arraySize, arraySize );
        aMessage.WriteL( 1, arrayPtr );
        }
    
    // Complete message in case of no errors with category count
    aMessage.Complete( categoryList.Count() );
    
    // Clean up
    CleanupStack::PopAndDestroy( &categoryList );
    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::GetCommandsL
// Helper method return registered commands. Uses Object handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::GetCommandsL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::GetCommandsL"));
    
    // Read domain & category information
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
    // Construct array to hold registered events
    RCommandList commandList;
    CleanupClosePushL( commandList );
    
    // Get command list using Object handler
    CMediatorServerObjectHandler& objectHandler = Server().ObjectHandler();
    objectHandler.GetCommandsL( category, commandList );
   
    RMediatorDebug::LogData(aMessage, *this, commandList);
    
    // Write command list to slot 1
    if ( CommandListFitsToClientMemory( commandList, aMessage, 1 ) )
        {
        WriteCommandListL( commandList, aMessage, 1 );
        }
    // if the list does not fit to memory, the client library makes a new synchronous request
    //  with a large enough write buffer
    // Complete message in case of no errors with command count
    aMessage.Complete( commandList.Count() );
       
     // Clean up
    CleanupStack::PopAndDestroy( &commandList );
    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::GetEventsL
// Helper method return registered events. Uses Object Handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::GetEventsL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::GetEventsL"));
    
    // Read domain & category information
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
    // Construct array to hold registered events
    REventList eventList;
    CleanupClosePushL( eventList );
    
    // Get events using Event handler
    CMediatorServerObjectHandler& objectHandler = Server().ObjectHandler();
    objectHandler.GetEventsL( category, eventList );
    
    RMediatorDebug::LogData(aMessage, *this, eventList);
    
    // Write event list to slot 1
    if ( EventListFitsToClientMemory( eventList, aMessage, 1 ) )
        {
        WriteEventListL( eventList, aMessage, 1 );
        } 
    // if the list does not fit to memory, the client library makes a new synchronous request
    //  with a large enough write buffer
    
    // Complete message in case of no errors with command count
    aMessage.Complete( eventList.Count() );
       
     // Clean up
    CleanupStack::PopAndDestroy( &eventList );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::RaiseEventL
// Helper method to raise events. Uses Event Handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::RaiseEventL( const RMessage2& aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::RaiseEventL"));
    
    // Read domain & category information
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
    
    // Read event information
    TEvent raiseEvent = ReadEventL( aMessage, 1 );
    
    // Then read parameter data from slot 2
    HBufC8* eventData = ReadDataLC( aMessage, 2 );

    RMediatorDebug::LogData(aMessage, *this, raiseEvent);
    RMediatorDebug::LogData(aMessage, *this, *eventData);
    
    // Get handler
    CMediatorServerEventHandler& eventHandler = Server().EventHandler();
    
    // Raise event
    eventHandler.RaiseEventL( category, raiseEvent, *eventData );
    
    // Cleanup
    CleanupStack::PopAndDestroy( eventData );

    // Complete message in case of no errors
    aMessage.Complete( KErrNone );
    
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::IssueCommandL
// Helper function to handle command request. Uses Command Handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::IssueCommandL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::IssueCommandL"));

    // Read domain & category information
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
     
    // Read command information
    TCommand issueCommand = ReadCommandL( aMessage, 1 ); 
    
    // Then read parameter data from slot 2
    HBufC8* commandData = ReadDataLC( aMessage, 2 );
    
    RMediatorDebug::LogData(aMessage, *this, issueCommand);
    RMediatorDebug::LogData(aMessage, *this, *commandData);
    
    // Get handler & issue command
    TSecurityInfo messageCaps( aMessage );
    CMediatorServerCommandHandler& commandHandler = Server().CommandHandler();
    commandHandler.IssueCommandL( category,
                                  issueCommand,
                                  *commandData,
                                  messageCaps.iCaps,
                                  this );
    
    
    // cleanup
    CleanupStack::PopAndDestroy( commandData );

    // Complete message in case of no errors
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::CancelCommandL
// Cancels ongoing command processing. Uses Command Handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::CancelCommandL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::CancelCommandL"));

    // Read domain & category information
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
     
    // Read command information
    TCommand cancelCommand = ReadCommandL( aMessage, 1 ); 
    
    RMediatorDebug::LogData(aMessage, *this, cancelCommand);
    
    // Get handler & cancel command
    CMediatorServerCommandHandler& commandHandler = Server().CommandHandler();
    commandHandler.CancelCommand( category, cancelCommand );
                                   
    // Complete initiators async request with KErrCancel
    if ( iCommandResponseMessage.Handle() )
        {
        
        // Write domain & category information to slot 0
        WriteCategoryL( category.iDomain, 
                        category.iCategory, 
                        iCommandResponseMessage, 
                        0 );
        
        // Write event to slot 1
        TVersion version( 0, 0, 0 );
        WriteCommandL( cancelCommand.iCommandId, 
                       version, 
                       iCommandResponseMessage, 
                       1 );
        
        // Status to slot 2
        TPckg<TInt> statusBuffer( KErrCancel );
        iCommandResponseMessage.WriteL( 2, statusBuffer );
        
        // Write data to slot 3
        WriteDataL( KNullDesC8, iCommandResponseMessage, 2 );
        
        iCommandResponseMessage.Complete( KErrCancel );
        }

    // Complete message in case of no errors
    aMessage.Complete( KErrNone );
    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::IssueResponseL
// Helper function to handle command response. Uses Command Handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::IssueResponseL( const RMessage2 &aMessage )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerSession::IssueResponseL"));

    // Read domain & category information
    TMediatorCategory category = ReadCategoryL( aMessage, 0 );
     
    // Read command information
    TCommand responseCommand = ReadCommandL( aMessage, 1 ); 
    
    // Then read status data from slot 2
    TPckgBuf<TInt> statusBuffer;
    aMessage.ReadL( 2, statusBuffer );
    TInt status = statusBuffer();
    
    // And result data from slot 3
    HBufC8* responseData = ReadDataLC( aMessage, 3 );
    
    RMediatorDebug::LogData(aMessage, *this, responseCommand);
    RMediatorDebug::LogData(aMessage, *this, *responseData);
    
    // Get handler & respond to command
    TSecurityInfo messageCaps( aMessage );
    CMediatorServerCommandHandler& commandHandler = Server().CommandHandler();
    
    TInt err = commandHandler.IssueResponse( category,
                                             responseCommand,
                                             *responseData,
                                             status );
                                             
    if ( err != KErrNone )
        {
        // If command is not found, it means that the command initiator has cancelled the command
        
        // Check the command service list and remove the cancelled command if found, because responder 
        // can interpret the return value as a cancellation and further notifications are thus not needed
        TBool found = EFalse;
        for ( TInt index = 0; index < iCommandServiceList.Count() && !found; index++ )
            {
            CCommand* commandPtr = iCommandServiceList[index];
            if ( commandPtr )
                {
                if ( commandPtr->Domain() == category.iDomain
                    && commandPtr->Category() == category.iCategory
                    && commandPtr->Id() == responseCommand.iCommandId
                    && commandPtr->Status() == ECommandCanceled )
                    {
                    iCommandServiceList.Remove( index );
                    delete commandPtr;
                    commandPtr = NULL;
                    found = ETrue;
                    }
                }
            }
        
        User::Leave( err );
        }
    
    // cleanup
    CleanupStack::PopAndDestroy( responseData );

    // Complete message in case of no errors
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::WaitForEventsL
// Helper method to receive events
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::WaitForEventsL( const RMessage2& aMessage )
    {
    // Check if we have already events to be notified
    if ( iEventServiceList.Count() > 0 )
        {
        CEvent* eventPtr = iEventServiceList[0];    // Take the first event
        CleanupStack::PushL( eventPtr );
        iEventServiceList.Remove(0);
        
        // Write domain & category to slot 0
        WriteCategoryL( eventPtr->Domain(), eventPtr->Category(), aMessage, 0 );
        
        // Write event to slot 1
        WriteEventL( eventPtr->Id(), aMessage, 1 );
        
        // Write data to slot 2
        WriteDataL( eventPtr->ParameterData(), aMessage, 2 );
       
        // Complete message with data amount
        aMessage.Complete( eventPtr->ParameterData().Length() );
        
        //Cleanup
        CleanupStack::PopAndDestroy( eventPtr );
        
        TRACE(Print(_L("[Mediator Server]\t Session %d - event buffer %d\n"), this, iEventServiceList.Count() ));
        }
    else    // Start async waiting
        {
        iEventNotificationMessage = aMessage;
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::WaitForCommandsL
// Helper method to receive commands.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::WaitForCommandsL( const RMessage2& aMessage )
    {
    // Check if we have already commands to be delivered
    if ( iCommandServiceList.Count() > 0 )
        {
        CCommand* commandPtr = iCommandServiceList[0];    // Take the first command
        CleanupStack::PushL( commandPtr );
        iCommandServiceList.Remove(0);
        
        // If it's a pending command it can now be delivered
        if ( commandPtr->Status() == ECommandPending )
            {
            // Write domain & category to slot 0
            WriteCategoryL( commandPtr->Domain(), commandPtr->Category(), aMessage, 0 );
        
            // Write event to slot 1
            WriteCommandL( commandPtr->Id(), commandPtr->Version(), aMessage, 1 );
        
            // Write data to slot 2
            WriteDataL( commandPtr->ParameterData(), aMessage, 2 );
       
            // Complete message with data amount
            aMessage.Complete( commandPtr->ParameterData().Length() );
    
            }
        // If it's a cancelled command then the responder will be notified about cancellation that
        // it was not able to handle before   
        else if ( commandPtr->Status() == ECommandCanceled )
            {
            // Write domain & category to slot 0
            WriteCategoryL( commandPtr->Domain(), commandPtr->Category(), aMessage, 0 );
            
            // Write command id and version to slot 1
            WriteCommandL( commandPtr->Id(), commandPtr->Version(), aMessage, 1 );
            
            // Status to slot 2
            TPckg<TInt> statusBuffer( KErrCancel );
            aMessage.WriteL( 2, statusBuffer );
        
            // Write data to slot 3
            WriteDataL( KNullDesC8, aMessage, 2 );
        
            // Complete message
            aMessage.Complete( KErrCancel );
            }
            
        //Cleanup
        CleanupStack::PopAndDestroy( commandPtr );            
                
        TRACE(Print(_L("[Mediator Server]\t Session %d - command buffer %d\n"), this, iCommandServiceList.Count() ));
        }
    else    // Start async waiting
        {
        iCommandInitMessage = aMessage;
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::WaitForNotificationsL
// Starts async notification waiting. Uses Object Handler
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::WaitForNotificationsL( const RMessage2& aMessage )
    {
    if ( !iNotificationMessage.Handle() )
        {
        if ( !iNotificationsRegistered )
            {
            // Get handler
            CMediatorServerObjectHandler& objectHandler = Server().ObjectHandler();
            objectHandler.AddObserverL( this );
            iNotificationsRegistered = ETrue;
            }
        iNotificationMessage = aMessage;
        
        if ( !iNotificationQueue.IsEmpty() )
            {
            PurgeNextBufferedNotificationL();
            }
        }
    else
        {
        ERROR_LOG(_L("[Mediator] CMediatorServerSession::WaitForNotificationsL: KErrAlreadyExists\n"));
        User::Leave( KErrAlreadyExists );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::CancelNotificationsL
// Stops async notification waiting.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::CancelNotificationsL( const RMessage2& aMessage )
    {
    CleanNotificationQueue();
    
    if ( iNotificationMessage.Handle() )
        {
        iNotificationMessage.Complete( KErrCancel );
        }   
    if ( iNotificationsRegistered )
        {
        // Get handler
        CMediatorServerObjectHandler& objectHandler = Server().ObjectHandler();
        objectHandler.RemoveObserverL( this );
        iNotificationsRegistered = EFalse;
        }
    else
        {
        ERROR_LOG(_L("[Mediator] CMediatorServerSession::CancelNotificationsL: KErrNotFound\n"));
        User::Leave( KErrNotFound );
        }
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::WaitForCommandResponseL
// Async waiting for command responses
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::WaitForCommandResponseL( const RMessage2& aMessage )
    {
    // Just take message to member variable to future use
    if ( !iCommandResponseMessage.Handle() )
        {
        iCommandResponseMessage = aMessage;
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::FetchParameterDataL
// Async waiting for command responses
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::FetchParameterDataL( const RMessage2& aMessage )
    {
    // Write data buffer to message slot 0 and complete with size of the buffer
    if ( iDataBuffer )
        {
        WriteDataL( *iDataBuffer, aMessage, 0 );
        aMessage.Complete( iDataBuffer->Length() );
        delete iDataBuffer;
        iDataBuffer = NULL;
        }
    else
        {
        aMessage.Complete( 0 ); // Just complete with zero sized data
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::FetchNotificationEventListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::FetchNotificationEventListL( const RMessage2& aMessage )
    {
    // Write event list to message slot 0 and complete with count of events in the list
    CMediatorQueItem* eventList = iNotificationQueue.First();
    iNotificationQueue.Remove( *eventList );
    CleanupStack::PushL( eventList );
    const REventList* events( eventList->Events() );
    
    if ( !events )
        {
        // should not ever end up leaving here, as this indicates a
        // problem with queue management i.e. a programming error
        ERROR_LOG( _L("[Mediator] CMediatorServerSession::FetchNotificationEventListL: notif queue corrupted\n") );
        User::Leave( KErrCorrupt );
        }
    
    WriteEventListL( *events, aMessage, 0 );
    aMessage.Complete( events->Count() );
    CleanupStack::PopAndDestroy( eventList );
    }
    
// -----------------------------------------------------------------------------
// CMediatorServerSession::FetchNotificationCommandListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::FetchNotificationCommandListL( const RMessage2& aMessage )
    {
    // Write command list buffer to message slot 0 and complete count of commands in the list
    CMediatorQueItem* commandList = iNotificationQueue.First();
    iNotificationQueue.Remove( *commandList );
    CleanupStack::PushL( commandList );
    const RCommandList* commands( commandList->Commands() );
    
    if ( !commands )
        {
        // should not ever end up leaving here, as this indicates a
        // problem with queue management i.e. a programming error
        ERROR_LOG( _L("[Mediator] CMediatorServerSession::FetchNotificationCommandListL: notif queue corrupted\n") );
        User::Leave( KErrCorrupt );
        }
    
    WriteCommandListL( *commands, aMessage, 0 );
    aMessage.Complete( commands->Count() );
    CleanupStack::PopAndDestroy( commandList );
    }


// -----------------------------------------------------------------------------
// CMediatorServerSession::CancelAll
// Async waiting for command responses
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::CancelAll( const RMessage2& aMessage )
    {
    // Cancel all possible async wait operations
    CancelAll( KErrCancel );
    // Finally complete own message
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CMediatorServerSession::CancelAll
// Async waiting for command responses
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerSession::CancelAll( TInt aError )
    {
    // Cancel all possible async wait operations
    if ( iCommandResponseMessage.Handle() )
        {
        iCommandResponseMessage.Complete( aError );
        }
    if ( iCommandInitMessage.Handle() )
        {
        iCommandInitMessage.Complete( aError );
        }
    if ( iNotificationMessage.Handle() )
        {
        CleanNotificationQueue();
        iNotificationMessage.Complete( aError );
        }
    if ( iEventNotificationMessage.Handle() )
        {
        iEventNotificationMessage.Complete( aError );
        }
    }

// End of File
