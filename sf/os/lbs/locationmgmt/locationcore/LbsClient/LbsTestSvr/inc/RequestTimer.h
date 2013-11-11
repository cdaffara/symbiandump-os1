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

#ifndef __REQUESTTIMER_H__
#define __REQUESTTIMER_H__

#include <e32base.h>

class MRequestTimerUser
	{
public:
	virtual void TimerCompleted(const RMessage2& aMessage) = 0;
	};

class CRequestTimer : public CTimer
	{
public:
	CRequestTimer();
	~CRequestTimer();
	void ConstructL();
	void Request(const RMessage2& aMessage, MRequestTimerUser* aSvrObject);
	void CancelMessage();
private:
	//from CActive
	void RunL();
	TInt RunError(TInt aError);
private:
	RMessage2 iMessage;
	MRequestTimerUser* iSvrObject;
	};

#endif // __REQUESTTIMER_H__
