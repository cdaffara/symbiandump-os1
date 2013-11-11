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
* Description:  Declaration of CTFAStifTestSuiteAdapterRoot class
*
*/


#ifndef __TFASTIFTESTSUITEADAPTERROOT_H__
#define __TFASTIFTESTSUITEADAPTERROOT_H__

#include "stiftestinterface.h"
#include "mtfatestsuiteadapter.h"

class CTFAStifTestCaseAdapter;
class CTFAStifTestSuiteAdapter;
class CTFATestSuite;
class CTFAEntryPoint;

/**
* This cannot inherit from CTFATestSuiteAdapter, since that would 
* result in multiple paths to CBase root class. Thus this needs
* to reimplement the MTFATestSuiteAdapter interface.
*/
NONSHARABLE_CLASS( CTFAStifTestSuiteAdapterRoot ): public CTestModuleBase, public MTFATestSuiteAdapter
    {
    public:
        /**
        * Creates a new test suite adapter root that wraps the given test suite
        */
        static CTFAStifTestSuiteAdapterRoot* NewLC( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint );

        /**
        * Destructor
        */
        virtual ~CTFAStifTestSuiteAdapterRoot( void );
    
    private:
        /**
        * Constructor
        */
        CTFAStifTestSuiteAdapterRoot( void );

        /**
        * 2nd phase constructor
        */
        void ConstructL( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint );
        
        /**
        * Copy constructor is hidden
        */
        CTFAStifTestSuiteAdapterRoot( const CTFAStifTestSuiteAdapterRoot& aAdapter );

        /**
        * Assignment operator is hidden
        */
        CTFAStifTestSuiteAdapterRoot& operator=( const CTFAStifTestSuiteAdapterRoot& aAdapter );
    
    public:
        /**
        * Returns test suite type
        */
        TTFATestType Type( void ) const;

        /**
        * Adds a sub-suite / test case
        */
        void AddL( MTFATestAdapter& aAdapter );

        /**
        * Adds a test case to this suite
        */
        void AddTestL( const CTFAStifTestCaseAdapter& aAdapter );

        /**
        * Gets the test cases. STIF-TF calls this
        */
        TInt GetTestCasesL( const TFileName& aIniFile, RPointerArray<TTestCaseInfo> &aCases );

        /**
        * Runs a test case. STIF-TF calls this
        */
        TInt RunTestCaseL( const TInt aCaseNumber, const TFileName& aIniFile, TTestResult& aResult );
    
    private:
        CTFAStifTestSuiteAdapter* iWrappedAdapter;
        RPointerArray<CTFAStifTestCaseAdapter> iTestCases;
        TBool iFirstTest;
    };
    
#endif
