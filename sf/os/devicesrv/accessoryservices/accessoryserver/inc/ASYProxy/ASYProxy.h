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
* Description:  Polymorphic interface to Accessory Server
*
*/


#ifndef CASYPROXY_H
#define CASYPROXY_H

// INCLUDES
#include "AsyProxyAPI.h"
#include "ASYProxyAsyLoader.h"
#include <e32svr.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CASYProxyASYHandler;

/**
*  Provides polymorphic interface to Accessory Server
*  for the module construction. 
*
*  @lib ASYProxy.dll
*  @since S60 3.1
*/
class CASYProxy : public CASYProxyAPI
    {
    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CASYProxy();

        /**
        * Destructor.
        */
        virtual ~CASYProxy();

    public: // New functions

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Creates and initializes loading objects.
        * @since S60 3.1
        * @param aAccServerObserver Observer class for call back methods.
        * @return void
        */
        void InitializeL( MASYProxyObserver* aAccServerObserver );

        /**
        * Begins to load ASYs.
        * @since S60 3.1
        * @param void
        * @return void
        */
        void LoadASYs();

        /**
        * Gets static information about ASY.
        * @since S60 3.1
        * @param aIndex Zero based index of ASY.
        * @return Implementation information.
        */
        const CImplementationInformation* ASYInfo( TInt aIndex ) const;

        /**
        * Gets dynamic information about ASY.
        * @since S60 3.1
        * @param aIndex Zero based index of ASY.
        * @return ASY's state information.
        */
        TASYProxyASYState ASYState( TInt aIndex ) const;

        /**
        * Gets ASY's count.
        * @since S60 3.1
        * @param void
        * @return Count of ASYs
        */
        TInt ASYsCount() const;

        /**
        * Sets the ASY's count.
        * @since S60 3.1
        * @param aCount Count of ASYs.
        * @return void
        */
        void SetASYsCount( TInt aCount );

        /**
        * Passes ASY loading completion status to Accessory Server
        * @since S60 3.1
        * @param aASYIndex Index of ASY to complete
        * @return void
        */
        void LoadCompleteL( const TInt aASYIndex );

        /**
        * Passes status to Accessory Server if ASYThread kills
        * @since S60 3.1
        * @param aASYIndex Index of killing ASY
        * @return void
        */
        void ThreadKills( const TInt aASYIndex );

        /**
        * Passes time out to Accessory Server
        * @since S60 3.1
        * @param void
        * @return void
        */
        void LoadTimeOutL();

    public:    // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

        /**
        * Deletes loader object
        * @since S60 3.1
        * @param void
        * @return void
        */
        void DestroyLoader();

    public:     // Data

    protected:  // Data

    private:    // Data

        // Array of ASY handler. CASYProxyAsyLoader uses.
        RPointerArray<CASYProxyASYHandler> iAccHandlerArray;

        // Pointer to loader of ASYs
        CASYProxyAsyLoader* iAsyLoader; // Owned

        // Pointer to observer class. 
        MASYProxyObserver* iAccServerObserver; // Not owned

        // Total count of "real" ASYs
        TInt iRealASYsCount;
        
        // Count of loaded "real" ASYs
        TInt iLoadedRealASYsCount;
        
        // Count of loaded  ASYs
        TInt iLoadedASYsCount;

    public:     // Friend classes

        friend void CASYProxyAsyLoader::DoIssueRequestL();
        friend void CASYProxyAsyLoader::ArrangeImplementationsL();

    protected:  // Friend classes

    private:    // Friend classes
};

#endif // CASYPROXY_H

// End of File
