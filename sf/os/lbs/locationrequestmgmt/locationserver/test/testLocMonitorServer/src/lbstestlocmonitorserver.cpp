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


#include <e32std.h>
#include <e32svr.h>
#include "lbstestlocmonitorserver.h"
#include "lbstestlocmonitorsession.h"
#include "lbstestlocmonitorsubsession.h"


CTe_LocMonitorServer::CTe_LocMonitorServer(TInt aPriority): CServer2(aPriority)
	{
	}

CSession2* CTe_LocMonitorServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{	
	return new (ELeave) CTe_LocMonitorServerSession(); 
	}

void CTe_LocMonitorServer::AddSession()
//
// A new session is being created
//
	{
	++iSessionCount;
	}

void CTe_LocMonitorServer::DropSession()
//
// A session is being destroyed
//
	{
	if (--iSessionCount==0)
		CActiveScheduler::Stop();
	}



// The test server thread
TInt CTe_LocMonitorServer::StartLocMonitorServer()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanupStack = CTrapCleanup::New();

	TInt ret = KErrNoMemory;
	if (cleanupStack)
		{
		TRAP(ret, StartLocMonitorServerL());
		delete cleanupStack;
		}

	__UHEAP_MARKEND;

	return ret;
	}


void CTe_LocMonitorServer::StartLocMonitorServerL()
	{
	// Rename thread to aid debugging
#ifndef EKA2
	User::LeaveIfError(RThread().RenameMe(KLocMonServerName));
#endif
	// start scheduler and server
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CTe_LocMonitorServer* server = new(ELeave) CTe_LocMonitorServer(EPriorityStandard);
	CleanupStack::PushL(server);

	server->StartL(KLocMonServerName);

	// signal that we've started
	RProcess().Rendezvous(KErrNone);

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, scheduler);
	}

TInt E32Main()
//
// Server process entry-point
// Recover the startup parameters and run the server
//
	{
	return CTe_LocMonitorServer::StartLocMonitorServer();
	}
