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
* Description:  Implementation of CTFAStifTestSuiteAdapterRoot class
*
*/


// INCLUDE FILES
#include "dsytesttooltrace.h"
#include "tfastiftestsuiteadapterroot.h"
#include "tfastiftestsuiteadapter.h"
#include "tfastiftestcaseadapter.h"

// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapterRoot::NewLC
// -----------------------------------------------------------------------------
CTFAStifTestSuiteAdapterRoot* CTFAStifTestSuiteAdapterRoot::NewLC( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint )
    {
    CTFAStifTestSuiteAdapterRoot* suite = new ( ELeave ) CTFAStifTestSuiteAdapterRoot();
    CleanupStack::PushL( suite );
    suite->ConstructL( aTestSuite, aEntryPoint );
    return suite;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapterRoot::CTFAStifTestSuiteAdapterRoot
// -----------------------------------------------------------------------------
CTFAStifTestSuiteAdapterRoot::CTFAStifTestSuiteAdapterRoot( void )
: iFirstTest( ETrue )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapterRoot::ConstructL
// -----------------------------------------------------------------------------
void CTFAStifTestSuiteAdapterRoot::ConstructL( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestSuiteAdapterRoot::ConstructL(0x%x, 0x%x)" ), &aTestSuite, aEntryPoint ) );
    iWrappedAdapter = CTFAStifTestSuiteAdapter::NewLC( aTestSuite, aEntryPoint );
    CleanupStack::Pop( iWrappedAdapter );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestSuiteAdapterRoot::ConstructL - return" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapterRoot::~CTFAStifTestSuiteAdapterRoot
// -----------------------------------------------------------------------------
CTFAStifTestSuiteAdapterRoot::~CTFAStifTestSuiteAdapterRoot( void )
    {
    // iWrappedAdapter deletes the test objects
    // stored in iTestCases array.
    iTestCases.Reset();
    delete iWrappedAdapter;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapterRoot::AddL
// -----------------------------------------------------------------------------
void CTFAStifTestSuiteAdapterRoot::AddL( MTFATestAdapter& aAdapter )
    {
    iWrappedAdapter->AddL( aAdapter );
    }
    

// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapterRoot::AddTestL
// -----------------------------------------------------------------------------
void CTFAStifTestSuiteAdapterRoot::AddTestL( const CTFAStifTestCaseAdapter& aAdapter )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestSuiteAdapterRoot::AddTestL(0x%x)" ), &aAdapter ) );
    User::LeaveIfError( iTestCases.Append( &aAdapter ) );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestSuiteAdapterRoot::AddTestL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapterRoot::Type
// -----------------------------------------------------------------------------
TTFATestType CTFAStifTestSuiteAdapterRoot::Type( void ) const
    {
    return ETFATestTypeSuite;
    }
    
    
//lint -e429 Pointer has not been freed or returned
// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapterRoot::GetTestCasesL
// -----------------------------------------------------------------------------
TInt CTFAStifTestSuiteAdapterRoot::GetTestCasesL( const TFileName& /*aIniFile*/, RPointerArray<TTestCaseInfo> &aCases )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestSuiteAdapterRoot::GetTestCasesL(0x%x)" ), &aCases ) );
    for ( TInt i = 0; i < iTestCases.Count(); i++ )
        {
        // Stif takes ownership of the test case objects,
        // so new ones must be allocated.
        TTestCaseInfo* copy = new ( ELeave ) TTestCaseInfo();
        *copy = iTestCases[i]->TestInfo();
        TInt val = aCases.Append( copy );
        if ( val != KErrNone )
            {
            delete copy;
            User::Leave( val );
            }
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestSuiteAdapterRoot::GetTestCasesL - return 0x%x" ), KErrNone ) );
    return KErrNone;
    }
//lint +e429
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapterRoot::RunTestCaseL
// -----------------------------------------------------------------------------
TInt CTFAStifTestSuiteAdapterRoot::RunTestCaseL( TInt aCaseId, const TFileName& /*aIniFile*/, TTestResult& aResult )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestSuiteAdapterRoot::RunTestCaseL(0x%x, 0x%x)" ), aCaseId, aResult ) );
    // All tests marked as auto-run are run.
    if ( iFirstTest )
        {
        iFirstTest = EFalse;
        for ( TInt i = 0; i < iTestCases.Count(); i++ )
            {
            if ( iTestCases[i]->RunOnStartup() )
                {
                iTestCases[i]->RunTestL( aResult );
                }
            }
        }

    TInt found( -1 );
    for ( TInt i = 0; i < iTestCases.Count() && found == -1; i++ )
        {
        if ( iTestCases[i]->TestInfo().iCaseNumber == aCaseId )
            {
            found = i;
            }
        }
    if ( found != -1 )
        {
        iTestCases[found]->RunTestL( aResult );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( KErrNotFound );
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAStifTestSuiteAdapterRoot::RunTestCaseL - return 0x%x" ), KErrNone ) );
    return KErrNone;
    }
    
