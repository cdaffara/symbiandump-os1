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
// ctlbsclientportedserver.cpp
// Test server for ported client tests
// 
//

#include "ctlbsclientportedsuiteserver.h"
#include "ctlbsclientpostp1.h"
#include "ctlbsclientpostp2.h"
#include "ctlbsclientpostp11.h"
#include "ctlbsclientpostp17.h"
#include "ctlbsclientpostp21.h"
#include "ctlbsclientpostp22.h"
#include "ctlbsclientpostp25.h"
#include "ctlbsclientpostp27.h"
#include "ctlbsclientpostp76.h"
#include "ctlbsclientpostp93.h"
#include "ctlbsclientpostp169.h"
#include "ctlbsclientpostp173.h"
#include "ctlbsclientpostp176.h"
#include "ctlbsclientpostp178.h"
#include "ctlbsclientpostp179.h"
#include "ctlbsclientpostp188.h"
#include "ctlbsclientpostp189.h"
#include "ctlbsclientpostp193.h"
#include "ctlbsclientpostp194.h"
#include "ctlbsclientpostp195.h"
#include "ctlbsclientpostp201.h"
#include "ctlbsclientpostp203.h"
#include "ctlbsclientpostp204.h"
#include "ctlbsclientpostp208.h"
#include "ctlbsclientpostp213.h"
#include "ctlbsclientpostp217.h"
#include "ctlbsclientpostp218.h"
#include "ctlbsclientpostp222.h"
#include "ctlbsclientpostp223.h"
#include "ctlbsclientpostp224.h"
#include "ctlbsclientpostp227.h"
#include "ctlbsclientpostp254.h"
#include "ctlbsclientpostp256.h"
#include "ctlbsclientpostp257.h"
#include "ctlbsclientpostp259.h"
#include "ctlbsclientpostp261.h"
#include "ctlbsclientpostp269.h"
#include "ctlbsclientpostp271.h"
#include "ctlbsclientpostp272.h"
#include "ctlbsclientpostp274.h"
#include "ctlbsclientpostp275.h"
#include "ctlbsclientpostp4701.h"
#include "ctlbsclientpostp4702.h"
#include "ctlbsclientpostp4703.h"
#include "ctlbsclientpostp4704.h"
#include "ctlbsclientpostp4705.h"
#include "ctlbsclientpostp4706.h"
#include "ctlbsclientpostp4707.h"
#include "ctlbsclientpostp4708.h"
#include "ctlbsclientpostp4709.h"
#include "ctlbsclientpostp4722.h"
#include "ctlbsclientpostp4723.h"
#include "ctlbsclientstepobserveinstall.h"
#include "ctlbsclientsteperroronuninstall.h"
#include "ctlbsclientstepfallbackinstall.h"
#include "ctlbsclientstepmultiintgpsfallback.h"


_LIT(KServerName,"te_LbsPortedSuite");
CT_LbsClientPortedSuite* CT_LbsClientPortedSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CT_LbsClientPortedSuite * server = new (ELeave) CT_LbsClientPortedSuite();
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
	__UHEAP_MARK;
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CT_LbsClientPortedSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CT_LbsClientPortedSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	__UHEAP_MARKEND;
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


CTestStep* CT_LbsClientPortedSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	if(aStepName == KTestProcedure1)
		{
		return new CT_LbsClientPosTp1(*this);
		}
	if(aStepName == KTestProcedure2)
		{
		return new CT_LbsClientPosTp2(*this);
		}
	if(aStepName == KTestProcedure11)
		{
		return new CT_LbsClientPosTp11(*this);
		}
	if(aStepName == KTestProcedure17)
		{
		return new CT_LbsClientPosTp17(*this);
		}
	if(aStepName == KTestProcedure21)
		{
		return new CT_LbsClientPosTp21(*this);
		}
	if(aStepName ==KTestProcedure22)
		{
		return new CT_LbsClientPosTp22(*this);
		}
	if(aStepName == KTestProcedure25)
		{
		return new CT_LbsClientPosTp25(*this);
		}
	if(aStepName == KTestProcedure27)
		{
		return new CT_LbsClientPosTp27(*this);
		}
	if(aStepName == KTestProcedure76)
		{
		return new CT_LbsClientPosTp76(*this);
		}
	if(aStepName == KTestProcedure93)
		{
		return new CT_LbsClientPosTp93(*this);
		}
	if(aStepName == KTestProcedure169)
		{
		return new CT_LbsClientPosTp169(*this);
		}
	if(aStepName == KTestProcedure173)
		{
		return new CT_LbsClientPosTp173(*this);
		}
	if(aStepName == KTestProcedure176)
		{
		return new CT_LbsClientPosTp176(*this);
		}
	if(aStepName == KTestProcedure178)
		{
		return new CT_LbsClientPosTp178(*this);
		}
	if(aStepName == KTestProcedure179)
		{
		return new CT_LbsClientPosTp179(*this);
		}
	if(aStepName == KTestProcedure188)
		{
		return new CT_LbsClientPosTp188(*this);
		}
	if(aStepName == KTestProcedure189)
		{
		return new CT_LbsClientPosTp189(*this);
		}
	if(aStepName == KTestProcedure193)
		{
		return new CT_LbsClientPosTp193(*this);
		}
	if(aStepName == KTestProcedure194)
		{
		return new CT_LbsClientPosTp194(*this);
		}
	if(aStepName == KTestProcedure195)
		{
		return new CT_LbsClientPosTp195(*this);
		}
	if(aStepName == KTestProcedure201)
		{
		return new CT_LbsClientPosTp201(*this);
		}
	if(aStepName == KTestProcedure203)
		{
		return new CT_LbsClientPosTp203(*this);
		}
	if(aStepName == KTestProcedure204)
		{
		return new CT_LbsClientPosTp204(*this);
		}
	if(aStepName == KTestProcedure208)
		{
		return new CT_LbsClientPosTp208(*this);
		}
	if(aStepName == KTestProcedure213)
		{
		return new CT_LbsClientPosTp213(*this);
		}
	if(aStepName == KTestProcedure217)
		{
		return new CT_LbsClientPosTp217(*this);
		}
	if(aStepName == KTestProcedure218)
		{
		return new CT_LbsClientPosTp218(*this);
		}
	if(aStepName == KTestProcedure222)
		{
		return new CT_LbsClientPosTp222(*this);
		}
	if(aStepName == KTestProcedure223)
		{
		return new CT_LbsClientPosTp223(*this);
		}
	if(aStepName == KTestProcedure224)
		{
		return new CT_LbsClientPosTp224(*this);
		}
	if(aStepName == KTestProcedure227)
		{
		return new CT_LbsClientPosTp227(*this);
		}
	if(aStepName == KTestProcedure254)
		{
		return new CT_LbsClientPosTp254(*this);
		}
	if(aStepName == KTestProcedure256)
		{
		return new CT_LbsClientPosTp256(*this);
		}
	if(aStepName == KTestProcedure257)
		{
		return new CT_LbsClientPosTp257(*this);
		}
	if(aStepName == KTestProcedure259)
		{
		return new CT_LbsClientPosTp259(*this);
		}
	if(aStepName == KTestProcedure261)
		{
		return new CT_LbsClientPosTp261(*this);
		}
	if(aStepName == KTestProcedure269)
		{
		return new CT_LbsClientPosTp269(*this);
		}
	if(aStepName == KTestProcedure271)
		{
		return new CT_LbsClientPosTp271(*this);
		}
	if(aStepName == KTestProcedure272)
		{
		return new CT_LbsClientPosTp272(*this);
		}
	if(aStepName == KTestProcedure274)
		{
		return new CT_LbsClientPosTp274(*this);
		}
	if(aStepName == KTestProcedure275)
		{
		return new CT_LbsClientPosTp275(*this);
		}
	if(aStepName == KTestProcedure4701)
		{
		return new CT_LbsClientPosTp4701(*this);
		}
	if(aStepName == KTestProcedure4702)
		{
		return new CT_LbsClientPosTp4702(*this);
		}
	if(aStepName == KTestProcedure4703)
		{
		return new CT_LbsClientPosTp4703(*this);
		}
	if(aStepName == KTestProcedure4704)
		{
		return new CT_LbsClientPosTp4704(*this);
		}
	if(aStepName == KTestProcedure4705)
		{
		return new CT_LbsClientPosTp4705(*this);
		}
	if(aStepName == KTestProcedure4706)
		{
		return new CT_LbsClientPosTp4706(*this);
		}
	if(aStepName == KTestProcedure4707)
		{
		return new CT_LbsClientPosTp4707(*this);
		}
	if(aStepName == KTestProcedure4708)
		{
		return new CT_LbsClientPosTp4708(*this);
		}
	if(aStepName == KTestProcedure4709)
		{
		return new CT_LbsClientPosTp4709(*this);
		}
	if(aStepName == KTestProcedure4722)
		{
		return new CT_LbsClientPosTp4722(*this);
		}
	if(aStepName == KTestProcedure4723)
		{
		return new CT_LbsClientPosTp4723(*this);
		}
 	if(aStepName == KTestProcedure300)
 		{
 		return new CT_LbsClientStepObserveInstall(*this);
 		}	
 	if(aStepName == KTestProcedure301)
 		{
 		return new CT_LbsClientStepErrorOnUninstall(*this);
 		}	
 	if(aStepName == KTestProcedure302)
 		{
 		return new CT_LbsClientStepFallbackInstall(*this);
 		}	
 	if(aStepName == KTestProcedure304)
 		{
 		return new CT_LbsClientStepMultiIntGpsFallback(*this);
 		}	

	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);

	}
