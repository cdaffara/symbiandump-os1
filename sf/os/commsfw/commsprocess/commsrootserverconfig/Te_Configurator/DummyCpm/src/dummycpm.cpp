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
//

#include <cflog.h>
#include <cfshared.h>
using namespace CommsFW;
#include <cfmsgs.h>
#include <rsshared.h>
#include "RootServerTest.h"
#include <cfextras.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_RootSrvdummycpm, "RootSrvdummycpm.");
#endif

__CFLOG_STMT(_LIT8(KLogDummyCPM, "DummyCPM");) // subsystem name
_LIT( KDummyCpmPanic, "DummyCpm" );
_LIT8(KWorkerIdLabel, "WorkerId");
_LIT8(KInitLabel, "Init");
_LIT8(KDeathLabel, "Death");
_LIT8(KDiscoverLabel, "Discover");
_LIT8(KBindLabel, "Bind");
_LIT8(KUnbindLabel, "Unbind");
_LIT8(KShutdownLabel, "Shutdown");


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
	enum {EPriority=1000}; //< priority of this Active Object
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
	TPtrC8 iWorkerId;
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


CTestModule* CTestModule::NewL(TCFModuleInfo* aStartupArgs)
/**
 * Create a new CTestModule.
 *
 * This is not a normal 2 phase contruction. If the allocation of the object
 * fails we will panic with -4 which means that something is very wrong.
 */
	{
	CTestModule* pS;
	CleanupStack::PushL(pS = new CTestModule);
	__ASSERT_ALWAYS(pS!=NULL, User::Panic( KDummyCpmPanic, KErrNoMemory));
	pS->ConstructL(aStartupArgs);
	CleanupStack::Pop(pS);
	return pS;
	}

void CTestModule::ConstructL(TCFModuleInfo* aStartupArgs)
	{

	if(aStartupArgs->iIniData)
		{
		TPtrC8 value;
		if (!GetVarFromIniData(*(aStartupArgs->iIniData), KNullDesC8(), KInitLabel(), value))
			{
			// bad ini data
			__ASSERT_DEBUG(NULL, User::Panic(KDummyCpmPanic, KErrCorrupt));
			User::Leave(KErrCorrupt);		
			}
		TLex8 lex(value);
		User::LeaveIfError(lex.Val(iDelayTimes.iInitDelay));

		if (!GetVarFromIniData(*(aStartupArgs->iIniData), KNullDesC8(), KDeathLabel(), value))
			{
			// bad ini data
			__ASSERT_DEBUG(1, User::Panic(KDummyCpmPanic, KErrCorrupt));
			User::Leave(KErrCorrupt);		
			}
		lex = value;
		User::LeaveIfError(lex.Val(iDelayTimes.iDeathDelay));

		if (!GetVarFromIniData(*(aStartupArgs->iIniData), KNullDesC8(), KDiscoverLabel(), value))
			{
			// bad ini data
			__ASSERT_DEBUG(1, User::Panic(KDummyCpmPanic, KErrCorrupt));
			User::Leave(KErrCorrupt);		
			}
		lex = value;
		User::LeaveIfError(lex.Val(iDelayTimes.iDiscoveryDelay));

		if (!GetVarFromIniData(*(aStartupArgs->iIniData), KNullDesC8(), KBindLabel(), value))
			{
			// bad ini data
			__ASSERT_DEBUG(1, User::Panic(KDummyCpmPanic, KErrCorrupt));
			User::Leave(KErrCorrupt);		
			}
		lex = value;
		User::LeaveIfError(lex.Val(iDelayTimes.iBindDelay));

		if (!GetVarFromIniData(*(aStartupArgs->iIniData), KNullDesC8(), KUnbindLabel(), value))
			{
			// bad ini data
			__ASSERT_DEBUG(1, User::Panic(KDummyCpmPanic, KErrCorrupt));
			User::Leave(KErrCorrupt);		
			}
		lex = value;
		User::LeaveIfError(lex.Val(iDelayTimes.iUnbindDelay));

		if (!GetVarFromIniData(*(aStartupArgs->iIniData), KNullDesC8(), KShutdownLabel(), value))
			{
			// bad ini data
			__ASSERT_DEBUG(1, User::Panic(KDummyCpmPanic, KErrCorrupt));
			User::Leave(KErrCorrupt);		
			}
		lex = value;
		User::LeaveIfError(lex.Val(iDelayTimes.iShutdownDelay));

		if (!GetVarFromIniData(*(aStartupArgs->iIniData), KNullDesC8(), KWorkerIdLabel(), iWorkerId))
			{
			// bad ini data
			__ASSERT_DEBUG(1, User::Panic(KDummyCpmPanic, KErrCorrupt));
			User::Leave(KErrCorrupt);		
			}

		iChannelHandler = CChannelHandler::NewL(aStartupArgs->iRxQueues, aStartupArgs->iTxQueues, this);
		RThread::Rendezvous(KErrNone);
		}
	}

void CTestModule::CFMessageReceived(TCFMessage& aMsg)
	{
	
	//It is called when a new message arrives from incomming channel
	switch (aMsg.Code())
		{
		case TCFCommsMessage::ECodeDiscover:
			{
			__CFLOG(KLogDummyCPM, KLogCode, _L("DummyCpm Discover message received"));
			// now wait for delay
			LoggedAfter(iDelayTimes.iDiscoveryDelay, _L8("Discovery"));
			const TCFDiscoverMsg& msg = reinterpret_cast<const TCFDiscoverMsg&>(aMsg);
			msg.SubModuleNames()[0] = iWorkerId;	// submodule name as the workerId
			TCFDiscoverRespMsg discresp(msg.Identifier(), 1, EFalse);

			TInt err = iChannelHandler->Send(discresp);
			if(KErrNone != err)
				{
       		    User::Invariant();
				}
			
			if(iDelayTimes.iDeathDelay != NO_PREMATURE_DEATH)
				{
				TRAP(err, iDeathTimer = CDeathTimer::NewL(iDelayTimes.iDeathDelay * 1000));
				__ASSERT_DEBUG(iDeathTimer, User::Panic(KSpecAssert_RootSrvdummycpm, 1));
				}
				
			}
			break;

		case TCFCommsMessage::ECodeBind:
			{
			__CFLOG(KLogDummyCPM, KLogCode, _L("DummyCpm Bind message received"));
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
			__CFLOG(KLogDummyCPM, KLogCode, _L("DummyCpm UnBind message received"));
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
			__CFLOG(KLogDummyCPM, KLogCode, _L("DummyCpm Shutdown message received"));
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
	__CFLOG_2(KLogDummyCPM, KLogCode, _L8("DummyCpm: %d ms %S pause"), aAfterMS, &aDescription);
	User::After(aAfterMS * 1000);
	__CFLOG_1(KLogDummyCPM, KLogCode, _L8("DummyCpm: %S pause complete"), &aDescription);
	}


//---------------

void DoModuleThreadL(TAny * aArg)
	{
	TCFModuleInfo* args = reinterpret_cast<TCFModuleInfo*>(aArg);
	
	CActiveScheduler*  scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler); 

	CTestModule *pTm = CTestModule::NewL(args);

    __CFLOG_1( KLogDummyCPM, KLogCode, _L("DummyCpm[%LU] - Completed rendezvous."), RThread().Id().Id() );


	CActiveScheduler::Start();

	__CFLOG_1( KLogDummyCPM, KLogCode, _L("DummyCpm[%LU] - Exiting thread function"), RThread().Id().Id() );

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
		__CFLOG(KLogDummyCPM, KLogCode, _L("ModuleThread failed to create DummyCpm"));
		}

	delete cleanup;
	return ret;
	}



