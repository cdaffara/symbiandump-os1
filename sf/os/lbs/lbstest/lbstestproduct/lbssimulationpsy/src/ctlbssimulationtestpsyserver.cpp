/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ctlbssimulationtestpsyserver.h"
#include "ctlbssimulationpsypostp115.h"
#include "ctlbssimulationpsypostp221.h"
#include "ctlbssimulationpsypostp242.h"
#include "ctlbssimulationpsypostp254.h"
#include "ctlbssimulationpsypostp256.h"
#include "ctlbssimulationpsypostp262.h"
#include "ctlbssimulationpsypostp300.h"
#include "ctlbssimulationpsypostp400.h"
#include "ctlbssimulationpsypostp500.h"
#include "ctlbssimulationpsypostp600.h"

_LIT(KServerName,"simulationpsytests");
CT_LbsSimulationTestPSYServer* CT_LbsSimulationTestPSYServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CT_LbsSimulationTestPSYServer * server = new (ELeave) CT_LbsSimulationTestPSYServer();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CT_LbsSimulationTestPSYServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CT_LbsSimulationTestPSYServer::NewL());
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
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CT_LbsSimulationTestPSYServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{

	if(aStepName == KTestProcedure115_0)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 0);
		}
	if(aStepName == KTestProcedure115_1)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 1);
		}
	if(aStepName == KTestProcedure115_2)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 2);
		}
	if(aStepName == KTestProcedure115_3)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 3);
		}
	if(aStepName == KTestProcedure115_4)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 4);
		}
	if(aStepName == KTestProcedure115_5)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 5);
		}
	if(aStepName == KTestProcedure115_6)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 6);
		}
	if(aStepName == KTestProcedure115_7)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 7);
		}
	if(aStepName == KTestProcedure115_8)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 8);
		}
	if(aStepName == KTestProcedure115_9)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 9);
		}
	if(aStepName == KTestProcedure115_10)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 10);
		}
	if(aStepName == KTestProcedure115_11)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 11);
		}
	if(aStepName == KTestProcedure115_12)
		{
		return new CT_LbsSimulationPSYPosTp115(*this, 12);
		}
	if(aStepName == KTestProcedure221)
		{
		return new CT_LbsSimulationPSYPosTp221(*this);
		}
	if(aStepName == KTestProcedure242)
		{
		return new CT_LbsSimulationPSYPosTp242(*this);
		}
	if(aStepName == KTestProcedure254)
		{
		return new CT_LbsSimulationPSYPosTp254(*this);
		}
	if(aStepName == KTestProcedure256)
		{
		return new CT_LbsSimulationPSYPosTp256(*this);
		}
	if(aStepName == KTestProcedure262)
		{
		return new CT_LbsSimulationPSYPosTp262(*this);
		}
	if(aStepName == KTestProcedure300_0)
		{
		return new CT_LbsSimulationPsyPosTp300(*this, 0);
		}
	if (aStepName == KTestProcedure300_1)
		{
		return new CT_LbsSimulationPsyPosTp300(*this, 1);
		}
	if (aStepName == KTestProcedure300_2)
		{
		return new CT_LbsSimulationPsyPosTp300(*this, 2);
		}
	if (aStepName == KTestProcedure300_3)
		{
		return new CT_LbsSimulationPsyPosTp300(*this, 3);
		}
	if (aStepName == KTestProcedure300_4)
		{
		return new CT_LbsSimulationPsyPosTp300(*this, 4);
		}
	if (aStepName == KTestProcedure300_5)
		{
		return new CT_LbsSimulationPsyPosTp300(*this, 5);
		}
	if (aStepName == KTestProcedure300_6)
		{
		return new CT_LbsSimulationPsyPosTp300(*this, 6);
		}
	if (aStepName == KTestProcedure300_7)
		{
		return new CT_LbsSimulationPsyPosTp300(*this, 7);
		}
	if (aStepName == KTestProcedure300_8)
		{
		return new CT_LbsSimulationPsyPosTp300(*this, 8);
		}
	if (aStepName == KTestProcedure400_0)
		{
		return new CT_LbsSimulationPsyPosTp400(*this, 0);
		}
	if (aStepName == KTestProcedure400_1)
		{
		return new CT_LbsSimulationPsyPosTp400(*this, 1);
		}	            
	if (aStepName == KTestProcedure400_2)
		{
		return new CT_LbsSimulationPsyPosTp400(*this, 2);
		}	
	if (aStepName == KTestProcedure400_3)
		{
		return new CT_LbsSimulationPsyPosTp400(*this, 3);
		}	
	if (aStepName == KTestProcedure400_4)
		{
		return new CT_LbsSimulationPsyPosTp400(*this, 4);
		}	
	if (aStepName == KTestProcedure400_5)
		{
		return new CT_LbsSimulationPsyPosTp400(*this, 5);
		}
	if (aStepName == KTestProcedure500_0)
		{
		return new CT_LbsSimulationPsyPosTp500(*this, 0);
		}
	if (aStepName == KTestProcedure500_1)
		{	
		return new CT_LbsSimulationPsyPosTp500(*this, 1);
		}
	if (aStepName == KTestProcedure500_2)
		{
		return new CT_LbsSimulationPsyPosTp500(*this, 2);
		}
	if (aStepName == KTestProcedure500_3)
		{
		return new CT_LbsSimulationPsyPosTp500(*this, 3);
		}
	if (aStepName == KTestProcedure500_4)
		{
		return new CT_LbsSimulationPsyPosTp500(*this, 4);
		}
	if (aStepName == KTestProcedure500_5)
		{
		return new CT_LbsSimulationPsyPosTp500(*this, 5);
		}
	if (aStepName == KTestProcedure500_6)
		{
		return new CT_LbsSimulationPsyPosTp500(*this, 6);
		}
	if (aStepName == KTestProcedure500_7)
		{
		return new CT_LbsSimulationPsyPosTp500(*this, 7);
		}
	if (aStepName == KTestProcedure600_0)
		{
		return new CT_LbsSimulationPsyPosTp600(*this, 0);
		}
	if (aStepName == KTestProcedure600_1)
		{
		return new CT_LbsSimulationPsyPosTp600(*this, 1);
		}
	if (aStepName == KTestProcedure600_2)
		{
		return new CT_LbsSimulationPsyPosTp600(*this, 2);
		}
	if (aStepName == KTestProcedure600_3)
		{
		return new CT_LbsSimulationPsyPosTp600(*this, 3);
		}
              
	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}
