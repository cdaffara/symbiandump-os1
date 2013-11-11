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
* Description:  Declaration of CTFTestCaseRepeater classes
*
*/


#ifndef __CTFTESTCASEREPEATER_H__
#define __CTFTESTCASEREPEATER_H__

#include "ctfatestcase.h"

class MTFStubControl;
class MTFStubTestCase;
class CTFATestSuite;
class CTFATestLog;

/**
* Implementation of repeater and randomizer test cases.
*/
class CTFTestCaseRepeater : public CTFATestCase
    {
    public:
        /**
        * Constructor
        * @param aCaseId The test case ID
        * @param aName The test case name
        * @param aSuite The test suite which contains the tests to be run with this repeater
        * @param aRepeatCount Number of times to run tests
        * @param aRandomRepeat If ETrue, tests are run in random order
        */
        IMPORT_C CTFTestCaseRepeater( TInt aCaseId, const TDesC& aName, CTFATestSuite* aSuite, TInt aRepeatCount, TBool aRandomRepeat );

        /**
        * Destructor
        */
        virtual ~CTFTestCaseRepeater( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFTestCaseRepeater( const CTFTestCaseRepeater& aCase );

        /**
        * Assignment operator is hidden
        */
        CTFTestCaseRepeater& operator=( const CTFTestCaseRepeater& aCase );
    
    public:
        /**
        * Sets the test environment to this test case
        */
        IMPORT_C void SetEnvironment( CTFATestEnvironment& aEnvironment );
    
    public:
        /**
        * Returns ETFTestTypeStubRepeater
        */
        TTFATestType Type( void ) const;

        /**
        * Logs this test case
        */
        void Log( TInt aDepth );

        /**
        * Prepares each test case within the test suite
        */
        void SetupL( void );

        /**
        * Runs the test cases
        */
        void RunTestL( void );

        /**
        * Closes each test case within the test suite
        */
        void Teardown( void );
    
    private:
        /**
        * Returns the test logger
        */
        CTFATestLog& Logger( void );

    private:
        CTFATestSuite* iSuite;
        TInt iRepeatCount;
        TBool iRandomRepeat;
    };

#endif
