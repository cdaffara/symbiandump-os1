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

#include "tconditionevaluatorserver.h"
#include "tconditionevaluatorstep.h"

_LIT(KTestConditionEvaluatorServerName, "CondCmdUnitTestServer");
_LIT(KTCConditionEvaluatorTestWrapper, "CondCmdUnitTestWrapper");
_LIT(KTCConditionEvaluatorTestWrapperUsingNewLC, "CondCmdUnitTestWrapperUsingNewLC");

CTestConditionEvaluatorServer* CTestConditionEvaluatorServer::NewL( )
	{
	CTestConditionEvaluatorServer * server = new (ELeave) CTestConditionEvaluatorServer();
	CleanupStack::PushL(server);
	TParsePtrC serverName(RProcess().FileName());
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

static void MainL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
  	CleanupStack::PushL (sched);
  	CActiveScheduler::Install (sched);
	
	CTestConditionEvaluatorServer* server = CTestConditionEvaluatorServer::NewL();
	CleanupStack::PushL(server);
	RProcess::Rendezvous(KErrNone);
	sched->Start();
	
	CleanupStack::PopAndDestroy (server );
  	CleanupStack::PopAndDestroy (sched );
	}

/**
 Process entry point. Called by client using RProcess API
 @return - Standard Epoc error code on process exit
 */
TInt E32Main(void)
	{
	//__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err,MainL());
	if (err)
		{
		RDebug::Print(_L("CTestConditionEvaluator::MainL - Error: %d"), err);
		User::Panic(KTestConditionEvaluatorServerName, err);
		}
	delete cleanup;

	//__UHEAP_MARKEND;
	return KErrNone;
	}
	
CTestConditionEvaluatorBlock* CTestConditionEvaluatorServer::CreateTestBlock()
	{
	CTestConditionEvaluatorBlock* controller = NULL;
	TRAPD(err, (controller= new (ELeave) CTestConditionEvaluatorBlock()));
	if(KErrNone != err)
		{
		User::Panic(KTestConditionEvaluatorServerName, err);
		}
	return controller;
	}

/**
 @return - A Test DataWrapper instance
 */
 
CDataWrapper* CTestConditionEvaluatorBlock::CreateDataL(const TDesC& aData)
	{
	RDebug::Print(_L("CTestConditionEvaluatorBlock::CreateDataL"));
	CDataWrapper* wrapper = NULL;
	if (KTCConditionEvaluatorTestWrapper() == aData)
		{
		wrapper = CConditionEvaluatorTestWrapper::NewL();
		}
	else if (KTCConditionEvaluatorTestWrapperUsingNewLC() == aData)
		{
		wrapper = CConditionEvaluatorTestWrapper::NewLC();
		CleanupStack::Pop(wrapper);
		}		
	return wrapper;
	}	

