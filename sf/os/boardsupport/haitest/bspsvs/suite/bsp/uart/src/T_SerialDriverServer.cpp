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

#include "T_SerialDriverServer.h"
#include "T_RBusDevCommData.h"
#include "T_TCommCapsV03Data.h"
#include "T_TCommConfigV02Data.h"

/*@{*/
_LIT(KT_RBusDevComm,		"RBusDevComm");
_LIT(KT_TCommCapsV01,		"TCommCapsV01");
_LIT(KT_TCommCapsV02,		"TCommCapsV02");
_LIT(KT_TCommCapsV03,		"TCommCapsV03");
_LIT(KT_TCommConfigV01,		"TCommConfigV01");
_LIT(KT_TCommConfigV02,		"TCommConfigV02");
/*@}*/

CT_SerialDriverServer* CT_SerialDriverServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
    CT_SerialDriverServer* server = new (ELeave) CT_SerialDriverServer();
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
#if (defined __DATA_CAGING__)
    RProcess().DataCaging(RProcess::EDataCagingOn);
    RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
    CActiveScheduler*	sched=NULL;
    sched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CT_SerialDriverServer* server = NULL;

    // Create the CTestServer derived server
    TRAPD(err,server = CT_SerialDriverServer::NewL());
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

CDataWrapper* CT_SerialDriverServer::CT_SerialDriverBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper=NULL;

	if( aData==KT_RBusDevComm() )
		{
		wrapper=CT_RBusDevCommData::NewL();
		}
	else if ( aData==KT_TCommCapsV01 )
		{
		wrapper=new (ELeave) CT_TCommCapsV01Data();
		}
	else if ( aData==KT_TCommCapsV02 )
		{
		wrapper=new (ELeave) CT_TCommCapsV02Data();
		}
	else if ( aData==KT_TCommCapsV03 )
		{
		wrapper=new (ELeave) CT_TCommCapsV03Data();
		}
	else if ( aData==KT_TCommConfigV01 )
		{
		wrapper=new (ELeave) CT_TCommConfigV01Data();
		}
	else if ( aData==KT_TCommConfigV02 )
		{
		wrapper=new (ELeave) CT_TCommConfigV02Data();
		}

	return wrapper;
	}
