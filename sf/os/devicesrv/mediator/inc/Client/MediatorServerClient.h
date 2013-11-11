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
* Description:  A client interface for Mediator Server.
*
*/

#ifndef MEDIATORSERVERSESSION_H
#define MEDIATORSERVERSESSION_H


// INCLUDES
#include <e32base.h>
#include "MediatorCommon.h"
#include "MediatorServiceDefs.h"

// NAMESPACE
using namespace MediatorService;


/**
*  Client API for Mediator Server.
*
*  The Mediator Server is used to pass information between clients. The 
*  services of Mediator Server include passing events and passing commands with
*  respective responses. For events there can be multiple event providers and 
*  consumers. For commands there can be only one initiator and one responder.
*
*  The operations of Mediator server have the following hierarchy.
*  Domain -> Category -> Event/Command -> Data
*
*  The domain means the application domain, the purpose, for which the services 
*  of Mediator Server are used.
*
*  The category means the functionality area within the domain to which an event
*  or a command belongs to.
*
*  An event or a command is a message passed to Mediator server. Each message
*  may have data assosiated with it. For each command exactly one response shall 
*  be sent.
*
*  Before any messages can be sent to Mediator Server, the messages need to be
*  registered. When registering messages, the capabilities required to consume 
*  that message are defined. In case of commands timeout values for responses
*  are also defined.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/

////////////////////////////////////////
//	class RMediatorServerSession
////////////////////////////////////////


NONSHARABLE_CLASS(RMediatorServer) : public RSessionBase
	{
    public:
    
        /**
        * Session constructor.
        */
	    RMediatorServer();
	
    public:
        
        /**
        * Creates a connection to server. If server is not started,
        * method will start the server first and then create a new session.
        * 
        * @since S60 3.1
        * @param none
        * @return TInt indicating the success of the call.
        */
        TInt Connect();
        
        /**
        * Closes the session to server
        * 
        * @since S60 3.1
        * @param none
        * @return None.
        */
        void Close();
    
        /**
        * Cancels all ongoing async operations
        * 
        * @since S60 3.1
        * @return None
        */                  
        void Cancel();
        

    public: // Mediator Service Registration API        

        /**
        * Registers an event list. After registering the list 
        * clients can provide and consume the events.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    List of events to be registered
        * @return TInt      an error value.
        */
        TInt RegisterEventList( TUid aDomain,  
                                TUid aCategory, 
                                const REventList& aEvents );

        /**
        * Register a command list. After registering the list 
        * clients can initiate and respond the commands.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommands  List of commands to be registered
        * @return TInt      an error value.
        */
        TInt RegisterCommandList( TUid aDomain, 
                                  TUid aCategory, 
                                  const RCommandList& aCommands );

        /**
        * Unregister an event list. After unregistering no clients can consume
        * or provide these events.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    List of events to be unregistered.
        * @return TInt      an error value.
        */
        TInt UnregisterEventList( TUid aDomain,  
                                  TUid aCategory,
                                  const REventList& aEvents );


        /**
        * Unregister a command list. After unregistering no clients can initiate
        * or respond to these events.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommands  List of commands to be unregistered.
        * @return TInt      an error value.
        */
        TInt UnregisterCommandList( TUid aDomain, 
                                    TUid aCategory,
                                    const RCommandList& aCommands );
        

    public: // Mediator Service Notification API - Event Provider   

        /**
        * Raise an event.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @param aVersion   The version of the event.
        * @param aData      The parameters of the event.
        * @return TInt      an error value.
        */
        TInt RaiseEvent( TUid aDomain, 
                         TUid aCategory, 
                         TInt aEventId,
                         TVersion aVersion, 
                         const TDesC8& aData );

    public: // Mediator Service Notification API - Event Consumer

        /**
        * Subscribes a list of events. Raised events will be
        * delivered via the observer interface.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    List of the events to be subscribed.
        * @return TInt      an error value.
        */
        TInt SubscribeEventList( TUid aDomain, 
                                 TUid aCategory, 
                                 const REventList& aEvents );

        /**
        * Unsubscribe an event list.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of events.
        * @return TInt      an error value.
        */
        TInt UnsubscribeEventList( TUid aDomain, 
                                   TUid aCategory, 
                                   const REventList& aEvents );

        /**
        * Unsubscribe an event.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @return TInt      an error value.
        */
        TInt UnsubscribeEvent( TUid aDomain, TUid aCategory, TInt aEventId );
        
        /**
        * Start to receive events from Mediator Server
        * 
        * @since S60 3.1
        * @param aStatus            TRequestStatus of the AO to be completed when event occurs.
        * @param aCategoryBuffer    Category buffer.
        * @param aEventData         Pointer descriptor to buffer where event data is written.
        * @return TInt indicating the status of the operation
        */
        void ReceiveEvents( TRequestStatus& aStatus, 
                            TMediatorCategoryRetBuffer& aCategoryBuffer,
                            TEventRetBuffer& aEventBuffer,
                            TPtr8& aEventData );


    public: // Mediator Service Command API - Command Initiatior

        /**
        * Issue a command.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aVersion   The version of the command.
        * @param aData      The parameters of the command.        
        * @return TInt indicating the success of the call.
        */
        TInt IssueCommand( TUid aDomain,
                           TUid aCategory, 
                           TInt aCommandId,
                           TVersion aVersion, 
                           const TDesC8& aData );
        
        /**
        * Wait for command response.
        * 
        * @since S60 3.1
        * @param aStatus            TRequestStatus of the AO to be completed when command response is received.
        * @param aCategoryBuffer    Category buffer.
        * @param aCommandBuffer     Command buffer
        * @param aCommandData       Pointer descriptor to buffer where command data is written.
        * @param aStatusBuffer      Status buffer.
        * @return None.
        */                  
        void WaitForCommandResponse( TRequestStatus& aStatus,
                                     TMediatorCategoryRetBuffer& aCategoryBuffer,
                                     TCommandRetBuffer& aCommandBuffer,
                                     TPtr8& aCommandData,
                                     TPckg<TInt>& aStatusBuffer );
        
        /**
        * Cancel a command.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.        
        * @return TInt indicating the status of the call.
        */
        TInt CancelCommand( TUid aDomain, TUid aCategory, TInt aCommandId );

    public: // Mediator Service Command API - Command Responder


        /**
        * Issue a response to a command.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aStatus    Processing status of the command
        * @param aData      The parameters of the response.        
        * @return TInt indicating the status of the operation.
        */
        TInt IssueResponse( TUid aDomain, 
                            TUid aCategory, 
                            TInt aCommandId,
                            TInt aStatus,
                            const TDesC8& aData );
        
        /**
        * Get command requests.
        * 
        * @since S60 3.1
        * @param aCategoryBuffer    Category buffer.
        * @param aCommandBuffer     Command buffer
        * @param aCommandData       Pointer descriptor to buffer where command data is written.
        * @return TInt indicating the status of the operation.
        */                  
        void ReceiveCommands( TRequestStatus& aStatus,
                              TMediatorCategoryRetBuffer& aCategoryBuffer,
                              TCommandRetBuffer& aCommandBuffer,
                              TPtr8& aCommandData );
        
    public: // Mediator Service  - Notifications
    
        /**
        * Get registered domains
        * 
        * @since S60 3.1
        * @param aDomains   The list of registered domains.
        * @return TInt indicating the success of the call.
        */
        TInt GetDomains( RDomainList& aDomains );
        
        /**
        * Get registered categories for the domain
        * 
        * @since S60 3.1
        * @param aDomain        UID for the domain.
        * @param aCategories    List of registered categories within the domain.
        * @return TInt indicating the success of the call.
        */
        TInt GetCategories( TUid aDomain, RCategoryList& aCategories );
        
        /**
        * Get a list of registered events.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of registered events for the category.
        * @return TInt indicating the status of the operation.
        */
        TInt GetEvents( TUid aDomain, 
                        TUid aCategory,
                        REventList& aEvents );

        /**
        * Get a list of registered commands.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommands  The list of registered commands for the category.
        * @return TInt indicating the status of the operation.
        */
        TInt GetCommands( TUid aDomain, 
                          TUid aCategory,
                          RCommandList& aCommands );
        
        /**
        * Get a list of registered commands.
        * 
        * @since S60 3.1
        * @param aStatus            TRequestStatus of the operation to be completed when notification is received.
        * @param aCategoryBuffer    Category buffer.
        * @param aTypeBuffer        Type buffer
        * @param aEventArrayPtr     Pointer descriptor to buffer where event data is written.
        * @param aCommandArrayPtr   Pointer descriptor to buffer where command data is written.
        * @return None.
        */                  
        void ReceiveNotifications( TRequestStatus& aStatus,
                                   TMediatorCategoryRetBuffer& aCategoryBuffer,
                                   TNotificationTypeRetBuffer& aTypeBuffer,
                                   TPtr8& aEventArrayPtr, 
                                   TPtr8& aCommandArrayPtr );
        
        /**
        * Get a list of registered commands.
        * 
        * @since S60 3.1
        * @return TInt indicating the status of the operation.
        */                  
        TInt CancelNotifications();
    
    public: // Mediator Service  - Common
    
        /**
        * Fetch parameter data, if data size was larger than default boundary.
        * 
        * @since S60 3.1
        * @param aParameterData pointer to data buffer where Mediator writes data
        * @return TInt indicating the status of the operation.
        */                  
        TInt FetchParameterData( TPtr8& aParameterData );
        
        /**
        * Fetch event list, if list size was larger than default boundary.
        * 
        * @since S60 3.1
        * @param aEventArrayPtr pointer to data buffer where Mediator writes data
        * @return TInt indicating the status of the operation.
        */  
        TInt FetchNotificationEventList( TPtr8& aEventArrayPtr );
        
        /**
        * Fetch command list, if list size was larger than default boundary.
        * 
        * @since S60 3.1
        * @param aCommandArrayPtr pointer to data buffer where Mediator writes data
        * @return TInt indicating the status of the operation.
        */  
        TInt FetchNotificationCommandList( TPtr8& aCommandArrayPtr );
                
    private:
    
        /**
        * Gets server version information.
        * @since S60 3.1
        * @param None
        * @return Version structure
        */
	    TVersion Version() const;
	    
        /**
        * Server start function
        *
        * @since S60 3.1
        * @param None
        * @return KErrNone          server process was succesfully started
        *         KErrAlreadyExists server process was already running
        *         TInt              other system wide error code
        */
        TInt StartServer();
	    
    private: // Data        
         
	};


#endif  // MEDIATORSERVERSESSION_H

// End of file
