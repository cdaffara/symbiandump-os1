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
* Description:  An interface for raising Mediator Service events.
*
*/

#ifndef MEDIATOREVENTPROVIDER_H
#define MEDIATOREVENTPROVIDER_H

// INCLUDES
#include <e32base.h>

#include <MediatorServiceDefs.h>

// FORWARD DECLARATION
class CMediatorEventProviderBody;

// CLASS DECLARATION

/**
*  A class used for raising Mediator Service events.
*
*  @lib MediatorClient.lib
*  @since S60 3.1
*/
class CMediatorEventProvider : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @return a new CMediatorEventProvider instance
        */
        IMPORT_C static CMediatorEventProvider* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMediatorEventProvider();

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
        IMPORT_C TInt RegisterEvent( TUid aDomain, 
                                     TUid aCategory, 
                                     const MediatorService::REventList& aEvents );

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
        IMPORT_C TInt RegisterEvent( TUid aDomain,
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
        IMPORT_C TInt UnregisterEvent( TUid aDomain,
                                       TUid aCategory,
                                       const MediatorService::REventList& aEvents );

        /**
        * Unregister an event.
        * 
        * @since S60 3.1        
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @return TInt      an error value.
        */
        IMPORT_C TInt UnregisterEvent( TUid aDomain, 
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
        IMPORT_C TInt RaiseEvent( TUid aDomain,
                                  TUid aCategory, 
                                  TInt aEventId,
                                  TVersion aVersion, 
                                  const TDesC8& aData );
    
    private:
    
        /**
        * C++ default constructor.
        */
        CMediatorEventProvider();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private: // Data

        /**
        * Pointer to implementation class
        * Own.
        */
        CMediatorEventProviderBody* iBody;
         
	};

#endif      // MEDIATOREVENTPROVIDER_H 
            
// End of File
