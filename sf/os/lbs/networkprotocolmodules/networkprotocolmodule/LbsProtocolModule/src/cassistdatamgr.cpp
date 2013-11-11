// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file provides the implementation of the class that manages
// assistance data during an active location request.
// 
//

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include "cassistdatamgr.h"
#include "lbsprotocolmoduleerrors.h"

/** Constant to identify an empty set of assistance data.
*/
const TInt KAssistDataEmpty = 0;


/** Static constructor.
@return A new instance of the CAssistDataMgr class.
*/  
CAssistDataMgr* CAssistDataMgr::NewL()
	{
	CAssistDataMgr* self = new (ELeave) CAssistDataMgr();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
*/  
CAssistDataMgr::CAssistDataMgr()
	{
	}


/** Standard destructor.
*/  
CAssistDataMgr::~CAssistDataMgr()
	{
	iValidData.Close();
	}


/** Private second-stage constructor.
*/  
void CAssistDataMgr::ConstructL()
	{
	iValidData.OpenL();
	ResetData();
	}


/** Valid data content.
This is called to retrieve currently stored valid assistance data.
@return A reference to the stored RLbsAssistanceDataBuilderSet
	containing the valid assistance data.
*/  
const RLbsAssistanceDataBuilderSet& CAssistDataMgr::ValidData()
	{
	return iValidData;
	}


/** Retrieve the request data mask to send to the network.
This is called to retrieve the data mask for any data that
is currently missing and is to be requested from the network.
@return A reference to the required set of assistance data.
*/  
const TLbsAsistanceDataGroup& CAssistDataMgr::RequestMask() const
	{
	return iMissingDataMask;
	}


/** Retrieve the valid data group mask.
This is called to retrieve the data group mask which represents
the valid set of data available to be sent to LBS.
@return A reference to the valid data group mask.
*/  
const TLbsAsistanceDataGroup& CAssistDataMgr::ValidGroupMask() const
	{
	return iValidDataMask;
	}


/** Retrieve the erroneous data group mask.
This is called to retrieve the data group mask which represents
the erroneous set of data requested by LBS.
@return A reference to the erroneous data group mask.
*/  
const TLbsAsistanceDataGroup& CAssistDataMgr::ErrorGroupMask() const
	{
	return iErrorDataMask;
	}


/** Retrieve the erroneous request data mask.
This is called to retrieve the data mask which represents
the erroneous parts of a request from LBS.
@return A reference to the erroneous request data mask.
*/  
const TLbsAsistanceDataGroup& CAssistDataMgr::RequestErrorMask() const
	{
	return iRequestErrorMask;
	}


/** Indicates if an assistance data error is to be reported to LBS.
The protocol manager calls this method when it decides what assistance data
actions need to be performed. 
@see CProtocolManager::DoAssistanceDataActions()
@return TBool ETrue if assistance data error to be reported to LBS.
*/  
TBool CAssistDataMgr::IsErrorToBeReported() const
	{
	return !(KErrNone == iDataResponseError);
	}


/** Indicates if assistance data request error to be reported to LBS.
The protocol manager calls this method when it decides what assistance data
actions need to be performed. 
@see CProtocolManager::DoAssistanceDataActions()
@return TBool ETrue if assistance data request error to be reported to LBS.
*/  
TBool CAssistDataMgr::IsRequestErrorToBeReported() const
	{
	return !(KAssistDataEmpty == iRequestErrorMask);
	}


/** Indicates if assistance data is to be requested from network.
The protocol manager calls this method when it decides what assistance data
actions need to be performed. 
@see CProtocolManager::DoAssistanceDataActions()
@return TBool ETrue if data is to be requested from network.
*/  
TBool CAssistDataMgr::IsDataToBeRequested() const
	{
	// If there is missing data AND not currently expecting a response
	// then a request for data is to be requested.
	// (Don't re-request data that has already been requested)
	return ((KAssistDataEmpty != iMissingDataMask) && !iExpectingResponse);
	}



/** Indicates if received assistance data is to be reported to LBS.
The protocol manager calls this method when it decides what assistance data
actions need to be performed. 
@see CProtocolManager::DoAssistanceDataActions()
@return TBool ETrue if received assistance data is to be reported to LBS.
*/  
TBool CAssistDataMgr::IsDataToBeReported() const
	{
	return iIsResponseWaiting;
	}


/** Assistance data error reason value.
This is called to retrieve assistance data error reason value.
@return TInt Assistance data error reason value.
*/  
TInt CAssistDataMgr::ErrorReason() const
	{
	return iDataResponseError;
	}


/** Assistance data response has been sent to LBS.
This is called by the protocol manager when it has sent a
pending assistance data response to LBS.
This resets internal status information to ensure further
assistance data actions only occur when necessary.
*/  
void CAssistDataMgr::DataReported()
	{
	iIsResponseWaiting = EFalse;
	}


/** Assistance data error response has been sent to LBS.
This is called by the protocol manager when it has sent a
pending assistance data error response to LBS.
This resets internal status information to ensure further
assistance data actions only occur when necessary.
*/  
void CAssistDataMgr::ErrorReported()
	{
	iDataResponseError = KErrNone;
	iErrorDataMask = KAssistDataEmpty;
	}


/** Assistance data request error has been sent to LBS.
This is called by the protocol manager when it has sent a
pending assistance data request error to LBS.
This resets internal status information to ensure further
assistance data actions only occur when necessary.
*/  
void CAssistDataMgr::RequestErrorReported()
	{
	iRequestErrorMask = KAssistDataEmpty;
	}


/** Assistance data request has been sent to network.
This is called by the protocol manager when it has sent a
pending assistance data request to the network.
This sets internal status information to ensure further
assistance data actions only occur as necessary and to identify
the set of data that any reported error would relate to.
*/
void CAssistDataMgr::RequestSent()
	{
	iErrorDataMask = iMissingDataMask;
	iMissingDataMask = KAssistDataEmpty;
	}


/** Assistance data request is being sent to network.
This is called by the protocol manager prior to an attempt to
sending an assistance data request to the network.
This sets internal status information to ensure that a
related response is expected.
*/  
void CAssistDataMgr::SendingRequest()
	{
	iExpectingResponse = ETrue;
	}


/** Reset data.
This is called to initialise the assistance data storage and
internal status information that is used to control related
assistance data actions.
*/  
void CAssistDataMgr::ResetData()
	{
	iDataResponseError = KErrNone;
	iDataRequestMask = KAssistDataEmpty;
	iRequestErrorMask = KAssistDataEmpty;
	iMissingDataMask = KAssistDataEmpty;
	iErrorDataMask = KAssistDataEmpty;
	iValidDataMask = KAssistDataEmpty;
	iExpectingResponse = EFalse;
	iIsResponseWaiting = EFalse;
	iIsDataReported = EFalse;

	iValidData.ClearAllBuilders();
	}


/** Set internal attributes to indicate no data has been received.
This is called by the protocol manager when it needs to set the
assistance data manager as if there has been no data received yet.
This ensures that newly arrived data is reported to LBS even if
it was sent during preceding protocol procedures.
*/  
void CAssistDataMgr::SetDataNotReceived()
	{
	iIsDataReported = EFalse;
	}


/** Set error reason.
This is called to store an error to be reported for the requested
assistance data. It is presumed that an error can only be associated
with the most recently requested set.

@param aReason The assistance data error.
*/  
void CAssistDataMgr::Error(TInt aReason)
	{
	// Record the error
	iDataResponseError = aReason;
	if (!iIsDataReported)
		{
		iIsDataReported = ETrue;
		}

	// Adjust the data request mask to avoid re-requesting this data
	if (KAssistDataEmpty != iDataRequestMask)
		{
		iDataRequestMask ^= iErrorDataMask;
		}

	}


/** Store assistance data.
This is called by the protocol manager when assistance data is received
from the network.

The data is added to the current stored data and compared with the 
requested data to determine if any is missing.

The protocol manager (and state machines) must decide when to perform related
assistance data actions, according to other prevailing conditions.
@see CProtocolManager::DoAssistanceDataActions()

@param aData The set of assistance data provided by the network.
*/
void CAssistDataMgr::StoreData(const RLbsAssistanceDataBuilderSet& aData)
	{
	// Set internal attributes relating to data requests/responses
	iExpectingResponse = EFalse;
	iIsResponseWaiting = ETrue;
	if (!iIsDataReported)
		{
		iIsDataReported = ETrue;
		}

	// Add the new data to the stored valid data
	TRAPD(error, iValidData.MergeL(const_cast<RLbsAssistanceDataBuilderSet&>(aData)));
	if (KErrNone != error)
		{
		// The merge failed...and this can only be due to a failed memory allocation
		// when opening an unopened data model. This should NEVER happen because we
		// always open the entire data set during construction.
		// So, there is nothing we can do with this error other than attempting
		// to request any missing data again.
		}

	// Identify missing data by comparing the set of stored data with the 
	// requested set. The value of iMissingDataMask identifies missing items.
	//
	// In future this may need to accumulate data which arrives in segments
	// before sending it onwards to LBS e.g. Almanac data.

	if (iDataRequestMask & EAssistanceDataAlmanac)
		{
		RUEPositioningGpsAlmanacBuilder* dataItem;
		if ((KErrNone != iValidData.GetDataBuilder(dataItem)) ||
			(!dataItem->IsDataAvailable()))
			{
			iMissingDataMask |= EAssistanceDataAlmanac;
			}
		}

	if (iDataRequestMask & EAssistanceDataBadSatList)
		{
		RBadSatListBuilder* dataItem;
		if ((KErrNone != iValidData.GetDataBuilder(dataItem)) ||
			(!dataItem->IsDataAvailable()))
			{
			iMissingDataMask |= EAssistanceDataBadSatList;
			}
		}

	if (iDataRequestMask & EAssistanceDataIonosphericModel)
		{
		RUEPositioningGpsIonosphericModelBuilder* dataItem;
		if ((KErrNone != iValidData.GetDataBuilder(dataItem)) ||
			(!dataItem->IsDataAvailable()))
			{
			iMissingDataMask |= EAssistanceDataIonosphericModel;
			}
		}

	if (iDataRequestMask & EAssistanceDataNavigationModel)
		{
		RUEPositioningGpsNavigationModelBuilder* dataItem;
		if ((KErrNone != iValidData.GetDataBuilder(dataItem)) ||
			(!dataItem->IsDataAvailable()))
			{
			iMissingDataMask |= EAssistanceDataNavigationModel;
			}
		}

	if (iDataRequestMask & EAssistanceDataReferenceLocation)
		{
		RReferenceLocationBuilder* dataItem;
		if ((KErrNone != iValidData.GetDataBuilder(dataItem)) ||
			(!dataItem->IsDataAvailable()))
			{
			iMissingDataMask |= EAssistanceDataReferenceLocation;
			}
		}

	if (iDataRequestMask & EAssistanceDataReferenceTime)
		{
		RUEPositioningGpsReferenceTimeBuilder* dataItem;
		if ((KErrNone != iValidData.GetDataBuilder(dataItem)) ||
			(!dataItem->IsDataAvailable()))
			{
			iMissingDataMask |= EAssistanceDataReferenceTime;
			}
		}

	if (iDataRequestMask & EAssistanceDataPositioningGpsUtcModel)
		{
		RUEPositioningGpsUtcModelBuilder* dataItem;
		if ((KErrNone != iValidData.GetDataBuilder(dataItem)) ||
			(!dataItem->IsDataAvailable()))
			{
			iMissingDataMask |= EAssistanceDataPositioningGpsUtcModel;
			}
		}

	if (iDataRequestMask & EAssistanceDataAquisitionAssistance)
		{
		RUEPositioningGpsAcquisitionAssistanceBuilder* dataItem;
		if ((KErrNone != iValidData.GetDataBuilder(dataItem)) ||
			(!dataItem->IsDataAvailable()))
			{
			iMissingDataMask |= EAssistanceDataAquisitionAssistance;
			}
		}

	// Make a note of the current valid set of data
	iValidDataMask = 0;
	RUEPositioningGpsAlmanacBuilder* dataItem1;
	if ((KErrNone == iValidData.GetDataBuilder(dataItem1)) &&
		(dataItem1->IsDataAvailable()))
		{
		iValidDataMask |= EAssistanceDataAlmanac;
		}

	RBadSatListBuilder* dataItem2;
	if ((KErrNone == iValidData.GetDataBuilder(dataItem2)) &&
		(dataItem2->IsDataAvailable()))
		{
		iValidDataMask |= EAssistanceDataBadSatList;
		}

	RUEPositioningGpsIonosphericModelBuilder* dataItem3;
	if ((KErrNone == iValidData.GetDataBuilder(dataItem3)) &&
		(dataItem3->IsDataAvailable()))
		{
		iValidDataMask |= EAssistanceDataIonosphericModel;
		}

	RUEPositioningGpsNavigationModelBuilder* dataItem4;
	if ((KErrNone == iValidData.GetDataBuilder(dataItem4)) &&
		(dataItem4->IsDataAvailable()))
		{
		iValidDataMask |= EAssistanceDataNavigationModel;
		}

	RReferenceLocationBuilder* dataItem5;
	if ((KErrNone == iValidData.GetDataBuilder(dataItem5)) &&
		(dataItem5->IsDataAvailable()))
		{
		iValidDataMask |= EAssistanceDataReferenceLocation;
		}

	RUEPositioningGpsReferenceTimeBuilder* dataItem6;
	if ((KErrNone == iValidData.GetDataBuilder(dataItem6)) &&
		(dataItem6->IsDataAvailable()))
		{
		iValidDataMask |= EAssistanceDataReferenceTime;
		}

	RUEPositioningGpsUtcModelBuilder* dataItem7;
	if ((KErrNone == iValidData.GetDataBuilder(dataItem7)) &&
		(dataItem7->IsDataAvailable()))
		{
		iValidDataMask |= EAssistanceDataPositioningGpsUtcModel;
		}

	RUEPositioningGpsAcquisitionAssistanceBuilder* dataItem8;
	if ((KErrNone == iValidData.GetDataBuilder(dataItem8)) &&
		(dataItem8->IsDataAvailable()))
		{
		iValidDataMask |= EAssistanceDataAquisitionAssistance;
		}

	}


/** Process assistance data request.
This is called by the protocol manager to process an assistance data request
received from LBS.

The method identifies any errors in the request and any new data items to be 
requested from the network. If this is a cancellation request then this is 
reported in the method return value.

The protocol manager (and state machines) must decide when to perform related
assistance data actions, according to other prevailing conditions.
@see CProtocolManager::DoAssistanceDataActions()

@param aDataRequest The set of assistance data requested by LBS.
@return TBool ETrue if this request represents a cancellation
*/
TBool CAssistDataMgr::ProcessDataRequest(const TLbsAsistanceDataGroup& aDataRequest)
	{
	if (KAssistDataEmpty != aDataRequest)
		{
		// Identify (in)valid parts of the request.
		TLbsAsistanceDataGroup validSet = EAssistanceDataAquisitionAssistance
										 | EAssistanceDataBadSatList
										 | EAssistanceDataNavigationModel
										 | EAssistanceDataReferenceTime
										 | EAssistanceDataIonosphericModel
										 | EAssistanceDataDgpsCorrections
										 | EAssistanceDataReferenceLocation
										 | EAssistanceDataAlmanac
										 | EAssistanceDataPositioningGpsUtcModel;

		// Set the request error mask for any invalid parts of the request
		iRequestErrorMask |= aDataRequest & ~validSet;

		// Identify additional data that must be requested.
		// If this is the first set of requested data then we do not
		// treat this as missing unless we have received the initial
		// set of data (or error) from the network.
		if ((KAssistDataEmpty != iDataRequestMask) || (iIsDataReported))
			{
			iMissingDataMask |= iDataRequestMask ^
								(iDataRequestMask | (aDataRequest & validSet));
			iMissingDataMask &= (aDataRequest & validSet) ^ 
								(iValidDataMask & (aDataRequest & validSet));
			}

		// Set the data request mask to represent valid items only
		iDataRequestMask |= aDataRequest & validSet;

		// Make sure that if we do not need to request any new data that we
		// pass back any data that is being re-requested as soon as possible
		if ((KAssistDataEmpty == iMissingDataMask) && (iValidDataMask & aDataRequest & validSet))
			{
			iIsResponseWaiting = ETrue;
			}

		}

	return (KAssistDataEmpty == aDataRequest) ? ETrue : EFalse;
	}

