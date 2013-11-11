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

#include "T_BTUserAPIServer.h"

/**
 * @name Constant Literals used.
 */
/*@{*/
/*@}*/
	
CT_BTUserAPIServer* CT_BTUserAPIServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CT_BTUserAPIServer*	server = new (ELeave) CT_BTUserAPIServer();
	CleanupStack::PushL(server);
	server->ConstructL();		 /*Parsing the server name from the file name*/
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
	CActiveScheduler*		sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CT_BTUserAPIServer*	server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CT_BTUserAPIServer::NewL());
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
	{
	TInt rerr = LoadDrivers();
	if(rerr)
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
	
