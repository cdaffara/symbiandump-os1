// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ACTIVEWAITER_H__
#define __ACTIVEWAITER_H__

#include <e32base.h>
#include <e32debug.h>


/**
A generic active object that can be used to wait for an iStatus to be signalled.
This has a similar effect to User::WaitForRequest(), but works without
blocking the thread (which allows other active objects within the thread
to continue).  Instead, a nested active scheduler loop is used.

To use a CActiveWaiter, pass its iStatus to an asynchronous method, as usual.
Then call WaitActive() instead of SetActive().

The call will block until the asynchronous call completes.

E.g.

void CMyClass::Foo()
	{
	CActiveWaiter* waiter = new(ELeave)CActiveWaiter;

	iAsyncObj->Bar(waiter->iStatus);
	waiter->WaitActive();
	
	delete waiter;
	}
@internalTechnology
@released
*/    
NONSHARABLE_CLASS( CActiveWaiter ) : public CActive
	{
public:
	CActiveWaiter();
	~CActiveWaiter();
	
	void WaitActive();

protected:
	// from CActive;
	void RunL();
	void DoCancel();
	TInt RunError( TInt aError );
	
private:
	CActiveSchedulerWait iScheduler;
	};
	
#endif // __ACTIVEWAITER_H__
