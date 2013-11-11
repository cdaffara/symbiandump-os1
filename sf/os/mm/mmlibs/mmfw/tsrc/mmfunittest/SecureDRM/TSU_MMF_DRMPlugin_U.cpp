// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include <e32std.h>
#include <s32mem.h>

// Test system includes
#include <testframework.h>
#include <mmf/server/mmfdrmpluginserverproxy.h>
#include <caf/caf.h>

#include "TSU_MMF_DRMPlugin_U.h"
#include "TSU_MMF_DRMPluginSuite_U.h"

using namespace ContentAccess;

//_LIT(KTestControllerSupplier, "TSU_MMF_DRMPlugin");
_LIT(KDelayInSecondInfo, "DelayInSecond");
_LIT(KDelaySmallerThanShutdown, "smallerthan_drmserver_shutdown");
_LIT(KDelayLargerThanShutdown, "largerthan_drmserver_shutdown");
const TInt KOneMbBaseByte = 1048576;
const TInt KOneSecBaseMicroSec = 1000000;

/**
 *
 * CTestStep_MMF_DRMPlugin constructor
 *
 * @xxxx
 * 
 */
CTestStep_MMF_DRMPlugin::CTestStep_MMF_DRMPlugin() 
	{
	}

/**
 *
 * CTestStep_MMF_DRMPlugin destructor
 *
 * @xxxx
 * 
 */
CTestStep_MMF_DRMPlugin::~CTestStep_MMF_DRMPlugin()
	{
	}

/**
 *
 * CTestStep_MMF_DRMPlugin::DoTestStepPreambleL
 *
 * @xxxx
 * 
 */
TVerdict CTestStep_MMF_DRMPlugin::DoTestStepPreambleL()
	{
	TVerdict verdict = EPass;
	INFO_PRINTF1(_L("Setting UHEAP_MARK"));
	__MM_HEAP_MARK;
	return verdict;
	}

/**
 *
 * CTestStep_MMF_DRMPlugin::DoTestStepPostambleL
 *
 * @xxxx
 * 
 */
TVerdict CTestStep_MMF_DRMPlugin::DoTestStepPostambleL()
	{
	TVerdict verdict = EPass;
	INFO_PRINTF1(_L("Setting UHEAP_MARKEND"));
	__MM_HEAP_MARKEND;
	return verdict;
	}



CTestStep_MMF_DRMPlugin_U_0001::CTestStep_MMF_DRMPlugin_U_0001()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0001");

	
	}


TVerdict CTestStep_MMF_DRMPlugin_U_0001::DoTestStepL( void )
/** 
 * Test the method Open()
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy;
	CleanupClosePushL(drmproxy);
	
	INFO_PRINTF1(_L("Create a session with DRM Plugin Server"));
	TESTL(KErrNone == drmproxy.Open());
	
	CleanupStack::PopAndDestroy(&drmproxy);
	return iTestStepResult;
	}
	

CTestStep_MMF_DRMPlugin_U_0002::CTestStep_MMF_DRMPlugin_U_0002()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0002");

	
	}


TVerdict CTestStep_MMF_DRMPlugin_U_0002::DoTestStepL( void )
/** 
 * Test the method Open() with s1 and s2 RMMFDRMPluginServerProxy objects
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy s1;
	CleanupClosePushL(s1);
	RMMFDRMPluginServerProxy s2;
	CleanupClosePushL(s2);
	
	INFO_PRINTF1(_L("Create 2 sessions with DRM Plugin Server"));
	TESTL(KErrNone == s1.Open());
	TESTL(KErrNone == s2.Open());

	CleanupStack::PopAndDestroy(2,&s1);
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0003::CTestStep_MMF_DRMPlugin_U_0003()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0003");
	}


TVerdict CTestStep_MMF_DRMPlugin_U_0003::DoTestStepL( void )
/** 
 * Test the method LaunchControllerServer() with max heap size = 1Mb and shared heap is disabled.
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy;
	CleanupClosePushL(drmproxy);
	
	INFO_PRINTF1(_L("Create a session with DRM Plugin Server"));
	TESTL(KErrNone == drmproxy.Open());

	TUint heapSize(KOneMbBaseByte);
	TThreadId controllerThreadId;

	INFO_PRINTF1(_L("Launch a controller server through DRM Plugin server"));
	TESTL(KErrNone == drmproxy.LaunchControllerServer(heapSize,EFalse,controllerThreadId, KDefaultStackSize));

	RThread controllerThread;
	CleanupClosePushL(controllerThread);
	
	INFO_PRINTF1(_L("Verify that the controller server is initialized in a new process"));
	TESTL(KErrNone == controllerThread.Open(controllerThreadId));
	
	RProcess owner;
	CleanupClosePushL(owner);
	RProcess currproc(KCurrentProcessHandle);
	CleanupClosePushL(currproc);

	TESTL(KErrNone == controllerThread.Process(owner));

	TESTL(owner.Id() != currproc.Id());
	
	CleanupStack::PopAndDestroy(4,&drmproxy);
	return iTestStepResult;
	}
	
CTestStep_MMF_DRMPlugin_U_0004::CTestStep_MMF_DRMPlugin_U_0004()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0004");
	}


TVerdict CTestStep_MMF_DRMPlugin_U_0004::DoTestStepL( void )
/** 
 * Test the method LaunchControllerServer() with: 1st controller server(cs1) has max heap size = 1Mb 
 * and shared heap is disabled; and 2nd controller server(cs2) has max heap size = 2 Mb and shared heap
 * is disabled.
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	RMMFDRMPluginServerProxy drmproxy2;
	CleanupClosePushL(drmproxy2);
	
	INFO_PRINTF1(_L("Create 2 sessions with DRM Plugin Server"));
	TESTL(KErrNone == drmproxy1.Open());
	TESTL(KErrNone == drmproxy2.Open());

	TUint heapSize(KOneMbBaseByte);
	TThreadId cs1;
	TThreadId cs2;

	INFO_PRINTF1(_L("Launch 2 controller servers through DRM Plugin server"));
	TESTL(KErrNone == drmproxy1.LaunchControllerServer(heapSize,EFalse,cs1,KDefaultStackSize));
	TESTL(KErrNone == drmproxy2.LaunchControllerServer(2*heapSize,EFalse,cs2,KDefaultStackSize));
	
	RThread thread1;
	CleanupClosePushL(thread1);
	RThread thread2;
	CleanupClosePushL(thread2);

	TESTL(KErrNone == thread1.Open(cs1));
	TESTL(KErrNone == thread2.Open(cs2));
	
	RProcess t1proc;
	CleanupClosePushL(t1proc);
	RProcess t2proc;
	CleanupClosePushL(t2proc);
	RProcess currproc(KCurrentProcessHandle);
	CleanupClosePushL(currproc);

	TESTL(KErrNone == thread1.Process(t1proc));
	TESTL(KErrNone == thread2.Process(t2proc));
	
	INFO_PRINTF1(_L("Verify that the controller servers are initialized in different threads"));
	TESTL(cs1 != cs2);

	INFO_PRINTF1(_L("Verify that the controller servers are initialized in same process"));
	TESTL(t1proc.Id() == t2proc.Id());
	
	INFO_PRINTF1(_L("Verify that the controller servers are initialized in a new process"));
	TESTL(t1proc.Id() != currproc.Id());
	
	CleanupStack::PopAndDestroy(7,&drmproxy1);

	return iTestStepResult;
	}
	
CTestStep_MMF_DRMPlugin_U_0005::CTestStep_MMF_DRMPlugin_U_0005()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0005");

	
	}


TVerdict CTestStep_MMF_DRMPlugin_U_0005::DoTestStepL( void )
/** 
 * Test the method GetControllerSessionHandle() after a controller server has been launched successfully.
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy;
	CleanupClosePushL(drmproxy);
	
	TESTL(KErrNone == drmproxy.Open());

	TUint heapSize(KOneMbBaseByte);
	TThreadId controllerThreadId;

	INFO_PRINTF1(_L("Launch a controller server through DRM Plugin server"));
	TESTL(KErrNone == drmproxy.LaunchControllerServer(heapSize,EFalse,controllerThreadId,KDefaultStackSize));

	TInt sessionHandle(0);
	sessionHandle = drmproxy.GetControllerSessionHandle();

	INFO_PRINTF1(_L("Verify that the new controller session handle returned is valid"));
	TESTL(sessionHandle != 0);

	CleanupStack::PopAndDestroy(&drmproxy);

	return iTestStepResult;
	}
	
CTestStep_MMF_DRMPlugin_U_0006::CTestStep_MMF_DRMPlugin_U_0006()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0006");

	
	}


TVerdict CTestStep_MMF_DRMPlugin_U_0006::DoTestStepL( void )
/** 
 * Launch a controller server through a valid DRM Plugin server session.  
 * Record the DRM Plugin server's process Id.  Close DRM Plugin server session and wait for 3 sec.
 * Launch another controller server with another DRM Plugin server session.  Record the process Id 
 * of this DRM Plugin server.
 *
 * Use case: 
 * @test Req. 
 */
	{
	//Session 1	
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	TUint heapSize(KOneMbBaseByte);
	TThreadId cs1;
	RThread thread1;
	CleanupClosePushL(thread1);
	RProcess t1proc;
	CleanupClosePushL(t1proc);
	TProcessId proc1Id;
	
	INFO_PRINTF1(_L("Launch a controller server through DRM Plugin server"));
	TESTL(KErrNone == drmproxy1.Open());
	TESTL(KErrNone == drmproxy1.LaunchControllerServer(heapSize,EFalse,cs1,KDefaultStackSize));
	TESTL(KErrNone == thread1.Open(cs1));
	TESTL(KErrNone == thread1.Process(t1proc));
	
	proc1Id = t1proc.Id();	
	CleanupStack::PopAndDestroy(3,&drmproxy1);

	//Short Delay
	TInt shortDelay;
	GetIntFromConfig(KDelayInSecondInfo, KDelaySmallerThanShutdown, shortDelay);
	User::After(shortDelay*KOneSecBaseMicroSec);
	
	//Session 2
	RMMFDRMPluginServerProxy drmproxy2;
	CleanupClosePushL(drmproxy2);
	TThreadId cs2;
	RThread thread2;
	CleanupClosePushL(thread2);
	RProcess t2proc;
	CleanupClosePushL(t2proc);

	INFO_PRINTF1(_L("Launch another controller server within 5sec shut-down delay"));
	TESTL(KErrNone == drmproxy2.Open());
	TESTL(KErrNone == drmproxy2.LaunchControllerServer(2*heapSize,EFalse,cs2,KDefaultStackSize));
	TESTL(KErrNone == thread2.Open(cs2));
	TESTL(KErrNone == thread2.Process(t2proc));
	
	INFO_PRINTF1(_L("Verify that both controller servers created are located in the same process"));
	TESTL(proc1Id == t2proc.Id());

	CleanupStack::PopAndDestroy(3, &drmproxy2);
		
	return iTestStepResult;
	}
	
CTestStep_MMF_DRMPlugin_U_0007::CTestStep_MMF_DRMPlugin_U_0007()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0007");
	}


TVerdict CTestStep_MMF_DRMPlugin_U_0007::DoTestStepL( void )
/** 
 * Launch a controller server through a valid DRM Plugin server session.  
 * Record the DRM Plugin server's process Id.  Close DRM Plugin server session and wait for 10 sec.  
 * Launch another controller server with another DRM Plugin server session.  
 * Record the process Id of this DRM Plugin server.
 *
 * Use case: 
 * @test Req. 
 */
	{
	//Session 1
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	TUint heapSize(KOneMbBaseByte);
	TThreadId cs1;
	RThread thread1;
	CleanupClosePushL(thread1);
	RProcess t1proc;
	CleanupClosePushL(t1proc);
	TProcessId proc1Id;
	
	INFO_PRINTF1(_L("Launch a controller server through DRM Plugin server"));
	TESTL(KErrNone == drmproxy1.Open());
	TESTL(KErrNone == drmproxy1.LaunchControllerServer(heapSize,EFalse,cs1,KDefaultStackSize));
	TESTL(KErrNone == thread1.Open(cs1));
	TESTL(KErrNone == thread1.Process(t1proc));
	
	proc1Id = t1proc.Id();	
	CleanupStack::PopAndDestroy(3, &drmproxy1);

	//Long Delay
	TInt longDelay;
	GetIntFromConfig(KDelayInSecondInfo, KDelayLargerThanShutdown, longDelay);
	User::After(longDelay*KOneSecBaseMicroSec);
	
	//Session 2
	RMMFDRMPluginServerProxy drmproxy2;
	CleanupClosePushL(drmproxy2);
	TThreadId cs2;
	RThread thread2;
	CleanupClosePushL(thread2);
	RProcess t2proc;
	CleanupClosePushL(t2proc);

	INFO_PRINTF1(_L("Launch another controller server after 5sec shut-down delay"));
	TESTL(KErrNone == drmproxy2.Open());
	TESTL(KErrNone == drmproxy2.LaunchControllerServer(2*heapSize,EFalse,cs2,KDefaultStackSize));
	TESTL(KErrNone == thread2.Open(cs2));
	TESTL(KErrNone == thread2.Process(t2proc));

	INFO_PRINTF1(_L("Verify that both controller servers created are located in the different process"));
	TESTL(proc1Id != t2proc.Id());

	CleanupStack::PopAndDestroy(3, &drmproxy2);

	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0008::CTestStep_MMF_DRMPlugin_U_0008()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0008");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0008::DoTestStepL( void )
/** 
 * Launch a controller server through a valid DRM Plugin server session.  
 * Record the DRM Plugin server's process Id.  Get the controller session 
 * handle and open it in client process.  Close the DRM Plugin server session 
 * (but not the client created session) and wait for 10 sec.  
 * Launch another controller server through a new valid DRM Plugin server session.  
 * Record the process Id the new DRM Plugin server.
 *
 * Use case: 
 * @test Req. 
 */
	{
	TInt err = KErrNone; 
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	
	INFO_PRINTF1(_L("Launch a controller server through DRM Plugin Server"));
	TESTL(KErrNone == drmproxy1.Open());
	TThreadId controllerTId1;
	TESTL(KErrNone == drmproxy1.LaunchControllerServer(KOneMbBaseByte, EFalse, controllerTId1,KDefaultStackSize));
		
	RMMFControllerServerTestProxy testProxy;
	CleanupClosePushL(testProxy);

	INFO_PRINTF1(_L("Keep the valid controller handle session alive"));
	TESTL(KErrNone == testProxy.Open(drmproxy1.GetControllerSessionHandle()));
		
	CleanupStack::Pop(&testProxy);
	CleanupStack::PopAndDestroy(&drmproxy1);	// drmproxy1
	CleanupClosePushL(testProxy);
		
	TInt longDelay;
	GetIntFromConfig(KDelayInSecondInfo, KDelayLargerThanShutdown, longDelay);
	User::After(longDelay*KOneSecBaseMicroSec);
	
	INFO_PRINTF1(_L("Launch another controller server after 10sec"));
	RMMFDRMPluginServerProxy drmproxy2;
	CleanupClosePushL(drmproxy2);
	
	TESTL(KErrNone == drmproxy2.Open());
	TThreadId controllerTId2;
	TESTL(KErrNone == drmproxy2.LaunchControllerServer(KOneMbBaseByte, EFalse, controllerTId2,KDefaultStackSize));
		
	RThread thread1;
	RThread thread2;
	RProcess process1;
	RProcess process2;
	CleanupClosePushL(thread1);
	CleanupClosePushL(thread2);
	CleanupClosePushL(process1);
	CleanupClosePushL(process2);
	TRAP(err,
		// Group into 1 error checking
		User::LeaveIfError(thread1.Open(controllerTId1));
		User::LeaveIfError(thread2.Open(controllerTId2));
		User::LeaveIfError(thread1.Process(process1));
		User::LeaveIfError(thread2.Process(process2));
	);
	
	TESTL(err == KErrNone);
	
	INFO_PRINTF1(_L("Verify that both controller servers are located within the same process"));
	TESTL(process1.Id() == process2.Id());
		
	CleanupStack::PopAndDestroy(4, &thread1);	// process2, process1, thread2, thread1
	CleanupStack::PopAndDestroy(2, &testProxy);	// drmproxy2, testProxy
 
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0009::CTestStep_MMF_DRMPlugin_U_0009()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0009");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0009::DoTestStepL( void )
/** 
 * Launch a controller server through a valid DRM Plugin server session.  
 * Record the DRM Plugin server's process Id.  Get the controller session 
 * handle and open it in client process.  Close the DRM Plugin server session
 * (but not the client created session) and wait for 2 sec.  
 * Close the controller session handle opened in client process as well and wait for 10sec.   
 * Launch another controller server through a new valid DRM Plugin server session.  
 * Record the process Id the new DRM Plugin server
 *.
 * Use case: 
 * @test Req. 
 */
	{
	TInt err = KErrNone; 
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);

	INFO_PRINTF1(_L("Launch a controller server through DRM Plugin Server"));
	TESTL(KErrNone == drmproxy1.Open());
	TThreadId controllerTId1;
	TESTL(KErrNone == drmproxy1.LaunchControllerServer(KOneMbBaseByte, EFalse, controllerTId1,KDefaultStackSize));
		
	RMMFControllerServerTestProxy testProxy;
	CleanupClosePushL(testProxy);
	
	INFO_PRINTF1(_L("Keep the valid controller handle session alive for 2sec and close it"));
	TESTL(KErrNone == testProxy.Open(drmproxy1.GetControllerSessionHandle()));
	CleanupStack::Pop(&testProxy);
	CleanupStack::PopAndDestroy(&drmproxy1);	// drmproxy1
	CleanupClosePushL(testProxy);
		
	TProcessId processId1;
	RThread thread1;
	RProcess process1;
	CleanupClosePushL(thread1);
	CleanupClosePushL(process1);
	
	TRAP(err,
		User::LeaveIfError(thread1.Open(controllerTId1));
		User::LeaveIfError(thread1.Process(process1));
		processId1 = process1.Id();
	);
	TESTL(err == KErrNone);
	CleanupStack::PopAndDestroy(2, &thread1);	// process1, thread1
		
	TInt shortDelay;
	GetIntFromConfig(KDelayInSecondInfo, KDelaySmallerThanShutdown, shortDelay);
	User::After(shortDelay*KOneSecBaseMicroSec);
	CleanupStack::PopAndDestroy(&testProxy);
		
	TInt longDelay;
	GetIntFromConfig(KDelayInSecondInfo, KDelayLargerThanShutdown, longDelay);
	User::After(longDelay*KOneSecBaseMicroSec);
		
	RMMFDRMPluginServerProxy drmproxy2;
	CleanupClosePushL(drmproxy2);
	
	INFO_PRINTF1(_L("Launch another controller server 10sec after closing the last controller session is closed"));
	TESTL(KErrNone == drmproxy2.Open());		
	TThreadId controllerTId2;
	TESTL(KErrNone == drmproxy2.LaunchControllerServer(KOneMbBaseByte, EFalse, controllerTId2,KDefaultStackSize));
		
	RThread thread2;
	RProcess process2;
	CleanupClosePushL(thread2);
	CleanupClosePushL(process2);
	
	TRAP(err,
		// Group into 1 error checking
		User::LeaveIfError(thread2.Open(controllerTId2));
		User::LeaveIfError(thread2.Process(process2));
	);
	TESTL(err == KErrNone);
	INFO_PRINTF1(_L("Verify that both controller servers are located within the different process"));
	TESTL(processId1 != process2.Id());
		
	CleanupStack::PopAndDestroy(2, &thread2);	// process2, thread2
	CleanupStack::PopAndDestroy(&drmproxy2);	// drmproxy2
 
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_ImproperClosing::CTestStep_MMF_DRMPlugin_U_ImproperClosing(
																	const TDesC& aTestName,
																	TImproperClosingType aClosingType,
																	TBool aWithinShutdownDelay)
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iClosingType = aClosingType;
	iWithinShutdownDelay = aWithinShutdownDelay;
	}

TVerdict CTestStep_MMF_DRMPlugin_U_ImproperClosing::DoTestStepL( void )
/** 
 * Closing controller server's thread through Kill() and Close()
 * Behaviour, in client's point-of-view, should be same as calling 
 * controller server session Close()
 *
 * Use case: 
 * @test Req. 
 */
	{
	//Session 1	
	RMMFControllerServerTestProxy testProxy;
	CleanupClosePushL(testProxy);
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	TUint heapSize(KOneMbBaseByte);
	TThreadId cs1;
	RThread thread1;
	CleanupClosePushL(thread1);
	RProcess t1proc;
	CleanupClosePushL(t1proc);
	TProcessId proc1Id;
	
	INFO_PRINTF1(_L("Launch a controller server through DRM Plugin Server"));	
	TESTL(KErrNone == drmproxy1.Open());
	TESTL(KErrNone == drmproxy1.LaunchControllerServer(heapSize,EFalse,cs1,KDefaultStackSize));
	TESTL(KErrNone == thread1.Open(cs1));
	TESTL(KErrNone == thread1.Process(t1proc));
	TESTL(KErrNone == testProxy.Open(drmproxy1.GetControllerSessionHandle()));
	
	proc1Id = t1proc.Id();	
	CleanupStack::PopAndDestroy(3,&drmproxy1);
	
	//Delay 1s
	User::After(KOneSecBaseMicroSec);
	RMMFDRMPluginServerProxy drmproxyTemp;
	CleanupClosePushL(drmproxyTemp);
	TESTL(KErrNone == drmproxyTemp.Open());
	if (iClosingType == ECloseByPanic)
		{
		INFO_PRINTF1(_L("Panic the controller thread"));
		TESTL(KErrNone == drmproxyTemp.PanicControllerThread(cs1, _L("Improper Closing-Panic"), 0));
		}
	else if (iClosingType == ECloseByKill)
		{
		INFO_PRINTF1(_L("Kill the controller thread"));
		TESTL(KErrNone == drmproxyTemp.KillControllerThread(cs1, 0));
		}
	else
		{
		INFO_PRINTF1(_L("Invalid TImproperClosingType type"));
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy(2, &testProxy);	// drmproxyTemp, testProxy

	if (iWithinShutdownDelay)
		{
		//Short Delay
		INFO_PRINTF1(_L("Initialize next controller within DRM Plugin Server shut-down delay"));
		TInt shortDelay;
		GetIntFromConfig(KDelayInSecondInfo, KDelaySmallerThanShutdown, shortDelay);
		User::After(shortDelay*KOneSecBaseMicroSec);
		}
	else
		{
		//Long Delay
		INFO_PRINTF1(_L("Initialize next controller after DRM Plugin Server shut-down delay"));
		TInt longDelay;
		GetIntFromConfig(KDelayInSecondInfo, KDelayLargerThanShutdown, longDelay);
		User::After(longDelay*KOneSecBaseMicroSec);
		}
	
	//Session 2
	RMMFDRMPluginServerProxy drmproxy2;
	CleanupClosePushL(drmproxy2);
	TThreadId cs2;
	RThread thread2;
	CleanupClosePushL(thread2);
	RProcess t2proc;
	CleanupClosePushL(t2proc);

	INFO_PRINTF1(_L("Launch another controller server"));	
	TESTL(KErrNone == drmproxy2.Open());
	TESTL(KErrNone == drmproxy2.LaunchControllerServer(2*heapSize,EFalse,cs2,KDefaultStackSize));
	TESTL(KErrNone == thread2.Open(cs2));
	TESTL(KErrNone == thread2.Process(t2proc));

	if (iWithinShutdownDelay)
		{
		INFO_PRINTF1(_L("Verify that both controller servers are located within the same process"));
		TESTL(proc1Id == t2proc.Id());
		}
	else
		{
		INFO_PRINTF1(_L("Verify that both controller servers are located within the different process"));
		TESTL(proc1Id != t2proc.Id());
		}

	CleanupStack::PopAndDestroy(3, &drmproxy2);
		
	return iTestStepResult;
	}	

CTestStep_MMF_DRMPlugin_U_0014::CTestStep_MMF_DRMPlugin_U_0014()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0014");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0014::DoTestStepL( void )
/** 
 * Launch a controller server through a valid DRM plugin server session. 
 * Record the DRM Plugin server's thread Id. Reset that thread with a new thread 
 * priority.
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	TThreadId controllerTId1;
	TThreadPriority targetPriority = EPriorityAbsoluteHigh;
	
	CleanupClosePushL(drmproxy1);
	INFO_PRINTF1(_L("Launch a controller server through DRM Plugin Server and set its thread priority"));	
	TESTL(KErrNone == drmproxy1.Open());
	TESTL(KErrNone == drmproxy1.LaunchControllerServer(KOneMbBaseByte, EFalse, controllerTId1,KDefaultStackSize));
	TESTL(KErrNone == drmproxy1.SetThreadPriority(controllerTId1, targetPriority));
	
	RThread controllerThread;
	CleanupClosePushL(controllerThread);
	TESTL(KErrNone == controllerThread.Open(controllerTId1));
	INFO_PRINTF1(_L("Verify the priority is set correctly"));
	TESTL(targetPriority == controllerThread.Priority());
	
	CleanupStack::PopAndDestroy(2, &drmproxy1);	// controllerThread, drmproxy1
	return iTestStepResult;
	}

_LIT(KNonprotectedContentFile, "c:\\securedrm\\small.wav");
CTestStep_MMF_DRMPlugin_U_0015::CTestStep_MMF_DRMPlugin_U_0015()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0015");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0015::DoTestStepL( void )
/** 
 * Create a DRM Plugin server session. Open a content through OpenDataContentL() 
 * with file path (c:\securedrm\small.wav) supplied.  NULL for Unique Id, Efalse 
 * for UIEnabled, and Eplay for intent are set in the init data.
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	
	TESTL(KErrNone == drmproxy1.Open());
	
	// Prepare initdata part
	CBufFlat* buf = CBufFlat::NewL(16) ;
	CleanupStack::PushL(buf);
	
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*buf);
	bufStream.WriteInt32L(0);	// 0 length unique Id
	bufStream.WriteInt32L(EFalse);	// EFalse UIEnabled
	TPckgBuf<ContentAccess::TIntent> intentPckg(EPlay);
	bufStream.WriteL(intentPckg);
	CleanupStack::PopAndDestroy(&bufStream);
	
	INFO_PRINTF2(_L("Open content data through RMMFDRMPluginServerProxy: %S"), &KNonprotectedContentFile);
	drmproxy1.OpenDataContentL(KNonprotectedContentFile, buf->Ptr(0));
	
	CleanupStack::PopAndDestroy(2, &drmproxy1);	// buf, drmproxy1 
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0016::CTestStep_MMF_DRMPlugin_U_0016()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0016");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0016::DoTestStepL( void )
/** 
 * Create a DRM Plugin server session. Open a content through OpenDataContentL() 
 * with RFile object (with pathc:\securedrm\small.wav) and corresponding RFs object 
 * has ShareProtected() called.  NULL for Unique Id, Efalse for UIEnabled, and 
 * Eplay for intent are set in the init data.
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	
	TESTL(KErrNone == drmproxy1.Open());
	
	// Prepare initdata part
	CBufFlat* buf = CBufFlat::NewL(16) ;
	CleanupStack::PushL(buf);
	
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*buf);
	bufStream.WriteInt32L(0);	// 0 length unique Id
	bufStream.WriteInt32L(EFalse);	// EFalse UIEnabled
	TPckgBuf<ContentAccess::TIntent> intentPckg(EPlay);
	bufStream.WriteL(intentPckg);
	CleanupStack::PopAndDestroy(&bufStream);
	
	RFs fs;
	CleanupClosePushL(fs);
	TESTL(KErrNone == fs.Connect());
	fs.ShareProtected();
	RFile file;
	CleanupClosePushL(file);
	TESTL(KErrNone == file.Open(fs, KNonprotectedContentFile, EFileShareReadersOrWriters));
	
	INFO_PRINTF2(_L("Open content file handle through RMMFDRMPluginServerProxy: %S"), &KNonprotectedContentFile);
	drmproxy1.OpenDataContentL(file, buf->Ptr(0));
	
	CleanupStack::PopAndDestroy(4, &drmproxy1);	// file, fs, buf, drmproxy1 
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0017::CTestStep_MMF_DRMPlugin_U_0017()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0017");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0017::DoTestStepL( void )
/** 
 * Create a DRM Plugin server session. Open a content through OpenDataContentL() 
 * with file path (c:\securedrm\small.wav) supplied.  NULL for Unique Id, Efalse 
 * for UIEnabled, and Eplay for intent are set in the init data.  Then check for
 * content info.
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	
	TESTL(KErrNone == drmproxy1.Open());
	
	// Prepare initdata part
	CBufFlat* buf = CBufFlat::NewL(16) ;
	CleanupStack::PushL(buf);
	
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*buf);
	bufStream.WriteInt32L(0);	// 0 length unique Id
	bufStream.WriteInt32L(EFalse);	// EFalse UIEnabled
	TPckgBuf<ContentAccess::TIntent> intentPckg(EPlay);
	bufStream.WriteL(intentPckg);
	CleanupStack::PopAndDestroy(&bufStream);
	
	INFO_PRINTF2(_L("Open content data through RMMFDRMPluginServerProxy: %S"), &KNonprotectedContentFile);
	drmproxy1.OpenDataContentL(KNonprotectedContentFile, buf->Ptr(0));
	
	// Check content info
	INFO_PRINTF1(_L("Verify the content intent through RMMFDRMPluginServerProxy: EPlay"));
	TESTL(KErrNone == drmproxy1.EvaluateDataContentIntent(EPlay));
	
	TBuf8<32> mimeType;
	INFO_PRINTF1(_L("Verify the content mime type through RMMFDRMPluginServerProxy: audio/wav"));
	TBool success = drmproxy1.GetDataContentMimeTypeL(mimeType);
	TESTL(success && mimeType == _L8("audio/wav"));
	
	TBuf8<8> header;
	const TUint8 expected[] = {82, 73, 70, 70, 178, 62, 0, 0};	// 8 bytes
	drmproxy1.GetDataContentFileHeaderL(header, header.MaxLength());
	INFO_PRINTF1(_L("Verify the content header through RMMFDRMPluginServerProxy"));
	TESTL(header == TPtrC8(expected, sizeof(expected)));
	
	CleanupStack::PopAndDestroy(2, &drmproxy1);	// buf, drmproxy1 
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0018::CTestStep_MMF_DRMPlugin_U_0018()
	: iHandleOpened(EFalse)
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0018");
	}

CTestStep_MMF_DRMPlugin_U_0018::~CTestStep_MMF_DRMPlugin_U_0018()
	{
	if (iActiveScheduler)
		{
		delete iActiveScheduler;
		}
	if (iFsmNotifier)
		{
		delete iFsmNotifier;
		}
	if (iServer)
		{
		delete iServer;
		}
	if (iThreadMonitor)
		{
		delete iThreadMonitor;
		}
	if (iHandleOpened)
		{
		iControllerServer.Close();
		}
	}
	
TVerdict CTestStep_MMF_DRMPlugin_U_0018::DoTestStepPreambleL()
	{
	TVerdict result = CTestStep_MMF_DRMPlugin::DoTestStepPreambleL();
	
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	
	iFsmNotifier = CFSMNotifier::NewL(this);
	return result;
	}
	
TVerdict CTestStep_MMF_DRMPlugin_U_0018::DoTestStepL( void )
/** 
 * Direct accessing the CMMFDRMPluginServer's functions. Create a valid
 * server object. After some operations, delete the server object.
 * Use case: 
 * @test Req. 
 */
	{
	// create and install the active scheduler we need
	INFO_PRINTF1(_L("Create and install active Scheduler"));
	
	iFsmNotifier->NextState(KOneSecBaseMicroSec);
	CActiveScheduler::Start();
	
	return iTestStepResult;
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0018::DoTestStepPostambleL()
	{
	delete iFsmNotifier;
	iFsmNotifier = NULL;
	
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	
	return CTestStep_MMF_DRMPlugin::DoTestStepPostambleL();
	}
	
void CTestStep_MMF_DRMPlugin_U_0018::StateActivate(TInt aState)
	{
	TInt err = KErrNone;
	switch (aState)
		{
		case EStartServer:
			TRAP(err, DoStartServerL());
			DoHandleStateResult(err);
			break;
		case EInitControllerThread:
			TRAP(err, DoInitControllerThreadL());
			DoHandleStateResult(err);
			break;
		case ECancelThreadMonitor:
			DoCancelThreadMonitor();
			DoHandleStateResult(KErrNone);
			break;
		case EDeleteServer:
			DoDeleteServer();
			DoHandleStateResult(KErrNone);
			break;
		case EStopScheduler:
			CActiveScheduler::Stop();
			break;
		}
	}

void CTestStep_MMF_DRMPlugin_U_0018::DoHandleStateResult(TInt aErr)
	{
	if (aErr)
		{
		INFO_PRINTF3(_L("State #%d exits with error [%d]"), 
						iFsmNotifier->CurrentState(),
						aErr);
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else
		{
		INFO_PRINTF1(_L("Kick start next state"));
		iFsmNotifier->NextState(KOneSecBaseMicroSec);
		}
	}
void CTestStep_MMF_DRMPlugin_U_0018::DoStartServerL()
	{
	iServer = CMMFDRMPluginServer::NewL();
	iServer->SetTimeout(50000000);	// 50 sec server timeout
									// Set a large enough timeout so that CActiveScheduler
									// will not be stopped by the server code, instead of 
									// the code within test unit
	}

void CTestStep_MMF_DRMPlugin_U_0018::DoInitControllerThreadL()
	{
	RThread thisThread;
	iThreadMonitor = CStartAndMonitorControllerThread::NewL(iServer);
	TInt err = iThreadMonitor->StartControllerServer(thisThread, KOneMbBaseByte, EFalse, 
										iControllerServer, iControllerThreadID,KDefaultStackSize);
	User::LeaveIfError(err);
	iHandleOpened = ETrue;					
	}

void CTestStep_MMF_DRMPlugin_U_0018::DoCancelThreadMonitor()
	{
	iThreadMonitor->Cancel();
	}
	
void CTestStep_MMF_DRMPlugin_U_0018::DoDeleteServer()
	{
	delete iThreadMonitor;
	iThreadMonitor = NULL;
	delete iServer;
	iServer = NULL;
	
	iControllerServer.Close();
	iHandleOpened = EFalse;
	}

CTestStep_MMF_DRMPlugin_U_InvalidHeapSize::CTestStep_MMF_DRMPlugin_U_InvalidHeapSize(
																	const TDesC& aTestName,
																	const TDesC& aSectName,
																	const TDesC& aKeyName)
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}
	
TVerdict CTestStep_MMF_DRMPlugin_U_InvalidHeapSize::DoTestStepL( void )
/** 
 * Test the method LaunchControllerServer() with an invalid heap size and shared heap is disabled.
 *
 * Use case: 
 * @test Req. 
 */
	{
	GetIntFromConfig(iSectName, iKeyName, iTestMaxHeapSize);
	INFO_PRINTF2(_L("Launch Controller with MaxHeapSize=%d"), iTestMaxHeapSize);
	
	//TInt err = KErrNone; 
	RMMFDRMPluginServerProxy drmproxy;
	CleanupClosePushL(drmproxy);
	TESTL(KErrNone == drmproxy.Open());
	TThreadId controllerThreadId;
	TESTL(KErrNone == drmproxy.LaunchControllerServer(iTestMaxHeapSize, EFalse, controllerThreadId,KDefaultStackSize));

	RThread controllerThread;
	CleanupClosePushL(controllerThread);
	TESTL(KErrNone == controllerThread.Open(controllerThreadId));
		
	RProcess controllerProcess;
	CleanupClosePushL(controllerProcess);
	TESTL(KErrNone == controllerThread.Process(controllerProcess));
		
	RProcess currproc(KCurrentProcessHandle);
	CleanupClosePushL(currproc);
	
	INFO_PRINTF1(_L("Verify that the controller server process Id is different from test program's"));
	TESTL(controllerProcess.Id() != currproc.Id());

	CleanupStack::PopAndDestroy(4, &drmproxy);	// currproc, controllerProcess, controllerThreadId, drmproxy
	
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0022::CTestStep_MMF_DRMPlugin_U_0022()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0022");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0022::DoTestStepL( void )
/** 
 * Test the method LaunchControllerServer() with max heap size = 1Mb and shared heap is disabled.
 * Execute LaunchControllerServer() again with the same session.
 *
 * Use case: 
 * @test Req. 
 */
	{
	//TInt err = KErrNone; 
	RMMFDRMPluginServerProxy drmproxy;
	CleanupClosePushL(drmproxy);
	
	TESTL(KErrNone == drmproxy.Open());
		
	TThreadId controllerTId1;
	TThreadId controllerTId2;
	
	INFO_PRINTF1(_L("Initialize a second controller server through a RMMFDRMPluginServerProxy and verify that the operation will fail"));
	TESTL(KErrNone == drmproxy.LaunchControllerServer(KOneMbBaseByte, EFalse, controllerTId1,KDefaultStackSize));
	TESTL(KErrAlreadyExists == drmproxy.LaunchControllerServer(KOneMbBaseByte, EFalse, controllerTId2,KDefaultStackSize));
		
	CleanupStack::PopAndDestroy(&drmproxy);	// drmproxy

	return iTestStepResult;
	}


CTestStep_MMF_DRMPlugin_U_0023::CTestStep_MMF_DRMPlugin_U_0023()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0023");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0023::DoTestStepL( void )
/** 
 * Test the method GetControllerSessionHandle() before a controller server has been launched.
 *
 * Use case: 
 * @test Req. 
 */
	{
	//TInt err = KErrNone; 
	RMMFDRMPluginServerProxy drmproxy;
	CleanupClosePushL(drmproxy);
	
	TESTL(KErrNone == drmproxy.Open());
	
	INFO_PRINTF1(_L("Try to get a controller session handle through RMMFDRMPluginServerProxy without \
					a controller server being created and verify that KErrNotReady will be returned"));
	TInt handle = drmproxy.GetControllerSessionHandle();
	TESTL(handle == KErrNotReady);

	CleanupStack::PopAndDestroy(&drmproxy);	// drmproxy
	
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_ImproperClosingInvalidThread::CTestStep_MMF_DRMPlugin_U_ImproperClosingInvalidThread(const TDesC& aTestName,
																									TImproperClosingType aClosingType)
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iClosingType = aClosingType;
	}

TVerdict CTestStep_MMF_DRMPlugin_U_ImproperClosingInvalidThread::DoTestStepL( void )
/** 
 * Closing invalid controller server's thread through Kill() and Close()
 *
 * Use case: 
 * @test Req. 
 */
	{
	//Session 1	
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	TUint heapSize(KOneMbBaseByte);
	TThreadId cs1;
	TThreadId invalidThread(0);
	
	TESTL(KErrNone == drmproxy1.Open());
	TESTL(KErrNone == drmproxy1.LaunchControllerServer(heapSize, EFalse, cs1,KDefaultStackSize));
	
	if (iClosingType == ECloseByPanic)
		{
		INFO_PRINTF1(_L("Try to panic a controller server invalid thread"));
		TESTL(KErrNone  == drmproxy1.PanicControllerThread(invalidThread, _L("Improper Closing Invalid thread-Panic"), 0));
		}
	else if (iClosingType == ECloseByKill)
		{
		INFO_PRINTF1(_L("Try to kill a controller server invalid thread"));
		TESTL(KErrNone == drmproxy1.KillControllerThread(invalidThread, 0));
		}
	else
		{
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy(&drmproxy1);
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0026::CTestStep_MMF_DRMPlugin_U_0026()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0026");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0026::DoTestStepL( void )
/** 
 * Change the priority of a thread (invalid thread Id) that's not belongin to 
 * the DRM Plugin Server.
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	TESTL(KErrNone == drmproxy1.Open());
	
	TThreadId controllerTId1;
	TESTL(KErrNone == drmproxy1.LaunchControllerServer(KOneMbBaseByte, EFalse, controllerTId1,KDefaultStackSize));
	
	RThread controllerThread;
	CleanupClosePushL(controllerThread);
	TESTL(KErrNone == controllerThread.Open(controllerTId1));
	TThreadPriority origPriority = controllerThread.Priority();
	
	TThreadId invalidTId(0);
	TThreadPriority targetPriority = EPriorityAbsoluteHigh;
	INFO_PRINTF1(_L("Try to set the thread priority through RMMFDRMPluginServerProxy with an invalid thread Id"));
	TESTL(KErrNotFound == drmproxy1.SetThreadPriority(invalidTId, targetPriority));
	
	TThreadPriority newPrioity = controllerThread.Priority();
	INFO_PRINTF1(_L("Verify that the valid controller server thread's priority hasn't been changed"));
	TESTL(targetPriority != newPrioity && origPriority == newPrioity);
	
	CleanupStack::PopAndDestroy(2, &drmproxy1);	// controllerThread, drmproxy1
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0027::CTestStep_MMF_DRMPlugin_U_0027()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0027");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0027::DoTestStepL( void )
/** 
 * Retreive content info when a content has not been initialized in DRM Plugin Server
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	TESTL(KErrNone == drmproxy1.Open());
	
	INFO_PRINTF1(_L("Try to evaluate a data content without a content initialized through RMMFDRMPluginServerProxy"));
	TESTL(KErrGeneral == drmproxy1.EvaluateDataContentIntent(EPlay));
	
	CleanupStack::PopAndDestroy(&drmproxy1);	// drmproxy1
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0028::CTestStep_MMF_DRMPlugin_U_0028()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0028");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0028::DoTestStepL( void )
/** 
 * Retreive content info when a content has not been initialized in DRM Plugin Server
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	TESTL(KErrNone == drmproxy1.Open());
	
	TBuf8<16> mimeType;
	INFO_PRINTF1(_L("Try to get mime type of a data content without a content initialized through RMMFDRMPluginServerProxy"));
	TRAPD(err, drmproxy1.GetDataContentMimeTypeL(mimeType));
	TESTL(KErrGeneral == err);
	
	CleanupStack::PopAndDestroy(&drmproxy1);	// drmproxy1
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0029::CTestStep_MMF_DRMPlugin_U_0029()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0029");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0029::DoTestStepL( void )
/** 
 * Retreive content info when a content has not been initialized in DRM Plugin Server
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	TESTL(KErrNone == drmproxy1.Open());
	
	TBuf8<16> header;
	INFO_PRINTF1(_L("Try to get data content without a content initialized through RMMFDRMPluginServerProxy"));
	TRAPD(err, drmproxy1.GetDataContentFileHeaderL(header, 16));
	TESTL(KErrGeneral == err);
	
	CleanupStack::PopAndDestroy(&drmproxy1);	// drmproxy1
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0030::CTestStep_MMF_DRMPlugin_U_0030()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0030");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0030::DoTestStepL( void )
/** 
 * Create a DRM Plugin server session. Open a content through OpenDataContentL() 
 * with file path (c:\securedrm\small.wav) supplied.  NULL for Unique Id, Efalse 
 * for UIEnabled, and Eplay for intent are set in the init data. Then open the
 * content again with the same session.
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	
	TESTL(KErrNone == drmproxy1.Open());
	
	// Prepare initdata part
	CBufFlat* buf = CBufFlat::NewL(16) ;
	CleanupStack::PushL(buf);
	
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*buf);
	bufStream.WriteInt32L(0);	// 0 length unique Id
	bufStream.WriteInt32L(EFalse);	// EFalse UIEnabled
	TPckgBuf<ContentAccess::TIntent> intentPckg(EPlay);
	bufStream.WriteL(intentPckg);
	CleanupStack::PopAndDestroy(&bufStream);
	
	drmproxy1.OpenDataContentL(KNonprotectedContentFile, buf->Ptr(0));
	
	INFO_PRINTF1(_L("Try to open a content through RMMFDRMPluginServerProxy which has a content opened already"));
	TRAPD(err, drmproxy1.OpenDataContentL(KNonprotectedContentFile, buf->Ptr(0)));
	TESTL(err == KErrAlreadyExists);
	
	CleanupStack::PopAndDestroy(2, &drmproxy1);	// buf, drmproxy1 
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0031::CTestStep_MMF_DRMPlugin_U_0031()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0031");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0031::DoTestStepL( void )
/** 
 * Create a DRM Plugin server session. Open a content through OpenDataContentL() 
 * with RFile object (with pathc:\securedrm\small.wav) and corresponding RFs object 
 * has ShareProtected() called.  NULL for Unique Id, Efalse for UIEnabled, and 
 * Eplay for intent are set in the init data. Then open the content again with the 
 * same session.
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	
	TESTL(KErrNone == drmproxy1.Open());
	
	// Prepare initdata part
	CBufFlat* buf = CBufFlat::NewL(16) ;
	CleanupStack::PushL(buf);
	
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*buf);
	bufStream.WriteInt32L(0);	// 0 length unique Id
	bufStream.WriteInt32L(EFalse);	// EFalse UIEnabled
	TPckgBuf<ContentAccess::TIntent> intentPckg(EPlay);
	bufStream.WriteL(intentPckg);
	CleanupStack::PopAndDestroy(&bufStream);
	
	RFs fs;
	CleanupClosePushL(fs);
	TESTL(KErrNone == fs.Connect());
	fs.ShareProtected();
	RFile file;
	CleanupClosePushL(file);
	TESTL(KErrNone == file.Open(fs, KNonprotectedContentFile, EFileShareReadersOrWriters));
	drmproxy1.OpenDataContentL(file, buf->Ptr(0));
	
	INFO_PRINTF1(_L("Try to open a content(RFile) through RMMFDRMPluginServerProxy which has a content opened already"));
	TRAPD(err, drmproxy1.OpenDataContentL(file, buf->Ptr(0)));
	TESTL(err == KErrAlreadyExists);
	
	CleanupStack::PopAndDestroy(4, &drmproxy1);	// file, fs, buf, drmproxy1 
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0032::CTestStep_MMF_DRMPlugin_U_0032()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0032");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0032::DoTestStepL( void )
/** 
 * Create a DRM Plugin server session. Open a content through OpenDataContentL() 
 * with KNullDesC8 as initdata.
 *
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	
	TESTL(KErrNone == drmproxy1.Open());
	
	INFO_PRINTF1(_L("Try to open a content through RMMFDRMPluginServerProxy without NULL initdata"));
	TRAPD(err, drmproxy1.OpenDataContentL(KNonprotectedContentFile, KNullDesC8));
	TESTL(err == KErrEof);
	
	CleanupStack::PopAndDestroy(&drmproxy1);	// drmproxy1 
	return iTestStepResult;
	}
	
CTestStep_MMF_DRMPlugin_U_0033::CTestStep_MMF_DRMPlugin_U_0033()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0033");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0033::DoTestStepL( void )
/** 
 * Create a DRM Plugin server session. Open a content through OpenDataContentL() 
 * with file path (c:\securedrm\small.wav) supplied.  NULL for Unique Id, Efalse 
 * for UIEnabled, and Eplay for intent are set in the init data.  Then check for
 * content info. Call GetDataContentFileHeaderL() which has maximum size larger
 * the header buffer maxium length.
 * Use case: 
 * @test Req. 
 */
	{
	RMMFDRMPluginServerProxy drmproxy1;
	CleanupClosePushL(drmproxy1);
	
	TESTL(KErrNone == drmproxy1.Open());
	
	// Prepare initdata part
	CBufFlat* buf = CBufFlat::NewL(16) ;
	CleanupStack::PushL(buf);
	
	RBufWriteStream bufStream;
	CleanupClosePushL(bufStream);
	bufStream.Open(*buf);
	bufStream.WriteInt32L(0);	// 0 length unique Id
	bufStream.WriteInt32L(EFalse);	// EFalse UIEnabled
	TPckgBuf<ContentAccess::TIntent> intentPckg(EPlay);
	bufStream.WriteL(intentPckg);
	CleanupStack::PopAndDestroy(&bufStream);
	
	drmproxy1.OpenDataContentL(KNonprotectedContentFile, buf->Ptr(0));
	
	INFO_PRINTF1(_L("Try to get header data of a content with max data size if larger than the buffer length"));
	TBuf8<8> header;
	TRAPD(err, drmproxy1.GetDataContentFileHeaderL(header, 16));
	TESTL(err == KErrOverflow);
	
	CleanupStack::PopAndDestroy(2, &drmproxy1);	// buf, drmproxy1 
	return iTestStepResult;
	}

CTestStep_MMF_DRMPlugin_U_0040::CTestStep_MMF_DRMPlugin_U_0040()
	/** Constructor
	*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-DRMPLUGIN-U-0040");
	}

TVerdict CTestStep_MMF_DRMPlugin_U_0040::DoTestStepL( void )
/** 
 * Alloc failure test of Open() function.
 *
 * Use case: 
 * @test Req. 
 */
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	
	// Printing to the console and log file
	INFO_PRINTF1(_L("this is Alloc Failure test of Opening a session with RMMFDRMPluginServerProxy"));

	//__MM_HEAP_MARK;
	TInt longDelay;
	GetIntFromConfig(KDelayInSecondInfo, KDelayLargerThanShutdown, longDelay);
	User::After(longDelay*KOneSecBaseMicroSec);	// Make sure DRM Plugin Server has already shutted-down
	RMMFDRMPluginServerProxy drmproxy;
	err = drmproxy.Open();
	drmproxy.Close();
	//__MM_HEAP_MARKEND;

	if (err != KErrNone)
		return EFail;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse; 
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		//__MM_HEAP_MARK;

		User::After(longDelay);	// Make sure DRM Plugin Server has already shutted-down
		RMMFDRMPluginServerProxy drmproxy1;
		err = drmproxy1.Open();
		
		completed = EFalse;
		if (err == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				{
				User::Free(testAlloc);
				}
			// see if valid result and break if wrong - might be premature result
			TInt err1 = KErrNone;
			err1 = drmproxy1.GetControllerSessionHandle();
			
			if (err1 != KErrNotReady)
				{
				badResult = ETrue;
				}
			drmproxy1.Close();
			completed = reachedEnd || badResult;
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		//__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	//failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.

	if (err != KErrNone || badResult)
		{
		if (badResult)
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
		else
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//------------------------------------------------------------------
// Helper classes and functions

TInt RMMFControllerServerTestProxy::Open(TInt aControllerSessionHandle)
	{
	TInt err = SetReturnedHandle(aControllerSessionHandle);
	return err;
	}

//------------------------------------------------------------------

