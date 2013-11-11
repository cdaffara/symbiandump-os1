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

#ifndef LBSTIMER_H
#define LBSTIMER_H

#include <e32base.h>

class MLbsTimerObserver
	{
public:
	virtual void HandleTimer(TInt aTimerId, const TTime& aTargetTime) = 0;
	};

class CLbsTimer : public CTimer
	{
public:
	static CLbsTimer* NewL(MLbsTimerObserver* aObserver, TInt aTimerId);
	
	~CLbsTimer();
	
	void SetTimer(const TTime& aUtcTargetTime);

	void CancelTimer();

protected:
	CLbsTimer(MLbsTimerObserver* aObserver, TInt aTimerId);
	void ConstructL();
	void RunL();
	
private:
	MLbsTimerObserver* iObserver;
	TInt  iTimerId;
	TTime iTargetTime;
	};

#endif // LBSTIMER_H

