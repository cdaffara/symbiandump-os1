// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "MmfAudioPolicyServer.h"

_LIT(KMMFAudioPolicyServerName,"MMFAudioPolicyServer");

EXPORT_C TInt CMMFAudioPolicyServer::StartThread(TAny* aParms)
/**
Thread entry-point function.
The TServerStart objects is passed as the thread parameter
**/
	{
	TInt err = KErrNone;
	__UHEAP_MARK;
	//This line will be used if parameters need to be passed.
	TServerStart* start = reinterpret_cast<TServerStart*>(aParms);
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
	__UHEAP_MARKEND;
	return err;
	}

void CMMFAudioPolicyServer::StartThreadL(TServerStart& aStart)
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	
	CActiveScheduler::Install(sched);
	//Rename Audio Policy server name
	RThread audioPolicyThread;
	TName name;
    name.Append(KMMFAudioPolicyServerName);
	//We are ignoring the error code returned from User::RenameThread
	//as it is not important here, may be for profiling
	User::RenameThread(name);
	CMMFAudioPolicyServer* server = CMMFAudioPolicyServer::NewL();
	CleanupStack::PushL(server);
	
	aStart.iPolicyServerHandle = server->Server();
	// Sync with the client and enter the active scheduler
	RThread::Rendezvous(KErrNone);
	sched->Start();

	CleanupStack::PopAndDestroy(2, sched); // sched, server
	}

