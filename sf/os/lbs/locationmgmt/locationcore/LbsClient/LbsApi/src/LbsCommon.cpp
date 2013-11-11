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
#include "LbsInternal.h"
#include <lbscriteria.h>

// This macro asserts that the class aClassType has size equal to aSize.
#define __ASSERT_CLASS_SIZE(aClassType, aSize) \
		__ASSERT_ALWAYS(sizeof(aClassType)==(aSize), \
		User::Panic(KPosClientFault, EPositionClassSizeMismatch))

// This should be the first thing called in the derived class' DoReset method which must
// be implemented. 
#define __TPOSITION_MODULE_STATUS_EVENT_RESET_BASE(aThisClass, aExpectedSize, aFirstBaseClass) \
		__ASSERT_CLASS_SIZE(aThisClass, (aExpectedSize)); \
		aFirstBaseClass::DoReset(sizeof(aFirstBaseClass))


//------------------------------------------------------------------------------
// TPositionClassTypeBase
//------------------------------------------------------------------------------
TPositionClassTypeBase::TPositionClassTypeBase()
/**
Default constructor for TPositionClassTypeBase

@internalComponent
 */
:	iPosClassType(0),
	iPosClassSize(0)
	{}

EXPORT_C TUint TPositionClassTypeBase::PositionClassSize() const
/**
PositionClassSize

@return The size of the TPositionClassTypeBase-derived class
 */
	{
	return iPosClassSize;
	}


EXPORT_C TUint32 TPositionClassTypeBase::PositionClassType() const
/**
PositionClassTypeType

@return The type of the TPositionClassTypeBase-derived class
 */
	{
	return iPosClassType;
	}

//-----------------------------------------------------------------------------
// TPositionModuleInfoBase
//-----------------------------------------------------------------------------
EXPORT_C TPositionModuleInfoBase::TPositionModuleInfoBase()
/**
Default constructor for TPositionModuleInfoBase

 */
:	TPositionClassTypeBase()
	{}

//-----------------------------------------------------------------------------
// TPositionModuleInfo
//-----------------------------------------------------------------------------
EXPORT_C TPositionModuleInfo::TPositionModuleInfo()
/**
Default constructor for TPositionModuleInfo

 */
:	TPositionModuleInfoBase(),
	iModuleId(KPositionNullModuleId),
	iIsAvailable(EFalse),
	iModuleName(),
	iPosQuality(),
	iTechnologyType(ETechnologyUnknown),
	iDeviceLocation(EDeviceUnknown),
	iCapabilities(ECapabilityNone),
	iSupportedClassTypes(),
	iVersion()
	//TVersion has default constructor that sets iVersion to (0,0,0)
	{
	iPosClassType |= EPositionModuleInfoClass;
	iPosClassSize = sizeof(TPositionModuleInfo);
	iSupportedClassTypes.Reset();
	}

EXPORT_C TPositionModuleId TPositionModuleInfo::ModuleId() const
/**
Module Id
@return The module ID
 */
	{
	return iModuleId;
	}

EXPORT_C TBool TPositionModuleInfo::IsAvailable() const
/**
Returns Availablity
@return Whether or not this module is available
 */
	{
	return iIsAvailable;
	}

EXPORT_C void TPositionModuleInfo::GetModuleName(TDes& aModuleName) const
/**
Gets Module Name
@param aModuleName The descriptor where the module's name will be placed
 */
	{
	aModuleName = iModuleName;
	}

EXPORT_C void TPositionModuleInfo::GetPositionQuality(
	TPositionQuality& aPosQuality) const
/**
Gets Position Quality
@param aPosQuality The quality of the position
 */
	{
	aPosQuality = iPosQuality;
	}

EXPORT_C TPositionModuleInfo::TTechnologyType
	TPositionModuleInfo::TechnologyType() const
/**
Technology Type
@return The technology type
 */
	{
	return iTechnologyType;
	}

EXPORT_C TPositionModuleInfo::TDeviceLocation
	TPositionModuleInfo::DeviceLocation() const
/**
Device Location
@return The location of the positioning device associated with this module
 */
	{
	return iDeviceLocation;
	}

EXPORT_C TPositionModuleInfo::TCapabilities
	TPositionModuleInfo::Capabilities() const
/**
Capabilities
@return The capabilities of this module
 */
	{
	return iCapabilities;
	}

EXPORT_C TUint32 TPositionModuleInfo::ClassesSupported(TPositionClassFamily aClassType) const
/**
Classes Supported
@param aClassType Classes Supported
@return The classes supported
 */
	{
	if(aClassType<EPositionLastFamily)
		{
		return iSupportedClassTypes[aClassType];
		}
	else
		{
		User::Panic(KPosClientFault, EPositionInvalidClassType);
		return 0;	//Needed to stop compiler warning
		}
	}

EXPORT_C TVersion TPositionModuleInfo::Version() const
/**
Module Version
@return The version of the module
 */
	{
	return iVersion;
	}

EXPORT_C void TPositionModuleInfo::SetModuleId(TPositionModuleId aModuleId)
/**
Sets module Id
@param aModuleId The module ID to set
 */
	{
	iModuleId = aModuleId;
	}

EXPORT_C void TPositionModuleInfo::SetIsAvailable(TBool aIsAvailable)
/**
Sets module availablity
@param aIsAvailable Whether or not the module is available for use
 */
	{
	iIsAvailable = aIsAvailable;
	}

EXPORT_C void TPositionModuleInfo::SetModuleName(const TDesC& aModuleName)
/**
Sets module name
@param aModuleName The module name to set
 */
	{
	iModuleName = aModuleName;
	}

EXPORT_C void TPositionModuleInfo::SetPositionQuality(
	const TPositionQuality& aPosQuality)
/**
Sets position quality
@param aPosQuality The position quality to set
 */
	{
	iPosQuality = aPosQuality;
	}

EXPORT_C void TPositionModuleInfo::SetTechnologyType(
	TTechnologyType aTechnologyType)
/**
Sets technology type
@param aTechnologyType The technology type to set
 */
	{
	iTechnologyType = aTechnologyType;
	}

EXPORT_C void TPositionModuleInfo::SetDeviceLocation(
	TDeviceLocation aDeviceLocation)
/**
Sets device location
@param aDeviceLocation The positioning device location to set
 */
	{
	iDeviceLocation = aDeviceLocation;
	}

EXPORT_C void TPositionModuleInfo::SetCapabilities(TCapabilities aCapabilities)
/**
Sets capabilities
@param aCapabilities Bitfield of the capabilities of this module to set
 */
	{
	iCapabilities = aCapabilities;
	}

EXPORT_C void TPositionModuleInfo::SetClassesSupported(TPositionClassFamily aClassType,
												       TUint32 aSupportedClasses)
/**
Sets supported classes
@param aClassType The class type
 * @param aSupportedClasses The supported classes
 */
	{
	if(aClassType<EPositionLastFamily)
		{
		iSupportedClassTypes[aClassType] = aSupportedClasses;
		}
	else
		{
		User::Panic(KPosClientFault, EPositionInvalidClassType);
		}
	}

EXPORT_C void TPositionModuleInfo::SetVersion(TVersion aVersion)
/**
Sets Version
@param aVersion The version to set
 */
	{
	iVersion = aVersion;
	}

//-----------------------------------------------------------------------------
// TPositionModuleStatusBase
//-----------------------------------------------------------------------------
EXPORT_C TPositionModuleStatusBase::TPositionModuleStatusBase()
/**
Default constructor for TPositionModuleStatusBase

 */
:	TPositionClassTypeBase()
	{}

//-----------------------------------------------------------------------------
// TPositionModuleStatus
//-----------------------------------------------------------------------------
EXPORT_C TPositionModuleStatus::TPositionModuleStatus()
/**
Default constructor for TPositionModuleStatus

 */
:	TPositionModuleStatusBase(),
	iDeviceStatus(EDeviceUnknown),
	iDataQualityStatus(EDataQualityUnknown)
	{
	iPosClassType |= EPositionModuleStatusClass;
	iPosClassSize = sizeof(TPositionModuleStatus);
	}


EXPORT_C TPositionModuleStatus::TDeviceStatus
	TPositionModuleStatus::DeviceStatus() const
/**
Device status
@return The module's device status
 */
	{
	return iDeviceStatus;
	}

EXPORT_C TPositionModuleStatus::TDataQualityStatus
	TPositionModuleStatus::DataQualityStatus() const
/**
Data quality status
@return  The data quality status
 */
	{
	return iDataQualityStatus;
	}


EXPORT_C void TPositionModuleStatus::SetDeviceStatus(TDeviceStatus aStatus)
/**
Sets device status
@param aStatus The device status to set
 */
	{
	iDeviceStatus = aStatus;
	}

EXPORT_C void TPositionModuleStatus::SetDataQualityStatus(TDataQualityStatus aStatus)
/**
Sets data quality status
@param aStatus The data quality status to set
 */
	{
	iDataQualityStatus = aStatus;
	}

//-----------------------------------------------------------------------------
// TPositionQualityItem
//-----------------------------------------------------------------------------

template<class T>
inline TBool IsEqual(const T* aCompare1, const T* aCompare2)
	{
	return (*aCompare1==*aCompare2);
	}

template<class T>
inline TBool IsLessThan(const T* aCompare1, const T* aCompare2)
	{
	return (*aCompare1 < *aCompare2);
	}

#define SETUP_EQUALS_AND_LESSTHAN(aIsEqual, aIsLessThan, aClassType) \
	{\
	const aClassType* aItemsData = reinterpret_cast<const aClassType*>(aItem.iData); \
	const aClassType* thisData = reinterpret_cast<const aClassType*>(iData); \
	aIsEqual = IsEqual(aItemsData, thisData); \
	aIsLessThan = IsLessThan(aItemsData, thisData); \
	}

/** compare method */
TInt TPositionQualityItem::Compare(const TPositionQualityItem& aItem, 
								   TResult& aComparison) const
/**
@internalComponent
Compare  method
@param aItem The item that we want to compare against ourself.
@param aComparison Set by this function to whether or not the TPositionQualityItem
provided is better or worse than this TPositionQualityItem.
@return KErrNone if comparison was successful.
@return KErrArgument if there is a mismatch of the datatypes of the value preference.
 */							
	{
	if((aItem.iDataType!=iDataType)||(aItem.iScaleDirection!=iScaleDirection))
		return KErrArgument;
	//Unusual implementation is to reduce code bloat.
	TBool isEqual(EFalse);
	TBool isLessThan(EFalse);
	switch(iDataType)
		{
		case ETInt8:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TInt8);
			break;
		case ETInt16:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TInt16);
			break;
		case ETInt32:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TInt32);
			break;
		case ETInt64:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TInt64);
			break;
		case ETUint8:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TUint8);
			break;
		case ETUint16:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TUint16);
			break;
		case ETUint32:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TUint32);
			break;
		case ETReal32:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TReal32);
			break;
		case ETReal64:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TReal64);
			break;
		case ETTime:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TTime);
			break;
		case ETTimeIntervalMicroSeconds:
			SETUP_EQUALS_AND_LESSTHAN(isEqual, isLessThan, TTimeIntervalMicroSeconds);
			break;
		default:	//catches EUndefined.
			return KErrArgument;
		}
	if(isEqual)
		aComparison = EIsEqual;
	else 
		{
		if (isLessThan)
			{
			if(iScaleDirection==EPreferGreaterValues)
				aComparison = EIsWorse;
			else
				aComparison = EIsBetter;			
			}
		else
			{
			if(iScaleDirection==EPreferGreaterValues)
				aComparison = EIsBetter;
			else
				aComparison = EIsWorse;
			}
		}
	return KErrNone;
	}

void TPositionQualityItem::Set(TDataType aDataType, TValuePreference aValuePreference,
											  const TAny* aData, TUint aSizeOfData)
/**
@internalComponent
Sets position quality
@param aDataType The datatype of the data to be set in the array.
@param aValuePreference Whether greater or lower values indicate 'better'.
@param aData A pointer to the data to be read from.
@param aSizeOfData The size of the data object to be copied.
 */
	{
	__ASSERT_ALWAYS(aSizeOfData <= KPositionMaxSizeQualityItem,
		User::Panic(KPosClientFault, EPositionQualityBadDataType));
	iDataType = aDataType;
	iScaleDirection = aValuePreference;
	Mem::Copy(iData, aData, aSizeOfData);
	}

TInt TPositionQualityItem::Get(TPositionQualityItem::TDataType aDataType,
											  TAny* aData, TUint aSizeOfData) const
/**
@internalComponent
Gets position quality
@param aDataType The datatype of the data to be fetched.
@param aData A pointer to the data to be written into.
@param aSizeOfData The size of the data object to be copied.
@return KErrArgument if there is a mismatch in the datatype between setting and getting.
@return KErrNone if successful.
 */
	{
	__ASSERT_ALWAYS(aSizeOfData <= KPositionMaxSizeQualityItem,
		User::Panic(KPosClientFault, EPositionQualityBadDataType));
	if(aDataType!=iDataType)	//This effectively asserts that iDataType can't be EUndefined.
		return KErrArgument;	
	else
		{
		Mem::Copy(aData, iData, aSizeOfData);
		return KErrNone;
		}
	}

TPositionQualityItem::TPositionQualityItem()
/**
Constructor for TPositionQualityItem

@internalComponent
 */
	{
	Mem::FillZ(this, sizeof(TPositionQualityItem)); // zero everything
	iDataType = EUndefined;
	}

TBool TPositionQualityItem::IsDefined() const
/**
Whether or not this element has a defined state
@internalComponent
@return Whether or not this element has a state of EUndefined.
 */
	{
	return iDataType != EUndefined;
	}

//-----------------------------------------------------------------------------
// TPositionQualityBase
//-----------------------------------------------------------------------------
EXPORT_C TPositionQualityBase::TPositionQualityBase()
/**
Default constructor for TPositionQualityBase

 */
:	TPositionClassTypeBase(),
	iHighwaterMark(-1)
	{
	// the iPositionQualityData array is zeroed via the default constructor
	// of the element type TPositionQualityItem
	}

EXPORT_C TInt TPositionQualityBase::Compare(
									const TPositionQualityBase& aPositionQuality, 
								    TInt aElementToCompare,
									TPositionQualityItem::TResult& aComparison) const
/**

Compares a specified element of another TPositionQualityBase object to this
TPositionQualityBase.

@param aPositionQuality The position quality to compare against this position quality.
@param aElementToCompare The element number that we want to compare.
@param aComparison Set by this function to whether or not the TPositionQualityBase provided
 is better or worse than this TPositionQualityBase.
@return KErrNone if comparison was successful.
@return KErrArgument if there is a mismatch of the datatypes of the value preference.
 */
	{
	return iPositionQualityData[aElementToCompare].
		Compare(aPositionQuality.iPositionQualityData[aElementToCompare], aComparison);
	}

EXPORT_C void TPositionQualityBase::DoSetElement(TInt aElementId,
											TPositionQualityItem::TDataType aDataType,
											TPositionQualityItem::TValuePreference aValuePreference,
											const TAny* aData, TUint aSizeOfData)
/**
@internalComponent
Sets the data for an element in the element array.

@param aElementId The element Id of that we're interested in.
@param aDataType The datatype of the data to be set in the array.
@param aValuePreference Whether greater or lower values indicate 'better'.
@param aData A pointer to the data to be read from.
@param aSizeOfData The size of the data object to be copied.
 */
	{
	iPositionQualityData.At(aElementId).Set(aDataType,
				aValuePreference, aData, aSizeOfData);	//Will ALWAYS bounds check
	if(aElementId>iHighwaterMark)
		iHighwaterMark = aElementId;
	}

EXPORT_C TInt TPositionQualityBase::DoGetElement(TInt aElementId,
												  TPositionQualityItem::TDataType aDataType,
												  TAny* aData, TUint aSizeOfData) const
/**
@internalComponent

Gets the data of an element from the array of elements.

@param aElementId The element Id of that we're interested in.
@param aDataType The datatype of the data to be fetched.
@param aData A pointer to the data to be written into.
@param aSizeOfData The size of the data object to be copied.
@return KErrArgument if there is a mismatch in the datatype between setting and getting.
@return KErrNone if successful.
 */
	{
	return iPositionQualityData[aElementId].Get(aDataType, aData, aSizeOfData);
	}

EXPORT_C TBool TPositionQualityBase::IsDefined(TInt aElementId) const
/**
Whether or not this element has a defined state
@param aElementId The element Id of that we're interested in.
@return Whether or not an element has a state of EUndefined.
 */
	{
	return iPositionQualityData[aElementId].IsDefined();
	}

EXPORT_C void TPositionQualityBase::ResetElement(TInt aElementId)
/**
Resets element
@param aElementId The id of the element to be reset to a state of EUndefined.
 * Maintains the iHighWaterMark.
 */
	{
	TPositionQualityItem item;
	iPositionQualityData.At(aElementId) = item;
	
	if(aElementId==iHighwaterMark)
		{
		iHighwaterMark = -1;
		for(TInt i=KPositionMaxSectionFields-1; i>=0; --i)
			{
			if(iPositionQualityData[i].IsDefined())
				{
				iHighwaterMark = i;
				break;
				}
			}
		}
	}

EXPORT_C TInt TPositionQualityBase::HighWaterMark() const
/**
Returns The High Water Mark
@return The High Water Mark.
 */
	{
	return iHighwaterMark;
	}

//-----------------------------------------------------------------------------
// TPositionQuality
//-----------------------------------------------------------------------------
EXPORT_C TPositionQuality::TPositionQuality()
/**
Default constructor for TPositionQuality

Note that the horizontal accuracy and vertical accuracy are initially NaNs.

*/
:	TPositionQualityBase()
	{
	//Check size of TPositionQualityItem is <= 16, to ensure iReserved does not overflow
	__ASSERT_COMPILE(sizeof(TPositionQualityItem) <= 16);
	
	iPosClassType |= EPositionQualityClass;
	iPosClassSize = sizeof(TPositionQuality);
	// need to set the times to default to zero,
	// accuracy to NaN
	// cost & power to unkown.
	// We must do this in the constructor becuase other code
	// See TPositionCriteriaBase::ClearCriteria()
	// assumes that using the default constructor will
	// reset as correctly.
	// This also has the side effect making the Get/Set functions
	// much simpler
	TRealX n;
	n.SetNaN();
	TReal32 nan;
	nan = n;
	SetHorizontalAccuracy(nan);
	SetVerticalAccuracy(nan);
	TTimeIntervalMicroSeconds zeroMs(0);
	SetTimeToFirstFix(zeroMs);
	SetTimeToNextFix(zeroMs);
	SetCostIndicator(TPositionQuality::ECostUnknown);
	SetPowerConsumption(TPositionQuality::EPowerUnknown);
	Mem::FillZ(&iReserved, sizeof(TPositionQualityItem)); 
	}

EXPORT_C TReal32 TPositionQuality::HorizontalAccuracy() const
/**
Horizontal Accuracy
@return The horizontal accuracy
 */
	{
	TReal32 real32;
	TInt err = GetElement(TPositionSelectionOrder::EFieldHorizontalAccuracy, TPositionQualityItem::ETReal32, real32);
	if(err==KErrNone)
		return real32;
	else
		{
		TRealX nan;
		nan.SetNaN();
		return nan;
		}
	}

EXPORT_C TReal32 TPositionQuality::VerticalAccuracy() const
/**
The vertical accuracy
@return The vertical accuracy
 */
	{
	TReal32 real32;
	TInt err = GetElement(TPositionSelectionOrder::EFieldVerticalAccuracy, TPositionQualityItem::ETReal32, real32);
	if(err==KErrNone)
		return real32;
	else
		{
		TRealX nan;
		nan.SetNaN();
		return nan;
		}
	}

EXPORT_C TPositionQuality::TCostIndicator TPositionQuality::CostIndicator() const
/**
The cost indicator
@return The cost indicator
 */
	{
	TInt32 temp;
	TInt err = GetElement(TPositionSelectionOrder::EFieldCost, TPositionQualityItem::ETInt32, temp);
	if(err==KErrNone)
		return static_cast<TPositionQuality::TCostIndicator>(temp);
	else
		// this means if there ever was an error here its never reported!
		return TPositionQuality::ECostUnknown;
	}

EXPORT_C TPositionQuality::TPowerConsumption TPositionQuality::PowerConsumption() const
/**
The power consumption
@return The power consumption
 */
	{
	TInt32 temp;
	TInt err = GetElement(TPositionSelectionOrder::EFieldPower, TPositionQualityItem::ETInt32, temp);
	if(err==KErrNone)
		return static_cast<TPositionQuality::TPowerConsumption>(temp);
	else
		// this means if there ever was an error here its never reported!
		return TPositionQuality::EPowerUnknown;
	}

EXPORT_C TTimeIntervalMicroSeconds TPositionQuality::TimeToFirstFix() const
/**
The time required to obtain an initial location fix
@return The time required to obtain an initial location fix
 */
	{
	TTimeIntervalMicroSeconds temp;
	TInt err = GetElement(TPositionSelectionOrder::EFieldTimeToFirstFix,
						TPositionQualityItem::ETTimeIntervalMicroSeconds, temp);
	if(err==KErrNone)
		return temp;
	else
		// this means if there ever was an error here its never reported!
		return TTimeIntervalMicroSeconds(0);
	}

EXPORT_C TTimeIntervalMicroSeconds TPositionQuality::TimeToNextFix() const
/**
The time required to obtain subsequent location fixes
@return The time required to obtain subsequent location fixes
 */
	{
	TTimeIntervalMicroSeconds temp;
	TInt err = GetElement(TPositionSelectionOrder::EFieldTimeToNextFix,
						TPositionQualityItem::ETTimeIntervalMicroSeconds, temp);
	if(err==KErrNone)
		return temp;
	else
		// this means if there ever was an error here its never reported!
		return TTimeIntervalMicroSeconds(0);
	}

EXPORT_C void TPositionQuality::SetHorizontalAccuracy(TReal32 aHorizontalAccuracy)
/**
Sets horizontal accuracy
@param aHorizontalAccuracy The horizontal accuracy to set
@panic "LocationClient EPositionBadAccuracy" if aHorizontalAccuracy
is negative.
 */
	{
	// set to NaN is legal as it need to be for initialization
	// zero is not legal. but NaN is :)
	if(!Math::IsNaN(aHorizontalAccuracy) && aHorizontalAccuracy <= TReal32(0))
		{
		User::Panic(KPosClientFault, EPositionBadAccuracy);
		}
	SetElement(TPositionSelectionOrder::EFieldHorizontalAccuracy,
					  TPositionQualityItem::ETReal32,
					  TPositionQualityItem::EPreferSmallerValues,
					  aHorizontalAccuracy);
	if(Math::IsNaN(aHorizontalAccuracy))
		{
		// reset rather then set - to reset the data type to EUndefined...
		ResetElement(TPositionSelectionOrder::EFieldHorizontalAccuracy);
		}
	}

EXPORT_C void TPositionQuality::SetVerticalAccuracy(TReal32 aVerticalAccuracy)
/**
Sets vertical accuracy
@param aVerticalAccuracy The vertical accuracy to set
@panic "LocationClient EPositionBadAccuracy" if aVerticalAccuracy
is negative.
 */
	{
	// set to NaN is legal as it need to be for initialization
	// zero is not legal but zero is
	if(!Math::IsNaN(aVerticalAccuracy) && aVerticalAccuracy <= TReal32(0))
		{
		User::Panic(KPosClientFault, EPositionBadAccuracy);
		}
	if(Math::IsNaN(aVerticalAccuracy))
		{
		ResetElement(TPositionSelectionOrder::EFieldVerticalAccuracy);
		}
	else
		{
		SetElement(TPositionSelectionOrder::EFieldVerticalAccuracy,
					  TPositionQualityItem::ETReal32,
					  TPositionQualityItem::EPreferSmallerValues,
					  aVerticalAccuracy);
		}
	}

EXPORT_C void TPositionQuality::SetCostIndicator(
	TPositionQuality::TCostIndicator aCost)
/**
The cost indicator to set
@param aCost The cost indicator to set
 */
	{
	if(aCost == TPositionQuality::ECostUnknown)
		{
		ResetElement(TPositionSelectionOrder::EFieldCost);
		}
	else
		{
		SetElement(TPositionSelectionOrder::EFieldCost, TPositionQualityItem::ETInt32,
					  TPositionQualityItem::EPreferSmallerValues, aCost);
		}
	}

EXPORT_C void TPositionQuality::SetPowerConsumption(
	TPositionQuality::TPowerConsumption aPower)
/**
The power consumption to set
@param aPower The power consumption to set
 */
	{
	if(aPower == TPositionQuality::EPowerUnknown)
		{
		ResetElement(TPositionSelectionOrder::EFieldPower);
		}
	else
		{
		SetElement(TPositionSelectionOrder::EFieldPower, TPositionQualityItem::ETInt32,
					  TPositionQualityItem::EPreferSmallerValues, aPower);
		}
	}

EXPORT_C void TPositionQuality::SetTimeToFirstFix(
	TTimeIntervalMicroSeconds aTimeToFirstFix)
/**
Sets time to first fix
@param aTimeToFirstFix The time required to obtain an initial location fix to set
@panic "LocationClient EPositionBadTime" if aTimeToFirstFix
is negative.
 */
	{
	TTimeIntervalMicroSeconds zero(0);
	__ASSERT_ALWAYS(aTimeToFirstFix >= zero,
		User::Panic(KPosClientFault, EPositionBadTime));

	if(aTimeToFirstFix == zero)
		{
		ResetElement(TPositionSelectionOrder::EFieldTimeToFirstFix);
		}
	else
		{
		SetElement(TPositionSelectionOrder::EFieldTimeToFirstFix,
					  TPositionQualityItem::ETTimeIntervalMicroSeconds,
					  TPositionQualityItem::EPreferSmallerValues,
					  aTimeToFirstFix);
		}
	}

EXPORT_C void TPositionQuality::SetTimeToNextFix(
	TTimeIntervalMicroSeconds aTimeToNextFix) 
/**
Sets time to next fix
@param aTimeToNextFix The time required to obtain subsequent location fixes to set
@panic "LocationClient EPositionBadTime" if aTimeToNextFix
is negative.
 */
	{
	TTimeIntervalMicroSeconds zero(0);
	__ASSERT_ALWAYS(aTimeToNextFix >= zero,
		User::Panic(KPosClientFault, EPositionBadTime));

	if(aTimeToNextFix==zero)
		{
		ResetElement(TPositionSelectionOrder::EFieldTimeToNextFix);
		}
	else
		{
		SetElement(TPositionSelectionOrder::EFieldTimeToNextFix,
					  TPositionQualityItem::ETTimeIntervalMicroSeconds,
					  TPositionQualityItem::EPreferSmallerValues,
					  aTimeToNextFix);
		}
	}

//-----------------------------------------------------------------------------
// TPositionModuleStatusEventBase
//-----------------------------------------------------------------------------
EXPORT_C TPositionModuleStatusEventBase::TPositionModuleStatusEventBase()
/**
Default constructor for TPositionModuleStatusEventBase

*/

: 	iRequestedEvents(EEventNone),
	iOccurredEvents(EEventNone),
	iSystemModuleEvent(ESystemUnknown),
	iModuleId(KPositionNullModuleId)
	{}

EXPORT_C TPositionModuleStatusEventBase::TPositionModuleStatusEventBase(
	TModuleEvent aRequestedEventMask)
/**
Constructor for TPositionModuleStatusEventBase (with events)

@param aRequestedEventMask The requested events
 */
: 	iRequestedEvents(aRequestedEventMask),
	iOccurredEvents(EEventNone),
	iSystemModuleEvent(ESystemUnknown),
	iModuleId(KPositionNullModuleId)
	{}

void TPositionModuleStatusEventBase::Reset()
/**
Method that's called by RPositionServer to reset the
TPositionModuleStatusEventBase derived object to it's unknown state.

@internalTechnology
*/
	{
	DoReset(iPosClassSize);
	}

EXPORT_C void TPositionModuleStatusEventBase::DoReset(TInt aSizeOfClass)
/**
Method to reset a TPositionModuleStatusEventBase to the 'unknown' state.

@param aSizeOfClass Function asserts that aSizeOfClass == 
 * sizeof(TPositionModuleStatusEventBase) to ensure DoReset was overridden in derived class.
 */
	{
	__ASSERT_CLASS_SIZE(TPositionModuleStatusEventBase, aSizeOfClass);

	iOccurredEvents = EEventNone;
	iSystemModuleEvent = ESystemUnknown;
	iModuleId = KPositionNullModuleId;
	}

EXPORT_C TPositionModuleId TPositionModuleStatusEventBase::ModuleId() const
/**
Returns the module ID
@return The module ID
 */
	{
	return iModuleId;
	}

EXPORT_C void TPositionModuleStatusEventBase::SetModuleId(TPositionModuleId aModuleId)
/**
The module ID to set
@param aModuleId The module ID to set
 */
	{
	iModuleId = aModuleId;
	}

EXPORT_C void TPositionModuleStatusEventBase::SetRequestedEvents(
	TModuleEvent aRequestedEventMask)
/**
Sets the requested events
@param aRequestedEventMask The requested events to set
 */
	{
	iRequestedEvents = aRequestedEventMask;
	}

EXPORT_C TPositionModuleStatusEventBase::TModuleEvent
	TPositionModuleStatusEventBase::RequestedEvents() const
/**
Returns the requested events
@return The requested events
 */
	{
	return iRequestedEvents;
	}

EXPORT_C void
	TPositionModuleStatusEventBase::SetOccurredEvents(TModuleEvent aOccurredEventMask)
/**
The occurred events to set
@param aOccurredEventMask The occurred events to set
 */
	{
	iOccurredEvents = aOccurredEventMask;
	}
	
EXPORT_C TPositionModuleStatusEventBase::TModuleEvent
	TPositionModuleStatusEventBase::OccurredEvents() const
/**
Returns the events which have occurred
@return The events which have occurred
 */
	{
	return iOccurredEvents;
	}

EXPORT_C void TPositionModuleStatusEventBase::SetSystemModuleEvent(
	TSystemModuleEvent aSystemModuleEvent)
/**
Sets the system event
@param aSystemModuleEvent The system event to set.
 */
	{
	iSystemModuleEvent = aSystemModuleEvent;
	}

EXPORT_C TPositionModuleStatusEventBase::TSystemModuleEvent
		TPositionModuleStatusEventBase::SystemModuleEvent() const
/**
Returns The system event type.
@return The system event type.
 */
	{
	return iSystemModuleEvent;
	}


//-----------------------------------------------------------------------------
// TPositionModuleStatusEvent
//-----------------------------------------------------------------------------
EXPORT_C TPositionModuleStatusEvent::TPositionModuleStatusEvent()
/**
Default constructor for TPositionModuleStatusEvent

*/
 :	iModuleStatus()
	{
	iPosClassType |= EPositionModuleStatusEventClass;
	iPosClassSize = sizeof(TPositionModuleStatusEvent);
	}

EXPORT_C TPositionModuleStatusEvent::TPositionModuleStatusEvent(
	TModuleEvent aRequestedEventMask)
/**
Constructor for TPositionModuleStatusEvent (with events)

@param aRequestedEventMask The requested events
 */
 :	TPositionModuleStatusEventBase(aRequestedEventMask),
	iModuleStatus()
	{
	iPosClassType |= EPositionModuleStatusEventClass;
	iPosClassSize = sizeof(TPositionModuleStatusEvent);
	}

EXPORT_C void TPositionModuleStatusEvent::SetModuleStatus(
	const TPositionModuleStatus& aModuleStatus)
/**
Sets the module status
@param aModuleStatus The module status to set
 */
	{
	iModuleStatus = aModuleStatus;
	}

EXPORT_C void TPositionModuleStatusEvent::GetModuleStatus(
	TPositionModuleStatus& aModuleStatus) const
/**
Gets Where the module status will be written
@param aModuleStatus Where the module status will be written to
 */
	{
	aModuleStatus = iModuleStatus;
	}

EXPORT_C void TPositionModuleStatusEvent::DoReset(TInt aSizeOfClass)
	{
/**
Method to reset a TPositionModuleStatusEvent to the 'unknown' state.

@param aSizeOfClass Function asserts that aSizeOfClass == 
sizeof(TPositionModuleStatusEvent) to ensure DoReset was overridden in derived class
if class has been derived from.
 */
	__TPOSITION_MODULE_STATUS_EVENT_RESET_BASE(TPositionModuleStatusEvent,
											   aSizeOfClass,
											   TPositionModuleStatusEventBase);
	(void) new(&iModuleStatus) TPositionModuleStatus;
	}

//-----------------------------------------------------------------------------
// TPositionUpdateOptionsBase
//-----------------------------------------------------------------------------
EXPORT_C TPositionUpdateOptionsBase::TPositionUpdateOptionsBase()
/**
Default constructor for TPositionUpdateOptionsBase

*/
 :	iUpdateInterval(0),
	iUpdateTimeOut(0),
	iMaxUpdateAge(0)
	{}

EXPORT_C TPositionUpdateOptionsBase::TPositionUpdateOptionsBase(
					TTimeIntervalMicroSeconds aInterval,
					TTimeIntervalMicroSeconds aTimeOut,
					TTimeIntervalMicroSeconds aMaxAge)
/**
Constructor for TPositionUpdateOptionsBase. All parameters except aInterval
have default values.

@param aInterval The update interval to be set.
@param aTimeOut The update timeout to be set. Defaults to zero.
@param aMaxAge The maximum update age to be set. Defaults to zero.
@panic "LocationClient EPositionBadTime" if aInterval, aMaxAge or
aTimeOut is negative.
 */
 :	iUpdateInterval(aInterval),
	iUpdateTimeOut(aTimeOut),
	iMaxUpdateAge(aMaxAge)
	{
	__ASSERT_ALWAYS(
	        aInterval >= TTimeIntervalMicroSeconds(0)
	     && aMaxAge >= TTimeIntervalMicroSeconds(0)
	     && aTimeOut >= TTimeIntervalMicroSeconds(0),
		User::Panic(KPosClientFault, EPositionBadTime));
	}

EXPORT_C void TPositionUpdateOptionsBase::SetUpdateInterval(TTimeIntervalMicroSeconds aInterval)
/**
Sets update interval
@param aInterval The update interval to be set
@panic "LocationClient EPositionBadTime" if aInterval is negative.
 */
	{
	__ASSERT_ALWAYS(aInterval >= TTimeIntervalMicroSeconds(0),
		User::Panic(KPosClientFault, EPositionBadTime));
	iUpdateInterval = aInterval;
	}

EXPORT_C void TPositionUpdateOptionsBase::SetMaxUpdateAge(TTimeIntervalMicroSeconds aMaxAge)
/**
sets the maximum update age
@param aMaxAge The maximum update age to be set
@panic "LocationClient EPositionBadTime" if aMaxAge is negative.
 */
	{
	__ASSERT_ALWAYS(aMaxAge >= TTimeIntervalMicroSeconds(0),
		User::Panic(KPosClientFault, EPositionBadTime));
	iMaxUpdateAge = aMaxAge;
	}

EXPORT_C void TPositionUpdateOptionsBase::SetUpdateTimeOut(TTimeIntervalMicroSeconds aTimeOut)
/**
Sets the update timeout
@param aTimeOut The update timeout to be set
@panic "LocationClient EPositionBadTime" if aTimeOut is negative.
 */
	{
	__ASSERT_ALWAYS(aTimeOut >= TTimeIntervalMicroSeconds(0),
		User::Panic(KPosClientFault, EPositionBadTime));
	iUpdateTimeOut = aTimeOut;
	}

EXPORT_C TTimeIntervalMicroSeconds TPositionUpdateOptionsBase::UpdateInterval() const
/**
Returns the update interval
@return The update interval
 */
	{
	return iUpdateInterval;
	}

EXPORT_C TTimeIntervalMicroSeconds TPositionUpdateOptionsBase::MaxUpdateAge() const
/**
Returns the maximum update age
@return The maximum update age
 */
	{
	return iMaxUpdateAge;
	}

EXPORT_C TTimeIntervalMicroSeconds TPositionUpdateOptionsBase::UpdateTimeOut() const
/**
Returns the update timeout
@return The update timeout
 */
	{
	return iUpdateTimeOut;
	}

//-----------------------------------------------------------------------------
// TPositionUpdateOptions
//-----------------------------------------------------------------------------
EXPORT_C TPositionUpdateOptions::TPositionUpdateOptions()
/**
Default constructor for TPositionUpdateOptions

*/
: TPositionUpdateOptionsBase(),
  iAcceptPartialUpdates(EFalse)
	{
	iPosClassType |= EPositionUpdateOptionsClass;
	iPosClassSize = sizeof(TPositionUpdateOptions);
	}

EXPORT_C TPositionUpdateOptions::TPositionUpdateOptions(TTimeIntervalMicroSeconds aInterval,
                                                        TTimeIntervalMicroSeconds aTimeOut,
                                                        TTimeIntervalMicroSeconds aMaxAge,
                                                        TBool aAcceptPartialUpdates)
/**
Constructor for TPositionUpdateOptions. All parameters except aInterval
have default values.

@param aInterval The update interval to be set.
@param aTimeOut The update timeout to be set. Defaults to zero.
@param aMaxAge The maximum update age to be set. Defaults to zero.
@param aAcceptPartialUpdates Whether partial updates will be accepted.
Defaults to false.
 */
 :  TPositionUpdateOptionsBase(aInterval, aTimeOut, aMaxAge),
	iAcceptPartialUpdates(aAcceptPartialUpdates)
	{
	iPosClassType |= EPositionUpdateOptionsClass;
	iPosClassSize = sizeof(TPositionUpdateOptions);
	}

EXPORT_C void TPositionUpdateOptions::SetAcceptPartialUpdates(TBool aAcceptPartialUpdates)
/**
Sets Whether partial updates will be accepted
@param aAcceptPartialUpdates Whether partial updates will be accepted
 */
	{
	iAcceptPartialUpdates = aAcceptPartialUpdates;
	}

EXPORT_C TBool TPositionUpdateOptions::AcceptPartialUpdates() const
/**
Returns Whether partial updates will be accepted
@return ETrue if partial updates are accepted, EFalse otherwise
 */
	{
	return iAcceptPartialUpdates;
	}
