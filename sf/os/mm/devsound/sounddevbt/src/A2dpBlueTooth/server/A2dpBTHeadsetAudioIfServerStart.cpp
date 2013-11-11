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
#include "A2dpBTHeadsetAudioIfServerStart.h"
#include "A2dpBTHeadsetAudioIfServer.h"


/**
Thread entry-point function.
**/
EXPORT_C TInt CA2dpBTHeadsetAudioIfServer::StartThread(TAny* /*aParms*/)
	{
	TInt err = KErrNone;
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		{
		err = KErrNoMemory;
		}
	else		
		{
		CActiveScheduler* sched = NULL;
		TRAP(err, sched = new(ELeave) CActiveScheduler);
		if (!err)
			{
			CActiveScheduler::Install(sched);
			CA2dpBTHeadsetAudioIfServer* server = NULL;
			TRAP(err, server = CA2dpBTHeadsetAudioIfServer::NewL());
			
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
	__UHEAP_MARKEND;
	return err;
	}
