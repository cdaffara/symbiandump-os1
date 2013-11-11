/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBS_LOCMONITORDBTIMER_H
#define LBS_LOCMONITORDBTIMER_H



//-------------------------------------------------------------------------------
#include <e32base.h>


//-------------------------------------------------------------------------------
/** 
*/

class MLocMonitorDBCallback
    {
public:
    virtual void FlushTimerCallback() = 0; 
    };



class CLbsLocMonitorDbTimer : public CActive
	{

public:		
	static CLbsLocMonitorDbTimer* NewL(MLocMonitorDBCallback& aObserver);
	virtual ~CLbsLocMonitorDbTimer();
	TBool IsRunning();
	TBool StartTimer(TTimeIntervalSeconds aTimeInterval);


private:
	CLbsLocMonitorDbTimer(MLocMonitorDBCallback& aObserver);
	void ConstructL();
	virtual void RunL();
	virtual void DoCancel();
	
	
private:
	MLocMonitorDBCallback& iObserver;
	RTimer iTimer;
	TBool iRunning;
	};
	



#endif //LBS_LOCATIONMONITORDBTIMER_H
