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
* Description:  Implementation of CTFATestCaseRepeater class
*
*/


// INCLUDE FILES
#include <e32math.h>
#include "tftypes.h"
#include "ctfatestenvironment.h"
#include "ctfatestlog.h"
#include "ctfatestsuite.h"
#include "ctftestcasewrapper.h"
#include "ctftestcaserepeater.h"
#include "dsytesttooltrace.h"

// Default 30-s timeout is expected for each test case
const TInt KTFDefaultTimeout = 30;

// Each test case is run 3 * aRepeatCount times. The default 30-s timer
// is expected for each test case.
const TInt KTFLoopCount = 3;

// -----------------------------------------------------------------------------
// CTFTestCaseRepeater::CTFTestCaseRepeater
// -----------------------------------------------------------------------------
EXPORT_C CTFTestCaseRepeater::CTFTestCaseRepeater( TInt aId, const TDesC& aName, CTFATestSuite* aSuite, TInt aRepeatCount, TBool aRandomRepeat )
: CTFATestCase( aId, aName )
, iSuite( aSuite )
, iRepeatCount( aRepeatCount )
, iRandomRepeat( aRandomRepeat )
    {
    if ( aSuite != NULL )
        {
        if ( iRandomRepeat )
            {
            SetTimeout( aRepeatCount * aSuite->Count() * KTFDefaultTimeout );
            }
        else
            {
            SetTimeout( aRepeatCount * aSuite->Count() * KTFLoopCount * KTFDefaultTimeout );
            }
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    }
    

// -----------------------------------------------------------------------------
// CTFTestCaseRepeater::~CTFTestCaseRepeater
// -----------------------------------------------------------------------------
CTFTestCaseRepeater::~CTFTestCaseRepeater( void )
    {
    iSuite = NULL;
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseRepeater::SetEnvironment
// -----------------------------------------------------------------------------
EXPORT_C void CTFTestCaseRepeater::SetEnvironment( CTFATestEnvironment& aEnvironment )
    {
    CTFATestCase::SetEnvironment( aEnvironment );
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseRepeater::Log
// -----------------------------------------------------------------------------
void CTFTestCaseRepeater::Log( TInt aDepth )
    {
    _LIT ( KCaseDesc, "Case %d: %S" );
    Environment().Logger().WriteList( aDepth, KCaseDesc, Id(), &Name() );
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseRepeater::SetupL
// -----------------------------------------------------------------------------
void CTFTestCaseRepeater::SetupL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseRepeater::SetupL()" ) ) );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseRepeater::SetupL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseRepeater::RunTestL
// -----------------------------------------------------------------------------
void CTFTestCaseRepeater::RunTestL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseRepeater::RunTestL()" ) ) );
    if ( iSuite != NULL )
        {
        RHeap& heap = User::Heap();
        TInt heapSizeStart = 0;
        TInt heapCountStart = 0;
        TInt err;
        heapCountStart = heap.AllocSize( heapSizeStart );
        if ( iRandomRepeat )
            {
            TTime time;
            time.HomeTime();
            TInt64 seed = time.Int64();
            for ( TInt i = 0; i < iRepeatCount; i++ )
                {
                TInt index = Math::Rand( seed ) % iSuite->Count();
                CTFATest& test = iSuite->At( index );
                // The test suites and repeater test cases are not run
                if ( test.Type() != ETFTestTypeStubRepeater && test.Type() != ETFATestTypeSuite )
                    {
                    STATIC_CAST( CTFATestCase*, &test )->SetupL();
                    TRAP( err, STATIC_CAST( CTFATestCase*, &test )->RunTestL() );
                    STATIC_CAST( CTFATestCase*, &test )->Teardown();
                    User::LeaveIfError( err );
                    }
                else
                    {
                    i--;
                    }
                }
            }
        else
            {
            TInt i;
            TInt j;
            // Run each test repeat count times.
            for ( i = 0; i < iSuite->Count(); i++ )
                {
                CTFATest& test = iSuite->At( i );
                // The test suites and repeater test cases are not run
                if ( test.Type() != ETFTestTypeStubRepeater && test.Type() != ETFATestTypeSuite )
                    {
                    for ( j = 0; j < iRepeatCount; j++ )
                        {
                        STATIC_CAST( CTFATestCase*, &test )->SetupL();
                        TRAP( err, STATIC_CAST( CTFATestCase*, &test )->RunTestL() );
                        STATIC_CAST( CTFATestCase*, &test )->Teardown();
                        User::LeaveIfError( err );
                        }
                    }
                }
            // Run the entire test suite repeat count times starting from the last test.
            for ( j = 0; j < iRepeatCount; j++ )
                {
                for ( i = iSuite->Count() - 1; i >= 0; i-- )
                    {
                    CTFATest& test = iSuite->At( i );
                    // The test suites and repeater test cases are not run
                    if ( test.Type() != ETFTestTypeStubRepeater && test.Type() != ETFATestTypeSuite )
                        {
                        STATIC_CAST( CTFATestCase*, &test )->SetupL();
                        TRAP( err, STATIC_CAST( CTFATestCase*, &test )->RunTestL() );
                        STATIC_CAST( CTFATestCase*, &test )->Teardown();
                        User::LeaveIfError( err );
                        }
                    }
                }
            // Run the entire test suite repeat count times starting from the first test.
            for ( j = 0; j < iRepeatCount; j++ )
                {
                for ( i = 0; i < iSuite->Count(); i++ )
                    {
                    CTFATest& test = iSuite->At( i );
                    // The test suites and repeater test cases are not run
                    if ( test.Type() != ETFTestTypeStubRepeater && test.Type() != ETFATestTypeSuite )
                        {
                        STATIC_CAST( CTFATestCase*, &test )->SetupL();
                        TRAP( err, STATIC_CAST( CTFATestCase*, &test )->RunTestL() );
                        STATIC_CAST( CTFATestCase*, &test )->Teardown();
                        User::LeaveIfError( err );
                        }
                    }
                }
            }
        TInt heapSizeEnd;
        TInt heapCountEnd = heap.AllocSize( heapSizeEnd );
        if ( heapCountEnd != heapCountStart )
            {
            COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - Allocated memory %d times: Total %d bytes\r\n" ), heapCountEnd - heapCountStart, heapSizeEnd - heapSizeStart ) );
            }
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseRepeater::RunTestL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseRepeater::Teardown
// -----------------------------------------------------------------------------
void CTFTestCaseRepeater::Teardown( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseRepeater::Teardown()" ) ) );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestCaseRepeater::Teardown - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFTestCaseRepeater::Type
// -----------------------------------------------------------------------------
TTFATestType CTFTestCaseRepeater::Type( void ) const
    {
    return ( TTFATestType )ETFTestTypeStubRepeater;
    }


// -----------------------------------------------------------------------------
// CTFTestCaseRepeater::Logger
// -----------------------------------------------------------------------------
CTFATestLog& CTFTestCaseRepeater::Logger( void )
    {
    return Environment().Logger();
    }


