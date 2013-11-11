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

#ifndef LBS_POSITIONCALC_H
#define LBS_POSITIONCALC_H

//************************************************************************************************************
#include <e32base.h>
#include <lbsposition.h>

//-------------------------------------------------------------------------------	
/** 
@publishedAll
@released
*/
class TPositionCalc
	{
public: 
	// overloaded functions to calculate distance
	IMPORT_C static TInt GetDistance(const TCoordinate& aStartCoor, const TCoordinate& aStopCoor, TReal32& aDistance);
	IMPORT_C static TInt GetDistance(const TLocality& aStartLoc, const TLocality& aStopLoc, TReal32& aDistance, TReal32& aDelta);
	// overloaded functions to calculate bearing
	IMPORT_C static TInt GetBearing(const TCoordinate& aStartCoor, const TCoordinate& aStopCoor, TReal32& aBearing);
	IMPORT_C static TInt GetBearing(const TLocality& aStartLoc, const TLocality& aStopLoc, TReal32& aBearing, TReal32& aDelta);
	// overloaded functions to calculate speed
	IMPORT_C static TInt GetSpeed(const TPosition& aStartPos, const TPosition& aStopPos, TReal32& aSpeed);
	IMPORT_C static TInt GetSpeed(const TPosition& aStartPos, const TPosition& aStopPos, TReal32& aSpeed, TReal32& aDelta);
	// function to translate coordinate
	IMPORT_C static TInt Translate(const TCoordinate& aStartCoor, const TReal32& aDistance, const TReal32& aBearing, TCoordinate& aStopCoor);
	
private:
	}; 

#endif //LBS_POSITIONCALC_H