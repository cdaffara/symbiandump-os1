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



// INCLUDE FILES
#include "EPos_TNmeaSentenceParser.h"

// CONSTANTS
const signed int KCoordTokenS = 'S';
const signed int KCoordTokenW = 'W';
const signed int KCoordTokenDot = '.';

const TReal32 KPosUERE = 10.0;

const TNmeaSentence KPosUnknownSentence = 0;
const TNmeaSentence KPosGGASentence = ('G'<<16) + ('G'<<8) + 'A';
const TNmeaSentence KPosGSASentence = ('G'<<16) + ('S'<<8) + 'A';
const TNmeaSentence KPosGSVSentence = ('G'<<16) + ('S'<<8) + 'V';
const TNmeaSentence KPosRMCSentence = ('R'<<16) + ('M'<<8) + 'C';
const TNmeaSentence KPosGLLSentence = ('G'<<16) + ('L'<<8) + 'L';

// PGRMM - propriatary GARMIN sentence
const TNmeaSentence KPosPGRMMSentence = ('G'<<24) + ('R'<<16) + ('M'<<8) + 'M';

const TInt KPosMinSentenceLength = 7;
const TUint KPosNMEASentenceStartToken = '$';
const TInt KPosNofSatelliteSlotsInGSA = 12;

const TReal32 KKnotsToMetrePerSecFactor = 1.852F / 3.6F;

const TInt KMaxSatellitesPerSentence = 4;

// ================= MEMBER FUNCTIONS =======================

TNmeaSentenceParser::TNmeaSentenceParser() :
    iProgress(KPosNotParsed),
    iGSVIndex(1),
    iNumNmeaSentences(0)
    {
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::ParseSentenceL
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::ParseSentence(
    const TDesC8& aSentence,
    TPositionInfo& aPosInfo, TBool aConst)
    {
    if (aSentence.Length() == 0 ||
        !(aSentence[0] == KPosNMEASentenceStartToken) ||
        !VerifyChecksum(aSentence))
        {
        // Return KPosErrorNotValidNMEASentence since this
        // sentence won't be set as parsed
        // and therefore PSY should make another try to get a full fix.
        return KPosErrorNotValidNMEASentence;
        }

    TNmeaSentence type = SentenceType(aSentence);

    // Do not permit duplicate sentences of the types GLL, GGA, GSA and RMC
    if(!aConst)
    	{
		if (type == KPosGLLSentence && (iProgress & KPosGLLParsed) ||
			type == KPosGGASentence && (iProgress & KPosGGAParsed) ||
			type == KPosGSASentence && (iProgress & KPosGSAParsed) ||
			type == KPosRMCSentence && (iProgress & KPosRMCParsed))
			{
			return KPosErrorAlreadyParsed;
			}
    	}
    
    if (aPosInfo.PositionClassType() & EPositionGenericInfoClass)
        {
        // Store raw NMEA sentence in position info
        HPositionGenericInfo& genInfo =
            static_cast<HPositionGenericInfo&> (aPosInfo);
        AddNmeaSentence(genInfo, aSentence);
        }

    switch (type)
        {
        case KPosGLLSentence:
            iProgress |= KPosGLLParsed;
            return KErrNone;
        case KPosGGASentence:
            return ParseGGA(aSentence, aPosInfo);
        case KPosRMCSentence:
            return ParseRMC(aSentence, aPosInfo);
        case KPosGSASentence:
            return ParseGSA(aSentence, aPosInfo);
        case KPosGSVSentence:
            return ParseGSV(aSentence, aPosInfo, aConst);
        case KPosPGRMMSentence:
            return ParsePGRMM(aSentence, aPosInfo);
        default:
            return KErrNone;
        }
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::Progress
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::Progress()
    {
    return iProgress;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::Reset
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void TNmeaSentenceParser::Reset()
    {
    iProgress = KPosNotParsed;
    iGSVIndex = 1;
    iNumNmeaSentences = 0;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::SentenceType
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TNmeaSentence TNmeaSentenceParser::SentenceType(const TDesC8& aSentence)
    {
    if (aSentence.Length() <= KPosMinSentenceLength)
        {
        return KPosUnknownSentence;
        }

    TInt type = (aSentence[3]<<16) + (aSentence[4]<<8) + aSentence[5];

    if (aSentence[1] == 'P')
        {
        type += (aSentence[2]<<24);
        }

    return type;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::ParseGGA
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::ParseGGA(
    const TDesC8& aSentence,
    TPositionInfo& aPositionInfo)
    {
    TDesTokeniser tokeniser(aSentence);

    tokeniser.SkipToken(2); // Initial token, timestamp

    TPtrC8 latitude = tokeniser.NextToken();
    TPtrC8 dLat = tokeniser.NextToken();

    TPtrC8 longitude = tokeniser.NextToken();
    TPtrC8 dLong = tokeniser.NextToken();

    TInt quality = DesToInt(tokeniser.NextToken());

    if (quality == 0)
        {
        // Return KPosErrorNotValidNMEASentence since this
        // sentence won't be set as parsed
        // and therefore PSY should make another try to get a full fix.
        return KPosErrorNotValidNMEASentence;
        }

    TRealX nan;
    nan.SetNaN();

    // Number of tracked satellites, Horizontal dilution
    tokeniser.SkipToken(2);

    TPtrC8 altitudeC = tokeniser.NextToken(); // // Mean-sea altitude
    TReal32 altitude = nan;
    if (altitudeC.Length() > 0)
        {
        altitude = DesToFloat(altitudeC);
        }

    TReal latCoord = nan;
    ToCoordinate(latitude, dLat, latCoord);
    TReal longCoord = nan;
    ToCoordinate(longitude, dLong, longCoord);

    tokeniser.SkipToken();  // Unit (meters)

    TPtrC8 gsValueC = tokeniser.NextToken(); // Geoidal Separation
    TReal32 gsValue = nan;
    if (gsValueC.Length() > 0)
        {
        gsValue = DesToFloat(gsValueC);
        }

    TReal32 wgs84alt = nan;
    if (!Math::IsNaN(TRealX(altitude)) && !Math::IsNaN(TRealX(gsValue)))
        {
        // Wgs84-altitude = Mean-sea level altitude + Geoidal separation
        wgs84alt = altitude + gsValue;
        }
    else
        {
        // If there is no altitude, there should not be a vertical accuracy.
        TPosition pos;
        aPositionInfo.GetPosition(pos);
        pos.SetVerticalAccuracy(nan);
        aPositionInfo.SetPosition(pos);
        }

    // Set position
    TPosition pos;
    aPositionInfo.GetPosition(pos);
    pos.SetCoordinate(latCoord, longCoord, wgs84alt);
    if (!Math::IsNaN(latCoord) || !Math::IsNaN(longCoord))
        {
        pos.SetCurrentTime();
        }
    aPositionInfo.SetPosition(pos);

    // Check if generic info class
    if (aPositionInfo.PositionClassType() & EPositionGenericInfoClass)
        {
        HPositionGenericInfo& genInfo =
            static_cast<HPositionGenericInfo&> (aPositionInfo);
        TInt err = KErrNone;
        if (genInfo.IsRequestedField(
                EPositionFieldSatelliteGeoidalSeparation) &&
                !Math::IsNaN(TRealX(gsValue)))
            {
            err = genInfo.SetValue(EPositionFieldSatelliteGeoidalSeparation,
                gsValue);
            }
        if (genInfo.IsRequestedField(
                EPositionFieldSatelliteSeaLevelAltitude) &&
                !Math::IsNaN(TRealX(altitude)))
            {
            err = genInfo.SetValue(EPositionFieldSatelliteSeaLevelAltitude,
                altitude);
            }
        if (err != KErrNone)
            {
            return err;
            }
        }

    iProgress |= KPosGGAParsed;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::ParseRMC
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::ParseRMC(
    const TDesC8& aSentence,
    TPositionInfo& aPositionInfo)
    {
    // Check if generic, course or satellite info class
    if (aPositionInfo.PositionClassType() & (EPositionGenericInfoClass |
            EPositionCourseInfoClass | EPositionSatelliteInfoClass))
        {
        TDesTokeniser tokeniser(aSentence);

        tokeniser.SkipToken(); // Initial token

        TReal timestamp = DesToReal(tokeniser.NextToken()); // Timestamp
        tokeniser.SkipToken(5); // status, latitude * 2, longitude * 2

        TRealX nan;
        nan.SetNaN();

        TPtrC8 speedC = tokeniser.NextToken(); // Speed
        TReal32 speed = nan;
        if (speedC.Length() > 0)
            {
            speed = DesToFloat(speedC);
            // Speed is in knots. Transform to metres/sec
            speed *= KKnotsToMetrePerSecFactor;
            }

        TPtrC8 trueCourseC = tokeniser.NextToken(); // True course
        TReal32 trueCourse = nan;
        if (trueCourseC.Length() > 0)
            {
            trueCourse = DesToFloat(trueCourseC);
            }

        TInt date = DesToInt(tokeniser.NextToken()); // Date

        TPtrC8 magVarC = tokeniser.NextToken(); // Magnetic variation
        TReal32 magVar = nan;
        if (magVarC.Length() > 0)
            {
            magVar = DesToFloat(magVarC);
            }
        TPtrC8 dirMagVar = tokeniser.NextToken(); // Magnetic direction
        TPtrC8 modeIndicator = tokeniser.NextToken('*'); // Mode indicator

        _LIT8(KDataNotValid, "N");
        if (modeIndicator.Compare(KDataNotValid) == 0)
            {
            return KPosErrorNotValidNMEASentence;
            }

        if (dirMagVar.Length() && dirMagVar[0] == 'E' && !Math::IsNaN(TRealX(magVar)))
            {
            magVar = -magVar;
            }

        TInt hours = static_cast<TInt> (timestamp / 10000);
        TInt minutes = (static_cast<TInt> (timestamp / 100)) % 100;
        TInt seconds = (static_cast<TInt> (timestamp)) % 100;
        TInt microSeconds =
            ((static_cast<TInt> (timestamp * 100)) % 100) * 10000;

        const TInt KInitialYear = 2000;
        TInt days = (date / 10000) % 100;
        TInt months = (date / 100) % 100;
        TInt years = date % 100 + KInitialYear;

        if (timestamp < 0 || hours > 23 || minutes > 59 || seconds > 59 ||
            date < 0 || days < 1 || days > 31 || months < 1 ||
            months > 12 || years > 2099)
            {
            return KErrCorrupt;
            }

        TDateTime time =  TDateTime
            (years, static_cast<TMonth> (months - 1), days, hours, minutes,
            seconds, microSeconds);

        // Check if generic info class
        if (aPositionInfo.PositionClassType() & EPositionGenericInfoClass)
            {
            HPositionGenericInfo& genInfo =
                static_cast<HPositionGenericInfo&> (aPositionInfo);
            TInt err = KErrNone;
            if (genInfo.IsRequestedField(EPositionFieldTrueCourse) &&
                !Math::IsNaN(TRealX(trueCourse)))
                {
                err = genInfo.SetValue(EPositionFieldTrueCourse, trueCourse);
                }
            if (genInfo.IsRequestedField(EPositionFieldHorizontalSpeed) &&
                !Math::IsNaN(TRealX(speed)))
                {
                err = genInfo.SetValue(EPositionFieldHorizontalSpeed, speed);
                }
            if (genInfo.IsRequestedField(EPositionFieldMagneticCourse) &&
                !Math::IsNaN(TRealX(magVar)) &&
                !Math::IsNaN(TRealX(trueCourse)))
                {
                err = genInfo.SetValue(EPositionFieldMagneticCourse,
                    magVar + trueCourse);
                }
            if (genInfo.IsRequestedField(EPositionFieldSatelliteTime))
                {
                err = genInfo.SetValue(EPositionFieldSatelliteTime,
                    TTime(time));
                }
            if (err != KErrNone)
                {
                return err;
                }
            }
        // course info, sat info
        else
            {
            TPositionCourseInfo& courseInfo =
                static_cast<TPositionCourseInfo&> (aPositionInfo);
            TCourse course;
            course.SetHeading(trueCourse);
            course.SetSpeed(speed);
            courseInfo.SetCourse(course);
            // satellite info
            if (aPositionInfo.PositionClassType() &
                EPositionSatelliteInfoClass)
                {
                TPositionSatelliteInfo& satInfo =
                    static_cast<TPositionSatelliteInfo&> (aPositionInfo);
                satInfo.SetSatelliteTime(TTime(time));
                }
            }
        }
    iProgress |= KPosRMCParsed;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::ParseGSA
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::ParseGSA(
    const TDesC8& aSentence,
    TPositionInfo& aPositionInfo)
    {
    TDesTokeniser tokeniser(aSentence);

    tokeniser.SkipToken(2); // Initial token, Selection

    TInt twoDthreeD = DesToInt(tokeniser.NextToken());
    if (twoDthreeD == 1)
        {
        // Return KPosErrorNotValidNMEASentence since this
        // sentence won't be set as parsed
        // and therefore PSY should make another try to get a full fix.
        return KPosErrorNotValidNMEASentence;
        }

    TInt8 numUsedSatellites = 0;
    for (TInt i = 0; i < KPosNofSatelliteSlotsInGSA; i++)
        {
        // Satelite Id nr used for fix
        TPtrC8 satIdNrC = tokeniser.NextToken();
        if (satIdNrC.Length() > 0)
            {
            // Check if satellite info class
            if (aPositionInfo.PositionClassType() &
                EPositionSatelliteInfoClass)
                {
                TPositionSatelliteInfo& satInfo =
                    static_cast<TPositionSatelliteInfo&> (aPositionInfo);

                TSatelliteData satData;
                TInt satIndex = 0;
                TInt satId = DesToInt(satIdNrC);
                TInt err = GetSatelliteListIndex(satInfo, satId, satIndex);
                if (err == KErrNotFound)
                    {
                    satData.SetSatelliteId(satId);
                    satData.SetIsUsed(ETrue);
                    err = satInfo.AppendSatelliteData(satData);
                    if (err != KErrNone)
                        {
                        return err;
                        }
                    }
                else // Satellite already exists
                    {
                    satInfo.GetSatelliteData(satIndex, satData);
                    satData.SetIsUsed(ETrue);
                    err = UpdateSatDataInList(satInfo, satData, satIndex);
                    if (err != KErrNone)
                        {
                        return err;
                        }
                    }
                }
            numUsedSatellites++;
            }
        }

    TRealX nan;
    nan.SetNaN();
    TPtrC8 pdopC = tokeniser.NextToken(); // PDop
    TReal32 pdop = nan;
    TPtrC8 hdopC = tokeniser.NextToken(); // HDop
    TReal32 hdop = nan;
    TPtrC8 vdopC = tokeniser.NextToken('*'); // VDop
    TReal32 vdop = nan;

    if (pdopC.Length() > 0)
        {
        pdop = DesToFloat(pdopC);
        }

    if (hdopC.Length() > 0)
        {
        hdop = DesToFloat(hdopC);
        // Set basic info
        TPosition pos;
        aPositionInfo.GetPosition(pos);
        pos.SetHorizontalAccuracy(KPosUERE * hdop);

        // Set vertical accuracy, but only if we have altitude, or if we may get
        // altitude in later NMEA sentences.
        if (twoDthreeD == 3 &&
            !((iProgress & KPosGGAParsed) &&
            Math::IsNaN(TRealX(pos.Altitude()))))
            {
            pos.SetVerticalAccuracy(1.5f * KPosUERE * hdop);
            }
        aPositionInfo.SetPosition(pos);
        }

    if (vdopC.Length() > 0)
        {
        vdop = DesToFloat(vdopC);
        }

    // Check if generic info class
    if (aPositionInfo.PositionClassType() & EPositionGenericInfoClass)
        {
        HPositionGenericInfo& genInfo =
                    static_cast<HPositionGenericInfo&> (aPositionInfo);
        TInt err = KErrNone;

        if (TRealX(pdop).IsNaN() &&
            !TRealX(vdop).IsNaN() &&
            !TRealX(hdop).IsNaN())
            {
            TReal trg(pdop);
            err = Math::Sqrt(trg, TReal(hdop * hdop + vdop * vdop));
            pdop = TReal32(trg);
            }

        if (genInfo.IsRequestedField(EPositionFieldSatelliteHorizontalDoP) &&
            !Math::IsNaN(TRealX(hdop)))
            {
            err = genInfo.SetValue(EPositionFieldSatelliteHorizontalDoP, hdop);
            }
        if (genInfo.IsRequestedField(EPositionFieldSatelliteVerticalDoP) &&
            !Math::IsNaN(TRealX(vdop)))
            {
            err = genInfo.SetValue(EPositionFieldSatelliteVerticalDoP, vdop);
            }
        if (genInfo.IsRequestedField(EPositionFieldSatellitePositionDoP) &&
            !Math::IsNaN(TRealX(pdop)))
            {
            err = genInfo.SetValue(EPositionFieldSatellitePositionDoP, pdop);
            }
        if (genInfo.IsRequestedField(EPositionFieldSatelliteNumUsed))
            {
            err = genInfo.SetValue(EPositionFieldSatelliteNumUsed,
                numUsedSatellites);
            }
        if (err != KErrNone)
            {
            return err;
            }
        }
    // satellite info
    else if (aPositionInfo.PositionClassType() & EPositionSatelliteInfoClass)
        {
        TPositionSatelliteInfo& satInfo =
                static_cast<TPositionSatelliteInfo&> (aPositionInfo);
        satInfo.SetHorizontalDoP(hdop);
        satInfo.SetVerticalDoP(vdop);
        }

    iProgress |= KPosGSAParsed;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::ParseGSV
//
// Must be called in sequence for each GSV sentence
// otherwise an error code will be returned.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::ParseGSV(
    const TDesC8& aSentence,
    TPositionInfo& aPositionInfo,
    TBool aConst)
    {
    TDesTokeniser tokeniser(aSentence);

    tokeniser.SkipToken(2); // Initial token, Number of sentences

    TInt sentenceIndex = DesToInt(tokeniser.NextToken()); // Message number

    // OK to try to parse a sentence with the wrong index, but progress
    // will not be updated.
    //
    // Because iGSVIndex is incremented each time function is called with aConst = EFalse
    // then to get same result like for previous read we have to temporary decrement
    // this value. Then if aConst is ETrue we are decrementing iGSVIndex
    TInt tempGSVIndex = iGSVIndex;
    if(aConst)
    	{
    	tempGSVIndex--;
    	}
    if (sentenceIndex != tempGSVIndex)
        {
        iGSVIndex = 1;
        return KErrNone;
        }
    if(!aConst)
    	{
    	iGSVIndex++;
    	}

    // Number of satellites in view
    TInt8 satInView = static_cast<TInt8> (DesToInt(tokeniser.NextToken()));

    // Check if generic info class
    if (aPositionInfo.PositionClassType() & EPositionGenericInfoClass)
        {
        HPositionGenericInfo& genInfo =
                    static_cast<HPositionGenericInfo&> (aPositionInfo);
        TInt err = KErrNone;
        if (genInfo.IsRequestedField(EPositionFieldSatelliteNumInView))
            {
            err = genInfo.SetValue(EPositionFieldSatelliteNumInView,
                satInView);
            if (err != KErrNone)
                {
                return err;
                }
            }
        }
    // satellite info
    else if (aPositionInfo.PositionClassType() & EPositionSatelliteInfoClass)
        {
        TPositionSatelliteInfo& satInfo =
                static_cast<TPositionSatelliteInfo&> (aPositionInfo);

        for (TInt n = 0; n < KMaxSatellitesPerSentence; n++)
            {
            TPtrC8 prnPtrC = tokeniser.NextToken(); // PRN
            TPtrC8 elevationPtrC = tokeniser.NextToken(); // Elevation
            TPtrC8 azimuthPtrC = tokeniser.NextToken(); // Azimuth

            if (prnPtrC.Length() > 0)
                {
                TPtrC8 signalStrengtPtrC;
                // Last satellite in sentence
                if (n == (KMaxSatellitesPerSentence - 1))
                    {
                    signalStrengtPtrC.Set(tokeniser.NextToken('*')); // SNR
                    }
                else
                    {
                    signalStrengtPtrC.Set(tokeniser.NextToken()); // SNR
                    }

                TSatelliteData satSata;
                TInt satIndex;
                TInt satId = DesToInt(prnPtrC);
                TInt err = GetSatelliteListIndex(satInfo, satId, satIndex);
                if (err == KErrNotFound)
                    {
                    satSata.SetSatelliteId(satId);
                    if (elevationPtrC.Length() > 0)
                        {
                        satSata.SetElevation(DesToFloat(elevationPtrC));
                        }
                    if (azimuthPtrC.Length() > 0)
                        {
                        satSata.SetAzimuth(DesToFloat(azimuthPtrC));
                        }
                    if (signalStrengtPtrC.Length() > 0)
                        {
                         satSata.SetSignalStrength(DesToInt(signalStrengtPtrC));
                        }
                    err = satInfo.AppendSatelliteData(satSata);
                    if (err != KErrNone)
                        {
                        return err;
                        }
                    }
                else // Satellite already exists
                    {
                    satInfo.GetSatelliteData(satIndex, satSata);
                    satSata.SetSatelliteId(DesToInt(prnPtrC));
                    if (elevationPtrC.Length() > 0)
                        {
                        satSata.SetElevation(DesToFloat(elevationPtrC));
                        }
                    if (azimuthPtrC.Length() > 0)
                        {
                        satSata.SetAzimuth(DesToFloat(azimuthPtrC));
                        }
                    if (signalStrengtPtrC.Length() > 0)
                        {
                        satSata.SetSignalStrength(DesToInt(signalStrengtPtrC));
                        }
                    err = UpdateSatDataInList(satInfo, satSata, satIndex);
                    if (err != KErrNone)
                        {
                        return err;
                        }
                    }
                }
            }
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::ParsePGRMM
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::ParsePGRMM(
    const TDesC8& aSentence,
    TPositionInfo& /*aPositionInfo*/)
    {
    _LIT8(KWgs84, "WGS 84");
    TDesTokeniser tokeniser(aSentence);

    tokeniser.SkipToken(); // initial token

    TPtrC8 datum = tokeniser.NextToken('*');
    if (datum.Compare(KWgs84) != 0)
        {
        return KErrNotSupported;
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::AddNmeaSentence
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::AddNmeaSentence(
    HPositionGenericInfo& aPositionGenericInfo,
    const TDesC8& aSentence, TBool aConst)
    {
    TInt err = KErrNone;

    // Check if NMEA data is requested
    if (aPositionGenericInfo.IsRequestedField(EPositionFieldNMEASentences))
        {
        TUint16 sentenceNumber =
            static_cast<TUint16> (EPositionFieldNMEASentencesStart +
            iNumNmeaSentences);
        err = aPositionGenericInfo.SetValue(sentenceNumber, aSentence);
        if(!aConst)
        	++iNumNmeaSentences;
        TUint8 sentences = static_cast<TUint8> (iNumNmeaSentences);
        err = aPositionGenericInfo.SetValue(EPositionFieldNMEASentences,
            sentences);
        }
    return err;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::ToCoordinate
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void TNmeaSentenceParser::ToCoordinate(
    const TDesC8& aDes,
    const TDesC8& aDir,
    TReal& aCoordinate)
    {
    const TInt  KPosMinutesIntegerLength = 2;
    const TReal KPosMinutesPerDegree = 60.0;

    TInt point = aDes.Locate(KCoordTokenDot);

    if (point >= KPosMinutesIntegerLength)
        {
        TInt deg = DesToInt(aDes.Left(point - KPosMinutesIntegerLength));
        TReal min = DesToReal(aDes.Mid(point - KPosMinutesIntegerLength));
        aCoordinate = TReal(deg) + min / KPosMinutesPerDegree;

        if (aDir.Length() && (aDir[0] == KCoordTokenS || aDir[0] == KCoordTokenW))
            {
            aCoordinate = - aCoordinate;
            }
        }
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::DesToInt
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::DesToInt(const TPtrC8& aDes)
    {
    TLex8 lexer(aDes);
    TInt integer;

    lexer.Val(integer);
    return integer;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::DesToFloat
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TReal32 TNmeaSentenceParser::DesToFloat(const TPtrC8& aDes)
    {
    TLex8 lexer(aDes);
    TReal32 floatNumber;

    lexer.Val(floatNumber, TChar(KCoordTokenDot));
    return floatNumber;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::DesToReal
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TReal TNmeaSentenceParser::DesToReal(const TPtrC8& aDes)
    {
    TLex8 lexer(aDes);
    TReal realNumber;

    lexer.Val(realNumber, TChar('.'));
    return realNumber;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::VerifyChecksum
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool TNmeaSentenceParser::VerifyChecksum(const TDesC8& aSentence)
    {
    TUint8 checksum = 0;
    TInt i;
    for (i = 1; i < aSentence.Length() && aSentence[i] != '*'; i++)
        {
        checksum ^= aSentence[i];
        }

    if (++i + 1 < aSentence.Length())
        {
        TUint8 sum = TUint8((CharToNibble(aSentence[i]) << 4) +
            CharToNibble(aSentence[i+1]));
        if (sum == checksum)
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::CharToNibble
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TUint8 TNmeaSentenceParser::CharToNibble(const TUint8 aChar)
    {
    if (aChar <= '9')
        {
        return TUint8(aChar - '0');
        }
    else
        {
        return TUint8(aChar - 'A' + 10);
        }
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::GetSatelliteListIndex
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::GetSatelliteListIndex(
    const TPositionSatelliteInfo& aSatInfo,
    TInt aPrnNumber,
    TInt& aIndex)
    {
    TSatelliteData satData;

    for (TInt i = 0; i < aSatInfo.NumSatellitesInView(); i++)
        {
        aSatInfo.GetSatelliteData(i, satData);
        if (satData.SatelliteId() == aPrnNumber)
            {
            aIndex = i;
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// TNmeaSentenceParser::UpdateSatDataInList
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt TNmeaSentenceParser::UpdateSatDataInList(
    TPositionSatelliteInfo& aSatInfo,
    TSatelliteData aSatData,
    TInt aIndex)
    {
    TInt nrOfSat = aSatInfo.NumSatellitesInView();
    RArray<TSatelliteData> satList;
    TInt i;
    TInt err;
    for (i = 0; i < nrOfSat; i++)
        {
        TSatelliteData satData;
        aSatInfo.GetSatelliteData(i,satData);
        err = satList.Append(satData);
        if (err != KErrNone)
            {
            satList.Close();
            return err;
            }
        }

    if (aIndex < nrOfSat)
        {
        satList[aIndex] = aSatData;
        }

    // Re-create satellite list in SatelliteInfo
    aSatInfo.ClearSatellitesInView();
    for (i = 0; i < satList.Count(); i++)
        {
        err = aSatInfo.AppendSatelliteData(satList[i]);
        if (err != KErrNone)
            {
            satList.Close();
            return err;
            }
        }
    satList.Close();
    return KErrNone;
    }

//  End of File
