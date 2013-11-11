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
// file te_sysutilburtestserver.cpp
// 
//

#include "te_sysutilburtestserver.h"
#include "te_sysutiltestdevicetypeinfo.h"
#include "te_sysutiltestversioninfo.h"

CSysUtilBurTestServer* server = NULL;

CSysUtilBurTestServer::~CSysUtilBurTestServer()
	{
	RDebug::Printf("========== DESTRUCTOR ===========");
	
	iBackupSessionWrapper->DeRegisterBackupOperationObserver(*iBackupObserver);
	delete iBackupObserver;
	delete iBackupSessionWrapper;
	}

CSysUtilBurTestServer* CSysUtilBurTestServer::NewL()
	{
	CSysUtilBurTestServer* server = new(ELeave) CSysUtilBurTestServer();
	CleanupStack::PushL(server);
	
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());
	
	server->ConstructL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

void CSysUtilBurTestServer::ConstructL(const TDesC& aName)
	{
	RDebug::Printf("========== CONSTRUCTL ===========");
	
	CTestServer::ConstructL(aName);
	
	// Create an instance of the backup/restore observer class and register it so it gets 
	// notified of backup/restore operations.
	iBackupSessionWrapper = CBaBackupSessionWrapper::NewL();
	iBackupObserver = new (ELeave) CBackupObserver();
	
	iBackupSessionWrapper->RegisterBackupOperationObserverL(*iBackupObserver);
	}

void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	// Create the CTestServer derived server
	TRAPD(err, server = CSysUtilBurTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}

	delete server;
	delete sched;
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	
	TRAPD(err,MainL());
	
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}

CTestStep* CSysUtilBurTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KSysUtilTestDeviceTypeInfoPreBackupRestoreStep)
		{
		testStep = new CSysUtilTestDeviceTypeInfoStep(aStepName, *this);
		}
	else if(aStepName == KSysUtilTestDeviceTypeInfoPostBackupRestoreStep)
		{
		testStep = new CSysUtilTestDeviceTypeInfoStep(aStepName, *this);
		}
	else if(aStepName == KSysUtilTestVersionInfoPreBackupRestoreStep)
		{
		testStep = new CSysUtilTestVersionInfoStep(aStepName, *this);
		}
	else if(aStepName == KSysUtilTestVersionInfoPostBackupRestoreStep)
		{
		testStep = new CSysUtilTestVersionInfoStep(aStepName, *this);
		}
	
	return testStep;
	}

TTestStep CSysUtilBurTestServer::GetTestStep()
	{
	return iTestStep;
	}

TInt CSysUtilBurTestServer::GetTestStepError()
	{
	return iTestStepError;
	}

void CSysUtilBurTestServer::SetTestStep(TTestStep aTestStep)
	{
	// Debugging information
	if(aTestStep == EDeviceTypeInfoAPIsStep)
		{
		RDebug::Printf("<test step> EDeviceTypeInfoAPIsStep");
		}
	else if(aTestStep == EVersionAPIsStep)
		{
		RDebug::Printf("<test step> EVersionAPIsStep");
		}
	else if(aTestStep == ENoStep)
		{
		RDebug::Printf("<test step> ENoStep");
		}
	
	iTestStep = aTestStep;
	}

void CSysUtilBurTestServer::SetTestStepError(TInt aTestStepError)
	{
	iTestStepError = aTestStepError;
	}


// *************** CDeviceTypeInfoBackupObserver Functions ***************

void CBackupObserver::HandleBackupOperationEventL(const TBackupOperationAttributes &aBackupOperationAttributes)
	{
	if(aBackupOperationAttributes.iOperation == MBackupOperationObserver::EStart)
		{
		RDebug::Printf("================ STARTING BACKUP/RESTORE ================");
		
		TTestStep testStep = server->GetTestStep();
		if(testStep == EDeviceTypeInfoAPIsStep)
			{
			RDebug::Printf("<Step> EDeviceTypeInfoAPIsStep");
			
			TRAPD(err, CSysUtilTestDeviceTypeInfoStep::TestAllAPIsL());
			server->SetTestStepError(err);
			}
		else if(testStep == EVersionAPIsStep)
			{
			RDebug::Printf("<Step> EVersionAPIsStep");
			
			TRAPD(err, CSysUtilTestVersionInfoStep::TestAllAPIsL());
			server->SetTestStepError(err);
			}
		else if(testStep == ENoStep)
			{
			RDebug::Printf("<Step> ENoStep");
			}
		else
			{
			RDebug::Printf("<Step> Invalid test step value");
			}
		}
	else if(aBackupOperationAttributes.iOperation == MBackupOperationObserver::EEnd)
		{
		RDebug::Printf("================ ENDING BACKUP/RESTORE ================");
		}
	else if(aBackupOperationAttributes.iOperation == MBackupOperationObserver::ENone)
		{
		RDebug::Printf("================ NO BACKUP/RESTORE ================");
		}
	else if(aBackupOperationAttributes.iOperation == MBackupOperationObserver::EAbort)
		{
		RDebug::Printf("================ ABORT BACKUP/RESTORE ================");
		}
	else
		{
		RDebug::Printf("================ ERROR ================");
		}
	}
