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
* Description:  Handler for ASY instance
*
*/


#ifndef CASYPROXYASYHANDLER_H
#define CASYPROXYASYHANDLER_H

// INCLUDES
#include "ASYProxyMainService.h"
#include "ASYProxy.h"
#include <AsyCmdTypes.h>
#include <AsyMainServiceBase.h>
#include <e32std.h>
#include <e32base.h>
#include <ecom/implementationinformation.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Handler for ASY instance
*  One handler exists for each ASY
*  Loads ASY into the new sub thread.
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYProxyASYHandler : public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CASYProxyASYHandler* NewL(
                CASYProxy* aASYProxy,
                const TInt aIndex,
                const TUid aHandlerUid,
                const CImplementationInformation& aImplementationInformation );

        /**
        * Destructor.
        */
        virtual ~CASYProxyASYHandler();

    public: // New functions

        /**
        * Sets ASY's state.
        * @since S60 3.1
        * @param aASYState State to be set for ASY
        * @return void
        */
        void UpdateASYStateL( TASYProxyASYState aASYState );

        /**
        * Gets the current status of ASY.
        * @since S60 3.1
        * @param void
        * @return Current status of ASY
        */
        TASYProxyASYState CurrentState() const;
        
        /**
        * Gets the index of ASY.
        * @since S60 3.1
        * @param void
        * @return Index of this ASY
        */
        TInt Index() const;

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes
 
    private:

        /**
        * C++ default constructor.
        */
        CASYProxyASYHandler(
            CASYProxy* aASYProxy,
            const TInt aIndex,
            const TUid aHandlerUid,
            const CImplementationInformation& aImplementationInformation );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        void SetMemoryValues();
        TInt ReadMemoryItem( const TDesC8& aOpaqData, const TDesC8& aMemKey );
        void LoadASYL();

    public:     // Data

    protected:  // Data

    private:    // Data

        // Pointer to the main service of ASY
        CASYProxyMainService* iASYServiceMain; // Owned

        // Implementation uid of ASYCommandHandlerBase -ASY.
        TUid iHandlerUid;

        // Pointer to ASYProxy.
        CASYProxy* iASYProxy; // Not owned

        // Current status of ASY.
        TASYProxyASYState iASYState;

        // Static information about the ASY.
        CImplementationInformation* iImplementationInformation;

        // Zero based index of this ASY in array of handlers.
        TInt iIndex;

        // ASY's thread heap size
        TInt iHeapValue;

        // ASY's thread stack size
        TInt iStackValue;


    public:     // Friend classes

        friend const CImplementationInformation* CASYProxy::ASYInfo( TInt aIndex ) const;

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CASYPROXYASYHANDLER_H

// End of File
