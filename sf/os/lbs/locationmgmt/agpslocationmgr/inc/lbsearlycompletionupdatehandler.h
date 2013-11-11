// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of a handler of updates received from Location Server due to
// an early completion from the Location Server's client.
// The handler is an active object that listens for location updates being
// published by the Location Server on a bus to which the handler subscribes.
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __LBSLOCUPDATEHANDLER_H__
#define __LBSLOCUPDATEHANDLER_H__

#include <e32base.h>
#include <lbspositioninfo.h>
#include "LbsInternalInterface.h"

class MLbsEarlyCompletionUpdateObserver
	{
public:

	// Notify observers of the update used by a LocationServer client
	virtual void EarlyCompletionUpdate
	(
	TPositionInfo& earlyCompletionUpdate,
	TTime& aTargetTime,
	TTime& aActualTime
	) = 0;
	};


class CEarlyCompletionUpdateHandler :	public CActive
	{
public:
	
	static CEarlyCompletionUpdateHandler* NewL(MLbsEarlyCompletionUpdateObserver* aObserver);
	
	~CEarlyCompletionUpdateHandler();
	
	void ListenForEarlyCompletionUpdates();

protected:
	CEarlyCompletionUpdateHandler(MLbsEarlyCompletionUpdateObserver* aObserver); 
	void ConstructL();

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
private:
	MLbsEarlyCompletionUpdateObserver* iObserver;
	RLbsPositionUpdates iEarlyCompletionPosUpdates;
	};

#endif // __LBSLOCUPDATEHANDLER_H__

