// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTSIMTIMER_H__
#define BTSIMTIMER_H__

#include <e32base.h>

class CBTPortProxy;

/**
	CBTTimerSimple is in effect an alarm.
	It will signal back after the alarm has went off, calling CBTProxy::Alarm().
*/ 
NONSHARABLE_CLASS(CBTTimerSimple) : public CTimer
	{
public:
	virtual ~CBTTimerSimple();
	static CBTTimerSimple* NewL(TInt aPriority, CBTPortProxy* aContext);
	virtual void CancelAlarm();
private:
	void RunL();
	CBTTimerSimple(TInt aPriority, CBTPortProxy* aContext);
	void ConstructL();
private:
	CBTPortProxy* iClient;
	};


#endif
