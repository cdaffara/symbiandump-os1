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
* Description:  Implementation of CTFATestCase class
*
*/


// INCLUDE FILES
#include "ctfatestcase.h"
#include "ctfatestenvironment.h"
#include "dsytesttooltrace.h"

const TInt KDefaultTestTimeout = 30;

// -----------------------------------------------------------------------------
// CTFATestCase::CTFATestCase
// -----------------------------------------------------------------------------
EXPORT_C CTFATestCase::CTFATestCase( TInt aCaseId, const TDesC& aName )
: CTFATest( aName )
, iCaseId( aCaseId )
, iTimeout( KDefaultTestTimeout )
    {
    TRACE_ASSERT( aCaseId > 0 );
    __ASSERT_ALWAYS( aCaseId > 0, User::Panic( KTFName, ETFPanicInvalidTestCaseNumber ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestCase::~CTFATestCase
// -----------------------------------------------------------------------------
EXPORT_C CTFATestCase::~CTFATestCase( void )
    {
    }
    

// -----------------------------------------------------------------------------
// CTFATestCase::Type
// -----------------------------------------------------------------------------
EXPORT_C TTFATestType CTFATestCase::Type( void ) const
    {
    return ETFATestTypeCase;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestCase::Id
// -----------------------------------------------------------------------------
EXPORT_C TInt CTFATestCase::Id( void ) const
    {
    return iCaseId;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestCase::SetTimeout
// -----------------------------------------------------------------------------
EXPORT_C void CTFATestCase::SetTimeout( TInt aTimeout )
    {
    iTimeout = aTimeout;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestCase::SetRunOnStartup
// -----------------------------------------------------------------------------
EXPORT_C void CTFATestCase::SetRunOnStartup( TBool aFlag )
    {
    iRunOnStartup = aFlag;
    }


// -----------------------------------------------------------------------------
// CTFATestCase::Timeout
// -----------------------------------------------------------------------------
EXPORT_C TInt CTFATestCase::Timeout( void ) const
    {
    return iTimeout;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestCase::RunOnStartup
// -----------------------------------------------------------------------------
EXPORT_C TBool CTFATestCase::RunOnStartup( void ) const
    {
    return iRunOnStartup;
    }


// -----------------------------------------------------------------------------
// CTFATestCase::SetFlags
// -----------------------------------------------------------------------------
EXPORT_C void CTFATestCase::SetFlags( TUint32 aFlags )
    {
    iFlags = aFlags;
    }


// -----------------------------------------------------------------------------
// CTFATestCase::Flags
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CTFATestCase::Flags( void ) const
    {
    return iFlags;
    }


// -----------------------------------------------------------------------------
// CTFATestCase::SetEnvironment
// -----------------------------------------------------------------------------
EXPORT_C void CTFATestCase::SetEnvironment( CTFATestEnvironment& aEnvironment )
    {
    CTFATest::SetEnvironment( aEnvironment );
    iFlags = aEnvironment.TestCaseFlags();
    }


