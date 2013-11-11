/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "lbsdevloggermacros.h"

#include "te_geolocmonitorsuiteserver.h"
#include "te_geolocmonitorstep1.h"
#include "te_geolocmonitorstep2.h"
#include "te_geolocmonitorstep3.h"
#include "te_geolocmonitorstep4.h"
#include "te_geolocmonitorstep5.h"
#include "te_geolocmonitorstep6.h"
#include "te_geolocmonitorstep7.h"


_LIT(KServerName,"te_geolocmonitorsuite");

CTe_GeoLocMonitorSuite* CTe_GeoLocMonitorSuite::NewL()
	{
	CTe_GeoLocMonitorSuite * server = new (ELeave) CTe_GeoLocMonitorSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

void CTe_GeoLocMonitorSuite::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);
	}

CTe_GeoLocMonitorSuite::~CTe_GeoLocMonitorSuite()
	{
	}

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_GeoLocMonitorSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_GeoLocMonitorSuite::NewL());
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
	LBSLOG_BEGIN();
	LBSLOG(ELogP3, "te_geolocmonitorsuite Start()\n");
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

CTestStep* CTe_GeoLocMonitorSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KLocMonitorStep1)
	            testStep = new CTe_LocMonitorStep1();
	else if(aStepName == KLocMonitorStep2)
	            testStep = new CTe_LocMonitorStep2();
	else if(aStepName == KLocMonitorStep3)
	            testStep = new CTe_LocMonitorStep3();
	else if(aStepName == KLocMonitorStep4)
	            testStep = new CTe_LocMonitorStep4();
	else if(aStepName == KLocMonitorStep5)
	            testStep = new CTe_LocMonitorStep5();
	else if(aStepName == KLocMonitorStep6)
	            testStep = new CTe_LocMonitorStep6();
	else if(aStepName == KLocMonitorStep7)
	            testStep = new CTe_LocMonitorStep7();


	return testStep;
	}
