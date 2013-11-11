// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_A2DPBLUETOOTH_TIMER_H__
#define __TSU_MMF_A2DPBLUETOOTH_TIMER_H__


/**
 * Class used by the test harness to stop the active scheduler 
 * after a timeout period
 */
class CStopActiveSchedulerTimer : public CTimer
	{
public:
	static CStopActiveSchedulerTimer* NewL();
	void StopAfter(TTimeIntervalMicroSeconds32 aTimeInterval);
private:
	CStopActiveSchedulerTimer();
	void ConstructL();
	virtual void RunL();
	};

#endif /* __TSU_MMF_A2DPBLUETOOTH_H__ */
