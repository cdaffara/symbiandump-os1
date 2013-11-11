/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observes and updates to PS the network signal strength.
*
*/



#ifndef CSAESIGNALSTRENGTHOBSERVER_H
#define CSAESIGNALSTRENGTHOBSERVER_H

//  INCLUDES
#include <etelmm.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CSaePubSubNotifier;

/**
*  Observes signal strengths from ETel
*/
NONSHARABLE_CLASS( CSaeSignalStrengthObserver ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSaeSignalStrengthObserver* NewL( CSaePubSubNotifier* aNotifier );
        
        /**
        * Destructor.
        */
        virtual ~CSaeSignalStrengthObserver();

    public: // New functions
        
        /**
        * Start listening to signal strength changes. 
        * Update current status with synchronous getter operation and request
        * notifications about state changes.
        * @return void
        */
        void Receive();

    protected:  // Functions from base classes
        
         /**
        * From CActive Cancels the asyncronous request
        * @since 
        * @return void
        */
        void DoCancel();

        /**
        * From CActive Handles the the event that has arrived from ETel.
        * @since 
        * @return void
        */
        void RunL();

        
    private:

        /**
        * C++ default constructor.
        */
        CSaeSignalStrengthObserver( CSaePubSubNotifier* aNotifier );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Issue notification request to RMobilePhone.
        */
        void IssueRequest();

    public:     // Data
    
    protected:  // Data

    private:    // Data
    
        // Session with Etel server
        RTelServer iEtelServer;

        // RMobilePhone for requesting the signal strength state
        RMobilePhone iMobilePhone;
        
        // Notifier to PubSub
        CSaePubSubNotifier* iNotifier;
        
        // The signal strength in dBm
        TInt32 iSignalStrength;
        
        // The number of signal strength bars
        TInt8 iBar;
        
        // The number of last notification signal strength bars
        TInt8 iPreviousBar;
    };

#endif      // CSAESIGNALSTRENGTHOBSERVER_H   
            
// End of File
