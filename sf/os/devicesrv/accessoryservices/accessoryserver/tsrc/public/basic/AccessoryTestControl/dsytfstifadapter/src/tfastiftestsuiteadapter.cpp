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
* Description:  Implementation of CTFAStifTestSuiteAdapter class
*
*/


// INCLUDE FILES
#include "dsytesttooltrace.h"
#include "tfastiftestsuiteadapter.h"
#include "tfastiftestcaseadapter.h"

// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapter::NewLC
// -----------------------------------------------------------------------------
CTFAStifTestSuiteAdapter* CTFAStifTestSuiteAdapter::NewLC( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint )
    {
    CTFAStifTestSuiteAdapter* suite = new ( ELeave ) CTFAStifTestSuiteAdapter( aTestSuite, aEntryPoint );
    CleanupStack::PushL( suite );
    return suite;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapter::CTFAStifTestSuiteAdapter
// -----------------------------------------------------------------------------
CTFAStifTestSuiteAdapter::CTFAStifTestSuiteAdapter( CTFATestSuite& aTestSuite, CTFAEntryPoint* aEntryPoint )
: CTFATestSuiteAdapter( aTestSuite, aEntryPoint )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFAStifTestSuiteAdapter::~CTFAStifTestSuiteAdapter
// -----------------------------------------------------------------------------
CTFAStifTestSuiteAdapter::~CTFAStifTestSuiteAdapter( void )
    {
    }
    
