/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Tilt compensation algorithm interface.
*
*/


#ifndef TILTCOMPENSATION_H
#define TILTCOMPENSATION_H

#include <e32std.h>

// CONSTANT DEFINITIONS

typedef RArray<TReal> RParamsArray;

/** 16bit integer vector */
class TInt16vector
    {
public:
    TInt16 iX;
    TInt16 iY;
    TInt16 iZ;
    };

/**
* Tilt compensation input.
* Input contains current accelerometer and magnetometer vectors.
*
* @since S60 5.0
*/
class TTiltCompensationInput
    {
public:
    /** Normalized magnetic vector in 12Q4 format */
    TInt16vector iMagneticVector;

    /** Normalized acceleration vector 12Q4 format */
    TInt16vector iAccelerationVector;
    };

/**
* Tilt compensation output.
* Output contains compensated value from accelerometer and magnetometer
* data.
*
* @since S60 5.0
*/
class TTiltCompensationOutput
    {
public:
    /** Heading (deg). Compensated tilt angle. */
    TInt16 iTheta;

    /** Inclination (deg) */
    TInt16 iDelta;

    /** Roll (deg) */
    TInt16 iRoll;

    /** Pitch (deg) */
    TInt16 iPitch;
    };

/** Name of the library providing tilt compensation algorithms. */
_LIT( KTiltCompensationLib, "tiltcompensation.dll" );

/** Function ordinal definitions for the dynamically loading the library. */
enum TTiltCompensationFuncOrdinal
    {
    /**
    * Ordinal for compensation functions which takes no parameters other than
    * input vector, output vector and previous theta value.
    */
    ECompensateNoParams = 1,

    /**
    * Ordinal for compensation functions which takes additional parameters in
    * a parameter array.
    */
    ECompensateWithParams
    };

/** Function prototype for compensation function in ordinal ECompensateNoParams. */
typedef TInt ( *CompensateFuncNoParams )(
    const TTiltCompensationInput&,
    TTiltCompensationOutput&,
    const TInt16 );

/** Function prototype for compensation function in ordinal ECompensateWithParams. */
typedef TInt ( *CompensateFuncWithParams )(
    const TTiltCompensationInput&,
    TTiltCompensationOutput&,
    const TInt16,
    const RParamsArray& );

/**
* Creates a compensation from accelerometer vector and magnetometer vector.
*
* @lib tiltcompensation.lib
* @since S60 5.0
* @param aInput Input for the compensation.
* @param aOutput Output from the compensation.
* @param aPreviousTheta Theta value from the previous calculation.
* @return KErrNone if no errors. KErrGeneral if compensation error.
*/
IMPORT_C TInt Compensate(
    const TTiltCompensationInput& aInput,
    TTiltCompensationOutput& aOutput,
    const TInt16 aPreviousTheta );

/**
* Creates a compensation from accelerometer vector and magnetometer vector.
*
* @lib tiltcompensation.lib
* @since S60 5.0
* @param aInput Input for the compensation.
* @param aOutput Output from the compensation.
* @param aPreviousTheta Theta value from the previous calculation.
* @param aParamsArray array is used in inputing parameters to the
*        tiltcompensation library.
* @return KErrNone if no errors. KErrGeneral if compensation error.
*/
IMPORT_C TInt Compensate(
    const TTiltCompensationInput& aInput,
    TTiltCompensationOutput& aOutput,
    const TInt16 aPreviousTheta,
    const RParamsArray& aParamsArray );

#endif

// End of File
