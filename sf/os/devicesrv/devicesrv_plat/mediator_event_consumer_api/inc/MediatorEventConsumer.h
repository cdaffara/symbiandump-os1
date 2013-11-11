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
* Description:  An interface for receiving Mediator events.
*
*/

#ifndef MEDIATOREVENTCONSUMER_H
#define MEDIATOREVENTCONSUMER_H

// INCLUDES
#include <e32base.h>
#include <MediatorServiceDefs.h>

// FORWARD DECLARATIONS
class CMediatorEventConsumerBody;

// CLASS DECLARATION

/**
*  An observer interface for receiving Mediator Service events.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
class MMediatorEventObserver
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


// CLASS DECLARATION

/**
*  A class used for receiving Mediator Service events.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
class CMediatorEventConsumer : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aObserver  mediator event observer
        * @return a new CMediatorEventConsumer instance
        */        
        IMPORT_C static CMediatorEventConsumer* NewL(
            MMediatorEventObserver* aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CMediatorEventConsumer();

    public: // Event subscribtion

        /**
        * Subscibe an event list. Raised events from the list will be
        * delivered via the observer interface.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.        
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of events.
        * @return TInt      an error value.
        */
        IMPORT_C TInt SubscribeEvent( TUid aDomain,
                                      TUid aCategory,
                                      const MediatorService::REventList& aEvents );

        /**
        * Subscibe an event. Raised events will be
        * delivered via the observer interface.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.           
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @param aVersion   Version information of the event
        * @return  TInt      an error value.
        */
        IMPORT_C TInt SubscribeEvent( TUid aDomain, 
                                      TUid aCategory,
                                      TInt aEventId,
                                      TVersion aVersion );

        /**
        * Unsubscibe event list.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEvents    The list of events
        * @return  TInt      an error value.
        */
        IMPORT_C TInt UnsubscribeEvent( TUid aDomain,
                                        TUid aCategory,
                                        const MediatorService::REventList& aEvents );

        /**
        * Unsubscibe an event.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @return  TInt     an error value.
        */
        IMPORT_C TInt UnsubscribeEvent( TUid aDomain,
                                        TUid aCategory, 
                                        TInt aEventId );
        
    private:
    
        /**
        * C++ default constructor.
        */
        CMediatorEventConsumer();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aObserver  mediator event observer
        * @return None.
        */
        void ConstructL( MMediatorEventObserver* aObserver );
    
    private: // Data

        /**
        * Pointer to implementation class
        * Own.
        */
        CMediatorEventConsumerBody*     iBody;
           
	};

#endif      // MEDIATOREVENTCONSUMER_H   
            
// End of File
