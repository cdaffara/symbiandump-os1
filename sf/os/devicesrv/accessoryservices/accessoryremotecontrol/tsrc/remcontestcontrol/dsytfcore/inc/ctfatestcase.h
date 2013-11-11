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
* Description:  Declaration of CTFATestCase class
*
*/


#ifndef __CTFATESTCASE_H__
#define __CTFATESTCASE_H__

#include "ctfatest.h"

/**
* Abstract base class for test cases
*/
class CTFATestCase : public CTFATest
    {
    protected:
        /**
        * Constructor
        * @param aCaseId The test case ID
        * @param aName The name of the test case
        */
        IMPORT_C CTFATestCase( TInt aCaseId, const TDesC& aName );

        /**
        * Destructor
        */
        IMPORT_C virtual ~CTFATestCase( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFATestCase( const CTFATestCase& aCase );

        /**
        * Assignment operator is hidden
        */
        CTFATestCase& operator=( const CTFATestCase& aCase );
    
    public:
        /**
        * Returns the test case ID
        */
        IMPORT_C TInt Id( void ) const;

        /**
        * Returns ETFATestTypeCase. Subclasses override this if needed.
        */
        IMPORT_C TTFATestType Type( void ) const;

        /**
        * Sets a new test timeout. Default timeout is 30 seconds.
        */
        IMPORT_C void SetTimeout( TInt aTimeout );

        /**
        * Sets a autorun on startup flag. This adapter library runs
        * the test case automatically on startup if this flag is set.
        */
        IMPORT_C void SetRunOnStartup( TBool aFlag );

        /**
        * Returns the test timeout
        */
        IMPORT_C TInt Timeout( void ) const;

        /**
        * Returns the autorun flag
        */
        IMPORT_C TBool RunOnStartup( void ) const;

        /**
        * Sets the test case flags
        */
        IMPORT_C void SetFlags( TUint32 aFlags );
    
        /**
        * Returns the test case flags
        */
        IMPORT_C TUint32 Flags( void ) const;

        /**
        * Sets the test environment
        */
        IMPORT_C void SetEnvironment( CTFATestEnvironment& aEnvironment );

    public:
        /**
        * Logs this test case
        */
        virtual void Log( TInt aDepth ) = 0;

        /**
        * Initializes this test case
        */
        virtual void SetupL( void ) = 0;

        /**
        * Runs this test case
        */
        virtual void RunTestL( void ) = 0;

        /**
        * Closes this test case
        */
        virtual void Teardown( void ) = 0;
    
    private:
        TInt iCaseId;
        TInt iTimeout;
        TBool iRunOnStartup;
        TUint32 iFlags;
    };

#endif
