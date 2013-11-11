// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
 @released
*/

#include "lbsprocessuiddefs.h"
#include "locationupdatemonitor.h"


CLbsLocUpdateMonitor::CLbsLocUpdateMonitor(
		MLbsLocUpdateObserver& aObserver) :
	CActive(EPriorityStandard),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

CLbsLocUpdateMonitor::~CLbsLocUpdateMonitor()
	{
	Cancel();
	iLbsPositionUpdateRequestor.Close();
	iLbsPositionUpdateMonitor.Close();
	}

CLbsLocUpdateMonitor* CLbsLocUpdateMonitor::NewL(
		MLbsLocUpdateObserver& aObserver)
	{
	CLbsLocUpdateMonitor* self = new (ELeave) CLbsLocUpdateMonitor(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLbsLocUpdateMonitor::ConstructL()
	{
	iLbsPositionUpdateMonitor.OpenL(KLbsGpsLocManagerUid);
	
    RLbsPositionUpdateRequests::TChannelIdentifer channelId;
    channelId.iModuleId = KLbsGpsLocManagerUid;
    channelId.iClientId = KLbsNetRequestHandlerUid;
	iLbsPositionUpdateRequestor.OpenL(channelId);
	}
	 
void CLbsLocUpdateMonitor::StartMonitor()
	{
	if (!IsActive())
		{
		iLbsPositionUpdateMonitor.NotifyPositionUpdate(iStatus);
		SetActive();
		}
	}

void CLbsLocUpdateMonitor::StopMonitor()
	{
	Cancel();
	}

void CLbsLocUpdateMonitor::SendLocationRequest(
		TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice,
		const TLbsNetPosRequestMethodInt& aMethod,
		const TLbsLocRequestQualityInt& aQuality)
	{
	TLbsPositionUpdateRequest updateRequest;
	updateRequest.SetPowerAdvice(aPowerAdvice);
	updateRequest.TargetTime() = TTime(0);
	updateRequest.RequestQuality() = aQuality;
	updateRequest.RequestMethod() = aMethod;
	iLbsPositionUpdateRequestor.SetPositionUpdateRequest(updateRequest);	
	}

void CLbsLocUpdateMonitor::SendCancelRequest(
		TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice)
	{
	TLbsPositionUpdateRequestCancel cancel;
	cancel.SetPowerAdvice(aPowerAdvice);
	iLbsPositionUpdateRequestor.SetPositionUpdateRequest(cancel);	
	}


void CLbsLocUpdateMonitor::SendPowerAdviceRequest(
		TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice)
	{
   	TLbsPositionUpdateRequest request;
   	iLbsPositionUpdateRequestor.GetPositionUpdateRequest(request);
	
	if ( request.Type()==TLbsPositionUpdateRequestBase::ECancel )
		{
		// if the last thing we sent to the manager was a cancel then re-issue it
		// (with the correct PowerAdvice) to prevent the cancel being overwritten and therefore lost
		TLbsPositionUpdateRequestCancel cancel;
		cancel.Tracking() = EFalse; 	// cancel always disables tracking 
		cancel.SetPowerAdvice(aPowerAdvice);
		iLbsPositionUpdateRequestor.SetPositionUpdateRequest(cancel);
		}
	else
		{
		TLbsPositionUpdateRequestStatus powerAdvice;
		powerAdvice.SetPowerAdvice(aPowerAdvice);
		iLbsPositionUpdateRequestor.SetPositionUpdateRequest(powerAdvice);
		}
	}

void CLbsLocUpdateMonitor::GetPosition(
		TBool& aConflictControl,
		TInt& aReason,
		TPositionExtendedSatelliteInfo& aPosInfo,
		TTime& aActualTime)
	{
	TTime targetTime;
	aReason = iLbsPositionUpdateMonitor.GetPositionInfo(
					aConflictControl,
					&aPosInfo, 
					sizeof(aPosInfo),
					targetTime, 
					aActualTime);
	}
	
void CLbsLocUpdateMonitor::RunL()
	{
	// Re-subscribe
	StartMonitor();
	
	// Get the latest update
	TBool conflictControl;
	TInt posReason;
	TPositionExtendedSatelliteInfo posInfo;
	TTime actualTime;
	GetPosition(conflictControl, posReason, posInfo, actualTime);
	
	// Notify the observer
	iObserver.OnPositionUpdate(conflictControl,
								posReason,
								posInfo,
								actualTime);
	}

TInt CLbsLocUpdateMonitor::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

void CLbsLocUpdateMonitor::DoCancel()
	{
	iLbsPositionUpdateMonitor.CancelNotifyPositionUpdate();
	}

