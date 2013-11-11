// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __RVOBSERVER_H__
#define __RVOBSERVER_H__

#include <e32base.h>
#include <apastarter.h>
#include <e32debug.h>

#include "timeoutwaiternotification.h"
#include "rvobservernotification.h"

/**
@internalTechnology
@released
*/
enum TRvObserverState
	{
	ERvObserverStateNotInvoked = 0,
	ERvObserverStateAwaitingRv
	};
	
	
	
class CTimeoutWaiter;
class CSsmStartupProperties;

NONSHARABLE_CLASS( CRvObserver ) : public CActive, public MTimeoutWaiterNotification
	{
public:
	static CRvObserver* NewL( RProcess& aProcess, MRvObserverNotification& aObserved, CApaStarter* aApaStarter );
	static CRvObserver* NewLC( RProcess& aProcess, MRvObserverNotification& aObserved, CApaStarter* aApaStarter );
	~CRvObserver();
	
	void RvAndObserveL( CSsmStartupProperties* aStartupProperties );
	// From MTimeoutWaiterNotification
	void NotifyTimeout();

protected:
	void RunL();
	void DoCancel();
	TInt RunError( TInt aError );

private:
	CRvObserver( RProcess& aProcess, MRvObserverNotification& aObserved, CApaStarter* aApaStarter );
	void ConstructL();
	
	void InvokeL();
	TInt GetProcessHandle( RProcess& aProcess, const TThreadId& aThreadId );
	void NotifyOrRetryL();
	TBool GoodStart();
	
private:
	CTimeoutWaiter* iTimeoutWaiter;
	CApaStarter* iApaStarter; // not owning
	CSsmStartupProperties* iStartupProperties; // Not owning.
	MRvObserverNotification& iStarter;
	TInt iHandleNumber;
	TThreadId iThreadId;
	RProcess& iProcess;
	TRvObserverState iObserverState;
	TInt iRetries;
	TBool iTimeOut;
	};

	
	
#endif // __RVOBSERVER_H__

