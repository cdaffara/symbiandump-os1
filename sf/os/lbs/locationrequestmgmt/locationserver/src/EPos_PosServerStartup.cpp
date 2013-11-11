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



// INCLUDE FILES
#include <e32base.h>
#include <lbsipc.h>
//#include <StartPosServer.h>
//#include "EPos_PosServerStartup.h"
#include "EPos_CPosServer.h"
#include "EPos_Global.h"

#ifdef _DEBUG
_LIT(KTraceFileName, "EPos_PosServerStartup.cpp");
#endif

// ================= GLOBAL FUNCTIONS =======================

#ifdef _DEBUG
void GlobalTraceCleanUp(TAny* /*aAny*/)
    {
    Global::TearDownTrace();
    }
#endif

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// StartEposServerL
// Server startup routine
// ---------------------------------------------------------
//
void StartEposServerL()
	{

	// Rename thread to aid debugging
	User::LeaveIfError(RThread().RenameMe(KPositionServerName));

	// Start scheduler and server

	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

#ifdef _DEBUG
    Global::SetupTraceL();
    TCleanupItem cItem(GlobalTraceCleanUp);
    CleanupStack::PushL(cItem);
#endif

    DEBUG_TRACE("Creating server object...", __LINE__)
    CPosServer* server = CPosServer::NewL();
	CleanupStack::PushL(server);

    DEBUG_TRACE("Rendezvous with LbsApi...", __LINE__)
	// Signal that we've started
	RProcess::Rendezvous(KErrNone);
	
	CleanupStack::Pop(server);

#ifdef _DEBUG
	CleanupStack::Pop(); // GlobalTrace CleanupItem
#endif
	CleanupStack::Pop(scheduler);
	
    DEBUG_TRACE("Starting scheduler...", __LINE__)
    CActiveScheduler::Start();
    DEBUG_TRACE("Scheduler stopped, destroying server.", __LINE__)

#ifdef _DEBUG
    Global::TearDownTrace();
#endif

	delete server;
	delete scheduler;
	}

//
// ---------------------------------------------------------
// E32Dll
// Server process entry-point
// Recover the startup parameters and run the server
// ---------------------------------------------------------
TInt E32Main()
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

//  End of File
