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

// INCLUDE FILES
#include <e32std.h>
#include <a3f/mmfaudioserverinterface.h>

// -----------------------------------------------------------------------------
// MainL
// Just an E32Main and a MainL()
// -----------------------------------------------------------------------------
//
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CAudioSvrLoader* server = NULL;
	TRAPD(err,server = CAudioSvrLoader::NewL() );
	// Sync with the client
	RProcess::Rendezvous(err);
	if(err == KErrNone)
		{
		// enter the active scheduler
		sched->Start();
		}
	delete server;
	server = NULL;
	delete sched;
	sched = NULL;
	}

// -----------------------------------------------------------------------------
// E32Main
// Just an E32Main and a MainL()
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	cleanup = NULL;
	return KErrNone;
	}

// End of file
