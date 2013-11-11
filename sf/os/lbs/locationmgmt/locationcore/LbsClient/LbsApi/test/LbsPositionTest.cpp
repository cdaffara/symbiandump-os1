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


#include "unittest.h"
#include "testregistry.h"
#include "test.h"
#include <LbsPosition.h>
#include <LbsErrors.h>
#include <E32Math.h>

#define ARRAYLENGTH(aArray) (static_cast<TInt>((sizeof(aArray) / sizeof(aArray[0]))))


const TReal64 KErrorToleranceValue = 0.00001;

inline TReal RealAbs(const TReal& aVal)
	{
	return Abs<TReal>(aVal);
	}

//
// TCoordinate Tests
//

TEST(TCoordinate, BlankConstruction, "COOR", 1)
	{
	TCoordinate coordinate;
	CHECK(Math::IsNaN(coordinate.Latitude()));
	CHECK(Math::IsNaN(coordinate.Longitude()));
	CHECK(Math::IsNaN(coordinate.Altitude()));
	CHECK_EQUAL(KPositionDatumWgs84.iUid, coordinate.Datum().iUid);
	}

TEST(TCoordinate, Construction_2_parameters, "COOR", 1)
	{
	const TReal64 KDummyValue1 = 36.756;
	const TReal64 KDummyValue2 = 123.546;

	TCoordinate coordinate(KDummyValue1, KDummyValue2);
	CHECK(RealAbs(coordinate.Latitude()-KDummyValue1)<KErrorToleranceValue);
	CHECK(RealAbs(coordinate.Longitude()-KDummyValue2)<KErrorToleranceValue);
	CHECK(Math::IsNaN(coordinate.Altitude()));
	CHECK_EQUAL(KPositionDatumWgs84.iUid, coordinate.Datum().iUid);
	}

TEST(TCoordinate, Construction_3_parameters, "COOR", 1)
	{
	const TReal64 KDummyValue1 = 36.756;
	const TReal64 KDummyValue2 = 123.546;
	const TReal32 KDummyValue3 = TReal32(74.263);

	TCoordinate coordinate(KDummyValue1, KDummyValue2, KDummyValue3);

	CHECK(RealAbs(coordinate.Latitude()-KDummyValue1)<KErrorToleranceValue);
	CHECK(RealAbs(coordinate.Longitude()-KDummyValue2)<KErrorToleranceValue);
	CHECK(RealAbs(coordinate.Altitude()-KDummyValue3)<KErrorToleranceValue);
	CHECK_EQUAL(KPositionDatumWgs84.iUid, coordinate.Datum().iUid);
	}

TEST(TCoordinate, SetCoordinate_2_parameters, "COOR", 2)
	{
	const TReal64 KDummyValue1 = 36.756;
	const TReal64 KDummyValue2 = 123.546;
	const TReal32 KDummyValue3 = TReal32(74.263);

	TCoordinate coordinate(KDummyValue1, KDummyValue2, KDummyValue3);

	const TReal64 KDummyValue4 = 15.54;
	const TReal64 KDummyValue5 = 23.2345;
	coordinate.SetCoordinate(KDummyValue4, KDummyValue5);

	CHECK(RealAbs(coordinate.Latitude()-KDummyValue4)<KErrorToleranceValue);
	CHECK(RealAbs(coordinate.Longitude()-KDummyValue5)<KErrorToleranceValue);
	CHECK(Math::IsNaN(coordinate.Altitude()));
	CHECK_EQUAL(KPositionDatumWgs84.iUid, coordinate.Datum().iUid);
	}

TEST(TCoordinate, SetCoordinate_3_parameters, "COOR", 2)
	{
	const TReal64 KDummyValue1 = 36.756;
	const TReal64 KDummyValue2 = 123.546;
	const TReal32 KDummyValue3 = TReal32(74.263);
	
	TCoordinate coordinate;
	coordinate.SetCoordinate(KDummyValue1, KDummyValue2, KDummyValue3);

	CHECK(RealAbs(coordinate.Latitude()-KDummyValue1)<KErrorToleranceValue);
	CHECK(RealAbs(coordinate.Longitude()-KDummyValue2)<KErrorToleranceValue);
	CHECK(RealAbs(coordinate.Altitude()-KDummyValue3)<KErrorToleranceValue);
	CHECK_EQUAL(KPositionDatumWgs84.iUid, coordinate.Datum().iUid);
	}

TEST(TCoordinate, SetDatum, "COOR", 3)
	{
	const TPositionDatumId KDummyDatum = {0x91110022};
	TCoordinate coordinate;
	coordinate.SetDatum(KDummyDatum);
	CHECK_EQUAL(KDummyDatum.iUid, coordinate.Datum().iUid);
	}

TEST(TCoordinate, Normalize_CheckBounds, "COOR", 4)
	{
	// Check that any values of lon, lat passed to the constructor that
	// are outside the valid are wrapped to the valid range

	const TReal KOffset = 0.25;

	for (TReal lon = -10000.0; lon <= 10000.0; lon += KOffset)
		{
		TCoordinate coord(0.0, lon);
		CHECK( coord.Longitude() <= 180.0 );
		CHECK( coord.Longitude() >= -180.0 );
		}

	for (TReal lat = -10000.0; lat <= 10000.0; lat += KOffset)
		{
		TCoordinate coord(lat, 0.0);
		CHECK( coord.Latitude() <= 90.0 );
		CHECK( coord.Latitude() >= -90.0 );
		}
	}

TEST(TCoordinate, Normalize_ValidLatitude, "COOR", 5)
	{
	// Check that a valid latitude does not alter the longitude of the
	// coordinate

	const TReal KOffset = 0.125;
	TReal lat;

	for (lat = -90.0; lat <= 90.0; lat += KOffset)
		{
		const TReal KLon = 0.0;
		TCoordinate coord(lat, KLon);
		CHECK_EQUAL( KLon, coord.Longitude() );
		}
	}

TEST(TCoordinate, Normalize_InvalidLatitude, "COOR", 6)
	{
	// Check that an invalid latitude that refers to a position on the other
	// side of the globe alters the longitude of the coordinate by 180 degrees, whilst
	// one that is in the -90 to 90 range does not alter the longitude.

	const TReal KLon = 0.0;
	const TReal KWrappedLon = -180.0;
	const TReal KOffset = 0.125;
	TReal lat;

	for (lat = -810.0 + KOffset; lat < -630.0; lat += KOffset)
		{
		TCoordinate coord(lat, KLon);
		CHECK_EQUAL( KLon, coord.Longitude() );
		}

	for (lat = -630.0 + KOffset; lat < -450.0; lat += KOffset)
		{
		TCoordinate coord(lat, KLon);
		CHECK_EQUAL( KWrappedLon, coord.Longitude() );
		}

	for (lat = -450.0 + KOffset; lat < -270.0; lat += KOffset)
		{
		TCoordinate coord(lat, KLon);
		CHECK_EQUAL( KLon, coord.Longitude() );
		}

	for (lat = -270.0 + KOffset; lat < -90.0; lat += KOffset)
		{
		TCoordinate coord(lat, KLon);
		CHECK_EQUAL( KWrappedLon, coord.Longitude() );
		}

	// -90 to 90 is valid

	for (lat = 90 + KOffset; lat < 270.0; lat += KOffset)
		{
		TCoordinate coord(lat, KLon);
		CHECK_EQUAL( KWrappedLon, coord.Longitude() );
		}

	for (lat = 270.0 + KOffset; lat < 450.0; lat += KOffset)
		{
		TCoordinate coord(lat, KLon);
		CHECK_EQUAL( KLon, coord.Longitude() );
		}

	for (lat = 450.0 + KOffset; lat < 630.0; lat += KOffset)
		{
		TCoordinate coord(lat, KLon);
		CHECK_EQUAL( KWrappedLon, coord.Longitude() );
		}

	for (lat = 630.0 + KOffset; lat < 810.0; lat += KOffset)
		{
		TCoordinate coord(lat, KLon);
		CHECK_EQUAL( KLon, coord.Longitude() );
		}
	}

TEST(TCoordinate, Normalize_LatLonInteraction, "COOR", 7)
	{
	// Check that when both longitude and latitude are wrapped, the resulting
	// coordinate is as expected.

	struct TLatLon { TReal iLat, iLon; };
	struct TLatLonPair { TLatLon iIn, iOut; }; // Input and expected output

#define LAT_LON(aLat, aLon)  { aLat, aLon }
#define DECL_TEST(aLat, aLon)  { aLat, aLon },

	// This is static, because otherwise ~70 rows is ~2.5K of (stack) memory.
	static const TLatLonPair KTestValues[] =
		{
		                // Input                  // Expected output
		DECL_TEST( LAT_LON(   0.0,    0.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(  90.0,  180.0),  LAT_LON(  90.0, -180.0) )
		DECL_TEST( LAT_LON( -90.0,  180.0),  LAT_LON( -90.0, -180.0) )
		DECL_TEST( LAT_LON(  90.0, -180.0),  LAT_LON(  90.0, -180.0) )
		DECL_TEST( LAT_LON( -90.0, -180.0),  LAT_LON( -90.0, -180.0) )

		DECL_TEST( LAT_LON(  91.0,  180.0),  LAT_LON(  89.0,    0.0) )
		DECL_TEST( LAT_LON(  92.0,  180.0),  LAT_LON(  88.0,    0.0) )
		DECL_TEST( LAT_LON( -91.0,  180.0),  LAT_LON( -89.0,    0.0) )
		DECL_TEST( LAT_LON( -92.0,  180.0),  LAT_LON( -88.0,    0.0) )

		DECL_TEST( LAT_LON(   0.0,  181.0),  LAT_LON(   0.0, -179.0) )
		DECL_TEST( LAT_LON(   0.0,  182.0),  LAT_LON(   0.0, -178.0) )
		DECL_TEST( LAT_LON(   0.0, -181.0),  LAT_LON(   0.0,  179.0) )
		DECL_TEST( LAT_LON(   0.0, -182.0),  LAT_LON(   0.0,  178.0) )

		DECL_TEST( LAT_LON( 180.0,    0.0),  LAT_LON(   0.0, -180.0) )
		DECL_TEST( LAT_LON( 360.0,    0.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON( 540.0,    0.0),  LAT_LON(   0.0, -180.0) )
		DECL_TEST( LAT_LON( 720.0,    0.0),  LAT_LON(   0.0,    0.0) )

		DECL_TEST( LAT_LON(-180.0,    0.0),  LAT_LON(   0.0, -180.0) )
		DECL_TEST( LAT_LON(-360.0,    0.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(-540.0,    0.0),  LAT_LON(   0.0, -180.0) )
		DECL_TEST( LAT_LON(-720.0,    0.0),  LAT_LON(   0.0,    0.0) )

		DECL_TEST( LAT_LON( 180.0,   45.0),  LAT_LON(   0.0, -135.0) )
		DECL_TEST( LAT_LON( 360.0,   45.0),  LAT_LON(   0.0,   45.0) )
		DECL_TEST( LAT_LON( 540.0,   45.0),  LAT_LON(   0.0, -135.0) )
		DECL_TEST( LAT_LON( 720.0,   45.0),  LAT_LON(   0.0,   45.0) )

		DECL_TEST( LAT_LON(-180.0,   45.0),  LAT_LON(   0.0, -135.0) )
		DECL_TEST( LAT_LON(-360.0,   45.0),  LAT_LON(   0.0,   45.0) )
		DECL_TEST( LAT_LON(-540.0,   45.0),  LAT_LON(   0.0, -135.0) )
		DECL_TEST( LAT_LON(-720.0,   45.0),  LAT_LON(   0.0,   45.0) )

		DECL_TEST( LAT_LON( 180.0,  -45.0),  LAT_LON(   0.0,  135.0) )
		DECL_TEST( LAT_LON( 360.0,  -45.0),  LAT_LON(   0.0,  -45.0) )
		DECL_TEST( LAT_LON( 540.0,  -45.0),  LAT_LON(   0.0,  135.0) )
		DECL_TEST( LAT_LON( 720.0,  -45.0),  LAT_LON(   0.0,  -45.0) )

		DECL_TEST( LAT_LON(-180.0,  -45.0),  LAT_LON(   0.0,  135.0) )
		DECL_TEST( LAT_LON(-360.0,  -45.0),  LAT_LON(   0.0,  -45.0) )
		DECL_TEST( LAT_LON(-540.0,  -45.0),  LAT_LON(   0.0,  135.0) )
		DECL_TEST( LAT_LON(-720.0,  -45.0),  LAT_LON(   0.0,  -45.0) )

		DECL_TEST( LAT_LON( 359.0,    0.0),  LAT_LON(  -1.0,    0.0) )
		DECL_TEST( LAT_LON( 360.0,    0.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON( 361.0,    0.0),  LAT_LON(   1.0,    0.0) )
		DECL_TEST( LAT_LON( 719.0,    0.0),  LAT_LON(  -1.0,    0.0) )
		DECL_TEST( LAT_LON( 720.0,    0.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON( 721.0,    0.0),  LAT_LON(   1.0,    0.0) )

		DECL_TEST( LAT_LON(   0.0,  359.0),  LAT_LON(   0.0,   -1.0) )
		DECL_TEST( LAT_LON(   0.0,  360.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(   0.0,  361.0),  LAT_LON(   0.0,    1.0) )
		DECL_TEST( LAT_LON(   0.0,  719.0),  LAT_LON(   0.0,   -1.0) )
		DECL_TEST( LAT_LON(   0.0,  720.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(   0.0,  721.0),  LAT_LON(   0.0,    1.0) )

	// Wrapping co-ords to 0, 0
		DECL_TEST( LAT_LON( 360.0,  360.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON( 720.0,  720.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(1080.0, 1080.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(1440.0, 1440.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(1800.0, 1800.0),  LAT_LON(   0.0,    0.0) )

		DECL_TEST( LAT_LON(-360.0, -360.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(-720.0, -720.0),  LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(-1080.0,-1080.0), LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(-1440.0,-1440.0), LAT_LON(   0.0,    0.0) )
		DECL_TEST( LAT_LON(-1800.0,-1800.0), LAT_LON(   0.0,    0.0) )

	// Wrapping both co-ords
		DECL_TEST( LAT_LON(  89.0,  181.0),  LAT_LON(  89.0, -179.0) )
		DECL_TEST( LAT_LON(  90.0,  181.0),  LAT_LON(  90.0, -179.0) )
		DECL_TEST( LAT_LON(  91.0,  181.0),  LAT_LON(  89.0,    1.0) )
		DECL_TEST( LAT_LON(  92.0,  181.0),  LAT_LON(  88.0,    1.0) )
		DECL_TEST( LAT_LON( -89.0,  181.0),  LAT_LON( -89.0, -179.0) )
		DECL_TEST( LAT_LON( -90.0,  181.0),  LAT_LON( -90.0, -179.0) )
		DECL_TEST( LAT_LON( -91.0,  181.0),  LAT_LON( -89.0,    1.0) )
		DECL_TEST( LAT_LON( -92.0,  181.0),  LAT_LON( -88.0,    1.0) )

		DECL_TEST( LAT_LON(  89.0, -181.0),  LAT_LON(  89.0,  179.0) )
		DECL_TEST( LAT_LON(  90.0, -181.0),  LAT_LON(  90.0,  179.0) )
		DECL_TEST( LAT_LON(  91.0, -181.0),  LAT_LON(  89.0,   -1.0) )
		DECL_TEST( LAT_LON(  92.0, -181.0),  LAT_LON(  88.0,   -1.0) )
		DECL_TEST( LAT_LON( -89.0, -181.0),  LAT_LON( -89.0,  179.0) )
		DECL_TEST( LAT_LON( -90.0, -181.0),  LAT_LON( -90.0,  179.0) )
		DECL_TEST( LAT_LON( -91.0, -181.0),  LAT_LON( -89.0,   -1.0) )
		DECL_TEST( LAT_LON( -92.0, -181.0),  LAT_LON( -88.0,   -1.0) )

		DECL_TEST( LAT_LON( 450.0,  540.0),  LAT_LON(  90.0, -180.0) )
		DECL_TEST( LAT_LON(-450.0,  540.0),  LAT_LON( -90.0, -180.0) )

		};

#define INPUT_LAT(aIndex)     KTestValues[aIndex].iIn.iLat
#define INPUT_LON(aIndex)     KTestValues[aIndex].iIn.iLon
#define EXPECTED_LAT(aIndex)  KTestValues[aIndex].iOut.iLat
#define EXPECTED_LON(aIndex)  KTestValues[aIndex].iOut.iLon

	TCoordinate coord;

	for (TInt i=0; i<ARRAYLENGTH(KTestValues); i++)
		{
		coord.SetCoordinate (INPUT_LAT(i), INPUT_LON(i));

		CHECK_EQUAL( EXPECTED_LAT(i), coord.Latitude() );
		CHECK_EQUAL( EXPECTED_LON(i), coord.Longitude() );
		}
	}

TEST(TCoordinate, Normalize_LatLonInteraction2, "COOR", 8)
	{
	//Check that if you have a NaN Longitude and you have to normalise the coordinate
	//that you don't get a panic...
	
	TCoordinate coordinate;
	TRealX nan;
	nan.SetNaN();
	TReal64 nan64(nan);
	TReal64 latitudeOver90 = 95;
	coordinate.SetCoordinate(latitudeOver90, nan64);
	}

TEST(TCoordinate, Distance, "CODB", 1)
	{
	TCoordinate coordinate;
	TReal32 dummyReal32;
	CHECK_EQUAL(KErrNotSupported, coordinate.Distance(TCoordinate(), dummyReal32));
	}

TEST(TCoordinate, BearingTo, "CODB", 1)
	{
	TCoordinate coordinate;
	TReal32 dummyReal32;
	CHECK_EQUAL(KErrNotSupported, coordinate.BearingTo(TCoordinate(), dummyReal32));
	}

TEST(TCoordinate, Move, "CODB", 1)
	{
	TCoordinate coordinate;
	TReal32 dummyReal1(0);
	TReal32 dummyReal2(0);
	CHECK_EQUAL(KErrNotSupported, coordinate.Move(dummyReal1, dummyReal2));
	}

// To do, add test code for bearing and speed for real method.

//
// TLocality Tests
//

TEST(TLocality, Blank_Construction, "LOCA", 1)
	{
	TLocality locality;
	CHECK(Math::IsNaN(locality.HorizontalAccuracy()));
	CHECK(Math::IsNaN(locality.VerticalAccuracy()));
	CHECK(Math::IsNaN(locality.Latitude()));
	CHECK(Math::IsNaN(locality.Longitude()));
	CHECK(Math::IsNaN(locality.Altitude()));
	}

TEST(TLocality, Construction_1_Accuracy_Parameter, "LOCA", 1)
	{
	const TReal KDummyLatitude = 34.2342;
	const TReal KDummyLongitude = 113.342;
	const TReal32 KDummyAltitude = (TReal32)123.2243;

	const TReal32 KDummyAccuracy = (TReal32)34.34534;
	TLocality locality(TCoordinate(KDummyLatitude, KDummyLongitude, KDummyAltitude), KDummyAccuracy);

	CHECK(RealAbs(locality.Latitude()-KDummyLatitude)<KErrorToleranceValue);
	CHECK(RealAbs(locality.Longitude()-KDummyLongitude)<KErrorToleranceValue);
	CHECK(RealAbs(locality.Altitude()-KDummyAltitude)<KErrorToleranceValue);

	CHECK(RealAbs(locality.HorizontalAccuracy()-KDummyAccuracy)<KErrorToleranceValue);
	CHECK(Math::IsNaN(locality.VerticalAccuracy()));

	// Test NaN is acceptable

	TRealX nan;
	nan.SetNaN();
	TLocality locality2(TCoordinate(KDummyLatitude, KDummyLongitude, KDummyAltitude), nan);
	CHECK(Math::IsNaN(locality2.HorizontalAccuracy()));
	CHECK(Math::IsNaN(locality2.VerticalAccuracy()));
	}

TEST(TLocality, Construction_2_Accuracy_Parameters, "LOCA", 1)
	{
	const TReal32 KDummyAccuracy1 = (TReal32)34.34534;
	const TReal32 KDummyAccuracy2 = (TReal32)564.45645;
	TCoordinate dummyCoordinate;
	TLocality locality(dummyCoordinate, KDummyAccuracy1, KDummyAccuracy2);

	CHECK(RealAbs(locality.HorizontalAccuracy()-KDummyAccuracy1)<KErrorToleranceValue);
	CHECK(RealAbs(locality.VerticalAccuracy()-KDummyAccuracy2)<KErrorToleranceValue);

	// Test NaN is acceptable

	TRealX nan;
	nan.SetNaN();
	TLocality locality2(dummyCoordinate, nan, nan);
	CHECK(Math::IsNaN(locality2.HorizontalAccuracy()));
	CHECK(Math::IsNaN(locality2.VerticalAccuracy()));
	}

TInt Construct_BadHorizontalAccuracy1(TAny* /*aPtr*/)
	{
	TCoordinate dummyCoordinate;
	TLocality locality(
		dummyCoordinate,
		TReal(-1) // This should cause a panic
		);
	(void) locality;
	return KErrGeneral;
	}

TEST(TLocality, Construct_BadHorizontalAccuracy1, "LOCA", 1)
	{
	DO_PANIC_TEST_L(Construct_BadHorizontalAccuracy1, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TInt Construct_BadHorizontalAccuracy2(TAny* /*aPtr*/)
	{
	TCoordinate dummyCoordinate;
	TLocality locality(
		dummyCoordinate,
		TReal(-1), // This should cause a panic
		TReal(554)
		);
	(void) locality;
	return KErrGeneral;
	}

TEST(TLocality, Construct_BadHorizontalAccuracy2, "LOCA", 1)
	{
	DO_PANIC_TEST_L(Construct_BadHorizontalAccuracy2, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TInt Construct_BadVerticalAccuracy(TAny* /*aPtr*/)
	{
	TCoordinate dummyCoordinate;
	TLocality locality(
		dummyCoordinate,
		TReal(456),
		TReal(-1) // This should cause a panic
		);
	(void) locality;
	return KErrGeneral;
	}

TEST(TLocality, Construct_BadVerticalAccuracy, "LOCA", 1)
	{
	DO_PANIC_TEST_L(Construct_BadVerticalAccuracy, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TEST(TLocality, SetHorizontalAccuracy, "LOCA", 2)
	{
	const TReal32 KDummyAccuracy1 = (TReal32)34.34534;
	const TReal32 KDummyAccuracy2 = (TReal32)564.45645;
	TCoordinate dummyCoordinate;
	TLocality locality(dummyCoordinate, KDummyAccuracy1, KDummyAccuracy2);

	const TReal32 KDummyAccuracy3 = (TReal32)234.435;
	locality.SetHorizontalAccuracy(KDummyAccuracy3);

	CHECK(RealAbs(locality.HorizontalAccuracy()-KDummyAccuracy3)<KErrorToleranceValue);
	CHECK(RealAbs(locality.VerticalAccuracy()-KDummyAccuracy2)<KErrorToleranceValue);

	TRealX nan;
	nan.SetNaN();

	locality.SetHorizontalAccuracy(nan);
	CHECK(Math::IsNaN(locality.HorizontalAccuracy()));
	}

TEST(TLocality, SetVerticalAccuracy, "LOCA", 2)
	{
	const TReal32 KDummyAccuracy1 = (TReal32)34.34534;
	const TReal32 KDummyAccuracy2 = (TReal32)564.45645;
	TCoordinate dummyCoordinate;
	TLocality locality(dummyCoordinate, KDummyAccuracy1, KDummyAccuracy2);

	const TReal32 KDummyAccuracy3 = (TReal32)234.435;
	locality.SetVerticalAccuracy(KDummyAccuracy3);

	CHECK(RealAbs(locality.HorizontalAccuracy()-KDummyAccuracy1)<KErrorToleranceValue);
	CHECK(RealAbs(locality.VerticalAccuracy()-KDummyAccuracy3)<KErrorToleranceValue);

	TRealX nan;
	nan.SetNaN();

	locality.SetVerticalAccuracy(nan);
	CHECK(Math::IsNaN(locality.VerticalAccuracy()));
	}

TEST(TLocality, SetAccuracy, "LOCA", 2)
	{
	TLocality locality;
	const TReal32 KDummyAccuracy1 = (TReal32)34.34534;
	const TReal32 KDummyAccuracy2 = (TReal32)564.45645;

	locality.SetAccuracy(KDummyAccuracy1, KDummyAccuracy2);

	CHECK(RealAbs(locality.HorizontalAccuracy()-KDummyAccuracy1)<KErrorToleranceValue);
	CHECK(RealAbs(locality.VerticalAccuracy()-KDummyAccuracy2)<KErrorToleranceValue);

	TRealX nan;
	nan.SetNaN();

	locality.SetAccuracy(nan, nan);
	CHECK(Math::IsNaN(locality.VerticalAccuracy()));
	CHECK(Math::IsNaN(locality.HorizontalAccuracy()));
	}

TInt SetHorizontalAccuracy_Invalid(TAny* /*aPtr*/)
	{
	TLocality locality;
	locality.SetHorizontalAccuracy(TReal32(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TLocality, SetHorizontalAccuracy_Invalid, "LOCA", 3)
	{
	DO_PANIC_TEST_L(SetHorizontalAccuracy_Invalid, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TInt SetVerticalAccuracy_Invalid(TAny* /*aPtr*/)
	{
	TLocality locality;
	locality.SetVerticalAccuracy(TReal32(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TLocality, SetVerticalAccuracy_Invalid, "LOCA", 4)
	{
	DO_PANIC_TEST_L(SetVerticalAccuracy_Invalid, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TInt SetAccuracyParam1_Invalid(TAny* /*aPtr*/)
	{
	TLocality locality;
	const TReal32 KDummyAccuracy1 = (TReal32)34.34534;
	locality.SetAccuracy(TReal32(-1), KDummyAccuracy1); // This should panic
	return KErrGeneral;
	}

TEST(TLocality, SetAccuracyParam1_Invalid, "LOCA", 5)
	{
	DO_PANIC_TEST_L(SetAccuracyParam1_Invalid, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TInt SetAccuracyParam2_Invalid(TAny* /*aPtr*/)
	{
	TLocality locality;
	const TReal32 KDummyAccuracy1 = (TReal32)34.34534;
	locality.SetAccuracy(KDummyAccuracy1, TReal32(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TLocality, SetAccuracyParam2_Invalid, "LOCA", 5)
	{
	DO_PANIC_TEST_L(SetAccuracyParam2_Invalid, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TEST(TLocality, Distance_No_Delta, "LODB", 1)
	{
	TLocality locality;
	TReal32 dummyDistance;
	CHECK_EQUAL(KErrNotSupported, locality.Distance(TCoordinate(), dummyDistance));
	}

TEST(TLocality, Distance_With_Delta, "LODB", 1)
	{
	TLocality locality, locality2;
	TReal32 calculatedDistance;
	TReal32 dummyDelta;	

	CHECK_EQUAL(KErrNotSupported, locality.Distance(locality2, calculatedDistance, dummyDelta));
	}

TEST(TLocality, Bearing_No_Delta, "LODB", 1)
	{
	TLocality locality, locality2;
	TReal32 calculatedBearing;

	CHECK_EQUAL(KErrNotSupported, locality.BearingTo(locality2, calculatedBearing));
	}

TEST(TLocality, Bearing_With_Delta, "LODB", 1)
	{
	TLocality locality, locality2;
	TReal32 calculatedBearing;
	TReal32 dummyDelta;	

	CHECK_EQUAL(KErrNotSupported, locality.BearingTo(locality2, calculatedBearing, dummyDelta));
	}

// To do, Add Distance and Bearing tests for real methods.

//
// TPosition Tests
//

TEST(TPosition, Blank_Construction, "PNTI", 1)
	{
	TPosition position;
	
	CHECK_EQUAL(TInt64(0), position.Time().Int64());
	CHECK(Math::IsNaN(position.HorizontalAccuracy()));
	CHECK(Math::IsNaN(position.VerticalAccuracy()));
	CHECK(Math::IsNaN(position.Latitude()));
	CHECK(Math::IsNaN(position.Longitude()));
	CHECK(Math::IsNaN(position.Altitude()));
	}

TEST(TPosition, Full_Construction, "PNTI", 1)
	{
	const TReal KDummyLatitude = 34.2342;
	const TReal KDummyLongitude = 113.342;
	const TReal32 KDummyAltitude = (TReal32)123.2243;

	const TReal32 KDummyAccuracy1 = (TReal32)34.34534;
	const TReal32 KDummyAccuracy2 = (TReal32)354.6988;
	TLocality locality(TCoordinate(KDummyLatitude, KDummyLongitude, KDummyAltitude), KDummyAccuracy1, KDummyAccuracy2);


	const TInt64 KDummyTimeValue = 3424455;
	TPosition position(locality, TTime(KDummyTimeValue));

	CHECK(RealAbs(position.Latitude()-KDummyLatitude)<KErrorToleranceValue);
	CHECK(RealAbs(position.Longitude()-KDummyLongitude)<KErrorToleranceValue);
	CHECK(RealAbs(position.Altitude()-KDummyAltitude)<KErrorToleranceValue);

	CHECK(RealAbs(position.HorizontalAccuracy()-KDummyAccuracy1)<KErrorToleranceValue);
	CHECK(RealAbs(position.VerticalAccuracy()-KDummyAccuracy2)<KErrorToleranceValue);

	CHECK_EQUAL(KDummyTimeValue, position.Time().Int64());
	}

TEST(TPosition, Set_Time, "PNTI", 2)
	{
	TPosition position;
	const TInt64 KDummyTimeValue = 34532534;
	position.SetTime(TTime(KDummyTimeValue));
	CHECK_EQUAL(KDummyTimeValue, position.Time().Int64());
	}

TEST(TPosition, Set_Current_Time, "PNTI", 3)
	{
	TPosition position;
	TTime time;
	time.UniversalTime();
	position.SetCurrentTime();
	CHECK((TInt64)0!=position.Time().Int64());	//Check the time has changed
	CHECK(position.Time()>=time);
	const TTimeIntervalMicroSeconds KTimeGranularityMicroSeconds(TInt64(2000));
	CHECK(position.Time().MicroSecondsFrom(time)<=KTimeGranularityMicroSeconds);
	}

TEST(TPosition, Speed_No_Delta, "PNSC", 1)
	{
	TPosition position;
	TReal32 dummyReal32;
	CHECK_EQUAL(KErrNotSupported, position.Speed(TPosition(), dummyReal32));
	}

TEST(TPosition, Speed_With_Delta, "PNSC", 1)
	{
	TPosition position;
	TReal32 dummyReal32;
	TReal32 anotherDummyReal32;
	CHECK_EQUAL(KErrNotSupported, position.Speed(TPosition(), dummyReal32, anotherDummyReal32));
	}

//To do. Add more TPosition::Speed tests

//
// TCourse Tests
//

TEST(TCourse, Construction, "CORS",  1)
	{
	TCourse course;
	CHECK(Math::IsNaN(course.Speed()));
	CHECK(Math::IsNaN(course.Heading()));
	CHECK(Math::IsNaN(course.SpeedAccuracy()));
	CHECK(Math::IsNaN(course.HeadingAccuracy()));
	}

TEST(TCourse, Getter_And_Setters, "CORS", 2)
	{
	const TReal32 KDummyValue1 = (TReal32)43.342;
	const TReal32 KDummyValue2 = (TReal32)34.5236;
	const TReal32 KDummyValue3 = (TReal32)983.45;
	const TReal32 KDummyValue4 = (TReal32)12.432;

	TCourse course;
	course.SetSpeed(KDummyValue1);
	course.SetHeading(KDummyValue2);
	course.SetSpeedAccuracy(KDummyValue3);
	course.SetHeadingAccuracy(KDummyValue4);

	CHECK(RealAbs(course.Speed()-KDummyValue1)<KErrorToleranceValue);
	CHECK(RealAbs(course.Heading()-KDummyValue2)<KErrorToleranceValue);
	CHECK(RealAbs(course.SpeedAccuracy()-KDummyValue3)<KErrorToleranceValue);
	CHECK(RealAbs(course.HeadingAccuracy()-KDummyValue4)<KErrorToleranceValue);

	TRealX nan;
	nan.SetNaN();

	course.SetSpeedAccuracy(nan);
	CHECK(Math::IsNaN(course.SpeedAccuracy()));

	course.SetHeadingAccuracy(nan);
	CHECK(Math::IsNaN(course.HeadingAccuracy()));
	}

TInt SetSpeedAccuracy_Invalid(TAny* /*aPtr*/)
	{
	TCourse course;
	course.SetSpeedAccuracy(TReal32(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TPositionUpdateOptions, SetSpeedAccuracy_Invalid, "CORS", 1)
	{
	DO_PANIC_TEST_L(SetSpeedAccuracy_Invalid, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TInt SetHeadingAccuracy_Invalid(TAny* /*aPtr*/)
	{
	TCourse course;
	course.SetHeadingAccuracy(TReal32(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TPositionUpdateOptions, SetHeadingAccuracy_Invalid, "CORS", 1)
	{
	DO_PANIC_TEST_L(SetHeadingAccuracy_Invalid, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}
