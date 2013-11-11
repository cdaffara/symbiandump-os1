// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// utility fucntions, and odds and sods for various bits of the location server
// 
//

/**
 @file
 @internalComponent
 @deprecated
*/

#include <lbssatellite.h>
#include "nmeafunctions.h"


// $GPGGA,hhmmss.sss,llll.lllll,a,yyyyy.yyyyy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx
// $GPGGA size < 90 
// In the NMEA standard the time is in hundreth of a second, but in previous code miliseconds are used
// In the NMEA standard latitude and longitude are specified with 2 decimals, but in previous code 5 decimals were used
_LIT8(KNmeaGga, "$GPGGA,%S,%S,%S,%d,%02d,%S,%S,%S,,");
// hhmmss.ss
_LIT8(KLbsNmeaUtcTimeFormat, "%02d%02d%02d.%03d");
// llll.ll,a
_LIT8(KLbsNmeaLatFormat, "%02u%08.5lf,%S");
// yyyyy.yy,a
_LIT8(KLbsNmeaLonFormat, "%03u%08.5lf,%S");
// x.x
_LIT8(KLbsNmeaOneDecimalFormat, "%.1f");
// x.x,M
_LIT8(KLbsNmeaOneDecimalMFormat, "%.1f,M");
_LIT8(KDefaultLatLongAltBuf, ",");
_LIT8(KLbsNmeaNorth, "N");
_LIT8(KLbsNmeaSouth, "S");
_LIT8(KLbsNmeaEast, "E");
_LIT8(KLbsNmeaWest, "W");

const TInt KMicrosecondToMiliseconds = 1000;
const TInt KMinutesToADegree = 60;

// $GPGLL,llll.lllll,a,yyyyy.yyyyy,a,hhmmss.sss,A,a
// size < 50
// In the NMEA standard the time is in hundreth of a second, but in previous code miliseconds are used
// In the NMEA standard latitude and longitude are specified with 2 decimals, but in previous code 5 decimals were used
_LIT8(KNmeaGll, "$GPGLL,%S,%S,%S,%S,%S");

_LIT8(KLbsNmeaValidStatus, "A");
_LIT8(KLbsNmeaValidModeIndicator, "A");
_LIT8(KLbsNmeaInvalidStatus, "V");
_LIT8(KLbsNmeaInvalidModeIndicator, "N");

// $GPGSA,a,x,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x
// size < 60
_LIT8(KLbsNmeaGsa, "$GPGSA,A,%d,%S,%S,%S,%S");

// $GPGST,hhmmss.sss,x.x,x.x,x.x,x.x,x.x,x.x,x.x
// size < 100
_LIT8(KNmeaGst, "$GPGST,%S,%S,%S,%S,%S,%S,%S,%S");

// $GPGSV,x,x,xx,xx,xx,xxx,xx,...,xx,xx,xxx,xx
// size < 64
_LIT8(KLbsNmeaGsv, "$GPGSV,%d,%d,%02d,");
_LIT8(KLbsNmeaGsvSatInfo, "%02d,%S,%S,%02d");
_LIT8(KLbsNmeaGsvEmptySatInfo, ",,,");
_LIT8(KLbsNmeaElevationFormat, "%02.0f");
_LIT8(KLbsNmeaAzimuthFormat, "%03.0f");

// $GPRMC,hhmmss.sss,A,llll.lllll,a,yyyyy.yyyyy,a,x.x,x.x,ddmmyy,x.x,a,a
// size < 50
_LIT8(KNmeaRmc, "$GPRMC,%S,%S,%S,%S,%S,%S,%S,%S,%S");
_LIT8(KLbsNmeaDateFormat, "%02d%02d%02d");
_LIT8(KLbsNmeaMagneticVariationFormat, "%.1f,%S");

// $GPVTG,x.x,T,x.x,M,x.x,N,x.x,K,a
// size < 45
_LIT8(KLbsNmeaVtg, "$GPVTG,%S,T,%S,M,%S,N,%S,K,%S");

/**
  Creates an NMEA GGA string:
  $GPGGA,hhmmss.sss,llll.lllll,a,yyyyy.yyyyy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx
  size < 90
  
  @param aTo The descriptor where the string gets written (anything inside is overwritten)
  @param aFrom The structure from which the information is taken
 */ 
void CreateGga(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom)
    {
    aTo.Zero();
    TPosition pos;
    aFrom.GetPosition(pos);
    
    TBuf8<10> utcTimeBuf;
    CreateUtcTimeBuffer(utcTimeBuf, pos);
    
    TBuf8<20> latBuf;
    CreateLatitudeBuffer(latBuf, pos);
    
    TBuf8<20> lonBuf;
    CreateLongitudeBuffer(lonBuf, pos);
    
    TInt quality = (!Math::IsNaN(pos.Latitude()) && !Math::IsNaN(pos.Longitude()) && !Math::IsNaN(pos.Altitude())) ? 1 : 0;
    
    TInt numSatsUsed = aFrom.NumSatellitesUsed();
    
    TBuf8<4> hdopBuf;
    CreateDoPBuffer(hdopBuf, aFrom.HorizontalDoP(), quality == 1);
    
    TReal32 geoid = aFrom.GeoidalSeparation();    
    TBuf8<9> altBuf;
    TBuf8<9> geoidBuf;
    TReal32 altitude = pos.Altitude();
    if(!Math::IsNaN(altitude) && !Math::IsNaN(geoid))
        {
        altBuf.AppendFormat(KLbsNmeaOneDecimalMFormat, altitude + geoid);
        geoidBuf.AppendFormat(KLbsNmeaOneDecimalMFormat, -geoid);
        }
    else
        {
        altBuf = KDefaultLatLongAltBuf;
        geoidBuf = KDefaultLatLongAltBuf;
        }

    aTo.AppendFormat(KNmeaGga, &utcTimeBuf, &latBuf, &lonBuf, quality, numSatsUsed, &hdopBuf, &altBuf, &geoidBuf);
    }

/**
  Creates an NMEA GLL string:
  $GPGLL,llll.lllll,a,yyyyy.yyyyy,a,hhmmss.sss,A,a
  size < 50
  
  @param aTo The descriptor where the string gets written (anything inside is overwritten)
  @param aFrom The structure from which the information is taken
 */ 
void CreateGll(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom)
    {
    aTo.Zero();
    TPosition pos;
    aFrom.GetPosition(pos);
    
    TBuf8<10> utcTimeBuf;
    CreateUtcTimeBuffer(utcTimeBuf, pos);
    
    TBuf8<20> latBuf;
    CreateLatitudeBuffer(latBuf, pos);
    
    TBuf8<20> lonBuf;
    CreateLongitudeBuffer(lonBuf, pos);

    TBuf8<1> status;
    TBuf8<1> modeIndicator;
    
    if((!Math::IsNaN(pos.Latitude()) && !Math::IsNaN(pos.Longitude()) && !Math::IsNaN(pos.Altitude())))
        {
        status = KLbsNmeaValidStatus;
        modeIndicator = KLbsNmeaValidModeIndicator;
        }
    else
        {
        status = KLbsNmeaInvalidStatus;
        modeIndicator = KLbsNmeaInvalidModeIndicator;
        }
    
    aTo.AppendFormat(KNmeaGll, &latBuf, &lonBuf, &utcTimeBuf, &status, &modeIndicator);
    }

/**
  Creates an NMEA GSA string:
  $GPGSA,a,x,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x
  size < 60
  
  @param aTo The descriptor where the string gets written (anything inside is overwritten)
  @param aFrom The structure from which the information is taken
 */ 
void CreateGsa(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom)
    {
    aTo.Zero();
    TPosition pos;
    aFrom.GetPosition(pos);
    
    TInt fixType = 1;
    
    if(!Math::IsNaN(pos.Latitude()) && !Math::IsNaN(pos.Longitude()))
            {
            if(!Math::IsNaN(pos.Altitude()))
                {
                fixType = 3;
                }
            else
                {
                fixType = 2;
                }
            }
    
    TBuf8<36> usedSatsBuffer;
    // because there can be more than 12 satellites in view we want to make sure the ones used
    // are being recorded
    // so the spaces left to fill will be checked against the remaining satellites in view
    // in order to decide if an empty field will be added
    TInt spacesLeft = 12;
    TInt satsInView = aFrom.NumSatellitesInView();
    // add sattelite data only if the fix is avaialable. Make sure exactly 12 spaces get filled
    TSatelliteData satData;
    for(TInt index = 0; (index < satsInView) && (fixType > 1) && (spacesLeft > 0); ++index)
        {
        aFrom.GetSatelliteData(index, satData);
        // add satellite data if satellite is used, otherwise insert blank field
        if(satData.IsUsed())
            {
            usedSatsBuffer.AppendNumFixedWidth(satData.SatelliteId(), EDecimal, 2);
            --spacesLeft;
            if(spacesLeft > 0)
                {
                // only add , if it's not the last element to be added
                usedSatsBuffer.Append(',');
                }
            }
        else if(spacesLeft >= satsInView - index)
            {
            --spacesLeft;
            if(spacesLeft > 0)
                {
                // only add , if it's not the last element to be added
                usedSatsBuffer.Append(',');
                }
            }
        }
    // fill the rest of the fields
    while(spacesLeft > 0)
        {
        --spacesLeft;
        if(spacesLeft > 0)
            {
            // only add , if it's not the last element to be added
            usedSatsBuffer.Append(',');
            }
        }

    TBuf8<4> pdopBuf;
    TReal64 pdop;
    TReal64 hdop = aFrom.HorizontalDoP();
    TReal64 vdop = aFrom.VerticalDoP();
    if(!Math::IsNaN(hdop) && !Math::IsNaN(vdop))
        {
        Math::Sqrt(pdop, hdop*hdop + vdop*vdop);
        }
    else
        {
        TRealX nan;
        nan.SetNaN();
        pdop = nan;
        }
    CreateDoPBuffer(pdopBuf, pdop, fixType > 1);

    TBuf8<4> hdopBuf;
    CreateDoPBuffer(hdopBuf, hdop, fixType > 1);

    TBuf8<4> vdopBuf;
    CreateDoPBuffer(vdopBuf, vdop, fixType > 1);

    aTo.AppendFormat(KLbsNmeaGsa, fixType, &usedSatsBuffer, &pdopBuf, &hdopBuf, &vdopBuf);
    }

/**
  Creates an NMEA GST string:
  $GPGST,hhmmss.sss,x.x,x.x,x.x,x.x,x.x,x.x,x.x
  size < 100
  
  @param aTo The descriptor where the string gets written (anything inside is overwritten)
  @param aFrom The structure from which the information is taken
 */ 
void CreateGst(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom)
    {
    aTo.Zero();
    TPosition pos;
    aFrom.GetPosition(pos);
    
    TBuf8<10> utcTimeBuf;
    CreateUtcTimeBuffer(utcTimeBuf, pos);
    
    TDetailedErrorReport errorReport;
    aFrom.GetDetailedErrorReport(errorReport);
    
    TBuf8<10> rmsValueBuffer;
    TReal32 value = errorReport.RmsValOfStanDeviOfRange();
    if(!Math::IsNaN(value))
        {
        rmsValueBuffer.AppendFormat(KLbsNmeaOneDecimalMFormat, value);
        }
    else
        {
        rmsValueBuffer = KNullDesC8;
        }

    TBuf8<10> deviSemiMajorAxisBuffer;
    value = errorReport.StanDeviOfSemiMajorAxisError();
    if(!Math::IsNaN(value))
        {
        deviSemiMajorAxisBuffer.AppendFormat(KLbsNmeaOneDecimalMFormat, value);
        }
    else
        {
        deviSemiMajorAxisBuffer = KNullDesC8;
        }

    TBuf8<10> deviSemiMinorAxisBuffer;
    value = errorReport.StanDeviOfSemiMinorAxisError();
    if(!Math::IsNaN(value))
        {
        deviSemiMinorAxisBuffer.AppendFormat(KLbsNmeaOneDecimalMFormat, value);
        }
    else
        {
        deviSemiMinorAxisBuffer = KNullDesC8;
        }

    TBuf8<10> orientationSemiMjorAxisBuffer;
    value = errorReport.OrientationOfSemiMajorAxisError();
    if(!Math::IsNaN(value))
        {
        orientationSemiMjorAxisBuffer.AppendFormat(KLbsNmeaOneDecimalMFormat, value);
        }
    else
        {
        orientationSemiMjorAxisBuffer = KNullDesC8;
        }

    TBuf8<10> deviLatitudeBuffer;
    value = errorReport.StanDeviOfLatiitudeError();
    if(!Math::IsNaN(value))
        {
        deviLatitudeBuffer.AppendFormat(KLbsNmeaOneDecimalMFormat, value);
        }
    else
        {
        deviLatitudeBuffer = KNullDesC8;
        }

    TBuf8<10> deviLongitudeBuffer;
    value = errorReport.StanDeviOfLongitudeError();
    if(!Math::IsNaN(value))
        {
        deviLongitudeBuffer.AppendFormat(KLbsNmeaOneDecimalMFormat, value);
        }
    else
        {
        deviLongitudeBuffer = KNullDesC8;
        }

    TBuf8<10> deviAltitudeBuffer;
    value = errorReport.StanDeviOfAltitudeError();
    if(!Math::IsNaN(value))
        {
        deviAltitudeBuffer.AppendFormat(KLbsNmeaOneDecimalMFormat, value);
        }
    else
        {
        deviAltitudeBuffer = KNullDesC8;
        }


    aTo.AppendFormat(KNmeaGst, &utcTimeBuf, &rmsValueBuffer, &deviSemiMajorAxisBuffer, 
            &deviSemiMinorAxisBuffer, &orientationSemiMjorAxisBuffer, &deviLatitudeBuffer, 
            &deviLongitudeBuffer, &deviAltitudeBuffer);
    }


/**
  Creates an NMEA GSV string (next one in sequence based on the sentences done so far. This 
  method should be called repeatedly, until all sentences have been written.
  $GPGSV,x,x,xx,xx,xx,xxx,xx,...,xx,xx,xxx,xx
  size < 64
  
  @param aTo The descriptor where the string gets written (anything inside is overwritten)
  @param aFrom The structure from which the information is taken
  @param aTotalSentences This is where the method writes the total number of sentences required
  @param aLastSentence This is where the method writes the value of the latest created sentence
 */ 
void CreateGsv(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom, TInt& aTotalSentences, TInt& aLastSentence)
    {
    aTo.Zero();
    TInt satsInView = aFrom.NumSatellitesInView();
    aTotalSentences = (satsInView+3)/4;
    ++aLastSentence;
    aTo.AppendFormat(KLbsNmeaGsv, aTotalSentences, aLastSentence, satsInView);
    
    for(TInt index = (aLastSentence-1)*4; index < aLastSentence*4; ++index)
        {
        if(index < satsInView)
            {
            TSatelliteData satData;
            aFrom.GetSatelliteData(index, satData);
            TBuf8<2> elevationBuffer;
            if(!Math::IsNaN(satData.Elevation()))
                {
                elevationBuffer.AppendFormat(KLbsNmeaElevationFormat, satData.Elevation());
                }
            else
                {
                elevationBuffer = KNullDesC8;
                }

            TBuf8<3> azimuthBuffer;
            if(!Math::IsNaN(satData.Azimuth()))
                {
                azimuthBuffer.AppendFormat(KLbsNmeaAzimuthFormat, satData.Azimuth());
                }
            else
                {
                azimuthBuffer = KNullDesC8;
                }
            TBuf8<12> satInfoBuffer;
            satInfoBuffer.AppendFormat(KLbsNmeaGsvSatInfo, satData.SatelliteId(), 
                    &elevationBuffer, &azimuthBuffer, satData.SignalStrength());
            aTo.Append(satInfoBuffer);
            }
        else
            {
            aTo.Append(KLbsNmeaGsvEmptySatInfo);
            }
        if(index < aLastSentence*4 - 1)
            {
            aTo.Append(',');
            }
        }
    }

/**
  Creates an NMEA RMC string:
  $GPRMC,hhmmss.sss,A,llll.lllll,a,yyyyy.yyyyy,a,x.x,x.x,ddmmyy,x.x,a,a
  size < 50
  
  @param aTo The descriptor where the string gets written (anything inside is overwritten)
  @param aFrom The structure from which the information is taken
 */ 
void CreateRmc(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom)
    {
    aTo.Zero();
    TPosition pos;
    aFrom.GetPosition(pos);
    
    TBuf8<10> utcTimeBuf;
    CreateUtcTimeBuffer(utcTimeBuf, pos);
    
    TBuf8<20> latBuf;
    CreateLatitudeBuffer(latBuf, pos);
    
    TBuf8<20> lonBuf;
    CreateLongitudeBuffer(lonBuf, pos);
    
    TBuf8<1> status;
    TBuf8<1> modeIndicator;
    if((!Math::IsNaN(pos.Latitude()) && !Math::IsNaN(pos.Longitude()) && !Math::IsNaN(pos.Altitude())))
        {
        status = KLbsNmeaValidStatus;
        modeIndicator = KLbsNmeaValidModeIndicator;
        }
    else
        {
        status = KLbsNmeaInvalidStatus;
        modeIndicator = KLbsNmeaInvalidModeIndicator;
        }
    
    TCourse course;
    aFrom.GetCourse(course);
    
    TBuf8<10> speedBuf;
    CreateSpeedBuffer(speedBuf, course);
    
    TBuf8<10> headingBuf;
    CreateHeadingBuffer(headingBuf, course);

    TBuf8<6> dateBuf;
    TDateTime date(pos.Time().DateTime());
    dateBuf.AppendFormat(KLbsNmeaDateFormat, date.Day(), date.Month()+1, date.Year() % 100);
    
    TBuf8<6> magneticVariationBuf;
    TReal32 magneticVariation = aFrom.MagneticVariation();
    if(!Math::IsNaN(magneticVariation))
        {
        TBuf8<1> direction;
        if(magneticVariation >= 0)
            {
            direction = KLbsNmeaEast;
            }
        else
            {
            direction = KLbsNmeaWest;
            magneticVariation = -magneticVariation;
            }
        magneticVariationBuf.AppendFormat(KLbsNmeaMagneticVariationFormat, magneticVariation, &direction);
        }
    else
        {
        magneticVariationBuf = KDefaultLatLongAltBuf;
        }    
    
    aTo.AppendFormat(KNmeaRmc, &utcTimeBuf, &status, &latBuf, &lonBuf, &speedBuf, 
            &headingBuf, &dateBuf, &magneticVariationBuf, &modeIndicator);
    }

/**
  Creates an NMEA VTG string:
  $GPVTG,x.x,T,x.x,M,x.x,N,x.x,K,a
  size < 45
  
  @param aTo The descriptor where the string gets written (anything inside is overwritten)
  @param aFrom The structure from which the information is taken
 */ 
void CreateVtg(TDes8& aTo, const TPositionExtendedSatelliteInfo& aFrom)
    {
    aTo.Zero();
    TCourse course;
    aFrom.GetCourse(course);
    
    TBuf8<10> headingBuf;
    CreateHeadingBuffer(headingBuf, course);
    
    TBuf8<10> courseDegreesMagneticBuf;
    if(!Math::IsNaN(aFrom.CourseOverGroundMagnetic()))
        {
        courseDegreesMagneticBuf.AppendFormat(KLbsNmeaOneDecimalFormat, aFrom.CourseOverGroundMagnetic());
        }
    else
        {
        courseDegreesMagneticBuf = KNullDesC8;
        }

    TBuf8<10> speedBuf;
    CreateSpeedBuffer(speedBuf, course);
    
    TBuf8<10> speedKmHBuf;
    if(!Math::IsNaN(course.Speed()))
        {
        speedKmHBuf.AppendFormat(KLbsNmeaOneDecimalFormat, course.Speed()*3.6);
        }
    else
        {
        speedKmHBuf = KNullDesC8;
        }
    TBuf8<1> modeIndicator;
    if(!Math::IsNaN(course.Speed()) && !Math::IsNaN(course.Heading()))
        {
        modeIndicator = KLbsNmeaValidModeIndicator;
        }
    else
        {
        modeIndicator = KLbsNmeaInvalidModeIndicator;
        }
    
    aTo.AppendFormat(KLbsNmeaVtg, &headingBuf, &courseDegreesMagneticBuf, &speedBuf, &speedKmHBuf, &modeIndicator);
    }

/**
  Helper for creating part of an NMEA string: UTC time
  
  @param aBuffer The descriptor where the string gets written
  @param aPos The structure from which the information is taken
 */ 
void CreateUtcTimeBuffer(TDes8& aBuffer, const TPosition& aPos)
    {
    TDateTime utcDateTime(aPos.Time().DateTime());
    aBuffer.AppendFormat(KLbsNmeaUtcTimeFormat, utcDateTime.Hour(), utcDateTime.Minute(), utcDateTime.Second(), utcDateTime.MicroSecond()/KMicrosecondToMiliseconds);
    }

/**
  Helper for creating part of an NMEA string: Latitude
  
  @param aBuffer The descriptor where the string gets written
  @param aPos The structure from which the information is taken
 */ 
void CreateLatitudeBuffer(TDes8& aBuffer, const TPosition& aPos)
    {
    TReal64 latitude = aPos.Latitude();
    if(!Math::IsNaN(latitude))
        {
        TBuf8<1> latDirection;
        if(latitude >= 0)
            {
            latDirection = KLbsNmeaNorth;
            }
        else
            {
            latDirection = KLbsNmeaSouth;
            latitude = -latitude;
            }
        TInt latDegrees = latitude;
        TReal64 latMinutes = (latitude - latDegrees) * KMinutesToADegree;
        __ASSERT_DEBUG((latDegrees >= 0) && (latDegrees <= 90) && (latMinutes >= 0) && (latMinutes < 60), User::Invariant());
        aBuffer.AppendFormat(KLbsNmeaLatFormat, latDegrees, latMinutes, &latDirection);
        }
    else
        {
        aBuffer = KDefaultLatLongAltBuf;
        }    
    }

/**
  Helper for creating part of an NMEA string: Longitude
  
  @param aBuffer The descriptor where the string gets written
  @param aPos The structure from which the information is taken
 */ 
void CreateLongitudeBuffer(TDes8& aBuffer, const TPosition& aPos)
    {
    TReal64 longitude = aPos.Longitude();
    if(!Math::IsNaN(longitude))
        {
        TBuf8<1> lonDirection;
        if(longitude >= 0)
            {
            lonDirection = KLbsNmeaEast;
            }
        else
            {
            lonDirection = KLbsNmeaWest;
            longitude = -longitude;
            }
        TInt lonDegrees = longitude;
        TReal64 lonMinutes = (longitude - lonDegrees) * KMinutesToADegree;
        __ASSERT_DEBUG((lonDegrees >= 0) && (lonDegrees <= 180) && (lonMinutes >= 0) && (lonMinutes < 60), User::Invariant());
        aBuffer.AppendFormat(KLbsNmeaLonFormat, lonDegrees, lonMinutes, &lonDirection);
        }
    else
        {
        aBuffer = KDefaultLatLongAltBuf;
        }    
    }

/**
  Helper for creating part of an NMEA string: Dilution of Precision
  
  @param aBuffer The descriptor where the string gets written
  @param aDop The structure from which the information is taken
  @param aValidFix Specifies if the fix is valid or not
 */ 
void CreateDoPBuffer(TDes8& aBuffer, TReal32 aDop, TBool aValidFix)
    {
    if(aValidFix && !Math::IsNaN(aDop))
        {
        aBuffer.AppendFormat(KLbsNmeaOneDecimalFormat, aDop);
        }
    else
        {
        aBuffer = KNullDesC8;
        }    
    }

/**
  Helper for creating part of an NMEA string: speed
  
  @param aBuffer The descriptor where the string gets written
  @param aCourse The structure from which the information is taken
 */ 
void CreateSpeedBuffer(TDes8& aBuffer, const TCourse& aCourse)
    {
    if(!Math::IsNaN(aCourse.Speed()))
        {
        aBuffer.AppendFormat(KLbsNmeaOneDecimalFormat, aCourse.Speed()*1.94384);
        }
    else
        {
        aBuffer = KNullDesC8;
        }    
    }

/**
  Helper for creating part of an NMEA string: heading
  
  @param aBuffer The descriptor where the string gets written
  @param aCourse The structure from which the information is taken
 */ 
void CreateHeadingBuffer(TDes8& aBuffer, const TCourse& aCourse)
    {
    if(!Math::IsNaN(aCourse.Heading()))
        {
        aBuffer.AppendFormat(KLbsNmeaOneDecimalFormat, aCourse.Heading());
        }
    else
        {
        aBuffer = KNullDesC8;
        }
    }
//
