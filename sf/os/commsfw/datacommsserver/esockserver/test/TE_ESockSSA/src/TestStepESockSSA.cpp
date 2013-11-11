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

// EPOC includes
#include <e32base.h>
#include <e32property.h>
#include <es_sock.h>
#include <in_sock.h>
#include <c32root.h>

// Test system includes
#include "TestStepESockSSA.h"
#include "blocker.h"
#include "ES_DUMMY.H"


RMultipleWait::RMultipleWait(TInt aInitialCount)
: iCount(aInitialCount)
	{
	iOwnerThread.Open(RThread().Id());
	}
	
void RMultipleWait::Close()
	{
	iOwnerThread.Close();
	inherited::Close();
	}
	
void RMultipleWait::Signal(TInt aDelta)
	{
	inherited::Wait();
	iCount += aDelta;
	iOwnerThread.RequestSignal();
	inherited::Signal();
	}
	
void RMultipleWait::Wait()
	{
	while(iCount < 0)
		{
		User::WaitForAnyRequest();
		}
	inherited::Wait();
	--iCount;
	inherited::Signal();
	}

CTestStepESockSSA::CTestStepESockSSA()
	{
	}

CTestStepESockSSA::~CTestStepESockSSA()
	{
	}

void CTestStepESockSSA::SetDummyBlocking()
	{
	// Set the blocking state, so that when the blocking module is loaded it will block
	TSecurityPolicy nullPolicy(ECapability_None);
	TInt err = RProperty::Define(TBlockerSID, CBlockerChannelHandler::EBlockingStateKey, RProperty::EInt, nullPolicy, nullPolicy);
	if(err == KErrNone || err == KErrAlreadyExists)
		{
		RProperty::Set(TBlockerSID, CBlockerChannelHandler::EBlockingStateKey, CBlockerChannelHandler::EStateBlock);
		}
	}

void CTestStepESockSSA::ClearDummyBlocking()
	{
	// Set the blocking state, so that when the blocking module is loaded it will block
	TSecurityPolicy nullPolicy(ECapability_None);
	TInt err = RProperty::Define(TBlockerSID, CBlockerChannelHandler::EBlockingStateKey, RProperty::EInt, nullPolicy, nullPolicy);
	if(err == KErrNone || err == KErrAlreadyExists)
		{
		RProperty::Set(TBlockerSID, CBlockerChannelHandler::EBlockingStateKey, CBlockerChannelHandler::EStateRelease);
		}
	}

void CTestStepESockSSA::DoDataThreadL(TDataThreadControl& aControl)
	{
	User::LeaveIfError(aControl.iSession.Connect());

	// Wait for blocker to start blocking; we can then create an IP socket as it will have already loaded
	RProperty blockProp;
	TInt err = blockProp.Attach(TBlockerSID, CBlockerChannelHandler::EBlockingStateKey);
	if(err == KErrNone)
		{
		TRequestStatus status;
		do
			{
			blockProp.Subscribe(status);
			TInt blockState;
			err = blockProp.Get(blockState);
			if(err != KErrNone || blockState >= CBlockerChannelHandler::EStateBlocking)
				{
				blockProp.Cancel();
				}
			User::WaitForRequest(status);
			} while(status == KErrNone);
		blockProp.Close();
		}
	
	switch(aControl.iRequest)
		{
	case TDataThreadControl::ESocketOpen:
		{
		RSocket sock;
		User::LeaveIfError(sock.Open(aControl.iSession, KAfInet, KSockDatagram, KProtocolInetUdp));
		sock.Close();
		aControl.iBlocked = ETrue;
		aControl.iBlockSemaphore.Signal(1);
		aControl.iResult = sock.Open(aControl.iSession, KDummyOneName);		// should block
		sock.Close();
		break;
		}
	case TDataThreadControl::EHostResolverOpen:
		{
		RHostResolver hr;
		hr.Open(aControl.iSession, KAfInet, KProtocolInetUdp);
		hr.Close();
		aControl.iBlocked = ETrue;
		aControl.iBlockSemaphore.Signal(1);
		aControl.iResult = hr.Open(aControl.iSession, KDummyAddrFamily, KDummyOne);		// should block
		hr.Close();
		break;
		}
	case TDataThreadControl::EHostResolverOpenMulti:
		{
		RHostResolver hr;
		aControl.iBlocked = ETrue;
		aControl.iBlockSemaphore.Signal(1);
		aControl.iResult = hr.Open(aControl.iSession, KDummyAddrFamily, KDummyOne);		// should block
		hr.Close();
		break;
		}
	case TDataThreadControl::EServiceResolverOpen:
		{
		RServiceResolver sr;
		sr.Open(aControl.iSession, KAfInet, KSockDatagram, KProtocolInetUdp);
		sr.Close();
		aControl.iBlocked = ETrue;
		aControl.iBlockSemaphore.Signal(1);
		aControl.iResult = sr.Open(aControl.iSession, KDummyAddrFamily, KSockDatagram, KDummyOne);		// should block
		sr.Close();
		break;
		}
	case TDataThreadControl::ENetDBOpen:
		{
		RNetDatabase ndb;
		ndb.Open(aControl.iSession, KAfInet, KProtocolInetUdp);
		ndb.Close();
		aControl.iBlocked = ETrue;
		aControl.iBlockSemaphore.Signal(1);
		aControl.iResult = ndb.Open(aControl.iSession, KDummyAddrFamily, KDummyOne);		// should block
		ndb.Close();
		break;
		}
	case TDataThreadControl::ENumProtocols:
		{
		TUint numOfProtocols;
		aControl.iBlocked = ETrue;
		aControl.iBlockSemaphore.Signal(1);
		aControl.iResult = aControl.iSession.NumProtocols(numOfProtocols);		// should block
		break;
		}
	case TDataThreadControl::EGetProtocolInfo:
		{
		TUint absentIndex = 99;
		TProtocolDesc protocolDesc;
		RHostResolver hr;
		hr.Open(aControl.iSession, KAfInet, KProtocolInetUdp);
		hr.Close();
		aControl.iBlocked = ETrue;
		aControl.iBlockSemaphore.Signal(1);
		aControl.iResult = aControl.iSession.GetProtocolInfo(absentIndex, protocolDesc);		// should block
		break;
		}
	case TDataThreadControl::EFindProtocol:
		{
		_LIT(KAbsentProtocolName,"NoSuchProtocol");
		TProtocolDesc protocolDesc;
		RHostResolver hr;
		hr.Open(aControl.iSession, KAfInet, KProtocolInetUdp);
		hr.Close();
		aControl.iBlocked = ETrue;
		aControl.iBlockSemaphore.Signal(1);
		aControl.iResult = aControl.iSession.FindProtocol(KAbsentProtocolName(), protocolDesc);		// should block
		break;
		}
	default:
		ASSERT(0);
		}
	}

TInt CTestStepESockSSA::DataThreadEntry(TDataThreadControl& aControl)
	{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if(!cleanupStack)
		{
		return KErrNoMemory;
		}
	TRAPD(err, DoDataThreadL(aControl));
	// Signal that the thread is exiting. If we got here by a Leave() we still need to signal the control thread
	aControl.iBlockSemaphore.Signal(1);

	delete cleanupStack;
	return err;
	}

TInt CTestStepESockSSA::CreateDataThread(TDataThreadControl::TRequestType aRequest, TDataThreadControl& aControl)
	{
	aControl.iRequest = aRequest;
	aControl.iResult = KRequestPending;
	aControl.iBlocked = EFalse;
	return aControl.iThread.Create(KNullDesC, reinterpret_cast<TThreadFunction>(CTestStepESockSSA::DataThreadEntry), 8192, NULL, &aControl);
	}

TVerdict CTestStepESockSSA::WaitForDataThreadsToBlock(TDataThreadControl* aThreads, TInt aNumThreads)
	{
	TVerdict verdict = EPass;
	Logger().WriteFormat(_L("Resuming %d data thread(s) & waiting for blocking"), aNumThreads);
	for(TInt i = 0; i < aNumThreads; ++i)
		{
		aThreads[i].iThread.Resume();
		User::After(1000000);
		Logger().WriteFormat(_L("Data thread: #%d %d %d"), i, aThreads[i].iThread.ExitReason(), aThreads[i].iBlocked);
		}
	
	aThreads[0].iBlockSemaphore.Wait();	// blocks until all data threads signal. If it hangs forever then check if one died
	User::After(KBlockedRequestIssueDelay);
	// Check whether all data threads have blocked
	for(TInt i = 0; i < aNumThreads; ++i)
		{
		if(!aThreads[i].iBlocked)
			{
			Logger().WriteFormat(_L("ERROR: Data thread #%d never got to block"), i);
			verdict =  EFail;
			}
		else if(aThreads[i].iResult != KRequestPending)
			{
			Logger().WriteFormat(_L("ERROR: Data thread #%d went past block with %d"), i, aThreads[i].iResult);
			verdict = EFail;
			}
		}
	return verdict;
	}

TVerdict CTestStepESockSSA::WaitForDataThreadsToComplete(TDataThreadControl* aThreads, TInt aNumThreads)
	{
	TVerdict verdict = EPass;
	Logger().WriteFormat(_L("Resuming ESOCK boot & waiting for %d data thread(s) to complete"), aNumThreads);
	aThreads[0].iBlockSemaphore.Signal(- aNumThreads + 1);
	ClearDummyBlocking();
	aThreads[0].iBlockSemaphore.Wait();	// blocks until all data threads signal. If it hangs forever then check if one died

	User::After(KBlockedRequestCompletionDelay);
	// Check whether all data threads completed their requests properly
	for(TInt i = 0; i < aNumThreads; ++i)
		{
		if(aThreads[i].iResult != aThreads[i].iExpectedResult)
			{
			Logger().WriteFormat(_L("ERROR: Data thread #%d completed with %d, expected %d"), i, aThreads[i].iResult, aThreads[i].iExpectedResult);
			verdict = EFail;
			}
		}
	return verdict;
	}

void CTestStepESockSSA::UnloadBlockerL()
	{
	// Unload the blocker module if it's there
	Logger().WriteFormat(_L("Unloading blocker module"));
	TAutoClose<RRootServ> rootserver;
	User::LeaveIfError(rootserver.iObj.Connect());
	rootserver.PushL();
	TRequestStatus status;
	TCFModuleName blockerName(KBlockerCPMName);
	rootserver.iObj.UnloadCpm(status, blockerName, EGraceful);
	User::WaitForRequest(status);
	rootserver.Pop();
	Logger().WriteFormat(_L("Request completed with %d"), status.Int());
	}

void CTestStepESockSSA::KillC32Start()
	{
	// Start by looking for the running configurator and kill it if found. The caller
	// needs the appropriate capability to do this

	RDebug::Printf("Finding existing configurator process.");	
	_LIT(KC32StartName, "*");
	TInt result;
	TBool configuratorWasPresent;
	TInt count = 0;
	const TInt KMaxKillRetry = 10;
	do 
		{
		configuratorWasPresent = EFalse;
		TFullName fn;
		TFindProcess fp(KC32StartName);
		while(fp.Next(fn) == KErrNone)
			{
			RProcess proc;
			result = proc.Open(fn);
			if(result == KErrNone)
				{
				TUidType type = proc.Type();
				if(type[2] == TUid::Uid(KUidC32StartProcess) && proc.ExitType() == EExitPending) 
					{
					// Kill the existing configurator process.
					RDebug::Print(_L("Opened existing configurator process \"%S\""), &fn);
					TRequestStatus status;
					proc.Logon(status);
					proc.Kill(KErrNone);
					RDebug::Printf("Killed process.");
					User::WaitForRequest(status);
					TExitType exitType = proc.ExitType();
					
					// Create a timer in case some other entity holds an open handle on the
					// configurator which prevents the kernel from destroying it.  We timeout
					// after one second.
					TAutoClose<RTimer> timer;
					if(timer.iObj.CreateLocal() == KErrNone)
						{
						// Request destruction notification so we know when it is safe to start
						// the process again.
						TRequestStatus destructionStatus;
						proc.NotifyDestruction(destructionStatus);
						proc.Close();
					
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
			
							RDebug::Printf("Existing configurator process has still not been destroyed after %f.0s", KProcessDestructionTimeout / 1000000.0);
							}
						}
					else
						{
						proc.Close();
						}
						
					RDebug::Printf("Process logon completed with %d, exitType %d", status.Int(), exitType);
					configuratorWasPresent = ETrue;
					}
				else
					{
					proc.Close();	
					}
				}
			}
		} while(configuratorWasPresent && ++count < KMaxKillRetry);

		if (count >= KMaxKillRetry)
			{
			RDebug::Printf("KillC32Start - Cannot kill the configurator process, we will try continue and hope all is well");
			}
		
	// Restart the boot sequence from scratch - the configurator reads this.
	RProperty::Set(KUidSystemCategory, KUidC32StartPropertyKey.iUid, EReset);
	}

TVerdict CTestStepESockSSA::doTestStepPreambleL()
	{
	SetDummyBlocking();
	RProperty ::Set(KUidSystemCategory, KUidC32StartPropertyKey.iUid, EReset);
	UnloadBlockerL();
	KillC32Start();
	return EPass;
	}

TVerdict CTestStepESockSSA::doTestStepPostambleL()
	{
	UnloadBlockerL();
	return EPass;
	}

