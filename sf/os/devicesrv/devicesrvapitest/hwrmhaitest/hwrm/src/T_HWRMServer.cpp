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

#include "t_hwrmserver.h"
#include "t_chwrmlightsdata.h"
#include "t_chwrmvibradata.h"
#include "t_crepositorydata.h"
#include "t_rpropertydata.h"


_LIT(KT_Lights, "CHWRMLight");
_LIT(KT_Vibra, 	"CHWRMVibra");
_LIT(KT_Repository, "CRepository");
_LIT(KT_Property, "RProperty");

/**
 * 
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 * @return - Instance of the test server
 */
CT_HWRMServer* CT_HWRMServer::NewL()
	{
    CT_HWRMServer* server = new (ELeave) CT_HWRMServer();
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
    CActiveScheduler* sched = NULL;
    sched = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CT_HWRMServer* server = NULL;

    // Create the CTestServer derived server
    TRAPD(err, server = CT_HWRMServer::NewL());
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
 * 
 */
CDataWrapper* CT_HWRMServer::CT_HWRMBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper* wrapper = NULL;

	if( KT_Lights() == aData )
		{
		wrapper = CT_CHWRMLightsData::NewL();
		}
	else if ( KT_Vibra() == aData )
		{
		wrapper = CT_CHWRMVibraData::NewL();
		}
	else if( KT_Repository() == aData )
		{
		wrapper = CT_CRepositoryData::NewL();
		}
	else if ( KT_Property() == aData )
		{
		wrapper = CT_RPropertyData::NewL();
		}

	return wrapper;
	}
