// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <ssm/ssmswp.h>
 
#include "tintconditionevaluator_server.h"
#include "tconditionevaluator_step_evaluate.h"

_LIT(KCondCmdTestServerName, "CondCmdTestServer");
_LIT(KCondCmdTestWrapper, "CondCmdTestWrapper");

CCondCmdTestServer* CCondCmdTestServer::NewL( )
	{
	CCondCmdTestServer * server = new (ELeave) CCondCmdTestServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

static void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CCondCmdTestServer* server = NULL;
	TRAPD(err, server = CCondCmdTestServer::NewL());
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
		_LIT(KMainErrorStatement, "CmdTestServer::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KCondCmdTestServerName, err);
		}		
	delete cleanup;
	return KErrNone;
	}

CTestBlockController* CCondCmdTestServer::CreateTestBlock()
	{
	CTestBlockController* controller = NULL;
	TRAPD(err, (controller = new (ELeave) CCondCmdTestBlock()));
	if(KErrNone != err)
		{
		User::Panic(KCondCmdTestServerName, err);
		}
	return controller;
	}

CDataWrapper* CCondCmdTestBlock::CreateDataL(const TDesC& aData)
	{
	// Print out the parameters for debugging
	CDataWrapper* wrapper = NULL;
	if (KCondCmdTestWrapper() == aData)
		{
		wrapper = CCondCmdTestEvaluateWrapper::NewL();
		}
	return wrapper;
	}
