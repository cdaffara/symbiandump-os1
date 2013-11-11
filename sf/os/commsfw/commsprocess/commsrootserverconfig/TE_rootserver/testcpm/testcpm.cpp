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
#include <cflog.h>
#include <cfshared.h>
using namespace CommsFW;
#include <cfmsgs.h>
#include <rsshared.h>
#include "RootServerTest.h"

__CFLOG_STMT(_LIT8(KLogTestCPM, "TestCPM");) // subsystem name
_LIT( KTestCpmPanic, "TestCpm" );
const TText8* const KSubModules[] = { _S8("Test Protocol1"), _S8("Test Protocol2"), _S8("Test Protocol3") };


class CDeathTimer : public CTimer
	{
public:
	static CDeathTimer* NewL(TInt aAfter)
		{
		CDeathTimer* self = new(ELeave) CDeathTimer;
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		CActiveScheduler::Add(self);
		self->After(aAfter);
		return self;
		}
protected:
	CDeathTimer() : CTimer(EPriorityStandard)
		{
		}
	void RunL()
		{
		// Go not gracefully into that good night
		User::Invariant();
		}
	};

class CChannelHandler;
class CTestModule : public CBase
	{
public:
	enum {EPriority=1000}; ///< priority of this Active Object
public:
	~CTestModule();
	void CFMessageReceived(TCFMessage& aMsg);
	static CTestModule* NewL(TCFModuleInfo* aStartupArgs);
	void ConstructL(TCFModuleInfo* aStartupArgs);
	void LoggedAfter(TInt aAfterMS, const TDesC8& aDescription);
private:
	CChannelHandler* iChannelHandler;
	TTestModuleIniDataContainer iDelayTimes;
	TInt iDiscoverPos;
	CDeathTimer* iDeathTimer;
	};

class CChannelHandler:public CActive
	{
public:
	static CChannelHandler* NewL(RCFChannel::TMsgQueues aRxQueues, 
									RCFChannel::TMsgQueues aTxQueues, CTestModule* aModule);
	void RunL();
	TInt Send(TCFMessage);
	~CChannelHandler();
protected:
	virtual void DoCancel();
private:
	void ConstructL(RCFChannel::TMsgQueues aRxQueues, 
					RCFChannel::TMsgQueues aTxQueues, CTestModule* aModule);
	CChannelHandler();
private:
	CTestModule* iModule;
	RCFChannelPair iChannelPair;
	};


//
// CChannelHandler class definitions
//

CChannelHandler::CChannelHandler():CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

CChannelHandler* CChannelHandler::NewL(RCFChannel::TMsgQueues aRxQueues, 
									RCFChannel::TMsgQueues aTxQueues, CTestModule* aModule)
	{
	CChannelHandler* pHandler = new (ELeave) CChannelHandler;
	pHandler->ConstructL(aRxQueues, aTxQueues, aModule);
	return pHandler;
	}

void CChannelHandler::ConstructL(RCFChannel::TMsgQueues aRxQueues, 
									RCFChannel::TMsgQueues aTxQueues, CTestModule* aModule)
	{
	iModule = aModule;
	iChannelPair.CreateSend(aTxQueues);
	iChannelPair.CreateRecv(aRxQueues);
	SetActive();
	iChannelPair.NotifyDataAvailable(*this);
	}

void CChannelHandler::DoCancel(void)
	{
	iChannelPair.CancelDataAvailable();
	}

TInt CChannelHandler::Send(TCFMessage aMsg)
	{
	TRequestStatus status;
	iChannelPair.NotifySpaceAvailable(*this);
	User::WaitForRequest(status);
	return iChannelPair.Send(aMsg);
	}

void CChannelHandler::RunL()
	{

	if (iStatus != KErrNone)
		{
		User::Invariant();
		}

	// No error. Take the message from the channel.
	TCFMessage msg;
	TInt err = iChannelPair.Receive(msg);
	if (KErrNone == err)
		{
		//Pass the message
		iModule->CFMessageReceived(msg);
		//Pend again
		SetActive();
		iChannelPair.NotifyDataAvailable(*this);
		}
	else
		{
		User::Invariant();
		}
	}

CChannelHandler::~CChannelHandler()
	{
	if(IsActive())
		Cancel();
	iChannelPair.CloseNonGracefully();
	}	


//
// implementation of CTestServer
//

CTestModule* CTestModule::NewL(TCFModuleInfo* aStartupArgs)
/**
 * Create a new CCommServer.
 *
 * This is not a normal 2 phase contruction. If the allocation of the object
 * fails we will panic with -4 which means that something is very wrong.
 */
	{
	CTestModule* pS;
	CleanupStack::PushL(pS = new CTestModule);
	__ASSERT_ALWAYS(pS!=NULL, User::Panic( KTestCpmPanic, KErrNoMemory));
#ifdef SYMBIAN_C32ROOT_API_V3
	pS->ConstructL(aStartupArgs);
#else
	TRAPD(r, pS->ConstructL(aStartupArgs));
	__ASSERT_ALWAYS(r==KErrNone, User::Panic( KTestCpmPanic, KErrNoMemory));
#endif
	CleanupStack::Pop(pS);
	return pS;
	}


void CTestModule::ConstructL(TCFModuleInfo* aStartupArgs)
	{
	if (aStartupArgs->iIniData != NULL)
		{
		if (aStartupArgs->iIniData->Length() == sizeof(iDelayTimes))
			{
			TTestModuleIniData pkg;
			pkg.Copy(*(aStartupArgs->iIniData));
#if defined(SYMBIAN_C32ROOT_API_V3)
			delete aStartupArgs->iIniData;
#endif

			iDelayTimes = pkg.iParams;

			if(iDelayTimes.iInitDelay == EXIT_BEFORE_RENDEZVOUS)
				User::Leave(KErrGeneral);
			else if(iDelayTimes.iInitDelay > 0)
				LoggedAfter(iDelayTimes.iInitDelay, _L8("Init"));
			else if(iDelayTimes.iInitDelay == PANIC_BEFORE_RENDEZVOUS)
				User::Panic(_L("TESTCPM"), 1);

			__CFLOG_5( KLogTestCPM, KLogCode, 
				_L8("TestCpm[%LU] - iIniData delays (msec): discoveryDelay %d bindDelay %d unbindDelay %d shutdownDelay %d"), 
						 RThread().Id().Id(), iDelayTimes.iDiscoveryDelay, iDelayTimes.iBindDelay,
						 iDelayTimes.iUnbindDelay, iDelayTimes.iShutdownDelay);

			iChannelHandler = CChannelHandler::NewL(aStartupArgs->iRxQueues, aStartupArgs->iTxQueues, this);

			RThread::Rendezvous(KErrNone);

			if(iDelayTimes.iInitDelay == PANIC_AFTER_RENDEZVOUS)
				User::Panic(_L("TESTCPM"), 1);

			}
		else
			{
			__CFLOG_1( KLogTestCPM, KLogCode, _L("TestCpm[%LU] - iIniData mismatched"), RThread().Id().Id() );
#if defined(SYMBIAN_C32ROOT_API_V3)
			delete aStartupArgs->iIniData;
#endif
			ASSERT(0);
			}
		}
	else
		{
		__CFLOG_1( KLogTestCPM, KLogCode, _L("TestCpm[%LU] - No iIniData"), RThread().Id().Id() );
		ASSERT(0);
		}
	}

void CTestModule::CFMessageReceived(TCFMessage& aMsg)
	{
	
	//It is called when a new message arrives from incomming channel
	switch (aMsg.Code())
		{
		case TCFCommsMessage::ECodeDiscover:
			{
			// now wait for delay
			__CFLOG(KLogTestCPM, KLogCode, _L("TestCpm Discover message received"));

			LoggedAfter(iDelayTimes.iDiscoveryDelay, _L8("Discovery"));
			const TCFDiscoverMsg& msg = reinterpret_cast<const TCFDiscoverMsg&>(aMsg);
			// we return our sub-modules one at a time to make sure the RootServer copes with iterative discovery
			const TInt numSubModules = sizeof(KSubModules) / sizeof(KSubModules[0]);
			ASSERT(msg.Size() > 0);
			ASSERT(iDiscoverPos < numSubModules);
			if(msg.Reset())
				iDiscoverPos = 0;
			TCFSubModuleName name;
			const TText8* subMod = KSubModules[iDiscoverPos++];
			name.Copy(subMod);
			msg.SubModuleNames()[0] = name;
			TBool discoveryEnded = (iDiscoverPos == numSubModules);
			TCFDiscoverRespMsg diskresp(msg.Identifier(), 1, !discoveryEnded);
			TInt err = iChannelHandler->Send(diskresp);
			if(KErrNone != err)
				{
       		    User::Invariant();
				}
			if(discoveryEnded && iDelayTimes.iDeathDelay != NO_PREMATURE_DEATH)
				{
				TRAP(err, iDeathTimer = CDeathTimer::NewL(iDelayTimes.iDeathDelay * 1000));
				ASSERT(iDeathTimer);
				}
				
			}
			break;

		case TCFCommsMessage::ECodeBind:
			{
			__CFLOG(KLogTestCPM, KLogCode, _L("TestCpm Bind message received"));
			// now wait for delay
			LoggedAfter(iDelayTimes.iBindDelay, _L8("Bind"));
			const TCFBindMsg& msg = reinterpret_cast<const TCFBindMsg&>(aMsg);
			TCFBindCompleteMsg resp(msg.Identifier(), KErrNone);
			TInt err = iChannelHandler->Send(resp);
			if(KErrNone != err)
				{
				User::Invariant();
				}
			}
			break;

		case TCFCommsMessage::ECodeUnbind: 
			{
			__CFLOG(KLogTestCPM, KLogCode, _L("TestCpm UnBind message received"));
			// now wait for delay
			LoggedAfter(iDelayTimes.iUnbindDelay, _L8("Unbind"));
			const TCFUnbindMsg& msg = reinterpret_cast<const TCFUnbindMsg&>(aMsg);
			TCFUnbindCompleteMsg resp(msg.Identifier(), KErrNone);
			TInt err = iChannelHandler->Send(resp);
			if(KErrNone != err)
				{
				User::Invariant();
				}
			}
			break;

		case TCFCommsMessage::ECodeShutdown:
			// now wait for delay
			__CFLOG(KLogTestCPM, KLogCode, _L("TestCpm Shutdown message received"));
			LoggedAfter(iDelayTimes.iShutdownDelay, _L8("Shutdown"));

			CActiveScheduler::Stop();
			break;
		default:;
			User::Invariant();
		}
	}

	
CTestModule::~CTestModule()
	{
	delete iChannelHandler;
	delete iDeathTimer;
	}

void CTestModule::LoggedAfter(TInt aAfterMS, const TDesC8& aDescription)
	{ 
	(void) aDescription;
	__CFLOG_2(KLogTestCPM, KLogCode, _L8("TestCpm: %d ms %S pause"), aAfterMS, &aDescription);
	User::After(aAfterMS * 1000);
	__CFLOG_1(KLogTestCPM, KLogCode, _L8("TestCpm: %S pause complete"), &aDescription);
	}


//---------------

void DoModuleThreadL(TAny * aArg)
	{
	TCFModuleInfo* args = reinterpret_cast<TCFModuleInfo*>(aArg);
	
	CActiveScheduler*  scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler); 

	CTestModule *pTm = CTestModule::NewL(args);

    __CFLOG_1( KLogTestCPM, KLogCode, _L("TestCpm[%LU] - Completed rendezvous."), RThread().Id().Id() );

	CActiveScheduler::Start();

	__CFLOG_1( KLogTestCPM, KLogCode, _L("TestCpm[%LU] - Exiting thread function"), RThread().Id().Id() );

	delete pTm;
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(scheduler);
	}

EXPORT_C TInt ModuleThread(TAny * aArg)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;	

	TRAPD(ret, DoModuleThreadL(aArg))
	if (ret!=KErrNone)
		{
		__CFLOG(KLogTestCPM, KLogCode, _L("ModuleThread failed to create TestCpm"));
		}

	delete cleanup;
	return ret;
	}


