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
//

/**
 @file
 @internalComponent
*/

#ifndef HCICMDQTIMER_H
#define HCICMDQTIMER_H

#include <e32base.h>

class CHCICommandQItem;
class MHCITimerClient;

/**
HCI Command Queue Timer	base class. It is intended that this class 
be derived from and not directly instantiated.
*/
NONSHARABLE_CLASS(CHCICmdQTimer) : public CTimer
	{
public:
	void Restart(TUint aMilliseconds, TUint aCommandQItemId, MHCITimerClient& aClient);

protected:
	CHCICmdQTimer();
	void ConstructL();

protected:
	TUint iCommandQItemId;
	MHCITimerClient* iClient;
	};

/**
HCI Command Queue starvation timer.	 
*/
NONSHARABLE_CLASS(CHCICmdQStarvationTimer) : public CHCICmdQTimer
	{
public:
	static CHCICmdQStarvationTimer* NewL();
	
private:
	CHCICmdQStarvationTimer();
	void RunL();
	};

/**
HCI Command Queue command completion timer.	 
*/
NONSHARABLE_CLASS(CHCICmdQCompletionTimer) : public CHCICmdQTimer
	{
public:
	static CHCICmdQCompletionTimer* NewL();
	
private:
	CHCICmdQCompletionTimer();
	void RunL();
	};

#endif // HCICMDQTIMER_H
