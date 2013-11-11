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
* Description:  An implementation class for raising Mediator Service events.
*
*/

#ifndef MEDIATOREVENTPROVIDERBODY_H
#define MEDIATOREVENTPROVIDERBODY_H

// INCLUDES
#include <e32base.h>

#include "MediatorServerClient.h"
#include "MediatorServiceDefs.h"

using namespace MediatorService;

// CLASS DECLARATION

/**
*  A class used for raising Mediator Service events.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS(CMediatorEventProviderBody) : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @return a new CMediatorEventProviderBody instance
        */
        static CMediatorEventProviderBody* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMediatorEventProviderBody();

    public: // Event registration
        
        /**
        * Registers a list of events within specified category. 
        * This command needs SwEvent -capability.
        * 
        * @since S60 3.1   
        * @param aDomain    The identifier of the domain     
        * @param aCategory  The identifier of the category.
        * @param aEvents    List of events to be registered.
        * @return TInt      an error value.
        */
        TInt RegisterEvent( TUid aDomain, 
                            TUid aCategory, 
                            const REventList& aEvents );

        /**
        * Register a single event. After registering an event it may be raised.
        * This command needs SwEvent -capability.
        * 
        * @since S60 3.1
        * @param aDomain    The identifier of the domain         
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @param aVersion   The version of the event.
        * @param aCaps      Capabilities required for consuming the event.
        * @return TInt      an error value.
        */
        TInt RegisterEvent( TUid aDomain,
                            TUid aCategory, 
                            TInt aEventId, 
                            TVersion aVersion,
                            TCapabilitySet aCaps );

        /**
        * Unregister an event category.
        * 
        * @since S60 3.1 
        * @param aDomain    The identifier of the domain         
        * @param aCategory  The identifier of the category.
        * @param aEvents    List of events to be unregistered.
        * @return TInt      an error value.
        */
        TInt UnregisterEvent( TUid aDomain,
                              TUid aCategory,
                              const REventList& aEvents );

        /**
        * Unregister an event.
        * 
        * @since S60 3.1        
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @return TInt      an error value.
        */
        TInt UnregisterEvent( TUid aDomain, 
                              TUid aCategory, 
                              TInt aEventId );
    
    public: // Event raising

        /**
        * Raise an event.
        * This command needs SwEvent -capability.
        * 
        * @since S60 3.1        
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @param aVersion   The version of the event.
        * @param aData      The parameters of the event.
        * @return TInt      an error value.
        */
        TInt RaiseEvent( TUid aDomain,
                         TUid aCategory, 
                         TInt aEventId,
                         TVersion aVersion, 
                         const TDesC8& aData );
    
    private:
    
        /**
        * C++ default constructor.
        */
        CMediatorEventProviderBody();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private: // Data

        // A session class to mediator server
        RMediatorServer     iMediatorServer;
         
	};

#endif      // MEDIATOREVENTPROVIDERBODY_H 
            
// End of File
