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



#include "t_devsoundserver.h"
#include "t_cmmfdevsounddata.h"
#include "t_crepositorydata.h"

/*@{*/
/** Object wrappers literals */
_LIT(KT_CMMFDevSound, 	"CMMFDevSound");
_LIT(KT_CRepository,	"CRepository");
_LIT(KApplicationName,"AccServer.exe");
_LIT(KPROCESSNAME, "*AccServer*");
_LIT(KCmdName,"");
/*@}*/

/**
 * 
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 * @return - Instance of the test server
 */
CT_DevSoundServer* CT_DevSoundServer::NewL()
	{
    CT_DevSoundServer* server = new (ELeave) CT_DevSoundServer();
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
    CT_DevSoundServer* server = NULL;

    // Create the CTestServer derived server
    TRAPD(err, server = CT_DevSoundServer::NewL());
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
 * Creates an instance of CDataWrapper that wraps a CT_CMMFDevSoundData object 
 * @return wrapper	- a CDataWrapper instance that wraps the CT_CMMFDevSoundData object
 */
CDataWrapper* CT_DevSoundServer::CT_DevSoundBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper* wrapper = NULL;

	if (KT_CMMFDevSound() == aData)
		{
		wrapper = CT_CMMFDevSoundData::NewL();
		}
	else if (KT_CRepository() == aData)
		{
		wrapper = CT_CRepositoryData::NewL();
		}	
	return wrapper;
	}

/*
 * Start the accessories server
 * @return err 		Symbian error code. KErrNone if successful
 */
void CT_DevSoundServer::DoCmdutilStartApp()
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
