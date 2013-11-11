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
* Description:  Implementation of CTFActiveScheduler class
*
*/


// INCLUDE FILES
#include "ctfactivescheduler.h"
#include "dsytesttooltrace.h"

// -----------------------------------------------------------------------------
// CTFActiveScheduler::CTFActiveScheduler
// -----------------------------------------------------------------------------
CTFActiveScheduler::CTFActiveScheduler( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFActiveScheduler::~CTFActiveScheduler
// -----------------------------------------------------------------------------
CTFActiveScheduler::~CTFActiveScheduler( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFActiveScheduler::Error
// -----------------------------------------------------------------------------
#if defined ( COMPONENT_TRACE_FLAG ) && defined ( _DEBUG )
void CTFActiveScheduler::Error( TInt aError ) const
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFActiveScheduler::Error(0x%x)" ), aError ) );
    TRACE_ASSERT_ALWAYS;
    }
#else
void CTFActiveScheduler::Error( TInt /*aError*/ ) const
    {
    TRACE_ASSERT_ALWAYS;
    }
#endif


