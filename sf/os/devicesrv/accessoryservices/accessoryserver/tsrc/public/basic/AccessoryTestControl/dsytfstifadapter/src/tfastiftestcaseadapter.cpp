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
* Description:  Implementation of CTFAStifTestCaseAdapter class
*
*/


// INCLUDE FILES
#include "ctfatestcase.h"
#include "dsytesttooltrace.h"
#include "tfastiftestcaseadapter.h"

const TInt KTFUsecMultiplier = 1000000L;
const TInt KTFTestKillMargin = 5000000L;


// -----------------------------------------------------------------------------
// CTFAStifTestCaseAdapter::NewLC
// -----------------------------------------------------------------------------
CTFAStifTestCaseAdapter* CTFAStifTestCaseAdapter::NewLC( CTFATestCase& aTestCase, CTestModuleBase* aTestModule )
    {
    CTFAStifTestCaseAdapter* adapter = new ( ELeave ) CTFAStifTestCaseAdapter( aTestCase, aTestModule );
    CleanupStack::PushL( adapter );
    adapter->ConstructL();
    return adapter;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestCaseAdapter::CTFAStifTestCaseAdapter
// -----------------------------------------------------------------------------
CTFAStifTestCaseAdapter::CTFAStifTestCaseAdapter( CTFATestCase& aTestCase, CTestModuleBase* aTestModule )
: CTFATestCaseAdapter( aTestCase )
, iTestModule( aTestModule )
    {
    }
    

// -----------------------------------------------------------------------------
// CTFAStifTestCaseAdapter::~CTFAStifTestCaseAdapter
// -----------------------------------------------------------------------------
CTFAStifTestCaseAdapter::~CTFAStifTestCaseAdapter( void )
    {
    iTestModule = NULL;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestCaseAdapter::ConstructL
// -----------------------------------------------------------------------------
void CTFAStifTestCaseAdapter::ConstructL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestCaseAdapter::ConstructL()" ) ) );
    if ( iTestCase.Name().Length() <= iTestInfo.iTitle.MaxLength() )
        {
        iTestInfo.iTitle.Copy( iTestCase.Name() );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        iTestInfo.iTitle.Copy( iTestCase.Name().Left( iTestInfo.iTitle.MaxLength() ) );
        }
    iTestInfo.iCaseNumber = iTestCase.Id();
    /* Leaves a 5-second margin for the test case to cancel 
       itself without the need to kill the test suite */
    TInt timeout = iTestCase.Timeout() * KTFUsecMultiplier + KTFTestKillMargin;
    TTimeIntervalMicroSeconds interval( timeout );
    iTestInfo.iTimeout = interval;
    iTestInfo.iPriority = TTestCaseInfo::EPriorityNormal;
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestCaseAdapter::ConstructL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFAStifTestCaseAdapter::TestInfo
// -----------------------------------------------------------------------------
TTestCaseInfo CTFAStifTestCaseAdapter::TestInfo( void ) const
    {
    return iTestInfo;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestCaseAdapter::RunTestL
// -----------------------------------------------------------------------------
void CTFAStifTestCaseAdapter::RunTestL( TTestResult& aResult )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestCaseAdapter::RunTestL(0x%x)" ), &aResult ) );

    // Test module behavior is set according to flags of current test case.
    TUint32 behavior( 0 );
    if ( iTestCase.Flags() & ETFMayLeakMemory )
        {
        behavior |= CTestModuleIf::ETestLeaksMem;
        }
    if ( iTestCase.Flags() & ETFMayLeakRequests )
        {
        behavior |= CTestModuleIf::ETestLeaksRequests;
        }
    if ( iTestCase.Flags() & ETFMayLeakHandles )
        {
        behavior |= CTestModuleIf::ETestLeaksHandles;
        }
    iTestModule->TestModuleIf().SetBehavior( ( CTestModuleIf::TTestBehavior )behavior );

    iTestCase.SetupL();
    TRAPD( err, iTestCase.RunTestL() );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestCaseAdapter::RunTestL - Result: %d" ), err ) );
    aResult.iResult = err;
    iTestCase.Teardown();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestCaseAdapter::RunTestL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestCaseAdapter::RunOnStartup
// -----------------------------------------------------------------------------
TBool CTFAStifTestCaseAdapter::RunOnStartup( void ) const
    {
    return iTestCase.RunOnStartup();
    }

