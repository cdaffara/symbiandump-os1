/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Startup for and handling statuses of ASYs
*
*/


#ifndef CASYPROXYSTARTER_H
#define CASYPROXYSTARTER_H

// INCLUDES
#include "AccServer.h"
#include "AsyProxyObserver.h"
#include "AccSrvASYProxyHandler.h"
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CASYProxyAPI;

// CLASS DECLARATION

/**
*  Startup class for loading ASYs and
*  handling changes in ASYs statuses
*
*  @lib AccServer.exe
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CASYProxyStarter ) : public CBase, public MASYProxyObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYProxyStarter* NewL(
            CASYProxyAPI* aAsyProxyAPI,
            CAccSrvConnectionController* aConnectionController );

        /**
        * Destructor.
        */
        ~CASYProxyStarter();

    public: // New functions

        /**
        * Starts ASYLoading process in ASYProxy side
        * @since S60 3.1
        * @param void
        * @return void
        */
        void LoadASYs();

        /**
        * Handles completion of loading ASY
        * @since S60 3.1
        * @param aASYIndex Index of ASY to handle this callback
        * @param aAllASYsComplete Indicates whether all of ASYs are loaded completely
        *                         EFalse All ASYs are not yet loaded completely
        *                         ETrue  All ASYs are loaded completely
        * @return void
        */
        virtual void LoadCompleteL( const TInt aASYIndex,
                                    const TBool aAllASYsComplete = EFalse );

        /**
        * Handles ASY thread kill
        * @since S60 3.1
        * @param aASYIndex Index of killing ASY
        * @return void
        */
        virtual void ThreadKills( const TInt aASYIndex );

        /**
        * Handles timeout of ASY's loading process
        * @since S60 3.1
        * @param void
        * @return void
        */
        virtual void LoadingTimedOutL();

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CASYProxyStarter( CASYProxyAPI* aAsyProxyAPI,
                          CAccSrvConnectionController* aConnectionController );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data

    private:    // Data

        // Pointer to an ASYProxy
        CASYProxyAPI* iAsyProxyAPI; // Not owned
        CAccSrvConnectionController* iConnectionController; // Not owned
        TBool iInitCalled;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CASYPROXYSTARTER_H

// End of File
