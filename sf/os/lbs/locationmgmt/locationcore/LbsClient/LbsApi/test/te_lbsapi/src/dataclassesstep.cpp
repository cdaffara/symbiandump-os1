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
 @file DataClassesStep.cpp
 @internalTechnology
*/
#include "dataclassesstep.h"
#include "te_lbsapisuitedefs.h"

#include "lcfsbucommondefinitions.h"

#include <lbs.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsgpstimingmeasurement.h>
#include <lbs/lbsgpsmeasurement.h>
   
// used to confirm that PositionXInfo class sizes do not change
const TUint KPositionSatelliteInfoSize = 920;  
const TUint KPositionGpsMeasurementInfoSize = 832;


CDataClassesStep::~CDataClassesStep()
/**
 * Destructor
 */
	{
	}

CDataClassesStep::CDataClassesStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KDataClassesStep);
	}

TVerdict CDataClassesStep::doTestStepPreambleL()
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


TVerdict CDataClassesStep::doTestStepL()
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



TVerdict CDataClassesStep::doTestStepPostambleL()
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

void CDataClassesStep::StartL(TInt aIndex)
	{
	switch (aIndex)
		{
	case 0:
		TCoordinate_BlankConstructionL ();
		break;
	case 1:
		TCoordinate_ConstructorsL ();
		break;
	case 2:
		TCoordinate_Constructors2L ();
		break;
	case 3:
		TCoordinate_InvalidL ();
		break;
	case 4:
		TCoordinate_NormalizationWithNansL ();
		break;
	case 5:
		TLocality_BlankConstructionL ();
		break;
	case 6:
		TLocality_ConstructorsL ();
		break;
	case 7:
		TLocality_Constructors2L ();
		break;
	case 8:
		TLocality_NegativeL ();
		break;
	case 9:
		TPosition_ConstructionL ();
		break;
	case 10:
		TPosition_SettersL ();
		break;
	case 11:
		TPositionInfo_ConstructionL ();
		break;
	case 12:
		TPositionInfo_ModificationL ();
		break;
	case 13:
		CourseClasses_ConstructionL ();
		break;
	case 14:
		CourseClasses_Modification2L ();
		break;
	case 15:
		CourseClasses_ModificationL ();
		break;
	case 16:
		CourseClasses_InvalidL ();
		break;
	case 17:
		SatelliteClasses_ConstructionL ();
		break;
	case 18:
		SatelliteClasses_PropertiesL ();
		break;
	case 19:
		SatelliteClasses_Properties2L ();
		break;
	case 20:
		SatelliteClasses_InfoL ();
		break;
	case 21:
		SatelliteClasses_SatelliteInfoL ();
		break;
	case 22:
		SatelliteClasses_OverflowL ();
		break;
	case 23:
       GpsMeasurementClasses_PropertiesL ();
       break;
	default:
		break;
		}
	}


extern void CheckPanic(const char* aFileName, TInt aLine);

//
//
// TCoordinate
//
//

// 2.1.1. Default constructor sets values to NAN
void CDataClassesStep::TCoordinate_BlankConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TCoordinate coordinate;
    CHECK(Math::IsNaN(coordinate.Latitude()));
    CHECK(Math::IsNaN(coordinate.Longitude()));
    CHECK(Math::IsNaN(coordinate.Altitude()));
    CHECK_EQUAL_(KPositionDatumWgs84.iUid, coordinate.Datum().iUid);

    StandardCleanup();
	}

// 2.1.2 Parameterized constructors and Setters work same
void CDataClassesStep::TCoordinate_ConstructorsL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    const TReal64 dumLat = 11.111;
    const TReal64 dumLon = 22.222;
    const TReal32 dumAlt = TReal32(33.333);

    TCoordinate coordSet;

    // 2 parameters

    coordSet.SetCoordinate(dumLat, dumLon);

    CHECK_REAL_EQUAL(coordSet.Latitude() , dumLat);
    CHECK_REAL_EQUAL(coordSet.Longitude(), dumLon);
    CHECK(Math::IsNaN(coordSet.Altitude()));

    TCoordinate coord2p(dumLat, dumLon);

    CHECK_REAL_EQUAL(coord2p.Latitude()  , coordSet.Latitude());
    CHECK_REAL_EQUAL(coord2p.Longitude() , coordSet.Longitude());
    CHECK(Math::IsNaN(coord2p.Altitude()));

    // 3 params
    coordSet.SetCoordinate(dumLat, dumLon, dumAlt);

    CHECK_REAL_EQUAL(coordSet.Latitude() , dumLat);
    CHECK_REAL_EQUAL(coordSet.Longitude(), dumLon);
    CHECK_REAL_EQUAL(coordSet.Altitude() , dumAlt);

    TCoordinate coord3p(dumLat, dumLon, dumAlt);

    CHECK_REAL_EQUAL(coord3p.Latitude() , coordSet.Latitude());
    CHECK_REAL_EQUAL(coord3p.Longitude(), coordSet.Longitude());
    CHECK_REAL_EQUAL(coord3p.Altitude() , coordSet.Altitude());

    // datum
    const TPositionDatumId otherDatum = {0x10101010};
    coordSet.SetDatum(otherDatum);
    CHECK_EQUAL_(otherDatum.iUid, coordSet.Datum().iUid);

    StandardCleanup();
	}

// 2.1.3 Parameterized constructors and Setters work same with NANs
void CDataClassesStep::TCoordinate_Constructors2L()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TRealX nan;
    nan.SetNaN();

    TReal64 dumLat(nan);
    TReal64 dumLon(nan);
    TReal32 dumAlt(nan);

    TCoordinate coordSet;

    // 2 parameters

    coordSet.SetCoordinate(dumLat, dumLon);

    CHECK(Math::IsNaN(coordSet.Latitude()));
    CHECK(Math::IsNaN(coordSet.Longitude()));
    CHECK(Math::IsNaN(coordSet.Altitude()));

    TCoordinate coord2p(dumLat, dumLon);

    CHECK(Math::IsNaN(coord2p.Latitude()));
    CHECK(Math::IsNaN(coord2p.Longitude()));
    CHECK(Math::IsNaN(coord2p.Altitude()));

    // 3 params
    coordSet.SetCoordinate(dumLat, dumLon, dumAlt);

    CHECK(Math::IsNaN(coordSet.Latitude()));
    CHECK(Math::IsNaN(coordSet.Longitude()));
    CHECK(Math::IsNaN(coordSet.Altitude()));

    TCoordinate coord3p(dumLat, dumLon, dumAlt);

    CHECK(Math::IsNaN(coord3p.Latitude()));
    CHECK(Math::IsNaN(coord3p.Longitude()));
    CHECK(Math::IsNaN(coord3p.Altitude()));

    StandardCleanup();
	}

// 2.1.4 Bad Input in TCoordinate

// The function is to normalize Lat/Lon parameters
// to [-90,+90] and (-180,+180] ranges respectively.
// If Latitude wraps +90 or -90 boundary, than Longitude
// is corrected to be shifted to another hemisphere
//
// IF Latitude is in 1st quarter of circle THEN nothing to do
// IF Latitude is in 2nd or 3rd quarter,
// THEN Longitude is shifted to opposite hemisphere, and Latitude
// is mirrored about 90 degrees (to accord with longitude)
// IF Latitude is in 4th quarter, THEN Latitude is mirrored
// about 0 degrees (to be below equator)
//
// NOTE that as +90 and -90 a valid Latitude values,
// then 4th quarter begins from +270 inclusively (it is -90 inclusively in fact)
//
// Latitude is processed first as it has its influence on Longitude
void NormalLatLon(TReal lat, TReal lon, TReal& nlat, TReal& nlon)
	{
    // 1. latitude

    // save sign, because negative and positive latitude
    // values are processed same way
    TBool isNegLat = (lat < 0);

    // truncate by 360, work with positive values
    TReal absLat = isNegLat ? -lat : lat;
    Math::Mod(lat, absLat, 360);  // 0 <= lat < 360

    // correct values, if latitude wraps 90 deg boundary
    if (lat >= 270)         // 4th quarter
    {
        lat = lat - 360;    // reverse
    }
    else
    if (lat > 90)           // 2nd or 3rd quarter
    {
        lat = 180 - lat;    // round about 90
        lon = lon + 180;    // another hemisphere
    }

    // recall latutide sign
    if (isNegLat) lat = - lat;

    // 2. longitude

    // -360 < lon < 360
    Math::Mod(lon, lon, 360);
    // let's put lon to -180 .. 180 range
    if (lon > 180)
    {
        lon = lon - 360;
    }
    else
    if (lon < - 180) // for negative longitudes
    {
        lon = lon + 360;
    }

    nlat = lat;
    nlon = lon;
	}

void XYZfromLatLon(TReal& x, TReal& y, TReal& z, TReal lat, TReal lon)
	{
    Math::Mod(lat, lat, 360);  // -360 < lat < 360
    Math::Mod(lon, lon, 360);  // -360 < lon < 360


    // if latitude is 90 deg or -90 deg, then reduce it by one degree : 89 deg.
    // it allows to check longitude direction more correctly (avoid cos(lat)==0)
    if ((lat == 90) || (lat == -270))
    {
        lat -= 1;
    }
    else
    if ((lat == -90) || (lat == 270))
    {
        lat += 1;
    }

    TReal latCos, latSin, lonCos, lonSin;
    Math::Cos(latCos, lat * KDegToRad);
    Math::Sin(latSin, lat * KDegToRad);
    Math::Cos(lonCos, lon * KDegToRad);
    Math::Sin(lonSin, lon * KDegToRad);

    x = latCos * lonCos;
    y = latCos * lonSin;
    z = latSin;
	}

TBool IsSamePoint(TReal lat1, TReal lon1, TReal lat2, TReal lon2)
	{
    TReal x1,y1,z1, x2,y2,z2;

    XYZfromLatLon(x1,y1,z1, lat1, lon1);    // source values
    XYZfromLatLon(x2,y2,z2, lat2, lon2);    // TCoordinate

    const TReal tolerance = 0.0000001; // 1 of million

    TBool same = (Abs(x1 - x2) < tolerance) &&
                 (Abs(y1 - y2) < tolerance) &&
                 (Abs(z1 - z2) < tolerance);
    return same;
	}


void CDataClassesStep::TCoordinate_InvalidL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    const TReal32 alt = 10;
    TBool noErrors = ETrue;

    // 1. Valid WGS84 range + Out of WGS84 range

    // check values (step 30 degrees) in (-540..+540] of longitude
    for (TReal64 lon = 540; lon > -540; lon -= 30)
    	{
        // check values (step 30 degrees) in [-450; 450] of latitude
        for (TReal64 lat = 720; lat >= -720; lat -= 30)
        	{
            // set
            TCoordinate coordinate2, coordinate3;
            coordinate2.SetCoordinate(lat,lon);
            coordinate3.SetCoordinate(lat,lon, alt);

            // construct
            TCoordinate coord2(lat, lon);
            TCoordinate coord3(lat, lon, alt);

            TBool isValidCoordinate2 = IsSamePoint(lat, lon, coordinate2.Latitude(), coordinate2.Longitude());
            TBool isValidCoordinate3 = IsSamePoint(lat, lon, coordinate3.Latitude(), coordinate3.Longitude());
            TBool isValidCoord2 = IsSamePoint(lat, lon, coord2.Latitude(), coord2.Longitude());
            TBool isValidCoord3 = IsSamePoint(lat, lon, coord3.Latitude(), coord3.Longitude());


            TReal nlat, nlon;
            NormalLatLon(lat, lon, nlat, nlon);
            TBool isValidNormalLatLon = IsSamePoint(lat, lon, nlat, nlon);

            if (!isValidCoordinate2 || !isValidCoordinate3 || !isValidCoord2 || !isValidCoord3 || !isValidNormalLatLon)
            	{
                noErrors = EFalse;
            	}
        	}
    	}

    if (!noErrors) LOG("There were errors in Normalization.");
    CHECK(noErrors);

    StandardCleanup();
	}



TInt TCoordinate_NormalizationWithNans_Panic(TAny* /*aPtr*/)
	{
    TRealX nan; nan.SetNaN();

    const TReal KBigLatitude = 120.0;
    const TReal KBigLongitude = 200.0;

    const TReal KGoodLatitude = 180 - KBigLatitude;
    const TReal KGoodLongitude = KBigLongitude - 360;

    TCoordinate coord;

    // this will cause normalization process
    // this should not panic even though longitude is NaN
    coord.SetCoordinate(KBigLatitude, nan);
    CHECK_PANIC(coord.Latitude() == KGoodLatitude);
    CHECK_PANIC(Math::IsNaN(coord.Longitude()));

    // this will cause normalization process
    // this should not panic even though latitude is NaN
    coord.SetCoordinate(nan, KBigLongitude);
    CHECK_PANIC(coord.Longitude() == KGoodLongitude);
    CHECK_PANIC(Math::IsNaN(coord.Latitude()));

    return KErrNone;
	}

// 2.1.5 Normalization if one of members is NaN
void CDataClassesStep::TCoordinate_NormalizationWithNansL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    DO_PANIC_TEST_L(
        TCoordinate_NormalizationWithNans_Panic,
        KNoClientFault,
        KErrNone,
        KDefaultTestTimeout);

    StandardCleanup();
	}


//
//
// TLocality
//
//

// 2.3.1. Blank constructor
void CDataClassesStep::TLocality_BlankConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TLocality locality;
    CHECK(Math::IsNaN(locality.Latitude()));
    CHECK(Math::IsNaN(locality.Longitude()));
    CHECK(Math::IsNaN(locality.Altitude()));
    CHECK(Math::IsNaN(locality.HorizontalAccuracy()));
    CHECK(Math::IsNaN(locality.VerticalAccuracy()));

    StandardCleanup();
	}

// 2.3.2. Compare constructor parameters setting and using setters
void CDataClassesStep::TLocality_ConstructorsL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    const TReal64 dumLat = 11.111;
    const TReal64 dumLon = 22.222;
    const TReal32 dumAlt = TReal32(33.333);
    const TReal32 dumHorAcc = TReal32(0.444);
    const TReal32 dumVerAcc = TReal32(0.555);

    TCoordinate coord(dumLat, dumLon, dumAlt);

    // hor
    TLocality localSetHor;
    localSetHor.SetHorizontalAccuracy(dumHorAcc);

    CHECK_REAL_EQUAL(localSetHor.HorizontalAccuracy() , dumHorAcc);
    CHECK(Math::IsNaN(localSetHor.VerticalAccuracy()));

    TLocality localHor(coord, dumHorAcc);

    CHECK_REAL_EQUAL(localHor.Latitude() , dumLat);
    CHECK_REAL_EQUAL(localHor.Longitude(), dumLon);
    CHECK_REAL_EQUAL(localHor.Altitude() , dumAlt);

    CHECK_REAL_EQUAL(localHor.HorizontalAccuracy(), dumHorAcc);
    CHECK(Math::IsNaN(localHor.VerticalAccuracy()));

    // ver
    TLocality localSetVer;
    localSetVer.SetVerticalAccuracy(dumVerAcc);

    CHECK(Math::IsNaN(localSetVer.HorizontalAccuracy()));
    CHECK_REAL_EQUAL(localSetVer.VerticalAccuracy() , dumVerAcc);

    // both
    TLocality localSet;
    localSet.SetAccuracy(dumHorAcc, dumVerAcc);

    CHECK_REAL_EQUAL(localSet.HorizontalAccuracy(), dumHorAcc);
    CHECK_REAL_EQUAL(localSet.VerticalAccuracy()  , dumVerAcc);

    TLocality localVer(coord, dumHorAcc, dumVerAcc);

    CHECK_REAL_EQUAL(localVer.Latitude() , dumLat);
    CHECK_REAL_EQUAL(localVer.Longitude(), dumLon);
    CHECK_REAL_EQUAL(localVer.Altitude() , dumAlt);

    CHECK_REAL_EQUAL(localVer.HorizontalAccuracy(), dumHorAcc);
    CHECK_REAL_EQUAL(localVer.VerticalAccuracy()  , dumVerAcc);

    StandardCleanup();
	}

// 2.3.3. Compare constructor parameters setting and using setters, NANs
TInt TLocality_Constructors_and_Setters_with_NANs(TAny* /*aPtr*/)
	{
    TRealX nan; nan.SetNaN();

    const TReal64 dumLat(nan);
    const TReal64 dumLon(nan);
    const TReal32 dumAlt(nan);
    const TReal32 dumHorAcc(nan);
    const TReal32 dumVerAcc(nan);

    TCoordinate coord(dumLat, dumLon, dumAlt);

    // hor
    TLocality localSetHor;
    localSetHor.SetHorizontalAccuracy(dumHorAcc);

    CHECK_PANIC(Math::IsNaN(localSetHor.HorizontalAccuracy()));
    CHECK_PANIC(Math::IsNaN(localSetHor.VerticalAccuracy()));

    TLocality localHor(coord, dumHorAcc);

    CHECK_PANIC(Math::IsNaN(localHor.Latitude()));
    CHECK_PANIC(Math::IsNaN(localHor.Longitude()));
    CHECK_PANIC(Math::IsNaN(localHor.Altitude()));

    CHECK_PANIC(Math::IsNaN(localHor.HorizontalAccuracy()));
    CHECK_PANIC(Math::IsNaN(localHor.VerticalAccuracy()));

    // ver
    TLocality localSetVer;
    localSetVer.SetVerticalAccuracy(dumVerAcc);

    CHECK_PANIC(Math::IsNaN(localSetVer.HorizontalAccuracy()));
    CHECK_PANIC(Math::IsNaN(localSetVer.VerticalAccuracy()));

    // both
    TLocality localSet;
    localSet.SetAccuracy(dumHorAcc, dumVerAcc);

    CHECK_PANIC(Math::IsNaN(localSet.HorizontalAccuracy()));
    CHECK_PANIC(Math::IsNaN(localSet.VerticalAccuracy()));

    TLocality localVer(coord, dumHorAcc, dumVerAcc);

    CHECK_PANIC(Math::IsNaN(localVer.Latitude()));
    CHECK_PANIC(Math::IsNaN(localVer.Longitude()));
    CHECK_PANIC(Math::IsNaN(localVer.Altitude()));

    CHECK_PANIC(Math::IsNaN(localVer.HorizontalAccuracy()));
    CHECK_PANIC(Math::IsNaN(localVer.VerticalAccuracy()));

    return KErrNone;
	}


void CDataClassesStep::TLocality_Constructors2L()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    DO_PANIC_TEST_L(
        TLocality_Constructors_and_Setters_with_NANs,
        KNoClientFault,
        KErrNone,
        KDefaultTestTimeout);

    StandardCleanup();
	}

// 2.3.4 Negative Accuracy values
TInt TLocality_Negative_Accuracy_Construct_HorAcc(TAny* /*aPtr*/)
	{
    const TReal32 dumAcc = TReal32(0.1);
    TLocality locality(TCoordinate(1,2,3), -dumAcc);

    locality.Altitude();
    return KErrGeneral;
	}


TInt TLocality_Negative_Accuracy_Construct_VerAcc(TAny* /*aPtr*/)
	{
    const TReal32 dumAcc = TReal32(0.1);
    TLocality locality(TCoordinate(1,2,3), dumAcc, -dumAcc);

    locality.Altitude();
    return KErrGeneral;
	}


TInt TLocality_Negative_Accuracy_Set_Both_HorNegative(TAny* /*aPtr*/)
	{
    const TReal32 dumAcc = TReal32(0.1);
    TLocality locality;
    locality.SetAccuracy(-dumAcc, dumAcc);
    return KErrGeneral;
	}


TInt TLocality_Negative_Accuracy_Set_Both_VerNegative(TAny* /*aPtr*/)
	{
    const TReal32 dumAcc = TReal32(0.1);
    TLocality locality;
    locality.SetAccuracy(dumAcc, -dumAcc);
    return KErrGeneral;
	}


TInt TLocality_Negative_Accuracy_Set_HorNegative(TAny* /*aPtr*/)
	{
    const TReal32 dumAcc = TReal32(0.1);
    TLocality locality;
    locality.SetHorizontalAccuracy(-dumAcc);
    return KErrGeneral;
	}


TInt TLocality_Negative_Accuracy_Set_VerNegative(TAny* /*aPtr*/)
	{
    const TReal32 dumAcc = TReal32(0.1);
    TLocality locality;
    locality.SetVerticalAccuracy(-dumAcc);
    return KErrGeneral;
	}


void CDataClassesStep::TLocality_NegativeL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    DO_PANIC_TEST_L(TLocality_Negative_Accuracy_Construct_HorAcc,
        KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);
    DO_PANIC_TEST_L(TLocality_Negative_Accuracy_Construct_VerAcc,
        KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);

    DO_PANIC_TEST_L(TLocality_Negative_Accuracy_Set_Both_HorNegative,
        KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);
    DO_PANIC_TEST_L(TLocality_Negative_Accuracy_Set_Both_VerNegative,
        KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);

    DO_PANIC_TEST_L(TLocality_Negative_Accuracy_Set_HorNegative,
        KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);
    DO_PANIC_TEST_L(TLocality_Negative_Accuracy_Set_VerNegative,
        KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);

    StandardCleanup();
	}


//
//
// TPosition
//
//

// 2.5.1. Constructors
void CDataClassesStep::TPosition_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    // blank
    TPosition position;

    CHECK(position.Time() == TTime(0));
    CHECK(Math::IsNaN(position.Latitude()));
    CHECK(Math::IsNaN(position.Longitude()));
    CHECK(Math::IsNaN(position.Altitude()));
    CHECK(Math::IsNaN(position.HorizontalAccuracy()));
    CHECK(Math::IsNaN(position.VerticalAccuracy()));

    // full

    const TReal64 dumLat = 11.111;
    const TReal64 dumLon = 22.222;
    const TReal32 dumAlt = TReal32(33.333);
    const TReal32 dumHorAcc = TReal32(0.444);
    const TReal32 dumVerAcc = TReal32(0.555);

    TTime dumTime;
    dumTime.HomeTime();

    TCoordinate coordinate(dumLat, dumLon, dumAlt);
    TLocality locality(coordinate, dumHorAcc, dumVerAcc);
    position = TPosition(locality, dumTime);

    CHECK(position.Time() == dumTime);
    CHECK_REAL_EQUAL(position.Latitude(), dumLat);
    CHECK_REAL_EQUAL(position.Longitude(), dumLon);
    CHECK_REAL_EQUAL(position.Altitude(), dumAlt);
    CHECK_REAL_EQUAL(position.HorizontalAccuracy(), dumHorAcc);
    CHECK_REAL_EQUAL(position.VerticalAccuracy(), dumVerAcc);

    StandardCleanup();
	}

// 2.5.2. Setters/Getters (time)
void CDataClassesStep::TPosition_SettersL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    // time
    TTime dumTime;
    dumTime.HomeTime();

    TPosition pos;
    pos.SetTime(dumTime);
    CHECK_EQUAL_(dumTime, pos.Time());

    // current time
    TPosition position;
    position.SetCurrentTime();

    TTime uniTime;
    uniTime.UniversalTime();

    TTimeIntervalSeconds last;
    uniTime.SecondsFrom(position.Time(), last);
    CHECK(last <= TTimeIntervalSeconds(1));

    StandardCleanup();
	}


//
//
// TPositionInfo
//
//

// 3.1.1. TPositionInfo construction
void CDataClassesStep::TPositionInfo_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TPositionInfo posInfo;

    // position
    TPosition pos;
    posInfo.GetPosition(pos);

    CHECK(Math::IsNaN(pos.Latitude()));
    CHECK(Math::IsNaN(pos.Longitude()));
    CHECK(Math::IsNaN(pos.Altitude()));
    CHECK(Math::IsNaN(pos.HorizontalAccuracy()));
    CHECK(Math::IsNaN(pos.VerticalAccuracy()));
    CHECK_EQUAL_(TTime(0), pos.Time());

    // From Base

    // class type
    CHECK(posInfo.PositionClassType() == EPositionInfoClass);
    CHECK(posInfo.PositionClassSize() == sizeof(TPositionInfo));

    // module id
    CHECK_EQUAL_((TInt)posInfo.ModuleId().iUid, (TInt)KPositionNullModuleId.iUid);

    // update info
    CHECK(posInfo.UpdateType() == EPositionUpdateGeneral);

    StandardCleanup();
	}

// 3.1.2. TPositionInfo modification
void CDataClassesStep::TPositionInfo_ModificationL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    const TReal64 dumLat = 11.111;
    const TReal64 dumLon = 22.222;
    const TReal32 dumAlt = TReal32(33.333);
    const TReal32 dumHorAcc = TReal32(0.444);
    const TReal32 dumVerAcc = TReal32(0.555);

    TTime dumTime;
    dumTime.HomeTime();

    TPositionInfo posInfo;
    TCoordinate dumCoordinate = TCoordinate(dumLat, dumLon, dumAlt);
    TLocality dumLocality = TLocality(dumCoordinate, dumHorAcc, dumVerAcc);
    
    // position
    TPosition pos(dumLocality, dumTime);
    posInfo.SetPosition(pos);

    TPosition pos2;
    posInfo.GetPosition(pos2);

    CHECK_REAL_EQUAL(pos2.Latitude(),  pos.Latitude());
    CHECK_REAL_EQUAL(pos2.Longitude(), pos.Longitude());
    CHECK_REAL_EQUAL(pos2.Altitude(),  pos.Altitude());
    CHECK_REAL_EQUAL(pos2.HorizontalAccuracy(), pos.HorizontalAccuracy());
    CHECK_REAL_EQUAL(pos2.VerticalAccuracy(),   pos.VerticalAccuracy());
    CHECK_EQUAL_(pos2.Time(), pos.Time());

    // From Base

    // module id

    const TPositionModuleId dumModId = {0x12345678};
    posInfo.SetModuleId(dumModId);
    CHECK_EQUAL_((TInt)dumModId.iUid, (TInt)posInfo.ModuleId().iUid);

    // update info

    posInfo.SetUpdateType(EPositionUpdateUnknown);
    CHECK(posInfo.UpdateType() == EPositionUpdateUnknown);

    posInfo.SetUpdateType(EPositionUpdateGeneral);
    CHECK(posInfo.UpdateType() == EPositionUpdateGeneral);

    StandardCleanup();
	}


//
//
// Course classes
//
//

// 4.1.1. Construction
void CDataClassesStep::CourseClasses_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    // course
    TCourse course;
    CHECK(Math::IsNaN(course.Speed()));
    CHECK(Math::IsNaN(course.Heading()));
    CHECK(Math::IsNaN(course.SpeedAccuracy()));
    CHECK(Math::IsNaN(course.HeadingAccuracy()));

        // course info
    TPositionCourseInfo courseInfo;
    CHECK(courseInfo.PositionClassType()==(EPositionInfoClass|EPositionCourseInfoClass));
    CHECK_EQUAL_(courseInfo.PositionClassSize(), sizeof(TPositionCourseInfo));

    CHECK_EQUAL_((TInt)courseInfo.ModuleId().iUid, (TInt)KPositionNullModuleId.iUid);
    CHECK(courseInfo.UpdateType() == EPositionUpdateGeneral);

    StandardCleanup();
	}


// 4.1.2. Modification
void CDataClassesStep::CourseClasses_ModificationL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    const TReal32 KDummyValue1 = (TReal32)43.342;
    const TReal32 KDummyValue2 = (TReal32)34.5236;
    const TReal32 KDummyValue3 = (TReal32)983.45;
    const TReal32 KDummyValue4 = (TReal32)12.432;

    TCourse course;
    course.SetSpeed(KDummyValue1);
    course.SetHeading(KDummyValue2);
    course.SetSpeedAccuracy(KDummyValue3);
    course.SetHeadingAccuracy(KDummyValue4);

    CHECK_REAL_EQUAL(course.Speed(), KDummyValue1);
    CHECK_REAL_EQUAL(course.Heading(), KDummyValue2);
    CHECK_REAL_EQUAL(course.SpeedAccuracy(), KDummyValue3);
    CHECK_REAL_EQUAL(course.HeadingAccuracy(), KDummyValue4);

    TCourse course2;        //Use new course object so we can be sure data is transfered properly

    TPositionCourseInfo courseInfo;
    courseInfo.SetCourse(course);
        courseInfo.GetCourse(course2);

    CHECK_REAL_EQUAL(course2.Speed(), KDummyValue1);
    CHECK_REAL_EQUAL(course2.Heading(), KDummyValue2);
    CHECK_REAL_EQUAL(course2.SpeedAccuracy(), KDummyValue3);
    CHECK_REAL_EQUAL(course2.HeadingAccuracy(), KDummyValue4);

    StandardCleanup();
	}


// 4.1.3. NaNs
TInt CourseClasses_Setters_with_NANs(TAny* /*aPtr*/)
	{
    TRealX nan;
    nan.SetNaN();

    TCourse course;
    course.SetSpeed(nan);
    course.SetHeading(nan);
    course.SetCourse(nan);
    course.SetSpeedAccuracy(nan);
    course.SetHeadingAccuracy(nan);
    course.SetCourseAccuracy(nan);

    CHECK_PANIC(Math::IsNaN(course.Speed()));
    CHECK_PANIC(Math::IsNaN(course.Heading()));
    CHECK_PANIC(Math::IsNaN(course.Course()));
    CHECK_PANIC(Math::IsNaN(course.SpeedAccuracy()));
    CHECK_PANIC(Math::IsNaN(course.HeadingAccuracy()));
    CHECK_PANIC(Math::IsNaN(course.CourseAccuracy()));

    return KErrNone;
	}


void CDataClassesStep::CourseClasses_Modification2L()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    DO_PANIC_TEST_L(CourseClasses_Setters_with_NANs,
        KNoClientFault, KErrNone, KDefaultTestTimeout);

    StandardCleanup();
	}


// 4.1.4. Invalid values
TInt CourseClasses_Invalid_Input_HeadAcc(TAny* /*aPtr*/)
	{
    TCourse course;
    course.SetHeadingAccuracy(TReal32(-0.1));
    return KErrGeneral;
	}


TInt CourseClasses_Invalid_Input_SpeedAcc(TAny* /*aPtr*/)
	{
    TCourse course;
    course.SetSpeedAccuracy(TReal32(-0.1));
    return KErrGeneral;
	}


void CDataClassesStep::CourseClasses_InvalidL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    DO_PANIC_TEST_L(CourseClasses_Invalid_Input_HeadAcc,
        KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);

    DO_PANIC_TEST_L(CourseClasses_Invalid_Input_SpeedAcc,
        KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);

    StandardCleanup();
	}

//
//
// Satellite classes
//
//

// 4.3.1. Construction
void CDataClassesStep::SatelliteClasses_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    // satellite
    TSatelliteData satData;

    CHECK_EQUAL_(-1, satData.SatelliteId());
    CHECK(Math::IsNaN(satData.Azimuth()));
    CHECK(Math::IsNaN(satData.Elevation()));
    CHECK(!satData.IsUsed());
    CHECK_EQUAL_(0, satData.SignalStrength());
    
	//Sizeof(class) not always same in emulator and hardware as alignment restriction in hardware.
    #if defined(_DEBUG) 
	// Check the size of the TPositionSatelliteInfo class is unchanged
    CHECK_EQUAL_(KPositionSatelliteInfoSize, sizeof(TPositionSatelliteInfo));
    #endif 
    
	// satellite info
    TPositionSatelliteInfo satInfo;

    CHECK_EQUAL_(KErrNotFound, satInfo.GetSatelliteData(0, satData));
    CHECK_EQUAL_(0, satInfo.NumSatellitesInView());
    CHECK_EQUAL_(0, satInfo.NumSatellitesUsed());
    CHECK(TTime(0) == satInfo.SatelliteTime());
    CHECK(Math::IsNaN(satInfo.HorizontalDoP()));
    CHECK(Math::IsNaN(satInfo.VerticalDoP()));
    CHECK(Math::IsNaN(satInfo.TimeDoP()));

    StandardCleanup();
	}


// 4.3.2. Properties
void CDataClassesStep::SatelliteClasses_PropertiesL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TSatelliteData satData;

    const TInt dumSatelliteId = 11111;
    const TInt dumSignalStrength = 22222;
    const TReal32 dumAzimuth = TReal32(234.2345);
    const TReal32 dumElevation = TReal32(4745.1231);

    satData.SetSignalStrength(dumSignalStrength);
    satData.SetSatelliteId(dumSatelliteId);
    satData.SetAzimuth(dumAzimuth);
    satData.SetElevation(dumElevation);

    CHECK_EQUAL_(dumSatelliteId,     satData.SatelliteId());
    CHECK_EQUAL_(dumSignalStrength,  satData.SignalStrength());
    CHECK_REAL_EQUAL(dumAzimuth,    satData.Azimuth());
    CHECK_REAL_EQUAL(dumElevation,  satData.Elevation());

    satData.SetIsUsed(ETrue);
    CHECK(satData.IsUsed());
    satData.SetIsUsed(EFalse);
    CHECK(!satData.IsUsed());

    TPositionSatelliteInfo satInfo;

    const TTime dumSatTime(TInt64(54));
    const TReal32 dumHorzDoP = (TReal32) 345.2131;
    const TReal32 dumVertDoP = (TReal32) 34.1;
    const TReal32 dumTimeDoP = (TReal32) 876.68;

    satInfo.SetSatelliteTime(dumSatTime);
    satInfo.SetHorizontalDoP(dumHorzDoP);
    satInfo.SetVerticalDoP(dumVertDoP);
    satInfo.SetTimeDoP(dumTimeDoP);

    CHECK_REAL_EQUAL(dumHorzDoP,    satInfo.HorizontalDoP());
    CHECK_REAL_EQUAL(dumVertDoP,    satInfo.VerticalDoP());
    CHECK_REAL_EQUAL(dumTimeDoP,    satInfo.TimeDoP());
    CHECK_EQUAL_(     dumSatTime,    satInfo.SatelliteTime());

    TPositionExtendedSatelliteInfo extSatInfo;
    TGpsTimingMeasurementData gpsTimingData;
           
    // all max values for UTran/Fdd 
    const TInt dumGpsTocMs  = (TInt) 16383;
    const TUint dumGpsTocLs = (TUint) 4294967295UL;
    const TInt dumRefId     = (TInt) 511;
    const TInt dumSfn       = (TInt) 4095;
   
    gpsTimingData.SetDataType(TGpsTimingMeasurementData::EGpsTimingDataTypeUtran);
    gpsTimingData.SetNetworkMode(TGpsTimingMeasurementData::ENetworkModeFdd);
    gpsTimingData.SetGPSTimingOfCellMsPart(dumGpsTocMs);
    gpsTimingData.SetGPSTimingOfCellLsPart(dumGpsTocLs);
    gpsTimingData.SetReferenceIdentity(dumRefId);
    gpsTimingData.SetSfn(dumSfn);
       
    extSatInfo.SetGpsTimingData(gpsTimingData);
       
    TGpsTimingMeasurementData gpsTimingDataReturned;
    CHECK_EQUAL_(KErrNone, extSatInfo.GetGpsTimingData(gpsTimingDataReturned));
      
    CHECK_EQUAL_(TGpsTimingMeasurementData::EGpsTimingDataTypeUtran, gpsTimingData.DataType());
    CHECK_EQUAL_(TGpsTimingMeasurementData::ENetworkModeFdd, gpsTimingData.NetworkMode());
    CHECK_EQUAL_(dumGpsTocMs, gpsTimingDataReturned.GPSTimingOfCellMsPart());
    CHECK_EQUAL_(dumGpsTocLs, gpsTimingDataReturned.GPSTimingOfCellLsPart());
    CHECK_EQUAL_(dumRefId,    gpsTimingDataReturned.ReferenceIdentity());
    CHECK_EQUAL_(dumSfn,      gpsTimingDataReturned.Sfn());

    
    TGpsTimingMeasurementData gpsTimingData2;
   
    // all max values for GSM 
    const TInt refFrameMsb      = (TInt) 63;
    const TInt gpsTowSubms      = (TInt) 9999;
    const TInt deltaTow         = (TInt) 127;
    const TInt gpsRefTimeUncert = (TInt) 127;

    gpsTimingData2.SetDataType(TGpsTimingMeasurementData::EGpsTimingDataTypeGsm);

    // check default GSM values
    CHECK_EQUAL_(TGpsTimingMeasurementData::EGpsTimingDataTypeGsm, gpsTimingData2.DataType());
    CHECK_EQUAL_(0,            gpsTimingData2.ReferenceFrameMsb());
    CHECK_EQUAL_(KErrNotFound, gpsTimingData2.GpsTowSubms());
    CHECK_EQUAL_(KErrNotFound, gpsTimingData2.DeltaTow());
    CHECK_EQUAL_(KErrNotFound, gpsTimingData2.GpsReferenceTimeUncertainty());
   
    // update values
    gpsTimingData2.SetReferenceFrameMsb(refFrameMsb);
    gpsTimingData2.SetGpsTowSubms(gpsTowSubms);
    gpsTimingData2.SetDeltaTow(deltaTow);
    gpsTimingData2.SetGpsReferenceTimeUncertainty(gpsRefTimeUncert);
   
    CHECK_EQUAL_(refFrameMsb,      gpsTimingData2.ReferenceFrameMsb());
    CHECK_EQUAL_(gpsTowSubms,      gpsTimingData2.GpsTowSubms());
    CHECK_EQUAL_(deltaTow,         gpsTimingData2.DeltaTow());
    CHECK_EQUAL_(gpsRefTimeUncert, gpsTimingData2.GpsReferenceTimeUncertainty());

       
    StandardCleanup();
	}


// 4.3.3. Properties with NANs
void CDataClassesStep::SatelliteClasses_Properties2L()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TRealX nan;
    nan.SetNaN();

    TSatelliteData satData;

    const TReal32 dumAzimuth = nan;
    const TReal32 dumElevation = nan;

    satData.SetAzimuth(dumAzimuth);
    satData.SetElevation(dumElevation);

    CHECK(Math::IsNaN(satData.Azimuth()));
    CHECK(Math::IsNaN(satData.Elevation()));

    TPositionSatelliteInfo satInfo;

    const TReal32 dumHorzDoP = nan;
    const TReal32 dumVertDoP = nan;
    const TReal32 dumTimeDoP = nan;

    satInfo.SetHorizontalDoP(dumHorzDoP);
    satInfo.SetVerticalDoP(dumVertDoP);
    satInfo.SetTimeDoP(dumTimeDoP);

    CHECK(Math::IsNaN(satInfo.HorizontalDoP()));
    CHECK(Math::IsNaN(satInfo.VerticalDoP()));
    CHECK(Math::IsNaN(satInfo.TimeDoP()));

    StandardCleanup();
	}


// 4.3.4.   TPositionSatelliteInfo accepts TSatelliteData
void CDataClassesStep::SatelliteClasses_InfoL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TSatelliteData satData;

    const TInt dumSatelliteId = 11111;
    const TInt dumSignalStrength = 22222;
    const TReal32 dumAzimuth = TReal32(111.111);
    const TReal32 dumElevation = TReal32(222.222);

    satData.SetSignalStrength(dumSignalStrength);
    satData.SetSatelliteId(dumSatelliteId);
    satData.SetAzimuth(dumAzimuth);
    satData.SetElevation(dumElevation);
    satData.SetIsUsed(ETrue);

    TPositionSatelliteInfo satInfo;
    TSatelliteData satData2;

    // append data
    satInfo.AppendSatelliteData(satData);

    CHECK_EQUAL_(1,        satInfo.NumSatellitesInView());                //Have added 1 satellite
    CHECK_EQUAL_(KErrNone, satInfo.GetSatelliteData(0, satData2));

    // check data
    CHECK_EQUAL_(dumSatelliteId,     satData2.SatelliteId());
    CHECK_EQUAL_(dumSignalStrength,  satData2.SignalStrength());
    CHECK_REAL_EQUAL(dumAzimuth,    satData2.Azimuth());
    CHECK_REAL_EQUAL(dumElevation,  satData2.Elevation());
    CHECK(satData2.IsUsed());

    StandardCleanup();
	}


//
//  Satellites list
//

// 4.4.1. Satellite data list integrity
void CDataClassesStep::SatelliteClasses_SatelliteInfoL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TSatelliteData satData, satData2;
    TPositionSatelliteInfo satInfo;

    // append data
    for (TInt ii = 0; (TUint)ii < KPositionMaxSatellitesInView; ii++)
    	{
        TBool isUsed = ( ii % 2 != 0 );

        satData.SetIsUsed( isUsed );

        CHECK(KErrNotFound == satInfo.GetSatelliteData(ii, satData2));
        CHECK_EQUAL_(KErrNone, satInfo.AppendSatelliteData(satData));

        CHECK_EQUAL_(ii + 1,   satInfo.NumSatellitesInView());                //Have added 1 satellite
        CHECK_EQUAL_((ii+1)/2, satInfo.NumSatellitesUsed());

        CHECK_EQUAL_(KErrNone, satInfo.GetSatelliteData(ii, satData2));
        CHECK_EQUAL_(isUsed,   satData2.IsUsed());
    	}

    satInfo.ClearSatellitesInView();
    CHECK_EQUAL_(KErrNotFound, satInfo.GetSatelliteData(0, satData2));
    CHECK_EQUAL_(0, satInfo.NumSatellitesInView());
    CHECK_EQUAL_(0, satInfo.NumSatellitesUsed());

    StandardCleanup();
	}

// 4.4.2. Too much SatelliteData is added
void CDataClassesStep::SatelliteClasses_OverflowL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TPositionSatelliteInfo satInfo;
    TSatelliteData satData;
    for (TInt i=0; i < static_cast<TInt>(KPositionMaxSatellitesInView); i++)
        {
        satData.SetSatelliteId(i);
        CHECK_EQUAL_(KErrNone, satInfo.AppendSatelliteData(satData));
        }

    CHECK_EQUAL_(KErrOverflow, satInfo.AppendSatelliteData(satData));

    StandardCleanup();
	}
void CDataClassesStep::GpsMeasurementClasses_PropertiesL()
     {
     DECLARE_ERROR_LOGGING;
     StandardPrepareL();
     
     // Check the size of the TPositionGpsMeasurementInfo class is unchanged
     CHECK_EQUAL_(KPositionGpsMeasurementInfoSize, sizeof(TPositionGpsMeasurementInfo));
     
     TPositionGpsMeasurementInfo gpsMeasurementInfo;
     TGpsTimingMeasurementData gpsTimingData;
     
     // all max values for UTran/Fdd 
     const TInt dumGpsTocMs  = (TInt) 16383;
     const TUint dumGpsTocLs = (TUint) 4294967295UL; 
     const TInt dumRefId     = (TInt) 511;
     const TInt dumSfn       = (TInt) 4095;
     
     gpsTimingData.SetDataType(TGpsTimingMeasurementData::EGpsTimingDataTypeUtran);
     gpsTimingData.SetNetworkMode(TGpsTimingMeasurementData::ENetworkModeFdd);
     gpsTimingData.SetGPSTimingOfCellMsPart(dumGpsTocMs);
     gpsTimingData.SetGPSTimingOfCellLsPart(dumGpsTocLs);
     gpsTimingData.SetReferenceIdentity(dumRefId);
     gpsTimingData.SetSfn(dumSfn);
     
     gpsMeasurementInfo.SetGpsTimingData(gpsTimingData);
     
     TGpsTimingMeasurementData gpsTimingDataReturned;
     CHECK_EQUAL_(KErrNone, gpsMeasurementInfo.GetGpsTimingData(gpsTimingDataReturned));
     
     CHECK_EQUAL_(TGpsTimingMeasurementData::EGpsTimingDataTypeUtran, gpsTimingData.DataType());
     CHECK_EQUAL_(TGpsTimingMeasurementData::ENetworkModeFdd, gpsTimingData.NetworkMode());
     CHECK_EQUAL_(dumGpsTocMs, gpsTimingDataReturned.GPSTimingOfCellMsPart());
     CHECK_EQUAL_(dumGpsTocLs, gpsTimingDataReturned.GPSTimingOfCellLsPart());
     CHECK_EQUAL_(dumRefId,    gpsTimingDataReturned.ReferenceIdentity());
     CHECK_EQUAL_(dumSfn,      gpsTimingDataReturned.Sfn());
     StandardCleanup();
     }

