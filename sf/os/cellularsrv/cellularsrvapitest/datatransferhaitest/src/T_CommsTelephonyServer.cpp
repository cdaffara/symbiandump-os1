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

#include "T_CommsTelephonyServer.h"
#include "T_RSocketData.h"
#include "T_RSocketServData.h"
#include "T_RConnectionData.h"
#include "T_RHostResolverData.h"
#include "T_DataVerify.h"

#include "T_RMmCustomAPIData.h"
#include "T_RMobileLineData.h"
#include "T_RMobilePhoneData.h"
#include "T_RTelServerData.h"
#include "T_RMobileCallData.h"



/*@{*/
_LIT(KT_RSocket,		"RSocket");
_LIT(KT_RConnection,	"RConnection");
_LIT(KT_RHostResolver,	"RHostResolver");
_LIT(KT_RSocketServ, 	"RSocketServ");
_LIT(KT_DataVerify, 	"DataVerify");

_LIT(KT_MmCustomApi,	"RMmCustomAPI");
_LIT(KT_TelServer,		"RTelServer");
_LIT(KT_MobileLine,		"RMobileLine");
_LIT(KT_MobilePhone, 	"RMobilePhone");
_LIT(KT_MobileCall,		"RMobileCall");

/*@}*/

/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
CT_CommsTelephonyServer* CT_CommsTelephonyServer::NewL()
	{
    CT_CommsTelephonyServer* server = new (ELeave) CT_CommsTelephonyServer();
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
    CT_CommsTelephonyServer* server = NULL;

    // Create the CTestServer derived server
    TRAPD(err,server = CT_CommsTelephonyServer::NewL());
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

CDataWrapper* CT_CommsTelephonyServer::CT_CommsTelephonyBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper=NULL;

	if( KT_RSocket() == aData )
		{
		wrapper=CT_RSocketData::NewL();
		}
	else if( KT_RConnection() == aData )
		{
		wrapper=CT_RConnectionData::NewL();
		}
	else if( KT_RSocketServ() == aData )
		{
		wrapper=CT_RSocketServData::NewL();
		}
	else if( KT_RHostResolver() == aData )
		{
		wrapper=CT_RHostResolverData::NewL();
		}
	else if( KT_DataVerify() == aData )
		{
		wrapper=CT_DataVerify::NewL();
		}
	else if(KT_MmCustomApi() == aData)
		{
		wrapper=CT_RMmCustomApiData::NewL();
		}
	else if(KT_TelServer() == aData)
		{
		wrapper=CT_RTelServerData::NewL();
		}
	else if(KT_MobileLine() == aData)
		{
		wrapper=CT_RMobileLineData::NewL();
		}
	else if(KT_MobilePhone() == aData)
		{
		wrapper=CT_RMobilePhoneData::NewL();
		}
	else if(KT_MobileCall() == aData)
		{
		wrapper=CT_RMobileCallData::NewL();
		}

	return wrapper;
	}
