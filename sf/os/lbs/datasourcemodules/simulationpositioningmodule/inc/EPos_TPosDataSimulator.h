/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef TPOSDATASIMULATOR_H
#define TPOSDATASIMULATOR_H

// INCLUDES
#include <badesca.h>

// FORWARD DECLARATIONS
class TDesTokeniser;
class TPositionInfo;

// CLASS DECLARATION

/**
 * A helper class for simulation of position info.
 */
class TPosDataSimulator
    {

    public:  // Constructor

        /**
        * C++ default constructor.
        */
        TPosDataSimulator();

    public:  // New functions

        /**
        * Computes a new position..
        *
        * @param aPosition reference to a position pointer.
        * @param aRequestStartTime the request start-time
        * @param aLastRequestTime the last request time
        *
        * @return KErrNone if the new position computed successfully,
        *         otherwise another of the system-wide error codes.
        */
        TInt ComputeNewPositionL(
        /* OUT    */    TPosition&  aPosition,
        /* IN     */    const TTime& aRequestStartTime,
        /* IN     */    const TTime& aLastRequestTime
        );

        /**
        * Parses and creates a position object.
        *
        * @param aPosition reference to a position pointer. This
        *                  pointer must be in scope until the request
        *                  has completed.
        * @param aDataArray an array containing simulation data
        *
        */
        void ParseAndCreatePosObjectL(
        /* OUT */   TPosition&  aPosition,
        /* IN  */   const CDesC8Array* aDataArray
        );

        /**
        * Computes a delay based on the request start time, min TTF and max TTF.
        *
        * @param aRequstStartTime the request start time.
        *
        * @return time to wait in microseconds.
        */
        TTimeIntervalMicroSeconds TimeToWait(
        /* IN */ const TTime& aRequestStartTime
        );

        /**
        * Checks if the request should fail or not.
        *
        * @param aNumberOfRequests the number of requests received so far.
        * @return KErrNone if function executed successfully,
        * KErrGeneral otherwise.
        */
        TInt FailThisRequest(
        /* IN */ TInt aNumberOfRequests
        );

        /**
        * Computes the power-up time.
        * @return TTimeIntervalMicroSeconds The powerup time.
        */
        TTimeIntervalMicroSeconds PowerupTime();

    private: // New functions

        /**
         * Calculates a random error.
         *
         * @return the calculated random error
         */
        TReal GetRandomError();

        /**
         * Adds random error to latitude, longitude, and altitude.
         */
        void ComputeNewErrorForLatLonAltL(
        /* IN */   TReal& aFromLatitude,
        /* IN */   TReal& aFromLongitude,
        /* OUT */   TReal& aToLatitude,
        /* OUT */   TReal& aToLongitude
        );

        /**
         * Converts a descriptor to an int.
         *
         * @param  the descriptor to convert
         * @return the TInt
         */
        TInt DesToIntL(const TDesC8& aDes);

        /**
         * Converts a descriptor to a real number.
         *
         * @param  aDes the descriptor to convert
         * @return the real number
         */
        TReal DesToRealL(const TDesC8& aDes);

        /**
         * Converts a descriptor to a real 32 bit number.
         *
         * @param  aDes the descriptor to convert
         * @return the real 32 bit number
         */
        TReal32 DesToFloatL(const TDesC8& aDes);

        /**
         * Checks if a int value is negative. If the value is
         * negative the function will leave.
         *
         * @param  aDes the descriptor to check
         */
        void LeaveIfNegativeIntL(const TDesC8& aDes);

        /**
         * Checks if a real value is negative. If the value is
         * negative the function will leave.
         *
         * @param  aDes the descriptor to check
         */
        void LeaveIfNegativeRealL(const TDesC8& aDes);

        /**
         * Checks whether the supplied latitude/longitude is correct.
         * The latitude should be between -aMinMaxValue and aMinMaxValue
         *
         * @param  aDes the descriptor to check
         */
        void CheckLatLonBoundariesL(const TDesC8& aDes, TReal aMinMaxValue);

        /**
         * Checks whether the supplied lexer is End of Stream or not.
         *
         * @param  aLexer the lexer to check
         */
        void CheckEosL(TLex8& aLexer);

        /**
         * Checks whether the supplied course is correct.
         * The latitude should be between 0 and 360
         *
         * @param  aDes the descriptor to check
         */
        void CheckCourseBoundariesL(const TDesC8& aDes);

        /**
         * Checks if the argument is a value between -1 and +1.
         *
         * @param aCosArg the argument of the arccos function
         */
        void ModifyCosArgIfNeeded(TReal& aCosArg);

        /**
         * Checks that the longitude is between 0 and 360.
         *
         * @param aLong The longitude to check
         */
        void LongitudeBoundsCheck(TReal& aLong);

    private:
        TReal32             iHAcc;
        TReal32             iVAcc;

        TReal               iLonErrorRadius;
        TReal               iLatErrorRadius;
        TReal               iErrorAngle;
        TReal32             iSpeed;
        TReal32             iTrueCourse;

        TReal               iOrigLatitude;
        TReal               iOrigLongitude;
        TReal               iLatitude;
        TReal               iLongitude;

        TReal32             iAltError;

        TInt                iPowerupTime;
        TBool               iRandomMode;
        TInt                iTTFMin;
        TInt                iTTFMax;
        TInt                iFailFrequency;
        TInt                iRandomTTF;
        TInt64              iSeed;
    };

#endif      // TPOSDATASIMULATOR_H

// End of File
