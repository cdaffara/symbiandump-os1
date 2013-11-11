// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <ecom/ecom.h>
#include "mmcameraserver.h"

// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
static void RunServerL()
	{	
	User::LeaveIfError(User::RenameProcess(KMMCameraServerName));

	// create and install the active scheduler we need
	CActiveScheduler* activeScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(activeScheduler);
	CActiveScheduler::Install(activeScheduler);

	// Create the server, leaving it on the cleanup stack.
	CMMCameraServer::NewLC();

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

    // Ready to run
	CActiveScheduler::Start();

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2, activeScheduler);
	}

/**
 * Program entry point.
 * Symbian OS will call this method when the server process is started.
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNoMemory;

	if (cleanup)
		{
		TRAP(err,RunServerL());
		delete cleanup;
		}

	REComSession::FinalClose();
	__UHEAP_MARKEND;
	return err;
	}
