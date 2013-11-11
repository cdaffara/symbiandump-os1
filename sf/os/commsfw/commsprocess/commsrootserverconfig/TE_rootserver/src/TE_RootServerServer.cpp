// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This main DLL entry point for the TE_ConServer.dll

// EPOC includes
#include <e32base.h>

// RootServer includes
#include <rsshared.h>

// Test system includes
#include <comms-infras/commsdebugutility.h>
#include "TestStepRootServer.h"
#include "TestAsynchHandler.h"
#include "RootServerTestSection1.h"
#include "RootServerTestSection2.h"
#include "RootServerTestSection3.h"
#include "RootServerTestSection4.h"
#include "RootServerTestSection5.h"
#include "RootServerTestSection6.h"
#include "RootServerTestSection7.h"
#include "RootServerTestSection8.h"

_LIT(KRootServerExe, "c32exe.exe");
_LIT(KRootServerProcessPattern, "c32exe*");

EXPORT_C CTE_RootServerServer* NewRootServerServer( void ) 
//
// NewTestSuiteRootServerL is exported at ordinal 1
// this provides the interface to allow schedule test
// to create instances of this test suite
//
	{

	return new(ELeave) CTE_RootServerServer();
    }

	
	
// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"te_rootserver");
// __EDIT_ME__ - Use your own server class name
CTE_RootServerServer* CTE_RootServerServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	// __EDIT_ME__ new your server class here
	CTE_RootServerServer * server = new (ELeave) CTE_RootServerServer();
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
	CTE_RootServerServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTE_RootServerServer::NewL());
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
CTestStep* CTE_RootServerServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	
	CTestStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	
	
	if(aStepName.Compare(_L("Test1.1")) == 0 )
		testStep = new(ELeave) CRootServerTest1_1(this);
	else if(aStepName.Compare(_L("Test1.2")) == 0 )
	    testStep = new(ELeave) CRootServerTest1_2(this);
	else if(aStepName.Compare(_L("Test2.1")) == 0 )
	    testStep = new(ELeave) CRootServerTest2_1(this);
	else if(aStepName.Compare(_L("Test2.2")) == 0 )
	    testStep = new(ELeave) CRootServerTest2_2(this);
	else if(aStepName.Compare(_L("Test2.3")) == 0 )
	    testStep = new(ELeave) CRootServerTest2_3(this);
	//else if(aStepName.Compare(_L("Test3.1")) == 0 )
	//    testStep = new(ELeave) CRootServerTest3_1(this);
	else if(aStepName.Compare(_L("Test3.2")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_2(this);
	else if(aStepName.Compare(_L("Test3.3")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_3(this);
	else if(aStepName.Compare(_L("Test3.4")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_4(this);
	else if(aStepName.Compare(_L("Test3.5")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_5(this);
	else if(aStepName.Compare(_L("Test3.6")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_6(this);
	else if(aStepName.Compare(_L("Test3.7")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_7(this);
	else if(aStepName.Compare(_L("Test3.8")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_8(this);
	else if(aStepName.Compare(_L("Test3.9")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_9(this);
	else if(aStepName.Compare(_L("Test3.10")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_10(this);
	else if(aStepName.Compare(_L("Test3.11")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_11(this);
	else if(aStepName.Compare(_L("Test3.12")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_12(this);
	else if(aStepName.Compare(_L("Test3.13")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_13(this);
	else if(aStepName.Compare(_L("Test3.14")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_14(this);
	else if(aStepName.Compare(_L("Test3.15")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_15(this);
	else if(aStepName.Compare(_L("Test3.16")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_16(this);
	else if(aStepName.Compare(_L("Test3.17")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_17(this);
	else if(aStepName.Compare(_L("Test3.18")) == 0 )
	    testStep = new(ELeave) CRootServerTest3_18(this);
	else if(aStepName.Compare(_L("Test4.1")) == 0 )
	    testStep = new(ELeave) CRootServerTest4_1(this);
	else if(aStepName.Compare(_L("Test5.1")) == 0 )
	    testStep = new(ELeave) CRootServerTest5_1(this);
	else if(aStepName.Compare(_L("Test5.2")) == 0 )
	    testStep = new(ELeave) CRootServerTest5_2(this);
	else if(aStepName.Compare(_L("Test5.3")) == 0 )
	    testStep = new(ELeave) CRootServerTest5_3(this);
	else if(aStepName.Compare(_L("Test5.4")) == 0 )
	    testStep = new(ELeave) CRootServerTest5_4(this);
	else if(aStepName.Compare(_L("Test5.5")) == 0 )
	    testStep = new(ELeave) CRootServerTest5_5(this);
	else if(aStepName.Compare(_L("Test5.6")) == 0 )
	    testStep = new(ELeave) CRootServerTest5_6(this);
	else if(aStepName.Compare(_L("Test5.7")) == 0 )
	    testStep = new(ELeave) CRootServerTest5_7(this);
	else if(aStepName.Compare(_L("Test5.8")) == 0 )
	    testStep = new(ELeave) CRootServerTest5_8(this);
	else if(aStepName.Compare(_L("Test6.1")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_1(this);
	else if(aStepName.Compare(_L("Test6.2")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_2(this);
	else if(aStepName.Compare(_L("Test6.3")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_3(this);
	else if(aStepName.Compare(_L("Test6.4")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_4(this);
	else if(aStepName.Compare(_L("Test6.5")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_5(this);
	else if(aStepName.Compare(_L("Test6.6")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_6(this);
	else if(aStepName.Compare(_L("Test6.7")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_7(this);
	else if(aStepName.Compare(_L("Test6.8")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_8(this);
	else if(aStepName.Compare(_L("Test6.9")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_9(this);
	else if(aStepName.Compare(_L("Test6.10")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_10(this);
	else if(aStepName.Compare(_L("Test6.11")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_11(this);
	else if(aStepName.Compare(_L("Test6.12")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_12(this);
	else if(aStepName.Compare(_L("Test6.13")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_13(this);
	else if(aStepName.Compare(_L("Test6.14")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_14(this);
	else if(aStepName.Compare(_L("Test6.15")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_15(this);
	else if(aStepName.Compare(_L("Test6.16")) == 0 )
	    testStep = new(ELeave) CRootServerTest6_16(this);
	else if(aStepName.Compare(_L("Test7.1")) == 0 )
	    testStep = new(ELeave) CRootServerTest7_1(this);
	else if(aStepName.Compare(_L("Test7.2")) == 0 )
	    testStep = new(ELeave) CRootServerTest7_2(this);
	else if(aStepName.Compare(_L("Test7.3")) == 0 )
	    testStep = new(ELeave) CRootServerTest7_3(this);
	else if(aStepName.Compare(_L("Test7.4")) == 0 )
	    testStep = new(ELeave) CRootServerTest7_4(this);
	else if(aStepName.Compare(_L("Test8.1")) == 0 )
	    testStep = new(ELeave) CRootServerTest8_1(this);

		
	return testStep;
	}

TInt CTE_RootServerServer::StartRootServer()
/**
 * Start the Root Server and connect the standard session to it
 *
 * @return TInt - An error code
 */
    {    
    TInt err = KErrNone;
    TFindServer findCS(KRootServerName);
    TFullName name;
    RProcess rootServer;

    if (findCS.Next(name) == KErrNone)
		{
		INFO_PRINTF1(_L("Shutting down existing RootServer"));

		// Try to shutdown any existing instance of the rootserver so we can
		// safely change the MBuf pool size before any modules are loaded.
		// A sticky module such as ETel may prevent complete shutdown so we
		// kill the process if necessary.
		if(!ShutdownRootServer())
			{
			INFO_PRINTF1(_L("Killing existing RootServer"));

			// Kill the rootserver process if normal shutdown fails.
			TFindProcess findPS(KRootServerProcessPattern);
			err = findPS.Next(name);
			if(err == KErrNone)
				{
	 			err = rootServer.Open(findPS);
	 			if(err == KErrNone)
	 				{
		 			TRequestStatus status;
					rootServer.Logon(status);
					rootServer.Kill(KErrNone);
					User::WaitForRequest(status);
					
					// Create a timer in case some other entity holds an open handle on the
					// configurator which prevents the kernel from destroying it.  We timeout
					// after one second.
					TAutoClose<RTimer> timer;
					if(timer.iObj.CreateLocal() == KErrNone)
						{
						// Request destruction notification so we know when it is safe to start
						// the process again.
						TRequestStatus destructionStatus;
						rootServer.NotifyDestruction(destructionStatus);
						rootServer.Close();
					
						enum{ KProcessDestructionTimeout = 1000000 };
				  					
						TRequestStatus timerStatus;
						timer.iObj.After(timerStatus, KProcessDestructionTimeout);
						
						// Wait for the process to be destroyed or for the timeout.
						User::WaitForRequest(destructionStatus, timerStatus);
						if(timerStatus.Int() == KRequestPending)
							{
							timer.iObj.Cancel();
							User::WaitForRequest(timerStatus);
							}
						else
							{
							User::CancelMiscNotifier(destructionStatus);
							User::WaitForRequest(destructionStatus);
							
							INFO_PRINTF2(_L("CTestSuiteRootServer::StartRootServer - existing RootServer process has still not been destroyed after %f.0s"), KProcessDestructionTimeout / 1000000.0);
							}
						}
	 				}
				}
			}
		}
	else
		{
	    err = rootServer.Create(KRootServerExe, KNullDesC);
		TRequestStatus status;
	    if (err == KErrNone)
			{
			rootServer.Rendezvous(status);
			rootServer.Resume();
			User::WaitForRequest(status);
			rootServer.Close();
			err = status.Int();
			}
		if(err == KErrNone)
			{
			err = RootSess().Connect();
			}
		}

    return err;
    }


TBool CTE_RootServerServer::ShutdownRootServer()
/**
 * Tell the Root Server to shutdown via its standard session, and validate that it did so.
 *
 * @return TBool - True if shutdown was successful
 */
    {
	TInt err;
	// Some tests close the session
	if(RootSess().Handle() == 0)
		{
		err = RootSess().Connect();
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Failed to reconnect session (err %d) - maybe dead already?"), err);
			return EFalse;
			}
		}

	TRSIter position;
	while((err = RootSess().Shutdown()) == KErrRSModulesStillRunning)
		{
		TCFModuleName moduleName;
		err = RootSess().EnumerateModules(position, moduleName);
		if(err == KErrEof)
			{
			INFO_PRINTF1(_L("RRootServ::Shutdown failed, reporting spurious modules"));
			return EFalse;
			}
		TRSModuleInfo moduleInfo;
		err = RootSess().GetModuleInfo(moduleName, moduleInfo);
		if(err == KErrRSModuleUnknown)
			continue;	// an unload probably just completed
		else if(err != KErrNone)
			{
			INFO_PRINTF1(_L("RRootServ::GetModuleInfo failed during shutdown"));
			return EFalse;
			}
		TBuf<KCFMaxModuleName> wideModuleName;
		wideModuleName.Copy(moduleName);
		INFO_PRINTF3(_L("RRootServ::Shutdown in progress; need to unload %S (state %d)"), &wideModuleName, moduleInfo().iState);
		TRequestStatus status;
		switch(moduleInfo().iState)
			{
			case EStarting:		// treat a starting module as a zombie coz we're in a hurry, and who knows how long that rendezvous could take?
			case EZombie:
				{
				RootSess().UnloadCpm(status, moduleName, EUnGraceful);
				User::WaitForRequest(status);
				if(status != KErrNone && status != KErrRSSuddenDeath && status != KErrRSModuleUnknown)
					{
					INFO_PRINTF1(_L("RRootServ::Shutdown failed, zombie could not be killed"));
					return EFalse;
					}
				break;
				}
			case ERunning:
				{
				RootSess().UnloadCpm(status, moduleName, EGraceful);
				User::WaitForRequest(status);
				if(status.Int() == KErrLocked)
					{
					INFO_PRINTF1(_L("Module is sticky and cannot be unloaded"));
					
					// Move to the next module.
					continue;
					}
				else
					{
				INFO_PRINTF2(_L("UnloadCPM returned  %d"), status.Int());
					}
				break;	// either it worked or it should have become a zombie
				}
			case EStopping:
				{
				// transition states; snooze and try again
				User::After(1000000);
				break;
				}
			default:
				{
				INFO_PRINTF1(_L("RRootServ::Shutdown failed, module in improper states"));
				return EFalse;
				}
			}
			
			// Try to unload this module again.
			position.Reset();
		}
	RootSess().Close();
	RRootServ probeSess;
	TBool res = (probeSess.Connect() != KErrNone);
	if(!res)
		{
		probeSess.Close();
		}

    return res;
    }



// make a version string available for test system 
TPtrC CTE_RootServerServer::GetVersion( void )
	{

#ifdef _DEBUG
	_LIT(KTxtVersion,"1.00 (udeb)");
#else
	_LIT(KTxtVersion,"1.00");
#endif

	return KTxtVersion();
	}


CSelfPopScheduler* CSelfPopScheduler::CreateLC()
	{
	CSelfPopScheduler* self = new(ELeave)CSelfPopScheduler;
	CleanupStack::PushL(self);
	CActiveScheduler::Install(self);
	return self;
	}

CSelfPopScheduler::operator TCleanupItem()
	{
	return TCleanupItem(Cleanup, this);
	}

void CSelfPopScheduler::Cleanup(TAny* aItem)
	{
	CActiveScheduler::Install(NULL);
	delete reinterpret_cast<CSelfPopScheduler*>(aItem);
	}
