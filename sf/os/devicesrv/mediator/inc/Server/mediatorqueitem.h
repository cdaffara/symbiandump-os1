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
* Description:  CMediatorQueItem declaration
*
*/

#ifndef MEDIATORQUEITEM_H
#define MEDIATORQUEITEM_H

#include    <e32base.h>
#include    "MediatorCommon.h"
#include    "MediatorServiceDefs.h"

using namespace MediatorService;

/**
* CMediatorQueItem is a base class for different items
* that are stored to a common queue.
*/
class CMediatorQueItem : public CBase
    {
public:
    /**
    * Destructor.
    */
    virtual ~CMediatorQueItem();

    /**
    * Returns the domain this item belongs to.
    */
    const TUid& Domain() const;
    
    /**
    * Returns the category this item belongs to.
    */
    const TUid& Category() const;

    /**
    * Returns the notification type this item is associated to.
    */
    TMediatorNotificationType NofiticationType() const;
    
    /**
    * Returns the event list of this queued item. If this item has no
    * events then returns NULL.
    */
    virtual const REventList* Events() const = 0;

    /**
    * Returns the command list of this queued item. If this item has no
    * commands then returns NULL.
    */
    virtual const RCommandList* Commands() const = 0;

protected:
    /**
    * Constructor.
    */
    CMediatorQueItem( TMediatorNotificationType aNotificationType,
                      const TUid& aDomain,
                      const TUid& aCategory );
    
private:
    /**
    * C++ default constructor.
    */ 
    CMediatorQueItem();

public:
    /**
    * Single linked list, needed by TSqlQue.
    */
    TSglQueLink iSlink;

private:
    /**
    * Notification type.
    */
    TMediatorNotificationType iNotificationType;
    
    /**
    * Domain of the item.
    */
    TUid iDomain;
    
    /**
    * Category item.
    */
    TUid iCategory;
    };

/**
 * An event list queue item.
 */
class CEventListQueItem : public CMediatorQueItem
                        
    {
public:
    /**
    * Symbian two-phased constructor.
    */
    static CEventListQueItem* NewL( TMediatorNotificationType aNotificationType,
                                    const TUid& aDomain,
                                    const TUid& aCategory,
                                    const REventList& aEventList );
    /**
    * Destructor.
    */
    virtual ~CEventListQueItem();
    
public: // from CMediatorQueItem
    /**
    * Returns the event list stored by this item.
    */
    virtual const REventList* Events() const;
    
    /**
    * Returns NULL.
    */
    virtual const RCommandList* Commands() const;
  
private: 
    /**
    * C++ default constructor. 
    */
    CEventListQueItem();
    
    /**
    * Constructor.
    */
    CEventListQueItem( TMediatorNotificationType aNotificationType,
                       const TUid& aDomain, 
                       const TUid& aCategory );
    
    /**
    * Symbian 2nd-phase constructor.
    */
    void ConstructL( const REventList& aEventList );
    
private:    
    /**
    * Event list.
    */
    REventList iEventList; 
    };


/**
 * A command list queue item.
 */
class CCommandListQueItem : public CMediatorQueItem
    {
public:
    /**
    * Symbian two-phased constructor.
    */
    static CCommandListQueItem* NewL( TMediatorNotificationType aNotificationType,
                                      const TUid& aDomain,
                                      const TUid& aCategory,
                                      const RCommandList& aCommandList );

    /**
    * Destructor.
    */
    virtual ~CCommandListQueItem();
    
public: // from CMediatorQueItem
    /**
    * Returns NULL.
    */
    virtual const REventList* Events() const;
    
    /**
    * Returns the command list stored by this item.
    */
    virtual const RCommandList* Commands() const;
  
private: 
    /**
    * C++ default constructor. 
    */
    CCommandListQueItem();
    
    /**
    * Constructor.
    */
    CCommandListQueItem( TMediatorNotificationType aNotificationType,
                         const TUid& aDomain, 
                         const TUid& aCategory );
    
    /**
    * Symbian 2nd-phase constructor.
    */
    void ConstructL( const RCommandList& aCommandList );

private:
    /**
    * Command list.
    */
    RCommandList iCommandList; 
    };


/**
 * A category queue item.
 */
class CCategoryQueItem : public CMediatorQueItem
    {
public:
    static CCategoryQueItem* NewL( const TUid& aDomain, const TUid& aCategory  );

    /**
    * Destructor.
    */
    virtual ~CCategoryQueItem();
    
public: // from CMediatorQueItem
    /**
    * Returns NULL.
    */
    virtual const REventList* Events() const;
    
    /**
    * Returns NULL.
    */
    virtual const RCommandList* Commands() const;
  
private: 
    /**
    * C++ default constructor. 
    */
    CCategoryQueItem();
    
    /**
    * Constructor.
    */
    CCategoryQueItem( const TUid& aDomain, const TUid& aCategory );
    
    /**
    * Symbian 2nd-phase constructor.
    */
    void ConstructL();
    };

#endif // MEDIATORQUEITEM_H
            
// End of File
