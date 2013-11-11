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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "ssmserver.h"
#include "ssmdebug.h"

#include "tssm_ssmclient.h"
#include "tssm_statepolicy_simple.h"

#include "tssm_startserver.h"



//
// Test-Server startup code
//

void RunTestSrvL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
    CleanupStack::PushL( sched );
    CActiveScheduler::Install( sched );

    CSsmServer* server = CSsmServer::NewLC(KSsmServerNameTest, ESsmTestSimpleState);
    
    // Don't need to be critical during test-step, better to just log errors.
    User::SetCritical(User::ENotCritical);
    
    RThread::Rendezvous( KErrNone );
    
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( sched );
	}

/**
 This function is called when the TestServer's thread is resumed.
 */
TInt TestSrvInitFunction( TAny* /*aNothing*/ )
	{
    CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if (cleanup)
		{
	    TRAP( err, RunTestSrvL());
	    delete cleanup;
		}
    return err;
	}

/**
 */
TInt StartServer(RThread& aServer)
	{
	DEBUGPRINT2( _L("Starting test-server %S"), &KSsmServerNameTest );
	
	const TInt KMinHeapSize =  0x1000; // 4kB
	const TInt KMaxHeapSize = 0x100000;// 1MB
	TInt err = aServer.Create( KSsmServerNameTest, &TestSrvInitFunction, 
								 KDefaultStackSize, KMinHeapSize, KMaxHeapSize, 
								 NULL); //NULL is the argument to TestSrvInitFunction
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


