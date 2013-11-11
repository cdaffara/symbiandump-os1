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


#ifndef __TE_GESTUREGENERATOR_H__
#define __TE_GESTUREGENERATOR_H__


#include "te_graphicsperformanceSuiteStepBase.h"


class GestureGenerator
	{
public:
	static void SimulateFlickGestureL(RSemaphore& aSemaphore, TPoint aStartPoint, TPoint aEndPoint);
	
private:
	static void GenerateEventL(RSemaphore& aSemaphore, TRawEvent::TType aEventType, TInt aX, TInt aY);
	};

#endif
