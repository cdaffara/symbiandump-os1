/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  An implementation class for receiving Mediator events.
*
*/

#ifndef MEDIATOREVENTCONSUMERBODY_H
#define MEDIATOREVENTCONSUMERBODY_H

// INCLUDES
#include <e32base.h>
#include "MediatorEventConsumer.h"
#include "MediatorServerClient.h"

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  A class used for receiving Mediator Service events.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS(CMediatorEventConsumerBody) : public CActive
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @param aObserver  mediator event observer
        * @return a new CMediatorEventConsumerBody instance
        */        
        static CMediatorEventConsumerBody* NewL(
            MMediatorEventObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CMediatorEventConsumerBody();

    public: // Event subscribtion

        /**
        * Subscribe an event list. Raised events from the list will be
        * delivered via the observer interface.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.        
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of events.
        * @return TInt      an error value.
        */
        TInt SubscribeEvent( TUid aDomain,
                             TUid aCategory,
                             const REventList& aEvents );

        /**
        * Subscribe an event. Raised events will be
        * delivered via the observer interface.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.           
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @param aVersion   Version information of the event
        * @return TInt      an error value.
        */
        TInt SubscribeEvent( TUid aDomain, 
                             TUid aCategory,
                             TInt aEventId,
                             TVersion aVersion );

        /**
        * Unsubscribe event list.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of events
        * @return TInt      an error value.
        */
        TInt UnsubscribeEvent( TUid aDomain,
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
        TInt UnsubscribeEvent( TUid aDomain,
                               TUid aCategory, 
                               TInt aEventId );
    
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
        * Starts event receiving.
        * 
        * @since S60 3.1
        * @return none.
        */
        void StartEventReceiving();
        
        /**
        * Re-init return data buffer
        *
        * @since S60 3.1
        * @param aSize size of the buffer
        * @return None.
        */
        void ResetDataBufferL( TInt aSize );
        
    private:
    
        /**
        * C++ default constructor.
        *
        * @param aObserver  mediator event observer
        */
        CMediatorEventConsumerBody( MMediatorEventObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private: // Data

        // A session class to mediator server
        RMediatorServer         iMediatorServer;
        
        // Observer for event callbacks (not owned)
        MMediatorEventObserver* iObserver;
        
        // Domain/Category cata for incoming event
        TMediatorCategory           iCategory;
        TMediatorCategoryRetBuffer  iCategoryBuffer;
        
        // Event Id data
        MediatorService::TEvent iEvent;
        TEventRetBuffer         iEventBuffer;
        
        // Event data to be received
        HBufC8*                 iEventData; 
        // Pointer to event data.
        TPtr8                   iEventDataPtr;
        
        // Stores information whether destructor has been called, checked in RunL
        TBool* iDestroyed;

	};

#endif      // MEDIATOREVENTCONSUMERBODY_H   
            
// End of File
