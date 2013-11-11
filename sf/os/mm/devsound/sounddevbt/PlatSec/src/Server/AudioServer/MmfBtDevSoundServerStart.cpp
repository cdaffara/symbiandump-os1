// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "MmfBtDevSoundServerStart.h"
#include "MmfBtDevSoundServer.h"




EXPORT_C TInt CMMFDevSoundServer::StartThread(TAny* aParms)
/**
Thread entry-point function.
The TServerStart objects is passed as the thread parameter
**/
	{
	TInt err = KErrNone;
	__UHEAP_MARK;
	TDevSoundServerStart* start = reinterpret_cast<TDevSoundServerStart*>(aParms);
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		err = KErrNoMemory;
	if (!err)
		{
		CActiveScheduler* sched=NULL;
		TRAP(err, sched=new(ELeave) CActiveScheduler);
		if (!err)
			{
			CActiveScheduler::Install(sched);
			CMMFDevSoundServer* server = NULL;
			TRAPD(err,server = CMMFDevSoundServer::NewL(start->iAudioServer, start->iProcessId));
			
			if(!err)
				{
				// Sync with the client and enter the active scheduler
				RThread::Rendezvous(KErrNone);
				sched->Start();
				}
			delete server;
			delete sched;		
			}
		}
	delete cleanup;
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	return err;
	}
