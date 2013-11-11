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
#include "lbsngnlmint.h"
#include "netlocationchannel.h"
#include "lbsprocessuiddefs.h"

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
	iPositionUpdates.Close();
	iLocationRequests.Close();
	iLocationResponses.Close();
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
	iLocationRequests.OpenL(KLbsNetLocManagerUid);
	iLocationResponses.OpenL(KLbsNetLocManagerUid);
	iPositionUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
	NotifyNetworkLocationRequestUpdate();
	}
	
void CNetworkLocationChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	NotifyNetworkLocationRequestUpdate();
	const TLbsNetSessionIdInt KInvalidSessionId;
	TLbsNetLocNetworkLocationRequestMsg msg(KInvalidSessionId, EFalse, TLbsNetPosRequestQualityInt());
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

void CNetworkLocationChannel::SetReferencePosition(TLbsNetSessionIdInt& aSessionId, const TPositionInfoBase& aLocation)
	{
	TTime actualTime;
	iPositionUpdates.SetPositionInfo(aSessionId, KErrNone, aLocation, actualTime);
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

