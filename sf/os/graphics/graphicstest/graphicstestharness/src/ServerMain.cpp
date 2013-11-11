// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "GraphicsTestUtilsServer.h"
#include "GraphicsTestUtils.h"

static void RunServerL()
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CSmlTestUtilsServer::NewLC();
	//
	// naming the server thread after the server helps to debug panics
//	User::LeaveIfError(RThread::RenameMe(KTestServerName)); //DEPRECATED
	User::LeaveIfError(User::RenameThread(KTestServerName));
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	//
	// Ready to run
	CActiveScheduler::Start();

	// Ready to exit.
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

static TInt RunSmlTestServer()
//
// Main entry-point for the server thread
//
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



TInt E32Main()
// EXE entry point function.
	{
	return RunSmlTestServer();
	}



//
//
//
