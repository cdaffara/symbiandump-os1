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
* Description:  Implementation of CTFAActiveTest class
*
*/


// INCLUDE FILES
#include "ctfaactivetest.h"
#include "mtfaactivetest.h"
#include "dsytesttooltrace.h"

// -----------------------------------------------------------------------------
// MTFAActiveTest::MTFAActiveTest
// -----------------------------------------------------------------------------
EXPORT_C MTFAActiveTest::MTFAActiveTest( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// MTFAActiveTest::~MTFAActiveTest
// -----------------------------------------------------------------------------
EXPORT_C MTFAActiveTest::~MTFAActiveTest( void )
    {
    }
    
    
// -----------------------------------------------------------------------------
// CTFAActiveTest::CTFAActiveTest
// -----------------------------------------------------------------------------
EXPORT_C CTFAActiveTest::CTFAActiveTest( MTFAActiveTest& aCallback )
: CActive( EPriorityNormal )
, iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAActiveTest::~CTFAActiveTest
// -----------------------------------------------------------------------------
EXPORT_C CTFAActiveTest::~CTFAActiveTest( void )
    {
    Cancel();
    }
    
    
// -----------------------------------------------------------------------------
// CTFAActiveTest::RunL
// -----------------------------------------------------------------------------
void CTFAActiveTest::RunL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAActiveTest::RunL()" ) ) );
    iCallback.ActiveTestRunL();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAActiveTest::RunL - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAActiveTest::DoCancel
// -----------------------------------------------------------------------------
void CTFAActiveTest::DoCancel( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAActiveTest::DoCancel()" ) ) );
    iCallback.ActiveTestCancel();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFAActiveTest::DoCancel - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFAActiveTest::Status
// -----------------------------------------------------------------------------
EXPORT_C TRequestStatus& CTFAActiveTest::Status( void )
    {
    return iStatus;
    }
    
    
// -----------------------------------------------------------------------------
// CTFAActiveTest::DoSetActive
// -----------------------------------------------------------------------------
EXPORT_C void CTFAActiveTest::DoSetActive( void )
    {
    CActive::SetActive();
    }
    
