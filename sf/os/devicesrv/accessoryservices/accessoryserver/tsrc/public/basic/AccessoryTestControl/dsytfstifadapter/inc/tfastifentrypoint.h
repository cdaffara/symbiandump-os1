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
* Description:  Declaration of CTFAStifEntryPoint class
*
*/


#ifndef __TFASTIFENTRYPOINT_H__
#define __TFASTIFENTRYPOINT_H__

#include "ctfaentrypoint.h"

NONSHARABLE_CLASS( CTFAStifEntryPoint ): public CTFAEntryPoint
    {
    public:
        /**
        * Creates a new entry point. Called from LibEntry
        */
        static CTFAStifEntryPoint* NewL( void );

        /**
        * Destructor
        */
        virtual ~CTFAStifEntryPoint( void );
    
    private:
        /**
        * Constructor
        */
        CTFAStifEntryPoint( void );

        /**
        * 2nd phase constructor
        */
        void ConstructL( void );

        /**
        * Copy constructor is hidden
        */
        CTFAStifEntryPoint( const CTFAStifEntryPoint& aEntry );

        /**
        * Assignment operator is hidden
        */
        CTFAStifEntryPoint& operator=( const CTFAStifEntryPoint& aEntry );
    
    public:
        /**
        * Creates a STIF test suite adapter.
        */
        MTFATestSuiteAdapter* CreateSuiteAdapterLC( CTFATestSuite& aSuite, TBool aRoot );

        /**
        * Creates a STIF test case adapter.
        */
        MTFATestCaseAdapter* CreateTestAdapterLC( CTFATestCase& aCase );

        /**
        * Creates STIF test environment.
        */
        CTFATestEnvironment* CreateEnvironmentL( void );
    };

#endif
