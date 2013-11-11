/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Power save mode client active object
*
*/


#include "psmclientimpl.h"
#include "psmclientactive.h"
#include "psmclientobserver.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmClientActive::CPsmClientActive
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmClientActive::CPsmClientActive( MPsmClientObserver& aObserver ) :
    CActive( EPriorityStandard ),
    iObserver( aObserver )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::CPsmClientActive()" ) ) );
    CActiveScheduler::Add( this );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::CPsmClientActive - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientActive::~CPsmClientActive
// Destructor
// -----------------------------------------------------------------------------
//
CPsmClientActive::~CPsmClientActive()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::~CPsmClientActive()" ) ) );
    Cancel();
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::~CPsmClientActive - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientActive::RunL
// -----------------------------------------------------------------------------
//
void CPsmClientActive::RunL()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::RunL() - %i" ), iStatus.Int() ) );

    // Note: As this class does not implement CActive::RunError, this RunL cannot never leave

    TInt result( iStatus.Int() );

    if ( result < KErrNone )
        {
        iObserver.PowerSaveModeChangeError( result );
        }
    else
        {
        iObserver.PowerSaveModeChanged( (TPsmsrvMode)result );
        }

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::RunL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientActive::DoCancel
// -----------------------------------------------------------------------------
//
void CPsmClientActive::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::DoCancel()" ) ) );
    iObserver.PowerSaveModeChangeError( KErrCancel );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::DoCancel - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientActive::SetActive
// -----------------------------------------------------------------------------
//
void CPsmClientActive::SetActive()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::SetActive()" ) ) );
    CActive::SetActive();
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientActive::SetActive - return" ) ) );
    }

// End of file
