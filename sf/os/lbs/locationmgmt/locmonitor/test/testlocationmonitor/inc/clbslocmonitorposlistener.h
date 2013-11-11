/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef LBSLOCMONITORPOSLISTENER_H
#define LBSLOCMONITORPOSLISTENER_H

#include <e32base.h>
#include "lbsnetinternalapi.h"
#include "clbslocmonitorgpsbus.h"
class MLbsLocMonitorPosListenerObserver
	{
public:
	virtual void PositionAvailableL(const TPosition& aPosition) = 0;	
	};

/**
A class that subscribes to all the sources of position
information available within LBS (currently this is the
gps  bus)

@see CLbsLocMonitorServer
@internalComponent
@prototype
*/
class CLbsLocMonitorPosListener :	public MLbsLocMonitorGpsBusObserver
	{
	
public:

	static CLbsLocMonitorPosListener* NewL(MLbsLocMonitorPosListenerObserver& aObserver);
	~CLbsLocMonitorPosListener();

	// From MLbsLocMonitorGpsBusObserver
	void GpsPositionAvailableL(const TPosition& aPosition);
private:

	CLbsLocMonitorPosListener(MLbsLocMonitorPosListenerObserver& aObserver);
	void ConstructL();

private:

	//Bus used to broadcast position within LBS
	CLbsLocMonitorGpsBus*			iGpsBus;
private:

	// Observer interested in knowing about newly arrived
	// positions (the Location Monitor)
	MLbsLocMonitorPosListenerObserver& iObserver;
	
	TPosition iLastGpsPosition;
	};
	
	
#endif //LBSLOCMONITORPOSLISTENER_H
