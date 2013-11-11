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
* Description:  Declaration of CTFATestSuiteAdapter class
*
*/


#ifndef __CTFATESTSUITEADAPTER_H__
#define __CTFATESTSUITEADAPTER_H__

#include <e32base.h>
#include "mtfatestsuiteadapter.h"

class CTFATestSuite;
class CTFAEntryPoint;

/**
* Base class for test suite adapter implementations
*/
//NONSHARABLE_CLASS( CTFATestSuiteAdapter ): public CBase, public MTFATestSuiteAdapter
class CTFATestSuiteAdapter : public CBase, public MTFATestSuiteAdapter
    {
    protected:
        /**
        * Constructor
        * @param aTestSuite The test suite to be wrapped into this adapter
        * @param aEntryPoint The entry point object. The entry point is non-NULL
        * if this adapter is the root adapter. The root adapter stores the entry
        * point and deletes it in its destructor. 
        */
        IMPORT_C CTFATestSuiteAdapter( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint );
    
    public:
        /**
        * Destructor
        */
        IMPORT_C ~CTFATestSuiteAdapter( void );

    private:
        /**
        * Copy constructor is hidden
        */
        CTFATestSuiteAdapter( const CTFATestSuiteAdapter& aAdapter );

        /**
        * Assignment operator is hidden
        */
        CTFATestSuiteAdapter& operator=( const CTFATestSuiteAdapter& aAdapter );

    public:
        /**
        * Adds a test adapter to this suite. The adapter may be a sub-suite or a test case
        */
        IMPORT_C virtual void AddL( MTFATestAdapter& aAdapter );

    public:
        /**
        * Returns ETFATestTypeSuite
        */
        IMPORT_C TTFATestType Type( void ) const;
    
    protected:
        CTFATestSuite& iTestSuite;
        RPointerArray<MTFATestAdapter> iTests;
        CTFAEntryPoint* iEntryPoint;
    };

#endif
