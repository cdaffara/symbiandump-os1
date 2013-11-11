/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/


// INCLUDE FILES
#include "saegprsavailabilityobs.h"
#include "saeetelobserver.h"
#include "PSVariables.h"
#include "saepubsubnotifier.h"
#include "sae_debug.h"

#include <e32svr.h>


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSaeGprsAvailabilityObs::CSaeGprsAvailabilityObs( CSaeEtelObserver* aEtelObserver, RPacketService* aService ) 
	: CActive( EPriorityStandard ), iEtelObserver( aEtelObserver ), iService( aService ), 
	iRegistrationStatus( RPacketService::EUnknown )
    {
	CActiveScheduler::Add( this );
	InitializeRegStatus();
    }

// Destructor
CSaeGprsAvailabilityObs::~CSaeGprsAvailabilityObs()
    {
	Cancel();
    }

// ---------------------------------------------------------
// CSaeGprsStatusObs::DoCancel()
// 
// ---------------------------------------------------------
//
void CSaeGprsAvailabilityObs::DoCancel()
	{
	iService->NotifyChangeOfNtwkRegStatus(iStatus, iRegistrationStatus);
	}

// ---------------------------------------------------------
// CSaeGprsStatusObs::RunL()
// 
// ---------------------------------------------------------
//
void CSaeGprsAvailabilityObs::RunL()
	{
	TInt status = iStatus.Int();
	COM_TRACE_1( "SAE (avail): CSaeGprsAvailabilityObs::RunL() TRequestStatus(%d) ", status);
	
	switch ( status )
		{
		// Update System Agent and issue new request.
		case KErrNone:
			CheckRegistrationStatusAndInformPubSub();
			IssueRequest();
			break;
		// If the request times out try to get current status.
		case KErrTimedOut:
			COM_TRACE_( "SAE (avail): ERROR CSaeGprsAvailabilityObs: Request timed out!" );
			CheckRegistrationStatusAndInformPubSub();
			IssueRequest();
			break;
		// Check that registration status information can be provided.
		case KErrNotSupported:
			COM_TRACE_( "SAE (avail): ERROR CSaeGprsAvailabilityObs: Registration status not supported!" );
			break;
		default:
			COM_TRACE_( "SAE (avail): ERROR CSaeGprsAvailabilityObs: Unhandled TRequestStatus case!" );
			break;
		}
	}

// ---------------------------------------------------------
// CSaeGprsStatusObs::IssueRequest()
// 
// ---------------------------------------------------------
//
void CSaeGprsAvailabilityObs::IssueRequest()
	{
	COM_TRACE_( "SAE (avail): CSaeGprsAvailabilityObs::IssueRequest() start" );
	if( !IsActive() )
		{
		iService->NotifyChangeOfNtwkRegStatus(iStatus, iRegistrationStatus);
		SetActive();
		}
	COM_TRACE_( "SAE (avail): CSaeGprsAvailabilityObs::IssueRequest() end" );
	}

// ---------------------------------------------------------
// CSaeGprsStatusObs::InitializeRegStatus()
// 
// ---------------------------------------------------------
//
void CSaeGprsAvailabilityObs::InitializeRegStatus()
{
	COM_TRACE_( "SAE (avail): CSaeGprsAvailabilityObs::InitializeRegStatus() start" );
	if( !IsActive() )
        {
        COM_TRACE_( "SAE (avail): CSaeGprsAvailabilityObs::InitializeRegStatus() query network status" );
        iService->GetNtwkRegStatus(iStatus, iRegistrationStatus);
        SetActive();
        }
    COM_TRACE_( "SAE (avail): CSaeGprsAvailabilityObs::InitializeRegStatus() end" );
}

// ---------------------------------------------------------
// CSaeGprsStatusObs::CheckReqistrationStatusAndInformPubSub()
// 
// ---------------------------------------------------------
//
void CSaeGprsAvailabilityObs::CheckRegistrationStatusAndInformPubSub()
	{
	COM_TRACE_1( "SAE (avail): registration status: %d ", iRegistrationStatus);
	switch (iRegistrationStatus )
		{
		case RPacketService::ERegisteredOnHomeNetwork:
		case RPacketService::ERegisteredRoaming:
			COM_TRACE_( "SAE (avail): KPSUidGprsAvailability, EPSGprsAvailable  " );
			iEtelObserver->NotifyPubSub( KPSUidGprsAvailability, EPSGprsAvailable );
			break;
		case RPacketService::ENotRegisteredAndNotAvailable:
			COM_TRACE_( "SAE (avail): KPSUidGprsAvailability, EPSGprsNotAvailable  " );
			iEtelObserver->NotifyPubSub( KPSUidGprsAvailability, EPSGprsNotAvailable );
			break;
		default:
			COM_TRACE_( "SAE (avail): KPSUidGprsAvailability, EPSGprsAvailabilityUnknown  " );
			iEtelObserver->NotifyPubSub( KPSUidGprsAvailability, EPSGprsAvailabilityUnknown );
			break;
		}
	}
//  End of File  
