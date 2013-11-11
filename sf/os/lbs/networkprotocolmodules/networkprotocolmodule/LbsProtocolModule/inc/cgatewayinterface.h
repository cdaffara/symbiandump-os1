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
// This file defines the class that implements the ECOM plug-in 
// interface for LBS Network Protocol Modules.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CGATEWAYINTERFACE_H__
#define __CGATEWAYINTERFACE_H__

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include "cprotocolmanager.h"


/** LBS Network Protocol Interface 
This class implements the LBS Network Gateway plug-in interface which
allows the module to be accessed by, and to interact with, LBS.
@see CLbsNetworkProtocolBase
@see MLbsNetworkProtocolObserver

The class also implements the internal MProtocolMgrObserver interface
which is used to receive calls from within the protocol module.
@see MProtocolMgrObserver
*/
NONSHARABLE_CLASS(CGatewayInterface)
 : public CLbsNetworkProtocolBase, public MProtocolMgrObserver
	{
	
public:

	static CGatewayInterface* NewL(TAny* aParams);
	~CGatewayInterface();


	// CLbsNetworkProtocolBase methods

	void RespondPrivacyRequest(const TLbsNetSessionId& aSessionId,
								const TLbsPrivacyResponse& aResponse);
	
	void RespondLocationRequest(const TLbsNetSessionId& aSessionId,
								TInt  aReason,
								const TPositionInfoBase& aPosInfo);

	void RequestTransmitLocation(const TLbsNetSessionId& aSessionId,
								const TDesC& aDestination,
								TInt aPriority);
									
	void CancelTransmitLocation(const TLbsNetSessionId& aSessionId,
								TInt  aReason);
	
	void RequestAssistanceData(TLbsAsistanceDataGroup aDataRequestMask);
	
	void RequestNetworkLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelNetworkLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason);

	void RequestSelfLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelSelfLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason);

	void AdviceSystemStatus(TLbsSystemStatus aStatus);

	// MProtocolMgrObserver methods	

	void PrivacyReq(const TLbsNetSessionId& aSessionId, TBool aEmergency, 
					const TLbsNetPosRequestPrivacy& aPrivacy,
					const TLbsExternalRequestInfo& aRequestInfo); 

	void NetworkLocationInd(const TLbsNetSessionId& aSessionId, const TPositionInfoBase& aPosInfo);

	void AssistanceDataInd(const TLbsAsistanceDataGroup& aGroupMask,
					const RLbsAssistanceDataBuilderSet& aData, TInt aReason);

	void LocationReq(const TLbsNetSessionId& aSessionId, TBool aEmergency,
					const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aType, 
					const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod);

	void SessionCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason);
	void GetCapabilities(TLbsNetPosCapabilities& aCapabilities);
	void StatusUpdate(MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask aActiveServiceMask);
	void ResetAssistanceData(TLbsAssistanceDataGroup aMask, const RLbsAssistanceDataBuilderSet& aData);
	
private:

	CGatewayInterface(MLbsNetworkProtocolObserver& aObserver);
	
	void ConstructL();

private:

	/** Reference to this protocol module's observer.
	This is used to make callbacks into LBS from the protocol module.
	*/
	MLbsNetworkProtocolObserver& iObserver;

	/** Protocol manager object pointer.
	This object is created and owned by this class and represents the
	internal manager for the protocol module. The object is used to
	make requests into the module in support of LBS interactions.
	*/
	CProtocolManager* iManager;
	
	};
    
#endif // __CGATEWAYINTERFACE_H__
