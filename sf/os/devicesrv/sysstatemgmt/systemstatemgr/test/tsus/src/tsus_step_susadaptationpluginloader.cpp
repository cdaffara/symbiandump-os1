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

#include "susadaptationpluginloader.h"
#include "tsus_step_susadaptationpluginloader.h"

//
// TestStep
//

CSsmSupAdaptationPluginLoaderTest::~CSsmSupAdaptationPluginLoaderTest()
	{
	}

CSsmSupAdaptationPluginLoaderTest::CSsmSupAdaptationPluginLoaderTest()
	{
	SetTestStepName(KTSsmSupAdaptationPluginLoaderStep);
	}

/** */
TVerdict CSsmSupAdaptationPluginLoaderTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmSupAdaptationPluginLoaderTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSupAdaptationPluginLoaderTest tests started...."));

	__UHEAP_MARK;
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	TRAPD(err,doTestCreateAdaptationPluginL());
	TEST(KErrNone == err);

	CleanupStack::PopAndDestroy(sched); 
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("CSsmSupAdaptationPluginLoaderTest tests completed!"));
	return TestStepResult();
	}

/** */
TVerdict CSsmSupAdaptationPluginLoaderTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

void CSsmSupAdaptationPluginLoaderTest::doTestCreateAdaptationPluginL()
	{
	INFO_PRINTF1(_L("doTestCreateAdaptationPluginL!"));
	TInt threadHandles_Before;
  	TInt processHandles_Before;
  	RThread().HandleCount(processHandles_Before, threadHandles_Before);
  	
  	CSusAdaptationPluginLoader* susAdaptationPluginLoader = CSusAdaptationPluginLoader::NewL();

  	INFO_PRINTF1(_L("Create state adapatation plugin"));
 	MStateAdaptation* stateAdaptation = susAdaptationPluginLoader->CreateStateAdaptationL();
 	stateAdaptation->Release();

 	INFO_PRINTF1(_L("Create emergency adapatation plugin"));
 	MEmergencyCallRfAdaptation* emergencyCallRfAdaptation = susAdaptationPluginLoader->CreateEmergencyCallRfAdaptationL();
 	emergencyCallRfAdaptation->Release();

 	INFO_PRINTF1(_L("Create sim adapatation plugin"));
 	MSimAdaptation* simAdaptation = susAdaptationPluginLoader->CreateSimAdaptationL();
 	simAdaptation->Release();

 	INFO_PRINTF1(_L("Create misc adapatation plugin"));
 	MMiscAdaptation* miscAdaptation = susAdaptationPluginLoader->CreateMiscAdaptationL();
 	miscAdaptation->Release();

 	INFO_PRINTF1(_L("Create RTC adapatation plugin"));
 	MRtcAdaptation* rtcAdaptation = susAdaptationPluginLoader->CreateRtcAdaptationL();
 	rtcAdaptation->Release();

	delete susAdaptationPluginLoader;

	TInt threadHandles_After;
  	TInt processHandles_After;
  	RThread().HandleCount(processHandles_After, threadHandles_After);
  	
  	INFO_PRINTF3(_L("Handles in thread: expected =  %d actual = %d"), threadHandles_Before, threadHandles_After);
  	INFO_PRINTF3(_L("Handles in process: expected = %d actual = %d"), processHandles_Before, processHandles_After);
  	
  	TEST(threadHandles_Before == threadHandles_After);
  	TEST(processHandles_Before == processHandles_After);
	}
