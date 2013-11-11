// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent 
*/

#include <e32capability.h>
#include "sysmonservsess.h"
#include "sysmoncliserv.h"
#include "sysmonserver.h"
#include "monitor.h"
#include <startupproperties.h>
#include <hal.h>
#include <hal_data.h>

#include "../src/SysStartDebug.h"


CSysMonSession::~CSysMonSession()
	{
	}


CSysMonSession::CSysMonSession()
	{
	}


CSysMonSession* CSysMonSession::NewL()
	{
	CSysMonSession *session = new(ELeave) CSysMonSession();
	return session;
	}


void CSysMonSession::ServiceL(const RMessage2 &aMessage)
	{
	DEBUGPRINT1(_L("SysMonServSess: Received message"));

	iMessage = const_cast<RMessage2&> (aMessage);
	switch (iMessage.Function())
		{
	case EMonitor:
		DoMonitorL(EFalse);
		iMessage.Complete(KErrNone);
		break;
	case EMonitorSelf:
		DoMonitorL(ETrue);
		iMessage.Complete(KErrNone);
		break;
	case ECancelSelf:
		CancelMonitorSelfL();
		iMessage.Complete(KErrNone);
		break;
	default:
		iMessage.Complete(KErrNotSupported);
		break;
		}
	}


void CSysMonSession::ServiceError(const RMessage2 &aMessage, TInt aError)
	{
	aMessage.Complete(aError);
	}


/*
Handles EMonitor and EMonitorSelf message to monitor a process
*/
void CSysMonSession::DoMonitorL(TBool aMonitorSelf)
	{
	// no new process monitoring will be initiate if shut down is in progress
	if (SysMonServer()->ShutDownInProgress())
		{
		User::Leave(KErrCancel);
		}

	HBufC8 *msg = HBufC8::NewL(iMessage.GetDesLengthL(0));
	CleanupStack::PushL(msg);

	TPtr8 msgPtr(msg->Des());
	iMessage.ReadL(0, msgPtr);

	CStartupProperties *prop = CStartupProperties::NewL();
	CleanupStack::PushL(prop);

	prop->InternalizeL(msgPtr);

	// Retrieve process id depending function type and check for platform security
	// For self monitoring, EIgnoreOnFailure does not require any capability
	// For moniting other process, EIgnoreOnFailure require capability ProtServ
	TProcessId id;
	if (aMonitorSelf)
		{
		RThread thread;
		User::LeaveIfError(iMessage.Client(thread));

		RProcess process;
		User::LeaveIfError(thread.Process(process));

		id = process.Id();
		
		// Use the filename from the proces
		TFileName name = process.FileName();
		prop->SetFileParamsL(name, prop->Args());
		
		process.Close();
		thread.Close();
		}
	else
		{
		if (prop->RecoveryMethod() == EIgnoreOnFailure)
			{
			if (!iMessage.HasCapability(ECapabilityProtServ))
				{
				User::Leave(KErrPermissionDenied);
				}
			}

		TPckg<TProcessId> procPckg(id);
		iMessage.ReadL(1, procPckg);
		}

	// For ERestartOS, capability ProtServ is required for both self-monitoring and monitoring other
	if (prop->RecoveryMethod() == ERestartOS)
		{
		if (!iMessage.HasCapability(ECapabilityProtServ))
			{
			User::Leave(KErrPermissionDenied);
			}
		}
	// For ERestartOSWithMode, capability ProtServ and PowerMgmt are required for both self-monitoring and monitoring other
	else if (prop->RecoveryMethod() == ERestartOSWithMode)
		{
		if ((!iMessage.HasCapability(ECapabilityProtServ)) || (!iMessage.HasCapability(ECapabilityPowerMgmt)))
			{
			User::Leave(KErrPermissionDenied);
			}
		
#ifndef __WINS__ // HAL::Get(EMaximumRestartStartupModes...) is only supported in H4
		// check that the startup mode is valid, first retrieve max startup mode if not retrieved yet
		TInt maxStartupMode = 0;
		TInt err = HAL::Get(HALData::EMaximumRestartStartupModes, maxStartupMode);
		if (err != KErrNone)
			{
			DEBUGPRINT2(_L("Failed to get max startup mode err=%d"), err);
			User::Leave(err);
			}
		
		if (prop->RestartMode() > maxStartupMode)
			{
			DEBUGPRINT2(_L("Invalid startupMode=%d"), prop->RestartMode());
			User::Leave(KErrArgument);
			}
#endif
		}

#ifdef _DEBUG
	TPtrC fileName = prop->FileName();
	DEBUGPRINT2(_L("SysMonServSess: Creating monitor for %S"), &fileName);
#endif

	const TBool KExecuteRecoveryMethodOnFailure = iMessage.Int2();
	CMonitor *monitor = CMonitor::NewL(*SysMonServer(), id, prop, KExecuteRecoveryMethodOnFailure); // a new monitor is created - ownership of prop transferred

	CleanupStack::Pop(prop); // ownership of prop is passed into monitor
	CleanupStack::PushL(monitor);

	DEBUGPRINT1(_L("SysMonServSess: Monitor created."));

	// new monitor is added to the list maintained by SysMonServer - ownership of monitor transferred.
	// monitor will be popped from cleanup stack in AddMonitorL
	SysMonServer()->AddAndStartMonitorL(monitor); 

	CleanupStack::PopAndDestroy(msg);
	}


/*
CSysMonSession::CancelMonitorSelfL() is invoked to cancel self-monitoring
*/
void CSysMonSession::CancelMonitorSelfL() const
	{
	RThread thread;
	User::LeaveIfError(iMessage.Client(thread));

	RProcess process;
	User::LeaveIfError(thread.Process(process));

	SysMonServer()->CancelMonitor(process.Id());

	DEBUGPRINT2(_L("SysMonServSess: Monitor Cancelled. ProcessId=%Lu"), process.Id().Id());
	
	process.Close();
	thread.Close();
	}


/*
CSysMonSession::SysMonServer() returns a pointer to the server object
*/
CSysMonServer* CSysMonSession::SysMonServer() const
	{
	return static_cast<CSysMonServer*> (const_cast<CServer2*> (Server()));
	}
