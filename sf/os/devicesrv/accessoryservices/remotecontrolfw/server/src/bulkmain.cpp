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
// Entry point for Rem Con Bulk server.
//



/**
 @file
 @internalComponent
*/

#include "remconserver.h"
#include "bulkserver.h"

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

static void RunBulkServerL(RMsgQueue<TBulkServerMsg>* aMsgQueue);

/**
Entry point for the Remote Control server.
@return Error.
*/
TInt BulkMain(TAny* aParam)
	{
	CONNECT_LOGGER;
	LOG_STATIC_FUNC;
	TInt ret = KErrNoMemory;

	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();

	if ( cleanup )
		{
		TRAP(ret, RunBulkServerL(reinterpret_cast<RMsgQueue<TBulkServerMsg>*>(aParam)));
		delete cleanup;
		}

	__UHEAP_MARKEND;

	LOG1(_L8("\tret = %d"), ret);
	CLOSE_LOGGER;
	return ret;
	}

/**
Create the active scheduler and Rem Con bulk server instances, and start the 
server.
*/
static void RunBulkServerL(RMsgQueue<TBulkServerMsg>* aMsgQueue)
	{
	LOG_STATIC_FUNC;
	LEAVEIFERRORL(User::RenameThread(KRemConBulkServerName));

	// Create and install the active scheduler.
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create the server. NB Odd syntax in this line because we never refer to 
	// it directly- it's always owned by the cleanup stack!
	static_cast<void>(CRemConBulkServer::NewLC(*aMsgQueue));

	// Initialisation complete, now signal the client
	RThread().Rendezvous(KErrNone);

	// Ready to run. This only returns when the server is closing down.
	CActiveScheduler::Start();

	// Clean up the server and scheduler.
	CleanupStack::PopAndDestroy(2, scheduler);
	}
