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
// Definition of power advice handler sub-component of agps manager
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef LBSPOWERADVICEHANDLER_H
#define LBSPOWERADVICEHANDLER_H

#include <e32base.h>

#include "LbsInternalInterface.h"

class MPowerModeObserver
	{
public:

	virtual void PowerModeAdvice(CLbsLocationSourceGpsBase::TPowerMode& aPowerMode) = 0;
	};
	

class CPowerModeAdviceHandler :	public CActive
	{
public:
	static CPowerModeAdviceHandler* NewL(MPowerModeObserver* aObserver);
	
	~CPowerModeAdviceHandler();
	void StartGettingPowerUpdates();
protected:
	CPowerModeAdviceHandler(MPowerModeObserver* aObserver); 
	void ConstructL();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	
private:
	MPowerModeObserver* iObserver;
	RLbsManagerPowerModeAdvice iManagerPowerModeAdvice;

	};

#endif // LBSPOWERADVICEHANDLER_H

