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
* Description:  Declaration of CTFTestCaseWrapper classes
*
*/


#ifndef __CTFTESTCASEWRAPPER_H__
#define __CTFTESTCASEWRAPPER_H__

#include "ctfatestcase.h"
#include "mtftestcase.h"
#include "mtftestcaseinterface.h"
#include "mtfaactivetest.h"

class MTFStubControl;
class MTFStubTestCase;
class CTFFactory;
class CTFATestTimer;

/**
* Implementation of a test case that contains plug-in specific parts.
*/
class CTFTestCaseWrapper : public CTFATestCase, public MTFTestCase, public MTFTestCaseInterface, public MTFAActiveTest
    {
    public:
        /**
        * Constructor.
        * @param aCaseId The test case identifier
        * @param aName The name of the test case
        */
        IMPORT_C CTFTestCaseWrapper( TInt aCaseId, const TDesC& aName );

        /**
        * Destructor
        */
        virtual ~CTFTestCaseWrapper( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFTestCaseWrapper( const CTFTestCaseWrapper& aCase );

        /**
        * Assignment operator is hidden
        */
        CTFTestCaseWrapper& operator=( const CTFTestCaseWrapper& aCase );
    
    public:
        /**
        * Adds a plug-in specific part to this test case
        */
        IMPORT_C void AddStubTestCaseL( MTFStubTestCase& aTestCase );

        /**
        * Sets the test environment to this test case
        */
        IMPORT_C void SetEnvironment( CTFATestEnvironment& aEnvironment );

    public:
        /**
        * Returns the plug-in specific part that has the given plug-in identifier
        */
        MTFStubTestCase* TestCase( TInt aTestCaseType );

        /**
        * Returns the ID of this test case
        */
        TInt TestCaseID( void ) const;

        /**
        * Called by the plug-in specific parts when they complete
        */
        void TestComplete( TInt aTestResult );

        /**
        * Returns the test case logger
        */
        CTFATestLog& Logger( void );

        /**
        * Logs this test case
        */
        void Log( TInt aDepth );

        /**
        * Initializes this test case
        */
        void SetupL( void );

        /**
        * Runs this test case
        */
        void RunTestL( void );

        /**
        * Closes this test case
        */
        void Teardown( void );

        /**
        * Returns ETFTestTypeStubWrapper
        */
        TTFATestType Type( void ) const;

        /**
        * Timeout handler
        */
        void ActiveTestRunL( void );
    
    private:
        MTFStubControl* iStubControl;
        CTFFactory* iFactory;
        CTFATestTimer* iTimer;
        RPointerArray<MTFStubTestCase> iTestCases;
        TBool iWaitStarted;
        TInt iTestResult;
    };

#endif
