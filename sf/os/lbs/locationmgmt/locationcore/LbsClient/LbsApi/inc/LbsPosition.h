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

#ifndef __LBSPOSITION_H__
#define __LBSPOSITION_H__

#include <e32std.h>

/**
@publishedAll
@released
Datum IDs
 */
typedef TUid TPositionDatumId;


/**
@publishedAll
@released
Constant for the co-ordinate system for GPS system
 */
const TPositionDatumId KPositionDatumWgs84 = {0x101FAA29};

class TCoordinate
/**
TCoordinate is used to hold the basic coordinates of a location (latitude,
longitude and altitude).

@publishedAll
@released
 */
	{
public:
	IMPORT_C TCoordinate();
	IMPORT_C TCoordinate(const TReal64& aLatitude,
	                     const TReal64& aLongitude);
	IMPORT_C TCoordinate(const TReal64& aLatitude,
	                     const TReal64& aLongitude,
	                     TReal32 aAltitude);

	IMPORT_C void SetCoordinate(const TReal64& aLatitude,
	                            const TReal64& aLongitude);
	IMPORT_C void SetCoordinate(const TReal64& aLatitude,
	                            const TReal64& aLongitude,
	                            TReal32 aAltitude);

	IMPORT_C void SetDatum(TPositionDatumId aDatum);

	IMPORT_C TReal64 Latitude() const;
	IMPORT_C TReal64 Longitude() const;
	IMPORT_C TReal32 Altitude() const;
	IMPORT_C TPositionDatumId Datum() const;

	IMPORT_C TInt Distance(const TCoordinate& aCoordinate,
	                       TReal32& aDistance) const;
	IMPORT_C TInt BearingTo(const TCoordinate& aTargetCoordinate,
	                        TReal32& aBearing) const;

	IMPORT_C TInt Move(TReal32 aBearing, TReal32 aDistance);

private:
	void NormalizeCoordinate();

protected:
	/** Latitude, defaults to WGS-84 format. Represented in degree. */
	TReal64 iLatitude;
	/** Longitude, defaults to WGS-84 format. Represented in degree. */
	TReal64 iLongitude;
	/** Altitude, defaults to WGS-84 format. Represented in meters. */
	TReal32 iAltitude;
	/** The ID of the datum the coordinate is in, defaults to WGS-84 format. */
	TPositionDatumId iDatum;
	/** Unused variable for future expansion. */
	TUint8 iReserved[4];
	};


class TLocality : public TCoordinate
/**
Adds an error estimate for the horizontal and vertical accuracy of the point
to TCoordinate. Accuracy information is held in a TReal32 and is measure in
metres. The class also provides its own methods for determining the distance
and bearing to a target point. These methods also provide an error estimate.

@publishedAll
@released
 */
	{
public:
	IMPORT_C TLocality();
	IMPORT_C TLocality(const TCoordinate& aCoordinate,
	                   TReal32 aHorizontalAccuracy);
	IMPORT_C TLocality(const TCoordinate& aCoordinate,
	                   TReal32 aHorizontalAccuracy,
	                   TReal32 aVerticalAccuracy);

	IMPORT_C void SetHorizontalAccuracy(TReal32 aHorizontalAccuracy);
	IMPORT_C void SetVerticalAccuracy(TReal32 aVerticalAccuracy);
	IMPORT_C void SetAccuracy(TReal32 aHorizontalAccuracy,
	                          TReal32 aVerticalAccuracy);

	IMPORT_C TReal32 HorizontalAccuracy() const;
	IMPORT_C TReal32 VerticalAccuracy() const;

	IMPORT_C TInt Distance(const TCoordinate& aCoordinate,
	                       TReal32& aDistance) const;
	IMPORT_C TInt Distance(const TLocality& aLocality,
	                       TReal32& aDistance,
	                       TReal32& aDelta) const;

	IMPORT_C TInt BearingTo(const TCoordinate& aTargetCoordinate,
	                        TReal32& aBearing) const;

	IMPORT_C TInt BearingTo(const TLocality& aTargetLocality,
	                        TReal32& aBearing,
	                        TReal32& aDelta) const;

protected:
	/** Horizontal (earths-surface) accuracy, in metres. */
	TReal32 iHorizontalAccuracy;
	/** Altitudinal accuracy, in metres. */
	TReal32 iVerticalAccuracy;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[16];
	};


class TPosition : public TLocality
/**
This class is the standard data structure for retrieving location
information. It adds a time dimension to the inherited TLocality information.
This enables the speed to be calculated from two TPosition instances.
 
The time reflects the system time (that is, the mobile terminal) of when the
location fix was obtained. It does not indicate the time as obtained from the
position technology (for example network or satellite time).

The time is contained in a TTime data structure that provides microsecond
resolution. However, it should be noted that system clocks only provide a
resolution of milliseconds or indeed hundredths of a second.

@publishedAll
@released
 */
	{
public:
	IMPORT_C TPosition();
	IMPORT_C TPosition(const TLocality& aLocality,
	                   TTime aTime);

	IMPORT_C void SetTime(TTime aTime);
	IMPORT_C void SetCurrentTime();

	IMPORT_C TTime Time() const;

	IMPORT_C TInt Speed(const TPosition& aPosition,
	                    TReal32& aSpeed) const;
	IMPORT_C TInt Speed(const TPosition& aPosition,
	                    TReal32& aSpeed,
	                    TReal32& aDelta) const;

protected:
	/** This is the system time when the position related member data was
	obtained. */
	TTime iTime;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[16];
	};


class TCourse
/**
This is used to hold information about the current speed and direction
of the device. It is generally used in conjunction with TPositionCourseInfo
when a positioning technology is able to supply these details as part of
its positioning information.

@publishedAll
@released
 */
	{
public:
	IMPORT_C TCourse();

	IMPORT_C TReal32 Speed() const;
	IMPORT_C TReal32 VerticalSpeed() const;
	IMPORT_C TReal32 Heading() const;
    IMPORT_C TReal32 Course() const;
    
    IMPORT_C TReal32 SpeedAccuracy() const;
    IMPORT_C TReal32 VerticalSpeedAccuracy() const;
    IMPORT_C TReal32 HeadingAccuracy() const;
    IMPORT_C TReal32 CourseAccuracy() const;
    
	
    IMPORT_C void SetSpeed(TReal32 aSpeed);
    IMPORT_C void SetVerticalSpeed(TReal32 aVerticalSpeed);
    IMPORT_C void SetHeading(TReal32 aHeading);
	IMPORT_C void SetSpeedAccuracy(TReal32 aSpeedAccuracy);
	IMPORT_C void SetVerticalSpeedAccuracy(TReal32 aVerticalSpeedAccuracy);
	IMPORT_C void SetHeadingAccuracy(TReal32 aHeadingAccuracy);
	IMPORT_C void SetCourse(TReal32 aCourse);
	IMPORT_C void SetCourseAccuracy(TReal32 aCourseAccuracy);
	
	


protected:
	/** Speed, in metres per second. */
	TReal32 iSpeed;
	/** True Heading, in degrees. */
	TReal32 iHeading;
	/** Speed accuracy, in metres per second. */
	TReal32 iSpeedAccuracy;
	/** Heading accuracy, in degrees. */
	TReal32 iHeadingAccuracy;
	/** True Course, in degrees. */
	TReal32 iCourse;
	/** Course accuracy, in degrees. */
	TReal32 iCourseAccuracy;
	/** Vertical Speed, in metres per second. */
	TReal32 iVerticalSpeed;
	/** Vertical Speed accuracy, in metres per second. */
	TReal32 iVerticalSpeedAccuracy;
	
private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[8];
	};

#endif //__LBSPOSITION_H__
