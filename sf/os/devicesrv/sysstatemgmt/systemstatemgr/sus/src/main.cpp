// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include "ssmdebug.h"
#include "susutilserver.h"
#include "susadaptionserver.h"
#include "susadaptionclisrv.h"


//
// Functions run in the Adaption Server's thread
//

/**
 This function is creating the AdaptionServer and starting its scheduler.
 */
static void RunAdaptionSrvL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
    CleanupStack::PushL( sched );
    CActiveScheduler::Install( sched );

    CSsmAdaptationServer* server = CSsmAdaptationServer::NewLC();
    RThread::Rendezvous( KErrNone );
    
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( sched );
	}

/**
 This function is called when the AdaptionServer's thread is resumed.
 */
static TInt AdaptionSrvInitFunction( TAny* /*aNull*/ )
	{
    CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if (cleanup)
		{
	    TRAP( err, RunAdaptionSrvL());
	    delete cleanup;
		}
    return err;
	}

//
// Functions to launch the servers (these will run in the main thread)
//

/**
Function to launch the AdaptionServer in a new thread.
 */
static TInt StartAdaptionSrv()
	{
	DEBUGPRINT1( _L("Starting SusAdaptionServer") );

	RThread server;
	const TInt KMinHeapSize =  0x1000; // 4kB
	const TInt KMaxHeapSize = 0x100000;// 1MB

	TInt err = server.Create( KSusAdaptionServerName, &AdaptionSrvInitFunction, 
								 KDefaultStackSize, KMinHeapSize, KMaxHeapSize, 
								 NULL );

	if( KErrNone == err )
		{
		TRequestStatus trs;
		server.Rendezvous( trs );
		server.Resume();
		
		User::WaitForRequest( trs );	
		
		//We can't use the 'exit reason' if the server panicked as this is the 
		//panic 'reason' and may be '0' which cannot be distinguished from KErrNone
		err = (server.ExitType()==EExitPanic) ? KErrGeneral : trs.Int();
		server.Close();
		}

	return err;
	}

/**
Function to launch the UtilServer in this thread.
 */
static void StartUtilSrvL()
	{
	DEBUGPRINT1( _L("Starting SusUtilServer") );
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);	
	CActiveScheduler::Install(sched);

	CSusUtilServer* server = CSusUtilServer::NewLC();
	RProcess::Rendezvous(KErrNone);
	sched->Start();

	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(sched);
	}

//
// The main() function itself
//
/**
Process entry point. Called by client using RProcess API
@return - Standard Epoc error code on process exit
*/
TInt E32Main() //lint -e765 -e714 Suppress 'not referenced' and 'could be static'
	{
	__UHEAP_MARK;
	TInt err = StartAdaptionSrv(); //started in a new thread
	if(KErrNone == err)
		{
		CTrapCleanup* cleanup = CTrapCleanup::New();
		err = KErrNoMemory;
		if(cleanup)
			{
			TRAP(err, StartUtilSrvL()); //started in this thread
			delete cleanup;
			}
		}
	__UHEAP_MARKEND;
	return err;
	}

