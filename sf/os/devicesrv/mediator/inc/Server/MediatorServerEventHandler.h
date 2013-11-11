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
* Description:  Managing of events that are registered to Mediator
*
*/


#ifndef MEDIATORSERVEREVENTHANDLER_H
#define MEDIATORSERVEREVENTHANDLER_H

// INCLUDES
#include <e32base.h>

#include "MediatorCommon.h"
#include "MediatorServerObjectHandler.h"
#include "MediatorServiceDefs.h"


// FORWARD DECLARATIONS
class MMediatorServerEventObserver;
class CMediatorServerObjectHandler;
class CDomain;
class CCategory;
class CEvent;
class CItem;
class CCommand;

// NAMESPACE
using namespace MediatorService;

// CLASS DECLARATION

/**
*  Event handler class for Mediator Server.
*
*  @lib MediatorServer
*  @since S60 3.1
*/
class CMediatorServerEventHandler : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aObjectHandler object handler
        * @return a new CMediatorServerEventHandler instance
        */
        static CMediatorServerEventHandler* NewL( 
                    CMediatorServerObjectHandler& aObjectHandler );
        
        /**
        * Destructor.
        */
        virtual ~CMediatorServerEventHandler();

    public: // New functions
        
        /**
        * Register list of new events.
        * Checks that no existing events are registered
        * @since S60 3.1
        * @param aCategory  information for the category
        * @param aEvents    list of events to be registered
        * @param aSecureId  secure Id of the client
        * @return none.
        */
        void RegisterEventListL( TMediatorCategory aCategory, 
                                 const REventList& aEvents,
                                 TSecureId aSecureId );
                                 
         
        /**
        * Unregisters event(s)
        * 
        * @since S60 3.1
        * @param aCategory  information for the category
        * @param aEvents    list of events to be unregistered
        * @param aSecureId  secure Id of the client
        * @return none.
        */
        void UnregisterEventListL( TMediatorCategory aCategory, 
                                   const REventList& aEvents,
                                   TSecureId aSecureId  );   
       
        /**
        * Subscribes event(s)
        * 
        * @since S60 3.1
        * @param aCategory  Information for the category
        * @param aEvents    List of the events to be subscribed
        * @param aCaps      Capabilities of the subscriber
        * @param aObserver  Event observer instance
        * @return none.
        */

        void SubscribeEventListL( TMediatorCategory aCategory, 
                                  const REventList& aEvents,
                                  TCapabilitySet aCaps,
                                  MMediatorServerEventObserver* aObserver );
                               
        /**
        * Unsubscribes event(s)
        *
        * @since S60 3.1
        * @param aCategory  information for the category
        * @param aEvents    List of the events to be unsubscribed.
        * @param aObserver  Event observer instance
        * @return none.
        */

        void UnsubscribeEventListL( TMediatorCategory aCategory, 
                                    const REventList& aEvents,
                                    MMediatorServerEventObserver* aObserver ); 
                                    
        /**
        * Raise Event
        * @since S60 3.1
        * @param aCategory  Information for the category
        * @param aEvent     Event information
        * @param aData      Event parameter data
        * @return none.
        */

        void RaiseEventL( TMediatorCategory aCategory, 
                          MediatorService::TEvent aEvent,
                          const TDesC8& aData );                                     
        
        /**
        * Returns count of events
        * @since S60 3.1
        * @return event count
        */
        TInt EventCount() { return iEventServiceList.Count(); }
    
    private: // From CActive
    
        /**
        * From CActive::RunL
        */
        void RunL();
        
        /**
        * From CActive::DoCancel()
        */
        void DoCancel();                                

    private:

        /**
        * C++ default constructor.
        *
        * @param aObjectHandler object handler
        */
        CMediatorServerEventHandler( 
                    CMediatorServerObjectHandler& aObjectHandler );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        // List of buffered events
        RPointerArray<CEvent>  iEventServiceList;
        
        // Handle to object handler
        CMediatorServerObjectHandler& iObjectHandler;
         
    };

#endif      // MEDIATORSERVEREVENTHANDLER_H   
            
// End of File
