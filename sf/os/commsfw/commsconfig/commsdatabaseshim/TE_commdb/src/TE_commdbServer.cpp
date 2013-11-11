//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
//
// This main DLL entry point for the TE_ConServer.dll


// EPOC includes
#include <e32base.h>

// Test system includes
#include "Teststepcommdb.h"
#include "step_001_01.h"
#include "step_002_01.h"
#include "Step_003_xx.h"
#include "step_004_xx.h"
#include "step_005_xx.h"
#include "step_006_xx.h"
#include "Step_007_xx.h"
#include "Step_008_xx.h"
#include "Step_009_xx.h"
#include "Step_010_xx.h"
#include "Step_011_xx.h"
#include "Step_012_xx.h"
#include "Step_013_xx.h"
#include "Step_014_xx.h"
#include "Step_015_xx.h"
#include "Step_016_xx.h"
#include "Step_017_xx.h"
#include "Step_018_xx.h"
#include "Step_019_xx.h"
#include "Step_020_xx.h"
#include "Step_021_xx.h"
#include "Step_022_xx.h"
#include "Step_023_xx.h"
#include "Step_024_xx.h"
#include "Step_025_xx.h"
#include "Step_026_xx.h"
#include "Step_027_xx.h"
#include "Step_028_xx.h"
#include "Step_029_xx.h"
#include "Step_030_xx.h"
#include "Step_031_xx.h"
#include "Step_032_xx.h"
#include "Step_033_xx.h"
#include "Step_034_xx.h"
#include "Step_035_xx.h"
#include "Step_036_xx.h"
#include "Step_037_xx.h"
#include "Step_038_xx.h"
#include "Step_039_xx.h"
#include "Step_040_xx.h"
#include "Step_041_xx.h"
#include "Step_042_xx.h"
#include "Step_043_xx.h"
#include "Step_044_xx.h"
#include "Step_045_xx.h"
#include "Step_046_xx.h"
#include "Step_047_xx.h"
#include "step_051_xx.h"
#include "step_052_xx.h"
#include "step_053_xx.h"
#include "step_054_xx.h"
#include "step_055_xx.h"
#include "step_056_xx.h"
#include "step_057_xx.h"
#include "step_058_xx.h"
#include "step_059_xx.h"
#include "Step_061_xx.h"
#include "Step_062_xx.h"
#include "step_063_xx.h"
#include "Step_064_xx.h"
#include "CommDbPreCopy.h"


EXPORT_C CTE_commdbServer* NewcommdbServer( void ) 
//
// NewTestSuitecommdbL is exported at ordinal 1
// this provides the interface to allow schedule test
// to create instances of this test suite
//
	{

	return new(ELeave) CTE_commdbServer();
    }

	
	
// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"te_commdb");
// __EDIT_ME__ - Use your own server class name
CTE_commdbServer* CTE_commdbServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	// __EDIT_ME__ new your server class here
	CTE_commdbServer * server = new (ELeave) CTE_commdbServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.

	//server->StartL(KServerName); 
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	// __EDIT_ME__ Your server name
	CTE_commdbServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTE_commdbServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

// __EDIT_ME__ - Use your own server class name
CTestStep* CTE_commdbServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
		
	// Ensure that the properties get created; normally the RootServer does this
	CommsDat::CMDBSession* ptrCommsdatSession = NULL;
	TRAPD(result, ptrCommsdatSession = CommsDat::CMDBSession::NewL(CommsDat::CMDBSession::LatestVersion()));
	if (result == KErrNone)
		{
		result = ptrCommsdatSession->PublishProperties();
		delete ptrCommsdatSession;
		}
		
	CTestStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	
	
	if(aStepName.Compare(_L("step_001_01")) == 0 )
		testStep = new(ELeave) CCommDbTest001_01;
	else if(aStepName.Compare(_L("step_001_03")) == 0 )
		testStep = new(ELeave) CCommDbTest001_03;
	else if(aStepName.Compare(_L("step_001_04")) == 0 )
		testStep = new(ELeave) CCommDbTest001_04;
	else if(aStepName.Compare(_L("step_001_05")) == 0 )
		testStep = new(ELeave) CCommDbTest001_05;
	else if(aStepName.Compare(_L("step_001_07")) == 0 )
		testStep = new(ELeave) CCommDbTest001_07;
	else if(aStepName.Compare(_L("step_001_08")) == 0 )
		testStep = new(ELeave) CCommDbTest001_08;	
	else if(aStepName.Compare(_L("step_001_09")) == 0 )
		testStep = new(ELeave) CCommDbTest001_09;
	else if(aStepName.Compare(_L("step_001_10")) == 0 )
		testStep = new(ELeave) CCommDbTest001_10;
	else if(aStepName.Compare(_L("step_002_01")) == 0 )
		testStep = new(ELeave) CCommDbTest002_01;
	else if(aStepName.Compare(_L("step_003_01")) == 0 )
		testStep = new(ELeave) CCommDbTest003_01;
	else if(aStepName.Compare(_L("step_003_02")) == 0 )
		testStep = new(ELeave) CCommDbTest003_02;
	else if(aStepName.Compare(_L("step_003_03")) == 0 )
		testStep = new(ELeave) CCommDbTest003_03;
	else if(aStepName.Compare(_L("step_003_04")) == 0 )
		testStep = new(ELeave) CCommDbTest003_04;
	else if(aStepName.Compare(_L("step_003_05")) == 0 )
		testStep = new(ELeave) CCommDbTest003_05;
	else if(aStepName.Compare(_L("step_003_06")) == 0 )
		testStep = new(ELeave) CCommDbTest003_06;
	else if(aStepName.Compare(_L("step_003_07")) == 0 )
		testStep = new(ELeave) CCommDbTest003_07;
	else if(aStepName.Compare(_L("step_003_08")) == 0 )
		testStep = new(ELeave) CCommDbTest003_08;
	else if(aStepName.Compare(_L("step_004_01")) == 0 )
		testStep = new(ELeave) CCommDbTest004_01;
	else if(aStepName.Compare(_L("step_004_01")) == 0 )
		testStep = new(ELeave) CCommDbTest004_01;
	else if(aStepName.Compare(_L("step_004_02")) == 0 )
		testStep = new(ELeave) CCommDbTest004_02;
	else if(aStepName.Compare(_L("step_004_03")) == 0 )
		testStep = new(ELeave) CCommDbTest004_03;	
	else if(aStepName.Compare(_L("step_004_04")) == 0 )
		testStep = new(ELeave) CCommDbTest004_04;
	else if(aStepName.Compare(_L("step_004_05")) == 0 )
		testStep = new(ELeave) CCommDbTest004_05;
	else if(aStepName.Compare(_L("step_004_06")) == 0 )
		testStep = new(ELeave) CCommDbTest004_06;
	else if(aStepName.Compare(_L("step_005_01")) == 0 )
		testStep = new(ELeave) CCommDbTest005_01;
	else if(aStepName.Compare(_L("step_005_02")) == 0 )
		testStep = new(ELeave) CCommDbTest005_02;
	else if(aStepName.Compare(_L("step_005_03")) == 0 )
		testStep = new(ELeave) CCommDbTest005_03;	
	else if(aStepName.Compare(_L("step_005_04")) == 0 )
		testStep = new(ELeave) CCommDbTest005_04;
	else if(aStepName.Compare(_L("step_006_01")) == 0 )
		testStep = new(ELeave) CCommDbTest006_01;
	else if(aStepName.Compare(_L("step_006_02")) == 0 )
		testStep = new(ELeave) CCommDbTest006_02;
	else if(aStepName.Compare(_L("step_006_03")) == 0 )
		testStep = new(ELeave) CCommDbTest006_03;	
	else if(aStepName.Compare(_L("step_006_04")) == 0 )
		testStep = new(ELeave) CCommDbTest006_04;
	else if(aStepName.Compare(_L("step_006_05")) == 0 )
		testStep = new(ELeave) CCommDbTest006_05;
	else if(aStepName.Compare(_L("step_007_01")) == 0 )
		testStep = new(ELeave) CCommDbTest007_01;
	else if(aStepName.Compare(_L("step_007_02")) == 0 )
		testStep = new(ELeave) CCommDbTest007_02;
	else if(aStepName.Compare(_L("step_007_03")) == 0 )
		testStep = new(ELeave) CCommDbTest007_03;	
	else if(aStepName.Compare(_L("step_007_04")) == 0 )
		testStep = new(ELeave) CCommDbTest007_04;
	else if(aStepName.Compare(_L("step_007_05")) == 0 )
		testStep = new(ELeave) CCommDbTest007_05;
	else if(aStepName.Compare(_L("step_007_06")) == 0 )
		testStep = new(ELeave) CCommDbTest007_06;
	else if(aStepName.Compare(_L("step_007_07")) == 0 )
		testStep = new(ELeave) CCommDbTest007_07;
	else if(aStepName.Compare(_L("step_007_08")) == 0 )
		testStep = new(ELeave) CCommDbTest007_08;	
	else if(aStepName.Compare(_L("step_007_09")) == 0 )
		testStep = new(ELeave) CCommDbTest007_09;
	else if(aStepName.Compare(_L("step_007_10")) == 0 )
		testStep = new(ELeave) CCommDbTest007_10;
	else if(aStepName.Compare(_L("step_007_11")) == 0 )
		testStep = new(ELeave) CCommDbTest007_11;
	else if(aStepName.Compare(_L("step_007_12")) == 0 )
		testStep = new(ELeave) CCommDbTest007_12;	
	else if(aStepName.Compare(_L("step_007_14")) == 0 )
		testStep = new(ELeave) CCommDbTest007_14;
	else if(aStepName.Compare(_L("step_007_15")) == 0 )
		testStep = new(ELeave) CCommDbTest007_15;
	else if(aStepName.Compare(_L("step_007_16")) == 0 )
		testStep = new(ELeave) CCommDbTest007_16;
	else if(aStepName.Compare(_L("step_007_17")) == 0 )
		testStep = new(ELeave) CCommDbTest007_17;
	else if(aStepName.Compare(_L("step_007_18")) == 0 )
		testStep = new(ELeave) CCommDbTest007_18;	
	else if(aStepName.Compare(_L("step_007_19")) == 0 )
		testStep = new(ELeave) CCommDbTest007_19;
	else if(aStepName.Compare(_L("step_007_20")) == 0 )
		testStep = new(ELeave) CCommDbTest007_20;
	else if(aStepName.Compare(_L("step_008_01")) == 0 )
		testStep = new(ELeave) CCommDbTest008_01;
	else if(aStepName.Compare(_L("step_008_02")) == 0 )
		testStep = new(ELeave) CCommDbTest008_02;
	else if(aStepName.Compare(_L("step_008_03")) == 0 )
		testStep = new(ELeave) CCommDbTest008_03;	
	else if(aStepName.Compare(_L("step_008_04")) == 0 )
		testStep = new(ELeave) CCommDbTest008_04;
	else if(aStepName.Compare(_L("step_009_01")) == 0 )
		testStep = new(ELeave) CCommDbTest009_01;
	else if(aStepName.Compare(_L("step_009_02")) == 0 )
		testStep = new(ELeave) CCommDbTest009_02;
	else if(aStepName.Compare(_L("step_009_03")) == 0 )
		testStep = new(ELeave) CCommDbTest009_03;	
	else if(aStepName.Compare(_L("step_009_04")) == 0 )
		testStep = new(ELeave) CCommDbTest009_04;
	else if(aStepName.Compare(_L("step_009_05")) == 0 )
		testStep = new(ELeave) CCommDbTest009_05;
	else if(aStepName.Compare(_L("step_009_06")) == 0 )
		testStep = new(ELeave) CCommDbTest009_06;
	else if(aStepName.Compare(_L("step_009_07")) == 0 )
		testStep = new(ELeave) CCommDbTest009_07;
	else if(aStepName.Compare(_L("step_009_08")) == 0 )
		testStep = new(ELeave) CCommDbTest009_08;	
	else if(aStepName.Compare(_L("step_009_09")) == 0 )
		testStep = new(ELeave) CCommDbTest009_09;
	else if(aStepName.Compare(_L("step_009_10")) == 0 )
		testStep = new(ELeave) CCommDbTest009_10;
	else if(aStepName.Compare(_L("step_009_11")) == 0 )
		testStep = new(ELeave) CCommDbTest009_11;
	else if(aStepName.Compare(_L("step_009_12")) == 0 )
		testStep = new(ELeave) CCommDbTest009_12;	
	else if(aStepName.Compare(_L("step_009_13")) == 0 )
		testStep = new(ELeave) CCommDbTest009_13;
	else if(aStepName.Compare(_L("step_009_14")) == 0 )
		testStep = new(ELeave) CCommDbTest009_14;
	else if(aStepName.Compare(_L("step_009_15")) == 0 )
		testStep = new(ELeave) CCommDbTest009_15;
	else if(aStepName.Compare(_L("step_009_16")) == 0 )
		testStep = new(ELeave) CCommDbTest009_16;
	else if(aStepName.Compare(_L("step_009_17")) == 0 )
		testStep = new(ELeave) CCommDbTest009_17;
	else if(aStepName.Compare(_L("step_009_18")) == 0 )
		testStep = new(ELeave) CCommDbTest009_18;	
	else if(aStepName.Compare(_L("step_009_19")) == 0 )
		testStep = new(ELeave) CCommDbTest009_19;
	else if(aStepName.Compare(_L("step_009_20")) == 0 )
		testStep = new(ELeave) CCommDbTest009_20;
	else if(aStepName.Compare(_L("step_009_21")) == 0 )
		testStep = new(ELeave) CCommDbTest009_21;
	else if(aStepName.Compare(_L("step_010_01")) == 0 )
		testStep = new(ELeave) CCommDbTest010_01;
	else if(aStepName.Compare(_L("step_010_02")) == 0 )
		testStep = new(ELeave) CCommDbTest010_02;
	else if(aStepName.Compare(_L("step_010_03")) == 0 )
		testStep = new(ELeave) CCommDbTest010_03;	
	else if(aStepName.Compare(_L("step_010_04")) == 0 )
		testStep = new(ELeave) CCommDbTest010_04;
	else if(aStepName.Compare(_L("step_010_05")) == 0 )
		testStep = new(ELeave) CCommDbTest010_05;
	else if(aStepName.Compare(_L("step_011_01")) == 0 )
		testStep = new(ELeave) CCommDbTest011_01;
	else if(aStepName.Compare(_L("step_011_02")) == 0 )
		testStep = new(ELeave) CCommDbTest011_02;
	else if(aStepName.Compare(_L("step_011_03")) == 0 )
		testStep = new(ELeave) CCommDbTest011_03;	
	else if(aStepName.Compare(_L("step_011_04")) == 0 )
		testStep = new(ELeave) CCommDbTest011_04;
	else if(aStepName.Compare(_L("step_012_01")) == 0 )
		testStep = new(ELeave) CCommDbTest012_01;
	else if(aStepName.Compare(_L("step_012_02")) == 0 )
		testStep = new(ELeave) CCommDbTest012_02;
	else if(aStepName.Compare(_L("step_012_03")) == 0 )
		testStep = new(ELeave) CCommDbTest012_03;	
	else if(aStepName.Compare(_L("step_012_04")) == 0 )
		testStep = new(ELeave) CCommDbTest012_04;
	else if(aStepName.Compare(_L("step_012_05")) == 0 )
		testStep = new(ELeave) CCommDbTest012_05;
	else if(aStepName.Compare(_L("step_012_06")) == 0 )
		testStep = new(ELeave) CCommDbTest012_06;
	else if(aStepName.Compare(_L("step_012_07")) == 0 )
		testStep = new(ELeave) CCommDbTest012_07;
	else if(aStepName.Compare(_L("step_013_01")) == 0 )
		testStep = new(ELeave) CCommDbTest013_01;
	else if(aStepName.Compare(_L("step_013_02")) == 0 )
		testStep = new(ELeave) CCommDbTest013_02;
	else if(aStepName.Compare(_L("step_013_03")) == 0 )
		testStep = new(ELeave) CCommDbTest013_03;	
	else if(aStepName.Compare(_L("step_013_04")) == 0 )
		testStep = new(ELeave) CCommDbTest013_04;
	else if(aStepName.Compare(_L("step_014_01")) == 0 )
		testStep = new(ELeave) CCommDbTest014_01;
	else if(aStepName.Compare(_L("step_014_02")) == 0 )
		testStep = new(ELeave) CCommDbTest014_02;
	else if(aStepName.Compare(_L("step_014_03")) == 0 )
		testStep = new(ELeave) CCommDbTest014_03;	
	else if(aStepName.Compare(_L("step_014_04")) == 0 )
		testStep = new(ELeave) CCommDbTest014_04;
	else if(aStepName.Compare(_L("step_015_01")) == 0 )
		testStep = new(ELeave) CCommDbTest015_01;
	else if(aStepName.Compare(_L("step_015_02")) == 0 )
		testStep = new(ELeave) CCommDbTest015_02;
	else if(aStepName.Compare(_L("step_015_03")) == 0 )
		testStep = new(ELeave) CCommDbTest015_03;	
	else if(aStepName.Compare(_L("step_015_04")) == 0 )
		testStep = new(ELeave) CCommDbTest015_04;
	else if(aStepName.Compare(_L("step_015_05")) == 0 )
		testStep = new(ELeave) CCommDbTest015_05;
	else if(aStepName.Compare(_L("step_015_06")) == 0 )
		testStep = new(ELeave) CCommDbTest015_06;
	else if(aStepName.Compare(_L("step_015_07")) == 0 )
		testStep = new(ELeave) CCommDbTest015_07;
	else if(aStepName.Compare(_L("step_015_08")) == 0 )
		testStep = new(ELeave) CCommDbTest015_08;
	else if(aStepName.Compare(_L("step_015_09")) == 0 )
		testStep = new(ELeave) CCommDbTest015_09;
	else if(aStepName.Compare(_L("step_015_10")) == 0 )
		testStep = new(ELeave) CCommDbTest015_10;
	else if(aStepName.Compare(_L("step_015_11")) == 0 )
		testStep = new(ELeave) CCommDbTest015_11;
	else if(aStepName.Compare(_L("step_015_12")) == 0 )
		testStep = new(ELeave) CCommDbTest015_12;
	else if(aStepName.Compare(_L("step_015_13")) == 0 )
		testStep = new(ELeave) CCommDbTest015_13;	
	else if(aStepName.Compare(_L("step_015_14")) == 0 )
		testStep = new(ELeave) CCommDbTest015_14;
	else if(aStepName.Compare(_L("step_015_15")) == 0 )
		testStep = new(ELeave) CCommDbTest015_15;
	else if(aStepName.Compare(_L("step_015_16")) == 0 )
		testStep = new(ELeave) CCommDbTest015_16;
	else if(aStepName.Compare(_L("step_016_01")) == 0 )
		testStep = new(ELeave) CCommDbTest016_01;
	else if(aStepName.Compare(_L("step_016_02")) == 0 )
		testStep = new(ELeave) CCommDbTest016_02;
	else if(aStepName.Compare(_L("step_016_04")) == 0 )
		testStep = new(ELeave) CCommDbTest016_04;
	else if(aStepName.Compare(_L("step_016_05")) == 0 )
		testStep = new(ELeave) CCommDbTest016_05;
	else if(aStepName.Compare(_L("step_016_07")) == 0 )
		testStep = new(ELeave) CCommDbTest016_07;
	else if(aStepName.Compare(_L("step_016_08")) == 0 )
		testStep = new(ELeave) CCommDbTest016_08;
	else if(aStepName.Compare(_L("step_016_09")) == 0 )
		testStep = new(ELeave) CCommDbTest016_09;
	else if(aStepName.Compare(_L("step_017_01")) == 0 )
		testStep = new(ELeave) CCommDbTest017_01;
	else if(aStepName.Compare(_L("step_017_02")) == 0 )
		testStep = new(ELeave) CCommDbTest017_02;
	else if(aStepName.Compare(_L("step_017_03")) == 0 )
		testStep = new(ELeave) CCommDbTest017_03;	
	else if(aStepName.Compare(_L("step_017_04")) == 0 )
		testStep = new(ELeave) CCommDbTest017_04;
	else if(aStepName.Compare(_L("step_017_05")) == 0 )
		testStep = new(ELeave) CCommDbTest017_05;
	else if(aStepName.Compare(_L("step_017_06")) == 0 )
		testStep = new(ELeave) CCommDbTest017_06;
	else if(aStepName.Compare(_L("step_017_07")) == 0 )
		testStep = new(ELeave) CCommDbTest017_07;
	else if(aStepName.Compare(_L("step_018_01")) == 0 )
		testStep = new(ELeave) CCommDbTest018_01;
	else if(aStepName.Compare(_L("step_018_02")) == 0 )
		testStep = new(ELeave) CCommDbTest018_02;
	else if(aStepName.Compare(_L("step_018_03")) == 0 )
		testStep = new(ELeave) CCommDbTest018_03;	
	else if(aStepName.Compare(_L("step_018_04")) == 0 )
		testStep = new(ELeave) CCommDbTest018_04;
	else if(aStepName.Compare(_L("step_018_05")) == 0 )
		testStep = new(ELeave) CCommDbTest018_05;
	else if(aStepName.Compare(_L("step_018_06")) == 0 )
		testStep = new(ELeave) CCommDbTest018_06;
	else if(aStepName.Compare(_L("step_018_07")) == 0 )
		testStep = new(ELeave) CCommDbTest018_07;
	else if(aStepName.Compare(_L("step_018_08")) == 0 )
		testStep = new(ELeave) CCommDbTest018_08;
	else if(aStepName.Compare(_L("step_019_01")) == 0 )
		testStep = new(ELeave) CCommDbTest019_01;
	else if(aStepName.Compare(_L("step_019_02")) == 0 )
		testStep = new(ELeave) CCommDbTest019_02;
	else if(aStepName.Compare(_L("step_019_03")) == 0 )
		testStep = new(ELeave) CCommDbTest019_03;	
	else if(aStepName.Compare(_L("step_019_04")) == 0 )
		testStep = new(ELeave) CCommDbTest019_04;
	else if(aStepName.Compare(_L("step_019_05")) == 0 )
		testStep = new(ELeave) CCommDbTest019_05;
	else if(aStepName.Compare(_L("step_019_06")) == 0 )
		testStep = new(ELeave) CCommDbTest019_06;
	else if(aStepName.Compare(_L("step_019_07")) == 0 )
		testStep = new(ELeave) CCommDbTest019_07;
	else if(aStepName.Compare(_L("step_019_08")) == 0 )
		testStep = new(ELeave) CCommDbTest019_08;
	else if(aStepName.Compare(_L("step_019_09")) == 0 )
		testStep = new(ELeave) CCommDbTest019_09;
	else if(aStepName.Compare(_L("step_019_10")) == 0 )
		testStep = new(ELeave) CCommDbTest019_10;
	else if(aStepName.Compare(_L("step_019_11")) == 0 )
		testStep = new(ELeave) CCommDbTest019_11;
	else if(aStepName.Compare(_L("step_019_12")) == 0 )
		testStep = new(ELeave) CCommDbTest019_12;
	else if(aStepName.Compare(_L("step_019_13")) == 0 )
		testStep = new(ELeave) CCommDbTest019_13;	
	else if(aStepName.Compare(_L("step_019_14")) == 0 )
		testStep = new(ELeave) CCommDbTest019_14;
	else if(aStepName.Compare(_L("step_020_01")) == 0 )
		testStep = new(ELeave) CCommDbTest020_01;
	else if(aStepName.Compare(_L("step_020_02")) == 0 )
		testStep = new(ELeave) CCommDbTest020_02;
	else if(aStepName.Compare(_L("step_020_03")) == 0 )
		testStep = new(ELeave) CCommDbTest020_03;	
	else if(aStepName.Compare(_L("step_020_04")) == 0 )
		testStep = new(ELeave) CCommDbTest020_04;
	else if(aStepName.Compare(_L("step_020_05")) == 0 )
		testStep = new(ELeave) CCommDbTest020_05;
	else if(aStepName.Compare(_L("step_020_06")) == 0 )
		testStep = new(ELeave) CCommDbTest020_06;
	else if(aStepName.Compare(_L("step_020_07")) == 0 )
		testStep = new(ELeave) CCommDbTest020_07;
	else if(aStepName.Compare(_L("step_020_08")) == 0 )
		testStep = new(ELeave) CCommDbTest020_08;
	else if(aStepName.Compare(_L("step_021_01")) == 0 )
		testStep = new(ELeave) CCommDbTest021_01;
	else if(aStepName.Compare(_L("step_021_02")) == 0 )
		testStep = new(ELeave) CCommDbTest021_02;
	else if(aStepName.Compare(_L("step_021_03")) == 0 )
		testStep = new(ELeave) CCommDbTest021_03;	
	else if(aStepName.Compare(_L("step_021_04")) == 0 )
		testStep = new(ELeave) CCommDbTest021_04;
	else if(aStepName.Compare(_L("step_021_05")) == 0 )
		testStep = new(ELeave) CCommDbTest021_05;
	else if(aStepName.Compare(_L("step_022_01")) == 0 )
		testStep = new(ELeave) CCommDbTest022_01;
	else if(aStepName.Compare(_L("step_022_02")) == 0 )
		testStep = new(ELeave) CCommDbTest022_02;
	else if(aStepName.Compare(_L("step_022_03")) == 0 )
		testStep = new(ELeave) CCommDbTest022_03;	
	else if(aStepName.Compare(_L("step_022_04")) == 0 )
		testStep = new(ELeave) CCommDbTest022_04;
	else if(aStepName.Compare(_L("step_022_05")) == 0 )
		testStep = new(ELeave) CCommDbTest022_05;
	else if(aStepName.Compare(_L("step_022_06")) == 0 )
		testStep = new(ELeave) CCommDbTest022_06;
	else if(aStepName.Compare(_L("step_022_07")) == 0 )
		testStep = new(ELeave) CCommDbTest022_07;
	else if(aStepName.Compare(_L("step_022_08")) == 0 )
		testStep = new(ELeave) CCommDbTest022_08;
	else if(aStepName.Compare(_L("step_022_09")) == 0 )
		testStep = new(ELeave) CCommDbTest022_09;
	else if(aStepName.Compare(_L("step_022_10")) == 0 )
		testStep = new(ELeave) CCommDbTest022_10;
	else if(aStepName.Compare(_L("step_022_11")) == 0 )
		testStep = new(ELeave) CCommDbTest022_11;
	else if(aStepName.Compare(_L("step_022_12")) == 0 )
		testStep = new(ELeave) CCommDbTest022_12;
	else if(aStepName.Compare(_L("step_022_13")) == 0 )
		testStep = new(ELeave) CCommDbTest022_13;	
	else if(aStepName.Compare(_L("step_022_14")) == 0 )
		testStep = new(ELeave) CCommDbTest022_14;
	else if(aStepName.Compare(_L("step_022_16")) == 0 )
		testStep = new(ELeave) CCommDbTest022_16;
	else if(aStepName.Compare(_L("step_022_17")) == 0 )
		testStep = new(ELeave) CCommDbTest022_17;
	else if(aStepName.Compare(_L("step_022_18")) == 0 )
		testStep = new(ELeave) CCommDbTest022_18;
	else if(aStepName.Compare(_L("step_022_19")) == 0 )
		testStep = new(ELeave) CCommDbTest022_19;
	else if(aStepName.Compare(_L("step_022_20")) == 0 )
		testStep = new(ELeave) CCommDbTest022_20;
	else if(aStepName.Compare(_L("step_022_21")) == 0 )
		testStep = new(ELeave) CCommDbTest022_21;
	else if(aStepName.Compare(_L("step_022_22")) == 0 )
		testStep = new(ELeave) CCommDbTest022_22;
	else if(aStepName.Compare(_L("step_022_23")) == 0 )
		testStep = new(ELeave) CCommDbTest022_23;	
	else if(aStepName.Compare(_L("step_022_24")) == 0 )
		testStep = new(ELeave) CCommDbTest022_24;
	else if(aStepName.Compare(_L("step_022_25")) == 0 )
		testStep = new(ELeave) CCommDbTest022_25;
	else if(aStepName.Compare(_L("step_022_26")) == 0 )
		testStep = new(ELeave) CCommDbTest022_26;
	else if(aStepName.Compare(_L("step_022_27")) == 0 )
		testStep = new(ELeave) CCommDbTest022_27;
	else if(aStepName.Compare(_L("step_022_28")) == 0 )
		testStep = new(ELeave) CCommDbTest022_28;
	else if(aStepName.Compare(_L("step_022_29")) == 0 )
		testStep = new(ELeave) CCommDbTest022_29;
	else if(aStepName.Compare(_L("step_022_30")) == 0 )
		testStep = new(ELeave) CCommDbTest022_30;
	else if(aStepName.Compare(_L("step_023_01")) == 0 )
		testStep = new(ELeave) CCommDbTest023_01;
	else if(aStepName.Compare(_L("step_023_02")) == 0 )
		testStep = new(ELeave) CCommDbTest023_02;
	else if(aStepName.Compare(_L("step_023_03")) == 0 )
		testStep = new(ELeave) CCommDbTest023_03;
	else if(aStepName.Compare(_L("step_023_04")) == 0 )
		testStep = new(ELeave) CCommDbTest023_04;
	else if(aStepName.Compare(_L("step_023_05")) == 0 )
		testStep = new(ELeave) CCommDbTest023_05;
	else if(aStepName.Compare(_L("step_023_06")) == 0 )
		testStep = new(ELeave) CCommDbTest023_06;
	else if(aStepName.Compare(_L("step_024_01")) == 0 )
		testStep = new(ELeave) CCommDbTest024_01;
	else if(aStepName.Compare(_L("step_024_02")) == 0 )
		testStep = new(ELeave) CCommDbTest024_02;
	else if(aStepName.Compare(_L("step_024_03")) == 0 )
		testStep = new(ELeave) CCommDbTest024_03;
	else if(aStepName.Compare(_L("step_024_04")) == 0 )
		testStep = new(ELeave) CCommDbTest024_04;
	else if(aStepName.Compare(_L("step_024_05")) == 0 )
		testStep = new(ELeave) CCommDbTest024_05;
	else if(aStepName.Compare(_L("step_024_06")) == 0 )
		testStep = new(ELeave) CCommDbTest024_06;
	else if(aStepName.Compare(_L("step_024_07")) == 0 )
		testStep = new(ELeave) CCommDbTest024_07;
	else if(aStepName.Compare(_L("step_024_08")) == 0 )
		testStep = new(ELeave) CCommDbTest024_08;
	else if(aStepName.Compare(_L("step_024_09")) == 0 )
		testStep = new(ELeave) CCommDbTest024_09;
	else if(aStepName.Compare(_L("step_025_01")) == 0 )
		testStep = new(ELeave) CCommDbTest025_01;
	else if(aStepName.Compare(_L("step_025_02")) == 0 )
		testStep = new(ELeave) CCommDbTest025_02;
	else if(aStepName.Compare(_L("step_026_01")) == 0 )
		testStep = new(ELeave) CCommDbTest026_01;
	else if(aStepName.Compare(_L("step_026_02")) == 0 )
		testStep = new(ELeave) CCommDbTest026_02;
	else if(aStepName.Compare(_L("step_026_03")) == 0 )
		testStep = new(ELeave) CCommDbTest026_03;
	else if(aStepName.Compare(_L("step_026_04")) == 0 )
		testStep = new(ELeave) CCommDbTest026_04;
	else if(aStepName.Compare(_L("step_026_05")) == 0 )
		testStep = new(ELeave) CCommDbTest026_05;
	else if(aStepName.Compare(_L("step_026_06")) == 0 )
		testStep = new(ELeave) CCommDbTest026_06;
	else if(aStepName.Compare(_L("step_026_07")) == 0 )
		testStep = new(ELeave) CCommDbTest026_07;
	else if(aStepName.Compare(_L("step_026_08")) == 0 )
		testStep = new(ELeave) CCommDbTest026_08;
	else if(aStepName.Compare(_L("step_026_09")) == 0 )
		testStep = new(ELeave) CCommDbTest026_09;
	else if(aStepName.Compare(_L("step_026_10")) == 0 )
		testStep = new(ELeave) CCommDbTest026_10;
	else if(aStepName.Compare(_L("step_026_11")) == 0 )
		testStep = new(ELeave) CCommDbTest026_11;
	else if(aStepName.Compare(_L("step_026_12")) == 0 )
		testStep = new(ELeave) CCommDbTest026_12;
	else if(aStepName.Compare(_L("step_026_13")) == 0 )
		testStep = new(ELeave) CCommDbTest026_13;
	else if(aStepName.Compare(_L("step_026_14")) == 0 )
		testStep = new(ELeave) CCommDbTest026_14;
	else if(aStepName.Compare(_L("step_026_15")) == 0 )
		testStep = new(ELeave) CCommDbTest026_15;
	else if(aStepName.Compare(_L("step_026_16")) == 0 )
		testStep = new(ELeave) CCommDbTest026_16;
	else if(aStepName.Compare(_L("step_027_01")) == 0 )
		testStep = new(ELeave) CCommDbTest027_01;
	else if(aStepName.Compare(_L("step_027_02")) == 0 )
		testStep = new(ELeave) CCommDbTest027_02;
	else if(aStepName.Compare(_L("step_028_01")) == 0 )
		testStep = new(ELeave) CCommDbTest028_01;
	else if(aStepName.Compare(_L("step_028_02")) == 0 )
		testStep = new(ELeave) CCommDbTest028_02;
	else if(aStepName.Compare(_L("step_028_03")) == 0 )
		testStep = new(ELeave) CCommDbTest028_03;
	else if(aStepName.Compare(_L("step_028_04")) == 0 )
		testStep = new(ELeave) CCommDbTest028_04;
	else if(aStepName.Compare(_L("step_028_05")) == 0 )
		testStep = new(ELeave) CCommDbTest028_05;
	else if(aStepName.Compare(_L("step_028_06")) == 0 )
		testStep = new(ELeave) CCommDbTest028_06;
	else if(aStepName.Compare(_L("step_028_07")) == 0 )
		testStep = new(ELeave) CCommDbTest028_07;
	else if(aStepName.Compare(_L("step_029_01")) == 0 )
		testStep = new(ELeave) CCommDbTest029_01;
	else if(aStepName.Compare(_L("step_029_02")) == 0 )
		testStep = new(ELeave) CCommDbTest029_02;
	else if(aStepName.Compare(_L("step_029_03")) == 0 )
		testStep = new(ELeave) CCommDbTest029_03;
	else if(aStepName.Compare(_L("step_029_04")) == 0 )
		testStep = new(ELeave) CCommDbTest029_04;
	else if(aStepName.Compare(_L("step_030_01")) == 0 )
		testStep = new(ELeave) CCommDbTest030_01;
	else if(aStepName.Compare(_L("step_030_02")) == 0 )
		testStep = new(ELeave) CCommDbTest030_02;
	else if(aStepName.Compare(_L("step_030_03")) == 0 )
		testStep = new(ELeave) CCommDbTest030_03 ;
	else if(aStepName.Compare(_L("step_031_01")) == 0 )
		testStep = new(ELeave) CCommDbTest031_01;
	else if(aStepName.Compare(_L("step_031_02")) == 0 )
		testStep = new(ELeave) CCommDbTest031_02;
	else if(aStepName.Compare(_L("step_031_03")) == 0 )
		testStep = new(ELeave) CCommDbTest031_03;
	else if(aStepName.Compare(_L("step_031_04")) == 0 )
		testStep = new(ELeave) CCommDbTest031_04;
	else if(aStepName.Compare(_L("step_031_05")) == 0 )
		testStep = new(ELeave) CCommDbTest031_05;
	else if(aStepName.Compare(_L("step_032_01")) == 0 )
		testStep = new(ELeave) CCommDbTest032_01;
	else if(aStepName.Compare(_L("step_032_02")) == 0 )
		testStep = new(ELeave) CCommDbTest032_02;
	else if(aStepName.Compare(_L("step_032_03")) == 0 )
		testStep = new(ELeave) CCommDbTest032_03;
	else if(aStepName.Compare(_L("step_032_04")) == 0 )
		testStep = new(ELeave) CCommDbTest032_04;
	else if(aStepName.Compare(_L("step_032_05")) == 0 )
		testStep = new(ELeave) CCommDbTest032_05;
	else if(aStepName.Compare(_L("step_033_01")) == 0 )
		testStep = new(ELeave) CCommDbTest033_01;
	else if(aStepName.Compare(_L("step_033_02")) == 0 )
		testStep = new(ELeave) CCommDbTest033_02;
	else if(aStepName.Compare(_L("step_033_03")) == 0 )
		testStep = new(ELeave) CCommDbTest033_03;
	else if(aStepName.Compare(_L("step_033_04")) == 0 )
		testStep = new(ELeave) CCommDbTest033_04;
	else if(aStepName.Compare(_L("step_033_05")) == 0 )
		testStep = new(ELeave) CCommDbTest033_05;
	else if(aStepName.Compare(_L("step_033_06")) == 0 )
		testStep = new(ELeave) CCommDbTest033_06;
	else if(aStepName.Compare(_L("step_033_07")) == 0 )
		testStep = new(ELeave) CCommDbTest033_07;
	else if(aStepName.Compare(_L("step_033_08")) == 0 )
		testStep = new(ELeave) CCommDbTest033_08;
	else if(aStepName.Compare(_L("step_033_09")) == 0 )
		testStep = new(ELeave) CCommDbTest033_09;
	else if(aStepName.Compare(_L("step_034_01")) == 0 )
		testStep = new(ELeave) CCommDbTest034_01;
	else if(aStepName.Compare(_L("step_034_02")) == 0 )
		testStep = new(ELeave) CCommDbTest034_02;
	else if(aStepName.Compare(_L("step_034_03")) == 0 )
		testStep = new(ELeave) CCommDbTest034_03;
	else if(aStepName.Compare(_L("step_034_04")) == 0 )
		testStep = new(ELeave) CCommDbTest034_04;
	else if(aStepName.Compare(_L("step_034_05")) == 0 )
		testStep = new(ELeave) CCommDbTest034_05;
	else if(aStepName.Compare(_L("step_034_06")) == 0 )
		testStep = new(ELeave) CCommDbTest034_06;
	else if(aStepName.Compare(_L("step_035_01")) == 0 )
		testStep = new(ELeave) CCommDbTest035_01;
	else if(aStepName.Compare(_L("step_035_02")) == 0 )
		testStep = new(ELeave) CCommDbTest035_02;
	else if(aStepName.Compare(_L("step_035_03")) == 0 )
		testStep = new(ELeave) CCommDbTest035_03;
	else if(aStepName.Compare(_L("step_035_04")) == 0 )
		testStep = new(ELeave) CCommDbTest035_04;
	else if(aStepName.Compare(_L("step_036_01")) == 0 )
		testStep = new(ELeave) CCommDbTest036_01;
	else if(aStepName.Compare(_L("step_036_02")) == 0 )
		testStep = new(ELeave) CCommDbTest036_02;
	else if(aStepName.Compare(_L("step_036_03")) == 0 )
		testStep = new(ELeave) CCommDbTest036_03;
	else if(aStepName.Compare(_L("step_036_04")) == 0 )
		testStep = new(ELeave) CCommDbTest036_04;
	else if(aStepName.Compare(_L("step_036_05")) == 0 )
		testStep = new(ELeave) CCommDbTest036_05;
	else if(aStepName.Compare(_L("step_036_06")) == 0 )
		testStep = new(ELeave) CCommDbTest036_06;
	else if(aStepName.Compare(_L("step_036_07")) == 0 )
		testStep = new(ELeave) CCommDbTest036_07;
	else if(aStepName.Compare(_L("step_036_08")) == 0 )
		testStep = new(ELeave) CCommDbTest036_08;
	else if(aStepName.Compare(_L("step_036_09")) == 0 )
		testStep = new(ELeave) CCommDbTest036_09;
	else if(aStepName.Compare(_L("step_036_10")) == 0 )
		testStep = new(ELeave) CCommDbTest036_10;
	else if(aStepName.Compare(_L("step_036_11")) == 0 )
		testStep = new(ELeave) CCommDbTest036_11;
	else if(aStepName.Compare(_L("step_036_12")) == 0 )
		testStep = new(ELeave) CCommDbTest036_12;
	else if(aStepName.Compare(_L("step_036_13")) == 0 )
		testStep = new(ELeave) CCommDbTest036_13;
	else if(aStepName.Compare(_L("step_036_14")) == 0 )
		testStep = new(ELeave) CCommDbTest036_14;
	else if(aStepName.Compare(_L("step_036_15")) == 0 )
		testStep = new(ELeave) CCommDbTest036_15;
	else if(aStepName.Compare(_L("step_036_16")) == 0 )
		testStep = new(ELeave) CCommDbTest036_16;
	else if(aStepName.Compare(_L("step_036_17")) == 0 )
		testStep = new(ELeave) CCommDbTest036_17;
	else if(aStepName.Compare(_L("step_036_18")) == 0 )
		testStep = new(ELeave) CCommDbTest036_18;
	else if(aStepName.Compare(_L("step_037_01")) == 0 )
		testStep = new(ELeave) CCommDbTest037_01;
	else if(aStepName.Compare(_L("step_037_02")) == 0 )
		testStep = new(ELeave) CCommDbTest037_02;
	else if(aStepName.Compare(_L("step_037_03")) == 0 )
		testStep = new(ELeave) CCommDbTest037_03;
	else if(aStepName.Compare(_L("step_038_01")) == 0 )
		testStep = new(ELeave) CCommDbTest038_01;
	else if(aStepName.Compare(_L("step_038_02")) == 0 )
		testStep = new(ELeave) CCommDbTest038_02;
	else if(aStepName.Compare(_L("step_038_03")) == 0 )
		testStep = new(ELeave) CCommDbTest038_03;
	else if(aStepName.Compare(_L("step_039_01")) == 0 )
		testStep = new(ELeave) CCommDbTest039_01;
	else if(aStepName.Compare(_L("step_039_02")) == 0 )
		testStep = new(ELeave) CCommDbTest039_02;
	else if(aStepName.Compare(_L("step_040_01")) == 0 )
		testStep = new(ELeave) CCommDbTest040_01;
	else if(aStepName.Compare(_L("step_040_02")) == 0 )
		testStep = new(ELeave) CCommDbTest040_02;
	else if(aStepName.Compare(_L("step_040_03")) == 0 )
		testStep = new(ELeave) CCommDbTest040_03;
	else if(aStepName.Compare(_L("step_041_01")) == 0 )
		testStep = new(ELeave) CCommDbTest041_01;
	else if(aStepName.Compare(_L("step_041_02")) == 0 )
		testStep = new(ELeave) CCommDbTest041_02;
	else if(aStepName.Compare(_L("step_041_03")) == 0 )
		testStep = new(ELeave) CCommDbTest041_03;
	else if(aStepName.Compare(_L("step_041_04")) == 0 )
		testStep = new(ELeave) CCommDbTest041_04;
	else if(aStepName.Compare(_L("step_041_05")) == 0 )
		testStep = new(ELeave) CCommDbTest041_05;
	else if(aStepName.Compare(_L("step_041_06")) == 0 )
		testStep = new(ELeave) CCommDbTest041_06;
	else if(aStepName.Compare(_L("step_041_07")) == 0 )
		testStep = new(ELeave) CCommDbTest041_07;
	else if(aStepName.Compare(_L("step_041_08")) == 0 )
		testStep = new(ELeave) CCommDbTest041_08;
	else if(aStepName.Compare(_L("step_041_09")) == 0 )
		testStep = new(ELeave) CCommDbTest041_09;
	else if(aStepName.Compare(_L("step_041_10")) == 0 )
		testStep = new(ELeave) CCommDbTest041_10;
	else if(aStepName.Compare(_L("step_041_11")) == 0 )
		testStep = new(ELeave) CCommDbTest041_11;
	else if(aStepName.Compare(_L("step_041_12")) == 0 )
		testStep = new(ELeave) CCommDbTest041_12;
	else if(aStepName.Compare(_L("step_041_13")) == 0 )
		testStep = new(ELeave) CCommDbTest041_13;
	else if(aStepName.Compare(_L("step_041_14")) == 0 )
		testStep = new(ELeave) CCommDbTest041_14;
	else if(aStepName.Compare(_L("step_041_15")) == 0 )
		testStep = new(ELeave) CCommDbTest041_15;
	else if(aStepName.Compare(_L("step_041_16")) == 0 )
		testStep = new(ELeave) CCommDbTest041_16;
	else if(aStepName.Compare(_L("step_041_17")) == 0 )
		testStep = new(ELeave) CCommDbTest041_17;
	else if(aStepName.Compare(_L("step_041_18")) == 0 )
		testStep = new(ELeave) CCommDbTest041_18;
	else if(aStepName.Compare(_L("step_041_19")) == 0 )
		testStep = new(ELeave) CCommDbTest041_19;
	else if(aStepName.Compare(_L("step_041_20")) == 0 )
		testStep = new(ELeave) CCommDbTest041_20;
	else if(aStepName.Compare(_L("step_041_21")) == 0 )
		testStep = new(ELeave) CCommDbTest041_21;
	else if(aStepName.Compare(_L("step_041_22")) == 0 )
		testStep = new(ELeave) CCommDbTest041_22;
	else if(aStepName.Compare(_L("step_041_23")) == 0 )
		testStep = new(ELeave) CCommDbTest041_23;
	else if(aStepName.Compare(_L("step_041_24")) == 0 )
		testStep = new(ELeave) CCommDbTest041_24;
	else if(aStepName.Compare(_L("step_041_25")) == 0 )
		testStep = new(ELeave) CCommDbTest041_25;
	else if(aStepName.Compare(_L("step_041_26")) == 0 )
		testStep = new(ELeave) CCommDbTest041_26;
	else if(aStepName.Compare(_L("step_041_27")) == 0 )
		testStep = new(ELeave) CCommDbTest041_27;
	else if(aStepName.Compare(_L("step_041_28")) == 0 )
		testStep = new(ELeave) CCommDbTest041_28;
	else if(aStepName.Compare(_L("step_041_29")) == 0 )
		testStep = new(ELeave) CCommDbTest041_29;
	else if(aStepName.Compare(_L("step_041_30")) == 0 )
		testStep = new(ELeave) CCommDbTest041_30;
	else if(aStepName.Compare(_L("step_041_31")) == 0 )
		testStep = new(ELeave) CCommDbTest041_31;
	else if(aStepName.Compare(_L("step_041_32")) == 0 )
		testStep = new(ELeave) CCommDbTest041_32;
	else if(aStepName.Compare(_L("step_041_33")) == 0 )
		testStep = new(ELeave) CCommDbTest041_33;
	else if(aStepName.Compare(_L("step_041_34")) == 0 )
		testStep = new(ELeave) CCommDbTest041_34;
	else if(aStepName.Compare(_L("step_042_01")) == 0 )
		testStep = new(ELeave) CCommDbTest042_01;
	else if(aStepName.Compare(_L("step_042_02")) == 0 )
		testStep = new(ELeave) CCommDbTest042_02;
	else if(aStepName.Compare(_L("step_042_03")) == 0 )
		testStep = new(ELeave) CCommDbTest042_03;
	else if(aStepName.Compare(_L("step_043_01")) == 0 )
		testStep = new(ELeave) CCommDbTest043_01;
	else if(aStepName.Compare(_L("step_043_02")) == 0 )
		testStep = new(ELeave) CCommDbTest043_02;
	else if(aStepName.Compare(_L("step_043_03")) == 0 )
		testStep = new(ELeave) CCommDbTest043_03;
	else if(aStepName.Compare(_L("step_043_04")) == 0 )
		testStep = new(ELeave) CCommDbTest043_04;
	else if(aStepName.Compare(_L("step_043_05")) == 0 )
		testStep = new(ELeave) CCommDbTest043_05;
	else if(aStepName.Compare(_L("step_044_01")) == 0 )
		testStep = new(ELeave) CCommDbTest044_01;
	else if(aStepName.Compare(_L("step_044_02")) == 0 )
		testStep = new(ELeave) CCommDbTest044_02;
	else if(aStepName.Compare(_L("step_044_03")) == 0 )
		testStep = new(ELeave) CCommDbTest044_03;
	else if(aStepName.Compare(_L("step_045_01")) == 0 )
		testStep = new(ELeave) CCommDbTest045_01;
	else if(aStepName.Compare(_L("step_045_02")) == 0 )
		testStep = new(ELeave) CCommDbTest045_02;
	else if(aStepName.Compare(_L("step_045_03")) == 0 )
		testStep = new(ELeave) CCommDbTest045_03;
	else if(aStepName.Compare(_L("step_046_01")) == 0 )
		testStep = new(ELeave) CCommDbTest046_01;
	else if(aStepName.Compare(_L("step_046_02")) == 0 )
		testStep = new(ELeave) CCommDbTest046_02;
	else if(aStepName.Compare(_L("step_046_03")) == 0 )
		testStep = new(ELeave) CCommDbTest046_03;
	else if(aStepName.Compare(_L("step_046_04")) == 0 )
		testStep = new(ELeave) CCommDbTest046_04;
	else if(aStepName.Compare(_L("step_046_05")) == 0 )
		testStep = new(ELeave) CCommDbTest046_05;
	else if(aStepName.Compare(_L("step_047_01")) == 0 )
		testStep = new(ELeave) CCommDbTest047_01;
	else if(aStepName.Compare(_L("step_047_02")) == 0 )
		testStep = new(ELeave) CCommDbTest047_02;
	else if(aStepName.Compare(_L("step_047_03")) == 0 )
		testStep = new(ELeave) CCommDbTest047_03;
	else if(aStepName.Compare(_L("step_047_04")) == 0 )
		testStep = new(ELeave) CCommDbTest047_04;
	else if(aStepName.Compare(_L("step_047_05")) == 0 )
		testStep = new(ELeave) CCommDbTest047_05;
	else if(aStepName.Compare(_L("step_047_06")) == 0 )
		testStep = new(ELeave) CCommDbTest047_06;
	else if(aStepName.Compare(_L("step_047_07")) == 0 )
		testStep = new(ELeave) CCommDbTest047_07;
	else if(aStepName.Compare(_L("step_047_08")) == 0 )
		testStep = new(ELeave) CCommDbTest047_08;
	else if(aStepName.Compare(_L("step_047_13")) == 0 )
		testStep = new(ELeave) CCommDbTest047_13;
	else if(aStepName.Compare(_L("step_051_01")) == 0 )
		testStep = new(ELeave) CCommDbTest051_01;
	else if(aStepName.Compare(_L("step_051_02")) == 0 )
		testStep = new(ELeave) CCommDbTest051_02;
	else if(aStepName.Compare(_L("step_052_01")) == 0 )
		testStep = new(ELeave) CCommDbTest052_01;
	else if(aStepName.Compare(_L("step_052_02")) == 0 )
		testStep = new(ELeave) CCommDbTest052_02;
	else if(aStepName.Compare(_L("step_053_01")) == 0 )
		testStep = new(ELeave) CCommDbTest053_01;
	else if(aStepName.Compare(_L("step_053_02")) == 0 )
		testStep = new(ELeave) CCommDbTest053_02;
	else if(aStepName.Compare(_L("step_054_01")) == 0 )
		testStep = new(ELeave) CCommDbTest054_01;
	else if(aStepName.Compare(_L("step_054_02")) == 0 )
		testStep = new(ELeave) CCommDbTest054_02;
	else if(aStepName.Compare(_L("step_055_01")) == 0 )
		testStep = new(ELeave) CCommDbTest055_01;
	else if(aStepName.Compare(_L("step_055_02")) == 0 )
		testStep = new(ELeave) CCommDbTest055_02;
	else if(aStepName.Compare(_L("step_056_01")) == 0 )
		testStep = new(ELeave) CCommDbTest056_01;
	else if(aStepName.Compare(_L("step_056_02")) == 0 )
		testStep = new(ELeave) CCommDbTest056_02;
	else if(aStepName.Compare(_L("step_057_01")) == 0 )
		testStep = new(ELeave) CCommDbTest057_01;
	else if(aStepName.Compare(_L("step_057_02")) == 0 )
		testStep = new(ELeave) CCommDbTest057_02;
	else if(aStepName.Compare(_L("step_058_01")) == 0 )
		testStep = new(ELeave) CCommDbTest058_01;
	else if(aStepName.Compare(_L("step_058_02")) == 0 )
		testStep = new(ELeave) CCommDbTest058_02;
	else if(aStepName.Compare(_L("step_059_01")) == 0 )
		testStep = new(ELeave) CCommDbTest059_01;
	else if(aStepName.Compare(_L("step_060_01")) == 0 )
		testStep = new(ELeave) CCommDbTest060_01;
	else if(aStepName.Compare(_L("step_060_02")) == 0 )
		testStep = new(ELeave) CCommDbTest060_02;
	else if(aStepName.Compare(_L("step_061_01")) == 0 )
		testStep = new(ELeave) CCommDbTest061_01;
	else if(aStepName.Compare(_L("step_061_02")) == 0 )
		testStep = new(ELeave) CCommDbTest061_02;
	else if(aStepName.Compare(_L("step_061_03")) == 0 )
		testStep = new(ELeave) CCommDbTest061_03;
	else if(aStepName.Compare(_L("step_061_04")) == 0 )
		testStep = new(ELeave) CCommDbTest061_04;
	else if(aStepName.Compare(_L("step_062_01")) == 0 )
		testStep = new(ELeave) CCommDbTest062_01;
	else if(aStepName.Compare(_L("step_062_02")) == 0 )
		testStep = new(ELeave) CCommDbTest062_02;
	else if(aStepName.Compare(_L("step_062_03")) == 0 )
		testStep = new(ELeave) CCommDbTest062_03;
	else if(aStepName.Compare(_L("step_062_04")) == 0 )
		testStep = new(ELeave) CCommDbTest062_04;
	else if(aStepName.Compare(_L("step_062_05")) == 0 )
		testStep = new(ELeave) CCommDbTest062_05;
	else if(aStepName.Compare(_L("step_063_01")) == 0 )
		testStep = new(ELeave) CCommDbTest063_01;
	else if(aStepName.Compare(_L("step_063_02")) == 0 )
		testStep = new(ELeave) CCommDbTest063_02;
	else if(aStepName.Compare(_L("step_063_03")) == 0 )
		testStep = new(ELeave) CCommDbTest063_03;
	else if(aStepName.Compare(_L("step_063_04")) == 0 )
		testStep = new(ELeave) CCommDbTest063_04;
	else if(aStepName.Compare(_L("step_063_05")) == 0 )
		testStep = new(ELeave) CCommDbTest063_05;
	else if(aStepName.Compare(_L("step_063_06")) == 0 )
		testStep = new(ELeave) CCommDbTest063_06;
	else if(aStepName.Compare(_L("step_063_07")) == 0 )
		testStep = new(ELeave) CCommDbTest063_07;
	else if(aStepName.Compare(_L("step_063_08")) == 0 )
		testStep = new(ELeave) CCommDbTest063_08;
	else if(aStepName.Compare(_L("step_064_01")) == 0 )
		testStep = new(ELeave) CCommDbTest064_01;
	else if(aStepName.Compare(_L("PreCopyTest")) == 0 )
		testStep = new(ELeave) CCommDbPreCopy;
	
	return testStep;
	}



// make a version string available for test system 
TPtrC CTE_commdbServer::GetVersion( void )
	{

#ifdef _DEBUG
_LIT(KTxtVersion,"1.0 (udeb)");
#else
_LIT(KTxtVersion,"1.0");
#endif

	return KTxtVersion();
	}


