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
#include <e32std.h>
#include <domainmanager.h>
#include <bacline.h> // for command line arguments

#include "ssmserver.h"
#include "ssmserverpanic.h"
#include <ssm/ssmstates.hrh>


static void MainL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);	
	CActiveScheduler::Install(sched);

	// Add the system state domain hierarchy, named KDmHierarchyIdStartup for
	// compatibility reasons.
	// Domain manager has already been started by estart
	TInt err = RDmDomainManager::AddDomainHierarchy(KDmHierarchyIdStartup);
	if(KErrNone != err && KErrAlreadyExists != err)
		{
		PanicNow(KPanicSysStateMgr, EAddDomainHierarchyError);
		}
	
	CSsmServer* server = CSsmServer::NewLC(ESsmStartup);
	sched->Start();

	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(sched);
	}

/**
This process entry point, is called by client using RProcess API
@return - Standard Epoc error code on process exit
*/
TInt E32Main() //lint -e765 -e714 Suppress 'not referenced' and 'could be static'
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if(cleanup)
		{
		TRAP(err, MainL());
		delete cleanup;
		}
	
	__UHEAP_MARKEND;
	return err;
    }

