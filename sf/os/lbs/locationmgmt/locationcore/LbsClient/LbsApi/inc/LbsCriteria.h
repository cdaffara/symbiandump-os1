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

#ifndef __LBSCRITERIA_H__
#define __LBSCRITERIA_H__

#include <lbscommon.h>


class TPositionSelectionOrder
/**
This class is used as part of the TPositionCriteria class to chose a
positioning module that will provide the desired quality of information.
It allows position-quality-metrics to be given priorities, enabling them
to be given preference in the decision.

@publishedAll
@released
 */
	{
public:
	/**
	 Specifies the priority of selection parameters.
	 */
	enum TOrder
		{
		/** Don't care */
		EOrderDontCare   = 0,
		/** Default */
		EOrderDefault    = 1,
		/** Very low */
		EOrderVeryLow    = 25,
		/** Low */
		EOrderLow        = 50,
		/** Fairly low */
		EOrderFairlyLow  = 75,
		/** Medium */
		EOrderMedium     = 100,
		/** Fairly high */
		EOrderFairlyHigh = 125,
		/** High */
		EOrderHigh       = 150,
		/** Very high */
		EOrderVeryHigh   = 175
		};

	/**
	 Specifies the quality of position metrics that can be prioritized.
	 */
	enum TField
		{
		/** Time to first fix */
		EFieldTimeToFirstFix,
		/** Time to next fix */
		EFieldTimeToNextFix,
		/** Field Horizontal Accuracy */
		EFieldHorizontalAccuracy,
		/** Field Vertical Accuracy */
		EFieldVerticalAccuracy,
		/** Field cost */
		EFieldCost,
		/** Field power */
		EFieldPower,
		/** Number of fields */
		EFieldNumFields,       // Do not use this value. Must appear after all other field values.
		/** Last field */
		EFieldLast = KMaxTInt8 // Do not use this value. Must appear at end.
		};

	IMPORT_C TPositionSelectionOrder();

	IMPORT_C TInt SetOrderTimeToFirstFix(TOrder aOrder);
	IMPORT_C TInt SetOrderTimeToNextFix(TOrder aOrder);

	IMPORT_C TInt SetOrderHorizontalAccuracy(TOrder aOrder);
	IMPORT_C TInt SetOrderVerticalAccuracy(TOrder aOrder);

	IMPORT_C TInt SetOrderCostIndicator(TOrder aOrder);
	IMPORT_C TInt SetOrderPowerConsumption(TOrder aOrder);

	IMPORT_C void ResetSelectionOrder();
	IMPORT_C void ClearSelectionOrder();

	IMPORT_C TUint NumSelectionItems() const;
	IMPORT_C TInt GetSelectionItem(TUint aIndex,
	                               TField& aField,
	                               TOrder& aOrder) const;

protected:
	struct SItem
	/**
	 Stores a field / priority pair.
	 */
		{
		/** A position quality metric identifier. */
		TField iField;
		/** The priority given to the iField position quality metric. */
		TOrder iOrder;
		};

protected:
	/**
	An internal method to add a selection criteria choice to the current set of
	criteria.
	 */
	TInt AddItem(TField aField, TOrder aOrder);

protected:
	/** The number of selection items in the iItems array. */
	TUint iNumItems;
	/** Array of users position quality priorities. */
	SItem iItems[KPositionMaxSectionFields];
	};

/**
TPositionCriteriaBase derived classes class types
// If the following line generates an error, too many fields have been added
// to the TPositionSelectionOrder::TOrder enumeration, some must be removed
// to maintain BC.
@publishedAll
@released
 */
POSITION_COMPILE_TIME_ASSERT(TPositionSelectionOrder::EFieldNumFields <= KPositionMaxSectionFields);


class TPositionCriteriaBase : public TPositionClassTypeBase
/**
The base class for classes used to store position module selection
criteria information.

@publishedAll
@released
 */
	{
protected:
	IMPORT_C TPositionCriteriaBase();
	IMPORT_C TPositionCriteriaBase(TPositionModuleInfo::TCapabilities aCapabilityMask);

public:
	IMPORT_C void AddRequiredCapabilities(TPositionModuleInfo::TCapabilities aCapabilityMask);
	IMPORT_C void ClearRequiredCapabilities();
	IMPORT_C TPositionModuleInfo::TCapabilities RequiredCapabilities() const;

	IMPORT_C void SetRequiredQuality(const TPositionQuality& aPosQuality);
	IMPORT_C void GetRequiredQuality(TPositionQuality& aPosQuality) const;

	IMPORT_C void SetSelectionOrder(const TPositionSelectionOrder& aSelectionOrder); 
	IMPORT_C void GetSelectionOrder(TPositionSelectionOrder& aSelectionOrder) const;

	IMPORT_C void ResetCriteria();
	IMPORT_C void ClearCriteria();

protected:
	/** The capabilities a positioning module must have. */
	TPositionModuleInfo::TCapabilities iRequiredCaps;
	/** The quality required of a positioning module. */
	TPositionQuality iRequiredQuality;
	/** The priorities assigned to the various quality metrics. */
	TPositionSelectionOrder	iSelectionOrder;
	};


class TPositionCriteria : public TPositionCriteriaBase
/**
The standard class criteria class for selecting a positioning module.

@publishedAll
@released
 */
	{
public:
	IMPORT_C TPositionCriteria();
	IMPORT_C TPositionCriteria(TPositionModuleInfo::TCapabilities aCapabilityMask);

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[4];
	};

#endif // __LBSCRITERIA_H__
