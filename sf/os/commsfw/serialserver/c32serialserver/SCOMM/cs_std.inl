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
// CC32Server
// 
//


inline CC32WorkerThread& CC32Server::WorkerThread() const 
	{
	return *iOwnerThread;
	};

inline CommsFW::TWorkerId CC32Server::WorkerId() const
	{
	return WorkerThread().WorkerId();
	}

//
// CCommSession
//

inline const CC32Dealer& CCommSession::Dealer() const 
	{ 
	return *reinterpret_cast<const CC32Dealer*>(Server());	
	}

inline CC32SubSessionIx& CCommSession::SubSessions()
	{
	return iSubSessions;
	}

inline void CCommSession::CompleteDisconnect()
	{
	RMessage2 dm = iDisconnectMessage;
	inherited::Disconnect(dm);
	}


//
// CCommSubSession
//

inline void CCommSubSession::Open()
	{
	iAccessCount++;
	}

inline void CCommSubSession::Close()
	{
	iAccessCount--;
	ASSERT(iAccessCount >= 0);
	if(iAccessCount == 0)
		{
		RemoveAndDestroy();
		}
	}

inline TInt CCommSubSession::AccessCount() const
	{
	return iAccessCount;
	}

inline CCommSession* CCommSubSession::Session() const
	{ 
	ASSERT(iSession); 
	return iSession;
	}

inline CC32Player& CCommSubSession::Player() const 
	{ 
	ASSERT(iPlayer); 
	return *iPlayer; 
	}

	
//
// CC32SubSessionIx
// 

/**
Optional locking. When accessing any of the public methods of this class in a multithreaded 
environment, locking is often necessary. This lock does not allow for lock nesting.
*/
inline void CC32SubSessionIx::Lock() const
	{ 
	iLock.Wait(); 
	}

/**
Unlocking.
@see Lock()
*/
inline void CC32SubSessionIx::Unlock() const
	{ 
	iLock.Signal(); 
	}


//
// RWorkerLock
//

inline void CC32SubSessionIx::RWorkerLock::Wait()
	{
#ifdef _DEBUG
	TThreadId self = RThread().Id();
	// If this faults then you're attempting to claim a lock you already hold. See below
	__ASSERT_ALWAYS(self != iHolder,Fault(EBadState,_L8("RWorkerLock::Wait: Attempt to claim a lock twice (self == iHolder(%lu))! Panicking."),&iHolder));
#endif
	RFastLock::Wait();
#ifdef _DEBUG
	iHolder = self;
#endif
	}



inline void CC32SubSessionIx::RWorkerLock::Signal()
	{
#ifdef _DEBUG
	TThreadId self = RThread().Id();
	// If this faults then you're attempting to release a lock you do not hold.
	__ASSERT_ALWAYS(self == iHolder,Fault(EBadState,_L8("RWorkerLock::Signal: Attempt to release a lock not held (self(%lu) != iHolder(%lu))! Panicking."),&self,&iHolder));
	iHolder = KNullHandle;
#endif
	RFastLock::Signal();
	}
	
	
		
inline void CC32SubSessionIx::RWorkerLock::AssertLockHeld() const
	{
#ifdef _DEBUG
	TThreadId self = RThread().Id();
	__ASSERT_ALWAYS(self == iHolder,Fault(EBadState,_L8("RWorkerLock::AssertLockHeld: failed: self(%lu) != iHolder(%lu)! Panicking."),&self,&iHolder));
#endif
	}
	
	


