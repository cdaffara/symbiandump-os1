// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description: Test Server for handling graceful notification incase of state change
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


 
#include "tcmn_gracefulnotify_server.h"
#include "t_stateawaresession.h"
#include "t_ssmstatemanager.h"

_LIT(KGracefulOfflineNotifyServer,   "GracefulOfflineNotifyServer");
_LIT(KStateAwareSession,             "RSsmStateAwareSession");
_LIT(KStateManager,                  "RSsmStateManager");

CAwareSessionTestServer* CAwareSessionTestServer::NewL( )
	{
	CAwareSessionTestServer* server = new (ELeave)CAwareSessionTestServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

static void MainL()
	{
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;	
	CActiveScheduler::Install(sched);
	CAwareSessionTestServer* server = NULL;
	TRAPD(err, server = CAwareSessionTestServer::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
 Process entry point. Called by client using RProcess API
 @return - Standard Epoc error code on process exit
 */
TInt E32Main(void)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());	
	if (err)
		{
		_LIT(KMainErrorStatement, "KGracefulOfflineNotifyServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KGracefulOfflineNotifyServer, err);
		}		
	delete cleanup;
	return KErrNone;
	}

CTestBlockController* CAwareSessionTestServer::CreateTestBlock()
	{
	CTestBlockController* controller = NULL;
	TRAPD(err, (controller = new (ELeave) CAwareSessionTestBlock()));
	if(KErrNone != err)
		{
		User::Panic(KGracefulOfflineNotifyServer, err);
		}
	return controller;
	}

CDataWrapper* CAwareSessionTestBlock::CreateDataL(const TDesC& aData)
	{
	// Print out the parameters for debugging
	CDataWrapper* wrapper = NULL;
	if ( KStateAwareSession() == aData )
		{
		wrapper = CTestRStateAwareSession::NewL();
		}
	else if ( KStateManager() == aData )
	    {
	    wrapper = CTestRSsmStateManager::NewL();
	    }
	return wrapper;
	}
