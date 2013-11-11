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


#ifndef SAEPUBSUBNOTIFIER_H
#define SAEPUBSUBNOTIFIER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>
#include <DosSvrServices.h>
#include "PSVariables.h"

// CLASS DECLARATION
class CSaeEtelObserver;

class CSaeSignalStrengthObserver;

/**
*  This class creates the observers and notifies about state changes to Publish and Subscribe.
*/
NONSHARABLE_CLASS( CSaePubSubNotifier ) : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CSaePubSubNotifier();

        /**
        * Two-phased constructor.
        */
        static CSaePubSubNotifier* NewL();

        /**
        * Destructor.
        */
        virtual ~CSaePubSubNotifier();

        /**
        * Through this method the Publish and Subscribe is notified about the state changes.
        * @param aVariable  The variable uid of the variable to change. 
        * @param aState     The new state of the variable
		* @return Return value of the call to RProperty::Set()
        */
        TInt NotifyPubSub( const TUid aVariable, const TInt aState );

    private:

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
        // By default, prohibit copy constructor
        CSaePubSubNotifier( const CSaePubSubNotifier& );
        // Prohibit assigment operator
        CSaePubSubNotifier& operator= ( const CSaePubSubNotifier& );
		// Define Publish and Subscribe properties
		void DefineProperties();

    private:  // Data
        // Etel observer.
        CSaeEtelObserver* iEtelObserver;
		// Network signal strength observer
		CSaeSignalStrengthObserver* iSignalObserver;
		// This variable indicates whether the calls forwarding information is
		// initialised.
        TBool iCfStatusInitialised;
		//Publish and Subscribe properties
		RProperty iSAProperty;
    };

#endif      // SAEPUBSUBNOTIFIER_H   
            
// End of File
