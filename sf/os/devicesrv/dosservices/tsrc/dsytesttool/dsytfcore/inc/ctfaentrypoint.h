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
* Description:  Declaration of CTFAEntryPoint class
*
*/


#ifndef __CTFAENTRYPOINT_H__
#define __CTFAENTRYPOINT_H__

#include <e32base.h>

class CTFATestSuite;
class CTFATestCase;
class CTFATestEnvironment;
class MTFATestSuiteAdapter;
class MTFATestCaseAdapter;
class MTFATestCaseInterface;

/**
* Entry point is inherited in the adapter DLL. ConstructL builds the test suite
* and adapter suite.
*/
//NONSHARABLE_CLASS( CTFAEntryPoint ): public CBase
class CTFAEntryPoint : public CBase
    {
    protected:
        /**
        * Default constructor
        */
        IMPORT_C CTFAEntryPoint( void );
    
    public:
        /**
        * Destructor
        */
        IMPORT_C virtual ~CTFAEntryPoint( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFAEntryPoint( const CTFAEntryPoint& aEntryPoint );

        /**
        * Assignment operator is hidden
        */
        CTFAEntryPoint& operator=( const CTFAEntryPoint& aEntryPoint );
    
    public:
        /**
        * Returns the root test suite adapter
        */
        IMPORT_C MTFATestSuiteAdapter* RootSuiteAdapter( void );
    
    protected:
        /**
        * Symbian constructor
        */
        IMPORT_C virtual void ConstructL( void );
    
    protected:
        /**
        * Creates a test suite adapter for the given test suite. 
        * The returned object is freed by the test framework
        * @param CTFATestSuite& aSuite The test suite
        * @param TBool aRoot A flag that determines if the test suite is the root suite
        * @return The adapter object
        */
        virtual MTFATestSuiteAdapter* CreateSuiteAdapterLC( CTFATestSuite& aSuite, TBool aRoot ) = 0;

        /**
        * Creates a test case adapter for given test case. 
        * The returned object is freed by the test framework
        * @param CTFATestCase& aCase The test case
        * @return The adapter object
        */
        virtual MTFATestCaseAdapter* CreateTestAdapterLC( CTFATestCase& aCase ) = 0;

        /**
        * Creates the test environment object. 
        * The returned object is freed by the test framework
        */
        virtual CTFATestEnvironment* CreateEnvironmentL( void ) = 0;

    private:
        /**
        * Creates adapter objects for the given test suite. This is called recursively for
        * each test suite within the given test suite.
        * @param CTFATestSuite& aSuite The test suite
        * @param MTFATestSuiteAdapter& aAdapter The adapter suite
        */
        void BuildAdapterSuiteL( CTFATestSuite& aSuite, MTFATestSuiteAdapter& aAdapter );

        /**
        * Called when the startup timer set in ConstructL fires
        * @param aParameter Not used
        * @return EFalse so that timer does not continue
        */
        static TInt TimerCallback( TAny* aParameter );

    private:
        CTFATestSuite* iRootSuite;
        MTFATestSuiteAdapter* iRootSuiteAdapter;
        CTFATestEnvironment* iEnvironment;
        TInt iDepth;
    };

#endif
