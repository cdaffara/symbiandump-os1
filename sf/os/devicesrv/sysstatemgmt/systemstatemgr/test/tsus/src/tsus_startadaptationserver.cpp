// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:Test Adaptation server 
//

/**
 @file
 @test
 @internalComponent - Internal test code 
*/

#include <e32base.h>
#include "tsus_startadaptationserver.h"
#include "susadaptionserver.h"
#include "ssmdebug.h"



////////////////////////////////////////////////////////////////////////////////
// Test-Server startup code
////////////////////////////////////////////////////////////////////////////////

void RunAdaptationTestSrvL()
	{
	__UHEAP_MARK;
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
    CleanupStack::PushL( sched );
    CActiveScheduler::Install( sched );

    CSsmAdaptationServer* server = CSsmAdaptationServer::NewLC(KTestAdaptationServerName);
    RThread::Rendezvous( KErrNone );
    
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( sched );
    __UHEAP_MARKEND;
	} //lint !e1746 Suppress parameter 'aSid' could be made const reference

/**
 This function is called when the TestServer's thread is resumed.
 */
TInt TestAdaptationSrvInitFunction(TAny* /*aNothing*/)
	{
    CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if (cleanup)
		{
	    TRAP( err, RunAdaptationTestSrvL());
	    delete cleanup;
		}
    return err;
	}

/**
 */
TInt StartAdaptationServer(RThread& aServer)
	{
	DEBUGPRINT2( _L("Starting test-server %S"), &KTestAdaptationServerName );
	
	const TInt KMinHeapSize =  0x1000; // 4kB
	const TInt KMaxHeapSize = 0x100000;// 1MB
	
	
	TInt err = aServer.Create( KTestAdaptationServerName, &TestAdaptationSrvInitFunction, 
	                                 KDefaultStackSize, KMinHeapSize, KMaxHeapSize, 
	                                 NULL);
	if( KErrNone == err )
		{
		TRequestStatus trs;
		aServer.Rendezvous( trs );
		aServer.Resume();
		
		User::WaitForRequest( trs );	
		
		//We can't use the 'exit reason' if the server panicked as this is the 
		//panic 'reason' and may be '0' which cannot be distinguished from KErrNone
		err = (aServer.ExitType()==EExitPanic) ? KErrGeneral : trs.Int();
		}

	return err;
	}
