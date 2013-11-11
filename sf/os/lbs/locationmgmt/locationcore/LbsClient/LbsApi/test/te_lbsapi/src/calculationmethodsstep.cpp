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
// Example CTestStep derived implementation
// 
//

/**
 @file CalculationMethodsStep.cpp
 @internalTechnology
*/
#include "calculationmethodsstep.h"
#include "te_lbsapisuitedefs.h"

#include "lcfsbucommondefinitions.h"

#include <lbs.h>
#include <lbssatellite.h>

CCalculationMethodsStep::~CCalculationMethodsStep()
/**
 * Destructor
 */
	{
	}

CCalculationMethodsStep::CCalculationMethodsStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KCalculationMethodsStep);
	}

TVerdict CCalculationMethodsStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsApiSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
		return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CCalculationMethodsStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TInt test;
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsApiSuiteInt,test)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		SetTestStepResult(EPass);
		StartL(test);
		}
	  return TestStepResult();
	}



TVerdict CCalculationMethodsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_LbsApiVariant2SuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}


void CCalculationMethodsStep::StartL(TInt aIndex)
	{
	switch(aIndex)
		{
		case 0:
			Calculation_InvalidL();
			break;
		case 1:
			Calculation_BasicsL();
			break;
		case 2:
			Calculation_TCoordinate_MoveBigL();
			break;
		}
	}


#define REPORT_REAL(_real) \
    report.Format(_L("%15.9f"), _real); \
    LOG_DES(report);

#define REPORT_REAL_16(_real) \
    report.Format(_L("%22.16f"), _real); \
    LOG_DES(report);

#define REPORT_REAL_G(_real) \
    report.Format(_L("%10g"), _real); \
    LOG_DES(report);

#define REPORT_CRLF \
    report.Format(_L("\n")); \
    LOG_DES(report);

#define REPORT_TAB \
    report.Format(_L("\t")); \
    LOG_DES(report);

//
//
// TCoordinate
//
//

void CCalculationMethodsStep::Calculation_BasicsL()
    {
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TRealX nan;
    nan.SetNaN();

    const TReal tlat = 10, tlon = 20, acc1 = 5, acc2 = 15;
    TReal32 distance, bearing, delta, speed;

    TCoordinate sCoord, tCoord, zero(0,0), target(tlat, tlon);
    TLocality sLoc, tLoc;
    TPosition sPos, tPos;

    //
    // 1. TCoordinate
    
    sCoord.SetCoordinate(0, 0);
    tCoord.SetCoordinate(tlat, tlon);
    
    CHECK(sCoord.Distance(tCoord, distance) == KErrNone);
    CHECK(sCoord.Distance(sCoord, distance) == KErrNone);
    CHECK(sCoord.BearingTo(tCoord, bearing) == KErrNone);
    CHECK(sCoord.Move(distance, bearing) == KErrNone);
    CHECK(sCoord.Move(distance, bearing) == KErrNone);

    //
    // 2. TLocality
    
    sLoc = TLocality(zero, acc1);
    tLoc = TLocality(target, acc2);

    CHECK(sLoc.Distance(tCoord, distance) == KErrNone);
    CHECK(sLoc.Distance(sCoord, distance) == KErrNone);
    CHECK(sLoc.Distance(tLoc, distance, delta) == KErrNone);
    CHECK(sLoc.Distance(sLoc, distance, delta) == KErrNone);
    CHECK(sLoc.BearingTo(tCoord, bearing) == KErrNone);
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrNone);

    //
    // 2. TPosition

    TTime time1, time2;
    time1.UniversalTime();
    time2 = time1 + TTimeIntervalMinutes(60);

    sPos = TPosition(sLoc, time1);
    tPos = TPosition(tLoc, time2);

    CHECK(sPos.Speed(tPos, speed) == KErrNone);
    CHECK(sPos.Speed(TPosition(sLoc, time2), speed) == KErrNone);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrNone);
    CHECK(sPos.Speed(TPosition(sLoc, time2), speed, delta) == KErrNone);

    StandardCleanup();
    }
    
void CCalculationMethodsStep::Calculation_InvalidL()
    {
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TRealX nan;
    nan.SetNaN();
    
    const TReal lat = 10, lon = 20, acc = 5;
    const TReal32 dist = 1000, bear = 60;
    
    const TCoordinate zeroCoord(0, 0), north(90, 0), south(-90, 0);
    
    enum ENanParam
        {
        LAT, LON, ACC,
        };

    const TCoordinate nanCoord[] = 
        {
        TCoordinate(nan, lon),
        TCoordinate(lat, nan)
        };

    const TLocality nanLoc[] = 
        {
        TLocality(nanCoord[LAT], acc),
        TLocality(nanCoord[LON], acc),
        TLocality(zeroCoord, nan)
        };

    //
    // 1. TCoordinate
    
    TCoordinate sCoord, tCoord;
    TReal32 distance, bearing;

    // NaN in Distance, BearingTo, Move

    tCoord = zeroCoord;

    sCoord = nanCoord[LAT];
    CHECK(sCoord.Distance(tCoord, distance) == KErrArgument);
    CHECK(sCoord.BearingTo(tCoord, bearing) == KErrArgument);
    CHECK(sCoord.Move(dist, bear) == KErrArgument);

    sCoord = nanCoord[LON];
    CHECK(sCoord.Distance(tCoord, distance) == KErrArgument);
    CHECK(sCoord.BearingTo(tCoord, bearing) == KErrArgument);
    CHECK(sCoord.Move(dist, bear) == KErrArgument);

    sCoord = zeroCoord;

    tCoord = nanCoord[LAT];
    CHECK(sCoord.Distance(tCoord, distance) == KErrArgument);
    CHECK(sCoord.BearingTo(tCoord, bearing) == KErrArgument);

    tCoord = nanCoord[LON];
    CHECK(sCoord.Distance(tCoord, distance) == KErrArgument);
    CHECK(sCoord.BearingTo(tCoord, bearing) == KErrArgument);
    
    // Incalculable cases
    
    // poles
    tCoord = zeroCoord;

    sCoord = north;
    CHECK(sCoord.BearingTo(tCoord, bearing) == KErrPositionIncalculable);
    CHECK(sCoord.Move(dist, bear) == KErrPositionIncalculable);

    sCoord = south;
    CHECK(sCoord.BearingTo(tCoord, bearing) == KErrPositionIncalculable);
    CHECK(sCoord.Move(dist, bear) == KErrPositionIncalculable);
    
    // same coords
    sCoord = tCoord = zeroCoord;
    CHECK(sCoord.BearingTo(tCoord, bearing) == KErrPositionIncalculable);
    
    // antipodal
    tCoord.SetCoordinate(-zeroCoord.Latitude(), zeroCoord.Longitude() + 180);
    CHECK(sCoord.BearingTo(tCoord, bearing) == KErrPositionIncalculable);

    //
    // 2. TLocality
    
    TLocality sLoc, tLoc;
    TReal32 delta;

    // NaN in Distance and BearingTo

    tLoc = TLocality(zeroCoord, acc);

    sLoc = nanLoc[LAT];
    CHECK(sLoc.Distance(tLoc, distance, delta) == KErrArgument);
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrArgument);

    sLoc = nanLoc[LON];
    CHECK(sLoc.Distance(tLoc, distance, delta) == KErrArgument);
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrArgument);

    sLoc = nanLoc[ACC];
    CHECK(sLoc.Distance(tLoc, distance, delta) == KErrArgument);
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrArgument);

    sLoc = TLocality(zeroCoord, acc);

    tLoc = nanLoc[LAT];
    CHECK(sLoc.Distance(tLoc, distance, delta) == KErrArgument);
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrArgument);

    tLoc = nanLoc[LON];
    CHECK(sLoc.Distance(tLoc, distance, delta) == KErrArgument);
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrArgument);
    
    tLoc = nanLoc[ACC];
    CHECK(sLoc.Distance(tLoc, distance, delta) == KErrArgument);
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrArgument);
    
    // Incalculable cases
    
    // 1 latitude degree is about 117 km
    const TReal32 circle = 200 * 1000;
    
    // pole in error circle
    sLoc = TLocality(TCoordinate(89, 0), circle); 
    tLoc = TLocality(zeroCoord, 0);
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrPositionIncalculable);
    
    // overlapping error circles
    sLoc = TLocality(TCoordinate(1, 0), circle);
    tLoc = TLocality(TCoordinate(-1, 0), circle);
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrPositionIncalculable);

    // overlapping error circles if target is antipodal
    sLoc = TLocality(TCoordinate(1, 0), circle);
    tLoc = TLocality(TCoordinate(1, 180), circle); // antipodal is (-1, 0)
    CHECK(sLoc.BearingTo(tLoc, bearing, delta) == KErrPositionIncalculable);

    //
    // 3. TPosition
    
    TPosition sPos, tPos;
    TReal32 speed;
    
    // Nan in Speed methods
    TTime time1, time2;
    time1.UniversalTime();
    time2 = time1 + TTimeIntervalMinutes(60);
    
    tPos = TPosition(TLocality(zeroCoord, acc), time2);

    sPos = TPosition(nanLoc[LAT], time1);
    CHECK(sPos.Speed(tPos, speed) == KErrArgument);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrArgument);

    sPos = TPosition(nanLoc[LON], time1);
    CHECK(sPos.Speed(tPos, speed) == KErrArgument);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrArgument);

    sPos = TPosition(nanLoc[ACC], time1);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrArgument);
    
    sPos = TPosition(TLocality(zeroCoord, acc), time1);

    tPos = TPosition(nanLoc[LAT], time2);
    CHECK(sPos.Speed(tPos, speed) == KErrArgument);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrArgument);

    tPos = TPosition(nanLoc[LON], time2);
    CHECK(sPos.Speed(tPos, speed) == KErrArgument);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrArgument);

    tPos = TPosition(nanLoc[ACC], time2);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrArgument);
    
    // Zero time

    sPos = TPosition(TLocality(zeroCoord, acc), 0);
    tPos = TPosition(TLocality(zeroCoord, acc), time2);
    CHECK(sPos.Speed(tPos, speed) == KErrArgument);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrArgument);

    sPos = TPosition(TLocality(zeroCoord, acc), time1);
    tPos = TPosition(TLocality(zeroCoord, acc), 0);
    CHECK(sPos.Speed(tPos, speed) == KErrArgument);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrArgument);
    
    // Same time

    sPos = TPosition(TLocality(zeroCoord, acc), time1);
    tPos = TPosition(TLocality(zeroCoord, acc), time1);
    CHECK(sPos.Speed(tPos, speed) == KErrArgument);
    CHECK(sPos.Speed(tPos, speed, delta) == KErrArgument);

    StandardCleanup();
    }




TReal32 NextReal32Value(TReal32 aReal32)
	{
    TInt32* pint32 = (TInt32*) &(aReal32);
    (*pint32) = (*pint32) + 1; // works ok only if increment does not touch exponent!!!
    return *((TReal32*) pint32);
	}

TReal32 PrevReal32Value(TReal32 aReal32)
	{
    TInt32* pint32 = (TInt32*) &(aReal32);
    (*pint32) = (*pint32) - 1; // works ok only if decrement does not touch exponent!!!
    return *((TReal32*) pint32);
	}

void CCalculationMethodsStep::Calculation_TCoordinate_MoveBigL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TBuf<256> report;
    const TReal32 KEarthRadius = 6371010;
    const TReal32 KAccuracy = 0.1;

    TInt bearing;
    TReal32 alpha, arc = 2*KPi*KEarthRadius;
    TCoordinate center(0,0), farPoint, closePoint, diff;

    _LIT(KHeader, "=== TCoordinate::Move() test for big distances ===\n");
    INFO_PRINTF1(KHeader);
    
    for (int lon = -180; lon < 180; lon += 60)
    	{
        center = TCoordinate(0, lon);

        report.Format(_L("Center = (%f,%f), Arc(m) = %g(%g)\n"),
                center.Latitude(), center.Longitude(), arc, NextReal32Value(arc));

        for (bearing = 0; bearing < 360; bearing +=60)
        	{
            for (alpha = 0; alpha < 180; alpha += 30)
            	{
                farPoint = closePoint = center;

                TReal32 smallDistance = alpha * KDegToRad * KEarthRadius;
                closePoint.Move(bearing, smallDistance);

                TReal bigDistance = ((2*KPi) - alpha*KDegToRad) * KEarthRadius;
                farPoint.Move(bearing + 180, bigDistance);

                diff = TCoordinate(closePoint.Latitude() - farPoint.Latitude(),
                                   closePoint.Longitude() - farPoint.Longitude());

                if ((Abs(diff.Latitude()) > KAccuracy) ||
                    (Abs(diff.Longitude()) > KAccuracy))
                	{
                    report.Format(_L("Alpha %g\n"), alpha);
                    LOG_DES(report);

                    _LIT(KBearingDistance, "\tBearing %3d, Dist(m) %g: (%f;%f)\n");
                    report.Format(KBearingDistance, bearing, smallDistance, closePoint.Latitude(), closePoint.Longitude());
                    LOG_DES(report);

                    report.Format(KBearingDistance, bearing + 180, bigDistance, farPoint.Latitude(), farPoint.Longitude());
                    LOG_DES(report);

                    report.Format(_L("\tDiff (%f;%f)\n"), diff.Latitude(), diff.Longitude());
                    LOG_DES(report);
                	}
            	}
        	}
    	}

    _LIT(KFooter, "=== TCoordinate::Move() test done ===\n");
    INFO_PRINTF1(KFooter);
    
    StandardCleanup();
	}
