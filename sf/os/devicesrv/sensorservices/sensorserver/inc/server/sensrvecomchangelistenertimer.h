/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Rescan timer for ECOM changes
*
*/


#ifndef CSENSSRVECOMCHANGELISTENERTIMER_H
#define CSENSSRVECOMCHANGELISTENERTIMER_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CSensrvEcomChangeListener;

// CLASS DECLARATION

/**
*  Timer class for rescanning ECOM changes
*
*  @since S60 5.0
*/
class CSensrvEcomChangeListenerTimer : public CTimer
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @since S60 5.0
        */
        static CSensrvEcomChangeListenerTimer* NewL( CSensrvEcomChangeListener* aListener );

        /**
        * Destructor.
        *
        * @since S60 5.0
        */
        virtual ~CSensrvEcomChangeListenerTimer();

    public: // New functions

       /**
        * Starts timer.
        * @param void
        * @return void
        */
        void StartTimer();

       /**
        * Starts timer.
        * @param TTimeIntervalMicroSeconds32
        * @return void
        */
        void StartTimer( TTimeIntervalMicroSeconds32 );

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
        CSensrvEcomChangeListenerTimer( CSensrvEcomChangeListener* aListener );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data

    private:    // Data

        // Pointer to a loader object
        CSensrvEcomChangeListener* iListener; // Not owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CSENSSRVECOMCHANGELISTENERTIMER_H

// End of File
