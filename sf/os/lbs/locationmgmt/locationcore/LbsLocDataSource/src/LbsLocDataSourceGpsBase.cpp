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
// LbsLocSourceGpsBase.cpp
// 
//

#include <ecom/ecom.h>
#include <e32debug.h>

#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbslocdatasourceclasstypes.h>
#include "lbsdevloggermacros.h"

//-----------------------------------------------------------------------------
// TLbsLocRequestQuality
//-----------------------------------------------------------------------------


EXPORT_C TLbsLocRequestQuality::TLbsLocRequestQuality()
	:
	iMinHorizontalAccuracy(0),
	iMinVerticalAccuracy(0),
	iMaxFixTime(0),
	iAssistedClientRequestPresent(EFalse)
	{
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}

EXPORT_C TTimeIntervalMicroSeconds  TLbsLocRequestQuality::MaxFixTime() const
	{
	return iMaxFixTime;
	}

EXPORT_C TReal32 TLbsLocRequestQuality::MinHorizontalAccuracy() const
	{
	return iMinHorizontalAccuracy;
	}

EXPORT_C TReal32 TLbsLocRequestQuality::MinVerticalAccuracy() const
	{
	return iMinVerticalAccuracy;
	}

EXPORT_C TBool TLbsLocRequestQuality::AssistedClientRequestPresent() const
    {
    return iAssistedClientRequestPresent;
    }

EXPORT_C void TLbsLocRequestQuality::SetMaxFixTime(TTimeIntervalMicroSeconds aTime)
	{
	iMaxFixTime = aTime;
	}

EXPORT_C void TLbsLocRequestQuality::SetMinHorizontalAccuracy(TReal32 aAccuracy)
	{
	iMinHorizontalAccuracy = aAccuracy;
	}

EXPORT_C void TLbsLocRequestQuality::SetMinVerticalAccuracy(TReal32 aAccuracy)
	{
	iMinVerticalAccuracy = aAccuracy;
	}

EXPORT_C void TLbsLocRequestQuality::SetAssistedClientRequestPresent(TBool aAssistedClientRequestPresent)
    {
    iAssistedClientRequestPresent = aAssistedClientRequestPresent;
    }


//-----------------------------------------------------------------------------
// TLbsGpsOptions
//-----------------------------------------------------------------------------


EXPORT_C TLbsGpsOptions::TLbsGpsOptions()
	:
	iGpsMode(CLbsAdmin::EGpsModeUnknown),
	iGpsTimeRelationReq(EFalse)
	{
	iClassType = ELbsGpsOptionsClass;
	iClassSize = sizeof(TLbsGpsOptions);
	}

EXPORT_C void TLbsGpsOptions::SetGpsMode(CLbsAdmin::TGpsMode aGpsMode)
	{
	iGpsMode = aGpsMode;
	}

EXPORT_C void TLbsGpsOptions::SetGpsTimeRelationReq(TBool aGpsTimeRelationReq)
	{
	iGpsTimeRelationReq = aGpsTimeRelationReq;
	}

EXPORT_C CLbsAdmin::TGpsMode TLbsGpsOptions::GpsMode() const
	{
	return iGpsMode;
	}

EXPORT_C TBool TLbsGpsOptions::GpsTimeRelationReq() const
	{
	return iGpsTimeRelationReq;
	}


//-----------------------------------------------------------------------------
// TLbsGpsOptionsItem
//-----------------------------------------------------------------------------

/** Default constructor for TLbsGpsOptionsItem.
    Items of this type holds one specific GPS option. These are passed to the
    GPS module as part of the an TLbsGpsOptionsArray. 
@see TLbsGpsOptionsArray
 */
EXPORT_C TLbsGpsOptionsItem::TLbsGpsOptionsItem()
	:
	iPosUpdateType(EPosUpdateUnknown)
	{
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}

/** Returns the required update type
@see TLbsGpsOptionsArray
@return Required update type
 */
EXPORT_C TLbsGpsOptionsItem::TPosUpdateType TLbsGpsOptionsItem::PosUpdateType() const
	{
	return iPosUpdateType;
	}

/** Assigns the required update type
@see TLbsGpsOptionsArray
@param aPosUpdateType Specifies the required update type
 */
EXPORT_C void TLbsGpsOptionsItem::SetLocUpdateType(TPosUpdateType aPosUpdateType)
	{	
	iPosUpdateType = aPosUpdateType;
	}


//-----------------------------------------------------------------------------
// TLbsGpsOptionsArray
//-----------------------------------------------------------------------------

/** TLbsGpsOptionsArray specifies and array of GPS Modes
    for location updates produced by the data source. 
 */
EXPORT_C TLbsGpsOptionsArray::TLbsGpsOptionsArray()
	:
	iNumItems(0)
	{
	iClassType |= ELbsGpsOptionsArrayClass;
	iClassSize  = sizeof(TLbsGpsOptionsArray);
	}

/**
Retrieves the specified GPS option item by index
@see NumOptionItems()
@see AppendOptionItem()
@see TLbsGpsOptionsItem
@param aIndex The index of the GPS option item requested.
       An index of zero specifies the first option item
@param aMeasurementData Holds the returned measurement data
@return KErrNone if successful
@return KErrNotFound if parameter aIndex is out of range
*/
EXPORT_C TInt TLbsGpsOptionsArray::GetOptionItem
	(
	TUint aIndex,
	TLbsGpsOptionsItem& aOptionItem
	) const
	{
	if (aIndex >= iNumItems)
		{
		return KErrNotFound;
		}
	
	aOptionItem = iOptions[aIndex];
	return KErrNone;
	}


/**
Adds the supplied GPS Option Item to the current list
@see NumOptionItems()
@see GetOptionItem()
@see TLbsGpsOptionsItem
@param aOptionItem The option item to append
@return KErrNone if successful
@return KErrOverflow if the number measurements held would exceed KLbsMaxGpsOptions
*/
EXPORT_C TInt TLbsGpsOptionsArray::AppendOptionItem
	(
	const TLbsGpsOptionsItem& aOptionItem
	)
	{
	if (iNumItems >= KLbsMaxGpsOptions)
		{
		return KErrOverflow;
		}

	iOptions[iNumItems++] = aOptionItem;

	return KErrNone;
	}

/**
The number of GPS option items held
@see GetOptionItem()
@see AppendOptionItem()
@see TLbsGpsOptionsItem
@return The number of measurements
*/
EXPORT_C TInt TLbsGpsOptionsArray::NumOptionItems() const
	{
	return iNumItems;
	}

/**
Remove all GPS option items
@see NumOptionItems()
@see GetOptionItem()
@see AppendOptionItem()
@see TLbsGpsOptionsItem
*/
EXPORT_C void TLbsGpsOptionsArray::ClearOptionItems()
	{
	iNumItems = 0;
	}


//-----------------------------------------------------------------------------
// CLbsLocationSourceGpsBase
//-----------------------------------------------------------------------------


EXPORT_C CLbsLocationSourceGpsBase* CLbsLocationSourceGpsBase::NewL
	(
	MLbsLocationSourceGpsObserver& aObserver,
	TUid aEcomModuleId
	)
	{
	LBSLOG(ELogP1, "CLbsLocationSourceGpsBase::NewL()");
	LBSLOG2(ELogP2, "Loading the GPS module plugin. Id: 0x%X", aEcomModuleId.iUid);
	TAny* ptr = REComSession::CreateImplementationL(aEcomModuleId,
													_FOFF(CLbsLocationSourceGpsBase, iDtor_ID_Key),
													reinterpret_cast<TAny*>(&aObserver));
	LBSLOG2(ELogP2, "GPS module plugin loaded.  Id: 0x%X", aEcomModuleId.iUid);
	return reinterpret_cast<CLbsLocationSourceGpsBase*>(ptr);
	}

EXPORT_C CLbsLocationSourceGpsBase::CLbsLocationSourceGpsBase(MLbsLocationSourceGpsObserver& aObserver)
	:iLocSourceGpsObserver(aObserver)
	{
	}


EXPORT_C void CLbsLocationSourceGpsBase::ConstructL()
	{	
	}

EXPORT_C CLbsLocationSourceGpsBase::~CLbsLocationSourceGpsBase()
	{
	LBSLOG(ELogP1, "CLbsLocationSourceGpsBase::~CLbsLocationSourceGpsBase()");
	LBSLOG(ELogP2, "Destroying the GPS module plugin.");
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

void CLbsLocationSourceGpsBase::SetGpsOptions
	(
	const TLbsGpsOptions& /*aGpsOptions*/
	)
	{	
	}

void CLbsLocationSourceGpsBase::RequestLocationUpdate
	(
	const TTime& /*aTargetTime*/,
	const TLbsLocRequestQuality& /*aQuality*/
	)
	{	
	}
	
void CLbsLocationSourceGpsBase::CancelLocationRequest()
	{
	}

void CLbsLocationSourceGpsBase::AdvisePowerMode
	(
	TPowerMode /*aMode*/
	)
	{	
	}

void CLbsLocationSourceGpsBase::AssistanceDataEvent
	(
	TInt /*aError*/,
	TLbsAsistanceDataGroup /*aDataMask*/
	)
	{	
	}

EXPORT_C TAny* CLbsLocationSourceGpsBase::ExtendedInterface
	(
	TInt  /*aFunctionNumber*/,
	TAny* /*aPtr1*/,
	TAny* /*aPtr2*/
	)
	{
	return NULL;
	}


//-----------------------------------------------------------------------------
// MLbsLocationSourceGpsObserver
//-----------------------------------------------------------------------------


EXPORT_C TVersion MLbsLocationSourceGpsObserver::Version()
	{
	return TVersion(1,0,0);
	}

void MLbsLocationSourceGpsObserver::UpdateLocation
	(
	TInt /*aStatus*/,
	const TPositionInfoBase** /*aPosInfoArray*/,
	TInt  /*aNumItems*/,
	const TTime& /*aTargetTime*/
	)
	{	
	}

void MLbsLocationSourceGpsObserver::UpdateDeviceStatus
	(
	TPositionModuleStatus::TDeviceStatus /*aDeviceStatus*/
	)
	{	
	}


void MLbsLocationSourceGpsObserver::UpdateDataQualityStatus
	(
	TPositionModuleStatus::TDataQualityStatus /*aDataQuality*/
	)
	{	
	}

TInt MLbsLocationSourceGpsObserver::GetAssistanceDataItem
	(
	TLbsAssistanceDataItem /*aItem*/,
	RDataReaderRootBase&   /*aDataRoot*/,
	TTime& /*aTimeStamp*/
	)
	{	
	return KErrNotFound;
	}

TInt MLbsLocationSourceGpsObserver::GetAssistanceDataItemTimeStamp
	(
	TLbsAssistanceDataItem /*aItem*/,
	TTime& /*aTimeStamp*/
	)
	{	
	return KErrNotFound;
	}
	
void MLbsLocationSourceGpsObserver::RequestAssistanceData
	(
	TLbsAsistanceDataGroup /*aDataItemMask*/
	)
	{	
	}

void MLbsLocationSourceGpsObserver::Shutdown()
	{	
	}

EXPORT_C TAny* MLbsLocationSourceGpsObserver::ExtendedInterface(TInt /*aFunctionNumber*/, TAny* /*aPtr1*/, TAny* /*aPtr2*/)
	{
	return NULL;
	}
