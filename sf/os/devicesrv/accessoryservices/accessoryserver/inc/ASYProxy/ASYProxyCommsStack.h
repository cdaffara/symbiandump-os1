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
* Description:  Stack for handling transactions
*
*/


#ifndef CASYPROXYCOMMSSTACK_H
#define CASYPROXYCOMMSSTACK_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CASYProxyTRRecord;
class CASYProxyCommandObserver;

// CLASS DECLARATION

/**
*  Class for handling concurrent transactions.
*  Uses array as a stack.
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYProxyCommsStack: public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYProxyCommsStack* NewL();

        /**
        * Destructor.
        */
        virtual ~CASYProxyCommsStack();

    public: // New functions

        /**
        * Creates and adds a new transaction record to stack.
        * @since S60 3.1
        * @param aASYProxyCommandObserver Pointer to a observer
        * @return void
        */
        void AddL( CASYProxyCommandObserver* aASYProxyCommandObserver );

        /**
        * Removes current transaction from stack.
        * @since S60 3.1
        * @param aTrId Transaction to be removed
        * @return void
        */
        void Remove( TInt aTrId );

        /**
        * Removes all transactions from stack.
        * @since S60 3.1
        * @param void
        * @return void
        */
        void RemoveAll();

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CASYProxyCommsStack();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data

    private:    // Data

        // Array used as a stack of transactions
        RPointerArray<CASYProxyTRRecord> iTRRecordArray;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CASYPROXYCOMMSSTACK_H

// End of File
