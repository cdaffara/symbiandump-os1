/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/




#ifndef __T_LBS_CALCULATIONCHECK_H__
#define __T_LBS_CALCULATIONCHECK_H__

//  INCLUDES
#include "tlbscalculationcheck.h"
#include <lbsposition.h>

// CONSTANTS
const TInt KErrCalculationCheckErrorCode= -100;
const TInt KErrCalculationCheckDistance= -101;
const TInt KErrCalculationCheckDistanceDelta= -102;

const TInt KErrCalculationCheckBearing=-103;
const TInt KErrCalculationCheckBearingDelta=-104;
const TInt KErrCalculationCheckSpeed=-105;
const TInt KErrCalculationCheckSpeedDelta=-106;
const TInt KErrCalculationCheckMove=-107;


// CLASS DECLARATION
class T_LbsCalculationCheck 
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        T_LbsCalculationCheck();
        
        T_LbsCalculationCheck(const TDesC& aDes);

        T_LbsCalculationCheck(const TPosition& aSourcePosition, const TPosition& aTargetPosition);

        TInt CheckDistance();
        
        TInt CheckDistanceCoordinate();

        TInt CheckDistanceDelta();

        TInt CheckBearing();

        TInt CheckBearingCoordinate();

        TInt CheckBearingDelta();

        TInt CheckSpeed();

        TInt CheckSpeedDelta();

        TInt CheckMove();

        void GetSourcePosition(TPosition& aSourcePosition) const;
        
        void GetTargetPosition(TPosition& aTargetPosition) const;

        void GetCalcCoordinate(TCoordinate& aCoordinate) const;

        TReal Distance() const;
        
        TReal Calculated() const;

        TReal Delta() const;

        TReal BearingDelta() const;

        TReal CalcDelta() const;

        TInt ErrorCode() const;
        
        TInt BearingErrorCode() const;

        TInt SpeedErrorCode() const;

        void SetErrorCode(TInt aErrorCode);

        void SetBearingErrorCode(TInt aErrorCode);

        void SetBearingDeltaErrorCode(TInt aErrorCode);

        void SetSpeedErrorCode(TInt aErrorCode);

        void SetMoveErrorCode(TInt aErrorCode);
        
        TInt ReturnedErrorCode() const;

        TReal Bearing() const;

        TReal Speed() const;

        TReal SpeedDelta() const;
        
    private:

        TBool Equals32(const TReal aValue1, const TReal aValue2, const TReal aAccuracy);
    
    private:    // Data

        enum TValueIndex
			{
            ELat1Index              =0,	
            ELong1Index             =1,	
            ELat2Index              =2,
            ELong2Index             =3,	
            EErr1Index              =4,	
            EErr2Index              =5,
            ETime1Index             =6,
            ETime2Index             =7,
            EDistanceIndex          =10,	
            EErrorFactorIndex       =11,	
            EDeltaIndex             =12,	
            EErrorCodeIndex         =13,
            EBearingIndex           =16,
            EBearingErrorCodeIndex  =17,
            EBearingDelta           =19,
            EDeltaErrorCode         =20,
            ESpeedIndex             =21,
            ESpeedErrorCodeIndex    =22,
            ESpeedDeltaIndex        =23,
            EMoveErrorCodeIndex     =25
			};
        
        TLocality           iSourceLocality;
        TLocality           iTargetLocality;
        
        TPosition           iSourcePosition;
        TPosition           iTargetPosition;
        
        // Distance
        TReal32             iDistance;
        TReal               iCalculated;
        TReal               iDelta;
  
        TInt                iErrorCode;
        TInt                iReturnedErrorCode;

        // Bearing
        TReal32             iBearing;
        TReal               iBearingDelta;

        TInt                iBearingErrorCode;
        TInt                iBearingDeltaErrorCode;    
        
        // Speed
        TReal               iSpeed;
        TReal               iSpeedDelta;
        TInt                iSpeedErrorCode;
        // Move
        TInt                iMoveErrorCode;
        TCoordinate         iCalcCoordinate;    
    };

#endif // __T_LBS_CALCULATIONCHECK_H__
            
// End of File
