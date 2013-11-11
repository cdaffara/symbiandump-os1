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
// This file defines the class that handles the interface with the 
// network on behalf of the Test Protocol Module.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CNETWORKINTERFACE_H__
#define __CNETWORKINTERFACE_H__

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/test/lbsnetsimgateway.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>

/** Network interface mixin observer definition.
This observer interface is implemented by observers of the network interface.
*/
class MNetworkObserver
	{
public:
		
	/** Handle Measurement Control indication
	A measurement control indication has been received from the network.
	@param aRefLocation Reference location provided in the measurement control
	@param aData Assistance data provided in the measurement control
	@param aQuality Location request quality specified in the measurement control
	*/
	virtual void MeasurementControlInd(const TPositionInfoBase& aPosInfo,
		const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality,
		const TLbsNetPosRequestMethod& aPosMethod) = 0;
	
	/** Handle Measurement Control error indication
	This is called by the protocol manager durinq an active MO-LR when
	a measurement control error is received from the network.
	@param aReason The error value reported.
	*/
	virtual void MeasurementControlErrorInd(TInt aReason) = 0;
	
	/** Handle MT-LR request
	An MT-LR request has been received from the network.
	@param aReqInfo External requestor information
	@param aPrivacy Privacy request information
	*/
	virtual void MtLrReq(const TLbsExternalRequestInfo& reqInfo,
		const TLbsNetPosRequestPrivacy& aPrivacy) = 0;
	
	/** Handle MT-LR cancel indication
	An MT-LR cancel indication has been received from the network.
	@param aReason The reported reason for the cancellation.
	*/
	virtual void MtLrCancelInd(TInt aReason) = 0;
	
	/** Handle Network session result indication
	A network session result indication has been received from the network.
	@param aResult The session result.
	@param aPosition A pointer to a result position from the network.
		This can be null if no result position is provided.
	*/
	virtual void NetworkResultInd(TInt aResult, const TPositionInfoBase* aPosInfo) = 0;
	
	/** Handle Network error indication
	A network connection error indication has been reported.
	@param aReason The error reason.
	*/
	virtual void NetworkErrorInd(TInt aReason) = 0;

	/** Network request for LBS capabilities
	@param aReason The error reason.
	*/
	virtual void GetCapabilities(TLbsNetPosCapabilities& aCapabilities) = 0;
	
	/** Network request that the device should reset its assistance data
	 @param aMask The mask for the assistance data to reset */
	virtual void ResetAssistanceData(TLbsAssistanceDataGroup aMask) = 0;
};


/** Class that handles the network interface.
In this implementation it accesses the RLbsNetSim server interface 
to make requests into the LBS Network Simulator and implements the 
observer interface MLbsNetSimObserver to receive indications from 
the Simulator.
*/
NONSHARABLE_CLASS(CNetworkInterface) : public CBase, public MLbsNetSimObserver
	{
public:
	/** Network error
	*/
	enum TNetworkError
		{
		ENone,
		EConnection
		};

public:

	static CNetworkInterface* NewL(MNetworkObserver& aObserver);
	~CNetworkInterface();

	void Connect();
	TNetworkError MoLrReq(const TDesC& aDestination);
	TNetworkError MoLrCompleteInd(const TInt& aReason);
	TNetworkError MeasurementControlReq(const TLbsAsistanceDataGroup& aAssistData);
	TNetworkError LocationResp(const TPositionInfoBase& aPosInfo);
	TNetworkError LocationResp(TInt aReason);
	TNetworkError MtLrResp(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse);

	// MLbsNetSimObserver methods
	
	void ProcessMeasurementControlLocation(const TPositionInfoBase& aPosInfo,
		const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
	void ProcessMeasurementControlLocationError(TInt aReason);
	void ProcessFacilityLcsMoLrResult(TInt aResult, const TPositionInfoBase* aPosInfo);
	void ProcessRegisterLcsLocationNotification(const TLbsExternalRequestInfo& reqInfo,
		const TLbsNetPosRequestPrivacy& aPrivacy);
	void ProcessReleaseLcsLocationNotification(TInt aReason);
	void ProcessResetAssistanceData(TLbsAssistanceDataGroup aMask);
	void ProcessError(TInt aReason);
	void GetCapabilities(TLbsNetPosCapabilities& aCapabilities) const;
	
private:

	CNetworkInterface(MNetworkObserver& aObserver);
	void ConstructL();

	TNetworkError CheckConnection();
	
private:

	/** Reference to the network interface observer
	*/
	MNetworkObserver& iObserver;

	/** Network client interface
	*/
	RLbsNetSim iNetwork;

	/** Network error
	*/
	TNetworkError iNetworkError;

	/** Positioning capabilities
	These are retrieved from LBS
	*/
	TLbsNetPosCapabilities iCapabilities;
	
	TBool	iInMOLRSession;
	};

#endif // __CNETWORKINTERFACE_H__
