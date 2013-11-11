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

#include "te_agpsmanagersuiteserver.h"
#include "te_agpsmanagerstep1.h"
#include "te_agpsmanagerstep2.h"
#include "te_agpsmanagerstep3.h"
#include "te_agpsmanagerstep4.h"
#include "te_agpsmanagerstep5.h"
#include "te_agpsmanagerstep6.h"
#include "te_agpsmanagerstep7.h"
#include "te_agpsmanagerstep8.h"
#include "te_agpsmanagerstep9.h"
#include "te_agpsmanagerstep10.h"
#include "te_agpsmanagerstep11.h"
#include "te_agpsmanagerstep12.h"
#include "te_agpsmanagerstep13.h"
#include "te_agpsmanagerstep14.h"
#include "te_agpsmanagerstep15.h"
#include "te_agpsmanagerstep16.h"
#include "te_agpsmanagerstep17.h"
#include "te_agpsmanagerstep18.h"
#include "te_agpsmanagerstep19.h"
#include "te_agpsmanagerstep20.h"
#include "te_agpsmanagerstep21.h"
#include "te_agpsmanagerstep22.h"
#include "te_agpsmanagerstep23.h"
#include "te_agpsmanagerstep24.h"
#include "te_agpsmanagerstep25.h"
#include "te_agpsmanagerstep26.h"
#include "te_agpsmanagerstep27.h"
#include "te_agpsmanagerstep28.h"
#include "te_agpsmanagerstep29.h"
#include "te_agpsmanagerstep30.h"
#include "te_agpsmanagerstep31.h"
#include "te_agpsmanagerstep32.h"
#include "te_agpsmanagerstep33.h"

_LIT(KServerName,"te_agpsmanagersuite");

CTe_AgpsManagerSuite* CTe_AgpsManagerSuite::NewL()
	{
	CTe_AgpsManagerSuite * server = new (ELeave) CTe_AgpsManagerSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

void CTe_AgpsManagerSuite::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);
	}

CTe_AgpsManagerSuite::~CTe_AgpsManagerSuite()
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
	CTe_AgpsManagerSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_AgpsManagerSuite::NewL());
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


CTestStep* CTe_AgpsManagerSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KAgpsManagerStep1)
	            testStep = new CTe_AgpsManagerStep1();
	else if(aStepName == KAgpsManagerStep2)
	            testStep = new CTe_AgpsManagerStep2();
	else if(aStepName == KAgpsManagerStep3)
	            testStep = new CTe_AgpsManagerStep3();
	else if(aStepName == KAgpsManagerStep4)
	            testStep = new CTe_AgpsManagerStep4();
	else if(aStepName == KAgpsManagerStep5)
	            testStep = new CTe_AgpsManagerStep5();
	else if(aStepName == KAgpsManagerStep6)
	            testStep = new CTe_AgpsManagerStep6();
	else if(aStepName == KAgpsManagerStep7)
	            testStep = new CTe_AgpsManagerStep7();
	else if(aStepName == KAgpsManagerStep8)
	            testStep = new CTe_AgpsManagerStep8();
	else if(aStepName == KAgpsManagerStep9)
	            testStep = new CTe_AgpsManagerStep9();
	else if(aStepName == KAgpsManagerStep10)
	            testStep = new CTe_AgpsManagerStep10();
	else if(aStepName == KAgpsManagerStep11)
	            testStep = new CTe_AgpsManagerStep11();
	else if(aStepName == KAgpsManagerStep12)
	            testStep = new CTe_AgpsManagerStep12();
	else if(aStepName == KAgpsManagerStep13)
	            testStep = new CTe_AgpsManagerStep13();	
	else if(aStepName == KAgpsManagerStep14)
	            testStep = new CTe_AgpsManagerStep14();
	else if(aStepName == KAgpsManagerStep15)
	            testStep = new CTe_AgpsManagerStep15();
	else if(aStepName == KAgpsManagerStep16)
	            testStep = new CTe_AgpsManagerStep16();
	else if(aStepName == KAgpsManagerStep17)
	            testStep = new CTe_AgpsManagerStep17();
	else if(aStepName == KAgpsManagerStep18)
	            testStep = new CTe_AgpsManagerStep18();
	else if(aStepName == KAgpsManagerStep19)
	            testStep = new CTe_AgpsManagerStep19();
	else if(aStepName == KAgpsManagerStep20)
	            testStep = new CTe_AgpsManagerStep20();
	else if(aStepName == KAgpsManagerStep21)
	            testStep = new CTe_AgpsManagerStep21();
	else if(aStepName == KAgpsManagerStep22)
	            testStep = new CTe_AgpsManagerStep22();
	else if(aStepName == KAgpsManagerStep23)
	            testStep = new CTe_AgpsManagerStep23();
	else if(aStepName == KAgpsManagerStep24)
	            testStep = new CTe_AgpsManagerStep24();
	else if(aStepName == KAgpsManagerStep25)
	            testStep = new CTe_AgpsManagerStep25();	
	else if(aStepName == KAgpsManagerStep26)
	            testStep = new CTe_AgpsManagerStep26();
	else if(aStepName == KAgpsManagerStep27)
	            testStep = new CTe_AgpsManagerStep27();
	else if(aStepName == KAgpsManagerStep28)
	            testStep = new CTe_AgpsManagerStep28();
	else if(aStepName == KAgpsManagerStep29)
	            testStep = new CTe_AgpsManagerStep29();
	else if(aStepName == KAgpsManagerStep30)
	            testStep = new CTe_AgpsManagerStep30();
	else if(aStepName == KAgpsManagerStep31)
	            testStep = new CTe_AgpsManagerStep31();
	else if(aStepName == KAgpsManagerStep32)
		        testStep = new CTe_AgpsManagerStep32();
	else if(aStepName == KAgpsManagerStep33)
			    testStep = new CTe_AgpsManagerStep33();	

	return testStep;
	}
