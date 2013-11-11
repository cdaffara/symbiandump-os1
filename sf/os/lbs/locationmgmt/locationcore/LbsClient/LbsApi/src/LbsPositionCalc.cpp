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

#include <e32base.h>
#include <flogger.h>
#include <f32file.h>

#include <lbs/lbspositioncalcalg.h>
#include "LbsPositionCalc.h"

//-----------------------------------------------------------------------------
// PublishAll class for using LbsMaths.dll
//-----------------------------------------------------------------------------

EXPORT_C TInt TPositionCalc::GetDistance(const TCoordinate& aStartCoor, const TCoordinate& aStopCoor, TReal32& aDistance)
/**
Calculate distance between two TCoordinates
@param aStartCoor, the start coordinate
@param aStopCoor, the stop coordinate
@param aDistance, the distance between
@return The calculation error
 */
 	{
	TReal32 tmp = 0.0;
	TInt err  = TPositionCalcAlg::CalcDistance(
					  aStartCoor.Latitude(),
					  aStartCoor.Longitude(),
					  aStartCoor.Altitude(),
					  aStartCoor.Datum(),
					  tmp,
					  tmp,
					  aStopCoor.Latitude(),
					  aStopCoor.Longitude(),
					  aStopCoor.Altitude(),
					  aStopCoor.Datum(),	
					  tmp,
					  tmp,
					  aDistance,
					  tmp
					  );
	return err;
	}
	
EXPORT_C TInt TPositionCalc::GetDistance(const TLocality& aStartLoc, const TLocality& aStopLoc, TReal32& aDistance, TReal32& aDelta)
/**
Calculate distance between two TLocality
@param aStartLoc, the start TLocality
@param aStopLoc, the stop TLocality
@param aDistance, the distance between
@param aDelta, the distance calculation error variance
@return The calculation error
 */
	{
	TInt err  = TPositionCalcAlg::CalcDistance(aStartLoc.Latitude(),
					  aStartLoc.Longitude(),
					  aStartLoc.Altitude(),
					  aStartLoc.Datum(),
					  aStartLoc.HorizontalAccuracy(),
					  aStartLoc.VerticalAccuracy(),
					  aStopLoc.Latitude(),
					  aStopLoc.Longitude(),
					  aStopLoc.Altitude(),
					  aStopLoc.Datum(),	
					  aStopLoc.HorizontalAccuracy(),
					  aStopLoc.VerticalAccuracy(),
					  aDistance,
					  aDelta
					  );
	return err;
	}
	
EXPORT_C TInt TPositionCalc::GetBearing(const TCoordinate& aStartCoor, const TCoordinate& aStopCoor, TReal32& aBearing)
/**
Calculate distance between two TCoordinates
@param aStartCoor, the start TCoordinate
@param aStopCoor, the stop TCoordinate
@param aDistance, the bearing between
@return The calculation error
 */
	{
	TReal32 tmp = 0.0;
	TInt err  = TPositionCalcAlg::CalcBearing(aStartCoor.Latitude(),
					  aStartCoor.Longitude(),
					  aStartCoor.Altitude(),
					  aStartCoor.Datum(),
					  tmp,
					  tmp,
					  aStopCoor.Latitude(),
					  aStopCoor.Longitude(),
					  aStopCoor.Altitude(),
					  aStopCoor.Datum(),	
					  tmp,
					  tmp,
					  aBearing,
					  tmp
					  );
	return err;
	}
	
EXPORT_C TInt TPositionCalc::GetBearing(const TLocality& aStartLoc, const TLocality& aStopLoc, TReal32& aBearing, TReal32& aDelta)
/**
Calculate distance between two TLocality
@param aStartLoc, the start TLocality
@param aStopLoc, the stop TLocality
@param aDistance, the distance between
@param aDelta, the bearing calculation error variance
@return The calculation error
 */
 	{
	TInt err  = TPositionCalcAlg::CalcBearing(aStartLoc.Latitude(),
					  aStartLoc.Longitude(),
					  aStartLoc.Altitude(),
					  aStartLoc.Datum(),
					  aStartLoc.HorizontalAccuracy(),
					  aStartLoc.VerticalAccuracy(),
					  aStopLoc.Latitude(),
					  aStopLoc.Longitude(),
					  aStopLoc.Altitude(),
					  aStopLoc.Datum(),	
					  aStopLoc.HorizontalAccuracy(),
					  aStopLoc.VerticalAccuracy(),
					  aBearing,
					  aDelta
					  );
	return err;
	}
	
EXPORT_C TInt TPositionCalc::GetSpeed(const TPosition& aStartPos, const TPosition& aStopPos, TReal32& aSpeed)
/**
Calculate distance between two TPositions
@param aStartPos, the start TPosition
@param aStopPos, the stop TPosition
@param aDistance, the average travelling speed
@return The calculation error
 */
	{
	TReal32 tmp = 0.0;
	TInt err  = TPositionCalcAlg::CalcSpeed(aStartPos.Latitude(),
					  aStartPos.Longitude(),
					  aStartPos.Altitude(),
					  aStartPos.Datum(),
					  tmp,
					  tmp,
					  aStartPos.Time(),
					  aStopPos.Latitude(),
					  aStopPos.Longitude(),
					  aStopPos.Altitude(),
					  aStopPos.Datum(),	
					  tmp,
					  tmp,
					  aStopPos.Time(),
					  aSpeed,
					  tmp
					  );
	return err;
	}
	
EXPORT_C TInt TPositionCalc::GetSpeed(const TPosition& aStartPos, const TPosition& aStopPos, TReal32& aSpeed, TReal32& aDelta)
/**
Calculate distance between two TPositions
@param aStartPos, the start TPosition
@param aStopPos, the stop TPosition
@param aDistance, the average travelling speed
@param aDelta, the calculation error variance
@return The calculation error
 */
 	{
	TInt err  = TPositionCalcAlg::CalcSpeed(aStartPos.Latitude(),
					  aStartPos.Longitude(),
					  aStartPos.Altitude(),
					  aStartPos.Datum(),
					  aStartPos.HorizontalAccuracy(),
					  aStartPos.VerticalAccuracy(),
					  aStartPos.Time(),
					  aStopPos.Latitude(),
					  aStopPos.Longitude(),
					  aStopPos.Altitude(),
					  aStopPos.Datum(),	
					  aStopPos.HorizontalAccuracy(),
					  aStopPos.VerticalAccuracy(),
					  aStopPos.Time(),
					  aSpeed,
					  aDelta
					  );
	return err;
	}

EXPORT_C TInt TPositionCalc::Translate(const TCoordinate& aStartCoor, const TReal32& aDistance, const TReal32& aBearing, TCoordinate& aStopCoor)
/**
Translate the stop coordinate by start point and given distance and bearing between two TCoordinates
@param aStartCoor, the start TCoordinate
@param aDistance, the average travelling speed
@param aBearing, the bearing between two coordinates
@param aStopCoor, return the stop TCoordinate
@return The calculation error
 */
	{
	TPositionDatumId datumId;
	TReal64 lat;
	TReal64 lon;
	TReal32 alt;
	TInt err  = TPositionCalcAlg::Translate(aStartCoor.Latitude(),
					  aStartCoor.Longitude(),
					  aStartCoor.Altitude(),
					  aStartCoor.Datum(),
					  aDistance,
					  aBearing,
					  lat,
					  lon,
					  alt,
					  (TUid&)datumId
					  );
					  
	aStopCoor.SetDatum(datumId);
	aStopCoor.SetCoordinate(lat, lon, alt);
	return err;
	}
