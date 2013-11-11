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
#include "MmfBtAudioPolicyStart.h"
#include "MmfBtAudioPolicyServer.h"
#include "mmfcontrollerframeworkbase.h"

TInt TServerStart::Signal()
//
// Signal the owning thread that the server has started successfully
//
	{
	RThread starter;
	TInt err = starter.Open(iId);
	if(err==KErrNone)
		{
		starter.RequestComplete(iStatus,KErrNone);
		starter.Close();
		}
	return(err);
	}

LOCAL_C void DoStartThreadL(TServerStart* aParams)
	{
	//create the active scheduler
	CActiveScheduler* s = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//create the server & leave it on the cleanupstack
	CleanupStack::PushL(CMMFAudioPolicyServer::NewL());
	//initialisation complete - now signal the client
	User::LeaveIfError(aParams->Signal());
	//start the server running
	CActiveScheduler::Start();
	//now exiting the server so cleanup
	CleanupStack::PopAndDestroy(2);//scheduler and server
	}


EXPORT_C TInt EntryPoint(TAny* aParam)
	{
	TInt err = KErrNone;
	__UHEAP_MARK;
	TServerStart* start = REINTERPRET_CAST(TServerStart*, aParam);
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		err = KErrNoMemory;
	if (!err)
		{
		TRAP(err, DoStartThreadL(start));
		}
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}


TInt TServerStart::GetCommand()
/**
In EPOC, the EPOCEXE target is a process, and the server startup
parameters are encoded in the command line
**/
	{
	if (User::CommandLineLength()!=sizeof(TServerStart)/sizeof(TText))
		return KErrGeneral;
	TPtr ptr(reinterpret_cast<TText*>(this),0,sizeof(TServerStart)/sizeof(TText));
	User::CommandLine(ptr);
	return KErrNone;
	}

TInt E32Main()
/**
Server process entry-point
Recover the startup parameters and run the server
**/
	{
	TServerStart start;
	TInt r=start.GetCommand();
	if (r==KErrNone)
		r=EntryPoint((TAny*)&start);
	return r;
	}

