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
// This file defines the class that implements the ECOM plug-in 
// interface for LBS Network Protocol Modules.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef __SUPLGATEWAYINTERFACE_H__
#define __SUPLGATEWAYINTERFACE_H__

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include "suplprotocolmanager.h"




/** Interface from LBS to the SUPL Protocol Module 
This class implements the LBS Network Gateway plug-in interface which
allows the module to be accessed by, and to interact with, LBS.
@see CLbsNetworkProtocolBase
@see MLbsNetworkProtocolObserver2

The class also implements the internal MSuplProtocolManagerObserver interface
which is used to receive calls from within the protocol module.
@see MSuplProtocolManagerObserver
*/
NONSHARABLE_CLASS(CSuplGatewayInterface)
 : public CLbsNetworkProtocolBase2, public MSuplProtocolManagerObserver
	{
	
public:

	static CSuplGatewayInterface* NewL(TAny* aParams);
	~CSuplGatewayInterface();


	// CLbsNetworkProtocolBase methods
	void RespondPrivacyRequest(	const TLbsNetSessionId& aSessionId,
								const TLbsPrivacyResponse& aResponse,
								TInt aReason);
	
	void RespondLocationRequest(const TLbsNetSessionId& aSessionId,
								TInt  aReason,
								const TPositionInfoBase& aPosInfo);

	virtual void RequestTransmitLocation(
									const TLbsNetSessionId& aSessionId,
									const TDesC& aDestination,
									TInt aPriority,
									const TLbsNetPosRequestOptionsBase& aOptions);
									
	void CancelTransmitLocation(const TLbsNetSessionId& aSessionId,
								TInt  aReason);
	
	void RequestAssistanceData(TLbsAsistanceDataGroup aDataRequestMask,
									const TLbsNetSessionIdArray& aSessionIdArray);
	
	void RequestNetworkLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelNetworkLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason);

	void RequestSelfLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelSelfLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason);

	void AdviceSystemStatus(TLbsSystemStatus aStatus);
	
	void CancelExternalLocation(const TLbsNetSessionId& aSessionId,
								TInt aReason);

	// MSuplProtocolManagerObserver methods	

	void PrivacyReq(const TLbsNetSessionId& aSessionId, TBool aEmergency, 
					const TLbsNetPosRequestPrivacy& aPrivacy,
					const TLbsExternalRequestInfo& aRequestInfo); 

	void NetworkLocationInd(const TLbsNetSessionId& aSessionId, const TPositionInfoBase& aPosInfo);

	void AssistanceDataInd(const TLbsAsistanceDataGroup& aGroupMask,
					const RLbsAssistanceDataBuilderSet& aData, TInt aReason);

	void LocationReq(const TLbsNetSessionId& aSessionId,
					const MLbsNetworkProtocolObserver2::TLbsNetProtocolService& aType, 
					const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod);

	void SessionCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason);
	void GetCapabilities(TLbsNetPosCapabilities& aCapabilities);
	void StatusUpdate(MLbsNetworkProtocolObserver2::TLbsNetProtocolServiceMask aActiveServiceMask);
	
private:

	CSuplGatewayInterface(MLbsNetworkProtocolObserver2& aObserver);
	
	void ConstructL();

private:

	/** Reference to this protocol module's observer.
	This is used to make callbacks into LBS from the protocol module.
	*/
	MLbsNetworkProtocolObserver2& iObserver;

	/** Protocol manager object pointer.
	This object is created and owned by this class and represents the
	internal manager for the protocol module. The object is used to
	make requests into the module in support of LBS interactions.
	*/
	CSuplProtocolManager* iManager;
	};
    
#endif // __SUPLGATEWAYINTERFACE_H__
