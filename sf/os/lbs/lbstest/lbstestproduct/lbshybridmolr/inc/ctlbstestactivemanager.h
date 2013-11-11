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

#ifndef __CT_LBS_TEST_ACTIVE_MANAGER_H__
#define __CT_LBS_TEST_ACTIVE_MANAGER_H__

#include <e32base.h>
#include <lbs/test/posserverwatch.h>
#include "ctlbshybridmolrstep.h"

class MTestFlowObserver
	{
public:
	virtual void OnReadyL(TInt aObjectId) = 0;
	virtual void OnTimedOutL(TInt aObjectId) = 0;
	virtual void OnGetLastKnownPositionL(TInt aObjectId, TInt32 aErr, const TPositionInfoBase& aPosInfo) = 0;
	virtual void OnNotifyPositionUpdateL(TInt aObjectId, TInt32 aErr, const TPositionInfoBase& aPosInfo) = 0;
	virtual void StopTest() = 0;
	virtual void OnSignalNetworkStep(TInt aSessionId, TInt aSessionStep) = 0;
	};

class CT_LbsTestActiveManager : public CTimer, public MPosServerObserver
	{
public:
	static CT_LbsTestActiveManager* NewL(TInt aObjectId, MTestFlowObserver& aObserver, TPriority aPriority, CTestStep& aStep);
	~CT_LbsTestActiveManager();
	
	void StartL(TTimeIntervalMicroSeconds32 aDelay);
	void WaitForPosition(TTimeIntervalMicroSeconds32 aDelay);
	void Deactivate();
	TBool IsDeactivated();

	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);

private:
	CT_LbsTestActiveManager(TInt aObjectId, MTestFlowObserver& aObserver, TPriority aPriority, CTestStep& aStep);
	
	void RunL();
	TInt RunError(TInt aError);
	
	
private:
	enum TState
		{
		EStateNone,
		EStateWaitingToStart,
		EStateWaitingForPosition
		};

private:
	TState iState;
	TInt iObjectId;
	TBool iWaiting;
	TBool iIsDeactivated;
	MTestFlowObserver& iObserver;
	CTestStep& iStep;
	};

#endif __CT_LBS_TEST_ACTIVE_MANAGER_H__
