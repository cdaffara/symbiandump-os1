// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TIMEOUT_H__
#define __TIMEOUT_H__

// Class definitions to support the recive with timeout methods

#include <e32base.h>

// MTimeOutNotify: used in conjunction with CTimeOutTimer class
class MTimeOutNotify
	{
public:
	virtual void TimerExpired() = 0;
	};

// CTimeOutTimer: timer for comms time-outs
class CTimeOutTimer: public CTimer
	{
public:
	static CTimeOutTimer* NewL(MTimeOutNotify& aTimeOutNotify);
	~CTimeOutTimer();

protected:
    CTimeOutTimer(MTimeOutNotify& aTimeOutNotify);
	void ConstructL();
    virtual void RunL();

private:
	MTimeOutNotify& iNotify;
	};

#endif // __TIMEOUT_H__
