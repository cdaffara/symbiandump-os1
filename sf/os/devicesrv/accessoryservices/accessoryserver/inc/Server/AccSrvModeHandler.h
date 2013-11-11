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
* Description:  Accessory Mode handler
*
*/



#ifndef CACCSRVMODEHANDLER_H
#define CACCSRVMODEHANDLER_H

// INCLUDE FILES
#include "AccSrvHandlerBase.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAccSrvConnectionControllerObserver;

// CLASS DECLARATION

/**
*  Starts asunchronous Accessory Mode update.
*
*  @lib AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvModeHandler ) : public CAccSrvHandlerBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAccSrvModeHandler* NewL( MAccSrvConnectionControllerObserver* aCallback );

        /**
        * Destructor.
        */
        virtual ~CAccSrvModeHandler();

    public: // New functions

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

        /**
        * Handles an active object's request completion event.
        *
        * @since S60 3.1
        * @return void
        */
        void RunL();

    private:

        /**
        * C++ default constructor.
        */
        CAccSrvModeHandler( MAccSrvConnectionControllerObserver* aCallback );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

};

#endif      // CACCSRVMODEHANDLER_H

// End of File
