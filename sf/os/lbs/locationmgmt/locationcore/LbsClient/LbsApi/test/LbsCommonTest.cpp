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
#include <LbsCommon.h>
#include <LbsCriteria.h>
#include <LbsPositionInfo.h>
#include <E32Math.h>
#include <LbsErrors.h>

//
// TPositionQualityBase Tests
//

class TPositionQualityTester : public TPositionQualityBase
	{
public:
	enum TElementValues
		{
		ETInt8ElementId,
		ETInt16ElementId,
		ETInt32ElementId,
		ETInt64ElementId,
		ETUint8ElementId,
		ETUint16ElementId,
		ETUint32ElementId,
		ETReal32ElementId,
		ETReal64ElementId,
		ETTimeElementId
		};

	void SetTInt8(TInt8 aValue, TPositionQualityItem::TValuePreference aDirection);
	void SetTInt16(TInt16 aValue, TPositionQualityItem::TValuePreference aDirection);
	void SetTInt32(TInt32 aValue, TPositionQualityItem::TValuePreference aDirection);
	void SetTInt64(TInt64 aValue, TPositionQualityItem::TValuePreference aDirection);
	void SetTUint8(TUint8 aValue, TPositionQualityItem::TValuePreference aDirection);
	void SetTUint16(TUint16 aValue, TPositionQualityItem::TValuePreference aDirection);
	void SetTUint32(TUint32 aValue, TPositionQualityItem::TValuePreference aDirection);
	void SetTReal32(TReal32 aValue, TPositionQualityItem::TValuePreference aDirection);
	void SetTReal64(TReal64 aValue, TPositionQualityItem::TValuePreference aDirection);
	void SetTTime(TTime aTime, TPositionQualityItem::TValuePreference aDirection);

	TInt GetTInt8(TInt8& aValue) const;
	TInt GetTInt16(TInt16& aValue) const;
	TInt GetTInt32(TInt32& aValue) const;
	TInt GetTInt64(TInt64& aValue) const;
	TInt GetTUint8(TUint8& aValue) const;
	TInt GetTUint16(TUint16& aValue) const;
	TInt GetTUint32(TUint32& aValue) const;
	TInt GetTReal32(TReal32& aValue) const;
	TInt GetTReal64(TReal64& aValue) const;
	TInt GetTTime(TTime& aTime) const;

	inline void ResetField(TInt aFieldId) {ResetElement(aFieldId);}
	};

void TPositionQualityTester::SetTInt8(TInt8 aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETInt8ElementId, TPositionQualityItem::ETInt8, aDirection, aValue);
	}

void TPositionQualityTester::SetTInt16(TInt16 aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETInt16ElementId, TPositionQualityItem::ETInt16, aDirection, aValue);
	}

void TPositionQualityTester::SetTInt32(TInt32 aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETInt32ElementId, TPositionQualityItem::ETInt32, aDirection, aValue);
	}

void TPositionQualityTester::SetTInt64(TInt64 aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETInt64ElementId, TPositionQualityItem::ETInt64, aDirection, aValue);
	}

void TPositionQualityTester::SetTUint8(TUint8 aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETUint8ElementId, TPositionQualityItem::ETUint8, aDirection, aValue);
	}

void TPositionQualityTester::SetTUint16(TUint16 aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETUint16ElementId, TPositionQualityItem::ETUint16, aDirection, aValue);
	}

void TPositionQualityTester::SetTUint32(TUint32 aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETUint32ElementId, TPositionQualityItem::ETUint32, aDirection, aValue);
	}

void TPositionQualityTester::SetTReal32(TReal32 aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETReal32ElementId, TPositionQualityItem::ETReal32, aDirection, aValue);
	}

void TPositionQualityTester::SetTReal64(TReal64 aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETReal64ElementId, TPositionQualityItem::ETReal64, aDirection, aValue);
	}

void TPositionQualityTester::SetTTime(TTime aValue, TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETTimeElementId, TPositionQualityItem::ETTime, aDirection, aValue);
	}

TInt TPositionQualityTester::GetTInt8(TInt8& aValue) const
	{
	return GetElement(ETInt8ElementId, TPositionQualityItem::ETInt8, aValue);
	}

TInt TPositionQualityTester::GetTInt16(TInt16& aValue) const
	{
	return GetElement(ETInt16ElementId, TPositionQualityItem::ETInt16, aValue);
	}

TInt TPositionQualityTester::GetTInt32(TInt32& aValue) const
	{
	return GetElement(ETInt32ElementId, TPositionQualityItem::ETInt32, aValue);
	}

TInt TPositionQualityTester::GetTInt64(TInt64& aValue) const
	{
	return GetElement(ETInt64ElementId, TPositionQualityItem::ETInt64, aValue);
	}

TInt TPositionQualityTester::GetTUint8(TUint8& aValue) const
	{
	return GetElement(ETUint8ElementId, TPositionQualityItem::ETUint8, aValue);
	}

TInt TPositionQualityTester::GetTUint16(TUint16& aValue) const
	{
	return GetElement(ETUint16ElementId, TPositionQualityItem::ETUint16, aValue);
	}

TInt TPositionQualityTester::GetTUint32(TUint32& aValue) const
	{
	return GetElement(ETUint32ElementId, TPositionQualityItem::ETUint32, aValue);
	}

TInt TPositionQualityTester::GetTReal32(TReal32& aValue) const
	{
	return GetElement(ETReal32ElementId, TPositionQualityItem::ETReal32, aValue);
	}

TInt TPositionQualityTester::GetTReal64(TReal64& aValue) const
	{
	return GetElement(ETReal64ElementId, TPositionQualityItem::ETReal64, aValue);
	}

TInt TPositionQualityTester::GetTTime(TTime& aValue) const
	{
	return GetElement(ETTimeElementId, TPositionQualityItem::ETTime, aValue);
	}

class TPositionQualityTester2 : public TPositionQualityBase
	{
public:
	enum TElementValues
		{
		ETTimeIntervalMicroSecondsElementId,
		ETTimeElementId	//Used to test mismatched data types.
		};
	void SetTTimeIntervalMicroSeconds(TTimeIntervalMicroSeconds aValue,
			TPositionQualityItem::TValuePreference aDirection);
	void SetTTime(TTime aTime, TPositionQualityItem::TValuePreference aDirection);
	void SetBadValue();

	TInt GetTTimeIntervalMicroSeconds(TTimeIntervalMicroSeconds& aValue) const;
	TInt GetTTime(TTime& aTime) const;
	TInt GetBadValue() const;

	inline void ResetField(TInt aFieldId) {ResetElement(aFieldId);}
	};

void TPositionQualityTester2::SetTTimeIntervalMicroSeconds(TTimeIntervalMicroSeconds aValue,
							TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETTimeIntervalMicroSecondsElementId, TPositionQualityItem::ETTimeIntervalMicroSeconds,
				  aDirection, aValue);
	}

void TPositionQualityTester2::SetTTime(TTime aValue,
			TPositionQualityItem::TValuePreference aDirection)
	{
	SetElement(ETTimeElementId, TPositionQualityItem::ETTime, aDirection, aValue);
	}

void TPositionQualityTester2::SetBadValue()
	{
	SetElement(ETTimeElementId, TPositionQualityItem::ETTime,
		TPositionQualityItem::EPreferGreaterValues, TPosition());
	}

TInt TPositionQualityTester2::GetTTimeIntervalMicroSeconds(TTimeIntervalMicroSeconds& aValue) const
	{
	return GetElement(ETTimeIntervalMicroSecondsElementId, 
		TPositionQualityItem::ETTimeIntervalMicroSeconds, aValue);
	}

TInt TPositionQualityTester2::GetTTime(TTime& aValue) const
	{
	return GetElement(ETTimeElementId, TPositionQualityItem::ETTime, aValue);
	}

TInt TPositionQualityTester2::GetBadValue() const
	{
	TPosition position;
	return GetElement(ETTimeElementId, TPositionQualityItem::ETTime, position);
	}

//

TEST(TPositionQualityBase, Construction, "PQBF", 1)
	{
	//Check that all fields are set to undefined, and that an error occurs if you
	//try and get the value.
	TPositionQualityTester posQuality;

	for(TUint i=0; i<KPositionMaxSectionFields; i++)
		{
		CHECK(!posQuality.IsDefined(i));
		}

	TInt8 int8;
	CHECK_EQUAL(posQuality.GetTInt8(int8), KErrArgument);

	TInt16 int16;
	CHECK_EQUAL(posQuality.GetTInt16(int16), KErrArgument);

	TInt32 int32;
	CHECK_EQUAL(posQuality.GetTInt32(int32), KErrArgument);

	TInt64 int64;
	CHECK_EQUAL(posQuality.GetTInt64(int64), KErrArgument);

	TUint8 uint8;
	CHECK_EQUAL(posQuality.GetTUint8(uint8), KErrArgument);

	TUint16 uint16;
	CHECK_EQUAL(posQuality.GetTUint16(uint16), KErrArgument);

	TUint32 uint32;
	CHECK_EQUAL(posQuality.GetTUint32(uint32), KErrArgument);

	TReal32 real32;
	CHECK_EQUAL(posQuality.GetTReal32(real32), KErrArgument);

	TReal64 real64;
	CHECK_EQUAL(posQuality.GetTReal64(real64), KErrArgument);

	TTime time;
	CHECK_EQUAL(posQuality.GetTTime(time), KErrArgument);
	}

TEST(TPositionQualityBase, Getters_And_Setters, "PQBF", 2)
	{
	TPositionQualityTester posQuality;
	TPositionQualityTester posQuality2;
	TPositionQualityItem::TResult comparison;

	//
	//TInt8
	//
	const TInt8 KDummyInt8 = 0x54;
	posQuality.SetTInt8(KDummyInt8, TPositionQualityItem::EPreferGreaterValues);
	TInt8 int8 = 0;
	CHECK_EQUAL(KErrNone, posQuality.GetTInt8(int8));
	CHECK_EQUAL(KDummyInt8, int8);
	
	posQuality2.SetTInt8(KDummyInt8 + 1,  TPositionQualityItem::EPreferGreaterValues);

	//posquality2 should be better.
	CHECK_EQUAL(KErrNone, 
		posQuality2.Compare(posQuality, TPositionQualityTester::ETInt8ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	CHECK_EQUAL(KErrNone, 
		posQuality.Compare(posQuality2, TPositionQualityTester::ETInt8ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	//Now make them equal
	posQuality2.SetTInt8(KDummyInt8, TPositionQualityItem::EPreferGreaterValues);
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETInt8ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETInt8ElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETInt8ElementId));
	//
	//TInt16
	//
	const TInt16 KDummyInt16 = 0x3454;
	posQuality.SetTInt16(KDummyInt16, TPositionQualityItem::EPreferSmallerValues);
	TInt16 int16 = 0;
	CHECK_EQUAL(KErrNone, posQuality.GetTInt16(int16));
	CHECK_EQUAL(KDummyInt16, int16);
	
	posQuality2.SetTInt16(KDummyInt16 + 1,  TPositionQualityItem::EPreferSmallerValues);
	//posquality2 should be worse.
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETInt16ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	CHECK_EQUAL(KErrNone, 
		posQuality.Compare(posQuality2, TPositionQualityTester::ETInt16ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	//Now make them equal
	posQuality2.SetTInt16(KDummyInt16, TPositionQualityItem::EPreferSmallerValues);
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETInt16ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETInt16ElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETInt16ElementId));

	//
	//TInt32
	//
	const TInt32 KDummyInt32 = 0xfe34ed54;
	posQuality.SetTInt32(KDummyInt32, TPositionQualityItem::EPreferGreaterValues);
	TInt32 int32 = 0;
	CHECK_EQUAL(KErrNone, posQuality.GetTInt32(int32));
	CHECK_EQUAL(KDummyInt32, int32);
	
	posQuality2.SetTInt32(KDummyInt32 + 1,  TPositionQualityItem::EPreferGreaterValues);

	//posquality2 should be better.
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETInt32ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionQualityTester::ETInt32ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	//Now make them equal
	posQuality2.SetTInt32(KDummyInt32, TPositionQualityItem::EPreferGreaterValues);
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETInt32ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETInt32ElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETInt32ElementId));

	//
	//TInt64
	//
	const TInt64 KDummyInt64 = 0x7e34ed5423ef45ac;
	posQuality.SetTInt64(KDummyInt64, TPositionQualityItem::EPreferSmallerValues);
	TInt64 int64 = 0;
	CHECK_EQUAL(KErrNone, posQuality.GetTInt64(int64));
	CHECK_EQUAL(KDummyInt64, int64);
	
	posQuality2.SetTInt64(KDummyInt64 + 1,  TPositionQualityItem::EPreferSmallerValues);

	//posquality2 should be worse.
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETInt64ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionQualityTester::ETInt64ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	//Now make them equal
	posQuality2.SetTInt64(KDummyInt64, TPositionQualityItem::EPreferSmallerValues);
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETInt64ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETInt64ElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETInt64ElementId));

	//
	//TUint8
	//
	const TUint8 KDummyUint8 = 0x54;
	posQuality.SetTUint8(KDummyUint8, TPositionQualityItem::EPreferGreaterValues);
	TUint8 uint8 = 0;
	CHECK_EQUAL(KErrNone, posQuality.GetTUint8(uint8));
	CHECK_EQUAL(KDummyUint8, uint8);
	
	posQuality2.SetTUint8(KDummyUint8 + 1,  TPositionQualityItem::EPreferGreaterValues);

	//posquality2 should be better.
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETUint8ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionQualityTester::ETUint8ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	//Now make them equal
	posQuality2.SetTUint8(KDummyUint8, TPositionQualityItem::EPreferGreaterValues);
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETUint8ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETUint8ElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETUint8ElementId));

	//
	//TUint16
	//
	const TUint16 KDummyUint16 = 0x54ac;
	posQuality.SetTUint16(KDummyUint16, TPositionQualityItem::EPreferSmallerValues);
	TUint16 uint16 = 0;
	CHECK_EQUAL(KErrNone, posQuality.GetTUint16(uint16));
	CHECK_EQUAL(KDummyUint16, uint16);
	
	posQuality2.SetTUint16(KDummyUint16 + 1,  TPositionQualityItem::EPreferSmallerValues);

	//posquality2 should be worse
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETUint16ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionQualityTester::ETUint16ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	//Now make them equal
	posQuality2.SetTUint16(KDummyUint16, TPositionQualityItem::EPreferSmallerValues);
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETUint16ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETUint16ElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETUint16ElementId));

	//
	//TUint32
	//
	const TUint32 KDummyUint32 = 0x54ac75be;
	posQuality.SetTUint32(KDummyUint32, TPositionQualityItem::EPreferGreaterValues);
	TUint32 uint32 = 0;
	CHECK_EQUAL(KErrNone, posQuality.GetTUint32(uint32));
	CHECK_EQUAL(KDummyUint32, uint32);
	
	posQuality2.SetTUint32(KDummyUint32 + 1,  TPositionQualityItem::EPreferGreaterValues);

	//posquality2 should be better
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETUint32ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionQualityTester::ETUint32ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	//Now make them equal
	posQuality2.SetTUint32(KDummyUint32, TPositionQualityItem::EPreferGreaterValues);
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETUint32ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETUint32ElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETUint32ElementId));

	//
	//TReal32
	//
	const TReal32 KDummyReal32 = (TReal32)45.12;
	posQuality.SetTReal32(KDummyReal32, TPositionQualityItem::EPreferSmallerValues);
	TReal32 real32 = 0;
	CHECK_EQUAL(KErrNone, posQuality.GetTReal32(real32));
	CHECK_EQUAL(KDummyReal32, real32);
	
	posQuality2.SetTReal32(KDummyReal32 + 1,  TPositionQualityItem::EPreferSmallerValues);

	//posquality2 should be worse
	CHECK_EQUAL(KErrNone, 
		posQuality2.Compare(posQuality, TPositionQualityTester::ETReal32ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionQualityTester::ETReal32ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	//Now make them equal
	posQuality2.SetTReal32(KDummyReal32, TPositionQualityItem::EPreferSmallerValues);
	CHECK_EQUAL(KErrNone, 
		posQuality2.Compare(posQuality, TPositionQualityTester::ETReal32ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETReal32ElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETReal32ElementId));

	//
	//TReal64
	//
	const TReal64 KDummyReal64 = 346.124;
	posQuality.SetTReal64(KDummyReal64, TPositionQualityItem::EPreferGreaterValues);
	TReal64 real64 = 0;
	CHECK_EQUAL(KErrNone, posQuality.GetTReal64(real64));
	CHECK_EQUAL(KDummyReal64, real64);
	
	posQuality2.SetTReal64(KDummyReal64 + 1,  TPositionQualityItem::EPreferGreaterValues);

	//posquality2 should be better
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETReal64ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	CHECK_EQUAL(KErrNone, 
		posQuality.Compare(posQuality2, TPositionQualityTester::ETReal64ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	//Now make them equal
	posQuality2.SetTReal64(KDummyReal64, TPositionQualityItem::EPreferGreaterValues);
	CHECK_EQUAL(KErrNone, 
		posQuality2.Compare(posQuality, TPositionQualityTester::ETReal64ElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETReal64ElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETReal64ElementId));

	//
	//TTime
	//
	const TTime KDummyTime (TInt64(34));
	posQuality.SetTTime(KDummyTime, TPositionQualityItem::EPreferSmallerValues);
	TTime time (TInt64(0));
	CHECK_EQUAL(KErrNone, posQuality.GetTTime(time));
	CHECK_EQUAL(KDummyTime, time);
	
	posQuality2.SetTTime(KDummyTime + TTimeIntervalMicroSeconds(1),
		TPositionQualityItem::EPreferSmallerValues);

	//posquality2 should be worse
	CHECK_EQUAL(KErrNone,
		posQuality2.Compare(posQuality, TPositionQualityTester::ETTimeElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionQualityTester::ETTimeElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);
	//Now make them equal
	posQuality2.SetTTime(KDummyTime, TPositionQualityItem::EPreferSmallerValues);
	CHECK_EQUAL(KErrNone, 
		posQuality2.Compare(posQuality, TPositionQualityTester::ETTimeElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality.ResetField(TPositionQualityTester::ETTimeElementId);
	CHECK(!posQuality.IsDefined(TPositionQualityTester::ETTimeElementId));

	//
	//TTimeIntervalMicroSeconds
	//

	TPositionQualityTester2 posQuality3;
	TPositionQualityTester2 posQuality4;
	const TTimeIntervalMicroSeconds KDummyTimeIntervalMicroSeconds (TInt64(34));
	posQuality3.SetTTimeIntervalMicroSeconds(KDummyTimeIntervalMicroSeconds, 
		TPositionQualityItem::EPreferGreaterValues);
	TTimeIntervalMicroSeconds timeIntervalMicroSeconds (TInt64(0));
	CHECK_EQUAL(KErrNone,
		posQuality3.GetTTimeIntervalMicroSeconds(timeIntervalMicroSeconds));
	CHECK_EQUAL(KDummyTime.Int64(), timeIntervalMicroSeconds.Int64());
	
	posQuality4.SetTTimeIntervalMicroSeconds(KDummyTimeIntervalMicroSeconds.Int64() + 1,
		TPositionQualityItem::EPreferGreaterValues);

	//posquality4 should be better
	CHECK_EQUAL(KErrNone, posQuality4.Compare(posQuality3, 
		TPositionQualityTester2::ETTimeIntervalMicroSecondsElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsBetter);

	CHECK_EQUAL(KErrNone, posQuality3.Compare(posQuality4, 
		TPositionQualityTester2::ETTimeIntervalMicroSecondsElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsWorse);

	//Now make them equal
	posQuality4.SetTTimeIntervalMicroSeconds(KDummyTimeIntervalMicroSeconds,
		TPositionQualityItem::EPreferGreaterValues);
	CHECK_EQUAL(KErrNone, posQuality4.Compare(posQuality3, 
		TPositionQualityTester2::ETTimeIntervalMicroSecondsElementId, comparison));
	CHECK_EQUAL((TInt)comparison, (TInt)TPositionQualityItem::EIsEqual);

	//Finally, check that resetting the data works
	posQuality3.ResetField(TPositionQualityTester2::ETTimeIntervalMicroSecondsElementId);
	CHECK(!posQuality3.IsDefined(TPositionQualityTester2::ETTimeIntervalMicroSecondsElementId));
	}

TEST(TPositionQualityBase, Check_Errors, "PQBF", 3)
	{
	TPositionQualityTester posQuality;
	TPositionQualityTester2 posQuality2;
	TPositionQualityItem::TResult comparison;

	//CHECK FOR MISMATCHED DATA TYPES

	//Both these methods set the element at offset 1 in TPositionQualityBase.
	posQuality.SetTInt16(32, TPositionQualityItem::EPreferGreaterValues);
	posQuality2.SetTTime(TTime(32), TPositionQualityItem::EPreferGreaterValues);

	CHECK_EQUAL(KErrArgument, 
		posQuality.Compare(posQuality2, TPositionQualityTester::ETInt16ElementId, comparison));
	
	//CHECK FOR MISMATCHED SCALE DIRECTIONS
	TPositionQualityTester posQuality3;

	posQuality3.SetTInt16(32, TPositionQualityItem::EPreferSmallerValues);

	CHECK_EQUAL(KErrArgument,
		posQuality.Compare(posQuality3, TPositionQualityTester::ETUint16ElementId, comparison));
	}

TEST(TPositionQualityBase, HighWaterMark, "PQBF", 4)
	{
	//The high water mark goes up and down in this method. It starts at -1, then goes
	//up to two, then another value is set below the highwater mark, then this is removed.
	//Then the mark goes up to 3, back to 2 and finally back to -1.
	TPositionQualityTester posQuality;
	CHECK_EQUAL(-1, posQuality.HighWaterMark());
	posQuality.SetTInt16(TInt16(0x1234), TPositionQualityItem::EPreferSmallerValues);

	CHECK_EQUAL((TInt)TPositionQualityTester::ETInt16ElementId, (TInt)posQuality.HighWaterMark());

	posQuality.SetTInt8(TInt8(0x43), TPositionQualityItem::EPreferSmallerValues);

	CHECK_EQUAL((TInt)TPositionQualityTester::ETInt16ElementId, (TInt)posQuality.HighWaterMark());

	posQuality.ResetField(TPositionQualityTester::ETInt8ElementId);

	CHECK_EQUAL((TInt)TPositionQualityTester::ETInt16ElementId, (TInt)posQuality.HighWaterMark());

	posQuality.SetTInt32(TInt32(0xabab4343), TPositionQualityItem::EPreferSmallerValues);

	CHECK_EQUAL((TInt)TPositionQualityTester::ETInt32ElementId, (TInt)posQuality.HighWaterMark());

	posQuality.ResetField(TPositionQualityTester::ETInt32ElementId);

	CHECK_EQUAL((TInt)TPositionQualityTester::ETInt16ElementId, (TInt)posQuality.HighWaterMark());

	posQuality.ResetField(TPositionQualityTester::ETInt16ElementId);

	CHECK_EQUAL(-1, posQuality.HighWaterMark());
	}

TInt Do_Set_Bad_Value(TAny* /*aPtr*/)
	{
	TPositionQualityTester2 posQuality;
	posQuality.SetBadValue();
	return KErrGeneral;
	}

TInt Do_Get_Bad_Value(TAny* /*aPtr*/)
	{
	TPositionQualityTester2 posQuality;
	(void)posQuality.GetBadValue();
	return KErrGeneral;
	}

TEST(TPositionQualityBase, Panic_Tests, "PQBF", 5)
	{
	DO_PANIC_TEST_L(Do_Set_Bad_Value, KPosClientFault, 
					EPositionQualityBadDataType, KDefaultTestTimeout);
	DO_PANIC_TEST_L(Do_Set_Bad_Value, KPosClientFault, 
					EPositionQualityBadDataType, KDefaultTestTimeout);
	}

//
// TPositionQuality Tests
//

TEST(TPositionQuality, TimeToFixes, "PQTF", 1)
	{
	TPositionQuality posQuality;
	CHECK_EQUAL(sizeof(TPositionQuality),posQuality.PositionClassSize());
	CHECK(EPositionQualityClass == posQuality.PositionClassType());

	CHECK_EQUAL(TInt64(0), posQuality.TimeToFirstFix().Int64());
	CHECK_EQUAL(TInt64(0), posQuality.TimeToNextFix().Int64());

	const TTimeIntervalMicroSeconds KTTFF(3425234);
	const TTimeIntervalMicroSeconds KTTNF(45256265);

	posQuality.SetTimeToFirstFix(KTTFF);
	posQuality.SetTimeToNextFix(KTTNF);

	CHECK_EQUAL(KTTFF.Int64(), posQuality.TimeToFirstFix().Int64());
	CHECK_EQUAL(KTTNF.Int64(), posQuality.TimeToNextFix().Int64());

	//Check the scale directions.
	TPositionQuality posQuality2;
	posQuality2.SetTimeToFirstFix(KTTFF.Int64()+1);
	posQuality2.SetTimeToNextFix(KTTNF.Int64()-1);
	//So posQuality2 has worse TTFF and better TTNF...

	TPositionQualityItem::TResult result;
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionSelectionOrder::EFieldTimeToFirstFix,
			result));
	CHECK_EQUAL((TInt)result, (TInt)TPositionQualityItem::EIsBetter);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionSelectionOrder::EFieldTimeToNextFix,
			result));
	CHECK_EQUAL((TInt)result, (TInt)TPositionQualityItem::EIsWorse);

	//Check that setting to 0 makes field undefined...
	posQuality.SetTimeToFirstFix(TTimeIntervalMicroSeconds(0));
	CHECK(!posQuality.IsDefined(TPositionSelectionOrder::EFieldTimeToFirstFix));

	posQuality.SetTimeToNextFix(TTimeIntervalMicroSeconds(0));
	CHECK(!posQuality.IsDefined(TPositionSelectionOrder::EFieldTimeToNextFix));
	}

TEST(TPositionQuality, Accuracy, "PQAC", 1)
	{
	TPositionQuality posQuality;
	CHECK(Math::IsNaN(posQuality.HorizontalAccuracy()));
	CHECK(Math::IsNaN(posQuality.VerticalAccuracy()));

	const TReal32 KHorizontalAccuracy(TReal32(34252.23452));
	const TReal32 KVerticalAccuracy(TReal32(423.456563));

	posQuality.SetHorizontalAccuracy(KHorizontalAccuracy);
	posQuality.SetVerticalAccuracy(KVerticalAccuracy);

	CHECK_EQUAL(KHorizontalAccuracy, posQuality.HorizontalAccuracy());
	CHECK_EQUAL(KVerticalAccuracy, posQuality.VerticalAccuracy());

	//Check the scale directions
	TPositionQuality posQuality2;
	posQuality2.SetHorizontalAccuracy(KHorizontalAccuracy+1);
	posQuality2.SetVerticalAccuracy(KVerticalAccuracy-1);
	//So posQuality2 has worse HA and better VA...

	TPositionQualityItem::TResult result;
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionSelectionOrder::EFieldHorizontalAccuracy,
			result));
	CHECK_EQUAL((TInt)result, (TInt)TPositionQualityItem::EIsBetter);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionSelectionOrder::EFieldVerticalAccuracy,
			result));
	CHECK_EQUAL((TInt)result, (TInt)TPositionQualityItem::EIsWorse);

	//Check setting to nan makes field undefined.
	TRealX realX;
	realX.SetNaN();
	posQuality.SetHorizontalAccuracy(realX);
	CHECK(!posQuality.IsDefined(TPositionSelectionOrder::EFieldHorizontalAccuracy));

	posQuality.SetVerticalAccuracy(realX);
	CHECK(!posQuality.IsDefined(TPositionSelectionOrder::EFieldVerticalAccuracy));

	}

TEST(TPositionQuality, Cost_And_Power, "PQCP", 1)
	{
	TPositionQuality posQuality;

	CHECK_EQUAL((TInt)TPositionQuality::ECostUnknown, (TInt)posQuality.CostIndicator());
	CHECK_EQUAL((TInt)TPositionQuality::EPowerUnknown, (TInt)posQuality.PowerConsumption());

	const TPositionQuality::TCostIndicator KDummyCost(TPositionQuality::ECostPossible);
	const TPositionQuality::TPowerConsumption KDummyPowerConsumption(TPositionQuality::EPowerMedium);

	posQuality.SetCostIndicator(KDummyCost);
	posQuality.SetPowerConsumption(KDummyPowerConsumption);
	
	CHECK_EQUAL((TInt)KDummyCost, (TInt)posQuality.CostIndicator());
	CHECK_EQUAL((TInt)KDummyPowerConsumption, (TInt)posQuality.PowerConsumption());

	//Check the scale directions
	TPositionQuality posQuality2;
	posQuality2.SetCostIndicator(TPositionQuality::ECostCharge);
	posQuality2.SetPowerConsumption(TPositionQuality::EPowerLow);
	//So posQuality2 has worse CI and better PC...

	TPositionQualityItem::TResult result;
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionSelectionOrder::EFieldCost,
			result));
	CHECK_EQUAL((TInt)result, (TInt)TPositionQualityItem::EIsBetter);
	CHECK_EQUAL(KErrNone,
		posQuality.Compare(posQuality2, TPositionSelectionOrder::EFieldPower,
			result));
	CHECK_EQUAL((TInt)result, (TInt)TPositionQualityItem::EIsWorse);

	//Check that setting fields to unknown makes them undefined.

	posQuality.SetCostIndicator(TPositionQuality::ECostUnknown);
	CHECK(!posQuality.IsDefined(TPositionSelectionOrder::EFieldCost));

	posQuality.SetPowerConsumption(TPositionQuality::EPowerUnknown);
	CHECK(!posQuality.IsDefined(TPositionSelectionOrder::EFieldPower));
	}

TInt SetHorizontalAccuracyQuality_Invalid(TAny* /*aPtr*/)
	{
	TPositionQuality posQuality;
	posQuality.SetHorizontalAccuracy(TReal32(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TPositionQuality, SetHorizontalAccuracy_Invalid, "PQAC", 2)
	{
	DO_PANIC_TEST_L(SetHorizontalAccuracyQuality_Invalid, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TInt SetVerticalAccuracyQuality_Invalid(TAny* /*aPtr*/)
	{
	TPositionQuality posQuality;
	posQuality.SetVerticalAccuracy(TReal32(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TPositionQuality, SetVerticalAccuracy_Invalid, "PQAC", 3)
	{
	DO_PANIC_TEST_L(SetVerticalAccuracyQuality_Invalid, KPosClientFault, 
					EPositionBadAccuracy, KDefaultTestTimeout);
	}

TInt SetTimeToFirstFix_Invalid(TAny* /*aPtr*/)
	{
	TPositionQuality posQuality;
	posQuality.SetTimeToFirstFix(TTimeIntervalMicroSeconds(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TPositionQuality, SetTimeToFirstFix_Invalid, "PQAC", 4)
	{
	DO_PANIC_TEST_L(SetTimeToFirstFix_Invalid, KPosClientFault, 
					EPositionBadTime, KDefaultTestTimeout);
	}

TInt SetTimeToNextFix_Invalid(TAny* /*aPtr*/)
	{
	TPositionQuality posQuality;
	posQuality.SetTimeToNextFix(TTimeIntervalMicroSeconds(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TPositionQuality, SetTimeToNextFix_Invalid, "PQAC", 5)
	{
	DO_PANIC_TEST_L(SetTimeToNextFix_Invalid, KPosClientFault, 
					EPositionBadTime, KDefaultTestTimeout);
	}

//
// TPositionModuleInfo Tests
//

TEST(TPositionModuleInfo, Construction, "PMTF", 1)
	{
	TPositionModuleInfo posModInfo;
	CHECK_EQUAL(sizeof(TPositionModuleInfo), posModInfo.PositionClassSize());
	CHECK(EPositionModuleInfoClass==posModInfo.PositionClassType());
	CHECK(KPositionNullModuleId.iUid == posModInfo.ModuleId().iUid);

	TPositionQuality posQuality;
	posQuality.SetHorizontalAccuracy(TReal32(34.34523));	//Make sure this TPositionQuality != default.

	posModInfo.GetPositionQuality(posQuality);

	CHECK(Math::IsNaN(posQuality.HorizontalAccuracy()));

	CHECK(!posModInfo.IsAvailable());

	TBuf<KPositionMaxModuleName> moduleName;
	posModInfo.GetModuleName(moduleName);

	CHECK_EQUAL(0, moduleName.Length());

	CHECK_EQUAL(TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyUnknown),
				posModInfo.TechnologyType());
	CHECK_EQUAL(TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceUnknown),
				posModInfo.DeviceLocation());
	CHECK_EQUAL(TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilityNone),
				posModInfo.Capabilities());

	for(TInt i=0; i<EPositionLastFamily; i++)
		CHECK(0 == posModInfo.ClassesSupported(TPositionClassFamily(i)));

	TVersion version = posModInfo.Version();
	TVersion defaultVersion;
	CHECK_EQUAL(version.iMajor, defaultVersion.iMajor);
	CHECK_EQUAL(version.iMinor, defaultVersion.iMinor);
	CHECK_EQUAL(version.iBuild, defaultVersion.iBuild);
	}

TEST(TPositionModuleInfo, Getters_And_Setters, "PMTF", 2)
	{
	TPositionModuleInfo posModInfo;

	const TPositionModuleId KDummyModuleId = {0x32};
	posModInfo.SetModuleId(KDummyModuleId);
	CHECK_EQUAL(KDummyModuleId.iUid, posModInfo.ModuleId().iUid);
	
	posModInfo.SetIsAvailable(ETrue);
	CHECK(posModInfo.IsAvailable());
	posModInfo.SetIsAvailable(EFalse);
	CHECK(!posModInfo.IsAvailable());

	_LIT(KDummyModuleName, "Testing 1,2,3");
	posModInfo.SetModuleName(KDummyModuleName);
	TBuf<KPositionMaxModuleName> tempBuf;
	posModInfo.GetModuleName(tempBuf);
	CHECK(!tempBuf.Compare(KDummyModuleName));

	TPositionQuality positionQuality;
	const TPositionQuality::TCostIndicator KDummyCostIndicator = TPositionQuality::ECostCharge;
	positionQuality.SetCostIndicator(KDummyCostIndicator);
	posModInfo.SetPositionQuality(positionQuality);
	positionQuality = TPositionQuality();

	CHECK_EQUAL(TPositionQuality::ECostUnknown, positionQuality.CostIndicator());	//check the reset worked OK.
	posModInfo.GetPositionQuality(positionQuality);
	CHECK_EQUAL(KDummyCostIndicator, positionQuality.CostIndicator());

	posModInfo.SetTechnologyType(TPositionModuleInfo::ETechnologyNetwork);
	CHECK_EQUAL(TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyNetwork),
				posModInfo.TechnologyType());

	posModInfo.SetDeviceLocation(TPositionModuleInfo::EDeviceExternal);
	CHECK_EQUAL(TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceExternal),
				posModInfo.DeviceLocation());

	const TPositionModuleInfo::TCapabilities KDummyCaps = 
		TPositionModuleInfo::ECapabilitySpeed|TPositionModuleInfo::ECapabilityNmea;
	posModInfo.SetCapabilities(KDummyCaps);
	CHECK_EQUAL(KDummyCaps, posModInfo.Capabilities());
	
	for(TUint i=0; i<EPositionLastFamily; i++)
		posModInfo.SetClassesSupported(TPositionClassFamily(i), 2*i);

	for(TUint j=0; j<EPositionLastFamily; j++)
		CHECK(2*j == posModInfo.ClassesSupported(TPositionClassFamily(j)));

	TVersion KDummyVersion(12,31,45);	//Should be const, but Name isn't a const method for some reason!
	posModInfo.SetVersion(KDummyVersion);
	CHECK(posModInfo.Version().Name()==KDummyVersion.Name());
	}

TInt Set_Bad_Class_Type_TPositionModuleInfo(TAny* /*aPtr*/)
	{
	TPositionModuleInfo modInfo;
	modInfo.SetClassesSupported(EPositionLastFamily, 0);	//This should panic
	return KErrGeneral;
	}

TInt Get_Bad_Class_Type_TPositionModuleInfo(TAny* /*aPtr*/)
	{
	TPositionModuleInfo modInfo;
	(void)modInfo.ClassesSupported(EPositionLastFamily);	//This should panic
	return KErrGeneral;
	}

TEST(TPositionModuleInfo, Panic_Test, "PMTF", 3)
	{
	DO_PANIC_TEST_L(Set_Bad_Class_Type_TPositionModuleInfo, KPosClientFault,
				EPositionInvalidClassType, KDefaultTestTimeout);
	DO_PANIC_TEST_L(Get_Bad_Class_Type_TPositionModuleInfo, KPosClientFault,
				EPositionInvalidClassType, KDefaultTestTimeout);
	}

//
// TPositionModuleStatus Tests
//

TEST(TPositionModuleStatus, Device_Status, "PMSI", 1)
	{
	TPositionModuleStatus moduleStatus;

	CHECK_EQUAL((TInt)TPositionModuleStatus::EDeviceUnknown, (TInt)moduleStatus.DeviceStatus());

	const TPositionModuleStatus::TDeviceStatus
		KDummyDeviceStatus(TPositionModuleStatus::EDeviceInactive);
	moduleStatus.SetDeviceStatus(KDummyDeviceStatus);

	CHECK_EQUAL(KDummyDeviceStatus, moduleStatus.DeviceStatus());	
	}

TEST(TPositionModuleStatus, Data_Quality, "PMSI", 2)
	{
	TPositionModuleStatus moduleStatus;

	CHECK_EQUAL((TInt)TPositionModuleStatus::EDataQualityUnknown, (TInt)moduleStatus.DataQualityStatus());
	
	const TPositionModuleStatus::TDataQualityStatus
		KDummyQualityStatus(TPositionModuleStatus::EDataQualityPartial);
	moduleStatus.SetDataQualityStatus(KDummyQualityStatus);

	CHECK_EQUAL(KDummyQualityStatus, moduleStatus.DataQualityStatus());
	}

//
// TPositionModuleStatusEvent Tests
//

TEST(TPositionModuleStatusEvent, Requested_Events, "PMSE", 1)
	{
	TPositionModuleStatusEvent posModEvent;
	CHECK_EQUAL(sizeof(TPositionModuleStatusEvent), posModEvent.PositionClassSize());
	CHECK(EPositionModuleStatusEventClass ==
			posModEvent.PositionClassType());
	
	CHECK(TPositionModuleStatusEvent::EEventNone==posModEvent.RequestedEvents());

	const TPositionModuleStatusEvent::TModuleEvent
		KDummyRequestedEvent(TPositionModuleStatusEvent::EEventDeviceStatus);
	posModEvent.SetRequestedEvents(KDummyRequestedEvent);

	CHECK(posModEvent.RequestedEvents()==KDummyRequestedEvent);

	//Just do a check here for construction with requested events parameter.
	TPositionModuleStatusEvent posModEvent2(TPositionModuleStatusEvent::EEventDataQualityStatus);
	CHECK(posModEvent2.RequestedEvents()==TPositionModuleStatusEvent::EEventDataQualityStatus);
	}

TEST(TPositionModuleStatusEvent, Observer_Events, "PMSE", 2)
	{
	TPositionModuleStatusEvent posModEvent;
	CHECK(0==posModEvent.OccurredEvents());

	const TPositionModuleStatusEvent::TModuleEvent
		KDummyOccurredEvent(TPositionModuleStatusEvent::EEventDataQualityStatus);
	posModEvent.SetOccurredEvents(KDummyOccurredEvent);

	CHECK(posModEvent.OccurredEvents()==KDummyOccurredEvent);
	}

TEST(TPositionModuleStatusEvent, Module_Status, "PMSE", 3)
	{
	// Check the module status is initially uninitialised and then whatever
	// the status is set to using SetModuleStatus.

	TPositionModuleStatus moduleStatus;
	TPositionModuleStatusEvent posModEvent;
	posModEvent.GetModuleStatus(moduleStatus);

	CHECK_EQUAL((TInt)TPositionModuleStatus::EDeviceUnknown, (TInt)moduleStatus.DeviceStatus());
	CHECK_EQUAL((TInt)TPositionModuleStatus::EDataQualityUnknown, (TInt)moduleStatus.DataQualityStatus());

	TPositionModuleStatus modStatus;
	modStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityPartial);
	modStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceInactive);

	posModEvent.SetModuleStatus(modStatus);
	modStatus = TPositionModuleStatus();	//reset modStatus

	posModEvent.GetModuleStatus(modStatus);
	CHECK_EQUAL((TInt)TPositionModuleStatus::EDeviceInactive, (TInt)modStatus.DeviceStatus());
	CHECK_EQUAL((TInt)TPositionModuleStatus::EDataQualityPartial, (TInt)modStatus.DataQualityStatus());
	}

TEST(TPositionModuleStatusEvent, SystemModuleEvent, "PMSE", 4)
	{
	// New test.

	TPositionModuleStatusEventBase::TSystemModuleEvent moduleEvent;
	TPositionModuleStatusEvent posModEvent;
	
	moduleEvent = posModEvent.SystemModuleEvent();
	CHECK_EQUAL((TInt)TPositionModuleStatusEventBase::ESystemUnknown, (TInt)moduleEvent);

	posModEvent.SetSystemModuleEvent(TPositionModuleStatusEventBase::ESystemModuleInstalled);

	moduleEvent = posModEvent.SystemModuleEvent();
	CHECK_EQUAL((TInt)TPositionModuleStatusEventBase::ESystemModuleInstalled, (TInt)moduleEvent);
	}

//
// TPositionUpdateOptions Tests
//

TEST(TPositionUpdateOptions, Construction, "PUOP", 1)
	{
	TPositionUpdateOptions updateOptions;
	CHECK_EQUAL(sizeof(TPositionUpdateOptions), updateOptions.PositionClassSize());
	CHECK(EPositionUpdateOptionsClass
		==updateOptions.PositionClassType());

	CHECK_EQUAL(TInt64(0), updateOptions.UpdateInterval().Int64());
	CHECK_EQUAL(TInt64(0), updateOptions.UpdateTimeOut().Int64());
	CHECK_EQUAL(TInt64(0), updateOptions.MaxUpdateAge().Int64());
	CHECK(!updateOptions.AcceptPartialUpdates());

	const TBool KDummyAcceptUpdates = ETrue;
	updateOptions.SetAcceptPartialUpdates(KDummyAcceptUpdates);

	const TTimeIntervalMicroSeconds KUpdateInterval(23412);
	updateOptions.SetUpdateInterval(KUpdateInterval);

	const TTimeIntervalMicroSeconds KTimeOut(4123523);
	updateOptions.SetUpdateTimeOut(KTimeOut);

	const TTimeIntervalMicroSeconds KMaxUpdateAge(3245);
	updateOptions.SetMaxUpdateAge(KMaxUpdateAge);

	CHECK(updateOptions.AcceptPartialUpdates());
	CHECK_EQUAL(KUpdateInterval.Int64(), updateOptions.UpdateInterval().Int64());
	CHECK_EQUAL(KTimeOut.Int64(), updateOptions.UpdateTimeOut().Int64());
	CHECK_EQUAL(KMaxUpdateAge.Int64(), updateOptions.MaxUpdateAge().Int64());
	}

TInt Construct_BadInterval(TAny* /*aPtr*/)
	{
	TPositionUpdateOptions updateOptions(
		TTimeIntervalMicroSeconds(-1), // This should cause a panic
		TTimeIntervalMicroSeconds(1),
		TTimeIntervalMicroSeconds(1)
		);
	(void) updateOptions;
	return KErrGeneral;
	}

TEST(TPositionUpdateOptions, Construct_BadInterval, "PUOP", 1)
	{
	DO_PANIC_TEST_L(Construct_BadInterval, KPosClientFault, 
					EPositionBadTime, KDefaultTestTimeout);
	}

TInt Construct_BadTimeOut(TAny* /*aPtr*/)
	{
	TPositionUpdateOptions updateOptions(
		TTimeIntervalMicroSeconds(1),
		TTimeIntervalMicroSeconds(-1), // This should cause a panic
		TTimeIntervalMicroSeconds(1)
		);
	(void) updateOptions;
	return KErrGeneral;
	}

TEST(TPositionUpdateOptions, Construct_BadTimeOut, "PUOP", 1)
	{
	DO_PANIC_TEST_L(Construct_BadTimeOut, KPosClientFault, 
					EPositionBadTime, KDefaultTestTimeout);
	}

TInt Construct_BadMaxUpdateAge(TAny* /*aPtr*/)
	{
	TPositionUpdateOptions updateOptions(
		TTimeIntervalMicroSeconds(1),
		TTimeIntervalMicroSeconds(1),
		TTimeIntervalMicroSeconds(-1) // This should cause a panic
		);
	(void) updateOptions;
	return KErrGeneral;
	}

TEST(TPositionUpdateOptions, Construct_BadMaxUpdateAge, "PUOP", 1)
	{
	DO_PANIC_TEST_L(Construct_BadMaxUpdateAge, KPosClientFault, 
					EPositionBadTime, KDefaultTestTimeout);
	}

TInt SetMaxUpdateAge_Invalid(TAny* /*aPtr*/)
	{
	TPositionUpdateOptions updateOptions;
	updateOptions.SetMaxUpdateAge(TTimeIntervalMicroSeconds(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TPositionUpdateOptions, SetMaxUpdateAge_Invalid, "PUOP", 2)
	{
	DO_PANIC_TEST_L(SetMaxUpdateAge_Invalid, KPosClientFault, 
					EPositionBadTime, KDefaultTestTimeout);
	}

TInt SetUpdateInterval_Invalid(TAny* /*aPtr*/)
	{
	TPositionUpdateOptions updateOptions;
	updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TPositionUpdateOptions, SetUpdateInterval_Invalid, "PUOP", 3)
	{
	DO_PANIC_TEST_L(SetUpdateInterval_Invalid, KPosClientFault, 
					EPositionBadTime, KDefaultTestTimeout);
	}

TInt SetUpdateTimeOut_Invalid(TAny* /*aPtr*/)
	{
	TPositionUpdateOptions updateOptions;
	updateOptions.SetUpdateTimeOut(TTimeIntervalMicroSeconds(-1)); // This should panic
	return KErrGeneral;
	}

TEST(TPositionUpdateOptions, SetUpdateTimeOut_Invalid, "PUOP", 3)
	{
	DO_PANIC_TEST_L(SetUpdateTimeOut_Invalid, KPosClientFault, 
					EPositionBadTime, KDefaultTestTimeout);
	}
