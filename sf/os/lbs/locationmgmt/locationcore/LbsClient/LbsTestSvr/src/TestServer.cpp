// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32svr.h>
#include "TestServer.h"
#include "TestSession.h"



CEposServServer::CEposServServer(TInt aPriority)
 : CServer2(aPriority)
	{
	}

CSession2* CEposServServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMessage*/) const
	{
	TVersion v(KPosMajorVersionNumber, KPosMinorVersionNumber, KPosBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);

	return new (ELeave) CEposServSession();
	}

void CEposServServer::AddSession()
//
// A new session is being created
//
	{
	++iSessionCount;
	}

void CEposServServer::DropSession()
//
// A session is being destroyed
//
	{
	if (--iSessionCount==0)
		CActiveScheduler::Stop();
	}



// The test server thread
TInt CEposServServer::StartEposServer()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanupStack = CTrapCleanup::New();

	TInt ret = KErrNoMemory;
	if (cleanupStack)
		{
		TRAP(ret, StartEposServerL());
		delete cleanupStack;
		}

	__UHEAP_MARKEND;

	return ret;
	}


void CEposServServer::StartEposServerL()
	{
	// Rename thread to aid debugging
#ifndef EKA2
	User::LeaveIfError(RThread().Rename(KPositionServerName));
#endif
	// start scheduler and server
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CEposServServer* server = new(ELeave) CEposServServer(EPriority);
	CleanupStack::PushL(server);

	server->StartL(KPositionServerName);

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
	return CEposServServer::StartEposServer();
	}
