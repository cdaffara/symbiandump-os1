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
* Description:  Thread creater for SAE
*
*/


#ifndef CSaeThread_H
#define CSaeThread_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "sae.h"

// CONSTANTS

// Thread's name
_LIT(KSAEThreadName, "SAEThread");

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Creates SAE to it's own thread
*
*  @lib DosSrv.dll
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CSaeThread ) : public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSaeThread* NewL();

        /**
        * Destructor.
        */
        virtual ~CSaeThread();

    public: // New functions

        /**
        * Thread's entry point
        * @since S60 3.1
        * @param
        * @return Error code of thread creating
        *         KErrNone if succesfully
        */
        static TInt ThreadEntryPoint( TAny* aParameter );

        /**
        * Starts the thread creating
        * @since S60 3.1
        * @param
        * @return void
        */
        void StartThreadL();

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CSaeThread();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Creates CActiveScheduler object and begins SAE creating
        */
        static TInt DoStartThreadL();

        /**
        * Create SAE
        */
        static TInt CreateSaeLC();

    public:     // Data

    protected:  // Data

    private:    // Data

        // Handle to created thread
        RThread iThread;
        
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

#endif // CSaeThread_H

// End of File
