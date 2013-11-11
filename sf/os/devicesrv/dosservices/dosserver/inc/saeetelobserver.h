/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


#ifndef SAEETELOBSERVER_H
#define SAEETELOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <etelpckt.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CSaePubSubNotifier;
class CSaeGprsAvailabilityObs;

// CLASS DECLARATION

/**
*  This class is responsible for creating the ETel observer
*/
NONSHARABLE_CLASS( CSaeEtelObserver ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CSaeEtelObserver( CSaePubSubNotifier* aPubSubNotifier );

        /**
        * Two-phased constructor.
        */
        static CSaeEtelObserver* NewL( CSaePubSubNotifier* aPubSubNotifier );

        /**
        * Destructor.
        */
        virtual ~CSaeEtelObserver();

    public: // New functions

		/**
        * Initialize the Etel observer.
		* Create GPRS session and observers, and start listening for events.
        * @return completion code (standard Symbian error code, KErrNone if ok)
        */
		TInt StartListeningL();

		/**
        * Notifies Publish and Subscribe (delegates it to iPubSubNotifier)
        * @param aVariable SA variable uid
        * @param aState New state
        */
		void NotifyPubSub( const TUid aVariable, const TInt aState );

    private:

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CSaeEtelObserver( const CSaeEtelObserver& );
        // Prohibit assigment operator
        CSaeEtelObserver& operator= ( const CSaeEtelObserver& );

    private:    // Data
    
		// A pointer to the Publish and Subscribe interface. Not owned.
		CSaePubSubNotifier* iPubSubNotifier;
		// Observes the GPRS availability
		CSaeGprsAvailabilityObs* iGprsAvailabilityObs;
		// RTelServer session
		RTelServer iEtelServer;
		// RPhone session
		RPhone iPhone;
		// MultiMode Etel API
		RPacketService iService;
  };

#endif      // SAEETELOBSERVER   
            
// End of File
