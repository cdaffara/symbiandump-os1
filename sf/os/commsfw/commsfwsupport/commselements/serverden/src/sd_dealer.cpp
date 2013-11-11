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
 @internalComponent
*/

#include <e32base.h>
#include "sd_log.h"
#include "sd_roles.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenDealrC, "ElemSvrDenDealrC");
#endif

using namespace Den;
using namespace CommsFW;

//
// CCommonDealer
//

/**
This function will first do the ConstructL and if that succeeds it will
start the server using the server name discovered during ConstructL. So when this
function returns the new dealer object, its server is in effect running.
*/
EXPORT_C CCommonDealer* CCommonDealer::NewL(CCommonServer* aServer)
	{
	CCommonDealer* self = new(ELeave) CCommonDealer(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CCommonDealer::ConstructL()
	{
	iServer->StartL(iServer->ServerName());
	}

EXPORT_C CCommonDealer::CCommonDealer(CCommonServer* aServer)
:	iServer(aServer)
	{
	}

EXPORT_C CCommonDealer::~CCommonDealer()
	{
	COMMONLOG((WorkerId(), KECommonServerTag, _L8("CCommonDealer::~CCommonDealer().")));
	iParkedRequests.Close();
	delete iServer;
	}

EXPORT_C const TDesC& CCommonDealer::ServerName() const
	{
	return iServer->ServerName();
	}

/**
When deleting a session it will be in response to a disconnect from the client, so we
signal that to the session. If this thread is in the process of shutting down we need to
check whether it was just waiting for this session (if this was the last one) and if this is
the case we can initiate the asynchronous process of shutting down the worker thread.
*/
EXPORT_C void CCommonDealer::DeleteSession(CWorkerSession* aSession)
	{
	// If we ever see evidence of shutdowns nested inside active calls (ie "this" deleted prematurely then consider
	// switching handling to use a CAsyncOneShot)
	aSession->CompleteDisconnect();
	if(iServer->WorkerThread().ShuttingDown() && CanShutdown())
		{
		iServer->WorkerThread().SetDealerShutdownComplete(ETrue);
		iServer->WorkerThread().MaybeTriggerThreadShutdownCallback();
		}
	}

/**
Iterate through all sessions and make sure they deploy the SubSessionProcessor on each owned sub-session.
It is not known here what the SubSessionProcessor actually does as it is implemented by the caller.
*/
void CCommonDealer::ProcessSubSessions(TWorkerId aPeerId, CWorkerSession::TSubSessionProcessor aSubSessionProcessor, TAny* aArg)
	{
	iServer->SessionIterator().SetToFirst();
	CWorkerSession* sess;
	while((sess = static_cast<CWorkerSession*>(iServer->SessionIterator()++)) != NULL)
		{
		sess->SubSessions().Lock();

		static_cast<CWorkerSession*>(sess)->ProcessSubSessions(aPeerId, aSubSessionProcessor, aArg);

		sess->SubSessions().Unlock();
		}
	}

/**
Forget all subsessions that belonged to a Player in the dead peer. The post-mortem cleanup mechanism will
separately complete any client requests against these.
*/
EXPORT_C void CCommonDealer::CleanupDeadWorker(TWorkerId aPeerId)
	{
	iServer->SessionIterator().SetToFirst();
	CSession2* sess;
	while((sess = iServer->SessionIterator()++) != NULL)
		{
		static_cast<CWorkerSession*>(sess)->CleanupDeadWorker(aPeerId);
		}
	}

TInt CCommonDealer::SubsessionCountInPlayer(TWorkerId aPeerId)
	{
	TInt numSubSessions = 0;
	ProcessSubSessions(aPeerId, CWorkerSession::CountSubSessions, &numSubSessions);
	return numSubSessions;
	}

/**
Check if we can unbind from a worker. This is only possible if the local Dealer doesn't
have any outstanding sub-sessions terminating in the peer and it doesn't have any sessions
with outstanding closes against the peer. Otherwise the channel is still needed.
*/
TBool CCommonDealer::CanUnbindFromWorker(TWorkerId aWorker)
	{
	//TBDAA ASSERT_HOME_THREAD;
	if (!iServer->WorkerThread().PitBoss().Player(aWorker))
		{
		return ETrue;
		}
	if(SubsessionCountInPlayer(aWorker) == 0)
		{
		// Check for any sessions which have outstanding session closes against the Worker
		iServer->SessionIterator().SetToFirst();
		CSession2* sess;
		while((sess = iServer->SessionIterator()++) != NULL)
			{
			if(static_cast<CWorkerSession*>(sess)->IsPlayerInDisconnectList(aWorker))
				{
				return EFalse;
				}
			}
		return ETrue;
		}
	return EFalse;
	}

/** Add to the queue of indeterminate requests parked for re-evaluation once configuration has completed
*/
EXPORT_C TInt CCommonDealer::ParkRequest(CWorkerSession* aSession, const RMessage2& aMessage, TParkReason aReason) const
	{
	COMMONLOG((WorkerId(), KECommonBootingTag, _L8("CCommonDealer::ParkRequest Session(%08x) Message(%08x) - search back through the logs for the message handle to see info about the message when it was decoded in ServiceL"), aSession, aMessage.Handle()));
	TInt err = iParkedRequests.Append(TParkedRequest(aSession, aMessage, aReason));

#if defined _DEBUG || defined SYMBIAN_TRACE_ENABLE
	if( ( err == KErrNone ) && ( iParkedRequests.Count() == 1 ) )
		{
		// Log hint to anyone who runs into a CPM deadlock due to a dependency on load order.
		// We log at the first ParkRequest to avoid unnecessary log messages as the risk only
		// exists if a module depends on a module with a parked request.
		// We log in C32Start as well as ESock and RDebug so that users are directed from the
		// CStart32 log to the ESock documentation in case they fail to realise the implications
		// of a park request.
		_LIT8(KParkRequestPotentialDeadlockWarning, "ESockSvr CCommonDealer::ParkRequest(reason=%d): If the log hangs before the \"ESockSvr startup modules loaded!\" log entry then there might be a deadlock involving a client calling too early during bootup before a component is ready. See Esock How To.");
		#ifdef _DEBUG
			RDebug::Printf(reinterpret_cast<const char*>(TPtrC8(KParkRequestPotentialDeadlockWarning).Ptr()), aReason);
		#endif
		COMMONLOG((WorkerId(), KECommonBootingTag, KParkRequestPotentialDeadlockWarning));
		}
#endif

	return err;
}

CCommonDealer::TParkedRequest::TParkedRequest(CWorkerSession* aSession, const RMessage2& aMessage, TParkReason aReason)
:	iSession(aSession),
 	iMessage(aMessage),
 	iReason(aReason)
	{
	}

/** Complete all requests parked for the given reason
*/
void CCommonDealer::ReleaseParkedRequests(TParkReason aReason)
	{
	COMMONLOG((WorkerId(),KECommonBootingTag, _L8("CCommonDealer::ReleaseParkedRequests(aReason = %d) - %d total parked messages"), aReason, iParkedRequests.Count()));
	TInt i = 0;
	while(i < iParkedRequests.Count())	// traverse in order of arrival to minimise odd effects for clients (they shouldn't have such dependencies really, but could)
		{
		const TParkedRequest& request = iParkedRequests[i];
		if(request.iReason == aReason)
			{
			// Check that the session still exists
			iServer->SessionIterator().SetToFirst();
			CWorkerSession* ss;
			while((ss = static_cast<CWorkerSession*>(iServer->SessionIterator()++)) != NULL && ss != request.iSession)
				{
				}

			if(ss)
				{
				COMMONLOG((WorkerId(),KECommonBootingTag, _L8("CCommonDealer::ReleaseParkedRequests() - parked message %08x for sess %08x"), request.iMessage.Handle(), ss));
				TRAPD(err, ss->ServiceL(request.iMessage));
				if(err != KErrNone && !request.iMessage.IsNull())
					{
					request.iMessage.Complete(err);
					}
				}
			iParkedRequests.Remove(i);
			}
		else
			{
			++i;
			}
		}
	}

#if defined(__ELEMENTS_MESSAGE_INTERCEPT_ACTIVE)

/** Complete a selection of requests parked by debug requests
*/
void CCommonDealer::ReleaseDebugParkedRequests(CWorkerSession* aSess, TInt aSubSessHandle)
	{
	COMMONLOG((WorkerId(),KECommonBootingTag, _L8("CCommonDealer::ReleaseDebugParkedRequests(aSess=%08x, aSubSessHandle=%08x) - %d total parked messages"), aSess, aSubSessHandle, iParkedRequests.Count()));
	TInt i = 0;
	while(i < iParkedRequests.Count())	// traverse in order of arrival to minimise odd effects for clients (they shouldn't have such dependencies really, but could)
		{
		const TParkedRequest& request = iParkedRequests[i];
		if(request.iReason == EDebugParking &&
			(!aSess || aSess == request.iSession) &&
			(!aSubSessHandle || aSubSessHandle == request.iMessage.Int3()))
			{
			// Check that the session still exists
			iServer->SessionIterator().SetToFirst();
			CWorkerSession* ss;
			while((ss = static_cast<CWorkerSession*>(iServer->SessionIterator()++)) != NULL && ss != request.iSession)
				{
				}
			if(ss)
				{
				COMMONLOG((WorkerId(),KECommonBootingTag, _L8("CCommonDealer::ReleaseDebugParkedRequests() - parked message %08x for sess %08x"), request.iMessage.Handle(), ss));
				TRAPD(err, ss->ServiceL(request.iMessage));
				if(err != KErrNone && !request.iMessage.IsNull() && !ss->Disconnecting())
					{
					request.iMessage.Complete(err);
					}
				}
			iParkedRequests.Remove(i);
			}
		else
			{
			++i;
			}
		}
	}

#endif // __ELEMENTS_MESSAGE_INTERCEPT_ACTIVE


/** Once server configuration is complete any requests parked because they required the full environment can be
re-evaluated
*/
EXPORT_C void CCommonDealer::ProcessConfigurationComplete(TConfigurationCompletionType aType)
	{
	// The enums are numerically equivalent (at time of writing), so have to hope compiler does the obvious here
	CCommonDealer::TParkReason reason(EIndeterminateDuringBoot);
	switch(aType)
		{
	case EModuleInitialisation:
		reason = EIndeterminateDuringBoot;
		break;
	case ETierMapping:
		reason = EAwaitingTierToWorkerMapping;
		break;
	default:
		__ASSERT_DEBUG(FALSE, User::Panic(KSpecAssert_ElemSvrDenDealrC, 1));
		}
	ReleaseParkedRequests(reason);
	}

void CCommonDealer::ProcessShutdownRequest(TCFShutdownType aType)
	{
	TBool shutdownImmediately = CanShutdown();

	// If have to do now and there are still sessions then we exit anyway but suppress the heap check and log a rude message. We
	// used to delete the sessions but that isn't safe
	if(CommsFW::EImmediate==aType)
		{
    	shutdownImmediately = ETrue;

#ifdef SYMBIAN_TRACE_ENABLE
		iServer->SessionIterator().SetToFirst();
		if(iServer->SessionIterator()++ != NULL)
			{
			TInt cnt = 0;
			iServer->SessionIterator().SetToFirst();
			CSession2* ss;
			while((ss = iServer->SessionIterator()++) != NULL)
				{
				COMMONLOG((WorkerId(), KECommonServerTag, _L8("<==Session(%08x): remaining"), ss));
				++cnt;
				}
			COMMONLOG((WorkerId(), KECommonServerTag, _L8("NB! Immediate shutdown commanded but #%d client sessions remaining (bad test code?)"), cnt));
			}
#endif
		}

	if(WorkerThread().IsMainThread())
		{
		if(CommsFW::EImmediate != aType)
			{
			WorkerThread().PitBoss().StartShutdown();
			}
		// Even the PitBoss yields to the immediate shutdown
		if(shutdownImmediately)
			{
			WorkerThread().PitBoss().SessionShutdownComplete();
			}
		}
	WorkerThread().SetDealerShutdownComplete(shutdownImmediately);
	}



//
// CCommonWorkerDealer
//

EXPORT_C CCommonWorkerDealer* CCommonWorkerDealer::NewL(CCommonServer* aServer)
	{
	CCommonWorkerDealer* self = new(ELeave) CCommonWorkerDealer(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCommonWorkerDealer::CCommonWorkerDealer(CCommonServer* aServer)
:	CCommonDealer(aServer)
	{
	}

/**
The name of the server will be postfixed with thread ID and Worker ID. If this worker is revived in
case of a crash, it will have a different thread ID and not cause a name clash with the dead server.
It is also useful for debugging/logging purposes.
*/
EXPORT_C void CCommonWorkerDealer::ConstructL()
	{
	iEligibleClients.ConstructL(); // Slightly unusual construction pattern, but does it reallly need to be CBase..

	// Replace server name
	_LIT(KServerNameFormat, "-%d-%x");
	TUint threadId = RThread().Id();
	iServer->ServerName().AppendFormat(KServerNameFormat, WorkerId(), threadId);

	CCommonDealer::ConstructL(); //Start the server etc
	}

EXPORT_C CCommonWorkerDealer::~CCommonWorkerDealer()
	{
	//CloseSessionSustainer();
	}

/**
This method is called directly from the PitBoss thread, never from the local worker thread.
*/
EXPORT_C void CCommonWorkerDealer::AddEligiblePidL(TProcessId aId)
	{
	iEligibleClients.AddL(aId);
	}

/**
This method is called directly from the PitBoss thread, never from the local worker thread.
*/
EXPORT_C void CCommonWorkerDealer::RemoveEligiblePid(TProcessId aId)
	{
	iEligibleClients.Remove(aId);
	}

/**
The WorkerDealer is using this when a client does a Connect, to check whether the process has
been allowed access by the PitBoss.
*/
EXPORT_C TBool CCommonWorkerDealer::IsEligible(TProcessId aId)
	{
	return iEligibleClients.IsEligible(aId);
	}

/*
void CCommonWorkerDealer::CloseSessionSustainer()
	{
	//iSessionSustainer.Close();
	}
*/

//
// EligibleClients
//

CCommonWorkerDealer::XEligibleClients::XEligibleClients()
	{
	// Initialize the list of eligible client PIDs
	for(TInt i=0; i<KMaxEligibleList; i++)
		{
		iEligibleClients[i] = KNullProcessId;
		}
	}

CCommonWorkerDealer::XEligibleClients::~XEligibleClients()
	{
	iLock.Close(); // Lock created/opened in the ConstructL
	}

void CCommonWorkerDealer::XEligibleClients::ConstructL()
	{
	User::LeaveIfError(iLock.CreateLocal());
	}

/**
Adds ProcessID to list of eligible clients that can connect to dealer.
@note This function MUST ONLY be called by the main dealer!
@param aId The TProcessId to add to the list.
@leave KErrServerBusy If table is full.
*/
void CCommonWorkerDealer::XEligibleClients::AddL(TProcessId aId)
	{
	TBool inserted=EFalse;

	iLock.Wait(); // Entering locked zone, no leaving or returning inside allowed.
		{
		for(TInt i=0; i<KMaxEligibleList && !inserted; i++)
			{
			if(KNullProcessId == iEligibleClients[i])
				{
				iEligibleClients[i]=aId;
				inserted=ETrue;
				}
			}
		}
	iLock.Signal();

	if(!inserted)
		{
		User::Leave(KErrServerBusy);
		}
	}

/**
Removes ProcessID from the list of eligible clients that can connect to dealer.
@note This function MUST ONLY be called by the main dealer!
@param aId The TProcessId to remove from the list.
*/
void CCommonWorkerDealer::XEligibleClients::Remove(TProcessId aId)
	{
	TBool removed=EFalse;

	iLock.Wait(); // Entering locked zone, no leaving or returning inside allowed.
		{
		for(TInt i=0; i<KMaxEligibleList && !removed; i++)
			{
			if(iEligibleClients[i]==aId)
				{
				iEligibleClients[i] = KNullProcessId;
				removed=ETrue;
				}
			}
		}
	iLock.Signal();

	__ASSERT_DEBUG(removed, User::Panic(KSpecAssert_ElemSvrDenDealrC, 2)); // Wanted to remove a nonexisting PID, something is VERY WRONG in the state of Denmark.
	}

/**
Check whether a client is eligible to connect to this dealer.
@param aId The TProcessId to look for in the list of eligible clients.
*/
TBool CCommonWorkerDealer::XEligibleClients::IsEligible(TProcessId aId)
	{
	TBool eligible=EFalse;

	iLock.Wait(); // Entering locked zone, no leaving or returning inside allowed.
		{
		for(TInt i=0; i<KMaxEligibleList && !eligible; i++)
			{
			if(iEligibleClients[i]==aId)
				{
				eligible=ETrue;
				}
			}
		}
	iLock.Signal();

	return eligible;
	}


