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
* Description:  Declaration of CTFATestEnvironment class
*
*/


#ifndef __CTFATESTENVIRONMENT_H__
#define __CTFATESTENVIRONMENT_H__

#include <e32base.h>

class CTFATestLog;
class CTFFactory;
class CTFActiveScheduler;

/**
* Test environment contains common data available to all test cases. This
* is an abstract base class and needs to be implemeted in the adapter DLL.
*/
//NONSHARABLE_CLASS( CTFATestEnvironment ): public CBase
class CTFATestEnvironment : public CBase
    {
    public:
        /**
        * Constructor
        * @param aLogger The test logger
        */
        IMPORT_C CTFATestEnvironment( CTFATestLog& aLogger );

        /**
        * Destructor
        */
        IMPORT_C ~CTFATestEnvironment( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFATestEnvironment( const CTFATestEnvironment& aCase );

        /**
        * Assignment operator is hidden
        */
        CTFATestEnvironment& operator=( const CTFATestEnvironment& aCase );
    
    public:
        /**
        * Returns the test logger
        */
        IMPORT_C CTFATestLog& Logger( void );

        /**
        * Gets the factory object
        */
        IMPORT_C CTFFactory* Factory( void );

        /**
        * Sets global test case flags.
        * These can be overridden by test-specific flags.
        */
        IMPORT_C void SetTestCaseFlags( TUint32 aTestCaseFlags );
    
    public:
        /**
        * Sets the factory object
        */
        void SetFactory( CTFFactory* aFactory );

        /**
        * Stores the active scheduler of the test framework.
        */
        void SetScheduler( CTFActiveScheduler* aScheduler );

        /**
        * Gets the test case flags
        */
        TUint32 TestCaseFlags( void ) const;

    private:
        CTFFactory* iFactory;
        CTFActiveScheduler* iScheduler;
        CTFATestLog* iLogger;
        TUint32 iTestCaseFlags;
    };

#endif
