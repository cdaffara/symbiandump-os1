// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "te_gesturegenerator.h"

#include <e32cmn.h>
#include <e32math.h>


void GestureGenerator::SimulateFlickGestureL(RSemaphore& aSemaphore, TPoint aStartPoint, TPoint aEndPoint)
	{
	GenerateEventL(aSemaphore, TRawEvent::EButton1Down, aStartPoint.iX, aStartPoint.iY);
	TPoint currentPosition(aStartPoint);
	TReal currentX = 0;
	TReal currentY = 0;
	TReal distance = 0;
	Math::Sqrt(distance, (aEndPoint.iX - aStartPoint.iX)*(aEndPoint.iX - aStartPoint.iX) + (aEndPoint.iY - aStartPoint.iY)*(aEndPoint.iY - aStartPoint.iY));	
	
	TReal deltaX = (aEndPoint.iX - aStartPoint.iX) / distance;
	TReal deltaY = (aEndPoint.iY - aStartPoint.iY) / distance;	
	while (currentPosition != aEndPoint)
	    {
	    currentX += deltaX;
	    currentY += deltaY;
	    currentPosition.SetXY(aStartPoint.iX + currentX, aStartPoint.iY + currentY);
	    GenerateEventL(aSemaphore, TRawEvent::EPointerMove, currentPosition.iX, currentPosition.iY);
	    }	
	GenerateEventL(aSemaphore, TRawEvent::EButton1Up, aStartPoint.iX, aStartPoint.iY);
	}

/**
 * Generates events to communicate with the control. Each time the control receives an event
 * it redraws itself. That's necessary because the draw method can't be called directly from
 * a different thread.
 */
void GestureGenerator::GenerateEventL(RSemaphore& aSemaphore, TRawEvent::TType aEventType, TInt aX, TInt aY)
    {
    TRawEvent rawEvent;
    rawEvent.Set(aEventType, aX, aY);
    User::LeaveIfError(UserSvr::AddEvent(rawEvent));
    aSemaphore.Wait();
    }

