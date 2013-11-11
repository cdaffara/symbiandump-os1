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
// Contains implementation to CNetObserver class that 'observes' protocol messages from
// a location protocol module.
// 
//

#include "netgateway.h"
#include "netobserver.h"

// constructor

CNetObserver::CNetObserver(CNetworkGateway* aParent, TLbsProtocolModuleId aModuleId)
	{
	iParentToCall = aParent;
	iModuleUId = aModuleId;
	}

void CNetObserver::GetCurrentCapabilities(TLbsNetPosCapabilities& aCapabilities) const
	{
	// Simply record from which PM this was received and call the parent equivalent of this function
	iParentToCall->SetReceivingProtocolModule(iModuleUId);
	iParentToCall->GetCurrentCapabilities(aCapabilities);
	}

void CNetObserver::ProcessStatusUpdate(TLbsNetProtocolServiceMask aActiveServiceMask)
	{
	// Simply record from which PM this was received and call the parent equivalent of this function
	iParentToCall->SetReceivingProtocolModule(iModuleUId);
	iParentToCall->ProcessStatusUpdate(aActiveServiceMask);
	}

void CNetObserver::ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId,
										    TBool aEmergency, 
										    const TLbsNetPosRequestPrivacy& aPrivacy,
										    const TLbsExternalRequestInfo& aRequestInfo)
	{
	// Simply record from which PM this was received and call the parent equivalent of this function
	iParentToCall->SetReceivingProtocolModule(iModuleUId);
	iParentToCall->ProcessPrivacyRequest(aSessionId, aEmergency, aPrivacy, aRequestInfo);
	}

void CNetObserver::ProcessLocationRequest(const TLbsNetSessionId& aSessionId,
											 TBool aEmergency,
											 TLbsNetProtocolService aService, 
											 const TLbsNetPosRequestQuality& aQuality,
											 const TLbsNetPosRequestMethod& aMethod)
	{
	// Simply record from which PM this was received and call the parent equivalent of this function
	iParentToCall->SetReceivingProtocolModule(iModuleUId);
	iParentToCall->ProcessLocationRequest(aSessionId, aEmergency, aService, aQuality, aMethod);
	}

void CNetObserver::ProcessSessionComplete(const TLbsNetSessionId& aSessionId,
											 TInt  aReason)
	{
	// Simply record from which PM this was received and call the parent equivalent of this function
	iParentToCall->SetReceivingProtocolModule(iModuleUId);
	iParentToCall->ProcessSessionComplete(aSessionId, aReason);
	}

void CNetObserver::ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
										 const RLbsAssistanceDataBuilderSet& aData,
						   				 TInt aReason)
	{
	// Simply record from which PM this was received and call the parent equivalent of this function
	iParentToCall->SetReceivingProtocolModule(iModuleUId);
	iParentToCall->ProcessAssistanceData(aDataMask, aData, aReason);
	}


void CNetObserver::ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
								const RLbsAssistanceDataBuilderSet& aData,
								TInt aReason,
								const TLbsNetSessionIdArray& aSessionIdArray)
	{
	// Simply record from which PM this was received and call the parent equivalent of this function
	iParentToCall->SetReceivingProtocolModule(iModuleUId);
	iParentToCall->ProcessAssistanceData(aDataMask, aData, aReason, aSessionIdArray);
	}



void CNetObserver::ProcessLocationUpdate(const TLbsNetSessionId& aSessionId,
											 const TPositionInfoBase& aPosInfo)
	{
	// Simply record from which PM this was received and call the parent equivalent of this function
	iParentToCall->SetReceivingProtocolModule(iModuleUId);
	iParentToCall->ProcessLocationUpdate(aSessionId, aPosInfo);
	}
