// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "shutdowntimer.h"
#include "savepriv.h"

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsrv.h"
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsess.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsrvpatchdata.h"

/*
Patchable data KShtdwnTimeoutBase can be configured as -1 not to apply the timer 
as backward compatibility.
*/
const TInt32 KShtdwnTimeoutNoApply = -1;

/**
Standard phase-one factory method for creating CShutdownTimer instances.
It might return a NULL pointer if the patchable constant KShtdwnTimeoutBase has been configured 
as -1 which indicates no timer should be started.
@param aShtdwnSrv The reference to CServShutdownServer.
@return 0 if no timer should be started, otherwise a pointer to CShutdownTimer object.
@leave Some system-wide error codes including KErrNoMemory.
@panic KErrNotSupported Incorrect patchable variables configuration.
*/
CShutdownTimer* CShutdownTimer::NewL(CServShutdownServer& aShtdwnSrv)
	{ 
	__ASSERT_ALWAYS( (KShtdwnTimeoutBase >= KShtdwnTimeoutNoApply) && (KShtdwnTimeoutClient >= 0), \
		User::Panic(__SHUTDOWN_SERVER_NAME, KErrNotSupported));

	CShutdownTimer* self = 0;
	if(KShtdwnTimeoutBase > KShtdwnTimeoutNoApply)
		{
		self = new(ELeave) CShutdownTimer(aShtdwnSrv);
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		}
	
	return self;
	}

/**
Constructor
@param aShtdwnSrv The reference to CServShutdownServer
*/
CShutdownTimer::CShutdownTimer(CServShutdownServer& aShtdwnSrv):
	CTimer(CActive::EPriorityStandard), iShtdwnSrv(aShtdwnSrv)
	{
	CActiveScheduler::Add(this);
    }
   
/**
Removes the timer object from the active scheduler's list if it has been added.
*/
void CShutdownTimer::DoCancel()
	{
	// Do nothing
	}
	
/**
Calculates the timeout value by the number of the clients and starts the timer.
@param aNumClients The number of the clients.
*/
void CShutdownTimer::Start(TInt aNumClients)
	{
	TInt32 timeout = KShtdwnTimeoutBase + KShtdwnTimeoutClient * aNumClients;
	After(timeout);
	}

	
/**
Gets called when the timeout expires.
Calls CServShutdownServer::SwitchOff() to execute the switch off.
*/
void CShutdownTimer::RunL()
	{
	iShtdwnSrv.SwitchOff();
	}
