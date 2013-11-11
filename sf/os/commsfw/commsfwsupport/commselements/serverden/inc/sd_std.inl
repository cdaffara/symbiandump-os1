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
// CCommonServer
// 
//

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenStdI, "ElemSvrDenStdI");
#endif

inline CCommonWorkerThread& CCommonServer::WorkerThread() const
	{
	return *iOwnerThread;
	};

inline CommsFW::TWorkerId CCommonServer::WorkerId() const
	{
	return WorkerThread().WorkerId();
	}

//
// CWorkeerSession
//
inline CCommonDealer* CWorkerSession::Dealer() const
	{
	// NB: this is really only a static_cast<> but the Gordian knot of class dependencies results in a forward
	// declaration
	CServer2* server=const_cast<CServer2*>(Server());
	CCommonServer* commonServer=static_cast<CCommonServer*>(server);
	return commonServer->Dealer();
	}

inline CSubSessionIx& CWorkerSession::SubSessions()
	{
	return iSubSessions;
	}

inline const CSubSessionIx& CWorkerSession::SubSessions() const
	{
	return iSubSessions;
	}

inline const RMessage2& CWorkerSession::Message() const
	{
	return *iOurMessage;
	}

inline RSafeMessage& CWorkerSession::SafeMessage() const
	{
	return static_cast<RSafeMessage&>(const_cast<RMessage2&>(*iOurMessage));
	}

inline RSafeMessage& CWorkerSession::SafeMessage(const RMessage2& aMessage) const
	{
	return static_cast<RSafeMessage&>(const_cast<RMessage2&>(aMessage));
	}

inline CCommonWorkerThread& CWorkerSession::WorkerThread() const
	{
	//return Dealer().WorkerThread();

	CServer2* server=const_cast<CServer2*>(Server());
	CCommonServer* commonServer=static_cast<CCommonServer*>(server);
	return commonServer->WorkerThread();
	}

inline CommsFW::TWorkerId CWorkerSession::WorkerId() const
	{
	//return Dealer().WorkerId();

	CServer2* server=const_cast<CServer2*>(Server());
	CCommonServer* commonServer=static_cast<CCommonServer*>(server);
	return commonServer->WorkerId();
	}

inline CCommonPitBoss& CWorkerSession::PitBoss() const
	{
	return WorkerThread().PitBoss();
	}

//
// CWorkerSubSession
//


inline CWorkerSession* CWorkerSubSession::Session()
	{
	__ASSERT_DEBUG(iSession, User::Panic(KSpecAssert_ElemSvrDenStdI, 1));
	return iSession;
	}

inline const CWorkerSession* CWorkerSubSession::Session() const
	{
	__ASSERT_DEBUG(iSession, User::Panic(KSpecAssert_ElemSvrDenStdI, 2));
	return iSession;
	}

inline CCommonPlayer& CWorkerSubSession::Player() const
	{
	__ASSERT_DEBUG(iPlayer, User::Panic(KSpecAssert_ElemSvrDenStdI, 3));
	return *iPlayer;
	}


inline void CWorkerSubSession::SetSession(CWorkerSession* aSession)
	{
	iSession = aSession;
	}

inline CCommonSessionProxy* CWorkerSubSession::SessionProxy()
	{
	return iSessionProxy;
	}

inline const CCommonSessionProxy* CWorkerSubSession::SessionProxy() const
	{
	return iSessionProxy;
	}

inline void CWorkerSubSession::SetSessionProxy(CCommonSessionProxy* aProxy)
	{
	iSessionProxy = aProxy;
	}

// migrate to using SafeMessage
/*
inline const RMessage2& CWorkerSubSession::Message() const
	{
	return static_cast<RMessage2&>(const_cast<RSafeMessage&>(SafeMessage()));
	}
*/

//
// CSubSessionIx
//

/**
Optional locking. When accessing any of the public methods of this class in a multithreaded
environment, locking is often necessary. This lock does not allow for lock nesting.
*/
inline void CSubSessionIx::Lock() const
	{
	iLock.Wait();
	}

/**
Unlocking.
@see Lock()
*/
inline void CSubSessionIx::Unlock() const
	{
	iLock.Signal();
	}

/**
@return the number of active sub-sessions in the container.
*/
inline TInt CSubSessionIx::ActiveCount() const
	{
	return iActiveCount;
	}

