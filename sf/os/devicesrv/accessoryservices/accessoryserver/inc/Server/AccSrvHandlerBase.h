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
* Description:  A base class for active handlers
*
*/



#ifndef CACCSRVHANDLERBASE_H
#define CACCSRVHANDLERBASE_H

// INCLUDE FILES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAccSrvConnectionControllerObserver;

// CLASS DECLARATION

/**
*  Base class for asynchronous handlers.
*  Class is meant only for inheritance.
*
*  @lib AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvHandlerBase ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CAccSrvHandlerBase();

    public: // New functions

        /**
        * Function provided by this active object for starting asynchronous handling.
        *
        * @since S60 3.1
        * @return void
        */
        void IssueRequest();

    protected:  // Functions from base classes

        /**
        * From CActive
        * Handles an active object's request completion event.
        *
        * @since S60 3.1
        * @return void
        */
        virtual void RunL()=0;

        /**
        * Implements cancellation of an outstanding request.
        * This function is called as part of the active object's Cancel().
        *
        * @since S60 3.1
        * @return void
        */
        void DoCancel();


        /**
        * From CActive
        * The active scheduler calls this function if this active object's
        * RunL() function leaves.
        *
        * @since S60 3.1
        * @param aError The leave code
        * @return KErrNone, when error has been handled
        */
        virtual TInt RunError( TInt aError );

    protected:  // New functions

        /**
        * C++ default constructor.
        */
        CAccSrvHandlerBase();

        /**
        * Constructor with notify handling.
        */
        CAccSrvHandlerBase( MAccSrvConnectionControllerObserver* aCallback );

    protected:  // Functions from base classes

    private:

    public:     // Data

    protected:  // Data

        MAccSrvConnectionControllerObserver* iCallback; //not owned

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

};

#endif      // CACCSRVHANDLERBASE_H

// End of File
