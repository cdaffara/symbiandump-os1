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

#include "sysmoncliserv.h"
#include "sysmonserver.h"
#include "monitor.h"
#include "sysmonservsess.h"
#include "timerlist.h"

#include "../src/SysStartDebug.h"


CSysMonServer* CSysMonServer::NewLC()
	{
	CSysMonServer* server = new(ELeave) CSysMonServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	return server;		
	}


void CSysMonServer::ConstructL()
	{
	StartL(KSysMonServerName);	
	}


CSysMonServer::CSysMonServer()
	:CServer2(EPriorityHigh),
	 iShutDownInProgress(EFalse),
	 iMonitors(CMonitor::iOffset),
	 iIter(iMonitors) 
	{
	}


CSysMonServer::~CSysMonServer()	
	{
	CancelAllMonitors();
	delete iTimerList;
	}

TBool CSysMonServer::ShutDownInProgress() const
	{
	return iShutDownInProgress;
	}

void CSysMonServer::CancelAllMonitors()
	{
	iShutDownInProgress = ETrue;

	iIter.SetToFirst();
	CMonitor *monitor = NULL;
	while (iIter++ != NULL)
		{
		monitor = iIter;
		iMonitors.Remove(*monitor); // removing each of the monitors from the list
		delete monitor; 	// delete will also cancel monitoring
		}	
	}


/*
Call CSysMonServer::AddMonitorL to add monitor to the linked list.
Also calls SetActive() on aMonitor.
*/	
void CSysMonServer::AddAndStartMonitorL(CMonitor* aMonitor)
	{
	iIter.SetToFirst();

	TProcessId id = aMonitor->ProcessId();
	CMonitor *monitor = NULL;
	while ((monitor = iIter++) != NULL)
		{
		if (monitor->ProcessId() == id) //  check if the process is already been monitored
			{
			DEBUGPRINT1(_L("This process is already been monitored."));
			User::Leave(KErrAlreadyExists);
			}			
		}

	aMonitor->Start(); // activate the monitoring

	iMonitors.AddFirst(*aMonitor);
	CleanupStack::Pop(aMonitor);

	DEBUGPRINT2(_L("Monitor inserted into SysMon server. ProcessId=%Lu"), id.Id());
	}
	

void CSysMonServer::CancelMonitor(const TProcessId& aProcessId)
	{
	iIter.SetToFirst();
	CMonitor *monitor = NULL;
	while ((monitor = iIter++) != NULL)
		{
		if (monitor->ProcessId() == aProcessId) // find the process in the list
			{
			break;
			}		
		}
	
	if (monitor == NULL)
		{
		return;
		}

	iMonitors.Remove(*monitor);// remove the monitor from the list.
	delete monitor;	// delete will also cancel monitoring
	}

CTimerList& CSysMonServer::TimerListL()
	{
	if (iTimerList == NULL)
		{
		iTimerList = CTimerList::NewL(EPriorityLow);
		}

	return *iTimerList;
	}


CSession2* CSysMonServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	return CSysMonSession::NewL();
	}


static void RunServerL()
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	DEBUGPRINT1(_L("SysMon: Going to run server"));	

	User::SetCritical(User::ESystemCritical);
	
	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	DEBUGPRINT1(_L("SysMon: Creating Server..."));	
	// create the server (leave it on the cleanup stack)
	CSysMonServer::NewLC();
	
	DEBUGPRINT1(_L("Doing Rendezvous..."));	
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	
	DEBUGPRINT1(_L("SysMon: Starting Scheduler..."));	
	// Ready to run
	CActiveScheduler::Start();

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2, s);
	}

TInt E32Main()
//
// Server process entry-point
//
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r = KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	
	__UHEAP_MARKEND;
	return r;
	}
