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
 @file CriteriaClassesStep.cpp
 @internalTechnology
*/
#include "criteriaclassesstep.h"
#include "te_lbsapisuitedefs.h"

#include "lcfsbucommondefinitions.h"

#include <lbs.h>
#include <lbssatellite.h>


CCriteriaClassesStep::~CCriteriaClassesStep()
/**
 * Destructor
 */
	{
	}

CCriteriaClassesStep::CCriteriaClassesStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KCriteriaClassesStep);
	}

TVerdict CCriteriaClassesStep::doTestStepPreambleL()
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


TVerdict CCriteriaClassesStep::doTestStepL()
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



TVerdict CCriteriaClassesStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_LbsApiSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

void CCriteriaClassesStep::StartL(TInt aIndex)
	{
	switch(aIndex)
		{
		case 0:
			TPositionSelectionOrder_ConstructionL();
			break;
		case 1:
			TPositionSelectionOrder_SetL();
			break;
		case 2:
			TPositionSelectionOrder_ClearL();
			break;
		case 3:
			TPositionSelectionOrder_NumSelectionItemsL();
			break;
		case 4:
			TPositionCriteria_ConstructionL();
			break;
		case 5:
			TPositionCriteria_SetL();
			break;
		case 6:
			TPositionCriteria_ResetAndClearL();
			break;
		default:
			break;
		}
	}





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
    TPositionSelectionOrder::EOrderHigh,
    TPositionSelectionOrder::EOrderVeryHigh
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
//
// Position Criteria classes
//
//

// 5.2.1. Construction
void CCriteriaClassesStep::TPositionSelectionOrder_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TPositionSelectionOrder selOrder;
    CHECK( IsNewlyConstructed(selOrder));

    StandardCleanup();
	}

// 5.2.2. Set/Get
void CCriteriaClassesStep::TPositionSelectionOrder_SetL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    __UHEAP_MARK;

    for (TInt i=0; i<ARRAYLENGTH(KTestOrders); i++)
    	{
        for (TInt j=0; j<ARRAYLENGTH(KSetterFunctions); j++)
        	{
        	TPositionSelectionOrder selOrder;
        	selOrder.ClearSelectionOrder();

            // Set field priority
            SetterFuncTYPE setterFunction = KSetterFunctions[j];
            CHECK_EQUAL_( KErrNone, (selOrder.*setterFunction)(KTestOrders[i]));

            // Get & check field priority
            TPositionSelectionOrder::TField field;
            TPositionSelectionOrder::TOrder order;

            CHECK_EQUAL_( KErrNone, selOrder.GetSelectionItem(j, field, order));
            CHECK_EQUAL_( KTestFields[j], field);
            CHECK_EQUAL_( KTestOrders[i], order);
        	}
    	}

    __UHEAP_MARKEND;

    StandardCleanup();
	}

// 5.2.3. Clear & Reset
void CCriteriaClassesStep::TPositionSelectionOrder_ClearL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TPositionSelectionOrder selOrder;
    TPositionSelectionOrder::TField field;
    TPositionSelectionOrder::TOrder order;

    selOrder.ClearSelectionOrder();

    CHECK_EQUAL_( TUint(0), selOrder.NumSelectionItems());

    selOrder.ResetSelectionOrder();

    CHECK_EQUAL_( 2, selOrder.NumSelectionItems());

    for (TInt i=0; i<ARRAYLENGTH(KTestFields); i++)
	    {
	    CHECK_EQUAL_( KErrNone, selOrder.GetSelectionItem(i, field, order));
	    if(i == TPositionSelectionOrder::EFieldHorizontalAccuracy)
	    	{
	    	CHECK_EQUAL_( TPositionSelectionOrder::EOrderHigh, order);
	    	}
	    else if(i == TPositionSelectionOrder::EFieldCost)
	    	{
	    	CHECK_EQUAL_( TPositionSelectionOrder::EOrderMedium, order);
	    	}
	    else
	    	{
	    	CHECK_EQUAL_(0, field);
	    	CHECK_EQUAL_( TPositionSelectionOrder::EOrderDontCare, order);
	    	}
	    
	    }

    StandardCleanup();
	}

// 5.2.5. NumSelectionItems
void CCriteriaClassesStep::TPositionSelectionOrder_NumSelectionItemsL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TPositionSelectionOrder selOrder;
    selOrder.ClearSelectionOrder();

    TInt i;
    for (i=0; i<ARRAYLENGTH(KSetterFunctions); i++)
    	{
        SetterFuncTYPE setterFunction = KSetterFunctions[i];
        CHECK_EQUAL_( KErrNone, (selOrder.*setterFunction)(KTestOrders[i]));
        CHECK_EQUAL_( TUint(i), selOrder.NumSelectionItems());
    	}

    for (i=0; i<ARRAYLENGTH(KSetterFunctions); i++)
    	{
        SetterFuncTYPE setterFunction = KSetterFunctions[i];
        CHECK_EQUAL_( KErrNone, (selOrder.*setterFunction)(KTestOrders[i]));
        CHECK_EQUAL_( (TUint(ARRAYLENGTH(KTestFields))-1), selOrder.NumSelectionItems());
    	}

    StandardCleanup();
	}

//
//
// TPositionCriteria
//
//

// 5.1.1. Construction
void CCriteriaClassesStep::TPositionCriteria_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    // Blank Constructor
    TPositionCriteria blankCriteria;

    // Check the required caps are blank
    CHECK_EQUAL_(
            DEFAULT_CRITERIA_MANDATORY_CAPABILITIES,
            blankCriteria.RequiredCapabilities()
    );

    // Check the TPositionSelectionOrder component is blank / newly constructed
    TPositionSelectionOrder selOrder;
    blankCriteria.GetSelectionOrder(selOrder);
    CHECK( IsNewlyConstructed(selOrder));

    // Check the position quality is blank
    TPositionQuality posQuality;
    blankCriteria.GetRequiredQuality(posQuality);
    CHECK( IsNewlyConstructed(posQuality));

    // Capabilities Constructor
    const TPositionModuleInfo::TCapabilities KCaps =
        TPositionModuleInfo::ECapabilityHorizontal |
        TPositionModuleInfo::ECapabilityVertical |
        TPositionModuleInfo::ECapabilityDirection |
        TPositionModuleInfo::ECapabilityCompass;
    
    TPositionCriteria criteria(KCaps);

    // Check the required caps are blank
    CHECK_EQUAL_( KCaps, criteria.RequiredCapabilities());

    // Check the TPositionSelectionOrder component is blank / newly constructed
    TPositionSelectionOrder selOrder2;
    criteria.GetSelectionOrder(selOrder2);
    CHECK( IsNewlyConstructed(selOrder2));

    // Check the position quality is blank
    TPositionQuality posQuality2;
    criteria.GetRequiredQuality(posQuality2);
    CHECK( IsNewlyConstructed(posQuality2));

    StandardCleanup();
	}

// 5.1.2. Get/Set properties
void CCriteriaClassesStep::TPositionCriteria_SetL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    TPositionCriteria criteria;

    // selction order
    TPositionSelectionOrder order, order2;

    CHECK_EQUAL_(KErrNone, order.SetOrderTimeToFirstFix(TPositionSelectionOrder::EOrderLow));
    CHECK_EQUAL_(KErrNone, order.SetOrderTimeToNextFix(TPositionSelectionOrder::EOrderFairlyHigh));
    CHECK_EQUAL_(KErrNone, order.SetOrderVerticalAccuracy(TPositionSelectionOrder::EOrderFairlyLow));
    CHECK_EQUAL_(KErrNone, order.SetOrderPowerConsumption(TPositionSelectionOrder::EOrderVeryHigh));

    criteria.SetSelectionOrder(order);
    criteria.GetSelectionOrder(order2);
    CHECK( AreEqualL(order, order2));

        // position quality
    TPositionQuality quality, quality2;

    quality.SetTimeToFirstFix(TTimeIntervalMicroSeconds (987));
    quality.SetTimeToNextFix(TTimeIntervalMicroSeconds (654));
    quality.SetHorizontalAccuracy(TReal32 (123.555));
    quality.SetVerticalAccuracy(TReal32 (45.88));
    quality.SetCostIndicator(TPositionQuality::ECostPossible);
    quality.SetPowerConsumption(TPositionQuality::EPowerZero);

    criteria.SetRequiredQuality(quality);
    criteria.GetRequiredQuality(quality2);
    CHECK( AreEqualL(quality, quality2));

    // required capabilities
    TPositionModuleInfo::TCapabilities caps = 0;

    // i ranges over all the bits in a TPositionModuleInfo::TCapabilities
    for (TUint i=0; i<sizeof(TPositionModuleInfo::TCapabilities) * 8; i++)
    	{
        caps |= (1 << i);
        criteria.AddRequiredCapabilities(1 << i);
        CHECK_EQUAL_( caps, criteria.RequiredCapabilities());
    	}

    criteria.ClearRequiredCapabilities();
    CHECK_EQUAL_( TPositionModuleInfo::TCapabilities(0), criteria.RequiredCapabilities());

    StandardCleanup();
	}

// 5.1.3. Reset and Clear
void CCriteriaClassesStep::TPositionCriteria_ResetAndClearL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();
    
    __UHEAP_MARK;

    // create it
    TPositionCriteria criteria( TPositionModuleInfo::ECapabilityHorizontal |
                            TPositionModuleInfo::ECapabilityVertical |
                            TPositionModuleInfo::ECapabilityDirection |
                            TPositionModuleInfo::ECapabilityCompass);

    TPositionSelectionOrder selOrder;
    CHECK_EQUAL_(KErrNone, selOrder.SetOrderTimeToFirstFix(TPositionSelectionOrder::EOrderLow));
    CHECK_EQUAL_(KErrNone, selOrder.SetOrderTimeToNextFix(TPositionSelectionOrder::EOrderFairlyHigh));
    CHECK_EQUAL_(KErrNone, selOrder.SetOrderVerticalAccuracy(TPositionSelectionOrder::EOrderFairlyLow));
    CHECK_EQUAL_(KErrNone, selOrder.SetOrderPowerConsumption(TPositionSelectionOrder::EOrderVeryHigh));
    criteria.SetSelectionOrder(selOrder);

    TPositionQuality quality;
    quality.SetTimeToFirstFix(TTimeIntervalMicroSeconds (987));
    quality.SetTimeToNextFix(TTimeIntervalMicroSeconds (654));
    quality.SetHorizontalAccuracy(TReal32 (123.555));
    quality.SetVerticalAccuracy(TReal32 (45.88));
    quality.SetCostIndicator(TPositionQuality::ECostPossible);
    quality.SetPowerConsumption(TPositionQuality::EPowerZero);
    criteria.SetRequiredQuality(quality);

    // Do a reset
    criteria.ResetCriteria();

    // Check the required caps are blank
    CHECK_EQUAL_(
            DEFAULT_CRITERIA_MANDATORY_CAPABILITIES,
            criteria.RequiredCapabilities());

    // Check the TPositionSelectionOrder component is blank / newly constructed
    TPositionSelectionOrder selOrder2;
    criteria.GetSelectionOrder(selOrder2);
    // TODO Investigate
    //CHECK( IsNewlyConstructed(selOrder2));

    // Check the position quality is blank
    TPositionQuality posQuality;
    criteria.GetRequiredQuality(posQuality);
    CHECK( IsNewlyConstructed(posQuality));

    // Do a clear
    criteria.ClearCriteria();

    // Should not be any selection items in the contains TPositionSelectionOrder
    criteria.GetSelectionOrder(selOrder);
    CHECK_EQUAL_( TUint(0), selOrder.NumSelectionItems());

    __UHEAP_MARKEND;

    StandardCleanup();
	}

//
//
// Utility functions
//
//

// Macro
// Only supposed to be used by the following utility functions
#define RETURN_IF_NOT_EQUAL(aLhs, aRhs) \
    if ((aLhs) != (aRhs))               \
            return EFalse;

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
    RETURN_IF_NOT_EQUAL( TPositionQuality::ECostUnknown, aPosQuality.CostIndicator());
    RETURN_IF_NOT_EQUAL( TPositionQuality::EPowerUnknown, aPosQuality.PowerConsumption());
    if (!Math::IsNaN(aPosQuality.HorizontalAccuracy())) return EFalse;
    if (!Math::IsNaN(aPosQuality.VerticalAccuracy())) return EFalse;
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
