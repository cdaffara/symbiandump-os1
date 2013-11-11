/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file
 @internalTechnology
*/

#ifndef __RWORKERLOCK_H__
#define __RWORKERLOCK_H__

#include <e32base.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemCommsFwrwrkr, "ElemCommsFwrwrkr");
#endif


/**
@class RWorkerLock
Thin veneer on locking code to provide for logging. Intention is to keep it inline.
*/
class RWorkerLock : public RFastLock
	{
public:
	void Wait()
		{
#ifdef _DEBUG
		TThreadId self = RThread().Id();
		__ASSERT_DEBUG(self != iHolder, User::Panic(KSpecAssert_ElemCommsFwrwrkr, 1));	// If this faults then you're attempting to claim a lock you already hold. See below
#endif
		RFastLock::Wait();
#ifdef _DEBUG
		iHolder = self;
#endif
		}

	void Signal()
		{
#ifdef _DEBUG
		TThreadId self = RThread().Id();
		__ASSERT_DEBUG(self == iHolder, User::Panic(KSpecAssert_ElemCommsFwrwrkr, 2));	// If this faults then you're attempting to release a lock you do not hold.
		iHolder = KNullHandle;
#endif
		RFastLock::Signal();
		}
	void AssertLockHeld() const
		{
#ifdef _DEBUG
		TThreadId self = RThread().Id();
		__ASSERT_DEBUG(self == iHolder, User::Panic(KSpecAssert_ElemCommsFwrwrkr, 3));
#endif
		}
#ifdef _DEBUG

	// check if the current thread already owns the lock
	TBool IsOwned() const
		{
		TThreadId threadId = RThread().Id();
		return IsOwned(threadId);
		}

	// check if the specified thread already owns the lock
	TBool IsOwned(TThreadId &aThreadId) const
		{
		return (aThreadId == iHolder);
		}

private:
	// RFastLock does not nest, ie if the current holder attempts to re-acquire they deadlock
	// We check for this in debug builds only as the cost of doing so likely negates the benefits
	// of using the class over & above semaphores
	TThreadId iHolder;
#endif
	};


#endif // __RWORKERLOCK_H__

