/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Timer for controlling asys loading
*
*/


#ifndef CASYPROXYLOADERTIMER_H
#define CASYPROXYLOADERTIMER_H

// INCLUDES
#include "acc_debug.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CASYProxyAsyLoader;

// CLASS DECLARATION

/**
*  Timer class for time out checking during loading ASYs.
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYProxyLoaderTimer : public CTimer
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYProxyLoaderTimer* NewL( CASYProxyAsyLoader* aLoader );

        /**
        * Destructor.
        */
        virtual ~CASYProxyLoaderTimer();

    public: // New functions

       /**
        * Starts timer.
        * @since S60 3.1
        * @param void
        * @return void
        */
        void StartTimer();

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

        /**
        * From CActive Handles an active object's request completion event.
        */
        void RunL();

        /**
        * From CActive Handles RunL's leave cases
        */
        TInt RunError( TInt aError );

    private:

        /**
        * C++ default constructor.
        */
        CASYProxyLoaderTimer( CASYProxyAsyLoader* aLoader );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data

    private:    // Data

        // Pointer to a loader object
        CASYProxyAsyLoader* iLoader; // Not owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CASYPROXYLOADERTIMER_H

// End of File
