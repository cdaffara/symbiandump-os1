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


#include "t_devvideoserver.h"
#include "t_cmmfdevvideoplaydata.h"
#include "t_cmmfdevvideorecorddata.h"

/*@{*/
_LIT(KT_VideoDriver,	"CMMFDevVideoPlay");
_LIT(KT_VideoRecording,	"CMMFDevVideoRecord");
/*@}*/


/**
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 * @return - Instance of the test server
 */
CT_DevVideoServer* CT_DevVideoServer::NewL()
	{
    CT_DevVideoServer* server = new (ELeave) CT_DevVideoServer();
    CleanupStack::PushL(server);
    server->ConstructL();
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
    CActiveScheduler*	sched=NULL;
    sched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CT_DevVideoServer* server = NULL;
    // Create the CTestServer derived server
    TRAPD(err,server = CT_DevVideoServer::NewL());
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
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
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

CDataWrapper* CT_DevVideoServer::CT_DevVideoBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper=NULL;

	if( KT_VideoDriver() == aData )
		{
		wrapper=CT_CMMFDevVideoPlay::NewL();
		}
	else if( KT_VideoRecording() == aData )
		{
		wrapper=CT_MMFDevVideoRecordData::NewL();
		}

	return wrapper;
	}
