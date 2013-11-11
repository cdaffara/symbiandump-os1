// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sd_log.h"
#include "sd_std.h"
#include "sd_roles.h"
#include "sd_errors.h"
#include "sd_msgs.h"
#include "sd_apiextensionrouting.h"
#include "sd_apiextensionipc.h"
#include <elements/nm_common.h>
#include <elements/cfmacro.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenWrkrSC, "ElemSvrDenWrkrSC");
#endif

using namespace Den;
using namespace Messages;
using namespace Elements;
using namespace CommsFW;

class RNullableMessage : public RMessage2
	{
public:
	void NullHandle() const
		{
		// Although this casting away of const looks awful it's in keeping with the ability to Complete() const
		// messages - perhaps iHandle should have been mutable?
		const_cast<RNullableMessage&>(*this).iHandle = 0;
		}
	};

//
// CSubSessionIx
//

void CSubSessionIx::InitialiseL()
	{
	User::LeaveIfError(iLock.CreateLocal());
	}

EXPORT_C CSubSessionIx::~CSubSessionIx()
	{
    iLock.Close();
	User::Free(iIx);
	}

EXPORT_C TInt CSubSessionIx::Find(CWorkerSubSession* aSubSession, TSubSessionHandle& aHandle) const
	{
	iLock.AssertLockHeld();
	for(TInt ix = 0; ix < iSize; ++ix)
		{
		TEntry& e = iIx[ix];
		if(e.iType != TSubSessInfo::ENull && e.iObject == aSubSession)
			{
			aHandle = MakeHandle(ix, e.iMagic, e.iType);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

TInt CSubSessionIx::Add(CWorkerSubSession* aSubSession, TSubSessionHandle& aHandle)
	{
	iLock.AssertLockHeld();
	if(TUint(iFreeListHead) >= TUint(iSize))
		{
		TInt err = ExpandArray();
		if(err != KErrNone)
			{
			return err;
			}
		}
	TInt ix = iFreeListHead;
	TEntry& e = iIx[ix];
	__ASSERT_DEBUG(e.iType == TSubSessInfo::ENull, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 1));
	iFreeListHead = e.iNextFree;
	__ASSERT_DEBUG(TUint(iFreeListHead) <= TUint(iSize), User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 2));
	e.iObject = aSubSession;
	e.iType = aSubSession->Type().iType;
	TInt magic = (e.iMagic + 1) & KMagicMask;
	e.iMagic = TUint16(magic);
	aHandle = MakeHandle(ix, magic, e.iType);
	++iActiveCount;
	return KErrNone;
	}

EXPORT_C CWorkerSubSession* CSubSessionIx::Replace(TSubSessionHandle aHandle, CWorkerSubSession* aSubSession)
	{
	iLock.AssertLockHeld();
	__ASSERT_DEBUG(TUint(iFreeListHead) <= TUint(iSize), User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 3));
	TEntry* e = At(aHandle);
	__ASSERT_DEBUG(e, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 4));
	CWorkerSubSession* origSubSession = e->iObject;
	e->iObject = aSubSession;
	return origSubSession;
	}

EXPORT_C CWorkerSubSession* CSubSessionIx::Remove(TSubSessionHandle aHandle)
	{
	iLock.AssertLockHeld();
	__ASSERT_DEBUG(TUint(iFreeListHead) <= TUint(iSize), User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 5));
	TEntry* e = At(aHandle);
	__ASSERT_DEBUG(e, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 6));
	CWorkerSubSession* subSess = e->iObject;
	e->iType = TUint8(TSubSessInfo::ENull);
	e->iNextFree = iFreeListHead;
	iFreeListHead = e - &iIx[0];
	__ASSERT_DEBUG(TUint(iFreeListHead) < TUint(iSize), User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 7));
	--iActiveCount;
	return subSess;
	}

EXPORT_C CWorkerSubSession* CSubSessionIx::At(TInt aHandle, TSubSessInfo aType) const
	{
	iLock.AssertLockHeld();
	TEntry* e = At(aHandle);
	if(e &&
		(e->iType == aType.iType || aType.iType == TSubSessInfo::EAny))
		{
		return e->iObject;
		}
	return NULL;
	}

EXPORT_C CSubSessionIx::TEntry* CSubSessionIx::At(TSubSessionHandle aHandle) const
	{
	TEntry* e = 0;
	TInt ix = IndexFromHandle(aHandle);
	if(TUint(ix) < TUint(iSize))
		{
		e = &iIx[ix];
		if(MakeHandle(ix, e->iMagic, e->iType) != aHandle)
			{
			e = 0;
			}
		}
	return e;
	}

TInt CSubSessionIx::ExpandArray()
	{
	__ASSERT_DEBUG(TUint(iFreeListHead) <= TUint(iSize), User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 8));
	TInt size = iSize + EIndexGranularity;
	if(size > KIndexLimit)
		{
		return KErrNoMemory;
		}
	TEntry* ix = reinterpret_cast<TEntry*>(User::ReAlloc(iIx, size * sizeof(TEntry)));
	if(!ix)
		{
		return KErrNoMemory;
		}
	iIx = ix;
	iSize = size;
	for(TInt ii = iFreeListHead; ii < size;)
		{
		TEntry& e = ix[ii];
		e.iNextFree = ++ii;
		e.iType = TUint8(TSubSessInfo::ENull);
		}
	return KErrNone;
	}

CSubSessionIx::TIter::TIter(CSubSessionIx& aContainer)
: iContainer(aContainer)
	{
	SetToFirst();
	}

void CSubSessionIx::TIter::SetToFirst()
	{
	iPos = 0;
	}

CWorkerSubSession* CSubSessionIx::TIter::operator++(TInt)
	{
	TSubSessionHandle dummyHandle;
	return Next(dummyHandle);
	}

CWorkerSubSession* CSubSessionIx::TIter::Next(TSubSessionHandle& aHandle)
	{
	iContainer.iLock.AssertLockHeld();
	TEntry* e = &iContainer.iIx[iPos];
	while(iPos < iContainer.iSize)
		{
		TInt oldPos = iPos++;
		if(e->iType != TSubSessInfo::ENull)
			{
			aHandle = iContainer.MakeHandle(oldPos, e->iMagic, e->iType);
			return e->iObject;
			}
		++e;
		}
	return NULL;
	}

void CWorkerSubSession::CleanupDeadWorker(TWorkerId aWorkerId)
	{
	CCommonSessionProxy* sessProxy = SessionProxy();
	if(sessProxy && sessProxy->Player().WorkerId() == aWorkerId)
		{
		delete this;
		}
	}

// API extension support
EXPORT_C TIpcExtensionRoutingMessageFromSubSession* CWorkerSubSession::IsSubSessionIpcExtensionSupported(TInt aInterfaceId, TDes8& aBuff)
	{
	TIpcExtensionRoutingMessageFromSubSession* routinMsg = DoIsSubSessionIpcExtensionSupported(aInterfaceId, aBuff);
	if (routinMsg!=NULL)
		{
		__ASSERT_ALWAYS(routinMsg->IsTypeOf(TIpcExtensionRoutingMessageFromSubSession::TypeId()), User::Panic(KDenFaultPanic, ECommonBadState));
		}
	return routinMsg;
	}

EXPORT_C void CWorkerSubSession::OpenSubSessionIpcExtensionL(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification)
	{
	iApiExtRegister.RegisterOpenInterfaceLC(aExtIdentification); //Fails with KErrAlreadyExists if already opened
	TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
	TIpcExtensionRoutingMessageFromSubSession* routingMsg = IsSubSessionIpcExtensionSupported(aExtIdentification.InterfaceId(), msgDst);
	if (routingMsg==NULL)
		{
		User::Leave(KErrNotSupported);
		}
	routingMsg->KickOffL(aMessage, *this, aExtIdentification);
	CleanupStack::Pop(); //RegisterInterfaceLC
	DontCompleteCurrentRequest();
	}

EXPORT_C void CWorkerSubSession::SendReceiveSubSessionIpcExtensionL(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification)
	{
	if (!iApiExtRegister.IsOpened(aExtIdentification))
		{
		//Out of band message..
		PanicClient(ECommonBadState);
		return;
		}

	TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
	TIpcExtensionRoutingMessageFromSubSession* routingMsg = IsSubSessionIpcExtensionSupported(aExtIdentification.InterfaceId(), msgDst);
	//routingMsg must be there now, otherwise the interface would not be opened!
	__ASSERT_ALWAYS(routingMsg, User::Panic(KDenFaultPanic, ECommonBadState));
	routingMsg->KickOffL(aMessage, *this, aExtIdentification);
	DontCompleteCurrentRequest();
	}

EXPORT_C void CWorkerSubSession::CloseSubSessionIpcExtension(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification)
	{
	if (iApiExtRegister.IsOpened(aExtIdentification))
		{
		TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
		TIpcExtensionRoutingMessageFromSubSession* routingMsg = IsSubSessionIpcExtensionSupported(aExtIdentification.InterfaceId(), msgDst);
		//routingMsg must be there, otherwise the interface would not be opened!
		__ASSERT_ALWAYS(routingMsg, User::Panic(KDenFaultPanic, ECommonBadState));
		routingMsg->KickOffL(aMessage, *this, aExtIdentification);
		iApiExtRegister.DeregisterClosedInterface(aExtIdentification);
		DontCompleteCurrentRequest();
		}
	}

EXPORT_C void CWorkerSubSession::CloseAllSubSessionIpcExtensions()
	{
	//This funtion does not expect not causes the SafeMessage() to be completed.
	//It is called from the session's destructor.
	TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
	for (TInt i = iApiExtRegister.InterfaceCount() - 1; i >= 0; --i)
		{
		const TApiExtIdentification& entry = iApiExtRegister[i];
		TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
		TIpcExtensionRoutingMessageFromSubSession* routingMsg = IsSubSessionIpcExtensionSupported(entry.InterfaceId(), msgDst);
		//routingMsg must be there now, otherwise the interface would not be opened!
		__ASSERT_ALWAYS(routingMsg, User::Panic(KDenFaultPanic, ECommonBadState));
		RSettableParameterMessage message(EIpcSubSessionApiExtIfaceClose);
		routingMsg->KickOffL(message, *this, entry);
		iApiExtRegister.DeregisterClosedInterface(entry);
		}
	}


#ifdef SYMBIAN_TRACE_ENABLE
TInt CWorkerSession::DisconnectPlayers()
	{
	return iDisconnectPlayers;
	}
#endif


EXPORT_C CWorkerSession::CWorkerSession(TUidType aUid, TSessionUniqueId aSessionUniqueId)
:	iUidType(aUid),
	iSessionUniqueId(aSessionUniqueId)
	{
	}

EXPORT_C void CWorkerSession::ConstructL(TProcessId aProcessId)
	{
	User::LeaveIfError(iProcess.Open(aProcessId));
	iSubSessions.InitialiseL();
	}

/** Sends SessionClosed to all non-resident players. Add self to Dealers list of disconnecting sessions.
*/
void CWorkerSession::Disconnect()
	{
	#ifdef SYMBIAN_TRACE_ENABLE
		TBuf8<KMaxProcessName> processName;
		processName.Copy(iProcess.Name().Left(KMaxProcessName));
	 	COMMONLOG((WorkerId(), KECommonSessDetailTag, _L8("CWorkerSession(%08x):\tDisconnect(), \"%S\""), this, &processName));
	 #endif

 		{
 		SubSessions().Lock();
		// Remove all subsessions from the session, i.e., just as if the client had closed them all individually
		ProcessSubSessions(TWorkerThreadPublicInfo::ENullWorkerId, ForgetSubSession, NULL);
		SubSessions().Unlock();
		}

	CCommonWorkerThread& owner=WorkerThread();
	TWorkerId self = owner.WorkerId();
	for(TWorkerId player = TWorkerThreadPublicInfo::EFirstPlayerThread; player <= TWorkerThreadPublicInfo::EMaxWorkerThreadId; ++player)
		{
		if(IsPlayerInDisconnectList(player))
			{
			if(owner.PeerReachable(player))
				{
				/* Sending closed session message to involved player, dont care if it times out. After this we just wait
				anyway for a while and then obliterate it.*/
				COMMONLOG((WorkerId(),KECommonSessDetailTag, _L8("CWorkerSession(%08x):\tDisconnect() Sending SessionClose to W%d"), this, player));
				TPlayerSessionCloseMsg msg(this);
				owner.PostMessage(player, reinterpret_cast<TCFCommsMessage&>(msg));
				}
			else
				{
				__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 9)); 	// Ouch! - binding gone & we need to talk to them. Maybe they crashed & there's a bug in the cleanup of that
				}
			}
		}
	}

EXPORT_C void CWorkerSession::Disconnect(const RMessage2& aMessage)
	{
	COMMONLOG((WorkerId(),KECommonSessDetailTag, _L8("CWorkerSession(%08x):\tDisconnect(const RMessage2& = %08x)"), this, aMessage.Handle()));

	iDisconnectMessage = aMessage;

	Disconnect();

	// If co-resident Player was only one then complete immediately
	if(IsDisconnectListEmpty())
		{
		COMMONLOG((WorkerId(),KECommonSessDetailTag, _L8("CWorkerSession(%08x):\tDisconnect(const RMessage2&) inherited disconnect"), this));
		CSession2::Disconnect(aMessage);
		}
	}

void CWorkerSession::AddPlayerToDisconnectList(TWorkerId aPlayerId)
	{
	__ASSERT_COMPILE(TWorkerThreadPublicInfo::EMaxWorkerThreadId < 32);	// Using a TUint32 as a flag container
	__ASSERT_DEBUG(aPlayerId >= TWorkerThreadPublicInfo::EFirstPlayerThread && aPlayerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 10));
#ifdef SYMBIAN_TRACE_ENABLE
		if(!IsPlayerInDisconnectList(aPlayerId))
			{
			COMMONLOG((WorkerId(),KECommonSessDetailTag, _L8("CWorkerSession(%08x):\tAddPlayerToDisconnectList(%d)"), this, aPlayerId));
			}
#endif
	iDisconnectPlayers |= (1 << aPlayerId);
	}

void CWorkerSession::RemovePlayerFromDisconnectList(TWorkerId aPlayerId)
	{
	__ASSERT_DEBUG(aPlayerId >= TWorkerThreadPublicInfo::EFirstPlayerThread && aPlayerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 11));
#ifdef SYMBIAN_TRACE_ENABLE
		if(IsPlayerInDisconnectList(aPlayerId))
			{
			COMMONLOG((WorkerId(),KECommonSessDetailTag, _L8("CWorkerSession(%08x):\tRemovePlayerFromDisconnectList(%d)"), this, aPlayerId));
			}
#endif
	iDisconnectPlayers &= ~(1 << aPlayerId);
	}

EXPORT_C TBool CWorkerSession::IsDisconnectListEmpty() const
	{
	return iDisconnectPlayers == 0;
	}

TBool CWorkerSession::IsPlayerInDisconnectList(TWorkerId aPlayerId) const
	{
	// A potential optimisation for the future would be to actively check whether the player still has subsessions here, rather
	// than blinding sending the session close message
	__ASSERT_DEBUG(aPlayerId >= TWorkerThreadPublicInfo::EFirstPlayerThread && aPlayerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 12));
	return iDisconnectPlayers & (1 << aPlayerId);
	}

void CWorkerSession::CleanupDeadWorker(TWorkerId aPeerId)
	{
	// Forget all subsessions belonging to that Player and no longer a need to notify Player of session closure
		{
		SubSessions().Lock();

		ProcessSubSessions(aPeerId, ForgetSubSession, NULL);

		SubSessions().Unlock();
		}
	RemovePlayerFromDisconnectList(aPeerId);
	}


void CWorkerSession::ForgetSubSession(CWorkerSession* aSelf, CWorkerSubSession* aSubSession, TInt aSubSessionHandle, TAny*)
	{
	(void) aSubSession;	// suppress UREL warnings
	VERIFY_RESULT(aSelf->iSubSessions.Remove(aSubSessionHandle), aSubSession);
	}

void CWorkerSession::CountSubSessions(CWorkerSession*, CWorkerSubSession*, TInt, TAny* aArg)
	{
	TInt* counter = reinterpret_cast<TInt*>(aArg);
	++*counter;
	}

#ifdef SYMBIAN_TRACE_ENABLE
EXPORT_C void CWorkerSession::LogSubSession(CWorkerSession*, CWorkerSubSession* aSubSession, TInt, TAny*)
#else
EXPORT_C void CWorkerSession::LogSubSession(CWorkerSession*, CWorkerSubSession*, TInt, TAny*)
#endif
	{
	COMMONLOG((KInvalidWorkerId, KECommonSessDetailTag, _L8("CSockSession(%08x):\tLogSubSession --subsess: %08x"), aSubSession));
	}


/**
Destroy
*/
EXPORT_C CWorkerSession::~CWorkerSession()
	{
	//All interfaces muxt be closed by now (by the derived session's destructor the latest)
	__ASSERT_DEBUG(iApiExtRegister.InterfaceCount() == 0, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 13));

	// On the normal pathway all subsessions are removed by Disconnect() before session destruction can proceed. The only time we just delete a session
	// without being instructed by the client is during an immediate shutdown
	__ASSERT_DEBUG((iSubSessions.ActiveCount() == 0 || PitBoss().TestImmediateShutdownPresent()), User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 18));

	iProcess.Close();

	const CCommonServer* s = static_cast<const CCommonServer*>(Server());
	if (s)     // guard against leave during construction (e.g. a leave from CWorkerSession::ConstructL()).
	    {
        __ASSERT_DEBUG(s->iNumSessions>0, User::Panic(KDenFaultPanic, ECommonNegativeSessionCount));
        s->iNumSessions--;
        COMMONLOG((WorkerId(), KECommonSessDetailTag, _L8("CWorkerSession(%08x):\t~CWorkerSession() iNumSessions=%d (remaining)"), this, s->iNumSessions));
    
        CCommonWorkerThread& selfWorker = WorkerThread();
        if (selfWorker.ShuttingDown())
            {
            COMMONLOG((WorkerId(), KECommonSessDetailTag, _L8("Shutdown requested: %d sessions"), s->iNumSessions));
            if (s->iNumSessions <= 0)
                {
                if(selfWorker.IsMainThread())
                    {
                    selfWorker.PitBoss().SessionShutdownComplete();
                    }
                else
                    {
                    selfWorker.MaybeTriggerThreadShutdownCallback();
                    }
                }
            }
	    }
	}

EXPORT_C void CWorkerSession::ProcessSubSessions(TWorkerId aPeerId, TSubSessionProcessor aSubSessionProcessor, TAny* aArg)
	{
	TInt count = iSubSessions.ActiveCount();

	if(count != 0)
		{
		COMMONLOG((WorkerId(), KECommonSessDetailTag, _L8("CSockSession(%08x):\tProcessSubSessions - Processing %d subsessions for W%d:"), this, count, aPeerId));

		CSubSessionIx::TIter iter(iSubSessions);

		CWorkerSubSession* ss;
		TInt ssHandle;
		while((ss = iter.Next(ssHandle)) != NULL)
			{
			COMMONLOG((WorkerId(), KECommonSessDetailTag, _L8("CSockSession(%08x):\tProcessSubSessions - Processing subsession %08X"), this, ss));

			if(aPeerId == TWorkerThreadPublicInfo::ENullWorkerId || aPeerId == ss->Player().WorkerId())
				{
				aSubSessionProcessor(this, ss, ssHandle, aArg);
				}
			}
		}
	}


/** Determine WorkedId for a subsession and deal with the message.
@param aMessage IPC message received from client.
@param aSubSess Subsession that needs to deal with this message.
*/
EXPORT_C void CWorkerSession::ForwardMessageL(const RMessage2 &aMessage, CWorkerSubSession& aSubSess)
	{
	ForwardMessageL(aMessage, &aSubSess, aSubSess.Player().WorkerId());
	}

/** Determine whether to deal with message directly or
to send it on a channel.
@param aMessage IPC message received from client.
@param aSS xx
@param aWorker Id of worker that can handle this message.
*/
EXPORT_C void CWorkerSession::ForwardMessageL(const RMessage2 &aMessage, CWorkerSubSession* aSS, TWorkerId aWorker)
	{
	COMMONLOG((WorkerId(), KECommonSessDetailTag, _L8("CSockSession::ForwardMessageL() - message (%08X), subsession (%08X), W%d"), aMessage.Handle(), aSS, aWorker));
	__ASSERT_DEBUG(aWorker >= TWorkerThreadPublicInfo::EFirstPlayerThread && aWorker <= TWorkerThreadPublicInfo::EMaxWorkerThreadId, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 14));

	CCommonWorkerThread& owner=WorkerThread();

	/* If the Player is co-resident in this worker thread */
	if(owner.WorkerId()==aWorker)
		{
		COMMONLOG((WorkerId(), KECommonSessDetailTag, _L8("CSockSession::ForwardMessageL() - forwarding to player co-resident in W%d"), aWorker));
		AddPlayerToDisconnectList(aWorker);
		owner.Player()->ProcessMessageL(SafeMessage(aMessage), aSS);
		DontCompleteCurrentRequest();
		}
	else // Forward to Player thread
		{
		if(owner.PeerReachable(aWorker))
			{
			COMMONLOG((WorkerId(), KECommonSessDetailTag, _L8("CSockSession::ForwardMessageL() - forwarding to player co-resident in W%d"), aWorker));
			AddPlayerToDisconnectList(aWorker);
			TPlayerForwardRequestMsg msg(aMessage, aSS);
			owner.PostMessage(aWorker, msg);
			DontCompleteCurrentRequest();
			const RNullableMessage& forwardedMsg = static_cast<const RNullableMessage&>(aMessage);
			forwardedMsg.NullHandle();
			}
		else
			{
			// Only normal case for absent peer is that they've unloaded, which they mustn't do until all sessions have gone. So it's an abnormal case:
			// either they've been told to do an immediate unload or they died, or possibly the bindings between workers are malformed (eg W1 hosts a
			// DealerPlayer, W2 a Player, both bound correctly to W0 but not to each other. So a request for a W2 protocol arrives at W1, it consults
			// the protocol lookups owned by the PitBoss in W0 and discovers that it needs to forward to W2, but has to fail the request.
			COMMONLOG((WorkerId(), KECommonServerTag, _L8("CSockSession::ForwardMessageL() - no peer handler or no binding for message (%08X) to W%d"), aMessage.Handle(), aWorker));
			aMessage.Complete(KErrPlayerNotFound);
			DontCompleteCurrentRequest();
			}
		}
	}

/** The player has handled the SessionClose message and has cleaned up */
EXPORT_C void CWorkerSession::SessionCloseResp(TWorkerId aPlayerId)
	{
	RemovePlayerFromDisconnectList(aPlayerId);
	}

EXPORT_C TIpcExtensionRoutingMessageFromSession* CWorkerSession::IsSessionIpcExtensionSupported(TInt aInterfaceId, TDes8& aBuff)
	{
	TIpcExtensionRoutingMessageFromSession* routinMsg = DoIsSessionIpcExtensionSupported(aInterfaceId, aBuff);
	if (routinMsg!=NULL)
		{
		__ASSERT_ALWAYS(routinMsg->IsTypeOf(TIpcExtensionRoutingMessageFromSession::TypeId()), User::Panic(KDenFaultPanic, ECommonBadState));
		}
	return routinMsg;
	}

EXPORT_C void CWorkerSession::OpenSessionIpcExtensionL(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification)
	{
	iApiExtRegister.RegisterOpenInterfaceLC(aExtIdentification); //Fails with KErrAlreadyExists if already opened
	TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
	TIpcExtensionRoutingMessageFromSession* routingMsg = IsSessionIpcExtensionSupported(aExtIdentification.InterfaceId(), msgDst);
	if (routingMsg==NULL)
		{
		User::Leave(KErrNotSupported);
		}
	routingMsg->KickOffL(aMessage, *this, aExtIdentification);
	CleanupStack::Pop(); //RegisterInterfaceLC
	DontCompleteCurrentRequest();
	}

EXPORT_C void CWorkerSession::SendReceiveSessionIpcExtensionL(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification)
	{
	if (!iApiExtRegister.IsOpened(aExtIdentification))
		{
		//Out of band message..
		aMessage.PanicClient(KWorkerClientPanic, ECommonBadState);
		return;
		}

	TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
	TIpcExtensionRoutingMessageFromSession* routingMsg = IsSessionIpcExtensionSupported(aExtIdentification.InterfaceId(), msgDst);
	//routingMsg must be there now, otherwise the interface would not be opened!
	__ASSERT_ALWAYS(routingMsg, User::Panic(KDenFaultPanic, ECommonBadState));
	routingMsg->KickOffL(aMessage, *this, aExtIdentification);
	DontCompleteCurrentRequest();
	}

EXPORT_C void CWorkerSession::CloseSessionIpcExtension(const RSafeMessage& aMessage, const TApiExtIdentification& aExtIdentification)
	{
	if (iApiExtRegister.IsOpened(aExtIdentification))
		{
		TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
		TIpcExtensionRoutingMessageFromSession* routingMsg = IsSessionIpcExtensionSupported(aExtIdentification.InterfaceId(), msgDst);
		//routingMsg must be there, otherwise the interface would not be opened!
		__ASSERT_ALWAYS(routingMsg, User::Panic(KDenFaultPanic, ECommonBadState));
		routingMsg->KickOffL(aMessage, *this, aExtIdentification);
		iApiExtRegister.DeregisterClosedInterface(aExtIdentification);
		DontCompleteCurrentRequest();
		}
	}

EXPORT_C void CWorkerSession::CloseAllSessionIpcExtensions()
	{
	//This funtion does not expect not causes the SafeMessage() to be completed.
	//It is called from the session's destructor.
	TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
	for (TInt i = iApiExtRegister.InterfaceCount() - 1; i >= 0; --i)
		{
		const TApiExtIdentification& entry = iApiExtRegister[i];
		TBuf8<TSignalBase::KMaxInlineMessageSize+TSignalBase::KMaxUnstoredOverhead> msgDst;
		TIpcExtensionRoutingMessageFromSession* routingMsg = IsSessionIpcExtensionSupported(entry.InterfaceId(), msgDst);
		//routingMsg must be there now, otherwise the interface would not be opened!
		__ASSERT_ALWAYS(routingMsg, User::Panic(KDenFaultPanic, ECommonBadState));
		RSettableParameterMessage message(EIpcSessionApiExtIfaceClose);
		routingMsg->KickOffL(message, *this, entry);
		iApiExtRegister.DeregisterClosedInterface(entry);
		}
	}

//
//
EXPORT_C CWorkerSubSession* CWorkerSession::SubSessionFromHandle(TUint aHandle, TSubSessInfo aType) const
    {
    SubSessions().Lock();
	CWorkerSubSession* subSess = iSubSessions.At(aHandle, aType);
	SubSessions().Unlock();
	return subSess;
	}


/** Requests that require protocol information can't be satisfactorally answered during initial server configuration
so get parked for re-processing when configuration completes. If the server is configured or parking fails then
the return code for client completion is set to the given error.
@param aMessage The client request which couldn't immediately be serviced.
@param aErrorIfConfigured Error code to return to the client if configuration is complete or parking fails.
*/
EXPORT_C void CWorkerSession::ParkIfIndeterminateRequest(const RMessage2& aMessage, TInt aErrorIfConfigured)
	{
	if(!PitBoss().ModuleConfigurationComplete() &&
		Dealer()->ParkRequest(this, aMessage, CCommonDealer::EIndeterminateDuringBoot) == KErrNone)
		{
		DontCompleteCurrentRequest();
		}
	else
		{
		SetReturn(aErrorIfConfigured);
		}
	}

/**
Constructor
*/
EXPORT_C CWorkerSubSession::CWorkerSubSession(CWorkerSession* aSession, CCommonPlayer* aPlayer, TSubSessionUniqueId aSubSessionUniqueId)
:	iSession(aSession),
	iPlayer(aPlayer),
	iSubSessionUniqueId(aSubSessionUniqueId),
	iNextScopeUniqueId(1)
	{
	COMMONLOG((Session()->WorkerId(),KECommonServerTag, _L8("CWorkerSubSession(%08x):\tCWorkerSubSession() Session(%08x)"), this, iSession));
	}

EXPORT_C void CWorkerSubSession::ConstructL()
	{
	iSessionProxy = Player().CurrentSessionProxyL();
	// Try reserving enough space in the transport sender queue so that we don't overflow the queue
	// when sending messages in bulk, which can happen in cases like closing the session without 
	// closing individual sub-sessions. Protyping reveals that with a high number of number of 
	// sub-sessions, a transport queue length twice the number of active sub-session is a safe bet
	CCommsTransport* transport = iPlayer->WorkerThread().Transport();
	TInt numSubsession = iSession->SubSessions().ActiveCount() + 1;
	const TInt numUndeliveredMessae = numSubsession * 2;
	transport->PreallocateQueueSpaceL(numUndeliveredMessae);
	}

EXPORT_C CWorkerSubSession::~CWorkerSubSession()
	{
	// Session may be disappeared by now, so DO NOT USE IT
	COMMONLOG((CommsFW::KInvalidWorkerId, KECommonServerTag, _L8("CWorkerSubSession(%08x):\t~CWorkerSubSession() Session(%08x)"), this, iSession));

	//All interfaces must be closed by now (by the derived subsession's destructor the latest)
	__ASSERT_DEBUG(iApiExtRegister.InterfaceCount() == 0, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 15));

	// Remove from the Player's container
	CCommonPlayer::TSubSessionContainer& subSessions = Player().SubSessions();
	TInt sessIdx = subSessions.Find(this);
	if(sessIdx >= 0)
		{
		subSessions.Remove(sessIdx);
		if(subSessions.Count() == 0)
			{
			CCommonWorkerThread& workerThread = Player().WorkerThread();
			if(workerThread.ShuttingDown())
				{
				workerThread.MaybeTriggerThreadShutdownCallback();
				}
			}
		}
	}

EXPORT_C void CWorkerSubSession::SetUniqueId(const TSubSessionUniqueId& aUniqueId)
	{
	if (iSubSessionUniqueId==0)
		{
		iSubSessionUniqueId = aUniqueId;
		}
	}

/**
Complete the RMessage2 if the sub-session is not Orphaned (the session have not been deleted).
*/
EXPORT_C void CWorkerSubSession::CompleteMessage(const RMessage2& aMessage, TInt aResult)
	{
	if(!aMessage.IsNull())
		{
		COMMONLOG((Session()->WorkerId(), KECommonSessDetailTag, _L8("CWorkerSubSession(%08x):\tCompleteMessage(%08x) with %d, session %08x."), this, aMessage.Handle(), aResult, iSession) );
		aMessage.Complete(aResult);
		}
	}

EXPORT_C const RSafeMessage& CWorkerSubSession::SafeMessage() const
	{
	return Player().SafeMessage();
	}

EXPORT_C void CWorkerSubSession::DontCompleteCurrentRequest() const
	{
	Player().DontCompleteCurrentRequest();
	}

EXPORT_C void CWorkerSubSession::SetReturn(TInt aComplete/*,TInt aError,TUid aOwner*/) const
	{
	Player().SetReturn(aComplete/*, aError, aOwner*/);
	}

EXPORT_C void CWorkerSubSession::PanicClient(TInt aPanic)
	{
	__ASSERT_DEBUG(iPlayer, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 16));
	iPlayer->PanicClient(*this, aPanic);
	}

EXPORT_C void CWorkerSubSession::PanicClient(const TDesC& aCategory, TInt aPanic)
	{
	__ASSERT_DEBUG(iPlayer, User::Panic(KSpecAssert_ElemSvrDenWrkrSC, 17));
	iPlayer->PanicClient(*this, aCategory, aPanic);
	}

/**
Stores information about the owner of this sub-session
The id of the thread that owns this sub-session is retrieved.
The session is also asked to store information about it's owner
if it hasn't done so already.
*/
EXPORT_C void CWorkerSubSession::StoreOwnerInfo()
	{
	RThread thread;
	SafeMessage().Client(thread);
	iThreadId = thread.Id();
	thread.Close();
	}

/**
Retrieves the information about the owner of this sub-session that was
previously discovered on a call to StoreOwnerInfo()
@param aProcessId the process id of the session's owner i.e. the process which owns the RSocket
@param aUidType the set of uids belonging to the session's owner
@param aThreadId the thread id of the sub-session's owner
@return ETrue if the owner information is valid, otherwise EFalse
*/
EXPORT_C void CWorkerSubSession::GetOwnerInfo(TProcessId& aProcessId, TUidType& aUidType, TThreadId& aThreadId) const
	{
	aThreadId = iThreadId;
	iSession->GetOwnerInfo(aProcessId, aUidType);
	}



/** Form a reference name for the subsession with the necessary contents to later locate it
through DecomposeSubSessionName()
@param aSubSess Subsession to form the name for
@param aName Storage for generated name
@see DecomposeSubSessionName()
*/
EXPORT_C void CWorkerSubSession::ComposeSubSessionName(const CWorkerSubSession* aSubSess, TName& aName)
	{
	// The subsession name is the the Worker's id and the subsession ptr, separated by a space. We do not
	// document these details with in-source doc as this is explicitly an opaque reference to all clients
	// so there should be no hint anyway that the layout is reliable
	_LIT(KQualifiedNameFormat, "%d %x");
	aName.Format(KQualifiedNameFormat, aSubSess->Player().WorkerId(), aSubSess);
	}

/** Extract the Player's worker id and subsession pointer from a subsession name composed by ComposeSubsessionName()
Note that values are only validated for plausibility and MUST not be trusted without appropriate further checks (eg
try finding the subsession pointer in the worker's Player's subsession container before dereferencing it)
@param aName Composed subsession name
@param aWorkerId Worker thread id extracted from the subsession name
@param aSubSess Subsession pointer extracted from the subsession name
@return KErrNone if subsession name was readable (see caution about its validity in the description), or an error code
@see ComposeSubsessionName()
*/
TInt CWorkerSubSession::DecomposeSubSessionName(const TName& aName, TWorkerId& aWorkerId, CWorkerSubSession*& aSubSess)
	{
	TLex lex(aName);
	TUint workerId;
 	TInt ret = lex.Val(workerId, EDecimal);
 	if(ret == KErrNone)
 		{
 		lex.SkipSpace();
 		TUint subSessAddr;
 		ret = lex.Val(subSessAddr, EHex);
 		if(ret == KErrNone)
 			{
			// Lightweight check of plausibility
			if(workerId < TWorkerThreadPublicInfo::ENullWorkerId &&
				workerId <= TWorkerThreadPublicInfo::EMaxWorkerThreadId &&
				subSessAddr != 0)
				{
				aWorkerId = (TWorkerId)workerId;
				aSubSess = reinterpret_cast<CWorkerSubSession*>(subSessAddr);
				}
			else
				{
				ret = KErrBadName;
				}
			}
		}
	return ret;
	}

/** Safely retrieve a subsession pointer from a from a subsession name composed by ComposeSubsessionName(), validating
that pointer is valid and optionally that it is of the expected type.
@param aName Composed subsession name
@param aType Required subsession type or TSubSessionType::EAny to match any type
@param aWorker Own worker thread
@param aSubSess Subsession pointer fetched from the subsession name
@return KErrNone if subsession successfully fetched, or an error code
@see ComposeSubsessionName()
*/
EXPORT_C TInt CWorkerSubSession::FetchSubSessionFromName(const TName& aName, TSubSessInfo aType, const CCommonWorkerThread& aWorker, CWorkerSubSession*& aSubSess)
	{
	TWorkerId workerId;
	TInt ret = DecomposeSubSessionName(aName, workerId, aSubSess);
	if(ret == KErrNone)
		{
 		// Verify that the given address is a socket
		CCommonPlayer* player = aWorker.PitBoss().Player(workerId);
		if(!player ||
			player->SubSessions().Find(aSubSess) < 0 ||
			(aType.iType != TSubSessInfo::EAny && aType.iType != aSubSess->Type().iType))
			{
			ret = KErrBadName;
			}
		}
 	return ret;
	}



EXPORT_C void CWorkerSession::DontCompleteCurrentRequest()
	{
	iComplete=EFalse;
	}

EXPORT_C void CWorkerSession::SetReturn(TInt aReturn /*,TInt anError,TUid anOwner*/)
	{
	iReturn=aReturn;
	}

/**
Returns the session's owner information discovered on a previous call to StoreOwnerInfo().
*/
EXPORT_C void CWorkerSession::GetOwnerInfo(TProcessId& aProcessId, TUidType& aUidType) const
	{
	aProcessId = iProcess.Id();
	aUidType = iUidType;
	}

void CWorkerSession::CompleteDisconnect()
	{
	RMessage2 dm(iDisconnectMessage);
	iDisconnectMessage = RMessage2();

	// Store the ID locally because our object will be deleted after the call to the base class...
	TWorkerId id = WorkerId();

	COMMONLOG((id, KECommonSessDetailTag, _L8("CWorkerSession(%08x):\tCompleteDisconnect - message (%08x)"), this, dm.Handle()));

	CSession2::Disconnect(dm);

	COMMONLOG((id, KECommonSessDetailTag, _L8("CWorkerSession(%08x):\tCompleteDisconnect - session closed"), this));
	}





