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
* Description:  Declaration of CTFDosServerControlTestCase class
*
*/


#ifndef __CTFDOSSERVERCONTROLTESTCASE_H__
#define __CTFDOSSERVERCONTROLTESTCASE_H__

#include <mtfaactivetest.h>
#include "ctfstubtestcase.h"
#include "tfdosservertypes.h"

class CTFATestTimer;
class CTFDosServerControlTestCaseParam;

/**
* Test case declaration macro
*/
#define TF_DOS_SERVER_CONTROL_TEST_CASE_L( aCaseId, aCaseStates ) \
    case aCaseId: \
        { \
        CTFDosServerControlTestCaseParam* param = new ( ELeave ) CTFDosServerControlTestCaseParam(); \
        CleanupStack::PushL( param ); \
        param->ConstructL( aCaseStates, sizeof ( aCaseStates ) / sizeof ( TTFDosServerControlTestCaseState ) ); \
        returnValue = new ( ELeave ) CTFDosServerControlTestCase( param ); \
        CleanupStack::Pop(); \
        CleanupStack::PushL( returnValue ); \
        STATIC_CAST( CTFDosServerControlTestCase*, returnValue )->ConstructL(); \
        CleanupStack::Pop(); \
        } \
        break

NONSHARABLE_CLASS( CTFDosServerControlTestCase ): public CTFStubTestCase, public MTFAActiveTest
    {
    public:
        /**
        * Constructor
        */
        CTFDosServerControlTestCase( CTFDosServerControlTestCaseParam* aParameters );

        /**
        * 2nd phase constructor
        */
        void ConstructL( void );

        /**
        * Destructor
        */
        ~CTFDosServerControlTestCase( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFDosServerControlTestCase( const CTFDosServerControlTestCase& aStub );

        /**
        * Assignment operator is hidden
        */
        CTFDosServerControlTestCase& operator=( const CTFDosServerControlTestCase& aStub );
    
    public:
        /**
        * Notifies about a DosServer event
        */
        void NotifyDosEvent( TInt aEvent, TInt aParameter );

    public:
        /**
        * Timer callback to cancel test if it takes too long
        */
        void ActiveTestRunL( void );

    protected:
        /**
        * Logs this test case
        */
        void Log( TInt aDepth );

        /**
        * Initializes this test case
        */
        void InitL( void );

        /**
        * Runs this test case
        */
        void RunL( void );

        /**
        * Releases this test case
        */
        void Teardown( void );

    protected:
        /**
        * Completes this test case with given result
        */
        void DoCompleteTest( TInt aResult );

        /**
        * Starts the next test case state
        */
        void StartNextState( TInt aResult );

        /**
        * Calls the DosServer function of the current test case state
        */
        void CallCurrentDosFunctionL( void );

        /**
        * Gets the index of current state
        */
        TInt CurrentStateIndex( void ) const;

        /**
        * Gets the test case parameters
        */
        CTFDosServerControlTestCaseParam& Parameters( void );

        /**
        * Gets the current DosServer function
        */
        TTFDosFunction CurrentDosFunction( void ) const;

        /**
        * Gets the current argument 1
        */
        TInt CurrentArg1( void ) const;

        /**
        * Gets the timer
        */
        CTFATestTimer* Timer( void );

    private:
        /**
        * Checks a test result
        */
        TInt CheckResult( TInt aResult, TBool aIsEvent );
    
    private:
        CTFDosServerControlTestCaseParam* iParameters;
        TTFDosServerControlTestCaseState iCurrentState;
        TInt iCurrentStateIndex;
        CTFATestTimer* iTimer;
        TBool iCleanupWait;
        TInt iStoredArg1;
        TInt iStoredArg2;
    };

#endif
