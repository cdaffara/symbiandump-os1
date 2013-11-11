
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

#include <e32std.h>
#include <ecom/ecom.h>
#include "mmfdevsoundserverstart.h"
#include "mmfdevsoundserver.h"


//OTHER EXPORTED FUNCTIONS 

/**
 	CMMFDevSoundServer::StartThread
 	Thread entry-point function.
 	The TServerStart objects is passed as the thread parameter
 */
EXPORT_C TInt CMMFDevSoundServer::StartThread(TAny* aParms)
	{
	TInt err = KErrNone;
#ifdef SYMBIAN_USE_SEPARATE_HEAPS
	__UHEAP_MARK;
#endif
	TDevSoundServerStart* start = reinterpret_cast<TDevSoundServerStart*>(aParms);
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		err = KErrNoMemory;
		}
	else
		{
		TRAP(err, StartThreadL(*start));
		}
	delete cleanup;
	
	REComSession::FinalClose();
#ifdef SYMBIAN_USE_SEPARATE_HEAPS
	__UHEAP_MARKEND;
#endif
	return err;
	}

void CMMFDevSoundServer::StartThreadL(TDevSoundServerStart& aStart)
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	
	CActiveScheduler::Install(sched);
	CMMFDevSoundServer* server = CMMFDevSoundServer::NewL(aStart.iAudioServer, aStart.iProcessId);
	CleanupStack::PushL(server);
	
	//Rename and set the priority of devsound server
	RenamePrioritizeServer();
	
	aStart.iDevSoundServerHandle = server->Server();
	// Sync with the client and enter the active scheduler
	RThread::Rendezvous(KErrNone);
	sched->Start();

	CleanupStack::PopAndDestroy(2, sched); // sched, server
	}

//  End of File
