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
* Description:  Implementation of CTFATestSuiteAdapter class
*
*/


// INCLUDE FILES
#include "ctfatestsuite.h"
#include "dsytesttooltrace.h"
#include "ctfatestsuiteadapter.h"
#include "ctfaentrypoint.h"

// -----------------------------------------------------------------------------
// MTFATestSuiteAdapter::MTFATestSuiteAdapter
// -----------------------------------------------------------------------------
EXPORT_C MTFATestSuiteAdapter::MTFATestSuiteAdapter( void )
    {
    }


// -----------------------------------------------------------------------------
// MTFATestSuiteAdapter::~MTFATestSuiteAdapter
// -----------------------------------------------------------------------------
EXPORT_C MTFATestSuiteAdapter::~MTFATestSuiteAdapter( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFATestSuiteAdapter::CTFATestSuiteAdapter
// -----------------------------------------------------------------------------
EXPORT_C CTFATestSuiteAdapter::CTFATestSuiteAdapter( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint )
: iTestSuite( aTestSuite )
, iEntryPoint( aEntryPoint )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestSuiteAdapter::~CTFATestSuiteAdapter
// -----------------------------------------------------------------------------
EXPORT_C CTFATestSuiteAdapter::~CTFATestSuiteAdapter( void )
    {
    iTests.ResetAndDestroy();
    // The entry point is non-NULL if this is the root suite
    delete iEntryPoint;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestSuiteAdapter::Type
// -----------------------------------------------------------------------------
EXPORT_C TTFATestType CTFATestSuiteAdapter::Type( void ) const
    {
    return ETFATestTypeSuite;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestSuiteAdapter::AddL
// -----------------------------------------------------------------------------
EXPORT_C void CTFATestSuiteAdapter::AddL( MTFATestAdapter& aAdapter )
    {
    User::LeaveIfError( iTests.Append( &aAdapter ) );
    }
