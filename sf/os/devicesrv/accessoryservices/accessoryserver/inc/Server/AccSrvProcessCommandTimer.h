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
* Description:  Timer for ProcessCommand handling
*
*/


#ifndef CACCSRVPROCESSCOMMANDTIMER_H
#define CACCSRVPROCESSCOMMANDTIMER_H

// INCLUDES
#include "acc_debug.h"
#include <e32base.h>

// CONSTANTS
const TInt KInitCmdTimeOut    = 5000000;
const TInt KProcessCmdTimeOut = 2500000;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAccSrvProcessCommandObserver;

// CLASS DECLARATION

/**
*  Timer class for time out of ProcessCommands.
*
*  @lib AccServer.exe
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvProcessCommandTimer ) : public CTimer
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAccSrvProcessCommandTimer* NewL(
            MAccSrvProcessCommandObserver* aProcessCommandObserver,
            TInt aTime = KProcessCmdTimeOut );

        /**
        * Destructor.
        */
        virtual ~CAccSrvProcessCommandTimer();

    public: // New functions

       /**
        * Starts the timer.
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
        CAccSrvProcessCommandTimer(
            MAccSrvProcessCommandObserver* aProcessCommandObserver,
            TInt aTime );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data

    private:    // Data

        TInt iTime; // TimeOut time

        // Pointer to ProcessCommandObserver object
        MAccSrvProcessCommandObserver* iProcessCommandObserver; // Not owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCSRVPROCESSCOMMANDTIMER_H

// End of File
