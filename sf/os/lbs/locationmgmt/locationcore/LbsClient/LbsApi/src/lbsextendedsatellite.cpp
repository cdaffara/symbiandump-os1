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
#include <lbs/lbsextendedsatellite.h>


//-----------------------------------------------------------------------------
// TDetailedErrorReport
//-----------------------------------------------------------------------------
/**
Default constructor for a TDetailedErrorReport.
*/
EXPORT_C TDetailedErrorReport::TDetailedErrorReport()
    {
	TRealX nan;
	nan.SetNaN();
	
    iStanDeviOfLongitudeError = nan;
    iStanDeviOfLatiitudeError = nan;
    iStanDeviOfAltitudeError = nan;
    iStanDeviOfSemiMajorAxisError = nan;
    iStanDeviOfSemiMinorAxisError = nan;                    
    iOrientationOfSemiMajorAxisError = nan;
    iRmsValOfStanDeviOfRange = nan;
	
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}
/**
The Standard deviation of Longitude error
@return The Standard deviation of Longitude error
 */
EXPORT_C TReal64 TDetailedErrorReport::StanDeviOfLongitudeError() const
	{
	return iStanDeviOfLongitudeError;
	}
/**
The Standard deviation of Longitude error to be set
@param aStanDeviOfLongitudeError 
 */
EXPORT_C void TDetailedErrorReport::SetStanDeviOfLongitudeError(TReal64 aStanDeviOfLongitudeError)
	{
	iStanDeviOfLongitudeError = aStanDeviOfLongitudeError;
	}
/**
The Standard deviation of latitude error
@return The Standard deviation of latitude error
 */
EXPORT_C TReal64 TDetailedErrorReport::StanDeviOfLatiitudeError() const
    {
    return iStanDeviOfLatiitudeError;
    }
/**
The Standard deviation of latitude error to be set
@param aStanDeviOfLatiitudeError 
 */
EXPORT_C void TDetailedErrorReport::SetStanDeviOfLatiitudeError(TReal64 aStanDeviOfLatiitudeError)
    {
    iStanDeviOfLatiitudeError = aStanDeviOfLatiitudeError;
    }
/**
The Standard deviation of altitude error
@return The StanDeviOfAltitudeError
 */
EXPORT_C TReal32 TDetailedErrorReport::StanDeviOfAltitudeError() const
    {
    return iStanDeviOfAltitudeError;
    }
/**
The Standard deviation of altitude error to be set
@param aStanDeviOfAltitudeError 
 */
EXPORT_C void TDetailedErrorReport::SetStanDeviOfAltitudeError(TReal32 aStanDeviOfAltitudeError)
    {
    iStanDeviOfAltitudeError = aStanDeviOfAltitudeError;
    }
/**
The Standard deviation of semi-major axis of error ellipse
@return The StanDeviOfSemiMajorAxisError
 */
EXPORT_C TReal32 TDetailedErrorReport::StanDeviOfSemiMajorAxisError() const
    {
    return iStanDeviOfSemiMajorAxisError;
    }
/**
The Standard deviation of semi-major axis of error ellipse to be set
@param aStanDeviOfSemiMajorAxisError 
 */
EXPORT_C void TDetailedErrorReport::SetStanDeviOfSemiMajorAxisError(TReal32 aStanDeviOfSemiMajorAxisError)
    {
    iStanDeviOfSemiMajorAxisError = aStanDeviOfSemiMajorAxisError;
    }
/**
The Standard deviation of semi-minor axis of error ellipse
@return The StanDeviOfSemiMinorAxisError
 */
EXPORT_C TReal32 TDetailedErrorReport::StanDeviOfSemiMinorAxisError() const
    {
    return iStanDeviOfSemiMinorAxisError;
    }
/**
The Standard deviation of semi-minor axis of error ellipse to be set
@param aStanDeviOfSemiMinorAxisError 
 */
EXPORT_C void TDetailedErrorReport::SetStanDeviOfSemiMinorAxisError(TReal32 aStanDeviOfSemiMinorAxisError)
    {
    iStanDeviOfSemiMinorAxisError = aStanDeviOfSemiMinorAxisError;
    }
/**
The Orientation of semi-major axis of error ellipse
@return The OrientationOfSemiMajorAxisError
 */
EXPORT_C TReal32 TDetailedErrorReport::OrientationOfSemiMajorAxisError() const
    {
    return iOrientationOfSemiMajorAxisError;
    }
/**
The Orientation of semi-major axis of error ellipse to be set
@param aOrientationOfSemiMajorAxisError 
 */
EXPORT_C void TDetailedErrorReport::SetOrientationOfSemiMajorAxisError(TReal32 aOrientationOfSemiMajorAxisError)
    {
    iOrientationOfSemiMajorAxisError = aOrientationOfSemiMajorAxisError;
    }
/**
The RMS value of the standard deviation of the range inputs to the navigation process
@return The RmsValOfStanDeviOfRange
 */
EXPORT_C TReal32 TDetailedErrorReport::RmsValOfStanDeviOfRange() const
    {
    return iRmsValOfStanDeviOfRange;
    }
/**
The RMS value of the standard deviation of the range inputs to the navigation process to be set
@param aRmsValOfStanDeviOfRange 
 */
EXPORT_C void TDetailedErrorReport::SetRmsValOfStanDeviOfRange(TReal32 aRmsValOfStanDeviOfRange)
    {
    iRmsValOfStanDeviOfRange = aRmsValOfStanDeviOfRange;
    }


//-----------------------------------------------------------------------------
// TPositionExtendedSatelliteInfo
//-----------------------------------------------------------------------------
/**
TPositionExtendedSatelliteInfo constructor.
*/
EXPORT_C TPositionExtendedSatelliteInfo::TPositionExtendedSatelliteInfo()
	{
	TRealX nan;
	nan.SetNaN();
    
	iGeoidalSeparation = nan;
    iMagneticVariation = nan;
    iCourseOverGroundMagnetic = nan;
	
    iPosClassType |= EPositionExtendedSatelliteInfoClass;
    iPosClassSize = sizeof(TPositionExtendedSatelliteInfo);
    
    Mem::FillZ(&iReserved, sizeof(iReserved));
	}
/**
Populates aGpsTimingData with the GPS Timing Measurement Data
@return KErrNotFound if the data has not been set, KErrNone otherwise
 */
EXPORT_C TInt TPositionExtendedSatelliteInfo::GetGpsTimingData(TGpsTimingMeasurementData& aGpsTimingData) const
    {
    if (iGpsTimingMeasurementData.DataType() == TGpsTimingMeasurementData::EGpsTimingDataTypeUndefined)
        {
        return KErrNotFound;
        }
    
    aGpsTimingData = iGpsTimingMeasurementData;
    return KErrNone;
    }
/**
Sets the GPS Timing Measurement Data
@param aGpsTimingData the populated GPS Timing Measurement Data to be set
 */
EXPORT_C void TPositionExtendedSatelliteInfo::SetGpsTimingData(const TGpsTimingMeasurementData& aGpsTimingData)
    {
    iGpsTimingMeasurementData = aGpsTimingData;
    }
/**
Populates aDetailedErrorReport with the GNSS Pseudorange Error Statistics(GST)data
*/
EXPORT_C void TPositionExtendedSatelliteInfo::GetDetailedErrorReport(TDetailedErrorReport& aDetailedErrorReport) const
    {
    aDetailedErrorReport = iDetailedErrorReport;
    }
/**
Sets the GNSS Pseudorange Error Statistics(GST)data
@param aDetailedErrorReport the populated GNSS Pseudorange Error Statistics(GST)data to be set
 */
EXPORT_C void TPositionExtendedSatelliteInfo::SetDetailedErrorReport(const TDetailedErrorReport& aDetailedErrorReport)
    {
    iDetailedErrorReport = aDetailedErrorReport;
    }
/**
Returns the Geoidal Separation
@return the GeoidalSeparation
 */
EXPORT_C TReal32 TPositionExtendedSatelliteInfo::GeoidalSeparation() const
    {
    return iGeoidalSeparation;
    }
/**
Sets the Geoidal Separation
@param aGeoidalSeparation The Geoidal Separation to be set
 */
EXPORT_C void TPositionExtendedSatelliteInfo::SetGeoidalSeparation(TReal32 aGeoidalSeparation)
    {
    iGeoidalSeparation = aGeoidalSeparation;
    }
/**
Returns the Magnetic Variation
@return the Magnetic Variation
 */
EXPORT_C TReal32 TPositionExtendedSatelliteInfo::MagneticVariation() const
    {
    return iMagneticVariation;
    }
/**
Sets the Magnetic Variation
@param aMagneticVariation The Magnetic Variation to be set
 */
EXPORT_C void TPositionExtendedSatelliteInfo::SetMagneticVariation(TReal32 aMagneticVariation)
    {
    iMagneticVariation = aMagneticVariation;
    }
/**
Returns the Course Over Ground Magnetic
@return the CourseOverGroundMagnetic
 */
EXPORT_C TReal32 TPositionExtendedSatelliteInfo::CourseOverGroundMagnetic() const
    {
    return iCourseOverGroundMagnetic;
    }
/**
Sets the Course Over Ground Magnetic
@param aCourseOverGroundMagnetic The Course Over Ground Magnetic to be set
 */
EXPORT_C void TPositionExtendedSatelliteInfo::SetCourseOverGroundMagnetic(TReal32 aCourseOverGroundMagnetic)
    {
    iCourseOverGroundMagnetic = aCourseOverGroundMagnetic;
    }

