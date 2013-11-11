// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include "tlbscalculationcheck.h"
#include <e32math.h>


//CONSTANTS
const TReal KAccuracyHigh = 5.96E-8;
const TReal KAccuracyMedium = 4E-5;
const TReal KAccuracyLow = 4E-3;


// C++ default constructor 
T_LbsCalculationCheck::T_LbsCalculationCheck() :
    iErrorCode(KErrArgument),
    iBearingErrorCode(KErrArgument),
    iBearingDeltaErrorCode(KErrArgument),
    iSpeedErrorCode(KErrArgument),
    iMoveErrorCode(KErrArgument)
    {
    TRealX nan;
    nan.SetNaN();
    iDistance=nan;
    }

T_LbsCalculationCheck::T_LbsCalculationCheck(const TPosition& aSourcePosition, const TPosition& aTargetPosition) :
    iSourcePosition(aSourcePosition),
    iTargetPosition(aTargetPosition),
    iErrorCode(KErrArgument),
    iBearingErrorCode(KErrArgument),
    iBearingDeltaErrorCode(KErrArgument),
    iSpeedErrorCode(KErrArgument),
    iMoveErrorCode(KErrNone)
    {
   
    }

// C++ Constructor TDesC
T_LbsCalculationCheck::T_LbsCalculationCheck(const TDesC& aDes)
    {
    TReal lat1;
    TReal long1;
    TReal lat2;
    TReal long2;
    TReal32 err1=0;
    TReal32 err2=0;
    TReal time1=0;
    TReal time2=0;
  
    TLex line(aDes);
    line.Mark();
    TInt index=0;
    while (line.Peek() != '\n' && !(line.Peek()).Eos())
        {
        line.Inc();
        if (line.Peek() == '\t')
            {
            TPtrC token = line.MarkedToken();
            TLex val(token);              
            switch(index)
                {
                case ELat1Index:
                    val.Val(lat1, TChar(','));  
                    break;
                case ELong1Index:
                    val.Val(long1, TChar(','));
                    break;
                case ELat2Index:
                    val.Val(lat2, TChar(','));
                    break;
                case ELong2Index:
                    val.Val(long2, TChar(','));
                    break;
                case EErr1Index:
                    val.Val(err1, TChar(','));
                    break;
                case EErr2Index:
                    val.Val(err2, TChar(','));
                    break;
                case ETime1Index:
                    val.Val(time1, TChar(','));
                    break;
                case ETime2Index:
                    val.Val(time2, TChar(','));
                    break;
                case EDistanceIndex:
                    val.Val(iDistance, TChar(','));
                    break;
                case EErrorCodeIndex:
                    val.Val(iErrorCode);
                    break;
                case EDeltaIndex:
                    val.Val(iDelta, TChar(','));
                    break;
                case EBearingIndex:
                    val.Val(iBearing, TChar(','));
                    if (iBearing == 360.0)
                        iBearing = 0.0; 
                    break;
                case EBearingErrorCodeIndex:
                    val.Val(iBearingErrorCode);
                    break;
                case EBearingDelta:
                    val.Val(iBearingDelta, TChar(','));
                    break;
                case EDeltaErrorCode:
                    val.Val(iBearingDeltaErrorCode);
                    break;
                case ESpeedIndex:
                    val.Val(iSpeed, TChar(','));
                    break;
                case ESpeedErrorCodeIndex:
                    val.Val(iSpeedErrorCode);
                    break;
                case ESpeedDeltaIndex:
                    val.Val(iSpeedDelta, TChar(','));
                    break;
                case EMoveErrorCodeIndex:
                    val.Val(iMoveErrorCode);
                    break;
                }    
            line.Inc();
            line.Mark();
            ++index;
            }
        }
    iSourceLocality.SetCoordinate(lat1, long1, 5.0);
    iTargetLocality.SetCoordinate(lat2, long2, 100.0);
    
    iSourceLocality.SetHorizontalAccuracy(err1);
    iTargetLocality.SetHorizontalAccuracy(err2);

    iSourcePosition.SetCoordinate(lat1, long1, 5.0);
    iTargetPosition.SetCoordinate(lat2, long2, 100.0);
    iSourcePosition.SetHorizontalAccuracy(err1);
    iTargetPosition.SetHorizontalAccuracy(err2);

    TTime ttime1;
    TTime ttime2;
    
    ttime1.UniversalTime();
    ttime2.UniversalTime(); 
    
    if ((time1 < 1.0 || time2 < 1.0) && (time1 > -1.0 && time2 > -1.0))
        {
        time1 *= 1000000;    
        time2 *= 1000000;
        ttime1 += TTimeIntervalMicroSeconds((TInt)time1);
        ttime2 += TTimeIntervalMicroSeconds((TInt)time2);
        }
    else
        {
        
        ttime1 += TTimeIntervalSeconds((TInt)time1);
        ttime2 += TTimeIntervalSeconds((TInt)time2);
        }
   
    iSourcePosition.SetTime(ttime1);
    iTargetPosition.SetTime(ttime2);   
    }   

TInt T_LbsCalculationCheck::CheckDistance()
    {
    TInt err=0;
    TReal32 distance;
    err = iSourceLocality.Distance(iTargetLocality, distance);
    
    if (err != iErrorCode)
        {
        iReturnedErrorCode = err;
        return KErrCalculationCheckErrorCode;
        }
    
    if(!err)
        {
        if (!Equals32(distance, iDistance, KAccuracyHigh))
            {
            iCalculated=distance;
            return KErrCalculationCheckDistance;
            }
        }
    return KErrNone;
    }

TInt T_LbsCalculationCheck::CheckBearing()
    {
    TInt err=0;
    TReal32 bearing;
  
    err = iSourceLocality.BearingTo(iTargetLocality, bearing);
    
    if (err != iBearingErrorCode)
        {
        iReturnedErrorCode = err;
        return KErrCalculationCheckErrorCode;
        }
    
    if(!err)
        {
        TReal accuracy;
        if (bearing < 1 )
            {    
            accuracy=KAccuracyLow;
            }
        else
            {
            accuracy=KAccuracyHigh;
            }

        if (!Equals32(bearing, iBearing, accuracy))
            {
            iCalculated=bearing;
            
            return KErrCalculationCheckBearing;
            }
        }

    return KErrNone;
    }
    
TInt T_LbsCalculationCheck::CheckDistanceCoordinate()
    {
    TInt err=0;
    TReal32 distance;
    
    TCoordinate source(iSourceLocality.Latitude(), iSourceLocality.Longitude());
    TCoordinate target(iTargetLocality.Latitude(), iTargetLocality.Longitude());

    err=source.Distance(target, distance);
    
    if (err != iErrorCode)
        {
        iReturnedErrorCode = err;
        return KErrCalculationCheckErrorCode;
        }
    if(!err)
        { 
        if (!Equals32(distance, iDistance, KAccuracyHigh))
            {
            iCalculated=distance;
            return KErrCalculationCheckDistance;
            }
        }
    return KErrNone;
    }

TInt T_LbsCalculationCheck::CheckBearingCoordinate()
    {
    TReal32 bearing;
    
    TCoordinate source(iSourceLocality.Latitude(), iSourceLocality.Longitude());
    TCoordinate target(iTargetLocality.Latitude(), iTargetLocality.Longitude());

    TInt err=0;
    err=source.BearingTo(target, bearing);
    
    if (err != iBearingErrorCode)
        {
        iReturnedErrorCode = err;
        return KErrCalculationCheckErrorCode;
        }
    if(!err)
        {
        TReal accuracy;
        if (bearing < 1 )
            {    
            accuracy=KAccuracyLow;
            }
        else
            {
            accuracy=KAccuracyHigh;
            }
        if (!Equals32(bearing, iBearing, accuracy))
            {
            iCalculated=bearing;
            return KErrCalculationCheckBearing;
            }
        }
    return KErrNone;
    }

TInt T_LbsCalculationCheck::CheckDistanceDelta()
    {
    TInt err=0;
    TReal32 distance;
    TReal32 delta;

    err = iSourceLocality.Distance(iTargetLocality, distance, delta);
    
    if (err != iErrorCode)
        {
        iReturnedErrorCode = err;
        return KErrCalculationCheckErrorCode;
        }
    if(!err)
        { 
        if (!Equals32(distance, iDistance, KAccuracyHigh) )
            {
            iCalculated=distance;
            return KErrCalculationCheckDistance;
            }

        if (!Equals32(delta, iDelta, KAccuracyLow))
            {
            iCalculated=delta;
            return KErrCalculationCheckDistanceDelta;
            }
        }

    return KErrNone;
    }

TInt T_LbsCalculationCheck::CheckBearingDelta()
    {
    TReal32 bearing;
    TReal32 delta;

    TInt err=0;
    err = iSourceLocality.BearingTo(iTargetLocality, bearing, delta);
    
    if (err != iBearingDeltaErrorCode)
        {
        iReturnedErrorCode = err;
        return KErrCalculationCheckErrorCode;
        }
    if(!err)
        {
        TReal accuracy;
        if (bearing < 1 )
            {    
            accuracy=KAccuracyLow;
            }
        else
            {
            accuracy=KAccuracyHigh;
            }

        if (!Equals32(bearing, iBearing, accuracy) )
            {
            iCalculated=bearing;
            return KErrCalculationCheckDistance;
            }

        if (!Equals32(delta, iBearingDelta, KAccuracyHigh))
            {
            iCalculated=delta;
            return KErrCalculationCheckBearingDelta;
            }
        }

    return KErrNone;
    }

TInt T_LbsCalculationCheck::CheckSpeed()
    {
    TInt err=0;
    TReal32 speed;

    err = iSourcePosition.Speed(iTargetPosition, speed);
    
    if (err != iSpeedErrorCode)
        {
        iReturnedErrorCode = err;
        return KErrCalculationCheckErrorCode;
        }
    
    if(!err)
        {
        if (!Equals32(speed, iSpeed, KAccuracyHigh))
            {
            iCalculated=speed;
            return KErrCalculationCheckSpeed;
            }
        }
    return KErrNone;    
    }

TInt T_LbsCalculationCheck::CheckSpeedDelta()
    {
    TInt err=0;
    TReal32 speed;
    TReal32 delta;

    err = iSourcePosition.Speed(iTargetPosition, speed, delta);
    
    if (err != iSpeedErrorCode)
        {
        iReturnedErrorCode = err;
        return KErrCalculationCheckErrorCode;
        }
    
    if(!err)
        {
        if (!Equals32(speed, iSpeed, KAccuracyHigh))
            {
            iCalculated=speed;
            return KErrCalculationCheckSpeed;
            }

         if (!Equals32(delta, iSpeedDelta, KAccuracyHigh))
            {
            iCalculated=delta;
            return KErrCalculationCheckSpeedDelta;
            }
        }

    return KErrNone;    
    }

TInt T_LbsCalculationCheck::CheckMove()
    {
    TInt err=0;
  
    TCoordinate source(iSourcePosition.Latitude(), iSourcePosition.Longitude());
    
    err=source.Move(iBearing, iDistance);
    
    if (err != iMoveErrorCode)
        {
        iReturnedErrorCode = err;
        return KErrCalculationCheckErrorCode;
        }
    
    if(!err)
        {
        if (iTargetPosition.Latitude() == 90.0 || iTargetPosition.Latitude() == -90.0)
            {
            if (!Equals32(source.Latitude(), iTargetPosition.Latitude(), KAccuracyMedium))
                {
                iCalcCoordinate=source;
                return KErrCalculationCheckMove;
                }
            }
        else
            {
            if (!Equals32(source.Latitude(), iTargetPosition.Latitude(), KAccuracyMedium) ||
                !Equals32(source.Longitude(), iTargetPosition.Longitude(), KAccuracyMedium))
                {
                iCalcCoordinate=source;
                return KErrCalculationCheckMove;
                }
            }
        }

    return KErrNone;    
    }

TBool T_LbsCalculationCheck::Equals32(const TReal aValue1, const TReal aValue2, const TReal aAccuracy)
    {
    TReal maxAllowedError = aAccuracy;
    
    TReal maxValue;
    aValue1 > aValue2 ? maxValue=aValue1 : maxValue=aValue2;

    while (maxValue >= 1.0)
        {
        maxValue /= 10.0;
        maxAllowedError *= 10.0;
        }

    TReal absError = Abs(aValue1-aValue2);
    
    return (absError < maxAllowedError); 

    }

void T_LbsCalculationCheck::GetSourcePosition(TPosition& aSourcePosition) const
    {
    aSourcePosition=iSourcePosition;
    }

void T_LbsCalculationCheck::GetTargetPosition(TPosition& aTargetPosition) const 
    {
    aTargetPosition=iTargetPosition;
    }

void T_LbsCalculationCheck::GetCalcCoordinate(TCoordinate& aCoordinate) const
    {
    aCoordinate=iCalcCoordinate;
    }
    
TReal T_LbsCalculationCheck::Distance() const
    {
    return iDistance;
    }

TReal T_LbsCalculationCheck::Calculated() const
    {
    return iCalculated;
    }
   
TReal T_LbsCalculationCheck::Delta() const
    {
    return iDelta;
    }

TReal T_LbsCalculationCheck::BearingDelta() const
    {
    return iBearingDelta;
    }

TInt T_LbsCalculationCheck::ErrorCode() const
    {
    return iErrorCode;
    }

TInt T_LbsCalculationCheck::BearingErrorCode() const
    {
    return iBearingErrorCode;
    }

TInt T_LbsCalculationCheck::SpeedErrorCode() const
    {
    return iSpeedErrorCode;
    }

void T_LbsCalculationCheck::SetErrorCode(TInt aErrorCode)
    {
    iErrorCode=aErrorCode;
    }

void T_LbsCalculationCheck::SetBearingErrorCode(TInt aErrorCode)
    {
    iBearingErrorCode=aErrorCode;
    }

void T_LbsCalculationCheck::SetBearingDeltaErrorCode(TInt aErrorCode)
    {
    iBearingDeltaErrorCode=aErrorCode;
    }

void T_LbsCalculationCheck::SetSpeedErrorCode(TInt aErrorCode)
    {
    iSpeedErrorCode=aErrorCode;
    }

void T_LbsCalculationCheck::SetMoveErrorCode(TInt aErrorCode)
    {
    iMoveErrorCode=aErrorCode;
    }

TInt T_LbsCalculationCheck::ReturnedErrorCode() const
    {
    return iReturnedErrorCode;
    }

TReal T_LbsCalculationCheck::Bearing() const
    {
    return iBearing;
    }

TReal T_LbsCalculationCheck::Speed() const
    {
    return iSpeed;
    }

TReal T_LbsCalculationCheck::SpeedDelta() const
    {
    return iSpeedDelta;
    }

//  End of File
