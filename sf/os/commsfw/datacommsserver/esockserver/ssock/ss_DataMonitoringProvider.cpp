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
// DataMonitoringProvider.cpp
// 
//

/**
 @file
 @internalComponent 
*/

#include <comms-infras/ss_nodemessages.h>
#include "ss_datamonitoringprovider.h"

#include <comms-infras/ss_log.h>

using namespace ESock;
using namespace Messages;
using namespace MeshMachine;

EXPORT_C ADataMonitoringProvider::ADataMonitoringProvider()
	{
	}

EXPORT_C void ADataMonitoringProvider::ConstructL()
	{
	InsertWrapMarkerL(EReceived);
	SetNextNotificationThreshold(EReceived);
	InsertWrapMarkerL(ESent);
	SetNextNotificationThreshold(ESent);
	}

EXPORT_C ADataMonitoringProvider::~ADataMonitoringProvider()
	{
	TUint requestIdx;
	TUint requestCount;
	TNotificationRequest* notificationRequest;

	// Cancel any outstanding received notification requests
	requestCount = iReceivedNotificationRequests.Count();
	for(requestIdx=0;requestIdx<requestCount;requestIdx++)
		{
		notificationRequest = iReceivedNotificationRequests[requestIdx];		
		if(!notificationRequest->Cancelled()) 
			{
			CDataMonitoringResponder::CancelRequest(notificationRequest->Responder());
			}
		}
	iReceivedNotificationRequests.ResetAndDestroy();

	// Cancel any outstanding sent notification requests
	requestCount = iSentNotificationRequests.Count();
	for(requestIdx=0;requestIdx<requestCount;requestIdx++)
		{
		notificationRequest = iSentNotificationRequests[requestIdx];
		if(!notificationRequest->Cancelled())
			{
			CDataMonitoringResponder::CancelRequest(notificationRequest->Responder());
			}
		}
	iSentNotificationRequests.ResetAndDestroy();
	}

/**
	Responds to client request for immediate notification of the amount of data transferred.
	@param aResponder The IPC responder for completing the client request
	@param aClientId Not used
*/
EXPORT_C void ADataMonitoringProvider::RequestDataTransferred(CDataMonitoringResponder*& aResponder, TSubSessionUniqueId /* aClientId */)
	{
	CDataMonitoringResponder::DataTransferred(aResponder, iDataVolumes.iReceivedBytes, iDataVolumes.iSentBytes);
	}

/**
	Cancels an outstanding client request for immediate notification of the amount of data transferred.
	@param aClientId Not used
	@note Method has no implementation as completion of request is immediate.
	@see ADataMonitoringProvider::RequestDataTransferred
*/
EXPORT_C void ADataMonitoringProvider::CancelDataTransferredRequest(TSubSessionUniqueId /* aClientId */)
	{
	// Nothing to do
	}

/**
	Responds to client request for notification of a particular volume of data having been
	received.
	@param aResponder The IPC responder for completing the client request
	@param aDelta An offset from the current volume transferred at which to notify the client
	@param aReceivedBytes An absolute data volume at which to notify the client
	@param aClientId Id of the requesting client
	@note Only one of the parameters aDelta and aReceivedBytes should be set for a given call
		  to this method
*/
EXPORT_C void ADataMonitoringProvider::RequestDataReceivedNotification(CDataMonitoringResponder*& aResponder, TUint32 aDelta, TUint32 aReceivedBytes, TSubSessionUniqueId aClientId)
	{	
	TNotificationRequest* notificationRequest = new TNotificationRequest(aResponder, aClientId);
	if (!notificationRequest)
		{
		CDataMonitoringResponder::Error(aResponder, KErrNoMemory);
		return;
		}

	// Ensure this client has not registered before or if it has, any outstanding requests are
	// marked as cancelled.
	TInt requestIdx = iReceivedNotificationRequests.Find(notificationRequest, TIdentityRelation<TNotificationRequest>(TNotificationRequest::CompareClientIdsAndCancelledFlag));
	if(requestIdx != KErrNotFound)
		{
		delete notificationRequest;
		CDataMonitoringResponder::Error(aResponder, KErrInUse);
		return;
		}
	
	if(aDelta)
		{ 	// If the delta was set then set the threshold from that and the current received data volume
		notificationRequest->SetThresholdFromDelta(iDataVolumes.iReceivedBytes, aDelta);
		}
	else
		{	// Otherwise, we were given an absolute value for the threshold
		if(iDataVolumes.iReceivedBytes >= aReceivedBytes) 
			{	// If the required threshold has already been reached then send immediate notification
			delete notificationRequest;
			CDataMonitoringResponder::DataReceivedNotification(aResponder, iDataVolumes.iReceivedBytes);
			return;
			}
		else
			{
			notificationRequest->SetThreshold(aReceivedBytes);
			}
		}

	TInt error = InsertNotificationRequest(notificationRequest, EReceived);
	if (error!=KErrNone)
		{
		delete notificationRequest;	
		CDataMonitoringResponder::Error(aResponder, error);
		return;
		}

	SetNextNotificationThreshold(EReceived);
	}

/**
	Cancels an outstanding request for data received notification.
	
	@param aClientId Id of the requesting client
*/
EXPORT_C void ADataMonitoringProvider::CancelDataReceivedNotificationRequest(TSubSessionUniqueId aClientId)
	{
	// Set up an exemplar request using the specified client ID
	TNotificationRequest exemplarRequest(NULL, aClientId);
	
	// Locate the request matching the specified client ID
	TInt requestIdx = iReceivedNotificationRequests.Find(&exemplarRequest, TIdentityRelation<TNotificationRequest>(TNotificationRequest::CompareClientIdsAndCancelledFlag));	
	if(requestIdx != KErrNotFound)
		{
		TNotificationRequest* notificationRequest = iReceivedNotificationRequests[requestIdx];
		
		// Cancel it and complete the client
		notificationRequest->SetCancelled();
		CDataMonitoringResponder::CancelRequest(notificationRequest->Responder());
		
		iReceivedNotificationRequests.Remove(requestIdx); 
        delete notificationRequest;
		}
	}

/**

*/
EXPORT_C void ADataMonitoringProvider::RequestDataSentNotification(CDataMonitoringResponder*& aResponder, TUint32 aDelta, TUint32 aSentBytes, TSubSessionUniqueId aClientId)
	{	
	TNotificationRequest* notificationRequest = new TNotificationRequest(aResponder, aClientId);
	if (!notificationRequest)
		{
		CDataMonitoringResponder::Error(aResponder, KErrNoMemory);
		return;
		}
	
	// Ensure this client has not registered before or if it has, any outstanding requests are
	// marked as cancelled.
	TInt requestIdx = iSentNotificationRequests.Find(notificationRequest, TIdentityRelation<TNotificationRequest>(TNotificationRequest::CompareClientIdsAndCancelledFlag));
	if(requestIdx != KErrNotFound)
		{
		delete notificationRequest;
		CDataMonitoringResponder::Error(aResponder, KErrInUse);
		return;
		}
		
	if(aDelta)
		{ 	// If the delta was set then set the threshold from that and the current sent data volume
		notificationRequest->SetThresholdFromDelta(iDataVolumes.iSentBytes, aDelta);
		}
	else
		{	// Otherwise, we were given an absolute value for the threshold
		if(iDataVolumes.iSentBytes >= aSentBytes)
			{	// If the required threshold has already been reached then send immediate notification
			delete notificationRequest;
			CDataMonitoringResponder::DataSentNotification(aResponder, iDataVolumes.iSentBytes);
			return;
			}
		else
			{
			notificationRequest->SetThreshold(aSentBytes);
			}
		}

	TInt error = InsertNotificationRequest(notificationRequest, ESent);
	if (error!=KErrNone)
		{
		delete notificationRequest;	
		CDataMonitoringResponder::Error(aResponder, error);
		return;
		}

	SetNextNotificationThreshold(ESent);
	}

/**
	Cancels an outstanding request for data sent notification.
	
	@param aClientId Client id of the request to be cancelled
*/
EXPORT_C void ADataMonitoringProvider::CancelDataSentNotificationRequest(TSubSessionUniqueId aClientId)
	{
	// Set up an exemplar request using the specified client id
	TNotificationRequest exemplarRequest(NULL, aClientId);
	
	// Locate the request matching the specified client id
	TInt requestIdx = iSentNotificationRequests.Find(&exemplarRequest, TIdentityRelation<TNotificationRequest>(TNotificationRequest::CompareClientIdsAndCancelledFlag));
	if(requestIdx != KErrNotFound)
		{
		TNotificationRequest* notificationRequest = iSentNotificationRequests[requestIdx];
		
		// Cancel it and complete the client
		notificationRequest->SetCancelled();
		CDataMonitoringResponder::CancelRequest(notificationRequest->Responder());

		iSentNotificationRequests.Remove(requestIdx);
		delete notificationRequest;
		}
	}

/**
	Call from data to control plane to indicate the received data volume notification
	threshold has been reached.
	
	@param aNotificationValue The volume of data received
*/
EXPORT_C void ADataMonitoringProvider::DataReceivedNotificationL(TUint32 aNotificationValue)
	{
	TNotificationRequest* request;
	
	// Complete the first request in the array immediately and loop for any
	// additional same value thresholds in the list.
	while((iReceivedNotificationRequests.Count() > 0) && 
		  (aNotificationValue >= iReceivedNotificationRequests[0]->Threshold()))
		{
		request = iReceivedNotificationRequests[0];
		if(!request->Cancelled())
			{	// Only complete if the request wasn't cancelled
			CDataMonitoringResponder::DataReceivedNotification(request->Responder(), aNotificationValue);
			}
		iReceivedNotificationRequests.Remove(0);
		delete request;
		}

	// If we've completed a wrap marker, we need to do a little housekeeping
	if(aNotificationValue == KMaxTUint32) 
		{	
		TInt requestCount = iReceivedNotificationRequests.Count();
		if(requestCount > 0)
			{
			// The remaining requests had been marked wrapped.  Now we've
			// completed the wrap marker they need to be 'unmarked' to allow
			// new wrapped requests to be positioned correctly in the request array
			for(TUint requestIdx=0;requestIdx<requestCount;requestIdx++)
				{
				iReceivedNotificationRequests[requestIdx]->SetWrapped(EFalse);
				}
			}
			InsertWrapMarkerL(EReceived);
		}
	SetNextNotificationThreshold(EReceived);
	}
	
/**
	Call from data to control plane to indicate the sent data volume notification
	threshold has been reached.
	
	@param aNotificationValue The volume of data received
*/
EXPORT_C void ADataMonitoringProvider::DataSentNotificationL(TUint32 aNotificationValue)
	{
	TNotificationRequest* request;
	
	// Complete the first request in the array immediately and loop for any
	// additional same value thresholds in the list.
	while((iSentNotificationRequests.Count() > 0) && 
		  (aNotificationValue >= iSentNotificationRequests[0]->Threshold()))
		{
		request = iSentNotificationRequests[0];
		if(!request->Cancelled())
			{	// Only complete if the request wasn't cancelled
			CDataMonitoringResponder::DataSentNotification(request->Responder(), aNotificationValue);
			}
		iSentNotificationRequests.Remove(0);
		delete request;
		}

	// If we've completed a wrap marker, we need to do a little housekeeping
	if(aNotificationValue == KMaxTUint32) 
		{	
		TInt requestCount = iSentNotificationRequests.Count();
		if(requestCount > 0)
			{
			// The remaining requests had been marked wrapped.  Now we've
			// completed the wrap marker they need to be 'unmarked' to allow
			// new wrapped requests to be positioned correctly in the request array
			for(TUint requestIdx=0;requestIdx<requestCount;requestIdx++)
				{
				iSentNotificationRequests[requestIdx]->SetWrapped(EFalse);
				}
			}
			InsertWrapMarkerL(ESent);
		}
	SetNextNotificationThreshold(ESent);
	}


EXPORT_C void ADataMonitoringProvider::CancelClientExtItfRequests(TSubSessionUniqueId aClientId)
	{
	CancelDataSentNotificationRequest(aClientId);
	CancelDataReceivedNotificationRequest(aClientId);
	}

/**
    Call from a node activity upon receiving TCFMessage::TDataMonitoringInternal 
    message extension.
*/
EXPORT_C void ADataMonitoringProvider::DataNotificationL(const ESock::TCFDataMonitoringNotification::TDataMonitoringNotification& aNotification)
    {
	switch(aNotification.iValue1)
		{
		case EReceived:
			DataReceivedNotificationL(aNotification.iValue2);
			break;
			
		case ESent:
			DataSentNotificationL(aNotification.iValue2);
			break;
			
		default:
			break;
		}	
    }
    
/**

*/
void ADataMonitoringProvider::InsertWrapMarkerL(TDataMonitoringDirection aDirection)
	{
	TNotificationRequest* wrapMarkerRequest = new(ELeave) TNotificationRequest(NULL, 0);
	
	wrapMarkerRequest->SetThreshold(KMaxTUint32);
	wrapMarkerRequest->SetCancelled();
	
	TInt ret = NotificationRequestArray(aDirection)->InsertInOrder(wrapMarkerRequest, TLinearOrder<TNotificationRequest>(TNotificationRequest::CompareThresholds));
	
	if(ret != KErrNone)
		{	
		// If the insert failed then the most likely reason is a KErrAlreadyExists.  In any
		// case, we definitely want to delete the wrap marker.  Then we'll try and carry on
		// as best we can.
		delete wrapMarkerRequest;
		}				
	}


/**

*/
RNotificationRequestArray* ADataMonitoringProvider::NotificationRequestArray(TDataMonitoringDirection aDirection)
	{
	if(aDirection == EReceived)
		{
		return &iReceivedNotificationRequests;
		}
	else
		{
		return &iSentNotificationRequests;
		}
	}

/**

*/
TInt ADataMonitoringProvider::InsertNotificationRequest(TNotificationRequest* aRequest, TDataMonitoringDirection aDirection)
	{
	TInt err;
	err = NotificationRequestArray(aDirection)->InsertInOrderAllowRepeats(aRequest, TLinearOrder<TNotificationRequest>(TNotificationRequest::CompareThresholds));

	LOG(
		switch(aDirection)
			{
		case ESent:
			ESockLog::Printf(_L("ADataMonitoringProvider(%08x)::InsertNotificationRequest - Direction: Sending"), this);
			break;			
		case EReceived:
			ESockLog::Printf(_L("ADataMonitoringProvider(%08x)::InsertNotificationRequest - Direction: Receiving"), this);
			break;
			}
		
		for(TInt idx=0;idx<NotificationRequestArray(aDirection)->Count();idx++)
			{
			TNotificationRequest* req = (*NotificationRequestArray(aDirection))[idx];
			ESockLog::Printf(_L("\tRequest %d, Threshold %d, Wrapped %d, Client id. 0x%08x"), idx, req->Threshold(), req->Wrapped(), req->ClientId());
			}
		)
	
	return err;
	}

/**
*/
void ADataMonitoringProvider::SetNextNotificationThreshold(TDataMonitoringDirection aDirection) 
	{
	if(NotificationRequestArray(aDirection)->Count())
		{
		switch(aDirection)
			{
		case EReceived:			
			iThresholds.iReceivedThreshold = (*NotificationRequestArray(EReceived))[0]->Threshold();				
			break;

		case ESent:			
			iThresholds.iSentThreshold = (*NotificationRequestArray(ESent))[0]->Threshold();			
			break;
			}
		}
	}


/**

*/
EXPORT_C TDataMonitoringConnProvisioningInfo::TDataMonitoringConnProvisioningInfo(TDataVolumes* aDataVolumesPtr, TNotificationThresholds* aThresholdsPtr) :
	TDataMonitoringProvisioningInfoBase(aDataVolumesPtr, aThresholdsPtr)
	{
	}


/**

*/
EXPORT_C TDataMonitoringSubConnProvisioningInfo::TDataMonitoringSubConnProvisioningInfo(TDataVolumes* aDataVolumesPtr, TNotificationThresholds* aThresholdsPtr) :
	TDataMonitoringProvisioningInfoBase(aDataVolumesPtr, aThresholdsPtr)
	{
	}


/**
	Sets the data volume notification threshold given the current transferred volume
	and a delta from the current amount.
	
	@param aVolume Current data volume transferred
	@param aDelta An offset from the current volume of data transferred
*/
void TNotificationRequest::SetThresholdFromDelta(TUint32 aVolume, TUint32 aDelta)
	{
	TUint64 threshold = aVolume + aDelta;
	
	if(threshold > KMaxTUint32) 
		{
		iWrapped = ETrue;
		iThreshold = threshold - KMaxTUint32;
		}
	else
		{
		iWrapped = EFalse;
		iThreshold = threshold;
		}	
	}
		
/**
	Compares two TNotificationRequest objects using their thresholds as the criteria.
*/
TInt TNotificationRequest::CompareThresholds(const TNotificationRequest& aFirst, const TNotificationRequest& aSecond)
	{
	TUint64 effectiveFirstThreshold = aFirst.iThreshold;
	TUint64 effectiveSecondThreshold = aSecond.iThreshold;
	
	if(aFirst.Wrapped()) effectiveFirstThreshold += KMaxTUint32;
	if(aSecond.Wrapped()) effectiveSecondThreshold += KMaxTUint32;

	if(effectiveFirstThreshold > effectiveSecondThreshold)
		{
		return 1;
		}
	else if(effectiveFirstThreshold < effectiveSecondThreshold) 
		{
		return -1;
		}
	else
		{
		return 0;
		}
	}

/**
	Compares two TNotificationRequest objects using their client ids as the criteria.
*/
TBool TNotificationRequest::CompareClientIds(const TNotificationRequest& aFirst, const TNotificationRequest& aSecond)
	{
	if(aFirst.ClientId() == aSecond.ClientId())
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
    Compares two TNotificationRequest objects using their client ids and the cancelled flag as the criteria.
*/
TBool TNotificationRequest::CompareClientIdsAndCancelledFlag(const TNotificationRequest& aFirst, const TNotificationRequest& aSecond)
    {
    if(aFirst.ClientId() == aSecond.ClientId())
        {
        return aFirst.Cancelled() == aSecond.Cancelled();
        }
    else
        {
        return EFalse;
        }
    }

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TDataMonitoringProvisioningInfoBase, TDataMonitoringProvisioningInfoBase::iUid, TDataMonitoringProvisioningInfoBase::iId)
	REGISTER_ATTRIBUTE(TDataMonitoringProvisioningInfoBase, iDataVolumesPtr, TMeta<TDataMonitoringProvisioningInfoBase*>)
	REGISTER_ATTRIBUTE(TDataMonitoringProvisioningInfoBase, iThresholdsPtr, TMeta<TDataMonitoringProvisioningInfoBase*>)
END_ATTRIBUTE_TABLE()

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TDataMonitoringConnProvisioningInfo, TDataMonitoringProvisioningInfoBase::iUid, TDataMonitoringConnProvisioningInfo::iId)
END_ATTRIBUTE_TABLE_BASE(TDataMonitoringProvisioningInfoBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TDataMonitoringSubConnProvisioningInfo, TDataMonitoringProvisioningInfoBase::iUid, TDataMonitoringSubConnProvisioningInfo::iId)
END_ATTRIBUTE_TABLE_BASE(TDataMonitoringProvisioningInfoBase, 0)

