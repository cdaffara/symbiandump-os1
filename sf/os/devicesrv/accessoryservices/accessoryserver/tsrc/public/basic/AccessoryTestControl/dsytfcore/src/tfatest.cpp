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
* Description:  Implementation of CTFATest class
*
*/


// INCLUDE FILES
#include "ctfatest.h"
#include "dsytesttooltrace.h"

// -----------------------------------------------------------------------------
// CTFATest::CTFATest
// -----------------------------------------------------------------------------
CTFATest::CTFATest( const TDesC& aName )
    {
    TRACE_ASSERT( aName.Length() > 0 && aName.Length() < KTFAMaxTestNameLength );
    __ASSERT_ALWAYS( aName.Length() > 0, User::Panic( KTFName, ETFPanicInvalidTestName ) );
    __ASSERT_ALWAYS( aName.Length() < KTFAMaxTestNameLength, User::Panic( KTFName, ETFPanicInvalidTestName ) );
    iName.Copy( aName );
    }
    
    
// -----------------------------------------------------------------------------
// CTFATest::~CTFATest
// -----------------------------------------------------------------------------
CTFATest::~CTFATest( void )
    {
    iEnvironment = NULL;
    }
    
    
// -----------------------------------------------------------------------------
// TDesC& CTFATest::Name
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& CTFATest::Name( void ) const
    {
    return iName;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATest::SetEnvironment
// -----------------------------------------------------------------------------
EXPORT_C void CTFATest::SetEnvironment( CTFATestEnvironment& aEnvironment )
    {
    iEnvironment = &aEnvironment;
    }
    
    
// -----------------------------------------------------------------------------
// CTFATest::Environment
// -----------------------------------------------------------------------------
EXPORT_C CTFATestEnvironment& CTFATest::Environment( void )
    {
    return *iEnvironment;
    }
    
