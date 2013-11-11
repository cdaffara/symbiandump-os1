// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file te_testprotocolmodulesuiteserver.cpp
*/

#include "te_testprotocolmodulesuiteserver.h"
#include "cpluginstep1.h"
#include "cpluginstep2.h"
#include "cmolrstep1.h"
#include "cmolrstep2.h"
#include "cmolrstep3.h"
#include "cmolrstep4.h"
#include "cmolrstep5.h"
#include "cmolrstep6.h"
#include "cmolrstep7.h"
#include "cmolrstep8.h"
#include "cmolrstep9.h"
#include "cmolrstep10.h"
#include "cmolrstep11.h"
#include "cmolrstep12.h"
#include "cmolrstep13.h"
#include "cmolrstep14.h"
#include "cmolrstep15.h"
#include "cmolrstep16.h"
#include "cmolrstep17.h"
#include "ctrackstep1.h"
#include "ctrackstep2.h"
#include "ctrackstep3.h"
#include "cmtlrstep1.h"
#include "cmtlrstep2.h"
#include "cmtlrstep3.h"
#include "cmtlrstep4.h"
#include "cmtlrstep5.h"
#include "cmtlrstep6.h"
#include "cmtlrstep7.h"
#include "cmtlrstep8.h"
#include "cmtlrstep9.h"
#include "cmtlrstep10.h"
#include "cmtlrstep11.h"
#include "cmtlrstep12.h"
#include "cmtlrstep13.h"
#include "cnilrstep1.h"
#include "cnilrstep2.h"
#include "cx3pstep1.h"
#include "cx3pstep2.h"
#include "cx3pstep3.h"
#include "cx3pstep4.h"
#include "cx3pstep5.h"
#include "cx3pstep6.h"
#include "cx3pstep7.h"
#include "cx3pstep8.h"
#include "cx3pstep9.h"
#include "cx3pstep10.h"
#include "cx3pstep11.h"
#include "cx3pstep12.h"
#include "cx3pstep13.h"
#include "cnetlocstep1.h"
#include "cnetlocstep2.h"
#include "cnetlocstep3.h"
#include "cnetlocstep4.h"
#include "cnetlocstep5.h"
#include "cnetlocstep6.h"
#include "cnetlocstep7.h"
#include "cnetlocstep8.h"
#include "cnetlocstep9.h"
#include "cnetlocstep10.h"
#include "cconflictstep1.h"
#include "cconflictstep2.h"
#include "cconflictstep3.h"
#include "cconflictstep4.h"
#include "cconflictstep5.h"
#include "cmiscstep1.h"

_LIT(KServerName,"te_testprotocolmodulesuite");


/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
LOCAL_C void MainL()
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	// Create the CTestServer derived server
	CTe_TestProtocolModuleSuite* server = NULL;
	TRAPD(err,server = CTe_TestProtocolModuleSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
		
	delete server;
	delete sched;
	}


/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
GLDEF_C TInt E32Main()
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


/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
CTe_TestProtocolModuleSuite* CTe_TestProtocolModuleSuite::NewL()
	{
	CTe_TestProtocolModuleSuite * server = new (ELeave) CTe_TestProtocolModuleSuite();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
CTestStep* CTe_TestProtocolModuleSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	// PLUGIN

              if(aStepName == KPluginStep1)
                            testStep = new CPluginStep1();
              else if(aStepName == KPluginStep2)
                            testStep = new CPluginStep2();
	
	// MO-LR

              else if(aStepName == KMoLrStep1)
                            testStep = new CMoLrStep1();
              else if(aStepName == KMoLrStep2)
                            testStep = new CMoLrStep2();
              else if(aStepName == KMoLrStep3)
                            testStep = new CMoLrStep3();
              else if(aStepName == KMoLrStep4)
                            testStep = new CMoLrStep4();
              else if(aStepName == KMoLrStep5)
                            testStep = new CMoLrStep5();
              else if(aStepName == KMoLrStep6)
                            testStep = new CMoLrStep6();
              else if(aStepName == KMoLrStep7)
                            testStep = new CMoLrStep7();
              else if(aStepName == KMoLrStep8)
                            testStep = new CMoLrStep8();
              else if(aStepName == KMoLrStep9)
                            testStep = new CMoLrStep9();
              else if(aStepName == KMoLrStep10)
                            testStep = new CMoLrStep10();
              else if(aStepName == KMoLrStep11)
                            testStep = new CMoLrStep11();
              else if(aStepName == KMoLrStep12)
                            testStep = new CMoLrStep12();
              else if(aStepName == KMoLrStep13)
                            testStep = new CMoLrStep13();
              else if(aStepName == KMoLrStep14)
                            testStep = new CMoLrStep14();
              else if(aStepName == KMoLrStep15)
                            testStep = new CMoLrStep15();
              else if(aStepName == KMoLrStep16)
                            testStep = new CMoLrStep16();
              else if(aStepName == KMoLrStep17)
                            testStep = new CMoLrStep17();

	// Tracking

              else if(aStepName == KTrackStep1)
                            testStep = new CTrackStep1();
              else if(aStepName == KTrackStep2)
                            testStep = new CTrackStep2();
              else if(aStepName == KTrackStep3)
                            testStep = new CTrackStep3();

	// MT-LR

              else if(aStepName == KMtLrStep1)
                            testStep = new CMtLrStep1();
              else if(aStepName == KMtLrStep2)
                            testStep = new CMtLrStep2();
              else if(aStepName == KMtLrStep3)
                            testStep = new CMtLrStep3();
              else if(aStepName == KMtLrStep4)
                            testStep = new CMtLrStep4();
              else if(aStepName == KMtLrStep5)
                            testStep = new CMtLrStep5();
              else if(aStepName == KMtLrStep6)
                            testStep = new CMtLrStep6();
              else if(aStepName == KMtLrStep7)
                            testStep = new CMtLrStep7();
              else if(aStepName == KMtLrStep8)
                            testStep = new CMtLrStep8();
              else if(aStepName == KMtLrStep9)
                            testStep = new CMtLrStep9();
              else if(aStepName == KMtLrStep10)
                            testStep = new CMtLrStep10();
              else if(aStepName == KMtLrStep11)
                            testStep = new CMtLrStep11();
              else if(aStepName == KMtLrStep12)
                            testStep = new CMtLrStep12();
              else if(aStepName == KMtLrStep13)
                            testStep = new CMtLrStep13();
              
	// NI-LR

              else if(aStepName == KNiLrStep1)
                            testStep = new CNiLrStep1();
              else if(aStepName == KNiLrStep2)
                            testStep = new CNiLrStep2();
              
	// X3P

              else if(aStepName == KX3pStep1)
                            testStep = new CX3pStep1();
              else if(aStepName == KX3pStep2)
                            testStep = new CX3pStep2();
              else if(aStepName == KX3pStep3)
                            testStep = new CX3pStep3();
              else if(aStepName == KX3pStep4)
                            testStep = new CX3pStep4();
              else if(aStepName == KX3pStep5)
                            testStep = new CX3pStep5();
              else if(aStepName == KX3pStep6)
                            testStep = new CX3pStep6();
              else if(aStepName == KX3pStep7)
                            testStep = new CX3pStep7();
              else if(aStepName == KX3pStep8)
                            testStep = new CX3pStep8();
              else if(aStepName == KX3pStep9)
                            testStep = new CX3pStep9();
              else if(aStepName == KX3pStep10)
                            testStep = new CX3pStep10();
              else if(aStepName == KX3pStep11)
                            testStep = new CX3pStep11();              
              else if(aStepName == KX3pStep12)
                            testStep = new CX3pStep12();
              else if(aStepName == KX3pStep13)
                            testStep = new CX3pStep13();                    
	// Network Based Loc

              else if(aStepName == KNetLocStep1)
                            testStep = new CNetLocStep1();
              else if(aStepName == KNetLocStep2)
                            testStep = new CNetLocStep2();
              else if(aStepName == KNetLocStep3)
                            testStep = new CNetLocStep3();
              else if(aStepName == KNetLocStep4)
                            testStep = new CNetLocStep4();
              else if(aStepName == KNetLocStep5)
                            testStep = new CNetLocStep5();
              else if(aStepName == KNetLocStep6)
                            testStep = new CNetLocStep6();
              else if(aStepName == KNetLocStep7)
                            testStep = new CNetLocStep7();
              else if(aStepName == KNetLocStep8)
                            testStep = new CNetLocStep8();
              else if(aStepName == KNetLocStep9)
                            testStep = new CNetLocStep9();
              else if(aStepName == KNetLocStep10)
                            testStep = new CNetLocStep10();

              
	// Conflicts

              else if(aStepName == KConflictStep1)
                            testStep = new CConflictStep1();
              else if(aStepName == KConflictStep2)
                            testStep = new CConflictStep2();
              else if(aStepName == KConflictStep3)
                            testStep = new CConflictStep3();
              else if(aStepName == KConflictStep4)
                            testStep = new CConflictStep4();
              else if(aStepName == KConflictStep5)
                            testStep = new CConflictStep5();

	// MISC

              else if(aStepName == KMiscStep1)
                            testStep = new CMiscStep1();
             
	return testStep;
	}
