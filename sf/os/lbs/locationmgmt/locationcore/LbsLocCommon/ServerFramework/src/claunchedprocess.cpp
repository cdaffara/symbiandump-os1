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

#include "lbsmemlogger.h"
#include "claunchedprocess.h"
#include "csecureprocessasbase.h"
#include "lbsdevloggermacros.h"

/** Create instance of trap handler and cleanup stack
called by the launched process! 

@return Symbian error code
@internalTechnology
@released
 */
 TInt CLaunchedProcess::CompleteProcessLaunch()
   	{
   	__UHEAP_MARK;
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt r=KErrNoMemory;
   	if (cleanup)
   		{
 		TRAP(r,DoCompleteProcessLaunchL());
   		delete cleanup;
   		}
   	__UHEAP_MARKEND;
   	return r;	
   
   	}
      	

/** Create the server process, create, install and run the active scheduler
wait for scheduler to be stopped to return process

@return Symbian error code
@internalTechnology
@released
 */
 TInt CLaunchedProcess::DoCompleteProcessLaunchL()
 	{ 	
   	TInt r = KErrNone;
   	// create and install the active scheduler we need
   	CSecureProcessASBase* s=new(ELeave) CSecureProcessASBase();
   	CleanupStack::PushL(s);
   	CActiveScheduler::Install(s);
   	
   	// Create the memory logger object for monitoring memory
   	LBSMEMLOG_BEGIN();
   
   	CBase* baseObj = s->CreateRootObjectL();
   
   	CleanupStack::PushL(baseObj);
   	
   	r = s->EntryPointL(baseObj);
   	
   	RProcess().Rendezvous(KErrNone);
   	
   	// Ready to run
   	CActiveScheduler::Start();
   	
   	//
   	// Cleanup the client
   	CleanupStack::PopAndDestroy(1);

   	// Cleanup the mem logger
   	LBSMEMLOG_END();
 
   	//
   	// do any _FINAL _ operations. If anything.. default does nothing
   	s->Final();
   	// Cleanup the scheduler
   	CleanupStack::PopAndDestroy(s);
   	return r;
   	}		


/** Create instance of trap cleanup stack and the process by specified parameters

@param aParams A reference to TProcessStartParams object
@return Symbian error code
@see TProcessStartParams
@internalTechnology
@released
 */
 TInt CLaunchedProcess::CompleteProcessLaunch(TProcessStartParams& aParams)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,DoCompleteProcessLaunchL(aParams));
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;	

	}

/** Create the server process by specified parameters, create, install and run the active scheduler
wait for scheduler to be stopped to return process

@param aParams A reference to TProcessStartParams object
@return Symbian error code
@see TProcessStartParams
@internalTechnology
@released
 */
 TInt CLaunchedProcess::DoCompleteProcessLaunchL(TProcessStartParams& aParams)
	{	
 	// first check that we are about to create the right type of server
	if(aParams.GetType() != KProcessStartParamsTypeUid)
		{
		// this is not a process type that we understand. Has the caller
		// pass TServerStartParams by mistake?
		User::Leave(KErrArgument); // this aborts the server startup
		}

	// rename our main thread	
	User::LeaveIfError(User::RenameThread(aParams.GetProcessName()));
	
	TInt r = KErrNone;
	// create and install the active scheduler we need
	CSecureProcessASBase* s=new(ELeave) CSecureProcessASBase();
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

   	// Create the memory logger object for monitoring memory
   	LBSMEMLOG_BEGIN();

	CBase* baseObj = s->CreateRootObjectL();

	CleanupStack::PushL(baseObj);
	
	r = s->EntryPointL(baseObj);
	
	// is the process that started us interested in known that we are alive and well
	if(aParams.GetRendezvousRequired())
		{		
		// tell any starting process that we have started successfully and are
		// now ready to receieve mesages or process events
		RProcess::Rendezvous(KErrNone);
		}
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the client
	CleanupStack::PopAndDestroy(1);

   	// Cleanup the mem logger
   	LBSMEMLOG_END();

	//
	// do any _FINAL _ operations. If anything.. default does nothing
	s->Final();
	// Cleanup the scheduler
	LBSLOG(ELogP1, "CLaunchedProcess::Destroy scheduler \n");
	CleanupStack::PopAndDestroy(s);
	LBSLOG(ELogP1, "CLaunchedProcess::Scheduler destroyed\n");
	return r;
	}		
	
