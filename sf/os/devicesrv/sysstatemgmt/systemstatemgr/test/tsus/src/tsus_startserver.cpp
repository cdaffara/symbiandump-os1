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

#include <e32base.h>
#include "tsus_startserver.h"
#include "susutilserver.h"
#include "ssmdebug.h"



//
// Test-Server startup code
//

void RunTestSrvL(TSecureId aSid)
	{
	__UHEAP_MARK;
	CActiveScheduler* sched = new(ELeave) CActiveScheduler();
    CleanupStack::PushL( sched );
    CActiveScheduler::Install( sched );

    CSusUtilServer* server = CSusUtilServer::NewLC(KTestServerName, aSid);
    RThread::Rendezvous( KErrNone );
    
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( sched );
    __UHEAP_MARKEND;
	} //lint !e1746 Suppress parameter 'aSid' could be made const reference

/**
 This function is called when the TestServer's thread is resumed.
 */
TInt TestSrvInitFunction( TAny* aSecureId )
	{
    CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if (cleanup)
		{
	    TRAP( err, RunTestSrvL((TUint32)aSecureId));
	    delete cleanup;
		}
    return err;
	}

/**
 */
TInt StartServer(RThread& aServer, TUint32 aClientSecureId)
	{
	DEBUGPRINT2( _L("Starting test-server %S"), &KTestServerName );
	
	const TInt KMinHeapSize =  0x1000; // 4kB
	const TInt KMaxHeapSize = 0x100000;// 1MB
	TInt err = aServer.Create( KTestServerName, &TestSrvInitFunction, 
								 KDefaultStackSize, KMinHeapSize, KMaxHeapSize, 
								 (TAny*)aClientSecureId); //the secureid of the only process allowed to connect
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
