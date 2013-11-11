/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CTFStubModuleInterface class
*
*/


#ifndef __CTFSTUBMODULEINTERFACE_H__
#define __CTFSTUBMODULEINTERFACE_H__

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <badesca.h>
#include "ctfstub.h"
#include "ctfatestsuite.h"

// MACROS

// CONSTANTS
const TUid KCTFStubModuleInterfaceUid = { 0x10200c7d };

// CLASS DECLARATION

/**
*  An abstract base class being representative of the concrete class which
*  the client wishes to use.
*/
NONSHARABLE_CLASS( CTFStubModuleInterface ): public CBase
    {
    public:        // Data types
        /**
        * For passing initialisation parameters to the derived class
        * constructor.
        */
        struct TInterfaceInitParams
            {
            TInt integer;
            const TDesC* descriptor;
            };

    public:        // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aMatchString Resolver parameters.
        * @return The created object.
        */
        static CTFStubModuleInterface* NewL( const TDesC8& aMatchString );

        /**
        * Destructor
        */
        virtual ~CTFStubModuleInterface();

    public:        // New functions
        /**
        * Request a list of all available implementations which satisfy
        * this given interface.
        * @param aImplInfoArray A pointer to the available implementations.
        */
        static void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray );

    public:     // Virtual functions to be implemented
        /**
        * Gets the stub implementation of this plug-in module
        */
        virtual CTFStub* GetStubL( void ) = 0;

        /**
        * Builds the test suite of this plug-in module.
        */
        virtual void BuildTestSuiteL( CTFATestSuite* aRootSuite ) = 0;

        /**
        * Gets the plug-in module specific part of a test case which has the given test case ID
        */
        virtual MTFStubTestCase* GetStubTestCaseL( TInt aTestCaseId ) = 0;

    protected:    // Constructor
        /**
        * C++ default constructor.
        */
        inline CTFStubModuleInterface( void );

    private:    // Data
        TUid iDtor_ID_Key;

    };

// INLINE FUNCTIONS
#include "ctfstubmoduleinterface.inl"

#endif    // __CTFSTUBMODULEINTERFACE_H__

// End of File
