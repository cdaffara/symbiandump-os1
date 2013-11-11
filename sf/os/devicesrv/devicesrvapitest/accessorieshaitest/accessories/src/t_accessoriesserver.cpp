/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "t_accessoriesserver.h"
#include "t_raccessoryconnectiondata.h"
#include "t_raccessoryserverdata.h"


_LIT(KT_Connection, "RAccessoryConnection");
_LIT(KT_Server, 	"RAccessoryServer");
_LIT(KApplicationName,"AccServer.exe");
_LIT(KPROCESSNAME, "*AccServer*");
_LIT(KCmdName,"");

/**
 * 
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 * @return - Instance of the test server
 */
CT_AccessoriesServer* CT_AccessoriesServer::NewL()
	{
    CT_AccessoriesServer* server = new (ELeave) CT_AccessoriesServer();
    CleanupStack::PushL(server);
    server->ConstructL();
    server->DoCmdutilStartApp();
    CleanupStack::Pop(server);
    return server;
    }

/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
LOCAL_C void MainL()
	{
#if (defined __DATA_CAGING__)
    RProcess().DataCaging(RProcess::EDataCagingOn);
    RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
    CActiveScheduler* sched = NULL;
    sched = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CT_AccessoriesServer* server = NULL;

    // Create the CTestServer derived server
    TRAPD(err, server = CT_AccessoriesServer::NewL());
    if(!err)
	    {
        // Sync with the client and enter the active scheduler
        RProcess::Rendezvous(KErrNone);
        sched->Start();
        }

    delete server;
    delete sched;
    }

/**
 * 
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 * @return - Standard Epoc error code on process exit
 */
GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
	    {
        return KErrNoMemory;
        }

#if (defined TRAP_IGNORE)
	TRAP_IGNORE(MainL());
#else
    TRAPD(err,MainL());
#endif

    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
    }
/*
 * Creates an instance of CDataWrapper that wraps a CT_AccessoriesDriverData object 
 * @return wrapper	- a CDataWrapper instance that wraps the CT_AccessoriesDriverData object
 */
CDataWrapper* CT_AccessoriesServer::CT_AccessoriesBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper* wrapper = NULL;

	if( KT_Connection() == aData )
		{
		wrapper = CT_RAccessoryConnectionData::NewL();
		}
	else if ( KT_Server() == aData )
		{
		wrapper = CT_RAccessoryServerData::NewL();
		}

	return wrapper;
	}

/*
 * Start the accessories server
 * @return void
 */
void CT_AccessoriesServer::DoCmdutilStartApp()
	{
	RProcess process;
	TInt err(KErrNone);
	TFullName processName;
	TFindProcess findProcess(KPROCESSNAME);
	findProcess.Next(processName);
  if (processName != KNullDesC)
    {
    return;
    }
  
	err = process.Create(KApplicationName,KCmdName);
	// start the process running! Don't forget this.
	if (err==KErrNone)
        {
        process.Resume(); //start .exe application
        }
	process.Close(); // Closes the handle, not the process.
	}
