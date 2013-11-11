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
// @file ctlbstimerutils.h
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_LBS_TIMER_UTILS_H__
#define __CT_LBS_TIMER_UTILS_H__

#include <e32base.h>

class MT_LbsTimerUtilsObserver
	{
public:
	virtual void HandleTimerL(TInt aTimerId, const TTime& aTargetTime) = 0;
	};


class CT_LbsTimerUtils : public CTimer
	{
public:
	IMPORT_C static CT_LbsTimerUtils* NewL(MT_LbsTimerUtilsObserver* aObserver, TInt aTimerId);
	
	IMPORT_C ~CT_LbsTimerUtils();
	
	IMPORT_C void SetTimer(const TTimeIntervalMicroSeconds32 aPeriod);
	IMPORT_C void SetTimer(const TTime& aUtcTargetTime);

	IMPORT_C void CancelTimer();

protected:
	CT_LbsTimerUtils(MT_LbsTimerUtilsObserver* aObserver, TInt aTimerId);
	void ConstructL();
	
	void RunL();
	
private:
	MT_LbsTimerUtilsObserver* iObserver;
	TInt  iTimerId;
	TTime iTargetTime;
	};

#endif //__CT_LBS_TIMER_UTILS_H__
