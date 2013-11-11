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

#include "te_locmonitorsuiteserver.h"
#include "te_locmonitorstep1.h"
#include "te_locmonitorstep2.h"
#include "te_locmonitorstep3.h"
#include "te_locmonitorstep4.h"
#include "te_locmonitorstep5.h"
#include "te_locmonitorstep6.h"
#include "te_locmonitorstep7.h"
#include "te_locmonitorstep8.h"
#include "te_locmonitorstep9.h"
#include "te_locmonitorstep10.h"
#include "te_locmonitorstep11.h"
#include "te_locmonitorstep12.h"
#include "te_locmonitorstep13.h"
#include "te_locmonitorstep14.h"
#include "te_locmonitorstep15.h"
#include "te_locmonitorstep16.h"
#include "te_locmonitorstep17.h"
#include "te_locmonitorstep18.h"
#include "te_locmonitorstep19.h"
#include "te_locmonitorstep20.h"
#include "te_locmonitorstep21.h"
#include "te_locmonitorstep22.h"
#include "te_locmonitorstep23.h"
#include "te_locmonitorstep24.h"
#include "te_locmonitorstep25.h"
#include "te_locmonitorstep26.h"
#include "te_locmonitorstep27.h"
#include "te_locmonitorstep28.h"
#include "te_locmonitorstep31.h"
#include "te_locmonitorstep32.h"

_LIT(KServerName,"te_locmonitorsuite");

CTe_LocMonitorSuite* CTe_LocMonitorSuite::NewL()
	{
	CTe_LocMonitorSuite * server = new (ELeave) CTe_LocMonitorSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

void CTe_LocMonitorSuite::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);
	}

CTe_LocMonitorSuite::~CTe_LocMonitorSuite()
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
	CTe_LocMonitorSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LocMonitorSuite::NewL());
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
	LBSLOG(ELogP3, "te_locmonitorsuite Start()\n");
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

CTestStep* CTe_LocMonitorSuite::CreateTestStep(const TDesC& aStepName)
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
	else if(aStepName == KLocMonitorStep8)
	            testStep = new CTe_LocMonitorStep8();
	else if(aStepName == KLocMonitorStep9)
	            testStep = new CTe_LocMonitorStep9();
	else if(aStepName == KLocMonitorStep10)
	            testStep = new CTe_LocMonitorStep10();
	else if(aStepName == KLocMonitorStep11)
	            testStep = new CTe_LocMonitorStep11();
	else if(aStepName == KLocMonitorStep12)
	            testStep = new CTe_LocMonitorStep12();
	else if(aStepName == KLocMonitorStep13)
	            testStep = new CTe_LocMonitorStep13();	
	else if(aStepName == KLocMonitorStep14)
	            testStep = new CTe_LocMonitorStep14();
	else if(aStepName == KLocMonitorStep15)
	            testStep = new CTe_LocMonitorStep15();
	else if(aStepName == KLocMonitorStep16)
	            testStep = new CTe_LocMonitorStep16();
	else if(aStepName == KLocMonitorStep17)
	            testStep = new CTe_LocMonitorStep17();
	else if(aStepName == KLocMonitorStep18)
	            testStep = new CTe_LocMonitorStep18();
	else if(aStepName == KLocMonitorStep19)
	            testStep = new CTe_LocMonitorStep19();
	else if(aStepName == KLocMonitorStep20)
	            testStep = new CTe_LocMonitorStep20();
	else if(aStepName == KLocMonitorStep21)
	            testStep = new CTe_LocMonitorStep21();
	else if(aStepName == KLocMonitorStep22)
	            testStep = new CTe_LocMonitorStep22();
	else if(aStepName == KLocMonitorStep23)
	            testStep = new CTe_LocMonitorStep23();
	else if(aStepName == KLocMonitorStep24)
	            testStep = new CTe_LocMonitorStep24();
	else if(aStepName == KLocMonitorStep25)
	            testStep = new CTe_LocMonitorStep25();	
	else if(aStepName == KLocMonitorStep26)
	            testStep = new CTe_LocMonitorStep26();
	else if(aStepName == KLocMonitorStep27)
	            testStep = new CTe_LocMonitorStep27();
	else if(aStepName == KLocMonitorStep28)
	            testStep = new CTe_LocMonitorStep28();
	else if(aStepName == KLocMonitorStep31)
	            testStep = new CTe_LocMonitorStep31();
	else if(aStepName == KLocMonitorStep32)
	            testStep = new CTe_LocMonitorStep32();
	return testStep;
	}
