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
* Description:  Observer class for Timeout call backfunctions of ProcessResponse
*
*/


#ifndef MACCSRVPROCESSCOMMANDOBSERVER_H
#define MACCSRVPROCESSCOMMANDOBSERVER_H

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
NONSHARABLE_CLASS( MAccSrvProcessCommandObserver )
    {
    public: // Constructors and destructor

    public: // New functions
       
        /**
        * Callback function for handling time out of ProcessResponse
        *  for ProcessCommand init
        * @since S60 3.1
        * @param void
        * @return void
        */
        virtual void CompleteProcessCommandInitL() = 0;
        
        /**
        * Callback function for handling time out of ProcessResponse
        * @since S60 3.1
        * @param void
        * @return void
        */
        virtual void ProcessCommandTimeOutCompleteL() = 0;        

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

#endif // MACCSRVPROCESSCOMMANDOBSERVER_H

// End of File
