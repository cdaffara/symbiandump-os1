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
* Description:  Session class for Mediator Server
*
*/


#ifndef MEDIATORSERVERSESSION_H
#define MEDIATORSERVERSESSION_H


// INCLUDES
#include <e32base.h>

#include "MediatorCommon.h"
#include "MediatorServerEventHandler.h"
#include "MediatorServerCommandHandler.h"
#include "MediatorServerObjectHandler.h"
#include "MediatorServerObjects.h"


// FORWARD DECLARATIONS
class CMediatorServer;
class CMediatorQueItem;

// CLASS DECLARATION

/**
*  Session class for Mediator Server.
*
*  @lib MediatorServer
*  @since S60 3.1
*/
class CMediatorServerSession : public CSession2, 
                               public MMediatorServerEventObserver,
                               public MMediatorServerNotificationObserver,
                               public MMediatorCommandObserver,
                               public MMediatorCommandResponseObserver
	{
    public: // Construction and destruction
        /**
        * Destructor.
        */
        ~CMediatorServerSession();

        /**
        * Two-phased constructor.
        */
    	static CMediatorServerSession* NewL();

    public: // from CSession2
    
        /**
        * Handles the servicing of a client request that has been passed
        * to the server.
        * @since S60 3.1
        * @param aMessage   message to be served
        * @return None.
        */
    	void ServiceL( const RMessage2& aMessage );
    	
    	/**
        * Reports error thas has occurred during service.
        * 
        * @since S60 3.1
        * @param aMessage   message that was served
        * @param aError     error code of service error
        * @return None.
        */
        
    	void ServiceError(const RMessage2& aMessage,TInt aError);
    
    public: // from MMediatorCommandObserver
    
        /**
        * A Mediator Service command.
        *
        * @since S60 3.1     
        * @param aDomain    The identifier of the domain         
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aVersion   The version information of the command.
        * @param aData      The parameters of the command.
        * @return None.
        */
        void MediatorCommandL( TUid aDomain,
                               TUid aCategory, 
                               TInt aCommandId,
                               TVersion aVersion, 
                               const TDesC8& aData );

        /**
        * Cancel a Mediator Service command.
        *
        * @since S60 3.1        
        * @param aDomain    The identifier of the domain      
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.        
        * @return None.
        */
        void CancelMediatorCommandL( TUid aDomain,
                                     TUid aCategory, 
                                     TInt aCommandId );
                                     
        /**
        * Mediator command timeout has occurred
        *
        * @since S60 3.1        
        * @param aDomain    The identifier of the domain      
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.        
        * @return None.
        */
        void MediatorCommandTimeoutL( TUid aDomain,
                                      TUid aCategory, 
                                      TInt aCommandId );                                      
    public:
    
        /**
        * A response to a Mediator Service command.
        *
        * @since S60 3.1
        * @param aDomain    The identifier of the domain          
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aStatus    Standard Symbian error code indicating the
        *                   success of the command.
        * @param aData      The parameters of the response.
        * @return None.
        */
        void CommandResponseL( TUid aDomain,
                               TUid aCategory, 
                               TInt aCommandId,
                               TInt aStatus, 
                               const TDesC8& aData );                                    
    
    public: // from MMediatorServerEventObserver
        
        /**
        * A Mediator Service event.
        *
        * @since S60 3.1        
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @param aData      The parameters of the event.
        * @return None.
        */
        void MediatorEventL( TUid aDomain,
                             TUid aCategory, 
                             TInt aEventId, 
                             const TDesC8& aData );
                             
    public:  // from MMediatorServerNotificationObserver

       /**
        * Events have been registered. The events can now be subscribed.
        * The list can contain 1-n new events into the category.
        *
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of events that have been registered.
        * @return None.
        */
        void MediatorEventsAddedL( TUid aDomain, 
                                   TUid aCategory, 
                                   const REventList& aEvents );

        /**
        * New commands have been registered. The commands can now be subscribed.
        * The list can contain 1-n new commands into the category.
        *
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommands  List of commands that have been registered.
        * @return None.
        */
        void MediatorCommandsAddedL( TUid aDomain, 
                                     TUid aCategory,
                                     const RCommandList& aCommands );
         
        /**
        * A category has been removed. All subscriptions are cancelled.
        *
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.        
        * @return None.
        */
        void MediatorCategoryRemovedL( TUid aDomain, TUid aCategory );

        /**
        * List of events has been removed. All subscriptions are cancelled.
        * List can contain 1-n events from the specified category.
        *
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of events removed.
        * @return None.
        */
        void MediatorEventsRemovedL( TUid aDomain, 
                                     TUid aCategory,
                                     const REventList& aEvents );
                                             
        /**
        * List of commands have been removed. All subscriptions are cancelled.
        * List can contain 1-n commands from the specified category.
        *
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommands  The list of commands removed
        * @return None.
        */
        void MediatorCommandsRemovedL( TUid aDomain, 
                                       TUid aCategory,
                                       const RCommandList& aCommands );
    private: // new functions
        
        /**
        * Server returns the server reference.
        * @since S60 3.1
        * @param void
        * @return Mediator Server reference
        */      
        CMediatorServer& Server();
        
        /**
        * Reads category (and domain) information from message.
        * @since S60 3.1
        * @param aMessage reference to message
        * @param aMessageSlot where category information can be found
        * @return TMediatorCategory information
        */      
        TMediatorCategory ReadCategoryL( const RMessage2& aMessage,
                                         const TInt aMessageSlot );
                                         
        /**
        * Writes category (and domain) information to message.
        * @since S60 3.1
        * @param aDomain UID of the domain
        * @param aCategory UID of the category
        * @param aMessage reference to target message
        * @param aMessageSlot where information is written
        * @return none
        */      
        void WriteCategoryL( TUid aDomain,
                             TUid aCategory,
                             const RMessage2& aMessage,
                             const TInt aMessageSlot );      
        
        /**
        * Reads event from message
        * @since S60 3.1
        * @param aMessage reference to message
        * @param aMessageSlot where event information can be found
        * @return MediatorService::TEvent information
        */      
        MediatorService::TEvent ReadEventL( const RMessage2& aMessage,
                                            const TInt aMessageSlot );
        
        /**
        * Reads command from message
        * @since S60 3.1
        * @param aMessage reference to message
        * @param aMessageSlot where event information can be found
        * @return MediatorService::TCommand information
        */      
        MediatorService::TCommand ReadCommandL( const RMessage2& aMessage,
                                                const TInt aMessageSlot );
        
        /**
        * Writes event to message.
        * @since S60 3.1
        * @param aEventId id of the event
        * @param aMessage reference to target message
        * @param aMessageSlot wwhere information is written
        * @return none
        */      
        void WriteEventL( TInt aEventId,
                          const RMessage2& aMessage,
                          const TInt aMessageSlot );   

        /**
        * Writes command to message.
        * @since S60 3.1
        * @param aCommandId id of the command
        * @param aVersion version of the command
        * @param aMessage reference to target message
        * @param aMessageSlot wwhere information is written
        * @return none
        */      
        void WriteCommandL( TInt aCommandId,
                            TVersion aVersion,
                            const RMessage2& aMessage,
                            const TInt aMessageSlot );                                 
        
        /**
        * Reads event list from message.
        * @since S60 3.1
        * @param aMessage reference to target message
        * @param aMessageSlot where information is written
        * @return REventList list of read events
        */      
        REventList ReadEventListL( const RMessage2& aMessage,
                                   const TInt aMessageSlot );   
                             
        /**
        * Writes event list to message.
        * @since S60 3.1
        * @param aEvents list of events
        * @param aMessage reference to target message
        * @param aMessageSlot where information is written
        * @return none
        */      
        void WriteEventListL( const REventList& aEvents,
                              const RMessage2& aMessage,
                              const TInt aMessageSlot );   
        
        /**
        * Writes event list to message.
        * @since S60 5.1
        * @param aEvents list of events        
        * @return none
        */      
        void WriteEventListToBufferL( const REventList& aEvents );

        /**
        * Reads command list from message.
        * @since S60 3.1
        * @param aMessage reference to target message
        * @param aMessageSlot where information is written
        * @return RCommandList list of read commands
        */      
        RCommandList ReadCommandListL( const RMessage2& aMessage,
                                       const TInt aMessageSlot );  
                                                         
        /**
        * Writes command list to message.
        * @since S60 3.1
        * @param aEvents list of events
        * @param aMessage reference to target message
        * @param aMessageSlot where information is written
        * @return none
        */      
        void WriteCommandListL( const RCommandList& aCommands,
                                const RMessage2& aMessage,
                                const TInt aMessageSlot );     
                                                              
        /**
        * Reads data from message.
        * Note that leaves the pointer to cleanup stack.
        * @since S60 3.1
        * @param aMessage reference to target message
        * @param aMessageSlot where information is read
        * @return HBufC8 pointer to data buffer
        */      
        HBufC8* ReadDataLC( const RMessage2& aMessage,
                            const TInt aMessageSlot );      
                                               
        /**
        * Writes data to message.
        * @since S60 3.1
        * @param aData reference to data buffer
        * @param aMessage reference to target message
        * @param aMessageSlot where category information can be found
        * @return none
        */      
        void WriteDataL( const TDesC8& aData,
                         const RMessage2& aMessage,
                         const TInt aMessageSlot );                                                                                                       
                                         
                                                 
        
        /**
        * Dispatches incoming client-server request
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void DispatchMessageL( const RMessage2& aMessage );
        
        /**
        * Registers list of events to Event Handler
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void RegisterEventListL( const RMessage2& aMessage );
        
        /**
        * Registers list of commands to Command Handler
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void RegisterCommandListL( const RMessage2& aMessage );
        
        /**
        * Unregisters event(s)
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void UnregisterEventListL( const RMessage2& aMessage );
        
         /**
        * Unregisters command(s)
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void UnregisterCommandListL( const RMessage2& aMessage );
        
        /**
        * Subscribes to event(s)
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void SubscribeEventListL( const RMessage2& aMessage );
        
        
        /**
        * Ubsubscribes to event(s)
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void UnsubscribeEventListL( const RMessage2& aMessage );
        
        /**
        * Handles incoming event notification request
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void RaiseEventL( const RMessage2& aMessage );
        
        /**
        * Handles incoming command request
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void IssueCommandL( const RMessage2& aMessage );
        
        /**
        * Cancels ongoing command process
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void CancelCommandL( const RMessage2& aMessage );
        
        /**
        * Handles incoming command response
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void IssueResponseL( const RMessage2& aMessage );
        
        /**
        * Gets registered domains
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void GetDomainsL( const RMessage2& aMessage );
        
        /**
        * Gets registered categories
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void GetCategoriesL( const RMessage2& aMessage );
        
        /**
        * Gets registered commands
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void GetCommandsL( const RMessage2& aMessage );
        
        /**
        * Gets registered events
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void GetEventsL( const RMessage2& aMessage );
     
    private:    // For async notifications
    
        /**
        * Turn on async waiting for events
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void WaitForEventsL( const RMessage2& aMessage );
        
        /**
        * Turn on async waiting for commands
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void WaitForCommandsL( const RMessage2& aMessage );
    
        /**
        * Turn on async waiting for events/command registrations changes
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void WaitForNotificationsL( const RMessage2& aMessage );
        
        /**
        * Turn off async waiting for events/command registrations changes
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void CancelNotificationsL( const RMessage2& aMessage );
        
        /**
        * Turn on async waiting for command responses
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void WaitForCommandResponseL( const RMessage2& aMessage );
        
        /**
        * Fetch method to get full amount of parameter data
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void FetchParameterDataL( const RMessage2& aMessage );
        
        /**
        * Fetch method to get full event list
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void FetchNotificationEventListL( const RMessage2& aMessage );
        
        /**
        * Fetch method to get full command list
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void FetchNotificationCommandListL( const RMessage2& aMessage );
        
        /**
        * Cancels all async operations
        * 
        * @since S60 3.1
        * @param aMessage message to be handled
        * @return None.
        */
        void CancelAll( const RMessage2& aMessage );
        
        /**
        * Cancels all async operations
        * 
        * @since S60 3.1
        * @param aError error code causing cancelation
        * @return None.
        */
        void CancelAll( TInt aError );
        
        /**
        * Returns whether event list can be passed with provided client message.
        */
        TBool EventListFitsToClientMemory( const REventList& aEvents,
                                           const RMessage2& aMessage,
                                           const TInt aMessageSlot );
        
        /**
        * Returns whether command list can be passed with provided client message.
        */
        TBool CommandListFitsToClientMemory( const RCommandList& aCommands,
                                             const RMessage2& aMessage,
                                             const TInt aMessageSlot );
        
        
        /**
        * Handles an event registration status change (events either added or removed). 
        * Events are buffered to nofication queue if they cannot be passed to the client
        * with the current request.  
        */
        void HandleMediatorEventsChangedL( TMediatorNotificationType aNotificationType,
                                           const TUid& aDomain, 
                                           const TUid& aCategory, 
                                           const REventList& aEvents );
        
        /**
        * Completes the current notification request due to events registration status change.
        * 
        * @param[out] aEventsPassedToClient if ETrue, the provided event list was passed to client
        *                                   with the current request.
        */
        void DoMediatorEventsChangedL( TMediatorNotificationType aNotificationType, 
                                       const TUid& aDomain, 
                                       const TUid& aCategory, 
                                       const REventList& aEvents,
                                       TBool& aEventsPassedToClient );
        
        /**
        * Completes the current notification request due to removal of a category.
        */
        void DoMediatorCategoryRemovedL( const TUid& aDomain, const TUid& aCategory );
        
        /**
        * Handles a command registrations status change (commands either added or removed). 
        * Commands are buffered to nofication queue if they cannot be passed to 
        * the client with the current request.  
        */
        void HandleMediatorCommandsChangedL( TMediatorNotificationType aNotificationType,
                                                 const TUid& aDomain, 
                                                 const TUid& aCategory, 
                                                 const RCommandList& aCommands );
        
        /**
        * Completes the current notification request due to commands registration status change.
        * 
        * @param[out] aCommandsPassedToClient if ETrue, the provided command list was passed to client
        *                                   with the current request.
        */
        void DoMediatorCommandsChangedL( TMediatorNotificationType aNotificationType, 
                                         const TUid& aDomain, 
                                         const TUid& aCategory, 
                                         const RCommandList& aCommands,
                                         TBool& aCommandsPassedToClient );

        /**
        * Empties the notification queue.
        */
        void CleanNotificationQueue();
        
        /**
        * Completes the current notification request with the next notification in queue,
        * if any.
        */
        void PurgeNextBufferedNotificationL();
        
    private:
    
    	/**
        * Constructor.
        */
        CMediatorServerSession();
    
    
    private:    // data
        
        friend class RMediatorDebug;
        // For async event notification
        RMessage2       iEventNotificationMessage;
        
        // For async notification
        RMessage2       iNotificationMessage;
        
        // For async command initiations
        RMessage2       iCommandInitMessage;
        
        // For async command responses
        RMessage2       iCommandResponseMessage;
        
        // Status if notifications are registered
        TBool           iNotificationsRegistered;
        
        // Event service buffer
        RPointerArray<CEvent>  iEventServiceList;
        
        // Command service buffer
        RPointerArray<CCommand>  iCommandServiceList;
        
        /**
        * Reserve buffer for big amount of data
        * Own.
        */
        HBufC8* iDataBuffer;
        
        /**
        * Queue for buffering notifications. 
        */
        TSglQue<CMediatorQueItem> iNotificationQueue;
    
	};


#endif  // MEDIATORSERVERSESSION_H

// End of File
