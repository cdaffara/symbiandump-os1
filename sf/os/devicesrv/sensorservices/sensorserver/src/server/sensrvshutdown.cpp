/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class implements the shutdown timer that is used by the 
*                SensorServer to shutdown the server after the last client has
*                disconnected.
*
*/



// INCLUDE FILES
#include "sensrvshutdown.h"
#include "sensrvtrace.h"
#include "sensrvproxymanager.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRadioServerShutdown::CRadioServerShutdown
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSensrvShutdown::CSensrvShutdown( CSensrvProxyManager& aProxyManager )
	:	CTimer(EPriorityStandard),
	iProxyManager(aProxyManager)
    {
    }

// -----------------------------------------------------------------------------
// CRadioServerShutdown::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSensrvShutdown::ConstructL()
    {
	COMPONENT_TRACE( _L( "Sensor Server - CSensrvShutdown::ConstructL" ) );
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CRadioServerShutdown::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensrvShutdown* CSensrvShutdown::NewL( CSensrvProxyManager& aProxyManager )
    {
    CSensrvShutdown* self = new( ELeave ) CSensrvShutdown( aProxyManager );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CSensrvShutdown::~CSensrvShutdown()
    {
    COMPONENT_TRACE( _L( "Sensor Server - CSensrvShutdown::CSensrvShutdown" ) );
	if ( IsActive() )
		{
		Cancel();
		}
    }

// -----------------------------------------------------------------------------
// CRadioServerShutdown::Start
// -----------------------------------------------------------------------------
//
void CSensrvShutdown::Start()
    {
    COMPONENT_TRACE( _L( "Sensor Server - CSensrvShutdown::Start" ) );
	if ( !IsActive() )
		{
		After(iProxyManager.TerminationPeriod());
		}
	COMPONENT_TRACE( _L( "Sensor Server - CSensrvShutdown::Start - return" ) );
    }

// -----------------------------------------------------------------------------
// CRadioServerShutdown::DoCancel
// -----------------------------------------------------------------------------
//
void CSensrvShutdown::DoCancel()
    {
    CTimer::DoCancel();
    }

// -----------------------------------------------------------------------------
// CRadioServerShutdown::RunL
// -----------------------------------------------------------------------------
//
void CSensrvShutdown::RunL()
    {
	COMPONENT_TRACE( _L( "Sensor Server - CSensrvShutdown::RunL - Stop scheduler" ) );
    iProxyManager.ShutdownServer();
    }

//  End of File
