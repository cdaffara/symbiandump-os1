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
* Description:  PSM Server shutdown
*
*/


#include "psmsrvshutdown.h"
#include "psmsrvserver.h"
#include "psmsrvsession.h"
#include "psmtrace.h"

const TTimeIntervalMicroSeconds32 KShutDownTime = 60000000; // One minute 

// -----------------------------------------------------------------------------
// CPsmSrvShutdown::CPsmSrvShutdown
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmSrvShutdown::CPsmSrvShutdown( CPsmSrvServer& aServer ) : 
    CActive( EPriorityNormal ),
    iServer( aServer )
    {
    CActiveScheduler::Add( this );
    iTimer.CreateLocal();
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmSrvShutdown::~CPsmSrvShutdown()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::~CPsmSrvShutdown()" ) ) );
    Cancel();
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::~CPsmSrvShutdown - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmSrvShutdown::ServiceL
// -----------------------------------------------------------------------------
//
void CPsmSrvShutdown::RunL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::RunL()" ) ) );

    // Note. This class does not implement CActive::RunError so this function cannot leave

    if ( KErrNone == iServer.ShutdownServer() )
        {
        iTimer.Close();
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::RunL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSrvShutdown::DoCancel
// -----------------------------------------------------------------------------
//
void CPsmSrvShutdown::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::DoCancel()" ) ) );

    iTimer.Cancel();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::DoCancel - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSrvShutdown::Start
// -----------------------------------------------------------------------------
//
void CPsmSrvShutdown::Start()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::Start()" ) ) );

    iTimer.After( iStatus, KShutDownTime );
    SetActive();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::Start - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSrvShutdown::Restart
// -----------------------------------------------------------------------------
//
void CPsmSrvShutdown::Restart()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::Restart()" ) ) );

    if ( IsActive() )
        {
        Cancel();
        }

    iTimer.After( iStatus, KShutDownTime );
    SetActive();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSrvShutdown::Restart - return" ) ) );
    }

// End of file
