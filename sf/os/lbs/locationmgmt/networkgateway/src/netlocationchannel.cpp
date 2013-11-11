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
// Definition of network location channel component.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32base.h>
#include "lbsdevloggermacros.h"
#include "lbsprocessuiddefs.h"
#include "netlocationchannel.h"


//
// CNetworkLocationChannel
//

CNetworkLocationChannel::CNetworkLocationChannel(MNetworkLocationObserver& aObserver) : 
	CActive(EPriorityStandard),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}
	
CNetworkLocationChannel::~CNetworkLocationChannel()
	{
	Cancel();
	iFinalPositionUpdates.Close();
	iReferencePositionUpdates.Close();
	iLocationResponses.Close();
	iLocationRequests.Close();
	}

CNetworkLocationChannel* CNetworkLocationChannel::NewL(MNetworkLocationObserver& aObserver)
	{
	CNetworkLocationChannel* self = new (ELeave) CNetworkLocationChannel(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
		
void CNetworkLocationChannel::ConstructL()
	{	
	iLocationRequests.OpenL(RProcess().SecureId());
	iLocationResponses.OpenL(RProcess().SecureId());
	iReferencePositionUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
	iFinalPositionUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionFinal);
	NotifyNetworkLocationRequestUpdate();
	}
	
void CNetworkLocationChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	NotifyNetworkLocationRequestUpdate();
	TLbsNetLocMsgBase msg;
	iLocationRequests.GetNetworkLocationRequest(msg);
	iObserver.ProcessNetworkLocationMessage(msg);
	}
	
void CNetworkLocationChannel::DoCancel()
	{
	iLocationRequests.CancelNotifyNetworkLocationRequestUpdate();
	}
	
TInt CNetworkLocationChannel::RunError(TInt aError)
	{
	return aError;
	}

const TPositionInfoBase	& CNetworkLocationChannel::GetLastReferencePosition()
	{
	return iRefPosInfo;
	}

void CNetworkLocationChannel::SendNetworkLocationMessage(const TLbsNetLocMsgBase& aMessage)
	{
	iLocationResponses.SetNetworkLocationResponse(aMessage);
	}

void CNetworkLocationChannel::NotifyNetworkLocationRequestUpdate()
	{
	__ASSERT_DEBUG(!IsActive(), User::Invariant());	
	iLocationRequests.NotifyNetworkLocationRequestUpdate(iStatus);
	SetActive();
	}

/* Store the latest position to be sent in from the network.

The network can send two types of position:
1) A 'reference' position - this is sent in before each location request.
   it is usually a fairly inaccurate position, e.g. the area of error
   can be as large as the size of one cell.
2) A 'final' position - this is sent at the end of a location request, before
   the final 'session complete' closes the request. It represents a position 
   returned from the network and may be more accurate than the position
   the terminal sent to the network in reply to the original location request.
   
Both types of location are published on an internal interface so that
other LBS components can access them.
*/
void CNetworkLocationChannel::SetNetworkPosition(const TLbsNetSessionIdInt& aSessionId, const TPositionInfoBase& aPosition)
	{
	// Check that aPosition is a valid TPositionInfoBase type
	TUint32 type = aPosition.PositionClassType();
	if (((type & EPositionInfoUnknownClass) != EPositionInfoUnknownClass)
	    && ((type & EPositionInfoClass) != EPositionInfoClass)
	    && ((type & EPositionCourseInfoClass) != EPositionCourseInfoClass)
	    && ((type & EPositionSatelliteInfoClass) != EPositionSatelliteInfoClass)
	    && ((type & EPositionExtendedSatelliteInfoClass) != EPositionExtendedSatelliteInfoClass))
		{
		// If the aPosition is not a supported positionInfo type, ignore it.
		// (Panic in debug builds)
		__ASSERT_DEBUG(EFalse, User::Panic(_L("LbsNetGateway"), 1));
		return;
		}
	
	// Treat the position as a reference position or final position based
	// on the technology type.
	switch (aPosition.PositionMode())
		{
		case TPositionModuleInfo::ETechnologyNetwork:
			{
			SetReferenceNetworkPosition(aSessionId, aPosition);
			break;
			}
 		case (TPositionModuleInfo::ETechnologyTerminal):
 			 // this will only happen if PM / AGPS module not setting the  'assisted' bit as they should
 			 __ASSERT_DEBUG(EFalse, User::Panic(_L("LbsNetGateway"), 1));
 			 // drop through				
		case (TPositionModuleInfo::ETechnologyNetwork 
			  | TPositionModuleInfo::ETechnologyAssisted):
		case (TPositionModuleInfo::ETechnologyTerminal 
			  | TPositionModuleInfo::ETechnologyAssisted):
			{
			SetFinalNetworkPosition(aSessionId, aPosition);
			break;
			}
		default:
			{
			// Default to treating network positions as a reference position
			SetReferenceNetworkPosition(aSessionId, aPosition);
			break;	
			}
		}
	}

void CNetworkLocationChannel::SetReferenceNetworkPosition(const TLbsNetSessionIdInt& aSessionId, const TPositionInfoBase& aLocation)
	{
	// Store the most recent reference position, so that it can be
	// read when sending a location request to the NRH.

	// The TPositionInfoBase is just the base class, so we need to copy over 
	// the data from the actual concrete class type.
	__ASSERT_DEBUG(aLocation.PositionClassSize() <= sizeof(TPositionExtendedSatelliteInfo), User::Invariant());
	Mem::Copy(&iRefPosInfo, &aLocation, aLocation.PositionClassSize());	
	
	// Broadcast the reference position to the other LBS components.
	TTime actualTime;
	actualTime.UniversalTime();
	iReferencePositionUpdates.SetPositionInfo(aSessionId, KErrNone, aLocation, actualTime);
	}

void CNetworkLocationChannel::SetFinalNetworkPosition(const TLbsNetSessionIdInt& aSessionId, const TPositionInfoBase& aLocation)
	{
	// Set the moduleID based on the PositionMode() of the position info
	// Make a copy so we don't change the posInfo that is owned by the protocol module.
	__ASSERT_DEBUG(iFinalPosInfo.PositionClassSize() >= aLocation.PositionClassSize(), 
				   User::Invariant());
	Mem::Copy(&iFinalPosInfo, &aLocation, aLocation.PositionClassSize());
	switch (iFinalPosInfo.PositionMode())
		{
		case (TPositionModuleInfo::ETechnologyTerminal
			  | TPositionModuleInfo::ETechnologyAssisted):
			{
			iFinalPosInfo.SetModuleId(KLbsGpsLocManagerUid);
			break;
			}
		case (TPositionModuleInfo::ETechnologyNetwork
			  | TPositionModuleInfo::ETechnologyAssisted):
			{
			iFinalPosInfo.SetModuleId(KLbsNetLocManagerUid);
			break;
			}
		default:
			{
			LBSLOG_WARN2(ELogP3, "Unrecognised PositionMode() for a Final Network Position (%d), ignoring update", iFinalPosInfo.PositionMode());
			break;
			}
		}
	
	// Broadcast the final position to the other LBS components.
	TTime actualTime;
	actualTime.UniversalTime();
	iFinalPositionUpdates.SetPositionInfo(aSessionId, KErrNone, iFinalPosInfo, actualTime);
	}
