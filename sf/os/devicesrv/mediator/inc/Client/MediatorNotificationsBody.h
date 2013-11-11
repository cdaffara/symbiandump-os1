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

#ifndef MEDIATORNOTIFICATIONSBODY_H
#define MEDIATORNOTIFICATIONSBODY_H

// INCLUDES
#include <e32base.h>

#include "MediatorServerClient.h"
#include "MediatorNotifications.h"
#include "MediatorCommon.h"

// NAMESPACE
using namespace MediatorService;

// CLASS DECLARATION



// CLASS DECLARATION

/**
*  A class used for receiving Mediator Service events.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS(CMediatorNotificationsBody) : public CActive
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @return a new CMediatorNotificationsBody instance
        */        
        static CMediatorNotificationsBody* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMediatorNotificationsBody();

    public: // Event subscribtion

        /**
        * Register a registration observer. Any changes in the registration 
        * information will be notified via the observer interface.
        * 
        * @since S60 3.1
        * @param aObserver  An observer for Mediator Server.
        * @return TInt  an error value.
        */
        TInt RegisterNotificationObserver( 
            MMediatorNotifications* aObserver );
            
        /**
        * Unregister a registration observer.
        * 
        * @since S60 3.1
        * @return TInt  an error value.
        */
        TInt UnregisterNotificationObserver();

        /**
        * Get a list of registered domains.
        * 
        * @since S60 3.1        
        * @param aDomains The list of registered domains.
        * @return TInt  an error value.
        */
        TInt GetDomains( RDomainList& aDomains );

        /**
        * Get a list of registered categories.
        * 
        * @since S60 3.1
        * @param aDomain     The identifier of the domain.  
        * @param aCategories The list of registered categories for the domain.      
        * @return TInt       an error value.
        */
        TInt GetCategories( TUid aDomain, 
                                     RCategoryList& aCategories );

        /**
        * Get a list of registered events.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of registered events for the category.
        * @return TInt      an error value.
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
        * @return TInt      an error value.
        */
        TInt GetCommands( TUid aDomain, 
                                   TUid aCategory,
                                   RCommandList& aCommands );
    
    private: // From CActive
    
        /**
        * From CActive::RunL
        */
        void RunL();
        
        /**
        * From CActive::DoCancel()
        */
        void DoCancel();
        
        /**
        * From CActive::RunError()
        */
        TInt RunError( TInt aError );
    
    private: // New functions
        
        /**
        * Receives notifications from the server
        * 
        * @since S60 3.1
        * @return None.
        */
        void ReceiveNotificationsL();
        
        /**
        * Sets the size of the event list.
        * 
        * @since S60 3.1
        * @param aEventCount new size
        * @return None.
        */
        void ResizeEventListL( TInt aEventCount );
        
        /**
        * Sets the size of the command list.
        * 
        * @since S60 3.1
        * @param aEventCount new size
        * @return None.
        */
        void ResizeCommandListL( TInt aCommandCount );
        
        /**
        * Retrieves syncronously event list from the server
        * after a notification has been received.
        * 
        * @since S60 3.1
        * @return None.
        */
        void FetchEventListL();
        
        /**
        * Retrieves syncronously command list from the server
        * after a notification has been received.
        * 
        * @since S60 3.1
        * @return None.
        */
        void FetchCommandListL();
        
    private:
    
        /**
        * C++ default constructor.
        */
        CMediatorNotificationsBody();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private: // Data

        // A session class to mediator server
        RMediatorServer         iMediatorServer;
        
        // Observer for event callbacks (not owned)
        MMediatorNotifications* iObserver;
        
        // List for notified events
        REventList              iEventList;
        
        // List for notitifer commands
        RCommandList            iCommandList;
        
        // Pointers for return array data
        TPtr8                   iCommandArrayPtr;
        TPtr8                   iEventArrayPtr;
        
        // Domain and category information
        TMediatorCategory           iCategory;
        TMediatorCategoryRetBuffer  iCategoryBuffer;
        
        // Notification type
        TMediatorNotificationType   iNotificationType;
        TNotificationTypeRetBuffer  iNotificationTypeBuffer;
        
        // Stores information whether destructor has been called, checked in RunL
        TBool* iDestroyed;
         
	};

#endif      // MEDIATORNOTIFICATIONSBODY_H   
            
// End of File
