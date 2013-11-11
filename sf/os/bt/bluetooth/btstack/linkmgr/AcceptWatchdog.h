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
// Defines the accept watchdog timer for baseband SAPs
// to guard against:
// 1) race condition in accepting a link just as the Controller drops the link
// automatically due to it Accept Timer firing
// 2) Broken HW not delivering this important event
// 3) HW that reports only via a useless Command Status event
// 
//

#include <e32base.h>
class CBTBasebandSAP;

#ifndef _ACCEPTWATCHDOG_H
#define _ACCEPTWATCHDOG_H

static const TInt KAcceptWatchdogTimeout = 60000000; // microsecondseconds(have to cover the case of security mode3)

NONSHARABLE_CLASS(TAcceptWatchdog)
	{
public:
	explicit TAcceptWatchdog(CBTBasebandSAP& aParent);
	static TInt AcceptTimeout(TAny* aTAcceptWatchdog);
	void Start();
	void Cancel();
#ifdef _DEBUG
	~TAcceptWatchdog();
#endif

private:
	TAcceptWatchdog(); // want to hide
private:
	CBTBasebandSAP&		iParent;
	TDeltaTimerEntry	iIdleTimerEntry;
	TBool				iActive;
	};


#endif //_ACCEPTWATCHDOG_H
