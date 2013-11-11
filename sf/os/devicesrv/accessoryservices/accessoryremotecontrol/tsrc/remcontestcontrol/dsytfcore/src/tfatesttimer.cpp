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
* Description:  Implementation of CTFATestTimer class
*
*/


// INCLUDE FILES
#include "ctfatesttimer.h"
#include "mtfaactivetest.h"
#include "dsytesttooltrace.h"

// -----------------------------------------------------------------------------
// CTFATestTimer::NewL
// -----------------------------------------------------------------------------
EXPORT_C CTFATestTimer* CTFATestTimer::NewL( MTFAActiveTest& aCallback )
    {
    CTFATestTimer* timer = new ( ELeave ) CTFATestTimer( aCallback );
    CleanupStack::PushL( timer );
    timer->ConstructL();
    CleanupStack::Pop( timer );
    return timer;
    }


// -----------------------------------------------------------------------------
// CTFATestTimer::CTFATestTimer
// -----------------------------------------------------------------------------
CTFATestTimer::CTFATestTimer( MTFAActiveTest& aCallback )
: CTimer( EPriorityNormal )
, iCallback( aCallback )
    {
    CActiveScheduler::Add( this );
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestTimer::~CTFATestTimer
// -----------------------------------------------------------------------------
EXPORT_C CTFATestTimer::~CTFATestTimer( void )
    {
    // CTimer calls Cancel
    }
    
    
// -----------------------------------------------------------------------------
// CTFATestTimer::RunL
// -----------------------------------------------------------------------------
void CTFATestTimer::RunL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFATestTimer::RunL()" ) ) );
    iCallback.ActiveTestRunL();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFATestTimer::RunL - return void" ) ) );
    }
    
