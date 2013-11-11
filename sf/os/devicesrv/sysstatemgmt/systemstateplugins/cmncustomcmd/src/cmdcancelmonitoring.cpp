// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: This command is used to issue a request to sysmon server during device shutdown to cancel all the outstanding monitors
//

/**
 @file
 @internalComponent
 @released
*/

#include "cmdcancelmonitoring.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

/**
 * Constructs and returns a new custom command, leaving on errors.
 * 
 */
CCustomCmdCancelMonitoring* CCustomCmdCancelMonitoring::NewL()
	{
	CCustomCmdCancelMonitoring* self = new (ELeave) CCustomCmdCancelMonitoring();
	return self;
	}

CCustomCmdCancelMonitoring::CCustomCmdCancelMonitoring()
	{
	}

CCustomCmdCancelMonitoring::~CCustomCmdCancelMonitoring()
	{
	iSysMonSession.Close();
	}

/**
 * Initializes this custom command.
 * 
 */
TInt CCustomCmdCancelMonitoring::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	TRAPD(ret, iSysMonSession.OpenL());
	DEBUGPRINT2A("CCustomCmdCancelMonitoring Initialized with %d", ret);
	return ret;
	}

/**
 * Closes the session with Sysmon Server.
 * 
 */
void CCustomCmdCancelMonitoring::Close()
	{
	iSysMonSession.Close();
	}

void CCustomCmdCancelMonitoring::Release()
	{
	delete this;
	}

/**
 * Issues a request to the sysmon server to cancel all the outstanding montiors.
 * 
 */
void CCustomCmdCancelMonitoring::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	const TInt err = iSysMonSession.CancelAllMonitors();

	aStatus = KRequestPending;
	TRequestStatus* statusPtr = &aStatus;
	User::RequestComplete(statusPtr, err);
	DEBUGPRINT2A("Call to Sysmon Server to cancel the monitoring completed with %d", err);
	}

/**
 * Cancels the requested execute
 * 
 */
void CCustomCmdCancelMonitoring::ExecuteCancel()
	{
	// Nothing to do as request already completed in Execute()
	}


