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
// Contains interface to CNetObserver class that 'observes' protocol messages from 
// a location protocol module.
// 
//

/**
 @file
 @internalTecnology
 @released
*/

#ifndef LBSNETOBSERVER_H
#define LBSNETOBSERVER_H

#include <lbs/lbsnetprotocolbase.h>

class CNetworkGateway;

class CNetObserver : public MLbsNetworkProtocolObserver2
	{
public:
	//construction
	CNetObserver(CNetworkGateway* aParent, TLbsProtocolModuleId aModuleId);
	

	void GetCurrentCapabilities(TLbsNetPosCapabilities& aCapabilities) const;

	void ProcessStatusUpdate(TLbsNetProtocolServiceMask aActiveServiceMask);
	
	void ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId,
							   TBool aEmergency, 
							   const TLbsNetPosRequestPrivacy& aPrivacy,
							   const TLbsExternalRequestInfo& aRequestInfo);
	
	void ProcessLocationRequest(const TLbsNetSessionId& aSessionId,
								TBool aEmergency,
								TLbsNetProtocolService aService, 
								const TLbsNetPosRequestQuality& aQuality,
								const TLbsNetPosRequestMethod& aMethod);

	void ProcessSessionComplete(const TLbsNetSessionId& aSessionId,
								TInt  aReason);

	void ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
							   const RLbsAssistanceDataBuilderSet& aData,
							   TInt aReason);
	
	void ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
								const RLbsAssistanceDataBuilderSet& aData,
								TInt aReason,
								const TLbsNetSessionIdArray& aSessionIdArray);
	
	void ProcessLocationUpdate(const TLbsNetSessionId& aSessionId,
								const TPositionInfoBase& aPosInfo);

private:

		// member variables
		CNetworkGateway* iParentToCall;
	    TLbsProtocolModuleId iModuleUId;
	};

#endif /*LBSNETOBSERVER_H*/
