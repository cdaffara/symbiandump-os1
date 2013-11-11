// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Thread entry point functions for the test framework's thread used to run the active scheduler
// 
//

/**
 @file
 @internalComponent
*/

#include "InputRequestListener.h"
#include <in_iface.h>
#include "reteldriverinput.h"
#include "ceteldriverfactory.h"
#include "cpdpfsmfactory.h"

/**
 Creates and starts the active scheduler, and initializes the units under test
 */
void ActiveSchedulerMainL(CInputRequestListener* aListener)
	{
	CActiveScheduler *sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	__UHEAP_MARK;

	CActiveScheduler::Add(aListener);
	
	aListener->CreateUnitUnderTestL();

	// listen for requests from the main test thread
	aListener->Activate();
	
	// notify the test execute thread that we are initialized and ready to handle FSM or Etel driver Input requests
	TRequestStatus *readyStatus = &aListener->iReadyForInputRequest;
 	aListener->iParentThread.RequestComplete(readyStatus, KErrNone);
	TRequestStatus *initStatus = &aListener->iThreadInitialized;
 	aListener->iParentThread.RequestComplete(initStatus, KErrNone);

	CActiveScheduler::Start();
	
	// this could be done in the destructor, but we do it here so that the UHEAP macros work out for this thread
	// (otherwise they would be deleted in the main test thread)
	aListener->DeleteUnitUnderTest();
	
	__UHEAP_MARKEND;

	CleanupStack::PopAndDestroy(sched);	

	// inform the main test thread that this thread is finished and that there are no memory leaks
	TRequestStatus *destructedStatus = &aListener->iThreadDestructed;
	aListener->iParentThread.RequestComplete(destructedStatus, KErrNone);	
	}
	
/**
 Create the trap cleanup and calls the main thread f'n
 */
TInt ActiveSchedulerThread(TAny* aActiveSchedulerParams)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	
	TActiveSchedulerThreadParams *params = (TActiveSchedulerThreadParams*)aActiveSchedulerParams;
	TRAPD(err, ActiveSchedulerMainL(params->iListener));
	
	delete cleanup;
	
	return err;
	}
