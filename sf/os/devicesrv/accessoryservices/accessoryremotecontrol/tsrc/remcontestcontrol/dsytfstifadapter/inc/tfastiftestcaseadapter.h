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
* Description:  Declaration of CTFAStifTestCaseAdapter class
*
*/


#ifndef __TFASTIFTESTCASEADAPTER_H__
#define __TFASTIFTESTCASEADAPTER_H__

#include <stiftestinterface.h>
#include "ctfatestcaseadapter.h"

class CTFATestCase;

NONSHARABLE_CLASS( CTFAStifTestCaseAdapter ): public CTFATestCaseAdapter
    {
    public:
        /**
        * Creates a new test case adapter
        */
        static CTFAStifTestCaseAdapter* NewLC( CTFATestCase& aTestCase, CTestModuleBase* aTestModule );

        /**
        * Destructor
        */
        virtual ~CTFAStifTestCaseAdapter( void );
    
    private:
        /**
        * Constructor
        */
        CTFAStifTestCaseAdapter( CTFATestCase& aTestCase, CTestModuleBase* aTestModule );

        /**
        * 2nd phase constructor
        */
        void ConstructL( void );

        /**
        * Copy constructor is hidden
        */
        CTFAStifTestCaseAdapter( const CTFAStifTestCaseAdapter& aAdapter );

        /**
        * Assignment operator is hidden
        */
        CTFAStifTestCaseAdapter& operator=( const CTFAStifTestCaseAdapter& aAdapter );
    
    public:
        /**
        * Determines if the case is run on startup
        */
        TBool RunOnStartup( void ) const;

        /**
        * Returns the STIF-TF test info
        */
        TTestCaseInfo TestInfo( void ) const;

        /**
        * Runs this test
        */
        void RunTestL( TTestResult& aResult );
    
    private:
        TTestCaseInfo iTestInfo;
        CTestModuleBase* iTestModule;
    };

#endif
