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
* Description:  Handler Mediator objects (domains, categories and 
*                commands/events)
*
*/

#ifndef MEDIATORSERVEROBJECTHANDLER_H
#define MEDIATORSERVEROBJECTHANDLER_H

// INCLUDES
#include <e32base.h>
#include "MediatorCommon.h"
#include "MediatorServerObjects.h"
#include "MediatorServiceDefs.h"

// FORWARD DECLARATIONS
class CDomain;
class CCategory;
class MMediatorCommandObserver;
class MMediatorServerEventObserver;

// NAMESPACE
using namespace MediatorService;


// CLASS DECLARATION

/**
*  An observer interface for receiving Mediator Service notifications.
*
*  @since S60 3.1
*/
class MMediatorServerNotificationObserver
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
                                           const REventList& aEvents ) = 0;

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
                                             const RCommandList& aCommands ) = 0;
         
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
                                             const REventList& aEvents ) = 0;
                                             
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
                                               const RCommandList& aCommands ) = 0;
    };


/**
*  Object handler class for Mediator Server.
*
*  @lib MediatorServer
*  @since S60 3.1
*/
class CMediatorServerObjectHandler : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMediatorServerObjectHandler* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMediatorServerObjectHandler();

    public: // New functions
        
        /**
        * Adds new observer to get events about registration changes
        *
        * @since S60 3.1
        * @param aObserver pointer to observer instance
        * @return None.
        */
        void AddObserverL( MMediatorServerNotificationObserver* aObserver );
        
        /**
        * Removes observer.
        *
        * @since S60 3.1
        * @param aObserver pointer to observer instance
        * @return None.
        */
        void RemoveObserverL( MMediatorServerNotificationObserver* aObserver );
        
        /**
        * Finds domain from the domain list
        * @since S60 3.1
        * @param aDomain identifying the domain
        * @return CDomain pointer to found domain. Null if not found
        */      
        CDomain* FindDomain( const TUid aDomain );
        
        /**
        * Adds new domain to domain list
        * @since S60 3.1
        * @param aDomain identifying the domain
        * @return CDomain pointer to new domain.
        */      
        CDomain* AddDomainL( const TUid aDomain );
        
        /**
        * Returns pointer to category specified
        * @since S60 3.1
        * @param aCategory structure defining domain and category
        * @return CCategory pointer to category
        */      
        CCategory* CategoryL( TMediatorCategory aCategory );
        
        
        /**
        * Gets all domains
        * @since S60 3.1
        * @param aDomains   Array where list of registered domains is written
        * @return none.
        */
        void GetDomainsL( RDomainList& aDomains );  
        
        /**
        * Gets all categories from the domains
        * @since S60 3.1
        * @param aCategory      Information for the domain
        * @param aCategories    Array for the registered categories
        * @return none.
        */
        void GetCategoriesL( TMediatorCategory aCategory, 
                             RCategoryList& aCategories );     
        
        /**
        * Gets registered commands from the category
        * @since S60 3.1
        * @param aCategory  Information for the domain & category
        * @param aCommands  Array where list of registered commands is written
        * @return none.
        */

        void GetCommandsL( TMediatorCategory aCategory, 
                           RCommandList& aCommands );    
        
        /**
        * Gets registered events from the category
        * @since S60 3.1
        * @param aCategory  Information for the domain & category
        * @param aEvents    Array where list of registered events is written
        * @return none.
        */

        void GetEventsL( TMediatorCategory aCategory, 
                         REventList& aEvents );     
                         
                         
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
        void EventsAdded( TUid aDomain, 
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
        void CommandsAdded( TUid aDomain, 
                            TUid aCategory,
                            const RCommandList& aCommands );

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
        void EventsRemoved( TUid aDomain, 
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
        void CommandsRemoved( TUid aDomain, 
                              TUid aCategory,
                              const RCommandList& aCommands );
                              
        /**
        * Clears all registrations made by this observer
        *
        * @since S60 3.1
        * @param aEventObserver     Pointer to event observer.
        * @param aCommandObserver   Pointer to command observer.
        * @param aNotifObserver     Pointer to notifications observer.
        * @return None.
        */
        void ClearRegistrations( MMediatorServerEventObserver* aEventObserver, 
                                 MMediatorCommandObserver* aCommandObserver,
                                 MMediatorServerNotificationObserver* aNotifObserver );                              
        
        
        /**
        * Gets statistical information about owned objects..
        *
        * @since S60 3.1
        * @param aDomainCount     domain count (OUT)
        * @param aCategoryCount   category count (OUT)
        * @param aEventCount      count of events in all categories (OUT)
        * @param aCommandCount    count of commands in all categories (OUT)
        * @param aObserverCount   observer count (OUT)
        * @return None.
        */
        void GetStatistics(TInt& aDomainCount, 
                           TInt& aCategoryCount,
                           TInt& aEventCount,
                           TInt& aCommandCount,
                           TInt& aObserverCount);
                              
    private:
        
         /**
        * Compares two domains according to their UIDs. User in array finds.
        * @since S60 3.1
        * @param aLeftDomain reference to left domain
        * @param aRightdomain reference to right domain
        * @return TBool indicating if items are same or not.
        */      
        static TBool CompareDomains( const CDomain& aLeftDomain, 
                                     const CDomain& aRightDomain );
                               
        /**
        * Checks whether category has any events and commands
        * @since S60 3.1
        * @param aDomain UID for the domain
        * @param aCategory UID for the category
        * @return TBool ETrue if empty, EFalse if not
        */      
        TBool IsCategoryEmptyL( TUid aDomain, TUid aCategory);
        
        /**
        * Removes category if it was empty
        * @since S60 3.1
        * @param aDomain UID for the domain
        * @param aCategory UID for the category
        * @return TBool ETrue if removed, EFalse if not
        */      
        TBool RemoveCategoryIfEmptyL( TUid aDomain, TUid aCategory);
        
    private:

        /**
        * C++ default constructor.
        */
        CMediatorServerObjectHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        
        /**
        * List of domains (includes categories and then commands and events  
        */

        RPointerArray<CDomain>  iDomainList;
        
        /**
        * List of registration change observers
        */
        RPointerArray<MMediatorServerNotificationObserver> iObserverList;
        
        /**
        * A helper object for searches. Does not manage any other data than UID.
        * Own.
        */
        CDomain* iSearchDomain;
         
    };

#endif      // MEDIATORSERVEROBJECTHANDLER_H  
            
// End of File
