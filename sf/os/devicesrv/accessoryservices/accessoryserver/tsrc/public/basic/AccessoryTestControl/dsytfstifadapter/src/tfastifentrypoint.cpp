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
* Description:  Implementation of CTFAStifEntryPoint class
*
*/


// INCLUDE FILES
#include <stiftestinterface.h>
#include "dsytesttooltrace.h"
#include "tfastifentrypoint.h"
#include "tfastiftestsuiteadapter.h"
#include "tfastiftestsuiteadapterroot.h"
#include "tfastiftestcaseadapter.h"
#include "tfastiftestenvironment.h"
#include "tfastiftestlog.h"


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
        RDebug::Print( _L( "Assertion failed: file=%s, line=%d, compiled=%s %s" ), buffer->Des().PtrZ(), aLine, date->Des().PtrZ(), time->Des().PtrZ() );
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
// LibEntryL
// -----------------------------------------------------------------------------
EXPORT_C CTestModuleBase* LibEntryL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::LibEntryL()" ) ) );
    CTFAStifEntryPoint* entry = CTFAStifEntryPoint::NewL();
    // Stif framework deletes the CTestModuleBase-derived object
    // which deletes the entry point object.
    CTestModuleBase* retval = STATIC_CAST( CTFAStifTestSuiteAdapterRoot*, entry->RootSuiteAdapter() );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::LibEntryL - return 0x%x" ), retval ) );
    return retval;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifEntryPoint::NewL
// -----------------------------------------------------------------------------
CTFAStifEntryPoint* CTFAStifEntryPoint::NewL( void )
    {
    CTFAStifEntryPoint* entry = new ( ELeave ) CTFAStifEntryPoint();
    CleanupStack::PushL( entry );
    entry->ConstructL();
    CleanupStack::Pop( entry );
    return entry;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifEntryPoint::CTFAStifEntryPoint
// -----------------------------------------------------------------------------
CTFAStifEntryPoint::CTFAStifEntryPoint( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifEntryPoint::~CTFAStifEntryPoint
// -----------------------------------------------------------------------------
CTFAStifEntryPoint::~CTFAStifEntryPoint( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifEntryPoint::ConstructL
// -----------------------------------------------------------------------------
void CTFAStifEntryPoint::ConstructL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::ConstructL()" ) ) );
    CTFAEntryPoint::ConstructL();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::ConstructL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifEntryPoint::CreateSuiteAdapterLC
// -----------------------------------------------------------------------------
MTFATestSuiteAdapter* CTFAStifEntryPoint::CreateSuiteAdapterLC( CTFATestSuite& aSuite, TBool aRoot )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::CreateSuiteAdapterLC(0x%x, %d )" ), &aSuite, aRoot ) );
    MTFATestSuiteAdapter* adapter;
    if ( aRoot )
        {
        adapter = CTFAStifTestSuiteAdapterRoot::NewLC( aSuite, this );
        }
    else
        {
        adapter = CTFAStifTestSuiteAdapter::NewLC( aSuite, NULL );
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::CreateSuiteAdapterLC - return 0x%x" ), adapter ) );
    return adapter;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifEntryPoint::CreateTestAdapterLC
// -----------------------------------------------------------------------------
MTFATestCaseAdapter* CTFAStifEntryPoint::CreateTestAdapterLC( CTFATestCase& aCase )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::CreateTestAdapterLC(0x%x)" ), &aCase ) );
    CTFAStifTestCaseAdapter* adapter = CTFAStifTestCaseAdapter::NewLC( aCase, 
        STATIC_CAST( CTFAStifTestSuiteAdapterRoot*, RootSuiteAdapter() ) );
    STATIC_CAST( CTFAStifTestSuiteAdapterRoot*, RootSuiteAdapter() )->AddTestL( *adapter );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::CreateTestAdapterLC - return 0x%x" ), adapter ) );
    return adapter;
    }
    

// -----------------------------------------------------------------------------
// CTFAStifEntryPoint::CreateEnvironmentL
// -----------------------------------------------------------------------------
CTFATestEnvironment* CTFAStifEntryPoint::CreateEnvironmentL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::CreateEnvironmentL()" ) ) );
    CTFAStifTestLog* log = CTFAStifTestLog::NewLC();
    CTFAStifTestEnvironment* env = new ( ELeave ) CTFAStifTestEnvironment( *log );
    CleanupStack::Pop( log );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifEntryPoint::CreateEnvironmentL - return 0x%x" ), env ) );
    return env;
    }
    
