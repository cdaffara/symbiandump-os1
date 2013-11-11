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
#include <LbsCriteria.h>
#include <E32Math.h>

#define ARRAYLENGTH(aArray) (static_cast<TInt>((sizeof(aArray) / sizeof(aArray[0]))))

//
// Constant test values used in many tests
//
const TPositionSelectionOrder::TOrder KTestOrders[] = 
	{
	TPositionSelectionOrder::EOrderVeryLow,
	TPositionSelectionOrder::EOrderLow,
	TPositionSelectionOrder::EOrderFairlyLow,
	TPositionSelectionOrder::EOrderMedium,
	TPositionSelectionOrder::EOrderFairlyHigh,
	TPositionSelectionOrder::EOrderHigh
	};

const TPositionSelectionOrder::TField KTestFields[] =
	{
	TPositionSelectionOrder::EFieldTimeToFirstFix,
	TPositionSelectionOrder::EFieldTimeToNextFix,
	TPositionSelectionOrder::EFieldHorizontalAccuracy,
	TPositionSelectionOrder::EFieldVerticalAccuracy,
	TPositionSelectionOrder::EFieldCost,
	TPositionSelectionOrder::EFieldPower
	};

typedef TInt (TPositionSelectionOrder::*SetterFuncTYPE)(TPositionSelectionOrder::TOrder);

// WARNING: The order of these must match the order of the KTestFields
// array declared above.
const SetterFuncTYPE KSetterFunctions[] =
	{
	&TPositionSelectionOrder::SetOrderTimeToFirstFix,
	&TPositionSelectionOrder::SetOrderTimeToNextFix,
	&TPositionSelectionOrder::SetOrderHorizontalAccuracy,
	&TPositionSelectionOrder::SetOrderVerticalAccuracy,
	&TPositionSelectionOrder::SetOrderCostIndicator,
	&TPositionSelectionOrder::SetOrderPowerConsumption
	};

//
// Constants that reflect the API design
//
#define NUM_SELECTION_CRITERIA    (TUint(ARRAYLENGTH(KSetterFunctions)))
#define DEFAULT_CRITERIA_MANDATORY_CAPABILITIES (static_cast<TPositionModuleInfo::TCapabilities>(TPositionModuleInfo::ECapabilityHorizontal))


//
// Utility function prototypes
//

TBool IsNewlyConstructed(const TPositionSelectionOrder& aSelOrder);
TBool IsNewlyConstructed(const TPositionQuality& aPosQuality);
TBool AreEqualL(const TPositionSelectionOrder& aLhs, const TPositionSelectionOrder& aRhs);
TBool AreEqualL(const TPositionQuality& aLhs, const TPositionQuality& aRhs);


//
// TPositionSelectionOrder Tests
//

TEST(TPositionSelectionOrder, Blank_Construction, "PSEL", 1)
	{
	TPositionSelectionOrder selOrder;
	CHECK( IsNewlyConstructed(selOrder));
	}

TEST(TPositionSelectionOrder, Clear_Reset, "PSEL", 2)
	{
	TPositionSelectionOrder selOrder;
	TPositionSelectionOrder::TField field;
	TPositionSelectionOrder::TOrder order;

	selOrder.ClearSelectionOrder();

	CHECK_EQUAL( TUint(0), selOrder.NumSelectionItems());
	CHECK_EQUAL( KErrNotFound, selOrder.GetSelectionItem(0, field, order));

	selOrder.ResetSelectionOrder();

	CHECK_EQUAL( NUM_SELECTION_CRITERIA, selOrder.NumSelectionItems());

	for (TInt i=0; i<ARRAYLENGTH(KTestFields); i++)
		{
		TPositionSelectionOrder::TField field;
		TPositionSelectionOrder::TOrder order;
		CHECK_EQUAL( KErrNone, selOrder.GetSelectionItem(i, field, order));
		CHECK_EQUAL( KTestFields[i], field);
		CHECK_EQUAL( TPositionSelectionOrder::EOrderDefault, order);
		}
	}

TEST(TPositionSelectionOrder, Set_Get, "PSEL", 3)
	{
	TPositionSelectionOrder selOrder;
	TPositionSelectionOrder::TField field;
	TPositionSelectionOrder::TOrder order;

	selOrder.ClearSelectionOrder();

	for (TInt i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		for (TInt j=0; j<ARRAYLENGTH(KSetterFunctions); j++)
			{
			selOrder.ClearSelectionOrder();

			// Set field priority
			SetterFuncTYPE setterFunction = KSetterFunctions[j];
			CHECK_EQUAL( KErrNone, (selOrder.*setterFunction)(KTestOrders[i]));

			// Get & check field priority
			CHECK_EQUAL( KErrNone, selOrder.GetSelectionItem(0, field, order));
			CHECK_EQUAL( KTestFields[j], field);
			CHECK_EQUAL( KTestOrders[i], order);
			}
		}
	}

TEST(TPositionSelectionOrder, SpecifyAll, "PSEL", 3)
	{
	TPositionSelectionOrder selOrder;
	selOrder.ClearSelectionOrder();

	TInt i;

	// Set a priority for all fields
	for (i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		SetterFuncTYPE setterFunction = KSetterFunctions[i];
		CHECK_EQUAL( KErrNone, (selOrder.*setterFunction)(KTestOrders[i]));
		}

	// Check a priority exists for all fields
	for (i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		TPositionSelectionOrder::TField field;
		TPositionSelectionOrder::TOrder order;
		CHECK_EQUAL( KErrNone, selOrder.GetSelectionItem(i, field, order));
		CHECK_EQUAL( KTestFields[i], field);
		CHECK_EQUAL( KTestOrders[i], order);
		}
	}

TEST(TPositionSelectionOrder, NumSelectionItems, "PSEL", 4)
	{
	TPositionSelectionOrder selOrder;
	selOrder.ClearSelectionOrder();

	TInt i;
	for (i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		SetterFuncTYPE setterFunction = KSetterFunctions[i];
		CHECK_EQUAL( KErrNone, (selOrder.*setterFunction)(KTestOrders[i]));
		CHECK_EQUAL( TUint(i+1), selOrder.NumSelectionItems());
		}

	for (i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		SetterFuncTYPE setterFunction = KSetterFunctions[i];
		CHECK_EQUAL( KErrNone, (selOrder.*setterFunction)(KTestOrders[i]));
		CHECK_EQUAL( TUint(ARRAYLENGTH(KTestFields)), selOrder.NumSelectionItems());
		}
	}

TEST(TPositionSelectionOrder, Bad_TOrder, "PSEL", 5)
	{
	TPositionSelectionOrder selOrder;
	selOrder.ClearSelectionOrder();

	TInt i;
	// Set a priority for all fields
	for (i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		SetterFuncTYPE setterFunction = KSetterFunctions[i];
		CHECK_EQUAL( KErrNone, (selOrder.*setterFunction)(KTestOrders[i]));
		}

	//Have too low an order.
	for(i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		SetterFuncTYPE setterFunction = KSetterFunctions[i];
		CHECK_EQUAL(KErrArgument,
			(selOrder.*setterFunction)
			(TPositionSelectionOrder::TOrder(TPositionSelectionOrder::EOrderDontCare-1)));
		}

	// Check the correct priority exists for all fields
	for (i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		TPositionSelectionOrder::TField field;
		TPositionSelectionOrder::TOrder order;
		CHECK_EQUAL( KErrNone, selOrder.GetSelectionItem(i, field, order));
		CHECK_EQUAL( KTestFields[i], field);
		CHECK_EQUAL( KTestOrders[i], order);
		}
	CHECK_EQUAL( NUM_SELECTION_CRITERIA, selOrder.NumSelectionItems());

	//Have too high an order
	for(i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		SetterFuncTYPE setterFunction = KSetterFunctions[i];
		CHECK_EQUAL(KErrArgument,
			(selOrder.*setterFunction)
			(TPositionSelectionOrder::TOrder(TPositionSelectionOrder::EOrderVeryHigh+1)));
		}

	// Check the correct priority exists for all fields
	for (i=0; i<ARRAYLENGTH(KTestOrders); i++)
		{
		TPositionSelectionOrder::TField field;
		TPositionSelectionOrder::TOrder order;
		CHECK_EQUAL( KErrNone, selOrder.GetSelectionItem(i, field, order));
		CHECK_EQUAL( KTestFields[i], field);
		CHECK_EQUAL( KTestOrders[i], order);
		}
	CHECK_EQUAL( NUM_SELECTION_CRITERIA, selOrder.NumSelectionItems());

	}

//
// TPositionCriteria Tests
//

TEST(TPositionCriteria, Blank_Construction, "PCRI", 1)
	{
	TPositionCriteria criteria;

	// Check the required caps are blank
	CHECK_EQUAL( 
		DEFAULT_CRITERIA_MANDATORY_CAPABILITIES,
		criteria.RequiredCapabilities()
	);

	// Check the TPositionSelectionOrder component is blank / newly constructed
	TPositionSelectionOrder selOrder;
	criteria.GetSelectionOrder(selOrder);
	CHECK( IsNewlyConstructed(selOrder));

	// Check the position quality is blank
	TPositionQuality posQuality;
	criteria.GetRequiredQuality(posQuality);
	CHECK( IsNewlyConstructed(posQuality));
	}

TEST(TPositionCriteria, Construction, "PCRI", 1)
	{
	const TPositionModuleInfo::TCapabilities KCaps = 
		TPositionModuleInfo::ECapabilityHorizontal
		| TPositionModuleInfo::ECapabilityVertical	
		| TPositionModuleInfo::ECapabilityDirection
		| TPositionModuleInfo::ECapabilityCompass;

	TPositionCriteria criteria(KCaps);

	// Check the required caps are blank
	CHECK_EQUAL( KCaps, criteria.RequiredCapabilities());

	// Check the TPositionSelectionOrder component is blank / newly constructed
	TPositionSelectionOrder selOrder;
	criteria.GetSelectionOrder(selOrder);
	CHECK( IsNewlyConstructed(selOrder));

	// Check the position quality is blank
	TPositionQuality posQuality;
	criteria.GetRequiredQuality(posQuality);
	CHECK( IsNewlyConstructed(posQuality));
	}

TEST(TPositionCriteria, SetGet_SelectionOrder, "PCRI", 2)
	{
	TPositionCriteria criteria;
	TPositionSelectionOrder original;

	User::LeaveIfError(original.SetOrderTimeToFirstFix(TPositionSelectionOrder::EOrderLow));
	User::LeaveIfError(original.SetOrderTimeToNextFix(TPositionSelectionOrder::EOrderFairlyHigh));
	User::LeaveIfError(original.SetOrderVerticalAccuracy(TPositionSelectionOrder::EOrderFairlyLow));
	User::LeaveIfError(original.SetOrderPowerConsumption(TPositionSelectionOrder::EOrderVeryHigh));
	criteria.SetSelectionOrder(original);

	TPositionSelectionOrder reference;
	criteria.GetSelectionOrder(reference);

	CHECK( AreEqualL(original, reference));	
	}

TEST(TPositionCriteria, SetGet_PositionQuality, "PCRI", 3)
	{
	TPositionCriteria criteria;
	TPositionQuality original;
	original.SetTimeToFirstFix(TTimeIntervalMicroSeconds (987));
	original.SetTimeToNextFix(TTimeIntervalMicroSeconds (654));
	original.SetHorizontalAccuracy(TReal32 (123.555));
	original.SetVerticalAccuracy(TReal32 (45.88));
	original.SetCostIndicator(TPositionQuality::ECostPossible);
	original.SetPowerConsumption(TPositionQuality::EPowerZero);
	criteria.SetRequiredQuality(original);

	TPositionQuality reference;
	criteria.GetRequiredQuality(reference);

	CHECK( AreEqualL(original, reference));	
	}

TEST(TPositionCriteria, SetGet_RequiredCapabilities, "PCRI", 4)
	{
	TPositionCriteria criteria;
	TPositionModuleInfo::TCapabilities caps = 0;

	// i ranges over all the bits in a TPositionModuleInfo::TCapabilities
	for (TUint i=0; i<sizeof(TPositionModuleInfo::TCapabilities) * 8; i++)
		{
		caps |= (1 << i);
		criteria.AddRequiredCapabilities(1 << i);
		CHECK_EQUAL( caps, criteria.RequiredCapabilities());
		}

	criteria.ClearRequiredCapabilities();
	CHECK_EQUAL( TPositionModuleInfo::TCapabilities(0), criteria.RequiredCapabilities());
	}

TEST(TPositionCriteria, ResetAndClear, "PCRI", 5)
	{
	TPositionCriteria criteria(
		TPositionModuleInfo::ECapabilityHorizontal
		| TPositionModuleInfo::ECapabilityVertical	
		| TPositionModuleInfo::ECapabilityDirection
		| TPositionModuleInfo::ECapabilityCompass);

		{
		TPositionSelectionOrder original;
		User::LeaveIfError(original.SetOrderTimeToFirstFix(TPositionSelectionOrder::EOrderLow));
		User::LeaveIfError(original.SetOrderTimeToNextFix(TPositionSelectionOrder::EOrderFairlyHigh));
		User::LeaveIfError(original.SetOrderVerticalAccuracy(TPositionSelectionOrder::EOrderFairlyLow));
		User::LeaveIfError(original.SetOrderPowerConsumption(TPositionSelectionOrder::EOrderVeryHigh));
		criteria.SetSelectionOrder(original);
		}

		{
		TPositionQuality original;
		original.SetTimeToFirstFix(TTimeIntervalMicroSeconds (987));
		original.SetTimeToNextFix(TTimeIntervalMicroSeconds (654));
		original.SetHorizontalAccuracy(TReal32 (123.555));
		original.SetVerticalAccuracy(TReal32 (45.88));
		original.SetCostIndicator(TPositionQuality::ECostPossible);
		original.SetPowerConsumption(TPositionQuality::EPowerZero);
		criteria.SetRequiredQuality(original);
		}

	// Do a reset
	criteria.ResetCriteria();

	// Check the required caps are blank
	CHECK_EQUAL(
		DEFAULT_CRITERIA_MANDATORY_CAPABILITIES,
		criteria.RequiredCapabilities()
	);

	// Check the TPositionSelectionOrder component is blank / newly constructed
	TPositionSelectionOrder selOrder;
	criteria.GetSelectionOrder(selOrder);
	CHECK( IsNewlyConstructed(selOrder));

	// Check the position quality is blank
	TPositionQuality posQuality;
	criteria.GetRequiredQuality(posQuality);
	CHECK( IsNewlyConstructed(posQuality));

	// Do a clear
	criteria.ClearCriteria();

	// Should not be any selection items in the contains TPositionSelectionOrder
	TPositionSelectionOrder::TField field;
	TPositionSelectionOrder::TOrder order;
	criteria.GetSelectionOrder(selOrder);
	CHECK_EQUAL( TUint(0), selOrder.NumSelectionItems());
	CHECK_EQUAL( KErrNotFound, selOrder.GetSelectionItem(0, field, order));
	}



//
// Utility functions
//

// Macro
// Only supposed to be used by the following utility functions
#define RETURN_IF_NOT_EQUAL(aLhs, aRhs) \
	if ((aLhs) != (aRhs))               \
		return EFalse

TBool IsNewlyConstructed(const TPositionSelectionOrder& aSelOrder)
	{
	RETURN_IF_NOT_EQUAL( NUM_SELECTION_CRITERIA, aSelOrder.NumSelectionItems());

	for (TInt i=0; i<ARRAYLENGTH(KTestFields); i++)
		{
		TPositionSelectionOrder::TField field;
		TPositionSelectionOrder::TOrder order;
		RETURN_IF_NOT_EQUAL( KErrNone, aSelOrder.GetSelectionItem(i, field, order));
		RETURN_IF_NOT_EQUAL( KTestFields[i], field);
		RETURN_IF_NOT_EQUAL( TPositionSelectionOrder::EOrderDefault, order);
		}
	return ETrue;
	}

TBool IsNewlyConstructed(const TPositionQuality& aPosQuality)
	{
	RETURN_IF_NOT_EQUAL( TTimeIntervalMicroSeconds(0), aPosQuality.TimeToFirstFix());
	RETURN_IF_NOT_EQUAL( TTimeIntervalMicroSeconds(0), aPosQuality.TimeToNextFix()); 
	RETURN_IF_NOT_EQUAL( !Math::IsNaN(aPosQuality.HorizontalAccuracy()), EFalse);
	RETURN_IF_NOT_EQUAL( !Math::IsNaN(aPosQuality.VerticalAccuracy()), EFalse);
	RETURN_IF_NOT_EQUAL( TPositionQuality::ECostUnknown, aPosQuality.CostIndicator());
	RETURN_IF_NOT_EQUAL( TPositionQuality::EPowerUnknown, aPosQuality.PowerConsumption());
	return ETrue;
	}

TBool AreEqualL(const TPositionSelectionOrder& aLhs, const TPositionSelectionOrder& aRhs)
/**
 * If this function leaves, it would fail the test
 */
	{
	RETURN_IF_NOT_EQUAL( aLhs.NumSelectionItems(), aRhs.NumSelectionItems());
	TUint num = aLhs.NumSelectionItems();
	for (TUint i=0; i<num; i++)
		{
		TPositionSelectionOrder::TField field1;
		TPositionSelectionOrder::TOrder order1;
		User::LeaveIfError(aLhs.GetSelectionItem(0, field1, order1));

		TUint j;
		for (j=0; j<num; j++)
			{
			TPositionSelectionOrder::TField field2;
			TPositionSelectionOrder::TOrder order2;
			User::LeaveIfError(aRhs.GetSelectionItem(0, field2, order2));
			if (field1 == field2 && order1 == order2)
				{
				break;
				}
			}

		if (j == num)
			{
			return EFalse;
			}
		}
	return ETrue;
	}

TBool AreEqualL(const TPositionQuality& aLhs, const TPositionQuality& aRhs)
/**
 * If this function leaves, it would fail the test
 */
	{
	RETURN_IF_NOT_EQUAL( aLhs.TimeToFirstFix(), aRhs.TimeToFirstFix());
	RETURN_IF_NOT_EQUAL( aLhs.TimeToNextFix(), aRhs.TimeToNextFix());
	RETURN_IF_NOT_EQUAL( aLhs.HorizontalAccuracy(), aRhs.HorizontalAccuracy());
	RETURN_IF_NOT_EQUAL( aLhs.VerticalAccuracy(), aRhs.VerticalAccuracy());
	RETURN_IF_NOT_EQUAL( aLhs.CostIndicator(), aRhs.CostIndicator());
	RETURN_IF_NOT_EQUAL( aLhs.PowerConsumption(), aRhs.PowerConsumption());
	return ETrue;
	}
