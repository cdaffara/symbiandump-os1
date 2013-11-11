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
#include <LbsSatellite.h>
#include <E32Math.h>

const TReal64 KErrorToleranceValue = 0.00001;
inline TReal RealAbs(const TReal& aVal)
	{
	return Abs<TReal>(aVal);
	}

//
// TSatelliteData Tests
//

TEST(TSatelliteData, Construction, "SDAT", 1)
	{
	TSatelliteData satData;
	CHECK_EQUAL(-1, satData.SatelliteId());
	CHECK(Math::IsNaN(satData.Azimuth()));
	CHECK(Math::IsNaN(satData.Elevation()));
	CHECK(!satData.IsUsed());
	CHECK_EQUAL(0, satData.SignalStrength());
	}

TEST(TSatelliteData, Getters_And_Setters, "SDAT", 2)
	{
	TSatelliteData satData;

	const TInt KSatelliteId = 12;
	satData.SetSatelliteId(KSatelliteId);
	CHECK_EQUAL(KSatelliteId, satData.SatelliteId());

	const TReal32 KAzimuth = (TReal32)234.2345;
	satData.SetAzimuth(KAzimuth);
	CHECK(RealAbs(KAzimuth-satData.Azimuth())<KErrorToleranceValue);

	const TReal32 KElevation = (TReal32)4745.1231;
	satData.SetElevation(KElevation);
	CHECK(RealAbs(KElevation-satData.Elevation())<KErrorToleranceValue);

	const TBool KOppositeOfDefaultIsUsed = !satData.IsUsed();
	satData.SetIsUsed(KOppositeOfDefaultIsUsed);
	//Done like this so is we change default for IsUsed the test code still works.
	if(KOppositeOfDefaultIsUsed)
		{
		CHECK(satData.IsUsed());
		}
	else
		{
		CHECK(!satData.IsUsed());
		}

	const TInt KSignalStrength = 34262;

	satData.SetSignalStrength(KSignalStrength);
	CHECK_EQUAL(KSignalStrength, satData.SignalStrength());
	}

//
// TPositionSatelliteInfo Tests
//

TEST(TPositionSatelliteInfo, Check_Defaults, "PSIN", 1)
	{
	TPositionSatelliteInfo satInfo;
	TSatelliteData satData;
	CHECK_EQUAL(KErrNotFound, satInfo.GetSatelliteData(0, satData));

	CHECK_EQUAL(0, satInfo.NumSatellitesInView());
	
	CHECK_EQUAL(0, satInfo.NumSatellitesUsed());

	CHECK(TTime(0)==satInfo.SatelliteTime());
	
	CHECK(Math::IsNaN(satInfo.HorizontalDoP()));

	CHECK(Math::IsNaN(satInfo.VerticalDoP()));

	CHECK(Math::IsNaN(satInfo.TimeDoP()));
	}

TEST(TPositionSatelliteInfo, Get_Append_Sat_Data, "PSIN", 2)
	{
	TPositionSatelliteInfo satInfo;
	TSatelliteData satData;

	const TInt KSatellite1Id = 6;
	const TReal32 KSatellite1Azimuth = (TReal32)43.123;
	const TReal32 KSatellite1Elevation = (TReal32)412.45;
	TBool KSatellite1IsUsed = ETrue;
	const TInt KSatellite1SigStrength = 23;

	satData.SetSatelliteId(KSatellite1Id);
	satData.SetAzimuth(KSatellite1Azimuth);
	satData.SetElevation(KSatellite1Elevation);
	satData.SetIsUsed(KSatellite1IsUsed);
	satData.SetSignalStrength(KSatellite1SigStrength);

	satInfo.AppendSatelliteData(satData);

	const TInt KSatellite2Id = 8;
	const TReal32 KSatellite2Azimuth = (TReal32)12.753;
	const TReal32 KSatellite2Elevation = (TReal32)42.45;
	TBool KSatellite2IsUsed = EFalse;
	const TInt KSatellite2SigStrength = 12;

	satData.SetSatelliteId(KSatellite2Id);
	satData.SetAzimuth(KSatellite2Azimuth);
	satData.SetElevation(KSatellite2Elevation);
	satData.SetIsUsed(KSatellite2IsUsed);
	satData.SetSignalStrength(KSatellite2SigStrength);

	satInfo.AppendSatelliteData(satData);


	//Now check data got entered properly.
	CHECK_EQUAL(satInfo.NumSatellitesInView(), 2);	//Have added 2 satellites.
	User::LeaveIfError(satInfo.GetSatelliteData(0, satData));

	CHECK_EQUAL(satData.SatelliteId(), KSatellite1Id);
	CHECK(RealAbs(KSatellite1Azimuth-satData.Azimuth())<KErrorToleranceValue);
	CHECK(RealAbs(KSatellite1Elevation-satData.Elevation())<KErrorToleranceValue);
	if(KSatellite1IsUsed)
		CHECK(satData.IsUsed())
	else
		CHECK(!satData.IsUsed());
	
	CHECK_EQUAL(satData.SignalStrength(), KSatellite1SigStrength);

	//Next satellite
	User::LeaveIfError(satInfo.GetSatelliteData(1, satData));

	CHECK_EQUAL(satData.SatelliteId(), KSatellite2Id);
	CHECK(RealAbs(KSatellite2Azimuth-satData.Azimuth())<KErrorToleranceValue);
	CHECK(RealAbs(KSatellite2Elevation-satData.Elevation())<KErrorToleranceValue);
	if(KSatellite2IsUsed)
		CHECK(satData.IsUsed())
	else
		CHECK(!satData.IsUsed());
	
	CHECK_EQUAL(satData.SignalStrength(), KSatellite2SigStrength);
	}

TEST(TPositionSatelliteInfo, NumSatellitesInView, "PSIN", 3)
	{
	TPositionSatelliteInfo satInfo;
	CHECK_EQUAL(satInfo.NumSatellitesInView(), 0);
	TSatelliteData satData;

	const TInt KSatellite1Id = 6;
	const TReal32 KSatellite1Azimuth = (TReal32)43.123;
	const TReal32 KSatellite1Elevation = (TReal32)412.45;
	TBool KSatellite1IsUsed = ETrue;
	const TInt KSatellite1SigStrength = 23;

	satData.SetSatelliteId(KSatellite1Id);
	satData.SetAzimuth(KSatellite1Azimuth);
	satData.SetElevation(KSatellite1Elevation);
	satData.SetIsUsed(KSatellite1IsUsed);
	satData.SetSignalStrength(KSatellite1SigStrength);

	satInfo.AppendSatelliteData(satData);

	const TInt KSatellite2Id = 8;
	const TReal32 KSatellite2Azimuth = (TReal32)12.753;
	const TReal32 KSatellite2Elevation = (TReal32)42.45;
	TBool KSatellite2IsUsed = EFalse;
	const TInt KSatellite2SigStrength = 12;

	satData.SetSatelliteId(KSatellite2Id);
	satData.SetAzimuth(KSatellite2Azimuth);
	satData.SetElevation(KSatellite2Elevation);
	satData.SetIsUsed(KSatellite2IsUsed);
	satData.SetSignalStrength(KSatellite2SigStrength);

	satInfo.AppendSatelliteData(satData);


	//Now check data got entered properly.
	CHECK_EQUAL(satInfo.NumSatellitesInView(), 2);	//Have added 2 satellites.
	}

TEST(TPositionSatelliteInfo, Other_Methods, "PSIN", 4)
	{
	TPositionSatelliteInfo satInfo;
	TSatelliteData satData;

	const TInt KSatellite1Id = 6;
	const TReal32 KSatellite1Azimuth = (TReal32)43.123;
	const TReal32 KSatellite1Elevation = (TReal32)412.45;
	TBool KSatellite1IsUsed = ETrue;
	const TInt KSatellite1SigStrength = 23;

	satData.SetSatelliteId(KSatellite1Id);
	satData.SetAzimuth(KSatellite1Azimuth);
	satData.SetElevation(KSatellite1Elevation);
	satData.SetIsUsed(KSatellite1IsUsed);
	satData.SetSignalStrength(KSatellite1SigStrength);

	satInfo.AppendSatelliteData(satData);

	satInfo.ClearSatellitesInView();
	CHECK_EQUAL(satInfo.NumSatellitesInView(), 0);
	CHECK_EQUAL(satInfo.NumSatellitesUsed(), 0);
	CHECK_EQUAL(KErrNotFound, satInfo.GetSatelliteData(0, satData));
	
	const TTime KDummySatelliteTime(TInt64(54));
	satInfo.SetSatelliteTime(KDummySatelliteTime);
	CHECK(satInfo.SatelliteTime()==KDummySatelliteTime);

	const TReal32 KDummyHorizDoP = (TReal32)345.2131;
	const TReal32 KDummyVertDoP = (TReal32)34.1;
	const TReal32 KDummyTimeDoP = (TReal32)876.68;

	satInfo.SetHorizontalDoP(KDummyHorizDoP);
	satInfo.SetVerticalDoP(KDummyVertDoP);
	satInfo.SetTimeDoP(KDummyTimeDoP);

	CHECK(RealAbs(KDummyHorizDoP-satInfo.HorizontalDoP()) < KErrorToleranceValue);	
	CHECK(RealAbs(KDummyVertDoP-satInfo.VerticalDoP()) < KErrorToleranceValue);
	CHECK(RealAbs(KDummyTimeDoP-satInfo.TimeDoP()) < KErrorToleranceValue);

	//Append a satellite that isn't used.
	satData = TSatelliteData();
	satData.SetIsUsed(EFalse);
	CHECK_EQUAL(satInfo.NumSatellitesUsed(), 0);	//Should just be no satellites.
	}

TEST(TPositionSatelliteInfo, Overflow_Detection, "PSIN", 5)
	{
	TPositionSatelliteInfo satInfo;
	TSatelliteData satData;
	for (TInt i=0; i < static_cast<TInt>(KPositionMaxSatellitesInView); i++)
		{
		satData.SetSatelliteId(i);
		CHECK_EQUAL(KErrNone, satInfo.AppendSatelliteData(satData));
		}

	satData.SetSatelliteId(KPositionMaxSatellitesInView);

	CHECK_EQUAL(KErrOverflow, satInfo.AppendSatelliteData(satData));
	}
