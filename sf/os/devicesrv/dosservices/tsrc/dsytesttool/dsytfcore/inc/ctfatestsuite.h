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
* Description:  Declaration of CTFATestSuite class
*
*/


#ifndef __CTFATESTSUITE_H__
#define __CTFATESTSUITE_H__

#include "ctfatest.h"

/**
* Test suite implementation. A test suite may contain other test suites
* and test cases.
*/
class CTFATestSuite : public CTFATest
    {
    public:
        /**
        * Constructor
        * @param aName The test suite name
        */
        IMPORT_C CTFATestSuite( const TDesC& aName );

        /**
        * Destructor
        */
        IMPORT_C virtual ~CTFATestSuite( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFATestSuite( const CTFATestSuite& aSuite );

        /**
        * Assignment operator is hidden
        */
        CTFATestSuite& operator=( const CTFATestSuite& aSuite );
    
    public:
        /**
        * Adds a test or sub-suite to this suite
        */
        IMPORT_C void AddL( const CTFATest& aTest );

        /**
        * Returns ETFATestTypeSuite
        */
        IMPORT_C TTFATestType Type( void ) const;

        /**
        * Returns the number of test cases and sub-suites within this suite
        */
        IMPORT_C TInt Count( void ) const;

        /**
        * Returns the test case or sub-suite at given index
        */
        IMPORT_C CTFATest& At( TInt aIndex );
    
    private:
        RPointerArray<CTFATest> iTests;
    };

#endif
