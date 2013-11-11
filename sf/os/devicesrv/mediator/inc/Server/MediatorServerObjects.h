/*
* Copyright (c) 2005 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Domain, category, event and command definitions
*
*/

#ifndef MEDIATORSERVEROBJECTS_H
#define MEDIATORSERVEROBJECTS_H


// INCLUDES
#include    <e32base.h>

#include    "MediatorCommon.h"
#include    "MediatorServiceDefs.h"
#include    "MediatorCommandTimer.h"

// NAMESPACE
using namespace MediatorService;

// CONSTANTS

// FORWARD DECLARATIONS
class CDomain;
class CCategory;
class CItem;
class CEvent;
class CCommand;
class MMediatorServerEventObserver;

// DATA TYPES

typedef CArrayFixFlat<CDomain>      CDomainList;
typedef CArrayFixFlat<CCategory>    CCategoryList;
typedef CArrayFixFlat<CItem>        CItemList;

// CLASS DECLARATION

/**
*  An observer interface for receiving Mediator Service events.
*
*  @since S60 3.1
*/
class MMediatorServerEventObserver
    {
    public: // Event receiving

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
        virtual void MediatorEventL( TUid aDomain,
                                     TUid aCategory, 
                                     TInt aEventId, 
                                     const TDesC8& aData ) = 0;
    };


/**
*  An observer interface for receiving Mediator Service commands.
*
*  @since S60 3.1
*/
class MMediatorCommandObserver
    {
    public: // Command handling

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
        virtual void MediatorCommandL( TUid aDomain,
                                       TUid aCategory, 
                                       TInt aCommandId,
                                       TVersion aVersion, 
                                       const TDesC8& aData ) = 0;

        /**
        * Cancel a Mediator Service command.
        *
        * @since S60 3.1        
        * @param aDomain    The identifier of the domain      
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.        
        * @return None.
        */
        virtual void CancelMediatorCommandL( TUid aDomain,
                                             TUid aCategory, 
                                             TInt aCommandId ) = 0;
                                             
        /**
        * Mediator command timeout has occurred
        *
        * @since S60 3.1        
        * @param aDomain    The identifier of the domain      
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.        
        * @return None.
        */
        virtual void MediatorCommandTimeoutL( TUid aDomain,
                                              TUid aCategory, 
                                              TInt aCommandId ) = 0;                                             
    };

/**
*  An observer interface for receiving responses to Mediator Service commands.
*
*  @since S60 3.1
*/
class MMediatorCommandResponseObserver
    {
    public: // Receiving responses

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
        virtual void CommandResponseL( TUid aDomain,
                                       TUid aCategory, 
                                       TInt aCommandId,
                                       TInt aStatus, 
                                       const TDesC8& aData ) = 0;
    };

/**
*  Class to contain domain information
*
*  @since S60 3.1
*/
class CDomain : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @param aDomain identifier of the domain
        * @return a new CDomain instance
        */
        static CDomain* NewL( TUid aDomain );
        
        /**
        * Destructor.
        */
        virtual ~CDomain();
    
    public: // New functions
        
        /**
        * Getter for Domain UId
        * @since S60 3.1
        * @param void
        * @return TUid indicating the UID of the domain
        */      
        TUid DomainUid() const;
        
        /**
        * Setter for Domain UId
        * @since S60 3.1
        * @param aDomain identifier of the domain
        * @return None.
        */   
        void SetDomainUid( TUid aDomain );
        
        /**
        * Adds category to domain
        * @since S60 3.1
        * @param aNewCategory pointer to new category
        * @return None.
        */      
        void AddCategoryL( const CCategory* aNewCategory );
        
        /**
        * Creates category according to UID and adds it to domain.
        * @since S60 3.1
        * @param aCategory UID for the new category
        * @return CCategory pointer to new category.
        */      
        CCategory* AddCategoryL( const TUid aCategory );
        
        /**
        * Removes category specified by UID
        *
        * @since S60 3.1
        * @param aCategory UID for the category to be removed.
        * @return TInt indicating the success of the call.
        */      
        TInt RemoveCategory( const TUid aCategory );
        
        /**
        * Gets reference to category specified by UID
        * @since S60 3.1
        * @param aCategory  UID of the category
        * @return CCategory pointer to category. NULL if not found.
        */      
        const CCategory* GetCategory( const TUid aCategory );
        
        /**
        * Gets pointer to category specified by index
        * @since S60 3.1
        * @param aIndex index to category array
        * @return CCategory pointer to category. NULL if not existing.
        */      
        CCategory* GetCategory( TInt aIndex );
        
        /**
        * Returns count of categories within the domain.
        * @since S60 3.1
        * @param none
        * @return TInt category count
        */      
        TInt CategoryCount();
    
        /**
        * Finds category from the category list
        * @since S60 3.1
        * @param aCategory identifying the category
        * @param aIndex reference to TInt where index is updated.
        * @return CCategory pointer to found category. NULL if not found.
        */      
        CCategory* FindCategory( const CCategory* aCategory, TInt& aIndex );
        
        /**
        * Finds categorty from the category list.
        * @since S60 3.1
        * @param aCategory identifying the category
        * @param aIndex reference to TInt where index is updated.
        * @return CCategory pointer to found category. NULL if not found.
        */      
        CCategory* FindCategory( const TUid aCategory, TInt& aIndex );
        
        /**
        * Gets all categories within domain
        * @since S60 3.1
        * @param aCategories list to registered categories
        * @return none
        */      
        void GetCategoriesL( RCategoryList& aCategories );
        
    private: // New functions    
        /**
        * Compares two categories according to their UIDs. User in array finds.
        * @since S60 3.1
        * @param aLeftCategory  reference to left category
        * @param aRightCategory reference to right category
        * @return TBool indicating if items are same or not.
        */      
        static TBool CompareCategories( const CCategory& aLeftCategory, 
                                 const CCategory& aRightCategory );
    
    private:
        /**
        * C++ default constructor.
        */
        CDomain();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aDomain identifier of the domain
        */
        void ConstructL( TUid aDomain );
    
    private:

        /**
        * UID of the domain
        */
        TUid iDomain;

        /**
        * List of categories
        */
        RPointerArray<CCategory> iCategoryList;
        
        /**
        * A helper object for searches.
        * Own.
        */
        CCategory* iSearchCategory;
    };

/**
*  Class to contain category information
*
*  @since S60 3.1
*/
class CCategory : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aCategory identifier of the category
        * @return a new CCategory instance
        */
        static CCategory* NewL( TUid aCategory );
        
        /**
        * Destructor.
        */
        virtual ~CCategory();
    
    public: // New functions
        
        /**
        * Getter for Category UId
        * @since S60 3.1
        * @param void
        * @return TUid indicating the UID of the category
        */      
        TUid CategoryUid() const;
        
        /**
        * Setter for Category UId
        * @since S60 3.1
        * @param aCategory identifier of the category
        * @return None
        */      
        void SetCategoryUid( TUid aCategory );
        
        /**
        * Add new item to category
        * @since S60 3.1
        * @param aNewItem pointer to new item
        * @return TInt indicating the success of the operation
        */      
        TInt AddEvent( const CEvent* aNewEvent );
        
        /**
        * Add new command to category
        * @since S60 3.1
        * @param aNewCommand pointer to new item
        * @return TInt indicating the success of the operation
        */      
        TInt AddCommand( const CCommand* aNewCommand );
        
        /**
        * Removes event from the category specified by index
        * @since S60 3.1
        * @param aIndex specifying the index of event
        * @return none
        */      
        void RemoveEvent( TInt aIndex );
        
        /**
        * Removes command from the category specified by index
        * @since S60 3.1
        * @param aIndex specifying the index of command
        * @return none
        */      
        void RemoveCommand( TInt aIndex );
    
        /**
        * Returns pointer to event specified by event id
        * @since S60 3.1
        * @param aId    identifying the event
        * @param aIndex index of the found item.
        * @return CEvent pointer to found event. NULL if not existing.
        */      
        CEvent* FindEvent( const TInt aId, TInt& aIndex );
        
        /**
        * Returns pointer to command specified by command id
        * @since S60 3.1
        * @param aId    identifying the command
        * @param aIndex index of the found item.
        * @return CCommand pointer to found command. NULL if not existing.
        */      
        CCommand* FindCommand( const TInt aId, TInt& aIndex );
        
        /**
        * Returns pointer to event specified by index
        * @since S60 3.1
        * @param aIndex index of the event.
        * @return CEvent pointer to event. NULL if not existing.
        */      
        CEvent* GetEvent( TInt aIndex );
        
        /**
        * Returns pointer to command specified by index
        * @since S60 3.1
        * @param aIndex index of the command.
        * @return CCommand pointer to command. NULL if not existing.
        */      
        CCommand* GetCommand( TInt aIndex );
              
        /**
        * Updates event list to array.
        * @since S60 3.1
        * @param aEvents list to registered events
        * @return none
        */      
        void GetEventsL( REventList& aEvents );
        
        /**
        * Updates command list to array.
        * @since S60 3.1
        * @param aCommands list to registered commands
        * @return none
        */      
        void GetCommandsL( RCommandList& aCommands );
        
        /**
        * Gets event count
        * @since S60 3.1
        * @return TInt count of events
        */      
        TInt EventCount();
        
        /**
        * Gets command count
        * @since S60 3.1
        * @return TInt count of commands
        */      
        TInt CommandCount();
        
        /**
        * Commits the changes made to the command list.
        * @since S60 3.1
        * @param None.
        * @return None.
        */
        void CommitCommands();
        
        /**
        * Rolls back the changes made to the command list.
        * @since S60 3.1
        * @param None.
        * @return None.
        */
        void RollbackCommands();
        
        /**
        * Commits the changes made to the event list.
        * @since S60 3.1
        * @param None.
        * @return None.
        */
        void CommitEvents();
        
        /**
        * Rolls back the changes made to the event list.
        * @since S60 3.1
        * @param None.
        * @return None.
        */
        void RollbackEvents();
    
    private: // New functions
        /**
        * Finds event from the event list
        * @since S60 3.1
        * @param aEvent reference to event to be searched
        * @return TInt indicating the index. KErrNotFound if event cannot be found
        */      
        TInt FindEvent( const CEvent& aEvent );
        
        /**
        * Finds command from the command list
        * @since S60 3.1
        * @param aCommand reference to command to be searched
        * @return TInt indicating the index. KErrNotFound if command cannot be found
        */      
        TInt FindCommand( const CCommand& aCommand );
        
        /**
        * Compares two events according to their IDs. User in array finds.
        * @since S60 3.1
        * @param aLeftEvent  reference to left event
        * @param aRightEvent reference to right event
        * @return TBool indicating if events are same or not.
        */      
        static TBool CompareEvents( const CEvent& aLeftEvent, 
                                    const CEvent& aRightEvent );
        
        /**
        * Compares two commands according to their IDs. User in array finds.
        * @since S60 3.1
        * @param aLeftCommand  reference to left command
        * @param aRightCommand reference to right command
        * @return TBool indicating if commands are same or not.
        */      
        static TBool CompareCommands( const CCommand& aLeftCommand, 
                                      const CCommand& aRightCommand );
          
    private:
        /**
        * C++ default constructor.
        */
        CCategory();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aCategory identifier of the category
        */
        void ConstructL( TUid aCategory );
            
    private:
        /**
        * UID of the category
        */
        TUid iCategory;
        
        /**
        * List of events included in category
        */
        RPointerArray<CEvent> iEventList;
        
        /**
        * List of commands included in category
        */
        RPointerArray<CCommand> iCommandList;
        
        /**
        * A helper command for seaches. Does not manage any other data than id.
        * Own.
        */
        CCommand* iSearchCommand;
        
        /**
        * A helper event for seaches. Does not manage any other data than id.
        * Own.
        */
        CEvent* iSearchEvent;
    };

/**
*  Class to contain item information (pure virtual)
*
*  @since S60 3.1
*/
class CItem : public CBase
    {
    public:  // Constructors and destructor
        
        
        /**
        * Destructor.
        */
        virtual ~CItem();
    
    public:
        
        /**
        * Commitment state enumeration for list transaction management.
        * 
        * @since S60 3.1
        */
        enum TCommitState
            {
            EAdded, // item has been added to a list
            ERemoved, // item has been removed from list
            ECommitted // no changes pending for the item
            };
            
        /**
        * Getter for item policy
        * @since S60 3.1
        * @param none
        * @return TCapabilitySet policy of the item
        */      
        TCapabilitySet Policy() const;
        
        /**
        * Getter for item type
        * Pure virtual
        * @since S60 3.1
        * @param none
        * @return TItemType type of the item
        */      
        virtual TItemType Type() const = 0;
        
        /**
        * Getter for Id of the item
        * @since S60 3.1
        * @param none
        * @return TInt indicating the Id of the item
        */      
        TInt Id() const;
        
        /**
        * Setter for Id of the item
        * @since S60 3.1
        * @param aId id of the item
        * @return None.
        */      
        void SetId( TInt aId );
        
        /**
        * Setter for the domain Uid of the item
        * @since S60 3.1
        * @param aDomain identifier of the domain
        * @return None.
        */      
        void SetDomain( TUid aDomain );
        
        /**
        * Getter for domain Uid of the item
        * @since S60 3.1
        * @param none
        * @return TInt indicating the Id of the item
        */      
        TUid Domain() const;
        
        /**
        * Setter for the category Uid of the item
        * @since S60 3.1
        * @param aCategory identifier of the category
        * @return None
        */      
        void SetCategory( TUid aCategory );
        
        /**
        * Getter for category Uid of the item
        * @since S60 3.1
        * @param none
        * @return TInt indicating the Id of the item
        */      
        TUid Category() const;
        
        /**
        * Setter for the item version
        * @since S60 3.1
        * @param aVersion indicating the version information
        * @return none.
        */      
        void SetVersion( TVersion aVersion );
        
        /**
        * Getter for the item version
        * @since S60 3.1
        * @param none
        * @return TVersion indicating the version of the item.
        */      
        TVersion Version() const;
        
        /**
        * Setter for the secure id
        * @since S60 3.1
        * @param aSecureId secure id information of the registrator
        * @return none.
        */      
        void SetSecureId( TSecureId aSecureId );
        
        /**
        * Getter for the secure Id
        * @since S60 3.1
        * @param none
        * @return TSecureId of the item
        */      
        TSecureId SecureId() const;
        
        /**
        * Setter for the parameter data
        * @since S60 3.1
        * @param aData reference to data buffer
        * @return none
        */      
        void SetParameterDataL( const TDesC8& aData );
        
        /**
        * Getter for the parameter data
        * @since S60 3.1
        * @param none
        * @return TDesC8& to parameter data
        */      
        const TDesC8& ParameterData() const;
    
        /**
        * Getter for the commitment state
        * @since S60 3.1
        * @param none
        * @return TCommitState commitment state
        */      
        TCommitState CommitState() const;
        
        /**
        * Setter for the commitment state
        * @since S60 3.1
        * @param aState commitment state
        * @return None
        */      
        void SetCommitState( TCommitState aState );
        
    protected:
        /**
        * C++ default constructor.
        */
        CItem();
           
    protected:
        /**
        * Domain information of the item
        */
        TUid iDomain;
        
        /**
        * Category information of the item
        */
        TUid iCategory;
        
        /**
        * ID of the item
        */
        TInt iId;
        
        /**
        * Policy of the item
        */
        TCapabilitySet iPolicy;
        
        /**
        * Version of the item
        */
        TVersion iVersion;
        
        /**
        * If item is persistent
        */
        TBool iPersistent;
        
        /**
        * Parameter data
        * Own.
        */
        HBufC8* iData;

        /**
        * Secure ID of the registrator
        */
        TSecureId iSecureId;
        
        /**
        * Commitment state
        */
        TCommitState iCommitState;
    };

/**
*  Class to contain event information. Derived from CItem
*
*  @since S60 3.1
*/
class CEvent : public CItem
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aId identifier of the event
        * @return a new CEvent instance
        */
        static CEvent* NewL( const TInt aId );
        
        /**
        * Two-phased constructor.
        *
        * @param aEvent source event
        * @return a new CEvent instance
        */
        static CEvent* NewL( const TEvent aEvent );
        
        /**
        * Destructor.
        */
        virtual ~CEvent();
    
    public: // New functions
        
        /**
        * Adds event observer to observer array
        * @since S60 3.1
        * @param aObserver pointer to observer (not owned by CItem)
        * @return TInt indicating the success of the call.
        */      
        TInt AddObserver( MMediatorServerEventObserver* aObserver );
        
        /**
        * Removes observer from event
        * @since S60 3.1
        * @param aObserver pointer to observer (not owned by CItem)
        * @return TInt indicating the success of the call.
        */    
        TInt RemoveObserver( MMediatorServerEventObserver* aObserver );
        
        /**
        * Returns an array of registered observers
        * @since S60 3.1
        * @param None.
        * @return RPointerArray of observers
        */  
        RPointerArray<MMediatorServerEventObserver>& GetObservers();
        
              
        /**
        * Gets TEvent structure of the current event
        * @since S60 3.1
        * @param None
        * @return MediatorService::TEvent structure.
        */  
        MediatorService::TEvent Event() const;
        
    private:
        /**
        * C++ default constructor.
        *
        * @param aId identifier of the event
        */
        CEvent( const TInt aId );
        
        /**
        * C++ default constructor.
        *
        * @param aEvent source event
        */
        CEvent( const TEvent aEvent );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    public: // From base class
    
        /**
        * Getter for item type
        * @since S60 3.1
        * @param void
        * @return TItemType type of the item
        */      
        TItemType Type() const;
    
    private:
        
        // Observers (not owned)
        RPointerArray<MMediatorServerEventObserver> iObservers;
        
    };

/**
*  Class to contain command information. Derived from CItem
*
*  @since S60 3.1
*/
class CCommand : public CItem
    {

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aId identifier of the command
        * @return a new CCommand instance
        */
        static CCommand* NewL( const TInt aId );
        
        /**
        * Two-phased constructor.
        *
        * @param aCommand source command
        * @return a new CCommand instance
        */
        static CCommand* NewL( const TCommand aCommand );
        
        /**
        * Destructor.
        */
        virtual ~CCommand();
    
    public: // New functions
        
        /**
        * Sets timeout for command (milliseconds)
        * @since S60 3.1
        * @param aTimeout timeout value
        * @return None.
        */  
        void SetTimeout( TInt aTimeout );
        
        /**
        * Gets timeout value of the command (milliseconds)
        * @since S60 3.1
        * @param None.
        * @return TInt timeout value
        */  
        TInt Timeout() const;
        
        /**
        * Starts timeout timer
        * @since S60 3.1
        * @param aCallBack pointer to callback observer
        * @return TInt indicating the success of the call.
        */  
        TInt StartTimer( MMediatorTimerCallback* aCallBack );
        
        /**
        * Cancels (ongoing) timeout timer
        * @since S60 3.1
        * @param None.
        * @return None.
        */  
        void CancelTimer();
        
        /**
        * Gets TCommand structure of the current command.
        * @since S60 3.1
        * @param None.
        * @return MediatorService::TCommand information
        */  
        MediatorService::TCommand Command() const;
        
        /**
        * Sets command handler (observer)
        * @since S60 3.1
        * @param aObserver pointer to command handling instance.
        * @return None
        */  
        void SetObserver( MMediatorCommandObserver* aObserver );
        
        /**
        * Gets command handler.
        * @since S60 3.1
        * @param None.
        * @return MMediatorCommandObserver pointer to command observer
        */  
        MMediatorCommandObserver* Observer();
        
        /**
        * Sets command response observer
        * @since S60 3.1
        * @param aObserver pointer to command response observer
        * @return None.
        */  
        void SetResponseObserver( MMediatorCommandResponseObserver* aObserver );
        
        /**
        * Gets command response observer
        * @since S60 3.1
        * @param None.
        * @return MMediatorCommandResponseObserver pointer to observer
        */  
        MMediatorCommandResponseObserver* ResponseObserver();
        
        /**
        * Setter for command status
        *
        * @since S60 3.1
        * @param aStatus command status
        * @return TCommandStatus None.
        */      
        void CCommand::SetStatus( TCommandStatus aStatus );
        
        /**
        * Getter for command status
        *
        * @since S60 3.1
        * @param void
        * @return TCommandStatus the command status
        */      
        TCommandStatus Status() const;
        
    public: // From base class
    
        /**
        * Getter for item type
        * @since S60 3.1
        * @param void
        * @return TItemType type of the item
        */      
        TItemType Type() const;

    private:
        /**
        * C++ default constructor.
        *
        * @param aId identifier of the command
        */
        CCommand( const TInt aId );
        
        /**
        * C++ default constructor.
        *
        * @param aCommand source command
        */
        CCommand( const TCommand aCommand );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private:
        // Command timeout
        TInt    iTimeout;
        
        /**
        * Command observer
        * Not own.
        */
        MMediatorCommandObserver* iObserver;
        
        /**
        * Command response observer
        * Not own.
        */
        MMediatorCommandResponseObserver* iResponseObserver;
        
        /**
        * Command timer
        * Own.
        */
        CMediatorCommandTimer* iCommandTimer;
        
        /**
        * Command status.
        */
        TCommandStatus iStatus;
    };

#endif      // MEDIATORSERVEROBJECTS_H
            
// End of File
