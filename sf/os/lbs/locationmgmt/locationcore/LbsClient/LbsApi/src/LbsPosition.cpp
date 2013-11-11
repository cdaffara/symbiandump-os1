// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <lbsposition.h>
#include "LbsInternal.h"
#include <lbserrors.h>


//-----------------------------------------------------------------------------
// TCoordinate
//-----------------------------------------------------------------------------
EXPORT_C TCoordinate::TCoordinate()
/**
Default constructor for TCoordinate. Sets the member data to
default values (NaN).

*/
 : iDatum(KPositionDatumWgs84)
	{
	TRealX nan;
	nan.SetNaN();

	iLatitude = nan;
	iLongitude = nan;
	iAltitude = nan;

	NormalizeCoordinate();
	}

EXPORT_C TCoordinate::TCoordinate(const TReal64& aLatitude,
                                  const TReal64& aLongitude)
/**
Constructor for TCoordinate. Allows the client to set just latitude and
longitude, and sets the altitude to its default value (NaN).

@param aLatitude specifies an initial value for latitude.
@param aLongitude specifies an initial value for longitude.
 */
 : iLatitude(aLatitude)
 , iLongitude(aLongitude)
 , iDatum(KPositionDatumWgs84)
	{
	TRealX nan;
	nan.SetNaN();
	iAltitude = nan;

	NormalizeCoordinate();
	}

EXPORT_C TCoordinate::TCoordinate(const TReal64& aLatitude,
                                  const TReal64& aLongitude,
                                  TReal32 aAltitude)
/**
Constructor for TCoordinate. Allows the client to set the latitude,
longitude and altitude.

@param aLatitude specifies an initial value for latitude.
@param aLongitude specifies an initial value for longitude.
@param aAltitude specifies an initial value for altitude.
 */
 : iLatitude(aLatitude)
 , iLongitude(aLongitude)
 , iAltitude(aAltitude)
 , iDatum(KPositionDatumWgs84)
	{
	NormalizeCoordinate();
	}

EXPORT_C void TCoordinate::SetCoordinate(const TReal64& aLatitude,
                                         const TReal64& aLongitude)
/**
This method assigns a new latitude and longitude to the object. The
altitude is set to its default value.

@param aLatitude specifies a new latitude. The latitude is represented in degrees.
@param aLongitude specifies a new longitude. The longitude is represented in degrees.
 */
	{
	iLatitude = aLatitude;
	iLongitude = aLongitude;

	TRealX nan;
	nan.SetNaN();
	iAltitude = nan;

	NormalizeCoordinate();
	}

EXPORT_C void TCoordinate::SetCoordinate(const TReal64& aLatitude,
                                         const TReal64& aLongitude,
                                         TReal32 aAltitude)
/**
 * This method assigns a new latitude, longitude and altitude to the object.
 *
@param aLatitude specifies a new latitude. The latitude is represented in degrees.
@param aLongitude specifies a new longitude. The longitude is represented in degrees.
@param aAltitude specifies a new altitude. The altitude is represented in meters.
 */
	{
	iLatitude = aLatitude;
	iLongitude = aLongitude;
	iAltitude = aAltitude;

	NormalizeCoordinate();
	}

void TCoordinate::NormalizeCoordinate()
/**
 This method ensures the coordinates are within coordinate boundaries. It
 will ensure latitude is between -90 and 90 degrees, and longitude is
 between -180 (not inclusive) and 180 degrees.

 If latitude wraps around to the other side of the globe, longitude is
 shifted by 180 degrees.

@internalComponent
 */
	{
	// 1. latitude
	if (!Math::IsNaN(iLatitude) && (iLatitude < -90 || iLatitude > 90))
		{
		// save sign, because negative and positive latitude
		// values are processed same way
		TBool isNegLat = (iLatitude < 0);

		// truncate by 360, work with positive values
		TReal absLat = isNegLat ? -iLatitude : iLatitude;
		Math::Mod(iLatitude, absLat, 360); // 0 <= iLatitude < 360

		// correct values, if latitude wraps 90 deg boundary
		if (iLatitude >= 270)              // 4th quarter
			{
			iLatitude = iLatitude - 360;   // reverse
			}
		else if (iLatitude > 90)           // 2nd or 3rd quarter
			{
			iLatitude = 180 - iLatitude;   // round about 90
			if (!Math::IsNaN(iLongitude))
				iLongitude = iLongitude + 180; // another hemisphere
			}

		// recall latutide sign
		if (isNegLat)
			iLatitude = - iLatitude;
		}

	// 2. longitude
	if (!Math::IsNaN(iLongitude) && (iLongitude < -180 || iLongitude >= 180))
		{
		// -360 < iLongitude < 360
		Math::Mod(iLongitude, iLongitude, 360);

		// let's put iLongitude to -180 .. 180 range
		if (iLongitude > 180)
			{
			iLongitude = iLongitude - 360;
			}
		else if (iLongitude < - 180)    // for negative longitudes
			{
			iLongitude = iLongitude + 360;
			}

		// 180.0 is not valid so make it -180.0
		if (iLongitude == 180.0)
			{
			iLongitude = -180.0;
			}
		}

	__ASSERT_DEBUG(
		Math::IsNaN(iLongitude) || (-180.0 <= iLongitude && iLongitude < 180.0),
		User::Panic(KPositionInternalFault, EBadLongitude));
	__ASSERT_DEBUG(
		Math::IsNaN(iLatitude) || (-90.0 <= iLatitude && iLatitude <= 90.0),
		User::Panic(KPositionInternalFault, EBadLatitude));
	}

EXPORT_C void TCoordinate::SetDatum(TPositionDatumId aDatum)
/**
This method assigns a new datum to the object.

@param aDatum specifies a new datum.
 */
	{
	iDatum = aDatum;
	}

EXPORT_C TReal64 TCoordinate::Latitude() const
/**
Accessor for latitude. The latitude is represented in degrees.

@return the latitude component of this coordinate.
 */
	{
	return iLatitude;
	}

EXPORT_C TReal64 TCoordinate::Longitude() const
/**
Accessor for longitude. The longitude is represented in degrees.

@return the longitude component of this coordinate.
 */
	{
	return iLongitude;
	}

EXPORT_C TReal32 TCoordinate::Altitude() const
/**
Accessor for altitude. The altitude is represented in meters.

@return the altitude component of this coordinate, in metres.
 */
	{
	return iAltitude;
	}

EXPORT_C TPositionDatumId TCoordinate::Datum() const
/**
Accessor for the datum.

@return the datum of this coordinate.
 */
	{
	return iDatum;
	}

//-----------------------------------------------------------------------------
// TLocality
//-----------------------------------------------------------------------------
EXPORT_C TLocality::TLocality()
/**
Default constructor for TLocality. Sets the member data to
default values (NaN).

*/
	{
	TRealX nan;
	nan.SetNaN();

	iHorizontalAccuracy = nan;
	iVerticalAccuracy = nan;
	}

EXPORT_C TLocality::TLocality(const TCoordinate& aCoordinate,
                              TReal32 aHorizontalAccuracy)
/**
Constructor for TLocality. Allows the client to set a coordinate and a
'surface' accuracy.

@param aCoordinate specifies an initial value for the coordinate.
@param aHorizontalAccuracy specifies an initial value for the
horizontal accuracy.
@panic aHorizontalAccuracy is negative and not set to NaN.
 */
 : TCoordinate(aCoordinate)
 , iHorizontalAccuracy(aHorizontalAccuracy)
	{
	__ASSERT_ALWAYS(
		Math::IsNaN(aHorizontalAccuracy) || aHorizontalAccuracy >= TReal32(0),
		User::Panic(KPosClientFault, EPositionBadAccuracy));

	TRealX nan;
	nan.SetNaN();

	iVerticalAccuracy = nan;
	}

EXPORT_C TLocality::TLocality(const TCoordinate& aCoordinate,
                              TReal32 aHorizontalAccuracy,
                              TReal32 aVerticalAccuracy)
/**
Constructor for TLocality. Allows the client to set a coordinate and both a
horizontal and vertical accuracy.

@param aCoordinate specifies an initial value for the coordinate.
@param aHorizontalAccuracy specifies an initial value for the
horizontal (earth-surface) accuracy.
@param aVerticalAccuracy specifies an initial value for the
vertical (altitudinal) accuracy.
@panic aHorizontalAccuracy or aVerticalAccuracy are negative and not set to NaN.
 */
	:
	TCoordinate(aCoordinate),
	iHorizontalAccuracy(aHorizontalAccuracy),
	iVerticalAccuracy(aVerticalAccuracy)
	{
	__ASSERT_ALWAYS(
		   (Math::IsNaN(aHorizontalAccuracy) || aHorizontalAccuracy >= TReal32(0))
		&& (Math::IsNaN(aVerticalAccuracy) || aVerticalAccuracy >= TReal32(0)),
		User::Panic(KPosClientFault, EPositionBadAccuracy));
	}

EXPORT_C void TLocality::SetHorizontalAccuracy(TReal32 aHorizontalAccuracy)
/**
Sets the horizontal accuracy of this coordinate.

@param aHorizontalAccuracy is the new horizontal accuracy, in metres.
@panic aHorizontalAccuracy is negative and not set to NaN.
 */
	{
	__ASSERT_ALWAYS(
		Math::IsNaN(aHorizontalAccuracy) || aHorizontalAccuracy >= TReal32(0),
		User::Panic(KPosClientFault, EPositionBadAccuracy));
	iHorizontalAccuracy = aHorizontalAccuracy;
	}

EXPORT_C void TLocality::SetVerticalAccuracy(TReal32 aVerticalAccuracy)
/**
Sets the vertical accuracy of this coordinate.

@param aVerticalAccuracy is the new vertical accuracy, in metres.
@panic aVerticalAccuracy is negative and not set to NaN.
 */
	{
	__ASSERT_ALWAYS(
		Math::IsNaN(aVerticalAccuracy) || aVerticalAccuracy >= TReal32(0),
		User::Panic(KPosClientFault, EPositionBadAccuracy));
	iVerticalAccuracy = aVerticalAccuracy;
	}

EXPORT_C void TLocality::SetAccuracy(TReal32 aHorizontalAccuracy,
                                     TReal32 aVerticalAccuracy)
/**
Sets both the horizontal and vertical accuracy of this coordinate.

@param aHorizontalAccuracy is the new horizontal accuracy, in metres.
@param aVerticalAccuracy is the new vertical accuracy, in metres.
@panic aHorizontalAccuracy or aVerticalAccuracy are negative and not set to NaN.
 */
	{
	__ASSERT_ALWAYS(
		   (Math::IsNaN(aHorizontalAccuracy) || aHorizontalAccuracy >= TReal32(0))
		&& (Math::IsNaN(aVerticalAccuracy) || aVerticalAccuracy >= TReal32(0)),
		User::Panic(KPosClientFault, EPositionBadAccuracy));
	iHorizontalAccuracy = aHorizontalAccuracy;
	iVerticalAccuracy = aVerticalAccuracy;
	}

EXPORT_C TReal32 TLocality::HorizontalAccuracy() const
/**
Retrieves the horizontal accuracy of this coordinate.

@return the horizontal accuracy, in metres.
 */
	{
	return iHorizontalAccuracy;
	}

EXPORT_C TReal32 TLocality::VerticalAccuracy() const
/**
Retrieves the vertical accuracy of this coordinate.

@return the vertical accuracy, in metres.
 */
	{
	return iVerticalAccuracy;
	}

EXPORT_C TInt TLocality::Distance(const TCoordinate& aCoordinate,
                                  TReal32& aDistance) const
/**
This method calculates the distance between this coordinate and the
supplied coordinate.

@param aCoordinate is another point to use in the calculation.
@param aDistance upon successful completion, this is set to the distance
between this coordinate and aCoordinate, in metres.
@return a Symbian OS error code.
@return KErrArgument if any of iLatitude, iLongitude, aCoordinate.iLatitude
or aCoordinate.iLongitude are set to NaN.
 */
	{
	return TCoordinate::Distance(aCoordinate, aDistance);
	}

EXPORT_C TInt TLocality::BearingTo(const TCoordinate& aTargetCoordinate,
	                        TReal32& aBearing) const
/**
This method calculates the bearing from this coordinate to the
supplied coordinate.

@param aTargetCoordinate is the supplied target coordinate.
@param aBearing upon successful completion, this is set to the bearing
from this coordinate to aTargetCoordinate, in degrees counting clockwise  
relative to true north.
@return a Symbian OS error code.
@return KErrArgument if any of iLatitude, iLongitude,
aTargetCoordinate.iLatitude or aTargetCoordinate.iLongitude are set to NaN.
@return KErrPositionIncalculable if this coordinate is at a pole or if the 
two coordinates are the same or antipodal.
 */
	{
	return TCoordinate::BearingTo(aTargetCoordinate, aBearing);
	}

//-----------------------------------------------------------------------------
// TPosition
//-----------------------------------------------------------------------------
EXPORT_C TPosition::TPosition()
/**
Default constructor for TCoordinate. Sets the member data to
default values (NaN for coordinates, 0 for time).

*/
 : iTime(0)
	{
	}

EXPORT_C TPosition::TPosition(const TLocality& aLocality,
                              TTime aTime) 
/**
Constructor for TPosition. Allows the client to set a locality and an
associated time.

@param aLocality specifies an initial value for the locality of the position.
@param aTime specifies an initial value for the time of the position.
 */
 : TLocality(aLocality)
 , iTime(aTime)
	{
	}

EXPORT_C void TPosition::SetTime(TTime aTime)
/**
Sets the time of this position data.

@param aTime is the new timing information.
 */
	{
	iTime = aTime;
	}

EXPORT_C void TPosition::SetCurrentTime()
/**
Sets the time of this position data to the current universal time as indicated
by the systems clock.

*/
	{
	iTime.UniversalTime();
	}

EXPORT_C TTime TPosition::Time() const
/**
Retrieves the time of this position data.

@return the timing component of this position information.
 */
	{
	return iTime;
	}

//-----------------------------------------------------------------------------
// TCourse
//-----------------------------------------------------------------------------
EXPORT_C TCourse::TCourse()
/**
Default constructor for TCourse. Sets the member data to
default values (NaN).

*/
	{
	TRealX nan;
	nan.SetNaN();

	iSpeed = nan;
	iHeading = nan;
	iSpeedAccuracy = nan;
	iHeadingAccuracy = nan;
	iCourse = nan;
	iCourseAccuracy = nan;
	iVerticalSpeed = nan;
	iVerticalSpeedAccuracy = nan;
	}

EXPORT_C TReal32 TCourse::Speed() const
/**
Retrieves the speed of this course.

@return the speed, in metres per second.
 */
	{
	return iSpeed;
	}

EXPORT_C TReal32 TCourse::SpeedAccuracy() const
/**
Retrieves the accuracy of the speed component of this course.

@return the accuracy of the speed, in metres per second.
 */
	{
	return iSpeedAccuracy;
	}

EXPORT_C TReal32 TCourse::Heading() const
/**
Retrieves the true heading of this course.

@return the true heading, in degrees.
 */
	{
	return iHeading;
	}

EXPORT_C TReal32 TCourse::HeadingAccuracy() const
/**
Retrieves the accuracy of the heading component of this course.

@return the accuracy of the heading, in degrees.
 */
	{
	return iHeadingAccuracy;
	}

/**
Retrieves the true course over ground.
@return the true course over ground, in degrees.
 */
EXPORT_C TReal32 TCourse::Course() const
	{
	return iCourse;
	}

/**
Retrieves the accuracy of the course over ground.
@return the accuracy of the course over ground, in degrees.
 */
EXPORT_C TReal32 TCourse::CourseAccuracy() const
	{
	return iCourseAccuracy;
	}

EXPORT_C TReal32 TCourse::VerticalSpeed() const
/**
Retrieves the vertical speed of this course.

@return the vertical speed, in metres per second.
 */
    {
    return iVerticalSpeed;
    }

EXPORT_C TReal32 TCourse::VerticalSpeedAccuracy() const
/**
Retrieves the accuracy of the vertical speed component of this course.

@return the accuracy of the vertical speed, in metres per second.
 */
    {
    return iVerticalSpeedAccuracy;
    }

EXPORT_C void TCourse::SetSpeed(TReal32 aSpeed)
/**
Changes the speed of this course.

@param aSpeed is the new speed, in metres per second.
 */
	{
	iSpeed = aSpeed;
	}

EXPORT_C void TCourse::SetSpeedAccuracy(TReal32 aSpeedAccuracy)
/**
Changes the accuracy of the speed component of this course.

@param aSpeedAccuracy is the new accuracy of the speed, in metres per second.
@panic aSpeedAccuracy is negative.
 */
	{
	__ASSERT_ALWAYS(
		Math::IsNaN(aSpeedAccuracy) || aSpeedAccuracy >= TReal32(0),
		User::Panic(KPosClientFault, EPositionBadAccuracy));
	iSpeedAccuracy = aSpeedAccuracy;
	}

EXPORT_C void TCourse::SetHeading(TReal32 aHeading)
/**
Changes the true heading of this course.

@param aHeading is the new true heading, in degrees.
 */
	{
	iHeading = aHeading;
	}

EXPORT_C void TCourse::SetHeadingAccuracy(TReal32 aHeadingAccuracy)
/**
Changes the accuracy of the heading component of this course.

@param aHeadingAccuracy is the new accuracy of the heading, in degrees.
@panic aHeadingAccuracy is negative.
 */
	{
	__ASSERT_ALWAYS(
		Math::IsNaN(aHeadingAccuracy) || aHeadingAccuracy >= TReal32(0),
		User::Panic(KPosClientFault, EPositionBadAccuracy));
	iHeadingAccuracy = aHeadingAccuracy;
	}

/**
Changes the true course over ground.
@param aCourse is the new true course, in degrees.
 */
EXPORT_C void TCourse::SetCourse(TReal32 aCourse)
	{
	iCourse = aCourse;
	}

/**
Changes the accuracy of the course component of this course.
@param aCourseAccuracy is the new accuracy of the course, in degrees.
@panic "Lbs Client Fault" 13 if aCourseAccuracy is negative.
 */
EXPORT_C void TCourse::SetCourseAccuracy(TReal32 aCourseAccuracy)
	{
	__ASSERT_ALWAYS(
		Math::IsNaN(aCourseAccuracy) || aCourseAccuracy >= TReal32(0),
		User::Panic(KPosClientFault, EPositionBadAccuracy));
	iCourseAccuracy = aCourseAccuracy;
	}

EXPORT_C void TCourse::SetVerticalSpeed(TReal32 aVerticalSpeed)
/**
Changes the vertical speed of this course.

@param aVerticalSpeed is the new vertical speed, in metres per second.
 */
    {
     iVerticalSpeed = aVerticalSpeed;
    }

EXPORT_C void TCourse::SetVerticalSpeedAccuracy(TReal32 aVerticalSpeedAccuracy)
/**
Changes the accuracy of the speed component of this course.

@param aVerticalSpeedAccuracy is the new accuracy of the vertical speed, in metres per second.
@panic aVerticalSpeedAccuracy is negative.
 */
    {
    __ASSERT_ALWAYS(
        Math::IsNaN(aVerticalSpeedAccuracy) || aVerticalSpeedAccuracy >= TReal32(0),
        User::Panic(KPosClientFault, EPositionBadAccuracy));
    
    iVerticalSpeedAccuracy = aVerticalSpeedAccuracy;
    }
