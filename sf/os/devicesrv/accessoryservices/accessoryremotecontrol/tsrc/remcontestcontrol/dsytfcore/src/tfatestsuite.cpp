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
* Description:  Implementation of CTFATestSuite class
*
*/


// INCLUDE FILES
#include "ctfatestsuite.h"
#include "dsytesttooltrace.h"

// -----------------------------------------------------------------------------
// CTFATestSuite::CTFATestSuite
// -----------------------------------------------------------------------------
EXPORT_C CTFATestSuite::CTFATestSuite( const TDesC& aName )
: CTFATest( aName )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestSuite::~CTFATestSuite
// -----------------------------------------------------------------------------
EXPORT_C CTFATestSuite::~CTFATestSuite( void )
    {
    // Deletes all test cases and sub-suites
    iTests.ResetAndDestroy();
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestSuite::Type
// -----------------------------------------------------------------------------
EXPORT_C TTFATestType CTFATestSuite::Type( void ) const
    {
    return ETFATestTypeSuite;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestSuite::Count
// -----------------------------------------------------------------------------
EXPORT_C TInt CTFATestSuite::Count( void ) const
    {
    return iTests.Count();
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestSuite::At
// -----------------------------------------------------------------------------
EXPORT_C CTFATest& CTFATestSuite::At( TInt aIndex )
    {
    TRACE_ASSERT( aIndex >= 0 && aIndex < iTests.Count() );
    return *(iTests[aIndex]);
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestSuite::AddL
// -----------------------------------------------------------------------------
EXPORT_C void CTFATestSuite::AddL( const CTFATest& aTest )
    {
    User::LeaveIfError( iTests.Append( &aTest ) );
    }
    
