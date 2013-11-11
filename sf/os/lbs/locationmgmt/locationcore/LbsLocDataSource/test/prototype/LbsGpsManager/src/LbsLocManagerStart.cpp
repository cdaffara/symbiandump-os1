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

#include <e32std.h>
#include <e32base.h>

#include "LbsLocManagerStart.h"
#include "LbsLocManagerRoot.h"
#include "LbsLasCommon.h"

void TLocationManagerStart::DoLasStartL()
	{
	LASLOGNEW(">>>>>>>>>>>>>>>>> Gps Location Manager Thread Started >>>>>>>>>>>>>>>>>");

//>>#if (defined __WINS__ && !defined EKA2)
#if (!defined EKA2)
	// Rename thread to aid debugging
	User::LeaveIfError(RThread().Rename(KPositionServerName));
#endif

	// start scheduler and server
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;

	CleanupStack::PushL(scheduler);

	CActiveScheduler::Install(scheduler);

	LASLOG("Server Thread Scheduler Installed");

	CLocationManagerRoot* server = CLocationManagerRoot::NewL();

	CleanupStack::PushL(server);

	//	server->StartL(KPositionServerName);


	LASLOG("----------------- Gps Location Manager Service Created -------------------");
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // server
	CleanupStack::PopAndDestroy(); // scheduler


	LASLOG("<<<<<<<<<<<<<<<<< Gps Location Manager Thread Terminating <<<<<<<<<<<<<<<<<");
	}


TInt TLocationManagerStart::StartLocationManager()
/**
The positioning server thread

@param aStart 
@return a Symbian OS error code 
*/
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanupStack = CTrapCleanup::New();

	TInt ret = KErrNoMemory;
	if (cleanupStack != NULL)
		{
		TRAP(ret, DoLasStartL());
		delete cleanupStack;
		}

	__UHEAP_MARKEND;

	return ret;
	}

#if (defined __WINS__ && !defined EKA2)

// In WINS, the EPOCEXE target is a DLL with an entry point called WinsMain,
// taking no parameters and returning TInt. This is not really valid as a thread
// function which takes a TAny* parameter which we need.
//
// So the DLL entry-point WinsMain() is used to return a TInt representing the
// real thread function within the DLL. This is good as long as
// sizeof(TInt)>=sizeof(TThreadFunction).
//

static TInt ThreadFunction()
/**
WINS thread entry-point function.
The TServerStart objects is passed as the thread parameter
*/
	{
	return TLocationManagerStart::StartLocationAquisitionService();
	}

IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
/**
WINS DLL entry-point. Just return the real thread function
cast to TInt
*/
	{
	return reinterpret_cast<TInt>(&ThreadFunction);
	}

TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}

#else

//
// In EPOC, the EPOCEXE target is a process, and the server startup
// parameters are encoded in the command line
//


TInt E32Main()
/**
Server process entry-point
*/
	{
	TInt ret = TLocationManagerStart::StartLocationManager();

	return ret;
	}

#endif
