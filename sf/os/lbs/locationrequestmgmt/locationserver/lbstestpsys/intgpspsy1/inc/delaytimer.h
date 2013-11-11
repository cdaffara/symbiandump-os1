// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DELAYTIMER_H__
#define __DELAYTIMER_H__

#include <e32base.h>

class MDelayTimerUser
{
public:
	virtual void        TimerCompleted(TInt aTimerId) = 0;
};

class CDelayTimer : public CTimer
{
public:
	CDelayTimer();
	~CDelayTimer();
	void                ConstructL();
	void                Start(const TInt aTimerId, 
                              const TTimeIntervalMicroSeconds32 aDelay,
                              MDelayTimerUser* aSvrObject);
private:
	//from CActive
	void                RunL();
	TInt                RunError(TInt aError);
private:
	TInt                iTimerId;
	MDelayTimerUser*    iSvrObject;
};

#endif // __DELAYTIMER_H__
