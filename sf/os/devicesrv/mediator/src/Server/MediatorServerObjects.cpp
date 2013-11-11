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
* Description:  Domain, category, event & command object implementations
*
*/


// INCLUDE FILES
#include    <e32base.h>

#include    "MediatorServerObjects.h"
#include    "MediatorCommon.h"
#include    "MediatorServer.h"
#include    "MediatorServiceDefs.h"
#include    "Debug.h"

// =============================================================================
// ============================ DOMAIN FUNCTIONS ===============================
// =============================================================================

// -----------------------------------------------------------------------------
// CDomain::CDomain
// -----------------------------------------------------------------------------
//
CDomain::CDomain()
    {
    }

// -----------------------------------------------------------------------------
// CDomain::ConstructL
// -----------------------------------------------------------------------------
//
void CDomain::ConstructL( TUid aDomain )
    {
    iDomain = aDomain;
    iSearchCategory = CCategory::NewL( TUid() ); // just assign some arbitrary domain, updated in search
    }

// -----------------------------------------------------------------------------
// CDomain::NewL
// -----------------------------------------------------------------------------
//
CDomain* CDomain::NewL( TUid aDomain )
    {
    CDomain* self = new( ELeave ) CDomain;
    
    CleanupStack::PushL( self );
    self->ConstructL( aDomain );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CDomain::~CDomain
// -----------------------------------------------------------------------------
//
CDomain::~CDomain()
    {
    iCategoryList.ResetAndDestroy();
    delete iSearchCategory;
    }

// -----------------------------------------------------------------------------
// CDomain::DomainUid
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CDomain::DomainUid() const
    {
    return iDomain;
    }

// -----------------------------------------------------------------------------
// CDomain::SetDomainUid
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CDomain::SetDomainUid( TUid aDomain )
    {
    iDomain = aDomain;
    }
    
// -----------------------------------------------------------------------------
// CDomain::AddCategoryL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
void CDomain::AddCategoryL( const CCategory* aNewCategory )
    {
    __ASSERT_DEBUG( aNewCategory, CMediatorServer::PanicServer( EMediatorServerIllegalParameter ) );
    TInt index = 0;     // not used here
    if ( FindCategory( aNewCategory->CategoryUid(), index ) )
        {
        ERROR_TRACE(Print(_L("CDomain::AddCategoryL: category %d already exists in domain %d\n"), aNewCategory->CategoryUid().iUid,
                                                                                                  iDomain.iUid ) );    
        User::Leave( KErrAlreadyExists );
        }
    iCategoryList.AppendL( aNewCategory );
    }

// -----------------------------------------------------------------------------
// CDomain::AddCategoryL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
CCategory* CDomain::AddCategoryL( const TUid aCategory )
    {
    CCategory* newCategory = CCategory::NewL( aCategory );
    CleanupStack::PushL( newCategory );
    
    TInt index = 0;
    if ( FindCategory( newCategory, index ) )
        {
        ERROR_TRACE(Print(_L("CDomain::AddCategoryL: category %d already exists in domain %d\n"), aCategory.iUid,
                                                                                                  iDomain.iUid ) );    
        User::Leave( KErrAlreadyExists );
        }
    iCategoryList.AppendL( newCategory );
    CleanupStack::Pop( newCategory );   // ownership is changed
    return newCategory;
    }
        
// -----------------------------------------------------------------------------
// CDomain::RemoveCategory
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
TInt CDomain::RemoveCategory( const TUid aCategory )
    {
    TInt error = KErrMediatorCategoryNotFound;
    // Find the category
    TInt index = 0;
    CCategory* category = FindCategory( aCategory, index );
    
    if ( category )
        {
        // Make temporary pointer to it, remove it from list and delete.
        CCategory* category = iCategoryList[ index ];
        iCategoryList.Remove( index );
        delete category;
        error = KErrNone;
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CDomain::GetCategory
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
const CCategory* CDomain::GetCategory( const TUid aCategory )
    {
    TInt index = 0;
    return FindCategory( aCategory, index );
    }

// -----------------------------------------------------------------------------
// CDomain::GetCategory
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
CCategory* CDomain::GetCategory( TInt aIndex )
    {
    CCategory* categoryPtr = NULL;
    if ( aIndex >= 0 && aIndex < CategoryCount() )
        {
        categoryPtr = iCategoryList[aIndex];
        }
    return categoryPtr;
    }

// -----------------------------------------------------------------------------
// CDomain::CategoryCount
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
TInt CDomain::CategoryCount()
    {
    return iCategoryList.Count();
    }

// -----------------------------------------------------------------------------
// CDomain::FindCategory
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
CCategory* CDomain::FindCategory( const TUid aCategory, TInt& aIndex )
    {
    iSearchCategory->SetCategoryUid( aCategory );
    
    return FindCategory( iSearchCategory, aIndex );
    }    

// -----------------------------------------------------------------------------
// CDomain::FindCategory
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
CCategory* CDomain::FindCategory( const CCategory* aCategory, TInt& aIndex )
    {
    // Create utilities to be used in find
    TIdentityRelation<CCategory> relation( CompareCategories );
	
	// Find the item from array
    aIndex = iCategoryList.Find( aCategory, relation );	
    // Then return it, if found. 
    CCategory* category = NULL;
    if ( aIndex != KErrNotFound )
        {
        category = iCategoryList[aIndex];
        }
	return category;
    }

// -----------------------------------------------------------------------------
// CDomain::GetCategoriesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
void CDomain::GetCategoriesL( RCategoryList& aCategories )
    {
    // Loop through category list and append to return list.
    for ( TInt index = 0; index < iCategoryList.Count(); index++ )
        {
        aCategories.AppendL( iCategoryList[index]->CategoryUid() );
        }
    }
          
// -----------------------------------------------------------------------------
// CDomain::CompareCategories
// Compares categories and returns boolean according to comparison
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TBool CDomain::CompareCategories( const CCategory& aLeftCategory, 
                                  const CCategory& aRightCategory )
    {
    return aLeftCategory.CategoryUid() == aRightCategory.CategoryUid();
    }

// =============================================================================
// ============================ CATEGORY FUNCTIONS =============================
// =============================================================================

// -----------------------------------------------------------------------------
// CCategory::CCategory()
// -----------------------------------------------------------------------------
//
CCategory::CCategory()
    {
    }

// -----------------------------------------------------------------------------
// CCategory::ConstructL
// -----------------------------------------------------------------------------
//
void CCategory::ConstructL( TUid aCategory )
    {
    iCategory = aCategory;
    iSearchCommand = CCommand::NewL(0); // arbitrary id, updated in search
    iSearchEvent = CEvent::NewL(0); // arbitrary id, updated in search
    }

// -----------------------------------------------------------------------------
// CCategory::NewL
// -----------------------------------------------------------------------------
//
CCategory* CCategory::NewL( TUid aCategory )
    {
    CCategory* self = new( ELeave ) CCategory;
    
    CleanupStack::PushL( self );
    self->ConstructL( aCategory );
    CleanupStack::Pop( self );

    return self;
    }
    
// -----------------------------------------------------------------------------
// CCategory::~CCategory
// -----------------------------------------------------------------------------
//
CCategory::~CCategory()
    {
    iEventList.ResetAndDestroy();
    iCommandList.ResetAndDestroy();
    delete iSearchCommand;
    delete iSearchEvent;
    }

// -----------------------------------------------------------------------------
// CCategory::CategoryUid
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CCategory::CategoryUid() const
    {
    return iCategory;
    }

// -----------------------------------------------------------------------------
// CCategory::SetCategoryUid
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CCategory::SetCategoryUid( TUid aCategory )
    {
    iCategory = aCategory;
    }

// -----------------------------------------------------------------------------
// CCategory::AddEventL
// Adds new event to eventlist
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
TInt CCategory::AddEvent( const CEvent* aNewEvent )
    {
    __ASSERT_DEBUG( aNewEvent, CMediatorServer::PanicServer( EMediatorServerIllegalParameter ) );
    
    TInt error = FindEvent( *aNewEvent );
    if ( error == KErrNotFound )
        {
#ifdef _DEBUG
        TRACE(Print(_L("[Mediator Server]\t CCategory::AddEvent id %d\n"), aNewEvent->Id() ));
        for ( TInt index = 0; index < ECapability_Limit; index ++ )
            {
            TCapabilitySet caps = aNewEvent->Policy();
            TBool required = caps.HasCapability( (TCapability) index );
            if ( required )
                {
                TRACE(Print(_L("[Mediator] CCategory::AddEvent: Capability %d required\n"), index ));
                }
            }
#endif // _DEBUG
        error = iEventList.Append( aNewEvent );
        }
    else
        {
        error = KErrMediatorEventAlreadyExists;
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CCategory::AddCommandL
// Adds new command to commandList
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
TInt CCategory::AddCommand( const CCommand* aNewCommand )
    {
    __ASSERT_DEBUG( aNewCommand, CMediatorServer::PanicServer( EMediatorServerIllegalParameter ) );
    
    TInt error = FindCommand( *aNewCommand );
    if ( error == KErrNotFound )
        {
#ifdef _DEBUG
        TRACE(Print(_L("[Mediator Server]\t CCategory::AddCommand id %d\n"), aNewCommand->Id() ));
        for ( TInt index = 0; index < ECapability_Limit; index ++ )
            {
            TCapabilitySet caps = aNewCommand->Policy();
            TBool required = caps.HasCapability( (TCapability) index );
            if ( required )
                {
                TRACE(Print(_L("[Mediator] CCategory::AddCommand: Capability %d required\n"), index ));
                }
            }
#endif // _DEBUG
        error = iCommandList.Append( aNewCommand );
        }
    else
        {
        error = KErrMediatorCommandAlreadyExists;
        }
    return error;
    }
        
// -----------------------------------------------------------------------------
// CCategory::RemoveEvent
// Removes item from the category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CCategory::RemoveEvent( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iEventList.Count() )
        {
        iEventList.Remove( aIndex );
        }
    }
    
// -----------------------------------------------------------------------------
// CCategory::RemoveCommand
// Removes item from the category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CCategory::RemoveCommand( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iCommandList.Count() )
        {
        iCommandList.Remove( aIndex );
        }
    }
    
// -----------------------------------------------------------------------------
// CCategory::FindEvent
// Returns item specified by Id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//         
CEvent* CCategory::FindEvent( const TInt aId, TInt& aIndex )
    {
    iSearchEvent->SetId( aId );
	
    aIndex = FindEvent( *iSearchEvent );
    
    // Then return it, if found. 
    CEvent* event = NULL;
    if ( aIndex != KErrNotFound )
        {
        event = iEventList[aIndex];
        }
    
	return event;
    }    

// -----------------------------------------------------------------------------
// CCategory::GetEvent
// Returns item specified by index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
CEvent* CCategory::GetEvent( TInt aIndex )
    {
    CEvent* eventPtr = NULL;
    if ( aIndex >= 0 && aIndex < EventCount() )
        {
        eventPtr = iEventList[aIndex];
        }
    return eventPtr;
    }

// -----------------------------------------------------------------------------
// CCategory::GetCommand
// Returns item specified by index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
CCommand* CCategory::GetCommand( TInt aIndex )
    {
    CCommand* commandPtr = NULL;
    if ( aIndex >= 0 && aIndex < CommandCount() )
        {
        commandPtr = iCommandList[aIndex];
        }
    return commandPtr;
    }

// -----------------------------------------------------------------------------
// CCategory::FindCommand
// Returns item specified by Id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
CCommand* CCategory::FindCommand( const TInt aId, TInt& aIndex )
    {
    iSearchCommand->SetId( aId );
    
    aIndex = FindCommand( *iSearchCommand );
    
    // Then return it, if found. 
    CCommand* command = NULL;
    if ( aIndex != KErrNotFound )
        {
        command = iCommandList[aIndex];
        }
    
	return command;
    }    


// -----------------------------------------------------------------------------
// CCategory::FindEvent
// Finds event from the event list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TInt CCategory::FindEvent( const CEvent& aEvent )
    {
    // Create utilities to be used in find
    TIdentityRelation<CEvent> relation( CompareEvents );

	// Find the event from array	
	return iEventList.Find( &aEvent, relation );
    }

// -----------------------------------------------------------------------------
// CCategory::FindCommand
// Finds item from the item list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TInt CCategory::FindCommand( const CCommand& aCommand )
    {
    // Create utilities to be used in find
    TIdentityRelation<CCommand> relation( CompareCommands );

	// Find the command from array	
	return iCommandList.Find( &aCommand, relation );
    }

// -----------------------------------------------------------------------------
// CCategory::GetEventsL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
void CCategory::GetEventsL( REventList& aEvents )
    {
    // Loop thought category's event list and append those to returned 
    // event list.
    for ( TInt index = 0; index < iEventList.Count(); index++ )
        {
        CEvent* event = iEventList[index];
        if ( event )
            {
            aEvents.AppendL( event->Event() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCategory::GetCommandsL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
void CCategory::GetCommandsL( RCommandList& aCommands )
    {
    // Loop thought category's event list and append those to returned 
    // event list.
    for ( TInt index = 0; index < iCommandList.Count(); index++ )
        {
        CCommand* command = iCommandList[index];
        if ( command )
            {
            aCommands.AppendL( command->Command() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCategory::EventCount
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
TInt CCategory::EventCount()
    {
    return iEventList.Count();
    }

// -----------------------------------------------------------------------------
// CCategory::CommandCount
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
TInt CCategory::CommandCount()
    {
    return iCommandList.Count();
    }

// -----------------------------------------------------------------------------
// CCategory::CompareCommands
// Compares two commands
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
TBool CCategory::CompareCommands( const CCommand& aLeftCommand, 
                                  const CCommand& aRightCommand )
    { 
    return aLeftCommand.Id() == aRightCommand.Id();
    }

// -----------------------------------------------------------------------------
// CCategory::CompareEvents
// Compares two commands
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
TBool CCategory::CompareEvents( const CEvent& aLeftEvent, 
                                const CEvent& aRightEvent )
    { 
    return aLeftEvent.Id() == aRightEvent.Id();
    }
 

// -----------------------------------------------------------------------------
// CCategory::CommitCommands
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCategory::CommitCommands()
    {
    // iterate through command list
    CCommand* command;
    
    for ( TInt i = iCommandList.Count() - 1; i >= 0; i-- ) // iterage in reverse order because count may change
        {
        command = iCommandList[i];
        
        if ( command->CommitState() == CItem::EAdded )
            {
            // if item state is Added -> set to Committed
            command->SetCommitState( CItem::ECommitted );
            }
        else if ( command->CommitState() == CItem::ERemoved )
            {
            // if item state is Removed -> remove item and release reserved memory
            iCommandList.Remove( i );
            delete command;
            }
        }
    }
    

// -----------------------------------------------------------------------------
// CCategory::RollbackCommands
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCategory::RollbackCommands()
    {
    CCommand* command;

    // iterate through command list
    for ( TInt i = iCommandList.Count() - 1; i >= 0; i-- ) // iterage in reverse order because count may change
        {
        command = iCommandList[i];
        
        if ( command->CommitState() == CItem::EAdded )
            {
            // if item state is Added -> remove item and release reserved memory
            iCommandList.Remove( i );
            delete command;
            }
        else if ( command->CommitState() == CItem::ERemoved )
            {
            // if item state is Removed -> set back to Committed
            command->SetCommitState( CItem::ECommitted );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCategory::CommitEvents
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CCategory::CommitEvents()
    {
    CEvent* event;
    
    // iterate through event list
    for ( TInt i = iEventList.Count() - 1; i >= 0; i-- ) // iterage in reverse order because count may change
        {
        event = iEventList[i];
        
        if ( event->CommitState() == CItem::EAdded )
            {
            // if item state is Added -> set to Committed
            event->SetCommitState( CItem::ECommitted );
            }
        else if ( event->CommitState() == CItem::ERemoved )
            {
            // if item state is Removed -> remove item and release reserved memory
            iEventList.Remove( i );
            delete event;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCategory::RollbackEvents
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CCategory::RollbackEvents()
    {
    CEvent* event;

    // iterate through command list
    for ( TInt i = iEventList.Count() - 1; i >= 0; i-- ) // iterage in reverse order because count may change
        {
        event = iEventList[i];
        
        if ( event->CommitState() == CItem::EAdded )
            {
            // if item state is Added -> remove item and release reserved memory
            iEventList.Remove( i );
            delete event;
            }
        else if ( event->CommitState() == CItem::ERemoved )
            {
            // if item state is Removed -> set back to Committed
            event->SetCommitState( CItem::ECommitted );
            }
        }
    }
    
// =============================================================================
// ============================ ITEM FUNCTIONS =================================
// =============================================================================

// -----------------------------------------------------------------------------
// CItem::CItem
// -----------------------------------------------------------------------------
//
CItem::CItem()
    {
    }

// -----------------------------------------------------------------------------
// CItem::~CItem
// -----------------------------------------------------------------------------
//
CItem::~CItem()
    {
    delete iData;
    }

// -----------------------------------------------------------------------------
// CItem::Id
// Returns id of the item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CItem::Id() const
    {
    return iId;
    }

// -----------------------------------------------------------------------------
// CItem::SetId
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CItem::SetId( TInt aId )
    {
    iId = aId;
    }

// -----------------------------------------------------------------------------
// CItem::SetDomain
// Sets domain to item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
void CItem::SetDomain( TUid aDomain )
    {
    iDomain = aDomain;
    }

// -----------------------------------------------------------------------------
// CItem::Domain
// Gets item domain
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
TUid CItem::Domain() const
    {
    return iDomain;
    }

// -----------------------------------------------------------------------------
// CItem::SetCategory
// Sets category to item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
void CItem::SetCategory( TUid aCategory )
    {
    iCategory = aCategory;
    }

// -----------------------------------------------------------------------------
// CItem::Category
// Gets item category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
TUid CItem::Category() const
    {
    return iCategory;
    }

// -----------------------------------------------------------------------------
// CItem::Policy
// Returns the required capability set for the item (command or event)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TCapabilitySet CItem::Policy() const
    {
    return iPolicy;
    }


// -----------------------------------------------------------------------------
// CItem::SetVersion
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
void CItem::SetVersion( TVersion aVersion )
    {
    iVersion = aVersion;
    }

// -----------------------------------------------------------------------------
// CItem::Version
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TVersion CItem::Version() const
    {
    return iVersion;
    }


// -----------------------------------------------------------------------------
// CItem::SetSecureId
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
void CItem::SetSecureId( TSecureId aSecureId )
    {
    iSecureId = aSecureId;
    }

// -----------------------------------------------------------------------------
// CItem::SecureId
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TSecureId CItem::SecureId() const
    {
    return iSecureId;
    }


// -----------------------------------------------------------------------------
// CItem::SetParameterDataL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CItem::SetParameterDataL( const TDesC8& aData )
    {
    // Empty existing just to be sure
    delete iData;
    iData = NULL;
    
    iData = aData.AllocL();
    }

// -----------------------------------------------------------------------------
// CItem::ParameterData
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
const TDesC8& CItem::ParameterData() const
    {
    if ( iData )
        {
        return *iData;
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// CItem::CommitState
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
CItem::TCommitState CItem::CommitState() const
    {
    return iCommitState;
    }
        
// -----------------------------------------------------------------------------
// CItem::SetCommitState
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//         
void CItem::SetCommitState( TCommitState aState )
    {
    iCommitState = aState;
    }

// =============================================================================
// ============================ EVENT FUNCTIONS ================================
// =============================================================================

// -----------------------------------------------------------------------------
// CEvent::CEvent
// -----------------------------------------------------------------------------
//
CEvent::CEvent( const TInt aId )
    {
    iId = aId;
    }

// -----------------------------------------------------------------------------
// CEvent::CEvent
// -----------------------------------------------------------------------------
//
CEvent::CEvent( const TEvent aEvent )
    {
    iId = aEvent.iEventId;
    iPolicy = aEvent.iCaps;
    iVersion = aEvent.iVersion;
    }


// -----------------------------------------------------------------------------
// CEvent::ConstructL
// -----------------------------------------------------------------------------
//
void CEvent::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CEvent::NewL
// -----------------------------------------------------------------------------
//
CEvent* CEvent::NewL( const TInt aId )
    {
    CEvent* self = new( ELeave ) CEvent( aId );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    

// -----------------------------------------------------------------------------
// CEvent::NewL
// -----------------------------------------------------------------------------
//
CEvent* CEvent::NewL( const TEvent aEvent )
    {
    CEvent* self = new( ELeave ) CEvent( aEvent );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// -----------------------------------------------------------------------------
// CEvent::~CEvent
// -----------------------------------------------------------------------------
//
CEvent::~CEvent()
    {
    }

// -----------------------------------------------------------------------------
// CEvent::Type
// Returns EItemEvent
// -----------------------------------------------------------------------------
//
TItemType CEvent::Type() const
    {
    return EItemEvent;
    }
 
// -----------------------------------------------------------------------------
// CEvent::AddObserver
// 
// -----------------------------------------------------------------------------
//
TInt CEvent::AddObserver( MMediatorServerEventObserver* aObserver )
    {
    TInt error = KErrNone;
    // Check that observer does not exists already for this event
    TBool found = EFalse;
    for (TInt index = 0; index < iObservers.Count() && !found; index++ )
        {
        if ( iObservers[index] == aObserver )
            {
            error = KErrMediatorAlreadySubscribed;
            found = ETrue;
            }
        }
    // Add observer in case no errors
    if ( error == KErrNone )
        {
        error = iObservers.Append( aObserver );
        }
    return error;
    }
 
// -----------------------------------------------------------------------------
// CEvent::RemoveObserver
// 
// -----------------------------------------------------------------------------
//       
TInt CEvent::RemoveObserver( MMediatorServerEventObserver* aObserver )
    {
    TInt error = KErrNone;
    TBool found = EFalse;
    for (TInt index = 0; index < iObservers.Count() && !found; index++ )
        {
        // Remove the fond observer
        if ( iObservers[index] == aObserver )
            {
            iObservers.Remove( index );
            found = ETrue;
            }
        }
    if ( !found )
        {
        error = KErrMediatorNoSubscription;
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CEvent::GetObservers
// 
// -----------------------------------------------------------------------------
//
RPointerArray<MMediatorServerEventObserver>& CEvent::GetObservers()
    {
    return iObservers;
    }

// -----------------------------------------------------------------------------
// CEvent::Event
// Returns EItemEvent
// -----------------------------------------------------------------------------
//
MediatorService::TEvent CEvent::Event() const
    {
    MediatorService::TEvent event;
    event.iEventId  = Id();
    event.iVersion  = Version();
    event.iCaps     = Policy();
    return event;
    }
 

// =============================================================================
// ============================ COMMAND FUNCTIONS ==============================
// =============================================================================

// -----------------------------------------------------------------------------
// CCommand::CCommand
// -----------------------------------------------------------------------------
//
CCommand::CCommand( const TInt aId )
    {
    iId = aId;
    iStatus = ECommandPending; // pending command by default
    }

// -----------------------------------------------------------------------------
// CCommand::CCommand
// -----------------------------------------------------------------------------
//
CCommand::CCommand( const TCommand aCommand )
    {
    iId = aCommand.iCommandId;
    iPolicy = aCommand.iCaps;
    iVersion = aCommand.iVersion;
    iTimeout = aCommand.iTimeout;
    iStatus = ECommandPending; // pending command by default
    }

// -----------------------------------------------------------------------------
// CCommand::ConstructL
// -----------------------------------------------------------------------------
//
void CCommand::ConstructL()
    {
    iCommandTimer = CMediatorCommandTimer::NewL();
    }

// -----------------------------------------------------------------------------
// CCommand::NewL
// -----------------------------------------------------------------------------
//
CCommand* CCommand::NewL( const TInt aId )
    {
    CCommand* self = new( ELeave ) CCommand( aId );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCommand::NewL
// -----------------------------------------------------------------------------
//
CCommand* CCommand::NewL( const TCommand aCommand )
    {
    CCommand* self = new( ELeave ) CCommand( aCommand );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCommand::~CCommand
// -----------------------------------------------------------------------------
//
CCommand::~CCommand()
    {
    if ( iCommandTimer )
        {
        iCommandTimer->Cancel();
        }
    delete iCommandTimer;
    }

// -----------------------------------------------------------------------------
// CCommand::Type
// Returns EItemCommand
// -----------------------------------------------------------------------------
//
TItemType CCommand::Type() const
    {
    return EItemCommand;
    }

// -----------------------------------------------------------------------------
// CCommand::SetTimeOut
// 
// -----------------------------------------------------------------------------
//
void CCommand::SetTimeout( TInt aTimeout )
    {
    iTimeout = aTimeout;
    }

// -----------------------------------------------------------------------------
// CCommand::TimeOut
// 
// -----------------------------------------------------------------------------
//
TInt CCommand::Timeout() const
    {
    return iTimeout;
    }

// -----------------------------------------------------------------------------
// CCommand::Command
// 
// -----------------------------------------------------------------------------
//
MediatorService::TCommand CCommand::Command() const
    {
    MediatorService::TCommand command;
    command.iCommandId  = Id();
    command.iVersion    = Version();
    command.iTimeout    = Timeout();
    command.iCaps       = Policy();
    return command;
    }

// -----------------------------------------------------------------------------
// CCommand::SetObserver
// 
// -----------------------------------------------------------------------------
// 
void CCommand::SetObserver( MMediatorCommandObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CCommand::Observer
// 
// -----------------------------------------------------------------------------
// 
MMediatorCommandObserver* CCommand::Observer()
    {
    return iObserver;
    }

// -----------------------------------------------------------------------------
// CCommand::SetResponseObserver
// 
// -----------------------------------------------------------------------------
//
void CCommand::SetResponseObserver( MMediatorCommandResponseObserver* aObserver )
    {
    iResponseObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CCommand::ResponseObserver
// 
// -----------------------------------------------------------------------------
// 
MMediatorCommandResponseObserver* CCommand::ResponseObserver()
    {
    return iResponseObserver;
    }

// -----------------------------------------------------------------------------
// CCommand::StartTimer
// 
// -----------------------------------------------------------------------------
// 
TInt CCommand::StartTimer( MMediatorTimerCallback* aCallBack )
    {
    // Start timer
    return iCommandTimer->StartTimer( aCallBack, Domain(), Category(), Id(), Timeout() );
    }

// -----------------------------------------------------------------------------
// CCommand::CancelTimer
// 
// -----------------------------------------------------------------------------
// 
void CCommand::CancelTimer()
    {
    iCommandTimer->Cancel();
    }

// -----------------------------------------------------------------------------
// CCommand::SetStatus
// 
// -----------------------------------------------------------------------------
//
void CCommand::SetStatus( TCommandStatus aStatus )
    {
    iStatus = aStatus;
    }

// -----------------------------------------------------------------------------
// CCommand::Status
// 
// -----------------------------------------------------------------------------
// 
TCommandStatus CCommand::Status() const
    {
    return iStatus;
    }

//  End of File  
