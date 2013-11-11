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

#include "T_DigitiserDriverServer.h"
#include "T_DigitiserDriverData.h"
#include "T_RConsoleData.h"


/*@{*/
_LIT(KT_DigitserDriverData,			"HalData");
_LIT(KT_RConsoleDigitiser,			"RConsole");

/*@}*/

CT_DigitiserDriverServer* CT_DigitiserDriverServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
    CT_DigitiserDriverServer* server = new (ELeave) CT_DigitiserDriverServer();
    CleanupStack::PushL(server);
    server->ConstructL();
    CleanupStack::Pop(server);
    return server;
    }



LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
    RProcess().DataCaging(RProcess::EDataCagingOn);
    RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
    CActiveScheduler*	sched=NULL;
    sched=new(ELeave) CActiveScheduler;
    //Install me as current ActiveScheduler
    CActiveScheduler::Install(sched);
    CT_DigitiserDriverServer* server = NULL;
	// Create CT_DigitiserDriverServer(the CTestServer derived server) 
    TRAPD(err,server = CT_DigitiserDriverServer::NewL());
    if(!err)
	    {
        // Sync with the client and enter the active scheduler
        RProcess::Rendezvous(KErrNone);
        sched->Start();
        }

    delete server;
    delete sched;
    }


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
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
    

CDataWrapper* CT_DigitiserDriverServer::CT_DigitiserDriverBlock::CreateDataL(const TDesC& aData)
/** 
* @return - A CDataWrapper derived instance
*/
	{
	CDataWrapper*	wrapper=NULL;

	if( aData==KT_DigitserDriverData() )
		{
		wrapper=CT_DigitiserDriverData::NewL();
		}
	else if ( aData==KT_RConsoleDigitiser() )
		{
		wrapper=CT_RConsoleData::NewL();
		}

	return wrapper;
	}
