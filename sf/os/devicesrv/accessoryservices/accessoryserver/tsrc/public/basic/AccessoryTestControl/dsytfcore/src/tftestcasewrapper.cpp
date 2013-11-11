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
* Description:  Implementation of CTFATestCaseWrapper class
*
*/


// INCLUDE FILES
#include "ctfatestlog.h"
#include "mtfstubtestcase.h"
#include "ctffactory.h"
#include "ctfatestenvironment.h"
#include "tftypes.h"
#include "ctftestcasewrapper.h"
#include "ctfatesttimer.h"
#include "dsytesttooltrace.h"
#include "mtfstubcontrol.h"

// -----------------------------------------------------------------------------
// MTFTestCaseInterface::MTFTestCaseInterface
// -----------------------------------------------------------------------------
MTFTestCaseInterface::MTFTestCaseInterface( void )
    {
    }


// -----------------------------------------------------------------------------
// MTFTestCaseInterface::~MTFTestCaseInterface
// -----------------------------------------------------------------------------
MTFTestCaseInterface::~MTFTestCaseInterface( void )
    {
    }


// -----------------------------------------------------------------------------
// MTFTestCase::MTFTestCase
// -----------------------------------------------------------------------------
MTFTestCase::MTFTestCase( void )
    {
    }


// -----------------------------------------------------------------------------
// MTFTestCase::~MTFTestCase
// -----------------------------------------------------------------------------
MTFTestCase::~MTFTestCase( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::CTFTestCaseWrapper
// -----------------------------------------------------------------------------
EXPORT_C CTFTestCaseWrapper::CTFTestCaseWrapper( TInt aId, const TDesC& aName )
: CTFATestCase( aId, aName )
    {
    }
    

// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::~CTFTestCaseWrapper
// -----------------------------------------------------------------------------
CTFTestCaseWrapper::~CTFTestCaseWrapper( void )
    {
    // Deletes the stub test cases
    delete iTimer;
    iTestCases.ResetAndDestroy();
    iStubControl = NULL;
    iFactory = NULL;
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::SetEnvironment
// -----------------------------------------------------------------------------
EXPORT_C void CTFTestCaseWrapper::SetEnvironment( CTFATestEnvironment& aEnvironment )
    {
    CTFATestCase::SetEnvironment( aEnvironment );
    iFactory = aEnvironment.Factory();
    TRACE_ASSERT( iFactory != NULL );
    __ASSERT_ALWAYS( iFactory != NULL, User::Panic( KTFName, ETFPanicFactory ) );
    iStubControl = iFactory->StubControl();
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::TestCaseID
// -----------------------------------------------------------------------------
TInt CTFTestCaseWrapper::TestCaseID( void ) const
    {
    return Id();
    }


// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::TestCase
// -----------------------------------------------------------------------------
MTFStubTestCase* CTFTestCaseWrapper::TestCase( TInt aTestCaseType )
    {
    MTFStubTestCase* retval = NULL;
    for ( TInt i = 0; i < iTestCases.Count() && retval == NULL; i++ )
        {
        MTFStubTestCase* testcase = iTestCases[i];
        if ( testcase->Type() == aTestCaseType )
            {
            retval = testcase;
            }
        }
    return retval;
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::AddStubTestCaseL
// -----------------------------------------------------------------------------
EXPORT_C void CTFTestCaseWrapper::AddStubTestCaseL( MTFStubTestCase& aTestCase )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::AddStubTestCaseL(0x%x)" ), &aTestCase ) );
    for ( TInt i = 0; i < iTestCases.Count(); i++ )
        {
        if ( iTestCases[i]->Type() == aTestCase.Type() )
            {
            TRACE_ASSERT_ALWAYS;
            User::Panic( KTFName, ETFPanicDuplicateCase );
            }
        }
    if ( iTimer == NULL )
        {
        iTimer = CTFATestTimer::NewL( *this );
        }
    User::LeaveIfError( iTestCases.Append( &aTestCase ) );
    aTestCase.SetCallback( *this );
    CTFStub* stub = iStubControl->Stub( aTestCase.Type () );
    TRACE_ASSERT( stub != NULL );
    __ASSERT_ALWAYS( stub != NULL, User::Panic( KTFName, ETFPanicStubMissing ) );
    aTestCase.SetStub( *stub );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::AddStubTestCaseL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::Log
// -----------------------------------------------------------------------------
void CTFTestCaseWrapper::Log( TInt aDepth )
    {
    if ( iTestCases.Count() > 0 )
        {
        _LIT ( KCaseDesc, "Case %d: %S, %d components" );
        Environment().Logger().WriteList( aDepth, KCaseDesc, Id(), &Name(), iTestCases.Count() );
        for ( TInt i = 0; i < iTestCases.Count(); i++ )
            {
            iTestCases[i]->Log( aDepth + 1 );
            }
        }
    else
        {
        _LIT( KCaseDesc, "Case %d: %S, no components" );
        Environment().Logger().WriteList( aDepth, KCaseDesc, Id(), &Name() );
        }
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::SetupL
// -----------------------------------------------------------------------------
void CTFTestCaseWrapper::SetupL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::SetupL()" ) ) );
    iTestResult = KErrNone;
    iStubControl->SetCurrentTestCase( this );
    for ( TInt i = 0; i < iTestCases.Count(); i++ )
        {
        TRAPD( err, iTestCases[i]->SetupL() );
        if ( err != KErrNone )
            {
            // If one setup leaves, the previous ones need to be torn down
            for ( TInt j = 0; j < i; j++ )
                {
                iTestCases[j]->Teardown();
                }
            i = iTestCases.Count();
            }
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::SetupL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::RunTestL
// -----------------------------------------------------------------------------
void CTFTestCaseWrapper::RunTestL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::RunTestL()" ) ) );
#ifdef _DEBUG
#ifdef TEST_NAME_TRACE_FLAG
    RDebug::Print( _L( "    DSYTESTTOOL - Running test case [%S]" ), &Name() );
#endif
#endif
    TBool needsToWait = EFalse;
    for ( TInt i = 0; i < iTestCases.Count(); i++ )
        {
        iTestCases[i]->RunL();
        if ( !iTestCases[i]->IsComplete() )
            {
            needsToWait = ETrue;
            }
        }
    if ( iTestResult == KErrNone && needsToWait )
        {
        iWaitStarted = ETrue;
        COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - Test case [%S] in active wait" ), &Name() ) );
        iTimer->After( TTimeIntervalMicroSeconds32( Timeout() * 1000000L ) );
        CActiveScheduler::Start();
        }
#ifdef _DEBUG
#ifdef TEST_NAME_TRACE_FLAG
    RDebug::Print( _L( "    DSYTESTTOOL - Test case [%S] finished with %d" ), &Name(), iTestResult );
#endif
#endif
    User::LeaveIfError( iTestResult );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::RunTestL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::ActiveTestRunL
// -----------------------------------------------------------------------------
void CTFTestCaseWrapper::ActiveTestRunL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::ActiveTestRunL()" ) ) );
    TestComplete( KErrTimedOut );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::ActiveTestRunL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::Teardown
// -----------------------------------------------------------------------------
void CTFTestCaseWrapper::Teardown( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::Teardown()" ) ) );
    iTimer->Cancel();
    for ( TInt i = 0; i < iTestCases.Count(); i++ )
        {
        iTestCases[i]->Teardown();
        }
    iStubControl->SetCurrentTestCase( NULL );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::Teardown - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::TestComplete
// -----------------------------------------------------------------------------
void CTFTestCaseWrapper::TestComplete( TInt aTestResult )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::TestComplete(%d)" ), aTestResult ) );
    if ( aTestResult != KErrNone )
        {
        if ( iTestResult == KErrNone )
            {
            iTestResult = aTestResult;
            }
        else
            {
            COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::TestComplete() - Multiple errors, new error ignored. Current = %d, new = %d" ), iTestResult, aTestResult ) );
            }
        }
    if ( iWaitStarted )
        {
        TBool canStop = ETrue;
        // If the test fails, it can be terminated.
        if ( iTestResult == KErrNone )
            {
            for ( TInt i = 0; i < iTestCases.Count(); i++ )
                {
                if ( !iTestCases[i]->IsComplete() )
                    {
                    canStop = EFalse;
                    }
                }
            }
        if ( canStop )
            {
            COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseWrapper::TestComplete() - End active wait" ) ) );
            CActiveScheduler::Stop();
            iWaitStarted = EFalse;
            }
        }
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::Logger
// -----------------------------------------------------------------------------
CTFATestLog& CTFTestCaseWrapper::Logger( void )
    {
    return Environment().Logger();
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseWrapper::Type
// -----------------------------------------------------------------------------
TTFATestType CTFTestCaseWrapper::Type( void ) const
    {
    return ( TTFATestType )ETFTestTypeStubWrapper;
    }


