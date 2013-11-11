// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This defines the TE_RConnectionSuite class which is the 
// container class for the TE_RConnectionStep derived test steps
// 
//

/**
 @file
*/


#include "TE_RConnectionStep.h"
#include "TE_RConnectionSuite.h"
#include "TE_RConnectionHelperSteps.h"
#include "TE_RConnectionDummyNifTestSteps.h"
#include "TE_RConnectionNtRasTestSteps.h"
#include "TE_RConnectionNaptTestSteps.h"
#include "TE_AllInterfaceNotification.h" 
#include "TE_RConnectionBadNTRASTestSteps.h"
#include "TE_RConnectionCMM.h"
#include "TE_RConnectionConfigDaemonTestSteps.h"
#include "TE_RConnectionNewDummyTests.h"
#include "TE_RConnectionCompleteAPICoverage.h"
#include "TE_RConnectionUpsTestSteps.h"
#include <c32comm.h>


TE_RConnectionSuiteTestServer* TE_RConnectionSuiteTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	TE_RConnectionSuiteTestServer* server = new (ELeave) TE_RConnectionSuiteTestServer();
	CleanupStack::PushL(server);
	server->StartL(server->ServerName());
	CleanupStack::Pop(server);
	return server;
	}
	
	
TE_RConnectionSuiteTestServer::~TE_RConnectionSuiteTestServer()
	{

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
	TE_RConnectionSuiteTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = TE_RConnectionSuiteTestServer::NewL());
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



CTestStep* TE_RConnectionSuiteTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName.Compare(_L("Test1")) == 0)
		testStep=new TE_RConnectionTest1();
	else if(aStepName.Compare(_L("Test1b")) == 0)
		testStep=new TE_RConnectionTest1b();
	else if(aStepName.Compare(_L("Test2")) == 0)
		testStep=new TE_RConnectionTest2();
	else if(aStepName.Compare(_L("Test3")) == 0)
		testStep=new TE_RConnectionTest3();
	else if(aStepName.Compare(_L("Test4")) == 0)
		testStep=new TE_RConnectionTest4();
	else if(aStepName.Compare(_L("Test5")) == 0)
		testStep=new TE_RConnectionTest5();
	else if(aStepName.Compare(_L("Test6")) == 0)
		testStep=new TE_RConnectionTest6();
	else if(aStepName.Compare(_L("Test7")) == 0)
		testStep=new TE_RConnectionTest7();
	else if(aStepName.Compare(_L("Test8")) == 0)
		testStep=new TE_RConnectionTest8();
	else if(aStepName.Compare(_L("Test9")) == 0)
		testStep=new TE_RConnectionTest9();
	else if(aStepName.Compare(_L("Test10")) == 0)
		testStep=new TE_RConnectionTest10();
	else if(aStepName.Compare(_L("Test11")) == 0)
		testStep=new TE_RConnectionTest11();
	else if(aStepName.Compare(_L("Test12")) == 0)
		testStep=new TE_RConnectionTest12();
	else if(aStepName.Compare(_L("Test13")) == 0)
		testStep=new TE_RConnectionTest13();
	else if(aStepName.Compare(_L("Test14")) == 0)
		testStep=new TE_RConnectionTest14();
	else if(aStepName.Compare(_L("Test15")) == 0)
		testStep=new TE_RConnectionTest15();
	else if(aStepName.Compare(_L("Test16")) == 0)
		testStep=new TE_RConnectionTest16();
	else if(aStepName.Compare(_L("Test17")) == 0)
		testStep=new TE_RConnectionTest17();
	else if(aStepName.Compare(_L("Test18")) == 0)
		testStep=new TE_RConnectionTest18();
	else if(aStepName.Compare(_L("Test19")) == 0)
		testStep=new TE_RConnectionTest19();
	else if(aStepName.Compare(_L("Test20")) == 0)
		testStep=new TE_RConnectionTest20();
	else if(aStepName.Compare(_L("Test21")) == 0)
		testStep=new TE_RConnectionTest21();
	else if(aStepName.Compare(_L("Test22")) == 0)
		testStep=new TE_RConnectionTest22();
	else if(aStepName.Compare(_L("Test23")) == 0)
		testStep=new TE_RConnectionTest23();
	else if(aStepName.Compare(_L("Test24")) == 0)
		testStep=new TE_RConnectionTest24();
	else if(aStepName.Compare(_L("Test25")) == 0)
		testStep=new TE_RConnectionTest25();
	else if(aStepName.Compare(_L("Test26")) == 0)
		testStep=new TE_RConnectionTest26();
	else if(aStepName.Compare(_L("Test27")) == 0)
		testStep=new TE_RConnectionTest27();
	else if(aStepName.Compare(_L("Test28")) == 0)
		testStep=new TE_RConnectionTest28();
	else if(aStepName.Compare(_L("Test29")) == 0)
		testStep=new TE_RConnectionTest29();
	else if(aStepName.Compare(_L("Test30")) == 0)
		testStep=new TE_RConnectionTest30();
	else if(aStepName.Compare(_L("Test31")) == 0)
		testStep=new TE_RConnectionTest31();
	else if(aStepName.Compare(_L("Test32")) == 0)
		testStep=new TE_RConnectionTest32();
	else if(aStepName.Compare(_L("Test33")) == 0)
		testStep=new TE_RConnectionTest33();
	else if(aStepName.Compare(_L("Test34")) == 0)
		testStep=new TE_RConnectionTest34();
	
	/*	not implemented
	else if(aStepName.Compare(_L("Test35")) == 0)
		testStep=new TE_RConnectionTest35();
	else if(aStepName.Compare(_L("Test36")) == 0)
		testStep=new TE_RConnectionTest36();
	*/
	else if(aStepName.Compare(_L("Test37")) == 0)
		testStep=new TE_RConnectionTest37();
	else if(aStepName.Compare(_L("Test38")) == 0)
		testStep=new TE_RConnectionTest38();
	else if(aStepName.Compare(_L("Test39")) == 0)
		testStep=new TE_RConnectionTest39();
	else if(aStepName.Compare(_L("Test40")) == 0)
		testStep=new TE_RConnectionTest40();
	else if(aStepName.Compare(_L("Test41")) == 0)
		testStep=new TE_RConnectionTest41();
	else if(aStepName.Compare(_L("Test42")) == 0)
		testStep=new TE_RConnectionTest42();
	else if(aStepName.Compare(_L("Test43")) == 0)
		testStep=new TE_RConnectionTest43();
	else if(aStepName.Compare(_L("Test44")) == 0)
		testStep=new TE_RConnectionTest44();
	else if(aStepName.Compare(_L("Test45")) == 0)
		testStep=new TE_RConnectionTest45();
	else if(aStepName.Compare(_L("Test46")) == 0)
		testStep=new TE_RConnectionTest46();
	else if(aStepName.Compare(_L("Test47")) == 0)
		testStep=new TE_RConnectionTest47();
	else if(aStepName.Compare(_L("Test48")) == 0)
		testStep=new TE_RConnectionTest48();
	else if(aStepName.Compare(_L("Test49")) == 0)
		testStep=new TE_RConnectionTest49();
	else if(aStepName.Compare(_L("Test50")) == 0)
		testStep=new TE_RConnectionTest50();
	else if(aStepName.Compare(_L("Test51")) == 0)
		testStep=new TE_RConnectionTest51();
	else if(aStepName.Compare(_L("Test52")) == 0)
		testStep=new TE_RConnectionTest52();
	else if(aStepName.Compare(_L("Test53")) == 0)
		testStep=new TE_RConnectionTest53();
	else if(aStepName.Compare(_L("Test54")) == 0)
		testStep=new TE_RConnectionTest54();
	else if(aStepName.Compare(_L("Test55")) == 0)
		testStep=new TE_RConnectionTest55();
	else if(aStepName.Compare(_L("Test56")) == 0)
		testStep=new TE_RConnectionTest56();
	else if(aStepName.Compare(_L("Test57")) == 0)
		testStep=new TE_RConnectionTest57();
	else if(aStepName.Compare(_L("Test58")) == 0)
		testStep=new TE_RConnectionTest58();
	else if(aStepName.Compare(_L("Test59")) == 0)
		testStep=new TE_RConnectionTest59();
	else if(aStepName.Compare(_L("Test60")) == 0)
		testStep=new TE_RConnectionTest60();
	else if(aStepName.Compare(_L("Test61")) == 0)
		testStep=new TE_RConnectionTest61();
	/*
	else if(aStepName.Compare(_L("Test62")) == 0)
		testStep=new TE_RConnectionTest62();
	*/
	else if(aStepName.Compare(_L("Test63")) == 0)
		testStep=new TE_RConnectionTest63();
	else if(aStepName.Compare(_L("Test64")) == 0)
		testStep=new TE_RConnectionTest64();
	else if(aStepName.Compare(_L("Test65")) == 0)
		testStep=new TE_RConnectionTest65();
	else if(aStepName.Compare(_L("Test66")) == 0)
		testStep=new TE_RConnectionTest66();
	else if(aStepName.Compare(_L("Test67")) == 0)
		testStep=new TE_RConnectionTest67();
	else if(aStepName.Compare(_L("Test68")) == 0)
		testStep=new TE_RConnectionTest68();
	else if(aStepName.Compare(_L("Test69")) == 0)
		testStep=new TE_RConnectionTest69();
	else if(aStepName.Compare(_L("Test70")) == 0)
		testStep=new TE_RConnectionTest70();
	else if(aStepName.Compare(_L("Test71")) == 0)
		testStep=new TE_RConnectionTest71();
	else if(aStepName.Compare(_L("Test72")) == 0)
		testStep=new TE_RConnectionTest72();
	else if(aStepName.Compare(_L("Test73")) == 0)
		testStep=new TE_RConnectionTest73();
	else if(aStepName.Compare(_L("Test74")) == 0)
		testStep=new TE_RConnectionTest74();
	else if(aStepName.Compare(_L("Test75")) == 0)
		testStep=new TE_RConnectionTest75();
	else if(aStepName.Compare(_L("Test76")) == 0)
		testStep=new TE_RConnectionTest76();
	else if(aStepName.Compare(_L("Test77")) == 0)
		testStep=new TE_RConnectionTest77();
	else if(aStepName.Compare(_L("Test78")) == 0)
		testStep=new TE_RConnectionTest78();
	else if(aStepName.Compare(_L("Test79")) == 0)
		testStep=new TE_RConnectionTest79();
	else if(aStepName.Compare(_L("Test80")) == 0)
		testStep=new TE_RConnectionTest80();
	else if(aStepName.Compare(_L("Test81")) == 0)
		testStep=new TE_RConnectionTest81();
	
	//Splitting the testStep creation for ccover "out of memory" problem which is caused due to 
	//large number of if..else loops
		
	// NTras tests
	if (testStep == NULL)
		{
		testStep = CreateRConnTestStep1(aStepName);
		}
	if (testStep == NULL)
		{
		testStep = CreateRConnTestStep2(aStepName);
		}
	if (testStep == NULL)
		{
		testStep = CreateRConnTestStep3(aStepName);
		}
	if (testStep == NULL)
		{
		testStep = CreateRConnTestStep4(aStepName);
		}
	if (testStep == NULL)
		{
		testStep = CreateRConnTestStep5(aStepName);
		}
	if (testStep == NULL)
		{
		testStep = CreateRConnTestStep6(aStepName);
		}
	if (testStep == NULL)
		{
		testStep = CreateRConnTestStep7(aStepName);
		}
	if (testStep == NULL)
		{
		testStep = CreateRConnTestStep8(aStepName);
		}		
	if (testStep == NULL)
		{
		testStep = CreateRConnTestStep9(aStepName);
		}
	
	// UPS tests
	if (testStep == NULL)
	 	{
	 	testStep = CreateUpsTestStep(aStepName);
	 	}
		
	return testStep;
	}

CTestStep* TE_RConnectionSuiteTestServer::CreateRConnTestStep1(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("Test101")) == 0)
		testStep=new TE_RConnectionTest101();
	else if(aStepName.Compare(_L("Test102")) == 0)
		testStep=new TE_RConnectionTest102();
	else if(aStepName.Compare(_L("Test103")) == 0)
		testStep=new TE_RConnectionTest103();
	else if(aStepName.Compare(_L("Test104")) == 0)
		testStep=new TE_RConnectionTest104();
	else if(aStepName.Compare(_L("Test105")) == 0)
		testStep=new TE_RConnectionTest105();
	else if(aStepName.Compare(_L("Test106")) == 0)
		testStep=new TE_RConnectionTest106();
	else if(aStepName.Compare(_L("Test107")) == 0)
		testStep=new TE_RConnectionTest107();
	else if(aStepName.Compare(_L("Test108")) == 0)
		testStep=new TE_RConnectionTest108();
	else if(aStepName.Compare(_L("Test109")) == 0)
		testStep=new TE_RConnectionTest109();
	else if(aStepName.Compare(_L("Test110")) == 0)
		testStep=new TE_RConnectionTest110();
	else if(aStepName.Compare(_L("Test111")) == 0)
		testStep=new TE_RConnectionTest111();
	else if(aStepName.Compare(_L("Test112")) == 0)
		testStep=new TE_RConnectionTest112();
	else if(aStepName.Compare(_L("Test113")) == 0)
		testStep=new TE_RConnectionTest113();
	else if(aStepName.Compare(_L("Test114")) == 0)
		testStep=new TE_RConnectionTest114();
	else if(aStepName.Compare(_L("Test115")) == 0)
		testStep=new TE_RConnectionTest115();
	else if(aStepName.Compare(_L("Test116")) == 0)
		testStep=new TE_RConnectionTest116();
	else if(aStepName.Compare(_L("SnapNotSupported")) == 0)
		testStep=new TE_RConnectionSnapNotSupported();
	
	return testStep;
	}
	
CTestStep* TE_RConnectionSuiteTestServer::CreateRConnTestStep2(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("Test201")) == 0)
		testStep=new TE_RConnectionTest201();
	else if(aStepName.Compare(_L("Test202")) == 0)
		testStep=new TE_RConnectionTest202();
	else if(aStepName.Compare(_L("Test203")) == 0)
		testStep=new TE_RConnectionTest203();
	else if(aStepName.Compare(_L("Test204")) == 0)
		testStep=new TE_RConnectionTest204();
	else if(aStepName.Compare(_L("Test205")) == 0)
		testStep=new TE_RConnectionTest205();
	else if(aStepName.Compare(_L("Test206")) == 0)
		testStep=new TE_RConnectionTest206();
	else if(aStepName.Compare(_L("Test207")) == 0)
		testStep=new TE_RConnectionTest207();
	else if(aStepName.Compare(_L("Test208")) == 0)
		testStep=new TE_RConnectionTest208();
	else if(aStepName.Compare(_L("Test209")) == 0)
		testStep=new TE_RConnectionTest209();
	else if(aStepName.Compare(_L("Test210")) == 0)
		testStep=new TE_RConnectionTest210();
	else if(aStepName.Compare(_L("Test211")) == 0)
		testStep=new TE_RConnectionTest211();
	else if(aStepName.Compare(_L("Test212")) == 0)
		testStep=new TE_RConnectionTest212();
	else if(aStepName.Compare(_L("Test213")) == 0)
		testStep=new TE_RConnectionTest213();
	else if(aStepName.Compare(_L("Test214")) == 0)
		testStep=new TE_RConnectionTest214();
	else if(aStepName.Compare(_L("Test215")) == 0)
		testStep=new TE_RConnectionTest215();
	else if(aStepName.Compare(_L("Test216")) == 0)
		testStep=new TE_RConnectionTest216();
	else if(aStepName.Compare(_L("Test217")) == 0)
		testStep=new TE_RConnectionTest217();
	else if(aStepName.Compare(_L("Test218")) == 0)
		testStep=new TE_RConnectionTest218();
	else if(aStepName.Compare(_L("Test219")) == 0)
		testStep=new TE_RConnectionTest219();
	else if(aStepName.Compare(_L("Test220")) == 0)
		testStep=new TE_RConnectionTest220();
	//Added for INC-094862 (Static DNS dynamic IP)
	else if(aStepName.Compare(_L("Test221")) == 0)
		testStep=new TE_RConnectionTest221();
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT
	else if(aStepName.Compare(_L("Test222")) == 0)
		testStep=new TE_RConnectionTest222();
	else if(aStepName.Compare(_L("Test223")) == 0)
		testStep=new TE_RConnectionTest223();
	else if(aStepName.Compare(_L("Test224")) == 0)
		testStep=new TE_RConnectionTest224();	
#endif	
	return testStep;
	}
	
CTestStep* TE_RConnectionSuiteTestServer::CreateRConnTestStep3(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("Test300")) == 0)
		testStep=new TE_RConnectionTest300();
	else if(aStepName.Compare(_L("Test301")) == 0)
		testStep=new TE_RConnectionTest301();
	else if(aStepName.Compare(_L("Test302")) == 0)
		testStep=new TE_RConnectionTest302();
	else if(aStepName.Compare(_L("Test303")) == 0)
		testStep=new TE_RConnectionTest303();
	else if(aStepName.Compare(_L("Test304")) == 0)
		testStep=new TE_RConnectionTest304();
	else if(aStepName.Compare(_L("Test305")) == 0)
		testStep=new TE_RConnectionTest305();
	else if(aStepName.Compare(_L("Test306")) == 0)
		testStep=new TE_RConnectionTest306();
	else if(aStepName.Compare(_L("Test307")) == 0)
		testStep=new TE_RConnectionTest307();
	else if(aStepName.Compare(_L("Test308")) == 0)
		testStep=new TE_RConnectionTest308();
	else if(aStepName.Compare(_L("Test309")) == 0)
		testStep=new TE_RConnectionTest309();
	else if(aStepName.Compare(_L("Test310")) == 0)
		testStep=new TE_RConnectionTest310();
	else if(aStepName.Compare(_L("Test311")) == 0)
		testStep=new TE_RConnectionTest311();
	else if(aStepName.Compare(_L("Test312")) == 0)
		testStep=new TE_RConnectionTest312();
	else if(aStepName.Compare(_L("Test313")) == 0)
		testStep=new TE_RConnectionTest313();
	else if(aStepName.Compare(_L("Test314")) == 0)
		testStep=new TE_RConnectionTest314();
	else if(aStepName.Compare(_L("Test315")) == 0)
		testStep=new TE_RConnectionTest315();
	else if(aStepName.Compare(_L("Test316")) == 0)
		testStep=new TE_RConnectionTest316();
	else if(aStepName.Compare(_L("Test317")) == 0)
		testStep=new TE_RConnectionTest317();
	else if(aStepName.Compare(_L("Test318")) == 0)
		testStep=new TE_RConnectionTest318();
	else if(aStepName.Compare(_L("Test319")) == 0)
		testStep=new TE_RConnectionTest319();
	else if (aStepName.Compare(_L("TestAllInterfaceNot210")) == 0)
	    testStep = new TE_RConnectionTestAllInterfaceNot210();
	
	return testStep;
	}
	
CTestStep* TE_RConnectionSuiteTestServer::CreateRConnTestStep4(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("Test400")) == 0)
		testStep=new TE_RConnectionTest400();
	else if(aStepName.Compare(_L("Test401")) == 0)
		testStep=new TE_RConnectionTest401();
	else if(aStepName.Compare(_L("Test402")) == 0)
		testStep=new TE_RConnectionTest402();
	else if(aStepName.Compare(_L("Test403")) == 0)
		testStep=new TE_RConnectionTest403();
	else if(aStepName.Compare(_L("Test404")) == 0)
		testStep=new TE_RConnectionTest404();
	else if(aStepName.Compare(_L("Test405")) == 0)
		testStep=new TE_RConnectionTest405();
	else if(aStepName.Compare(_L("Test406")) == 0)
		testStep=new TE_RConnectionTest406();
	else if(aStepName.Compare(_L("Test407")) == 0)
		testStep=new TE_RConnectionTest407();
	else if(aStepName.Compare(_L("Test408")) == 0)
		testStep=new TE_RConnectionTest408();
	else if(aStepName.Compare(_L("Test409")) == 0)
		testStep=new TE_RConnectionTest409();
	else if(aStepName.Compare(_L("Test410")) == 0)
		testStep=new TE_RConnectionTest410();
	else if(aStepName.Compare(_L("Test411")) == 0)
		testStep=new TE_RConnectionTest411();
	else if(aStepName.Compare(_L("Test412")) == 0)
		testStep=new TE_RConnectionTest412();
	else if(aStepName.Compare(_L("Test413")) == 0)
		testStep=new TE_RConnectionTest413();
	else if(aStepName.Compare(_L("Test414")) == 0)
		testStep=new TE_RConnectionTest414();
	else if(aStepName.Compare(_L("Test415")) == 0)
		testStep=new TE_RConnectionTest415();
	else if(aStepName.Compare(_L("Test416")) == 0)
		testStep=new TE_RConnectionTest416();
	else if(aStepName.Compare(_L("Test417")) == 0)
		testStep=new TE_RConnectionTest417();
	else if(aStepName.Compare(_L("Test418")) == 0)
		testStep=new TE_RConnectionTest418();
	else if(aStepName.Compare(_L("Test419")) == 0)
		testStep=new TE_RConnectionTest419();
	else if(aStepName.Compare(_L("Test419b")) == 0)
		testStep=new TE_RConnectionTest419b();
	else if(aStepName.Compare(_L("Test420")) == 0)
		testStep=new TE_RConnectionTest420();
	else if(aStepName.Compare(_L("Test421")) == 0)
		testStep=new TE_RConnectionTest421();
	else if(aStepName.Compare(_L("Test422")) == 0)
		testStep=new TE_RConnectionTest422();
	else if(aStepName.Compare(_L("Test423")) == 0)
		testStep=new TE_RConnectionTest423();
	else if(aStepName.Compare(_L("Test424")) == 0)
		testStep=new TE_RConnectionTest424();
	else if(aStepName.Compare(_L("Test425")) == 0)
		testStep=new TE_RConnectionTest425();
	else if(aStepName.Compare(_L("Test426")) == 0)
		testStep=new TE_RConnectionTest426();
	else if(aStepName.Compare(_L("Test427")) == 0)
		testStep=new TE_RConnectionTest427();
	else if(aStepName.Compare(_L("Test428")) == 0)
		testStep=new TE_RConnectionTest428();
	else if(aStepName.Compare(_L("Test429")) == 0)
		testStep=new TE_RConnectionTest429();
	else if(aStepName.Compare(_L("Test430")) == 0)
		testStep=new TE_RConnectionTest430();
	else if(aStepName.Compare(_L("Test431")) == 0)
		testStep=new TE_RConnectionTest431();
	else if(aStepName.Compare(_L("Test432")) == 0)
		testStep=new TE_RConnectionTest432();
	else if(aStepName.Compare(_L("Test433")) == 0)
		testStep=new TE_RConnectionTest433();
	else if(aStepName.Compare(_L("Test434")) == 0)
		testStep=new TE_RConnectionTest434();
	else if(aStepName.Compare(_L("Test435")) == 0)
		testStep=new TE_RConnectionTest435();
	else if(aStepName.Compare(_L("Test436")) == 0)
		testStep=new TE_RConnectionTest436();
	else if(aStepName.Compare(_L("Test437")) == 0)
		testStep=new TE_RConnectionTest437();
	else if(aStepName.Compare(_L("Test438")) == 0)
		testStep=new TE_RConnectionTest438();
	else if(aStepName.Compare(_L("Test439")) == 0)
		testStep=new TE_RConnectionTest439();
	else if(aStepName.Compare(_L("Test440")) == 0)
		testStep=new TE_RConnectionTest440();
	else if(aStepName.Compare(_L("Test441")) == 0)
		testStep=new TE_RConnectionTest441();
	else if(aStepName.Compare(_L("Test442")) == 0)
		testStep=new TE_RConnectionTest442();
	else if(aStepName.Compare(_L("Test443")) == 0)
		testStep=new TE_RConnectionTest443();
	else if(aStepName.Compare(_L("Test444")) == 0)
		testStep=new TE_RConnectionTest444();
	else if(aStepName.Compare(_L("Test445")) == 0)
		testStep=new TE_RConnectionTest445();
	else if(aStepName.Compare(_L("Test446")) == 0)
		testStep=new TE_RConnectionTest446();
	else if(aStepName.Compare(_L("Test447")) == 0)
		testStep=new TE_RConnectionTest447();
	else if(aStepName.Compare(_L("Test448")) == 0)
		testStep=new TE_RConnectionTest448();
	else if(aStepName.Compare(_L("Test449")) == 0)
		testStep=new TE_RConnectionTest449();
	else if(aStepName.Compare(_L("Test450")) == 0)
		testStep=new TE_RConnectionTest450();
	else if(aStepName.Compare(_L("Test451")) == 0)
		testStep=new TE_RConnectionTest451();
	else if(aStepName.Compare(_L("Test452")) == 0)
		testStep=new TE_RConnectionTest452();
	else if(aStepName.Compare(_L("Test453")) == 0)
		testStep=new TE_RConnectionTest453();
	else if(aStepName.Compare(_L("Test454")) == 0)
		testStep=new TE_RConnectionTest454();
	else if(aStepName.Compare(_L("Test455")) == 0)
		testStep=new TE_RConnectionTest455();
	else if(aStepName.Compare(_L("Test456")) == 0)
		testStep=new TE_RConnectionTest456();
	else if(aStepName.Compare(_L("Test457")) == 0)
		testStep=new TE_RConnectionTest457();
	else if(aStepName.Compare(_L("Test458")) == 0)
		testStep=new TE_RConnectionTest458();
	else if(aStepName.Compare(_L("Test459")) == 0)
		testStep=new TE_RConnectionTest459();
	else if(aStepName.Compare(_L("Test460")) == 0)
		testStep=new TE_RConnectionTest460();
	else if(aStepName.Compare(_L("Test461")) == 0)
		testStep=new TE_RConnectionTest461();
	else if(aStepName.Compare(_L("Test462")) == 0)
		testStep=new TE_RConnectionTest462();
	else if(aStepName.Compare(_L("Test463")) == 0)
		testStep=new TE_RConnectionTest463();
	else if(aStepName.Compare(_L("Test464")) == 0)
		testStep=new TE_RConnectionTest464();
	else if(aStepName.Compare(_L("Test465")) == 0)
		testStep=new TE_RConnectionTest465();
	else if(aStepName.Compare(_L("Test466")) == 0)
		testStep=new TE_RConnectionTest466();
	else if(aStepName.Compare(_L("Test467")) == 0)
		testStep=new TE_RConnectionTest467();
	else if(aStepName.Compare(_L("Test468")) == 0)
		testStep=new TE_RConnectionTest468();
	else if(aStepName.Compare(_L("Test469")) == 0)
		testStep=new TE_RConnectionTest469();
	else if(aStepName.Compare(_L("Test470")) == 0)
		testStep=new TE_RConnectionTest470();
	else if(aStepName.Compare(_L("Test471")) == 0)
		testStep=new TE_RConnectionTest471();
	else if(aStepName.Compare(_L("Test472")) == 0)
		testStep=new TE_RConnectionTest472();
	else if(aStepName.Compare(_L("Test473")) == 0)
		testStep=new TE_RConnectionTest473();
	else if(aStepName.Compare(_L("Test474")) == 0)
		testStep=new TE_RConnectionTest474();
	else if(aStepName.Compare(_L("Test475")) == 0)
		testStep=new TE_RConnectionTest475();
	else if(aStepName.Compare(_L("Test476")) == 0)
		testStep=new TE_RConnectionTest476();
	else if(aStepName.Compare(_L("Test477")) == 0)
		testStep=new TE_RConnectionTest477();
	else if(aStepName.Compare(_L("Test478")) == 0)
		testStep=new TE_RConnectionTest478();
	else if(aStepName.Compare(_L("Test479")) == 0)
		testStep=new TE_RConnectionTest479();
	else if(aStepName.Compare(_L("Test480")) == 0)
		testStep=new TE_RConnectionTest480();
    else if(aStepName.Compare(_L("Test481")) == 0)
        testStep=new TE_RConnectionTest481();
    else if(aStepName.Compare(_L("Test482")) == 0)
        testStep=new TE_RConnectionTest482();
    else if(aStepName.Compare(_L("Test483")) == 0)
        testStep=new TE_RConnectionTest483();
    else if(aStepName.Compare(_L("Test484")) == 0)
        testStep=new TE_RConnectionTest484();

	return testStep;
	}
	
CTestStep* TE_RConnectionSuiteTestServer::CreateRConnTestStep5(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("Test500")) == 0)
		testStep=new TE_RConnectionTest500();
	else if(aStepName.Compare(_L("Test501")) == 0)
		testStep=new TE_RConnectionTest501();
	
	return testStep;
	}
	
CTestStep* TE_RConnectionSuiteTestServer::CreateRConnTestStep6(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("Test600")) == 0)
		testStep=new TE_RConnectionTest600();
	else if(aStepName.Compare(_L("Test601")) == 0)
		testStep=new TE_RConnectionTest601();
	else if(aStepName.Compare(_L("Test602")) == 0)
		testStep=new TE_RConnectionTest602();
	else if(aStepName.Compare(_L("Test603")) == 0)
		testStep=new TE_RConnectionTest603();
	else if(aStepName.Compare(_L("Test604")) == 0)
		testStep=new TE_RConnectionTest604();
	else if(aStepName.Compare(_L("Test605")) == 0)
		testStep=new TE_RConnectionTest605();
	else if(aStepName.Compare(_L("Test606")) == 0)
		testStep=new TE_RConnectionTest606();
	else if(aStepName.Compare(_L("Test607")) == 0)
		testStep=new TE_RConnectionTest607();
	else if(aStepName.Compare(_L("Test608")) == 0)
		testStep=new TE_RConnectionTest608();
	else if(aStepName.Compare(_L("Test609")) == 0)
		testStep=new TE_RConnectionTest609();
	
	return testStep;
	}
	
CTestStep* TE_RConnectionSuiteTestServer::CreateRConnTestStep7(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("Test700")) == 0)
		testStep=new TE_RConnectionTest700();
	else if(aStepName.Compare(_L("Test701")) == 0)
		testStep=new TE_RConnectionTest701();
	else if(aStepName.Compare(_L("Test702")) == 0)
		testStep=new TE_RConnectionTest702();
	else if(aStepName.Compare(_L("Test703")) == 0)
		testStep=new TE_RConnectionTest703();
	else if(aStepName.Compare(_L("Test705")) == 0)
		testStep=new TE_RConnectionTest705();
	else if(aStepName.Compare(_L("Test706")) == 0)
		testStep=new TE_RConnectionTest706();
	else if(aStepName.Compare(_L("Test707")) == 0)
		testStep=new TE_RConnectionTest707();
	else if(aStepName.Compare(_L("Test708")) == 0)
		testStep=new TE_RConnectionTest708();
	else if(aStepName.Compare(_L("Test709")) == 0)
		testStep=new TE_RConnectionTest709();
	else if(aStepName.Compare(_L("Test710")) == 0)
		testStep=new TE_RConnectionTest710();
	else if(aStepName.Compare(_L("Test711")) == 0)
		testStep=new TE_RConnectionTest711();
	else if(aStepName.Compare(_L("Test712")) == 0)
		testStep=new TE_RConnectionTest712();
	else if(aStepName.Compare(_L("Test713")) == 0)
		testStep=new TE_RConnectionTest713();
	
	return testStep;
	}
	
CTestStep* TE_RConnectionSuiteTestServer::CreateRConnTestStep8(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("Test800")) == 0)
		testStep=new TE_RConnectionTest800();
	else if(aStepName.Compare(_L("Test801")) == 0)
		testStep=new TE_RConnectionTest801();
	else if(aStepName.Compare(_L("Test802")) == 0)
		testStep=new TE_RConnectionTest802();
	else if(aStepName.Compare(_L("Test803")) == 0)
		testStep=new TE_RConnectionTest803();
	else if(aStepName.Compare(_L("Test804")) == 0)
		testStep=new TE_RConnectionTest804();
	else if(aStepName.Compare(_L("Test805")) == 0)
		testStep=new TE_RConnectionTest805();
	else if(aStepName.Compare(_L("Test806")) == 0)
		testStep=new TE_RConnectionTest806();
	else if(aStepName.Compare(_L("Test807")) == 0)
		testStep=new TE_RConnectionTest807();
	else if(aStepName.Compare(_L("Test808")) == 0)
		testStep=new TE_RConnectionTest808();
	else if(aStepName.Compare(_L("Test809")) == 0)
		testStep=new TE_RConnectionTest809();
	else if(aStepName.Compare(_L("Test810")) == 0)
		testStep=new TE_RConnectionTest810();
	else if(aStepName.Compare(_L("Test811")) == 0)
		testStep=new TE_RConnectionTest811();
	else if(aStepName.Compare(_L("Test812")) == 0)
		testStep=new TE_RConnectionTest812();
	
	return testStep;
	}
	
CTestStep* TE_RConnectionSuiteTestServer::CreateRConnTestStep9(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("Test950")) == 0)
		testStep=new TE_RConnectionTest950();
	else if(aStepName.Compare(_L("Test951")) == 0)
		testStep=new TE_RConnectionTest951();
	else if(aStepName.Compare(_L("Test952")) == 0)
		testStep=new TE_RConnectionTest952();
	else if(aStepName.Compare(_L("Test953")) == 0)
		testStep=new TE_RConnectionTest953();
	else if(aStepName.Compare(_L("Test954")) == 0)
		testStep=new TE_RConnectionTest954();
	else if(aStepName.Compare(_L("Test955")) == 0)
		testStep=new TE_RConnectionTest955();
	else if(aStepName.Compare(_L("Test956")) == 0)
		testStep=new TE_RConnectionTest956();
	else if(aStepName.Compare(_L("Test957")) == 0)
		testStep=new TE_RConnectionTest957();
	else if(aStepName.Compare(_L("Test958")) == 0)
		testStep=new TE_RConnectionTest958();
	else if(aStepName.Compare(_L("Test959")) == 0)
		testStep=new TE_RConnectionTest959();
	else if(aStepName.Compare(_L("Test960")) == 0)
		testStep=new TE_RConnectionTest960();
	else if(aStepName.Compare(_L("Test961")) == 0)
		testStep=new TE_RConnectionTest961();
	else if(aStepName.Compare(_L("Test962")) == 0)
	  testStep=new TE_RConnectionTest962();
	else if(aStepName.Compare(_L("Test966")) == 0)
		testStep=new TE_RConnectionTest966();
	else if(aStepName.Compare(_L("Test967")) == 0)
		testStep=new TE_RConnectionTest967();
	else if(aStepName.Compare(_L("Test968")) == 0)
		testStep=new TE_RConnectionTest968();
	else if(aStepName.Compare(_L("TestConnectionEnum")) == 0)
		testStep=new TE_RConnectionTest52();
	else if(aStepName.Compare(_L("TestClientEnum")) == 0)
		testStep=new TE_RConnectionTest53();
	else if(aStepName.Compare(_L("TestSocketEnum")) == 0)
		testStep=new TE_RConnectionTest54();
	
	return testStep;
	}
	
CTestStep* TE_RConnectionSuiteTestServer::CreateUpsTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName.Compare(_L("CloseInterfaces")) == 0)
		testStep=new TE_RConnectionUPSCloseInterfaces();
	else if(aStepName.Compare(_L("SetNotifFileOverride")) == 0)
		testStep=new TE_RConnectionUPSSetNotifFileOverride();
	else if(aStepName.Compare(_L("UnsetNotifFileOverride")) == 0)
		testStep=new TE_RConnectionUPSUnsetNotifFileOverride();
	else if(aStepName.Compare(_L("UPSPreMultiThreaded")) == 0)
		testStep=new TE_RConnectionUPSPreMulti();
	else if(aStepName.Compare(_L("UPSPostMultiThreaded")) == 0)
		testStep=new TE_RConnectionUPSPostMulti();
	else if(aStepName.Compare(_L("UPSTest1a")) == 0)
		testStep=new TE_RConnectionUPSTest1a();
	else if(aStepName.Compare(_L("UPSTest1b")) == 0)
		testStep=new TE_RConnectionUPSTest1b();
	else if(aStepName.Compare(_L("UPSTest1c")) == 0)
		testStep=new TE_RConnectionUPSTest1c();
	else if(aStepName.Compare(_L("UPSTest1d")) == 0)
		testStep=new TE_RConnectionUPSTest1d();
	else if(aStepName.Compare(_L("UPSTest2a")) == 0)
		testStep=new TE_RConnectionUPSTest2a();
	else if(aStepName.Compare(_L("UPSTest2b")) == 0)
		testStep=new TE_RConnectionUPSTest2b();
	else if(aStepName.Compare(_L("UPSTest2c")) == 0)
		testStep=new TE_RConnectionUPSTest2c();
	else if(aStepName.Compare(_L("UPSTest2d")) == 0)
		testStep=new TE_RConnectionUPSTest2d();
	else if(aStepName.Compare(_L("UPSTest3a")) == 0)
		testStep=new TE_RConnectionUPSTest3a();
	else if(aStepName.Compare(_L("UPSTest3b")) == 0)
		testStep=new TE_RConnectionUPSTest3b();
	else if(aStepName.Compare(_L("UPSTest3c")) == 0)
		testStep=new TE_RConnectionUPSTest3c();
	else if(aStepName.Compare(_L("UPSTest3d")) == 0)
		testStep=new TE_RConnectionUPSTest3d();
	else if(aStepName.Compare(_L("UPSTest4a")) == 0)
		testStep=new TE_RConnectionUPSTest4a();
	else if(aStepName.Compare(_L("UPSTest4c")) == 0)
		testStep=new TE_RConnectionUPSTest4c();
	else if(aStepName.Compare(_L("UPSTest5a")) == 0)
		testStep=new TE_RConnectionUPSTest5a();
	else if(aStepName.Compare(_L("UPSTest6a")) == 0)
		testStep=new TE_RConnectionUPSTest6a();
	else if(aStepName.Compare(_L("UPSTest6b")) == 0)
		testStep=new TE_RConnectionUPSTest6b();
	else if(aStepName.Compare(_L("UPSTest6c")) == 0)
		testStep=new TE_RConnectionUPSTest6c();
	else if(aStepName.Compare(_L("UPSTest6d")) == 0)
		testStep=new TE_RConnectionUPSTest6d();
	else if(aStepName.Compare(_L("UPSTest7a")) == 0)
		testStep=new TE_RConnectionUPSTest7a();
	else if(aStepName.Compare(_L("UPSTest7b")) == 0)
		testStep=new TE_RConnectionUPSTest7b();
	else if(aStepName.Compare(_L("UPSTest7c")) == 0)
		testStep=new TE_RConnectionUPSTest7c();
	else if(aStepName.Compare(_L("UPSTest7d")) == 0)
		testStep=new TE_RConnectionUPSTest7d();
/*	else if(aStepName.Compare(_L("UPSTest8a")) == 0)
		testStep=new TE_RConnectionUPSTest8a();
	else if(aStepName.Compare(_L("UPSTest8b")) == 0)
		testStep=new TE_RConnectionUPSTest8b();
	else if(aStepName.Compare(_L("UPSTest8c")) == 0)
		testStep=new TE_RConnectionUPSTest8c();
	else if(aStepName.Compare(_L("UPSTest8d")) == 0)
		testStep=new TE_RConnectionUPSTest8d();*/
	else if(aStepName.Compare(_L("UPSTest10")) == 0)
		testStep=new TE_RConnectionUPSTest10();
	else if(aStepName.Compare(_L("UPSTest11")) == 0)
		testStep=new TE_RConnectionUPSTest11();
	else if(aStepName.Compare(_L("UPSTest12")) == 0)
		testStep=new TE_RConnectionUPSTest12();
	else if(aStepName.Compare(_L("UPSTest13")) == 0)
		testStep=new TE_RConnectionUPSTest13();
	else if(aStepName.Compare(_L("UPSTest14")) == 0)
		testStep=new TE_RConnectionUPSTest14();
	else if(aStepName.Compare(_L("UPSTest15")) == 0)
		testStep=new TE_RConnectionUPSTest15();
	else if(aStepName.Compare(_L("UPSTest16")) == 0)
		testStep=new TE_RConnectionUPSTest16();
	else if(aStepName.Compare(_L("UPSTest17")) == 0)
		testStep=new TE_RConnectionUPSTest17();
	else if(aStepName.Compare(_L("UPSTest18")) == 0)
		testStep=new TE_RConnectionUPSTest18();
	else if(aStepName.Compare(_L("UPSTest19")) == 0)
		testStep=new TE_RConnectionUPSTest19();
	else if(aStepName.Compare(_L("UPSTest20")) == 0)
		testStep=new TE_RConnectionUPSTest20();
	else if(aStepName.Compare(_L("UPSTest21")) == 0)
		testStep=new TE_RConnectionUPSTest21();
	else if(aStepName.Compare(_L("UPSTest22")) == 0)
		testStep=new TE_RConnectionUPSTest22();
	else if(aStepName.Compare(_L("UPSTest23")) == 0)
		testStep=new TE_RConnectionUPSTest23();
	else if(aStepName.Compare(_L("UPSTest24")) == 0)
		testStep=new TE_RConnectionUPSTest24();
	else if(aStepName.Compare(_L("UPSTest25")) == 0)
		testStep=new TE_RConnectionUPSTest25();
	else if(aStepName.Compare(_L("UPSTest26")) == 0)
		testStep=new TE_RConnectionUPSTest26();
	else if(aStepName.Compare(_L("UPSTest27a")) == 0)
		testStep=new TE_RConnectionUPSTest27a();
	else if(aStepName.Compare(_L("UPSTest27b")) == 0)
		testStep=new TE_RConnectionUPSTest27b();
	else if(aStepName.Compare(_L("UPSTest27c")) == 0)
		testStep=new TE_RConnectionUPSTest27c();
	else if(aStepName.Compare(_L("UPSTest27d")) == 0)
		testStep=new TE_RConnectionUPSTest27d();
	else if(aStepName.Compare(_L("UPSTest28")) == 0)
		testStep=new TE_RConnectionUPSTest28();
	else if(aStepName.Compare(_L("UPSTest29")) == 0)
		testStep=new TE_RConnectionUPSTest29();
	else if(aStepName.Compare(_L("UPSTest30")) == 0)
		testStep=new TE_RConnectionUPSTest30();
	else if(aStepName.Compare(_L("UPSTest31")) == 0)
		testStep=new TE_RConnectionUPSTest31();
	else if(aStepName.Compare(_L("UPSTest32")) == 0)
		testStep=new TE_RConnectionUPSTest32();
	else if(aStepName.Compare(_L("UPSTest33")) == 0)
		testStep=new TE_RConnectionUPSTest33();
	else if(aStepName.Compare(_L("UPSTest34")) == 0)
		testStep=new TE_RConnectionUPSTest34();
    else if(aStepName.Compare(_L("UPSTest35")) == 0)
        testStep=new TE_RConnectionUPSTest35();
	
	return testStep;
	}
			
const TPtrC TE_RConnectionSuiteTestServer::ServerName() const
	{
	// On EKA2, test server runs in its own process.
	// So we arrive at the server name using the exe from which it is loaded.
	// This is useful when doing cap tests, as we create duplicate exe's using setcap then.
	TParsePtrC serverName(RProcess().FileName());
	return serverName.Name();
	}

