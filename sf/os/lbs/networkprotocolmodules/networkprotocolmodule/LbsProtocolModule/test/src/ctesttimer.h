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
// This file defines a timer class used during tests
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef __CTESTTIMER_H__
#define __CTESTTIMER_H__

#include <e32base.h>


//-----------------------------------------------------------------------------
// MTestTimerObserver
//-----------------------------------------------------------------------------
class MTestTimerObserver
{
public:
	virtual void OnTimerEvent(TInt aEventId) = 0;
}; 


//-----------------------------------------------------------------------------
// CTestTimer
//-----------------------------------------------------------------------------
NONSHARABLE_CLASS(CTestTimer) : public CTimer
{

public:
	static CTestTimer* NewL(TInt aEventId, MTestTimerObserver& aObserver);
	void After(TTimeIntervalMicroSeconds32 aInterval);
	
protected:
	CTestTimer(TInt aEventId, MTestTimerObserver& aObserver);
	void ConstructL();
	virtual void RunL();
	
protected:
	TInt iEventId;
	MTestTimerObserver& iObserver;
};


#endif // __CTESTTIMER_H__
