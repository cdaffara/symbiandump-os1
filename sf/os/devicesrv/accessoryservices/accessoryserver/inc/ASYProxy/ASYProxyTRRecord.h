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
* Description:  Data container for transaction data
*
*/


#ifndef CASYPROXYTRRECORD_H
#define CASYPROXYTRRECORD_H

// INCLUDES
#include <AsyCmdTypes.h>
#include <e32std.h>
#include <e32base.h>
#include "AccClientServerMessages.h"

// CONSTANTS
const TInt KResetRecord = -1;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CASYProxyCommandObserver;

// CLASS DECLARATION

/**
*  Container of transaction command data
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYProxyTRRecord: public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYProxyTRRecord* NewL( CASYProxyCommandObserver* aASYProxyCommandObserver );

        /**
        * Destructor.
        */
        virtual ~CASYProxyTRRecord();

    public: // New functions

        /**
        * Resets transaction id.
        * @since S60 3.1
        * @param void
        * @return void
        */
        void ResetTrId();

        /**
        * Gets transaction id.
        * @since S60 3.1
        * @param void
        * @return Transaction id
        */
        TInt TrId();

        /**
         * Gets timeout transaction id.
         * @param void
         * @return Transaction id
         */        
        TInt TimeOutTrId();
        
        /**
        * Gets process command id.
        * @since S60 3.1
        * @param void
        * @return Process command id
        */
        TProcessCmdId CmdId();

        /**
        * Gets command parameters
        * @since S60 3.1
        * @param void
        * @return Command parameters
        */
        TASYCmdParams& AsyCmdParams();

        /**
        * Deletes the observer.
        * @since S60 3.1
        * @param void
        * @return void
        */
        void DeleteCmdObserver();
        
        /**
        * Marks the observer for final destruction, that avoids creation of new observer.
        * @since S60 3.1
        * @param void
        * @return void
        */                
        void MarkForFinalDestruction ();

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CASYProxyTRRecord();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CASYProxyCommandObserver* aASYProxyCommandObserver );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Pointer to observer object
        // This class deletes
        CASYProxyCommandObserver* iASYProxyCommandObserver;
        
        // Parameters transferred between ASY proxy and Accessory Server
        TProxyCommandPckgBuf iParams;
        
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

        friend void CASYProxyCommandObserver::IssueRequestL();
    };

#endif  // CASYPROXYTRRECORD_H

// End of File
