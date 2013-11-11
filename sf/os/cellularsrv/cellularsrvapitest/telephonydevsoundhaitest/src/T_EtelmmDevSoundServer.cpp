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

#include "T_EtelmmDevSoundServer.h"
#include "T_RMmCustomAPIData.h"
#include "T_RMobileLineData.h"
#include "T_RMobilePhoneData.h"
#include "T_RTelServerData.h"
#include "T_RMobileCallData.h"
#include "T_RMobileConferenceCallData.h"
#include "T_CMMFDevSoundData.h"
#include "T_CRepositoryData.h"
#include "T_CAudioInputData.h"
#include "T_CAudioOutputData.h"
#include "T_CRestrictedAudioOutputData.h"
#include "T_CTelephonyAudioRoutingData.h"

/*@{*/
_LIT(KT_MmCustomApi,	"RMmCustomAPI");
_LIT(KT_TelServer,		"RTelServer");
_LIT(KT_MobileLine,		"RMobileLine");
_LIT(KT_MobilePhone, 	"RMobilePhone");
_LIT(KT_MobileCall,		"RMobileCall");
_LIT(KT_ConferenceCall,	"RMobileConferenceCall");
_LIT(KT_CMMFDevSound, 	"CMMFDevSound");
_LIT(KT_CRepository,	"CRepository");
_LIT(KT_CRestrictedAudioOutput, "CRestrictedAudioOutput");
_LIT(KT_CTelephonyAudioRouting, "CTelephonyAudioRouting");
_LIT(KT_CAudioInput, 	"CAudioInput");
_LIT(KT_CAudioOutput,	"CAudioOutput");


/*@}*/

/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
CT_EtelmmDevSoundServer* CT_EtelmmDevSoundServer::NewL()
	{
    CT_EtelmmDevSoundServer* server = new (ELeave) CT_EtelmmDevSoundServer();
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
    CT_EtelmmDevSoundServer* server = NULL;

    // Create the CTestServer derived server
    TRAPD(err,server = CT_EtelmmDevSoundServer::NewL());
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

CDataWrapper* CT_EtelmmDevSoundServer::CT_EtelmmDevSoundBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper=NULL;

	if(KT_MmCustomApi() == aData)
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
	else if(KT_ConferenceCall() == aData)
		{
		wrapper=CT_RMobileConferenceCallData::NewL();
		}
	else if (KT_CMMFDevSound() == aData)
		{
		wrapper = CT_CMMFDevSoundData::NewL();
		}
	else if (KT_CRepository() == aData)
		{
		wrapper = CT_CRepositoryData::NewL();
		}	
	else if (KT_CRestrictedAudioOutput() == aData)
		{
		wrapper = CT_CRestrictedAudioOutputData::NewL();
		}
	else if (KT_CTelephonyAudioRouting() == aData)
		{
		wrapper = CT_CTelephonyAudioRoutingData::NewL();
		}
	else if(KT_CAudioInput() == aData)
		{
		wrapper = CT_CAudioInputData::NewL();
		}
	else if(KT_CAudioOutput() == aData)
		{
		wrapper = CT_CAudioOutputData::NewL();
		}

	return wrapper;
	}
