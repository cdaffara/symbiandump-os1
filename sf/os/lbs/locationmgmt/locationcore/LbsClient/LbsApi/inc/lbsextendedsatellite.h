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

#ifndef __LBSEXTENDEDSATELLITE_H__
#define __LBSEXTENDEDSATELLITE_H__

#include <lbssatellite.h>
#include <lbs/lbsgpstimingmeasurement.h>


/**
This class is used to store GNSS Pseudorange Error Statistics(GST)data. 
@publishedPartner
@prototype
 */

class TDetailedErrorReport
    {
public:
    IMPORT_C TDetailedErrorReport();
    // Error Accessors
    IMPORT_C TReal64 StanDeviOfLongitudeError() const;
    IMPORT_C TReal64 StanDeviOfLatiitudeError() const;
    IMPORT_C TReal32 StanDeviOfAltitudeError() const;
    IMPORT_C TReal32 StanDeviOfSemiMajorAxisError() const;
    IMPORT_C TReal32 StanDeviOfSemiMinorAxisError() const;
    IMPORT_C TReal32 OrientationOfSemiMajorAxisError() const;
    IMPORT_C TReal32 RmsValOfStanDeviOfRange() const;
    
    // Error Setters
    IMPORT_C void SetStanDeviOfLongitudeError(TReal64 aStanDeviOfLongitudeError);
    IMPORT_C void SetStanDeviOfLatiitudeError(TReal64 aStanDeviOfLatiitudeError);
    IMPORT_C void SetStanDeviOfAltitudeError(TReal32 aStanDeviOfAltitudeError);
    IMPORT_C void SetStanDeviOfSemiMajorAxisError(TReal32 aStanDeviOfSemiMajorAxisError);              
    IMPORT_C void SetStanDeviOfSemiMinorAxisError(TReal32 aStanDeviOfSemiMinorAxisError);              
    IMPORT_C void SetOrientationOfSemiMajorAxisError(TReal32 aOrientationOfSemiMajorAxisError);      
    IMPORT_C void SetRmsValOfStanDeviOfRange(TReal32 aRmsValOfStanDeviOfRange);
      
protected:
    /** Standard deviation of longitude error in meters */
    TReal64 iStanDeviOfLongitudeError;
    /** Standard deviation of latitude error in meters */
    TReal64 iStanDeviOfLatiitudeError;
    /** Standard deviation of altitude error in meters */
    TReal32 iStanDeviOfAltitudeError;
    /** Standard deviation of semi-major axis of error ellipse in meters */
    TReal32 iStanDeviOfSemiMajorAxisError;
    /** Standard deviation of semi-minor axis of error ellipse in meters */
    TReal32 iStanDeviOfSemiMinorAxisError;                    
    /** Orientation of semi-major axis of error ellipse in degrees from true north */
    TReal32 iOrientationOfSemiMajorAxisError;
    /** RMS value of the standard deviation of the range inputs to the navigation process 
        Range inputs include pseudoranges & DGNSS corrections.
    */
    TReal32 iRmsValOfStanDeviOfRange;
private:
    /** Unused variable for future expansion. */
    TUint8 iReserved[16];
    };

/**
This class is used to store information about timing assistance measurement data 
and data required to generate NMEA strings. 
@publishedPartner
@prototype
 */
class TPositionExtendedSatelliteInfo : public TPositionSatelliteInfo
	{
public:
	IMPORT_C TPositionExtendedSatelliteInfo();
	
	IMPORT_C TInt GetGpsTimingData(TGpsTimingMeasurementData& aGpsTimingData) const;
	IMPORT_C void SetGpsTimingData(const TGpsTimingMeasurementData& aGpsTimingData);
     	
    IMPORT_C void GetDetailedErrorReport(TDetailedErrorReport& aDetailedErrorReport) const;
	IMPORT_C void SetDetailedErrorReport(const TDetailedErrorReport& aDetailedErrorReport);
	
	IMPORT_C TReal32 GeoidalSeparation() const;
	IMPORT_C void SetGeoidalSeparation(TReal32 aGeoidalSeparation);
	IMPORT_C TReal32 MagneticVariation() const;
	IMPORT_C void SetMagneticVariation(TReal32 aMagneticVariation);
	IMPORT_C TReal32 CourseOverGroundMagnetic() const;
	IMPORT_C void SetCourseOverGroundMagnetic(TReal32 aCourseOverGroundMagnetic);	
	
protected:
	
    /** GPS/Network Timing Measurements */
    TGpsTimingMeasurementData iGpsTimingMeasurementData;
    /** GNSS Pseudorange Error Statistics */
    TDetailedErrorReport iDetailedErrorReport;
    /** Geoidal separation, meters */
    TReal32 iGeoidalSeparation;
    /** Magnetic variation, degrees*/
    TReal32 iMagneticVariation;
    /** Course over ground, degrees  */
    TReal32 iCourseOverGroundMagnetic;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[24];
	};

#endif //__LBSEXTENDEDSATELLITE_H__
