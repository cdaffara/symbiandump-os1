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
* Description:  Implementation of CTFAEntryPoint class
*
*/


// INCLUDE FILES
#include "ctfatestenvironment.h"
#include "ctfatestlog.h"
#include "ctfatestsuite.h"
#include "ctfatestcase.h"
#include "ctffactory.h"
#include "ctfaentrypoint.h"
#include "ctfatestcaseadapter.h"
#include "ctfatestsuiteadapter.h"
#include "dsytesttooltrace.h"
#include "ctfactivescheduler.h"

// 2-second timer is used to wait for asynchronous startup tasks
// performed by tested components.
const TInt KTFStartupWait = 2000000;

// ================= TRACE FUNCTIONS ==============
#ifdef _DEBUG

const TInt KTFAssertDateSize = 32;


// -----------------------------------------------------------------------------
// TraceAssertFunc
// -----------------------------------------------------------------------------
void TraceAssertFunc( const TDesC8& aFile, TInt aLine )
    {
    HBufC* buffer = HBufC::New( aFile.Length() + 1 );
    HBufC* date = HBufC::New( KTFAssertDateSize );
    HBufC* time = HBufC::New( KTFAssertDateSize );
    if ( buffer != NULL && date != NULL && time != NULL )
        {
        buffer->Des().Copy( aFile );
        date->Des().Copy( _L8( __DATE__ ) );
        time->Des().Copy( _L8( __TIME__ ) );
        RDebug::Print( _L( "Assertion failed: file=%s, line=%d, compiled=%s %s" ), 
            buffer->Des().PtrZ(), aLine, date->Des().PtrZ(), time->Des().PtrZ() );
        }
    else
        {
        RDebug::Print( _L( "Assertion and memory allocation failed" ) );
        }
    delete buffer;
    delete date;
    delete time;
    }
#endif


// -----------------------------------------------------------------------------
// CTFAEntryPoint::CTFAEntryPoint
// -----------------------------------------------------------------------------
EXPORT_C CTFAEntryPoint::CTFAEntryPoint( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFAEntryPoint::~CTFAEntryPoint
// -----------------------------------------------------------------------------
EXPORT_C CTFAEntryPoint::~CTFAEntryPoint( void )
    {
    delete iRootSuite;
    delete iEnvironment;
    // The root suite adapter is deleted by the test framework.
    // This object is deleted by the root suite adapter.
    iRootSuiteAdapter = NULL;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAEntryPoint::RootSuiteAdapter
// -----------------------------------------------------------------------------
EXPORT_C MTFATestSuiteAdapter* CTFAEntryPoint::RootSuiteAdapter( void )
    {
    return iRootSuiteAdapter;
    }


// -----------------------------------------------------------------------------
// CTFAEntryPoint::TimerCallback
// -----------------------------------------------------------------------------
TInt CTFAEntryPoint::TimerCallback( TAny* )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAEntryPoint::TimerCallback()" ) ) );
    CActiveScheduler::Stop();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAEntryPoint::TimerCallback - return 0x%x" ), EFalse ) );
    return EFalse;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAEntryPoint::ConstructL
// -----------------------------------------------------------------------------
EXPORT_C void CTFAEntryPoint::ConstructL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAEntryPoint::ConstructL()" ) ) );
    iRootSuite = new ( ELeave ) CTFATestSuite( _L( "Root" ) );
    iRootSuiteAdapter = CreateSuiteAdapterLC( *iRootSuite, ETrue );
    CleanupStack::Pop();
    iEnvironment = CreateEnvironmentL();
    if ( CActiveScheduler::Current() == NULL )
        {
        CTFActiveScheduler* scheduler = new ( ELeave ) CTFActiveScheduler();
        CActiveScheduler::Install( scheduler );
        iEnvironment->SetScheduler( scheduler );
        COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAEntryPoint::ConstructL() - Active scheduler installed" ) ) );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAEntryPoint::ConstructL() - Active scheduler was already installed" ) ) );
        }
    CTFFactory* factory = CTFFactory::NewL();
    iEnvironment->SetFactory( factory );
    iRootSuite->SetEnvironment( *iEnvironment );
    factory->BuildTestSuiteL( iRootSuite );
    iEnvironment->Logger().Write( _L( "Test suite built:" ) );
    iEnvironment->Logger().StartList();
    BuildAdapterSuiteL( *iRootSuite, *iRootSuiteAdapter );
    iEnvironment->Logger().EndList();
    CPeriodic* timer = CPeriodic::NewL( EPriorityNormal );
    // Waits for the tested modules to perform asynchronous startup tasks
    timer->Start( TTimeIntervalMicroSeconds32( KTFStartupWait ), 
        TTimeIntervalMicroSeconds32( KTFStartupWait ), TCallBack( TimerCallback, NULL ) );
    CActiveScheduler::Start();
    delete timer;
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAEntryPoint::ConstructL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAEntryPoint::BuildAdapterSuiteL
// -----------------------------------------------------------------------------
void CTFAEntryPoint::BuildAdapterSuiteL( CTFATestSuite& aSuite, MTFATestSuiteAdapter& aAdapter )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAEntryPoint::BuildAdapterSuiteL(0x%x, 0x%x)" ), &aSuite, &aAdapter ) );
    _LIT( KRoot, "%S" );
    iEnvironment->Logger().WriteList( iDepth++, KRoot, &aSuite.Name() );
    for ( TInt i = 0; i < aSuite.Count(); i++ )
        {
        CTFATest* test = &aSuite.At( i );
        if ( test->Type() == ETFATestTypeSuite )
            {
            //lint -e826 Suspicious pointer-to-pointer conversion
            CTFATestSuite* suite = STATIC_CAST( CTFATestSuite*, test );
            //lint +e826
            MTFATestSuiteAdapter* adapter = CreateSuiteAdapterLC( *suite, EFalse );
            aAdapter.AddL( *adapter );
            BuildAdapterSuiteL( *suite, *adapter );
            CleanupStack::Pop();
            }
        else
            {
            //lint -e826 Suspicious pointer-to-pointer conversion
            CTFATestCase* testCase = STATIC_CAST( CTFATestCase*, test );
            //lint +e826
            testCase->Log( iDepth );
            MTFATestCaseAdapter* adapter = CreateTestAdapterLC( *testCase );
            aAdapter.AddL( *adapter );
            CleanupStack::Pop();
            }
        }
    iDepth--;
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAEntryPoint::BuildAdapterSuiteL - return void" ) ) );
    }
    
