// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsrv.h"
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsess.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "savepriv.h"
#include <w32std.h>
#include <e32std.h>
#include <e32svr.h>

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
#include <ssm/ssmpatchableconstants.h>
#endif

_LIT(KRomPath,"Z:\\System\\Libs\\");
_LIT(KFullShutDownServer,"eikshtsrv.dll");

void LoadServerL(CServShutdownServer*& aServer, RLibrary& aLibrary)
	{
	TFullName threadName(_L("*"));
	threadName.Append(KWSERVThreadName);
	TFindThread findWserv(threadName);
	TFullName name;
	if (findWserv.Next(name)!=KErrNone)
		{
		aServer=CServShutdownServer::NewL();
		}
	else
		{
		User::LeaveIfError(aLibrary.Load(KFullShutDownServer, KRomPath));
		TLibraryFunction loadServer = aLibrary.Lookup(1);
		aServer=reinterpret_cast<CServShutdownServer*>((*loadServer)());
		}
	}
static void RunServerL()
	{
	RThread thread;
	User::LeaveIfError(User::RenameThread(__SHUTDOWN_SERVER_NAME));
	thread.SetPriority(EPriorityRealTime);
	thread.Close();


	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CServShutdownServer* server = NULL;
	RLibrary library;
	CleanupClosePushL(library);

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	if (IsSsmGracefulShutdown())	// SSM should be used for device shutdown
		{
		server = CServShutdownServer::NewL();
		}
	else// continue using eikshtsrv.dll
		{
		LoadServerL(server,library);
		}
#else
	// use eikshtsrv.dll not SSM
	LoadServerL(server,library);
#endif // SYMBIAN_SSM_GRACEFUL_SHUTDOWN

	CleanupStack::PushL(server);
	server->ConstructL();

	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(3,scheduler);
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}


