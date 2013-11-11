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
#include <e32base.h>
#include "clbslocmonitorposlistener.h"
#include "lbsdevloggermacros.h"


CLbsLocMonitorPosListener* CLbsLocMonitorPosListener::NewL(MLbsLocMonitorPosListenerObserver& aObserver)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorPosListener::NewL");
	CLbsLocMonitorPosListener* self = new(ELeave) CLbsLocMonitorPosListener(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CLbsLocMonitorPosListener::CLbsLocMonitorPosListener(MLbsLocMonitorPosListenerObserver& aObserver):
							iObserver(aObserver)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorPosListener::CLbsLocMonitorPosListener");
	}
	
void CLbsLocMonitorPosListener::ConstructL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorPosListener::ConstructL");
	iGpsBus = 	CLbsLocMonitorGpsBus::NewL(*this);
	}
	
CLbsLocMonitorPosListener::~CLbsLocMonitorPosListener()
	{
	delete 	iGpsBus;
	}
	
void CLbsLocMonitorPosListener::GpsPositionAvailableL(const TPosition& aPosition)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorPosListener::GpsPositionAvailableL");
	if (Math::IsNaN(aPosition.Latitude()) ||
        Math::IsNaN(aPosition.Longitude()))
		{
		// Donnot send invalid positions to observer
		return;			
		}
	
	if ((aPosition.Latitude() != iLastGpsPosition.Latitude()) ||
		(aPosition.Longitude() != iLastGpsPosition.Longitude()))
		{
		iObserver.PositionAvailableL(aPosition);
		iLastGpsPosition = aPosition;
		}
	}


