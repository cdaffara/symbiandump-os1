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

#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <lbserrors.h>
#include <lbscriteria.h>
#include "LbsInternal.h"


//------------------------------------------------------------------------------
// TPositionSelectionOrder
//------------------------------------------------------------------------------
EXPORT_C TPositionSelectionOrder::TPositionSelectionOrder()
/**
Default constructor for TPositionSelectionOrder.

*/
	{
	SetOrderTimeToFirstFix(EOrderDefault);
	SetOrderTimeToNextFix(EOrderDefault);
	SetOrderHorizontalAccuracy(EOrderDefault);
	SetOrderVerticalAccuracy(EOrderDefault);
	SetOrderCostIndicator(EOrderDefault);
	SetOrderPowerConsumption(EOrderDefault);
	iNumItems = 6; // all 6 identical priorities need to be looked at!
	}

EXPORT_C void TPositionSelectionOrder::ResetSelectionOrder()
/**
Used to restore the default selection ordering.

*/
	{
	// initalize to zero
	Mem::FillZ(this, sizeof(TPositionSelectionOrder));
	// then patch the bits we need
	// NB this says nothing about the "vertical" accuracy
	SetOrderHorizontalAccuracy(EOrderHigh);
	SetOrderCostIndicator(EOrderMedium);
	// now patch iNumItems
	iNumItems = 2;
	}

EXPORT_C void TPositionSelectionOrder::ClearSelectionOrder()
/**
Clear all values including defaults. This is useful if the application
wishes to specify its own priorities.

*/
	{
	// clear everything
	Mem::FillZ(this, sizeof(TPositionSelectionOrder));
	}

TInt TPositionSelectionOrder::AddItem(TField aField, TOrder aOrder)
/**
An internal method to add a selection criteria choice to the current set of
criteria.

@internalTechnology
@param aField is a position quality metric, eg. horizontal accuracy
@param aOrder is the priority for the given quality metric.
@return a Symbian OS error code.
 */
	{
	//Range check
	if(aOrder<EOrderDontCare || aOrder>EOrderVeryHigh)
		return KErrArgument;
	if(aField<EFieldTimeToFirstFix || aField>EFieldPower)
		return KErrArgument;
	// is this a new field being added?
	if(iItems[aField].iField != aField)
		{
		// NB number of items in the array has NO relation to the position of the item.
		++iNumItems; // yes, so increase the number of revelent fields
		}
	iItems[aField].iField = aField;
	iItems[aField].iOrder = aOrder;
	
	return KErrNone;
	}


EXPORT_C TInt TPositionSelectionOrder::SetOrderTimeToFirstFix(TOrder aOrder)
/**
Assigns a priority to the time to first fix when selecting a positioning
module.

@param aOrder is the priority of the time to first fix when choosing a
positioning module.
@return a Symbian OS error code.
 */
	{
	return AddItem(EFieldTimeToFirstFix, aOrder);
	}

EXPORT_C TInt TPositionSelectionOrder::SetOrderTimeToNextFix(TOrder aOrder)
/**
Assigns a priority to the time to subsequent fixes when selecting a
positioning module.

@param aOrder is the priority of the time to subsequent fixes when choosing
a positioning module.
@return a Symbian OS error code.
 */
	{
	return AddItem(EFieldTimeToNextFix, aOrder);
	}

EXPORT_C TInt TPositionSelectionOrder::SetOrderHorizontalAccuracy(TOrder aOrder)
/**
Assigns a priority to the horizontal accuracy when selecting a positioning
module.

@param aOrder is the priority of horizontal accuracy when choosing a
positioning module.
@return a Symbian OS error code.
 */
	{
	return AddItem(EFieldHorizontalAccuracy, aOrder);
	}

EXPORT_C TInt TPositionSelectionOrder::SetOrderVerticalAccuracy(TOrder aOrder)
/**
Assigns a priority to the vertical accuracy when selecting a positioning
module.

@param aOrder is the priority of vertical accuracy when choosing a
positioning module.
@return a Symbian OS error code.
 */
	{
	return AddItem(EFieldVerticalAccuracy, aOrder);
	}

EXPORT_C TInt TPositionSelectionOrder::SetOrderCostIndicator(TOrder aOrder)
/**
Assigns a priority to the cost of a positioning technology when selecting
a positioning module.

@param aOrder is the priority of cost when choosing a positioning module.
@return a Symbian OS error code.
 */
	{
	return AddItem(EFieldCost, aOrder);
	}

EXPORT_C TInt TPositionSelectionOrder::SetOrderPowerConsumption(TOrder aOrder)
/**
Assigns a priority to the power consumption of a positioning technology
when selecting a positioning module.

@param aOrder is the priority of power consumption when choosing a
positioning module.
@return a Symbian OS error code.
 */
	{
	return AddItem(EFieldPower, aOrder);
	}

EXPORT_C TUint TPositionSelectionOrder::NumSelectionItems() const
/**
Not generally called by client applications. It retrieves the number of
selection parameters.

@return the number of selection items.
 */
	{
	return iNumItems;
	}

EXPORT_C TInt TPositionSelectionOrder::GetSelectionItem(TUint aIndex,
                                                        TField& aField,
                                                        TOrder& aOrder) const
/**
Not generally called by client applications. It retrieves a particular
selection parameter.

@param aIndex the index of a selection parameter
@param aField is set, upon successful completion, to the field ID at
index aIndex.
@param aOrder is set, upon successful completion, to the priority of the
field at index aIndex.
@return a Symbian OS error code.
 */
	{
	if (aIndex >= sizeof(iItems)/sizeof(SItem))
		{
		return KErrNotFound; // index out of bounds
		}

	const SItem& thisItem = iItems[aIndex];

	aField = thisItem.iField;
	aOrder = thisItem.iOrder;

	return KErrNone;
	}

//------------------------------------------------------------------------------
// TPositionCriteriaBase
//------------------------------------------------------------------------------
EXPORT_C TPositionCriteriaBase::TPositionCriteriaBase()
/**
Constructor for the abstract base class TPositionCriteriaBase

*/
 : iRequiredCaps(TPositionModuleInfo::ECapabilityHorizontal)
	{}

EXPORT_C TPositionCriteriaBase::TPositionCriteriaBase(TPositionModuleInfo::TCapabilities aCapabilityMask)
/**
Constructor for TPositionCriteriaBase that specifies mandatory additional
capabilities of the positioning module used to provide the application with
position information.

@param aCapabilityMask is a bit mask of values from the enumeration
TPositionModuleInfo::_TCapabilities. Defaults to TPositionModuleInfo::ECapabilityHorizontal.
 */
 : iRequiredCaps(aCapabilityMask)
	{}

EXPORT_C void TPositionCriteriaBase::AddRequiredCapabilities(TPositionModuleInfo::TCapabilities aCapabilityMask)
/**
Specifies mandatory additional capabilities of the positioning module used
to provide the application with position information.

@param aCapabilityMask is a bit mask of values from the enumeration
 * TPositionModuleInfo::_TCapabilities.
 */
	{
	iRequiredCaps |= aCapabilityMask;
	}

EXPORT_C void TPositionCriteriaBase::ClearRequiredCapabilities()
/**
Clears the mandatory additional capabilities of the positioning module used
to provide the application with position information.

*/
	{
	iRequiredCaps = TPositionModuleInfo::ECapabilityNone;
	}

EXPORT_C void TPositionCriteriaBase::ResetCriteria()
/**
Used to restore the criteria to the default values. This includes the
required capabilities, quality of position information and selection
ordering details.

*/
	{
	iRequiredCaps = TPositionModuleInfo::ECapabilityHorizontal;
	iRequiredQuality = TPositionQuality();
	iSelectionOrder.ResetSelectionOrder();
	}

EXPORT_C void TPositionCriteriaBase::ClearCriteria()
/**
Removes all criteria including the default values. This includes all
required capabilities, all quality of position information and all selection
ordering details. This is useful if horizontal position information is
not required by the application.

*/
	{
	iRequiredCaps = 0;
	iRequiredQuality = TPositionQuality();
	iSelectionOrder.ClearSelectionOrder();
	}

EXPORT_C TPositionModuleInfo::TCapabilities TPositionCriteriaBase::RequiredCapabilities() const
/**
Returns the current set of required criteria including the default.

@return the current set of required criteria including the default.
 */
	{
	return iRequiredCaps;
	}

EXPORT_C void TPositionCriteriaBase::SetRequiredQuality(const TPositionQuality& aPosQuality)
/**
Used to specify the mandatory upper bound for the quality of information that
the application will accept.

@param aPosQuality is the new required quality of positioning information.
 */
	{
	iRequiredQuality = aPosQuality;
	}

EXPORT_C void TPositionCriteriaBase::GetRequiredQuality(TPositionQuality& aPosQuality) const
/**
Returns any currently specified quality limits.

@param aPosQuality will be set to the required quality of positioning
information.
 */
	{
	aPosQuality = iRequiredQuality;
	}

EXPORT_C void TPositionCriteriaBase::SetSelectionOrder(const TPositionSelectionOrder& aSelectionOrder)
/**
Sets the selection order to be used by the positioning server in
selecting a positioning module.

@param aSelectionOrder is the new selection ordering to be used.
 */
	{
	iSelectionOrder = aSelectionOrder;
	}

EXPORT_C void TPositionCriteriaBase::GetSelectionOrder(TPositionSelectionOrder& aSelectionOrder) const
/**
Retrieves the current selection order that will be used by the positioning
server in selecting a positioning module.

@param aSelectionOrder will be set to the current selection ordering.
 */
	{
	aSelectionOrder = iSelectionOrder;
	}

//------------------------------------------------------------------------------
// TPositionCriteria
//------------------------------------------------------------------------------
EXPORT_C TPositionCriteria::TPositionCriteria()
/**
Default constructor for TPositionCriteria.

*/
 : TPositionCriteriaBase()
	{
	iPosClassType |= EPositionCriteriaClass;
	iPosClassSize = sizeof(TPositionCriteria);
	}

EXPORT_C TPositionCriteria::TPositionCriteria(TPositionModuleInfo::TCapabilities aCapabilityMask)
/**
Constructor for TPositionCriteria that specifies mandatory additional
capabilities of the positioning module used to provide the application with
position information.

@param aCapabilityMask is a bit mask of values from the enumeration
TPositionModuleInfo::_TCapabilities. Defaults to TPositionModuleInfo::ECapabilityHorizontal.
 */
 : TPositionCriteriaBase(aCapabilityMask)
	{
	iPosClassType |= EPositionCriteriaClass;
	iPosClassSize = sizeof(TPositionCriteria);
	}
