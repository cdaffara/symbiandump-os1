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
#include <e32math.h>
#include <lbspositioninfo.h>
#include "EPos_TPosDataSimulator.h"
#include "EPos_TDesTokeniser.h"
#include "EPos_SimPsyConstants.h"

// CONSTANTS
const TUint8 KPosDataSeparator = '=';
const TUint8 KPosDefaultDataEndMarker = ';';

_LIT8(KHacc, "Horizontal accuracy");
_LIT8(KVacc, "Vertical accuracy");
_LIT8(KPowerupTime, "Powerup time");
_LIT8(KLongitude, "Longitude");
_LIT8(KLatitude, "Latitude");
_LIT8(KSpeed, "Speed");
_LIT8(KCourse, "Course");
_LIT8(KRandom, "Random");
_LIT8(KDeterministic, "Deterministic");
_LIT8(KTimeToFixMin, "TimeToFix min");
_LIT8(KTimeToFixMax, "TimeToFix max");

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TPosDataSimulator::TPosDataSimulator() :
    iHAcc(0),
    iVAcc(0),
    iLonErrorRadius(0),
    iLatErrorRadius(0),
    iErrorAngle(0),
    iSpeed(0),
    iTrueCourse(0),
    iOrigLatitude(0),
    iOrigLongitude(0),
    iLatitude(0),
    iLongitude(0),
    iAltError(0),
    iPowerupTime(0),
    iRandomMode(EFalse),
    iTTFMin(0),
    iTTFMax(0),
    iFailFrequency(0),
    iRandomTTF(0),
    iSeed(0)
    {
    TTime time;
    time.UniversalTime();
    iSeed = time.Int64();
    }

// ---------------------------------------------------------
// TPosDataSimulator::ComputeNewPositionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosDataSimulator::ComputeNewPositionL(
    TPosition& aPosition,
    const TTime& aRequestStartTime,
    const TTime& aLastRequestTime)
    {
    if (iSpeed != 0)
        {
        TReal cHdng1;
        TReal cLat1;
        TReal sAlpha;
        TReal sLat1;
        TReal cAlpha;

        TReal latitude = aPosition.Latitude() - iLatErrorRadius;
        TReal delay = (static_cast<TReal>
            (aRequestStartTime.MicroSecondsFrom(aLastRequestTime).
            Int64()))/KToSeconds;
        if (delay == 0)
            {
            delay = 1;
            }
        TReal alpha = (iSpeed*delay)/KEarthRadius;

        // Compute new latitude
        User::LeaveIfError(Math::Cos(cHdng1, iTrueCourse * KToRadians));
        User::LeaveIfError(Math::Cos(cLat1,  latitude * KToRadians));
        User::LeaveIfError(Math::Sin(sAlpha, alpha));
        User::LeaveIfError(Math::Sin(sLat1,  latitude * KToRadians));
        User::LeaveIfError(Math::Cos(cAlpha, alpha));

        TReal sLat2 = cHdng1 * cLat1 * sAlpha + sLat1 * cAlpha;
        TReal lat2;
        User::LeaveIfError(Math::ASin(lat2, sLat2));

        iLatitude = lat2 * KToDegrees;

        // Compute new longitude
        TReal cLat2;
        User::LeaveIfError(Math::Cos(cLat2, lat2));
        TReal tmp =(cAlpha - sLat1 * sLat2) / (cLat1 * cLat2);
        TReal newLong;
        ModifyCosArgIfNeeded(tmp);
        User::LeaveIfError(Math::ACos(newLong, tmp));

        newLong *= KToDegrees;

        // newLong will always be positive. If the course indicates that
        // longitude should be decreased (sin(course) < 0), the sign should
        // be changed.
        TReal sCourse;
        User::LeaveIfError(Math::Sin(sCourse, iTrueCourse * KToRadians));
        if (sCourse < 0)
            {
            newLong = -newLong;
            }

        iLongitude = aPosition.Longitude() + newLong;
        LongitudeBoundsCheck(iLongitude);

        ComputeNewErrorForLatLonAltL(iLatitude, iLongitude,
            iLatitude, iLongitude);

        // Compute new course
        TReal newCourse;
        tmp = (sLat1 - sLat2*cAlpha) / (cLat2 * sAlpha);
        ModifyCosArgIfNeeded(tmp);
        User::LeaveIfError(Math::ACos(newCourse, tmp));
        newCourse *= KToDegrees;
        }
    else
        {
        ComputeNewErrorForLatLonAltL(iOrigLatitude, iOrigLongitude,
            iLatitude, iLongitude);
        }

    // Set position
    aPosition.SetCoordinate(iLatitude, iLongitude, iAltError);
    aPosition.SetAccuracy(iHAcc, iVAcc);

    return KErrNone;
    }

// ---------------------------------------------------------
// TPosDataSimulator::ParseAndCreatePosObjectL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosDataSimulator::ParseAndCreatePosObjectL(
    TPosition& aPosition,
    const CDesC8Array* aDataArray)
    {
    TInt itemsFound = 0;
    for (TInt i = 0; i < aDataArray->Count(); i++)
        {
        TDesTokeniser tokeniser(aDataArray->MdcaPoint(i));
        TPtrC8 field = tokeniser.NextToken(KPosDataSeparator);
        TPtrC8 value = tokeniser.NextToken(KPosDefaultDataEndMarker);
        
        // trim any trailing spaces off the field:
        const TText8* first = field.Ptr();
        const TText8* last = first + field.Length() - 1;

        while ((last >= first) && TChar(*last).IsSpace() )
        	{
        	last--;
        	}

        TUint8 len = (last + 1 - first);
        TPtrC8  fieldTrimmed(first, len);   
        
        if (fieldTrimmed.MatchF(KHacc) == 0)
            { // {float} 0 -
            LeaveIfNegativeRealL(value);
            iHAcc = DesToFloatL(value);
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KVacc) == 0)
            { // {float} 0 -
            LeaveIfNegativeRealL(value);
            iVAcc = DesToFloatL(value);
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KTimeToFixMin) == 0)
            { // {integer} 0 -
            LeaveIfNegativeIntL(value);
            iTTFMin = DesToIntL(value);
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KTimeToFixMax) == 0)
            { // {integer} 0 -
            LeaveIfNegativeIntL(value);
            iTTFMax = DesToIntL(value);
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KPowerupTime) == 0)
            { // {integer} 0 -
            LeaveIfNegativeIntL(value);
            iPowerupTime = DesToIntL(value);
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KLongitude) == 0)
            { // {float} - 180 - 180
            CheckLatLonBoundariesL(value, TReal(180));
            iOrigLongitude = DesToRealL(value);
            LongitudeBoundsCheck(iOrigLongitude);
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KLatitude) == 0)
            { // {float} - 90 - 90
            CheckLatLonBoundariesL(value, TReal(90));
            iOrigLatitude = DesToRealL(value);
            if (iOrigLatitude == 90 || iOrigLatitude == -90)
                {
                iOrigLatitude = 89.9999;
                }
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KSpeed) == 0)
            { // {float} 0 -
            LeaveIfNegativeRealL(value);
            // Speed NOT used in current supported classes
            iSpeed = DesToFloatL(value);
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KCourse) == 0)
            { // {float} 0 - 360
            // TrueCourse NOT used in current supported classes
            CheckCourseBoundariesL(value);
            iTrueCourse = DesToFloatL(value);
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KRandom) == 0)
            { // {integer} 0 -
            LeaveIfNegativeIntL(value);
            iFailFrequency = DesToIntL(value);
            iRandomMode = ETrue;
            itemsFound++;
            }
        else if (fieldTrimmed.MatchF(KDeterministic) == 0)
            { // {integer} 0 -
            LeaveIfNegativeIntL(value);
            iFailFrequency = DesToIntL(value);
            iRandomMode = EFalse;
            itemsFound++;
            }
        }

    if (itemsFound != KNoOfSimulatedDataItems)
        {
        User::Leave(KErrCorrupt);
        }

    aPosition.SetCoordinate(iOrigLatitude, iOrigLongitude, iAltError);
    aPosition.SetAccuracy(iHAcc, iVAcc);
    }

// ---------------------------------------------------------
// TPosDataSimulator::TimeToWait
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds TPosDataSimulator::TimeToWait(
    const TTime& aRequestStartTime)
    {
    // 500 ms as execution time after wait.
    //
    TInt executionTimeAfterWait = 500000;
    TInt timetowait = iTTFMin;

    iRandomTTF = (iTTFMin + (Math::Rand(iSeed) % (iTTFMax-iTTFMin+1)))
        * KToMicroSeconds;
    TTime now;
    now.UniversalTime();
    TInt delay = I64INT(now.MicroSecondsFrom(aRequestStartTime).Int64());
    if (delay < iRandomTTF)
        {
        timetowait = Min(iTTFMax * KToMicroSeconds - executionTimeAfterWait,
                        Max(iTTFMin * KToMicroSeconds + executionTimeAfterWait,
                            iRandomTTF - delay));
        }
    else
        {
        timetowait = Max(iTTFMin * KToMicroSeconds + executionTimeAfterWait,
            iRandomTTF - delay);
        }
    return TTimeIntervalMicroSeconds(TInt64(timetowait));
    }

// ---------------------------------------------------------
// TPosDataSimulator::FailThisRequest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosDataSimulator::FailThisRequest(TInt aNumberOfRequests)
    {
    TInt err = KErrNone;
    if (iFailFrequency != 0)
        {
        if (iRandomMode)
            {
            TInt randomNum = Math::Rand(iSeed);
            TInt res = KMaxTInt/iFailFrequency;
            if (randomNum < res)
                {
                err = KErrorCodeForFailedRequest;
                }
            }
        else
            {
            if (aNumberOfRequests % iFailFrequency == 0)
                {
                err = KErrorCodeForFailedRequest;
                }
            }
        }
    return err;
    }

// ---------------------------------------------------------
// TPosDataSimulator::PowerupTime
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds TPosDataSimulator::PowerupTime()
    {
    TInt powerUp = iPowerupTime * KToMicroSeconds;
    return TTimeIntervalMicroSeconds(TInt64(powerUp));
    }

// ---------------------------------------------------------
// TPosDataSimulator::GetRandomError
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TReal TPosDataSimulator::GetRandomError()
    {
    TReal error = 0;
    for (TInt j = 0; j < KTimesToRandom; j++)
        {
        error += Math::FRand(iSeed);
        }
    error -= KTimesToRandom/2;
    return error;
    }

// ---------------------------------------------------------
// TPosDataSimulator::ComputeNewErrorForLatLonAltL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosDataSimulator::ComputeNewErrorForLatLonAltL(
    TReal& aFromLatitude,
    TReal& aFromLongitude,
    TReal& aToLatitude,
    TReal& aToLongitude)
    {
    // Add random error to lon and lat
    // Latitude
    TReal error = GetRandomError();
    error *= iHAcc;
    iLatErrorRadius = (error * 360) / (2 * KEarthRadius * KPi);

    // Longitude
    error = GetRandomError();
    error *= iHAcc;
    iLonErrorRadius = (error * 360) / (2 * KEarthRadius * KPi);

    // Angle
    TReal rand = Math::FRand(iSeed);
    iErrorAngle = rand * 360;

    TReal cosAlpha;
    User::LeaveIfError(Math::Cos(cosAlpha, iErrorAngle * KToRadians));
    TReal sinAlpha;
    User::LeaveIfError(Math::Sin(sinAlpha, iErrorAngle * KToRadians));

    // Add error to latitude
    aToLatitude = aFromLatitude + iLatErrorRadius * sinAlpha;
    // Add error to longitude
    aToLongitude = aFromLongitude + iLonErrorRadius * cosAlpha;

    // Add random error to altitude
    iAltError = (TReal32) GetRandomError();
    iAltError *= iVAcc;
    }

// ---------------------------------------------------------
// TPosDataSimulator::DesToIntL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosDataSimulator::DesToIntL(const TDesC8& aDes)
    {
    TLex8   lexer(aDes);
    TInt    integer;
    User::LeaveIfError(lexer.Val(integer));
    return integer;
    }

// ---------------------------------------------------------
// TPosDataSimulator::DesToFloatL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TReal32 TPosDataSimulator::DesToFloatL(const TDesC8& aDes)
    {
    TLex8       lexer(aDes);
    TReal32     floatNumber;
    User::LeaveIfError(lexer.Val(floatNumber, TChar('.')));
    return floatNumber;
    }

// ---------------------------------------------------------
// TPosDataSimulator::DesToRealL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TReal TPosDataSimulator::DesToRealL(const TDesC8& aDes)
    {
    TLex8 lexer(aDes);
    TReal realNumber;
    User::LeaveIfError(lexer.Val(realNumber, TChar('.')));
    return realNumber;
    }

// ---------------------------------------------------------
// TPosDataSimulator::LeaveIfNegativeIntL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosDataSimulator::LeaveIfNegativeIntL(const TDesC8& aDes)
    {
    TLex8 lexer(aDes);
    TInt  number;
    User::LeaveIfError(lexer.Val(number));
    if (number < 0)
        {
        User::Leave(KErrCorrupt);
        }
    CheckEosL(lexer);
    }

// ---------------------------------------------------------
// TPosDataSimulator::LeaveIfNegativeRealL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosDataSimulator::LeaveIfNegativeRealL(const TDesC8& aDes)
    {
    TLex8 lexer(aDes);
    TReal  number;
    User::LeaveIfError(lexer.Val(number, TChar('.')));
    if (number < 0)
        {
        User::Leave(KErrCorrupt);
        }
    CheckEosL(lexer);
    }

// ---------------------------------------------------------
// TPosDataSimulator::CheckLatLonBoundariesL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosDataSimulator::CheckLatLonBoundariesL(
    const TDesC8& aDes, TReal aMinMaxValue)
    {
    TLex8 lexer(aDes);
    TReal lat;
    User::LeaveIfError(lexer.Val(lat, TChar('.')));
    if (lat < -aMinMaxValue || lat > aMinMaxValue)
        {
        User::Leave(KErrCorrupt);
        }
    CheckEosL(lexer);
    }

// ---------------------------------------------------------
// TPosDataSimulator::CheckEosL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosDataSimulator::CheckEosL(TLex8& aLexer)
    {
    aLexer.SkipSpace();
    if (!aLexer.Eos())
        {
        User::Leave(KErrCorrupt);
        }
    }

// ---------------------------------------------------------
// TPosDataSimulator::CheckCourseBoundariesL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosDataSimulator::CheckCourseBoundariesL(
    const TDesC8& aDes)
    {
    TLex8 lexer(aDes);
    TReal32  course;
    User::LeaveIfError(lexer.Val(course, TChar('.')));
    if (course < 0 || course > 360)
        {
        User::Leave(KErrCorrupt);
        }
    CheckEosL(lexer);
    }

// ---------------------------------------------------------
// TPosDataSimulator::ModifyCosArgIfNeeded
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosDataSimulator::ModifyCosArgIfNeeded(TReal& aCosArg)
    {
    if (aCosArg > 1.0)
        {
        aCosArg = 1.0;
        }
    else if (aCosArg < -1.0)
        {
        aCosArg = -1.0;
        }
    }

// ---------------------------------------------------------
// TPosDataSimulator::LongitudeBoundsCheck
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosDataSimulator::LongitudeBoundsCheck(TReal& aLong)
    {
    // Make sure -180.0 < aLong =< 180.0
    while (aLong < -KLongitudeUpperBound)
        {
        aLong += KFullCircle;
        }
    while (aLong >= KLongitudeUpperBound)
        {
        aLong -= KFullCircle;
        }
    }

//  End of File
