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
#include "MmfBtAudioServerStart.h"
#include "MmfBtAudioServer.h"

#include "A2dpBTHeadsetAudioIfServerStart.h"
#include "A2dpBTHeadsetAudioIfServer.h"

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
 	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CMMFAudioServer* server = NULL;
	TRAPD(err,server = CMMFAudioServer::NewL());

	// Start BT audio server in its own thread here...
	// Assume that the server's not been started already
	// 
	RThread btServerThread;
	TThreadFunction btServerThreadFunc = CA2dpBTHeadsetAudioIfServer::StartThread;
	TName btServerName(KA2DPAudioServerName);


	err = btServerThread.Create(btServerName, btServerThreadFunc, KBTAudioServerStackSize,
								KBTAudioServerInitHeapSize, KBTAudioServerMaxHeapSize,
								NULL, EOwnerProcess);	// NULL => not passing any params to thread

	if(!err)
		{
		// Synchronise with the server
		TRequestStatus reqStatus;
		btServerThread.Rendezvous(reqStatus);

		if (reqStatus != KRequestPending)
			{
			btServerThread.Kill(0);
			}
		else 
			{
			// Start the thread
			btServerThread.Resume();
			// Server will call the reciprocal static synchronise call
			}
		User::WaitForRequest(reqStatus); // wait for start or death
		}
		
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	
	btServerThread.Close();
		
	delete server;
	delete sched;	
	
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }
