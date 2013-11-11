// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <lbsposition.h>
#include <lbserrors.h>
#include <e32math.h>

const TReal64 KEarthRadius = 6371010; // metres
const TReal64 KMetresPerMicroSecondToMetresPerSecond = 1000000; // Conversion factor

// This macro is used as an equivalent to User::LeaveIfError but this macro
// instead returns if any error is found.
// Although macros should be avoided, this error check is so commonly used in
// this file that the macro is justified in this case.
#define RETURN_IF_ERROR(aExp) {TInt error = aExp; if (error) { return error;} }

//-----------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------

/**
This function ensures that a sine or cosine value is within its value 
boundaries. It will ensure that the value is between -1 and 1 (inclusive).
 
This is needed to compensate for rounding errors that might have occured 
when the value was calculated.
 
@param aSinOrCosValue
*/
void NormalizeSinOrCosValue(TReal64& aSinOrCosValue)
    {        
    if (aSinOrCosValue > 1.0)
        {
        aSinOrCosValue = 1.0;
        }
    else if (aSinOrCosValue < -1.0)
        {
        aSinOrCosValue = -1.0;
        }
    }

/**
 This method calculates the value of cosine alpha between two given 
 coordinates (a source coordinate and a target coordinate).
 
 @param aCosAlpha
 @param aSrc
 @param aTar
 */
TInt CosAlpha(TReal64& aCosAlpha,
              const TCoordinate& aSrc,
              const TCoordinate& aTar)
    {
    TReal64 sinSrcLat;
    RETURN_IF_ERROR(Math::Sin(sinSrcLat, aSrc.Latitude() * KDegToRad));

    TReal64 sinTarLat;
    RETURN_IF_ERROR(Math::Sin(sinTarLat, aTar.Latitude() * KDegToRad));

    TReal64 cosSrcLat;
    RETURN_IF_ERROR(Math::Cos(cosSrcLat, aSrc.Latitude() * KDegToRad));

    TReal64 cosTarLat;
    RETURN_IF_ERROR(Math::Cos(cosTarLat, aTar.Latitude() * KDegToRad));

    TReal64 cosLonDiff;
    RETURN_IF_ERROR(
        Math::Cos(cosLonDiff, 
                  (aTar.Longitude() - aSrc.Longitude()) * KDegToRad));

    aCosAlpha = sinSrcLat * sinTarLat + cosSrcLat * cosTarLat * cosLonDiff;
    NormalizeSinOrCosValue(aCosAlpha);

    return KErrNone;
    }

/**
This method calculates the distance between two given coordinates (a source
coordinate and a target coordinate).
The distance is given as a TReal64 which is needed when used in other 
calculations.
This method assumes that all arguments already have been error checked.
 
@param aSrc
@param aTar
@param aDistance
 */
TInt Distance64(const TCoordinate& aSrc,
                const TCoordinate& aTar,
                TReal64& aDistance)
    {
    // Calculate the value of cos alpha
    TReal64 cosAlpha;
    RETURN_IF_ERROR(CosAlpha(cosAlpha, aSrc, aTar));

    // Calculate the value of alpha
    TReal64 alphaInRad;
    RETURN_IF_ERROR(Math::ACos(alphaInRad, cosAlpha));

    // Calculate the distance
    aDistance = alphaInRad * KEarthRadius;

	return KErrNone;
    }

/**
This method calculates the distance between two given localities (a source
locality and a target locality). An estimate of the accuracy of the result
is also provided.
Both the distance and the distance delta are given as TReal64 values which  
is needed when used in other calculations.
This method assumes that all arguments already have been error checked.
 
@param aSrc
@param aTar
@param aDistance
@param aDelta
  */
TInt Distance64(const TLocality& aSrc,
                const TLocality& aTar,
                TReal64& aDistance,
                TReal64& aDelta)
	{
    // Calculate the distance between the localities
    RETURN_IF_ERROR(Distance64(aSrc, aTar, aDistance));

    // Calculate the distance delta
    aDelta = aSrc.HorizontalAccuracy() + aTar.HorizontalAccuracy();
                                    
    return KErrNone;
	}

//-----------------------------------------------------------------------------
// TCoordinate
//-----------------------------------------------------------------------------

/**
This method calculates the distance between this coordinate and the
supplied coordinate.

@param[in] aCoordinate is another point to use in the calculation.
@param[out] aDistance upon successful completion, this is set to the distance
between this coordinate and aCoordinate, in metres.
@return a Symbian OS error code.
@return KErrArgument if any of iLatitude, iLongitude, aCoordinate.iLatitude
or aCoordinate.iLongitude are set to NaN.
*/
EXPORT_C TInt TCoordinate::Distance(const TCoordinate& aCoordinate,
                                    TReal32& aDistance) const
	{
    if (Math::IsNaN(iLatitude) ||
        Math::IsNaN(iLongitude) ||
        Math::IsNaN(aCoordinate.iLatitude) ||
        Math::IsNaN(aCoordinate.iLongitude))
        {
        return KErrArgument;
        }
    if (iLatitude == aCoordinate.iLatitude &&
        iLongitude == aCoordinate.iLongitude)
        {
	    aDistance = TReal32(0);		
        return KErrNone;
        }

    TReal64 distance64;
    RETURN_IF_ERROR(Distance64(*this, aCoordinate, distance64));

    aDistance = TReal32(distance64);

	return KErrNone;
	}

/**
This method calculates the bearing from this coordinate to the
supplied coordinate.

@param[in] aTargetCoordinate is the supplied target coordinate.
@param[out] aBearing upon successful completion, this is set to the bearing
from this coordinate to aTargetCoordinate, in degrees counting clockwise  
relative to true north.
@return a Symbian OS error code.
@return KErrArgument if any of iLatitude, iLongitude,
aTargetCoordinate.iLatitude or aTargetCoordinate.iLongitude are set to NaN.
@return KErrPositionIncalculable if this coordinate is at a pole or if the 
two coordinates are the same or antipodal.
*/
EXPORT_C TInt TCoordinate::BearingTo(const TCoordinate& aTargetCoordinate,
                                     TReal32& aBearing) const
	{
    if (Math::IsNaN(iLatitude) ||
        Math::IsNaN(iLongitude) ||
        Math::IsNaN(aTargetCoordinate.iLatitude) ||
        Math::IsNaN(aTargetCoordinate.iLongitude))
        {
        return KErrArgument;
        }

    // Validate iLatitude in order to check that the source coordinate is not
    // at a pole.
    // It is necessary to also validate sin iLatitude in order to check that 
    // the source coordinate is not so close to a pole at it should be 
    // considered to be at the pole.
    TReal64 sinSrcLat;
    RETURN_IF_ERROR(Math::Sin(sinSrcLat, iLatitude * KDegToRad));

    if (iLatitude == 90 || iLatitude == -90 ||
        sinSrcLat == 1 || sinSrcLat == -1)
        {
        // At a pole
        return KErrPositionIncalculable;
        }

    // Calculate the value of cos alpha
    TReal64 cosAlpha;
    RETURN_IF_ERROR(CosAlpha(cosAlpha, *this, aTargetCoordinate));

    // Validate that the coordinates are neither the same nor antipodal.
    // It is necessary to also validate cos alpha in order to check that the 
    // coordinates are not so close to the same or antipodal that they should be
    // considered to be the same or antipodal.
    TCoordinate antipodalTarget(-aTargetCoordinate.iLatitude, 
                                aTargetCoordinate.iLongitude + 180);

    if ((iLatitude == aTargetCoordinate.iLatitude && 
         iLongitude == aTargetCoordinate.iLongitude) ||
        (iLatitude == antipodalTarget.iLatitude && 
         iLongitude == antipodalTarget.iLongitude) ||
        cosAlpha == 1 || 
        cosAlpha == -1)
        {
        return KErrPositionIncalculable;
        }

    // Calculate the value of alpha
    TReal64 alphaInRad;
    RETURN_IF_ERROR(Math::ACos(alphaInRad, cosAlpha));

    // Calculate the value of sin alpha
    TReal64 sinAlpha;
    RETURN_IF_ERROR(Math::Sin(sinAlpha, alphaInRad));

    // Calculate the value of cos beta
    TReal64 sinTarLat;
    RETURN_IF_ERROR(Math::Sin(sinTarLat, 
                    aTargetCoordinate.iLatitude * KDegToRad));

    TReal64 cosSrcLat;
    RETURN_IF_ERROR(Math::Cos(cosSrcLat, iLatitude * KDegToRad));

    TReal64 cosBeta = (sinTarLat - sinSrcLat * cosAlpha) / 
                      (cosSrcLat * sinAlpha);
    NormalizeSinOrCosValue(cosBeta);

    // Calculate the value of beta
    TReal64 betaInRad;
    RETURN_IF_ERROR(Math::ACos(betaInRad, cosBeta));

    TReal32 beta = TReal32(betaInRad * KRadToDeg);

    // Check if the bearing is on a northern or a southern course.
    if ((beta == 0) || (beta == 180))
        {
        aBearing = beta;
        }
    else
        {
        // Check if the bearing should be on an eastern or a western course.
        // Western course is used when the source position is east of target 
        // position, i.e. if (source longitude > target longitude) XOR 
        //                   (abs(source longitude - target longitude) > 180)
        TReal64 lonDiff = Abs(iLongitude - aTargetCoordinate.iLongitude);
        TBool lonDiffLargerThan180 = (lonDiff > 180);
        TBool srcLonLargerThanTarLon = 
                (iLongitude > aTargetCoordinate.iLongitude);

        TBool useWesternCourse = 
                (srcLonLargerThanTarLon && !lonDiffLargerThan180) || 
                (!srcLonLargerThanTarLon && lonDiffLargerThan180);

        if (useWesternCourse)
            {
            aBearing = 360 - beta;
            }
        else
            {
            aBearing = beta;
            }
        }

    return KErrNone;
	}

/**
This method moves this coordinate aDistance in the direction specified
by aBearing

@param aBearing is the direction to move this coordinate, in degrees 
counting clockwise relative to true north.
@param aDistance is the distance to move this coordinate, in metres.
@return a Symbian OS error code.
@return KErrArgument if any of iLongitude or iLatitude are set to NaN.
@return KErrPositionIncalculable if this coordinate is at a pole.
*/
EXPORT_C TInt TCoordinate::Move(TReal32 aBearing, TReal32 aDistance)
	{
    if (Math::IsNaN(iLatitude) ||
        Math::IsNaN(iLongitude))
        {
        return KErrArgument;
        }

    // Validate iLatitude in order to check that the source coordinate is not
    // at a pole.
    // It is necessary to also validate sin iLatitude in order to check that 
    // the source coordinate is not so close to a pole at it should be 
    // considered to be at the pole.
    TReal64 sinSrcLat;
    RETURN_IF_ERROR(Math::Sin(sinSrcLat, iLatitude * KDegToRad));

    if (iLatitude == 90 || iLatitude == -90 ||
        sinSrcLat == 1 || sinSrcLat == -1)
        {
        // At a pole
        return KErrPositionIncalculable;
        }

    // Calculate the value of alpha
    TReal64 alphaInRad = aDistance / KEarthRadius;
    
    // Calculate the value of sin target latitude
    TReal64 cosSrcLat;
    RETURN_IF_ERROR(Math::Cos(cosSrcLat, iLatitude * KDegToRad));

    TReal64 sinAlpha;
    RETURN_IF_ERROR(Math::Sin(sinAlpha, alphaInRad));

    TReal64 cosAlpha;
    RETURN_IF_ERROR(Math::Cos(cosAlpha, alphaInRad));

    TReal64 cosBeta;
    RETURN_IF_ERROR(Math::Cos(cosBeta, aBearing * KDegToRad));

    TReal64 sinTarLat = cosSrcLat * sinAlpha * cosBeta + sinSrcLat * cosAlpha;
    NormalizeSinOrCosValue(sinTarLat);

    // Calculate the value of target latitude
    TReal64 tarLatInRad;
    RETURN_IF_ERROR(Math::ASin(tarLatInRad, sinTarLat));

    TReal64 tarLat = tarLatInRad * KRadToDeg;

    // Calculate the delta longitude
    TReal64 sinBeta;
    RETURN_IF_ERROR(Math::Sin(sinBeta, aBearing * KDegToRad));

    TReal64 dLonInRad;
	TInt err = Math::ATan(dLonInRad, sinBeta * sinAlpha * cosSrcLat,
	    cosAlpha - sinSrcLat * sinTarLat);

    // A calculation error means that the target position is at a pole. In this
    // case, the target longitude is set to 0.
	TReal64 tarLon = 0;
    if (!(err || tarLat == 90 || tarLat == -90))
        {
        tarLon = iLongitude + dLonInRad * KRadToDeg;
        }
        
    // Set the new coordinates
    iLatitude = tarLat;
    iLongitude = tarLon;
    NormalizeCoordinate();

    return KErrNone;
	}

//-----------------------------------------------------------------------------
// TLocality
//-----------------------------------------------------------------------------

/**
This method calculates the distance between this locality and the
supplied locality. An estimate of the accuracy of the result is also
provided.

@param[in] aLocality is another point to use in the calculation.
@param[out] aDistance upon successful completion, this is set to the distance
between this locality and aLocality, in metres.
@param[out] aDelta upon successful completion, this is set to the estimated
accuracy of the distance calculation, in metres.
@return a Symbian OS error code.
@return KErrArgument if any of iLatitude, iLongitude, aLocality.iLatitude
or aLocality.iLongitude are set to NaN.
@return KErrArgument if any of iHorizontalAccuracy or
aLocality.iHorizontalAccuracy are set to NaN.
*/
EXPORT_C TInt TLocality::Distance(const TLocality& aLocality,
                                  TReal32& aDistance,
                                  TReal32& aDelta) const
	{
    if (Math::IsNaN(iLatitude) ||
        Math::IsNaN(iLongitude) ||
        Math::IsNaN(iHorizontalAccuracy) ||
        Math::IsNaN(aLocality.iLatitude) ||
        Math::IsNaN(aLocality.iLongitude) ||
        Math::IsNaN(aLocality.iHorizontalAccuracy))
        {
        return KErrArgument;
        }
	if (iLatitude == aLocality.iLatitude &&
        iLongitude == aLocality.iLongitude)
        {
	    aDistance = TReal32(0);	
		aDelta = TReal32(iHorizontalAccuracy + aLocality.iHorizontalAccuracy);
        return KErrNone;
        }


    TReal64 distance64;
    TReal64 delta64;
    RETURN_IF_ERROR(Distance64(*this, aLocality, distance64, delta64));

    aDistance = TReal32(distance64);
    aDelta = TReal32(delta64);
                                    
    return KErrNone;
	}

/**
This method calculates the bearing from this locality to the
supplied locality.

@param[in] aTargetLocality is the supplied target locality.
@param[out] aBearing upon successful completion, this is set to the bearing
from this locality to aTargetLocality, in degrees  counting clockwise
relative to true north.
@param[out] aDelta upon successful completion, this is set to an estimate of the
accuracy of the calculation, in degrees relative to aBearing.
@return a Symbian OS error code.
@return KErrArgument if any of iLatitude, iLongitude,
aTargetLocality.iLatitude or aTargetLocality.iLongitude are set to NaN.
@return KErrArgument if any of iHorizontalAccuracy
or aTargetLocality.iHorizontalAccuracy are set to NaN.
@return KErrPositionIncalculable if the error circle (horizontal accuracy)
of this locality includes a pole.
@return KErrPositionIncalculable if the two localities has overlapping 
error circles.
@return KErrPositionIncalculable if the error circle of this locality 
overlaps with the error circle of aTargetLocality when projected antipodal.
*/
EXPORT_C TInt TLocality::BearingTo(const TLocality& aTargetLocality,
                                   TReal32& aBearing,
                                   TReal32& aDelta) const
	{
    if (Math::IsNaN(iLatitude) ||
        Math::IsNaN(iLongitude) ||
        Math::IsNaN(iHorizontalAccuracy) ||
        Math::IsNaN(aTargetLocality.iLatitude) ||
        Math::IsNaN(aTargetLocality.iLongitude) ||
        Math::IsNaN(aTargetLocality.iHorizontalAccuracy))
        {
        return KErrArgument;
        }

    // Validate that this locality does not include the North Pole
    TCoordinate northPole(90, 0);
    TReal64 distance;
    RETURN_IF_ERROR(Distance64(*this, northPole, distance));

    if (distance <= iHorizontalAccuracy)
        {
        return KErrPositionIncalculable;
        }

    // Validate that this locality does not include the South Pole
    TCoordinate southPole(-90, 0);
    RETURN_IF_ERROR(Distance64(*this, southPole, distance));

    if (distance <= iHorizontalAccuracy)
        {
        return KErrPositionIncalculable;
        }

    // Validate that this locality does not overlap with the target locality 
    // when target is projected antipodal.
    TCoordinate antipodalTarget(-aTargetLocality.iLatitude, 
                                aTargetLocality.iLongitude + 180);
    RETURN_IF_ERROR(Distance64(*this, antipodalTarget, distance));
    TReal64 accuracySum = iHorizontalAccuracy + 
                          aTargetLocality.iHorizontalAccuracy;

    if (distance <= accuracySum)
        {
        return KErrPositionIncalculable;
        }

    // Validate that this locality does not overlap with the target locality.
    RETURN_IF_ERROR(Distance64(*this, aTargetLocality, distance));

    if (distance <= accuracySum)
        {
        return KErrPositionIncalculable;
        }

    // Calculate the bearing
    RETURN_IF_ERROR(BearingTo(aTargetLocality, aBearing));

    // Calculate the bearing delta
    TReal64 gammaInRad;
    TReal64 sinGamma = accuracySum / distance;
    RETURN_IF_ERROR(Math::ASin(gammaInRad, sinGamma));

    aDelta = TReal32(gammaInRad * KRadToDeg);

	return KErrNone;
	}


//-----------------------------------------------------------------------------
// TPosition
//-----------------------------------------------------------------------------

/**
This method calculates the horizontal speed between the current position
and the supplied instance aPosition. The speed is calculated based on the
coordinates and time associated with each instance.

@param[in] aPosition is another position to use in the calculation.
@param[out] aSpeed upon successful completion, this is set to the speed 
indicated by being at this position at its recorded time, and at aPosition
at its recorded time. Always positive, in metres per second.
@return a Symbian OS error code.
@return KErrArgument if any of iLatitude, iLongitude, aPosition.iLatitude
or aPosition.iLongitude are set to NaN.
@return KErrArgument if either of iTime or aPosition.iTime are set to zero.
@return KErrArgument if iTime and aPosition.iTime are the same.
*/
EXPORT_C TInt TPosition::Speed(const TPosition& aPosition,
                               TReal32& aSpeed) const
	{
    if (Math::IsNaN(iLatitude) ||
        Math::IsNaN(iLongitude) ||
        Math::IsNaN(aPosition.iLatitude) ||
        Math::IsNaN(aPosition.iLongitude) ||
        iTime == TTime(0) ||
        aPosition.iTime == TTime(0) ||
        iTime == aPosition.iTime)
        {
        return KErrArgument;
        }

    // Calculate the distance between the positions
    TReal64 distance;
    RETURN_IF_ERROR(Distance64(*this, aPosition, distance));

    // Calculate the speed between the positions
    // It is important to calculate distance/time before converting to 
    // metres per second so no division by zero occurs.
    TInt64 timeDiff = Abs(iTime.MicroSecondsFrom(aPosition.iTime).Int64());
    aSpeed = TReal32((distance / timeDiff) * 
                     KMetresPerMicroSecondToMetresPerSecond); 

	return KErrNone;
	}

/**
This method calculates the horizontal speed between the current position
and the supplied instance aPosition. The speed is calculated based on the
coordinates and time associated with each instance. An estimate of the
accuracy of the result is also provided.

@param[in] aPosition is another position to use in the calculation.
@param[out] aSpeed upon successful completion, this is set to the speed 
indicated by being at this position at its recorded time, and at aPosition
at its recorded time. Always positive, in metres per second.
@param[out] aDelta upon successful completion, this is set to an estimate of the
accuracy of the calculation, in metres per second.
@return a Symbian OS error code.
@return KErrArgument if any of iLatitude, iLongitude, aPosition.iLatitude
or aPosition.iLongitude are set to NaN.
@return KErrArgument if any of iHorizontalAccuracy
or aPosition.iHorizontalAccuracy are set to NaN.
@return KErrArgument if either of iTime or aPosition.iTime are set to zero.
@return KErrArgument if iTime and aPosition.iTime are the same.
*/
EXPORT_C TInt TPosition::Speed(const TPosition& aPosition,
                               TReal32& aSpeed,
                               TReal32& aDelta) const
	{
    if (Math::IsNaN(iLatitude) ||
        Math::IsNaN(iLongitude) ||
        Math::IsNaN(iHorizontalAccuracy) ||
        Math::IsNaN(aPosition.iLatitude) ||
        Math::IsNaN(aPosition.iLongitude) ||
        Math::IsNaN(aPosition.iHorizontalAccuracy) ||
        iTime == TTime(0) ||
        aPosition.iTime == TTime(0) ||
        iTime == aPosition.iTime)
        {
        return KErrArgument;
        }

    // Calculate the distance between the positions
    TReal64 distance;
    TReal64 delta;
    RETURN_IF_ERROR(Distance64(*this, aPosition, distance, delta));

    // Calculate the speed and the speed delta between the positions
    // It is important to calculate distance/time before converting to 
    // metres per second so no division by zero occurs.
    TInt64 timeDiff = Abs(iTime.MicroSecondsFrom(aPosition.iTime).Int64());

    aSpeed = TReal32((distance / timeDiff) * 
                     KMetresPerMicroSecondToMetresPerSecond); 
    aDelta = TReal32((delta / timeDiff) * 
                     KMetresPerMicroSecondToMetresPerSecond); 

	return KErrNone;
	}
