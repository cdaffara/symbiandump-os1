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
* Description:  API class for ASYProxy
*
*/


#ifndef CASYPROXYAPI_H
#define CASYPROXYAPI_H

// INCLUDES
#include "AccSrvProxy.h"
#include <ecom/implementationinformation.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MASYProxyObserver;

// CLASS DECLARATION

/**
*  ASYProxy API definition
*  @lib AccServer.exe
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CASYProxyAPI ) : public CBase
    {
    public: // Constructors and destructor

    public: // New functions

        /**
        * Load ASYs
        * @since S60 3.1
        * @param void
        * @return void
        */
        virtual void LoadASYs() = 0;

        /**
        * Initializes loading object of ASYs
        * @since S60 3.1
        * @param aAccServerObserver Poinetr to a observer class for call back methods.
        * @return void
        */
        virtual void InitializeL( MASYProxyObserver* aAccServerObserver ) = 0;

        /**
        * Gets ASYInfo
        * @since S60 3.1
        * @param aIndex Zero based index of ASY.
        * @return Implementation information.
        */
        virtual const CImplementationInformation* ASYInfo( TInt aIndex ) const = 0;

        /**
        * Gets status of ASY
        * @since S60 3.1
        * @param aIndex Zero based index of ASY.
        * @return ASY's state information.
        */
        virtual TASYProxyASYState ASYState( TInt aIndex ) const = 0;

        /**
        * Gets ASYs count
        * @since S60 3.1
        * @param void
        * @return Count of ASYs.
        */
        virtual TInt ASYsCount() const = 0;

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

#endif // CASYPROXYAPI_H

// End of File
