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
* Description:  An interface for receiving command/event notifications and info.
*
*/

#ifndef MEDIATORNOTIFICATIONS_H
#define MEDIATORNOTIFICATIONS_H

// INCLUDES
#include <e32base.h>

#include <MediatorServiceDefs.h>

// FORWARD DECLARATIONS
class CMediatorNotificationsBody;

// CLASS DECLARATION

/**
*  An observer interface for receiving command/event notifications.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
class MMediatorNotifications
    {
    public: 

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
        virtual void MediatorEventsAddedL( TUid aDomain, 
                                           TUid aCategory, 
                                           const MediatorService::REventList& aEvents ) = 0;

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
        virtual void MediatorCommandsAddedL( TUid aDomain, 
                                             TUid aCategory,
                                             const MediatorService::RCommandList& aCommands ) = 0;
         
        /**
        * A category has been removed. All subscriptions are cancelled.
        *
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.        
        * @return None.
        */
        virtual void MediatorCategoryRemovedL( TUid aDomain, TUid aCategory ) = 0;

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
        virtual void MediatorEventsRemovedL( TUid aDomain, 
                                             TUid aCategory,
                                             const MediatorService::REventList& aEvents ) = 0;
                                             
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
        virtual void MediatorCommandsRemovedL( TUid aDomain, 
                                               TUid aCategory,
                                               const MediatorService::RCommandList& aCommands ) = 0;
    };


// CLASS DECLARATION

/**
*  A class used for receiving Mediator Service notifications and command/event
*  information.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
class CMediatorNotifications : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @return a new CMediatorNotifications instance
        */        
        IMPORT_C static CMediatorNotifications* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMediatorNotifications();

    public: // Event subscribtion

        /**
        * Register a registration observer. Any changes in the registration 
        * information will be notified via the observer interface.
        * 
        * @since S60 3.1
        * @param aObserver  An observer for Mediator Server.
        * @return TInt  an error value
        */
        IMPORT_C TInt RegisterNotificationObserver( 
            MMediatorNotifications* aObserver );
            
        /**
        * Unregister a registration observer.
        * 
        * @since S60 3.1
        * @return TInt  an error value
        */
        IMPORT_C TInt UnregisterNotificationObserver();

        /**
        * Get a list of registered domains.
        * 
        * @since S60 3.1        
        * @param aDomains The list of registered domains.
        * @return TInt  an error value
        */
        IMPORT_C TInt GetDomains( MediatorService::RDomainList& aDomains );

        /**
        * Get a list of registered categories.
        * 
        * @since S60 3.1
        * @param aDomain     The identifier of the domain.  
        * @param aCategories The list of registered categories for the domain.      
        * @return TInt  an error value
        */
        IMPORT_C TInt GetCategories( TUid aDomain, 
                                     MediatorService::RCategoryList& aCategories );

        /**
        * Get a list of registered events.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of registered events for the category.
        * @return TInt  an error value
        */
        IMPORT_C TInt GetEvents( TUid aDomain, 
                                 TUid aCategory,
                                 MediatorService::REventList& aEvents );

        /**
        * Get a list of registered commands.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aCommands  The list of registered commands for the category.
        * @return TInt  an error value
        */
        IMPORT_C TInt GetCommands( TUid aDomain, 
                                   TUid aCategory,
                                   MediatorService::RCommandList& aCommands );
    
    
        /**
        * C++ default constructor.
        */
        CMediatorNotifications();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private: // Data

        /**
        * Pointer to implementation class
        * Own.
        */
        CMediatorNotificationsBody* iBody;
         
	};

#endif      // MEDIATORNOTIFICATIONS_H   
            
// End of File
