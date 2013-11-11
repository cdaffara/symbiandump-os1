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

#include "T_BTSdpAPIServer.h"
#include <rsshared.h>

CT_BTSdpAPIServer* CT_BTSdpAPIServer::NewL()
	{
	CT_BTSdpAPIServer* server = new (ELeave) CT_BTSdpAPIServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

TInt LoadDrivers()
	{
#ifdef __WINS__
	#define KPDDName _L("ECDRV")
	#define KLDDName _L("ECOMM")
#else
	#define KPDDName _L("EUART1")
	#define KLDDName _L("ECOMM")
#endif
	TInt rerr = KErrNone;
	
	rerr = StartC32();
	if( rerr!=KErrNone && rerr!=KErrAlreadyExists )
		{
		return rerr;	
		}
	
	rerr = User::LoadPhysicalDevice(KPDDName);
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
	rerr = User::LoadLogicalDevice(KLDDName);	
	if(rerr != KErrNone && rerr != KErrAlreadyExists)
		{
		return rerr;
		}
	return KErrNone;	
	}

LOCAL_C void MainL()
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CT_BTSdpAPIServer* server = NULL;
	TRAPD(err, server = CT_BTSdpAPIServer::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
	{
	TInt rerr = LoadDrivers();
	if( rerr != KErrNone )
		{
		return rerr;
		}		
		
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }


