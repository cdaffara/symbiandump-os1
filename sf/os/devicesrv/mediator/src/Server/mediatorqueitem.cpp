/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CMediatorQueItem implementation
*
*/


// INCLUDE FILES
#include "mediatorqueitem.h"

// -----------------------------------------------------------------------------
// CMediatorQueItem::NofiticationType
// -----------------------------------------------------------------------------
TMediatorNotificationType CMediatorQueItem::NofiticationType() const
    {
    return iNotificationType;
    }

// -----------------------------------------------------------------------------
// CMediatorQueItem::Domain
// -----------------------------------------------------------------------------
const TUid& CMediatorQueItem::Domain() const 
    {
    return iDomain; 
    }

// -----------------------------------------------------------------------------
// CMediatorQueItem::Category
// -----------------------------------------------------------------------------
const TUid& CMediatorQueItem::Category() const 
    {
    return iCategory;
    }

// -----------------------------------------------------------------------------
// CMediatorQueItem::CMediatorQueItem
// -----------------------------------------------------------------------------
CMediatorQueItem::CMediatorQueItem( TMediatorNotificationType aNotificationType,
                                          const TUid& aDomain, 
                                          const TUid& aCategory ) :
                                          iNotificationType( aNotificationType ),
                                          iDomain( aDomain ),
                                          iCategory( aCategory )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorQueItem::~CMediatorQueItem
// -----------------------------------------------------------------------------
CMediatorQueItem::~CMediatorQueItem()
    {
    }

// -----------------------------------------------------------------------------
// CEventListQueItem::NewL
// -----------------------------------------------------------------------------
CEventListQueItem* CEventListQueItem::NewL( TMediatorNotificationType aNotificationType,
                                          const TUid& aDomain, 
                                          const TUid& aCategory,
                                          const REventList& aEventList )
    {
    CEventListQueItem* self = new (ELeave) CEventListQueItem( aNotificationType,
                                                            aDomain, 
                                                            aCategory );
    CleanupStack::PushL( self );
    self->ConstructL( aEventList );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CEventListQueItem::CEventListQueItem
// -----------------------------------------------------------------------------
CEventListQueItem::~CEventListQueItem()
    {
    iEventList.Reset();
    }

// -----------------------------------------------------------------------------
// CEventListQueItem::CEventListQueItem
// -----------------------------------------------------------------------------
CEventListQueItem::CEventListQueItem( TMediatorNotificationType aNotificationType,
                                    const TUid& aDomain, 
                                    const TUid& aCategory ) :
                                    CMediatorQueItem( aNotificationType,
                                                         aDomain, 
                                                         aCategory )
    {
    }

// -----------------------------------------------------------------------------
// CEventListQueItem::ConstructL
// -----------------------------------------------------------------------------
void CEventListQueItem::ConstructL( const REventList& aEventList )
    {
    TInt eventCount( aEventList.Count() );
    iEventList.ReserveL( eventCount );
    
    for ( TInt i = 0; i < eventCount; i++ )
        {
        iEventList.AppendL( aEventList[i] );
        }
    }

// -----------------------------------------------------------------------------
// CEventListQueItem::Events
// -----------------------------------------------------------------------------
const REventList* CEventListQueItem::Events() const
    {
    return &iEventList;
    }
        
// -----------------------------------------------------------------------------
// CEventListQueItem::Commands
// -----------------------------------------------------------------------------
const RCommandList* CEventListQueItem::Commands() const
    {
    return NULL;
    }


// -----------------------------------------------------------------------------
// CCommandListQueItem::NewL
// -----------------------------------------------------------------------------
CCommandListQueItem* CCommandListQueItem::NewL( TMediatorNotificationType aNotificationType, 
                                              const TUid& aDomain, 
                                              const TUid& aCategory,
                                              const RCommandList& aCommandList )
    {
    CCommandListQueItem* self = new (ELeave) CCommandListQueItem( aNotificationType, 
                                                                aDomain,
                                                                aCategory );
    CleanupStack::PushL( self );
    self->ConstructL( aCommandList );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCommandListQueItem::~CCommandListQueItem
// -----------------------------------------------------------------------------
CCommandListQueItem::~CCommandListQueItem()
    {
    iCommandList.Reset();
    }

// -----------------------------------------------------------------------------
// CCommandListQueItem::CCommandListQueItem
// -----------------------------------------------------------------------------
CCommandListQueItem::CCommandListQueItem( TMediatorNotificationType aNotificationType,
                                        const TUid& aDomain, 
                                        const TUid& aCategory ) :
                                        CMediatorQueItem( aNotificationType,
                                                             aDomain, 
                                                             aCategory )
    {
    }

// -----------------------------------------------------------------------------
// CCommandListQueItem::ConstructL
// -----------------------------------------------------------------------------
void CCommandListQueItem::ConstructL( const RCommandList& aCommandList )
    {
    TInt commandCount( aCommandList.Count() );
    iCommandList.ReserveL( commandCount );
    
    for ( TInt i = 0; i < commandCount; i++ )
        {
        iCommandList.AppendL( aCommandList[i] );
        }
    }

// -----------------------------------------------------------------------------
// CCommandListQueItem::Events
// -----------------------------------------------------------------------------
const REventList* CCommandListQueItem::Events() const
    {
    return NULL;
    }
        
// -----------------------------------------------------------------------------
// CCommandListQueItem::Commands
// -----------------------------------------------------------------------------
const RCommandList* CCommandListQueItem::Commands() const
    {
    return &iCommandList;
    }

// -----------------------------------------------------------------------------
// CCategoryQueItem::NewL
// -----------------------------------------------------------------------------
CCategoryQueItem* CCategoryQueItem::NewL( const TUid& aDomain, const TUid& aCategory  )
    {
    CCategoryQueItem* self = new (ELeave) CCategoryQueItem( aDomain, aCategory );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCategoryQueItem::~CCategoryQueItem
// -----------------------------------------------------------------------------
CCategoryQueItem::~CCategoryQueItem()
    {
    }
    
// -----------------------------------------------------------------------------
// CCategoryQueItem::Events
// -----------------------------------------------------------------------------
const REventList* CCategoryQueItem::Events() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCategoryQueItem::Commands
// -----------------------------------------------------------------------------
const RCommandList* CCategoryQueItem::Commands() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCategoryQueItem::CCategoryBuffer
// -----------------------------------------------------------------------------
CCategoryQueItem::CCategoryQueItem( const TUid& aDomain, 
                                  const TUid& aCategory ) :
                                  CMediatorQueItem( EMediatorCategoryUnregistered, 
                                                       aDomain,
                                                       aCategory )
    {
    }

// -----------------------------------------------------------------------------
// CCategoryQueItem::ConstructL
// -----------------------------------------------------------------------------
void CCategoryQueItem::ConstructL()
    {
    }

//  End of File  
