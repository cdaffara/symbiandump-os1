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
#include <LbsPositionInfo.h>
#include <E32Math.h>

inline TReal RealAbs(const TReal& aVal)
	{
	return Abs<TReal>(aVal);
	}

const TReal64 KErrorToleranceValue = 0.00001;

//
// TPositionInfo Tests
//

TEST(TPositionInfo, TPositionInfo_Module_Id, "PINF", 1)
	{
	TPositionInfo posInfo;
	//Check info in TPositionClassTypeBase base class.

	CHECK(posInfo.PositionClassType()==EPositionInfoClass);
	CHECK_EQUAL(posInfo.PositionClassSize(), sizeof(TPositionInfo));
	
	//Check info in TPositionInfoBase base class.
	CHECK_EQUAL((TInt)posInfo.ModuleId().iUid, (TInt)KPositionNullModuleId.iUid);

	const TPositionModuleId KDummyId = {0x12345678};
	posInfo.SetModuleId(KDummyId);
	CHECK_EQUAL((TInt)KDummyId.iUid, (TInt)posInfo.ModuleId().iUid);
	}

TEST(TPositionInfo, TPositionInfo_Update_Type, "PINF", 2)
	{
	TPositionInfo posInfo;
	CHECK(posInfo.UpdateType() == EPositionUpdateGeneral);

	posInfo.SetUpdateType(EPositionUpdateUnknown);
	CHECK(posInfo.UpdateType()==EPositionUpdateUnknown);

	posInfo.SetUpdateType(EPositionUpdateGeneral);
	CHECK(posInfo.UpdateType()==EPositionUpdateGeneral);
	}

TEST(TPositionInfo, TPositionInfo_Get_Set_Position, "PPOS", 1)
	{
	TPositionInfo posInfo;
	TPosition position;
	
	posInfo.GetPosition(position);
	CHECK_EQUAL(TInt64(0), position.Time().Int64());
	CHECK(Math::IsNaN(position.HorizontalAccuracy()));
	CHECK(Math::IsNaN(position.VerticalAccuracy()));
	CHECK(Math::IsNaN(position.Latitude()));
	CHECK(Math::IsNaN(position.Longitude()));
	CHECK(Math::IsNaN(position.Altitude()));

	const TReal KDummyLatitude = 84.2342;
	const TReal KDummyLongitude = 99.342;
	const TReal32 KDummyAltitude = (TReal32)123.2243;

	const TReal32 KDummyAccuracy1 = (TReal32)34.34534;
	const TReal32 KDummyAccuracy2 = (TReal32)354.6988;
	TLocality locality(TCoordinate(KDummyLatitude, KDummyLongitude, KDummyAltitude), KDummyAccuracy1, KDummyAccuracy2);

	const TInt64 KDummyTimeValue = 3424455;
	position = TPosition(locality, TTime(KDummyTimeValue));
	posInfo.SetPosition(position);

	TPosition position2;
	posInfo.GetPosition(position2);

	CHECK(RealAbs(position2.Latitude()-KDummyLatitude)<KErrorToleranceValue);
	CHECK(RealAbs(position2.Longitude()-KDummyLongitude)<KErrorToleranceValue);
	CHECK(RealAbs(position2.Altitude()-KDummyAltitude)<KErrorToleranceValue);

	CHECK(RealAbs(position2.HorizontalAccuracy()-KDummyAccuracy1)<KErrorToleranceValue);
	CHECK(RealAbs(position2.VerticalAccuracy()-KDummyAccuracy2)<KErrorToleranceValue);

	CHECK_EQUAL(KDummyTimeValue, position2.Time().Int64());

	}

//
// TPositionCourseInfo Tests
//

TEST(TPositionCourseInfo, TPositionCourseInfo_All_Tests,  "PCIN", 1)
	{
	TPositionCourseInfo courseInfo;
	CHECK(courseInfo.PositionClassType()==(EPositionInfoClass|EPositionCourseInfoClass));
	CHECK_EQUAL(courseInfo.PositionClassSize(), sizeof(TPositionCourseInfo));

	CHECK_EQUAL((TInt)courseInfo.ModuleId().iUid, (TInt)KPositionNullModuleId.iUid);
	CHECK(courseInfo.UpdateType() == EPositionUpdateGeneral);

	TCourse course;
	courseInfo.GetCourse(course);
	CHECK(Math::IsNaN(course.Heading()));
	CHECK(Math::IsNaN(course.HeadingAccuracy()));
	CHECK(Math::IsNaN(course.Speed()));
	CHECK(Math::IsNaN(course.SpeedAccuracy()));

	const TReal32 KDummyValue1 = (TReal32)43.342;
	const TReal32 KDummyValue2 = (TReal32)34.5236;
	const TReal32 KDummyValue3 = (TReal32)983.45;
	const TReal32 KDummyValue4 = (TReal32)12.432;

	course.SetSpeed(KDummyValue1);
	course.SetHeading(KDummyValue2);
	course.SetSpeedAccuracy(KDummyValue3);
	course.SetHeadingAccuracy(KDummyValue4);

	courseInfo.SetCourse(course);
	TCourse course2;	//Use new course object so we can be sure data is transferred properly
	courseInfo.GetCourse(course2);

	CHECK(RealAbs(course2.Speed()-KDummyValue1)<KErrorToleranceValue);
	CHECK(RealAbs(course2.Heading()-KDummyValue2)<KErrorToleranceValue);
	CHECK(RealAbs(course2.SpeedAccuracy()-KDummyValue3)<KErrorToleranceValue);
	CHECK(RealAbs(course2.HeadingAccuracy()-KDummyValue4)<KErrorToleranceValue);
	}
