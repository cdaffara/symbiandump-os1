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
* Description:  Mediator server object handler.
*
*/


// INCLUDE FILES
#include    <e32base.h>

#include    "MediatorServerObjectHandler.h"
#include    "Debug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::CMediatorServerObjectHandler
// -----------------------------------------------------------------------------
//
CMediatorServerObjectHandler::CMediatorServerObjectHandler()
    {
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CMediatorServerObjectHandler::ConstructL()
    {
    iSearchDomain = CDomain::NewL( TUid() );
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::NewL
// -----------------------------------------------------------------------------
//
CMediatorServerObjectHandler* CMediatorServerObjectHandler::NewL()
    {
    LOG(_L("[Mediator Server]\t CMediatorServerObjectHandler::NewL"));
    CMediatorServerObjectHandler* self = new( ELeave ) CMediatorServerObjectHandler;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::~CMediatorServerObjectHandler
// -----------------------------------------------------------------------------
//    
CMediatorServerObjectHandler::~CMediatorServerObjectHandler()
    {
    // Clear domain list
    iDomainList.ResetAndDestroy();
    delete iSearchDomain;
    }


// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::AddObserverL
// Adds notification observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CMediatorServerObjectHandler::AddObserverL( 
                    MMediatorServerNotificationObserver* aObserver )
    {
    TBool found = EFalse;
    for ( TInt index = 0; index < iObserverList.Count() && !found; index ++ )
        {
        if ( iObserverList[index] == aObserver )
            {
            found = ETrue;
            }
        }
    if ( !found )
        {
        iObserverList.AppendL( aObserver );
        } 
    else
        {
        User::Leave( KErrAlreadyExists );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::RemoveObserverL
// Removes notification observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CMediatorServerObjectHandler::RemoveObserverL( 
                    MMediatorServerNotificationObserver* aObserver )
    {
    TBool found = EFalse;
    for (TInt index = 0; index < iObserverList.Count() && !found; index ++ )
        {
        if ( iObserverList[index] == aObserver )
            {
            iObserverList.Remove( index );
            found = ETrue;
            }
        }
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::FindDomain
// Returns pointer to found domain
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
CDomain* CMediatorServerObjectHandler::FindDomain( const TUid aDomain )
    {
     // Create utilities to be used in find
    TIdentityRelation<CDomain> relation( CompareDomains );
	
	iSearchDomain->SetDomainUid( aDomain );
	
	// Find the item from array
	TInt index = iDomainList.Find( iSearchDomain, relation );	
	
	// Then return domain, if found. 
    CDomain* domain = NULL;
    if ( index != KErrNotFound )
        {
        domain = iDomainList[index];
        }
	return domain;    
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::AddDomainL
// Returns pointer to new domain
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
CDomain* CMediatorServerObjectHandler::AddDomainL( const TUid aDomain )
    {
    CDomain* newDomain = CDomain::NewL( aDomain );
    CleanupStack::PushL( newDomain );
    iDomainList.AppendL( newDomain );
    CleanupStack::Pop( newDomain );
    return newDomain;
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::CategoryL
// Returns pointer to found category
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
CCategory* CMediatorServerObjectHandler::CategoryL( TMediatorCategory aCategory )
    {
    // Check that domain exists 
    CDomain* domain = FindDomain( aCategory.iDomain );
    if ( !domain )
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorServerObjectHandler::CategoryL: domain %d not found\n"), aCategory.iDomain ) );
        User::Leave( KErrMediatorDomainNotFound );
        }
    // Check that category exists
    TInt index = 0; // not used
    CCategory* category = domain->FindCategory( aCategory.iCategory, index );
    if ( !category )
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorServerObjectHandler::CategoryL: category %d not found\n"), aCategory.iCategory ) );
        User::Leave( KErrMediatorCategoryNotFound );
        }
    return category;
    }
// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::CompareDomains
// Compares domains and returns boolean according to comparison
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TBool CMediatorServerObjectHandler::CompareDomains( const CDomain& aLeftDomain, 
                                                    const CDomain& aRightDomain )
    {
    return aLeftDomain.DomainUid() == aRightDomain.DomainUid();
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::GetDomainsL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerObjectHandler::GetDomainsL( RDomainList& aDomains )
    {
    // Loop through array and append all domains
    for ( TInt index = 0; index < iDomainList.Count(); index++ )
        {
        aDomains.AppendL( iDomainList[index]->DomainUid() );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::GetCategoriesL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerObjectHandler::GetCategoriesL( TMediatorCategory aCategory, 
                                                   RCategoryList& aCategories )
    {
    // Check that domain exists 
    CDomain* domain = FindDomain( aCategory.iDomain );
    if ( !domain )
        {
        ERROR_TRACE(Print(_L("[Mediator] CMediatorServerObjectHandler::GetCategoriesL: domain %d not found\n"), aCategory.iDomain ) );
        User::Leave( KErrMediatorDomainNotFound );
        }
    else
        {
        // Ok, get categories from domain.
        domain->GetCategoriesL( aCategories );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::GetCommandsL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerObjectHandler::GetCommandsL( TMediatorCategory aCategory, 
                                                 RCommandList& aCommands )
    {
    // Find domain & category
    CCategory* category = CategoryL( aCategory ); 
    if ( category )
        {
        category->GetCommandsL( aCommands );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::GetEventsL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerObjectHandler::GetEventsL( TMediatorCategory aCategory, 
                                               REventList& aEvents )
    {
    // Find domain & category
    CCategory* category = CategoryL( aCategory ); 
    if ( category )
        {
        category->GetEventsL( aEvents );
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::EventsAdded
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerObjectHandler::EventsAdded( TUid aDomain, 
                                                TUid aCategory, 
                                                const REventList& aEvents )
    {
    for ( TInt index = 0; index < iObserverList.Count(); index++ )
        {
        MMediatorServerNotificationObserver* observer = iObserverList[index];
        if ( observer )
            {
            // In case of leave, ignore the leave value and continue notifications
            TRAP_IGNORE( observer->MediatorEventsAddedL( aDomain, 
                                                         aCategory, 
                                                         aEvents ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::CommandsAdded
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerObjectHandler::CommandsAdded( TUid aDomain, 
                     TUid aCategory,
                     const RCommandList& aCommands )
    {
    for ( TInt index = 0; index < iObserverList.Count(); index++ )
        {
        MMediatorServerNotificationObserver* observer = iObserverList[index];
        if ( observer )
            {
            // In case of leave, ignore the leave value and continue notifications
            TRAP_IGNORE( observer->MediatorCommandsAddedL( aDomain, 
                                                           aCategory, 
                                                           aCommands ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::EventsRemoved
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerObjectHandler::EventsRemoved( TUid aDomain, 
                     TUid aCategory,
                     const REventList& aEvents )
    {
    // If category is empty, remove it
    TBool categoryRemoved = EFalse;
    TRAP_IGNORE( categoryRemoved = RemoveCategoryIfEmptyL( aDomain, aCategory ));
    for ( TInt index = 0; index < iObserverList.Count(); index++ )
        {
        MMediatorServerNotificationObserver* observer = iObserverList[index];
        if ( observer )
            {
            if ( categoryRemoved )  // different callback if category was removed
                {
                // In case of leave, ignore the leave value and continue 
                // notifications
                TRAP_IGNORE( observer->MediatorCategoryRemovedL( aDomain, 
                                                                 aCategory ) );
                }
            else
                {
                // In case of leave, ignore the leave value and continue notifications
                TRAP_IGNORE( observer->MediatorEventsRemovedL( aDomain, 
                                                               aCategory, 
                                                               aEvents ) );
                }
            }
        }
    }
                                     
// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::CommandsRemoved
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMediatorServerObjectHandler::CommandsRemoved( TUid aDomain, 
                                               TUid aCategory,
                                               const RCommandList& aCommands )
    {
    // If category is empty, remove it
    TBool categoryRemoved = EFalse;
    
    TRAP_IGNORE( categoryRemoved = RemoveCategoryIfEmptyL( aDomain, aCategory ));
    
    for ( TInt index = 0; index < iObserverList.Count(); index++ )
        {
        MMediatorServerNotificationObserver* observer = iObserverList[index];
        if ( observer )
            {
            if ( categoryRemoved )  // different callback if category was removed
                {
                // In case of leave, ignore the leave value and continue 
                // notifications
                TRAP_IGNORE( observer->MediatorCategoryRemovedL( aDomain, 
                                                                 aCategory ) );
                }
            else
                {
                // In case of leave, ignore the leave value and continue 
                // notifications
                TRAP_IGNORE( observer->MediatorCommandsRemovedL( aDomain, 
                                                                 aCategory, 
                                                                 aCommands ) );
                }
            
            }
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::IsCategoryEmptyL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
TBool CMediatorServerObjectHandler::IsCategoryEmptyL( TUid aDomain, TUid aCategory)
    {
    TBool empty = EFalse;
    TMediatorCategory category;
    category.iDomain = aDomain;
    category.iCategory = aCategory;
    CCategory* categoryPtr = CategoryL( category );
    if ( categoryPtr )
        {
        TInt eventCount = categoryPtr->EventCount();
        TInt commandCount = categoryPtr->CommandCount();
        if ( !eventCount && !commandCount )
            {
            empty = ETrue;
            }
        categoryPtr = NULL; // Not owned
        }
    return empty;
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::RemoveCategoryIfEmptyL
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
TBool CMediatorServerObjectHandler::RemoveCategoryIfEmptyL( TUid aDomain, TUid aCategory)
    {
    TBool removed = EFalse;
    // Check if category is empty
    if ( IsCategoryEmptyL( aDomain, aCategory ))
        {
        CDomain* domainPtr = FindDomain( aDomain );
        if ( domainPtr )
            {
            domainPtr->RemoveCategory( aCategory );
            removed = ETrue;
            domainPtr = NULL; // Not owned
            }
        }
    return removed;
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::ClearRegistrations
//  
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CMediatorServerObjectHandler::ClearRegistrations( 
                          MMediatorServerEventObserver* aEventObserver, 
                          MMediatorCommandObserver* aCommandObserver,
                          MMediatorServerNotificationObserver* aNotifObserver )
    {
    LOG(_L("[Mediator Server]\t CMediatorServerObjectHandler::ClearRegistrationsL"));
    
    // Clear all pending subscriptions -- loop through domains
    for ( TInt index = iDomainList.Count()-1; index >= 0; index-- )
        {
        CDomain* domainPtr = iDomainList[index];
        if ( domainPtr )
            {
            // Loop through categories
            for( TInt index2 = domainPtr->CategoryCount()-1; index2 >= 0; index2-- )
                {
                CCategory* categoryPtr = domainPtr->GetCategory( index2 );
                if ( categoryPtr )
                    {
                    // Loop through events within category
                    for ( TInt index3 = categoryPtr->EventCount()-1; index3 >= 0; index3-- )
                        {
                        CEvent* eventPtr = categoryPtr->GetEvent( index3 );
                        if ( eventPtr )
                            {
                            // Unregister if found --> ignore the return value
                            eventPtr->RemoveObserver( aEventObserver );
                            }
                        eventPtr = NULL;
                        }
                    RCommandList removedCommands;
                    // Loop through commands within category
                    for ( TInt index4 = categoryPtr->CommandCount()-1; index4 >= 0; index4-- )
                        {
                        CCommand* commandPtr = categoryPtr->GetCommand( index4 );
                        if ( commandPtr )
                            {
                            if ( commandPtr->Observer() == aCommandObserver )
                                {
                                // Found the command --> remove it and append
                                // to notification list
                                TCommand removeCommand;
                                removeCommand.iCommandId = commandPtr->Id();
                                removeCommand.iVersion = commandPtr->Version();
                                removedCommands.Append( removeCommand );
                                categoryPtr->RemoveCommand( index4 );
                                delete commandPtr;
                                commandPtr = NULL;
                                }
                            }
                        commandPtr = NULL;
                        }
                    // Notify command removal
                    if ( removedCommands.Count() > 0 )
                        {
                        CommandsRemoved( domainPtr->DomainUid(),
                                         categoryPtr->CategoryUid(),
                                         removedCommands );
                        }
                    removedCommands.Reset();
                    }
                categoryPtr = NULL;
                }
            }
        domainPtr = NULL;
        }
    
    // Loop through the notification subscriber array still
    TBool found = EFalse;
    for ( TInt index = 0; index < iObserverList.Count() && !found; index++ )
        {
        if ( iObserverList[index] == aNotifObserver )
            {
            iObserverList.Remove( index );
            found = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMediatorServerObjectHandler::GetStatistics
// Get count of domains/categories/observers.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CMediatorServerObjectHandler::GetStatistics(TInt& aDomainCount, 
                                                 TInt& aCategoryCount,
                                                 TInt& aEventCount,
                                                 TInt& aCommandCount,
                                                  TInt& aObserverCount)
    {
    aDomainCount = iDomainList.Count();
    aCategoryCount = 0;
    aEventCount = 0;
    aCommandCount = 0;
    
    for (TInt i = 0; i < aDomainCount; i++)
        {
        CDomain* domain = iDomainList[i];
        aCategoryCount += domain->CategoryCount();
        
        for ( TInt j = 0; j < domain->CategoryCount(); j++ )
            {
            CCategory* categoryPtr = domain->GetCategory( j );
            aEventCount += categoryPtr->EventCount();
            aCommandCount += categoryPtr->CommandCount();
            }
        }
    aObserverCount = iObserverList.Count();
    }

//  End of File  
