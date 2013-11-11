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
* Description:  Observer class for call back functions from ASY to Server side
*
*/


#ifndef MASYPROXYOBSERVER_H
#define MASYPROXYOBSERVER_H

// INCLUDES
#include <e32svr.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Observer class for call back functions from ASY to Server
*
*  @lib AccServer.exe
*  @since S60 3.1
*/
NONSHARABLE_CLASS( MASYProxyObserver )
    {
    public: // Constructors and destructor

    public: // New functions

        /**
        * Callback function for handling completion of loading ASY
        * @since S60 3.1
        * @param aASYIndex Index of the ASY
        * @param aAllASYsComplete Indicating if all ASYs loading is completed
        * @return void
        */
        virtual void LoadCompleteL(
                        const TInt aASYIndex,
                        const TBool aAllASYsComplete = EFalse ) = 0;

        /**
        * Callback function for handling kill of ASY
        * @since S60 3.1
        * @param aASYIndex Index of the ASY
        * @return void
        */
        virtual void ThreadKills( const TInt aASYIndex ) = 0;

        /**
        * Callback function for handling time out of ASY loading
        * @since S60 3.1
        * @param void
        * @return void
        */
        virtual void LoadingTimedOutL() = 0;

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

#endif // MASYPROXYOBSERVER_H

// End of File
