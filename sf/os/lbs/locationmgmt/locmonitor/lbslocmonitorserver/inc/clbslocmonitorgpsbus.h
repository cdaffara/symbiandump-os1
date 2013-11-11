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


/**
@file
@internalComponent
@released
*/
#ifndef CLBSLOCMONITORGPSBUS_H
#define CLBSLOCMONITORGPSBUS_H

#include <e32base.h>
#include "LbsInternalInterface.h"

class MLbsLocMonitorGpsBusObserver;

/**
 Active object to listen on GPS updates bus
*/
class CLbsLocMonitorGpsBus : public CActive
	{

public:

	static CLbsLocMonitorGpsBus* NewL(MLbsLocMonitorGpsBusObserver& aObserver);
	~CLbsLocMonitorGpsBus();

private:

	CLbsLocMonitorGpsBus(MLbsLocMonitorGpsBusObserver& aObserver);
	void ConstructL();

private:

	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:

	/** Data bus to subscribe to in order to receive
	 notifications of gps-calculated positions.
	 */
	RLbsPositionUpdates			iGpsPositionBus;	

	MLbsLocMonitorGpsBusObserver& iObserver;
	};

/**
The Mixin observer interface for the location data bus object.

@internalComponent
@released
*/
class MLbsLocMonitorGpsBusObserver
	{
public:
	virtual void GpsPositionAvailableL(const TPosition& aPosition) = 0;
	};

#endif // CLBSLOCMONITORGPSBUS_H