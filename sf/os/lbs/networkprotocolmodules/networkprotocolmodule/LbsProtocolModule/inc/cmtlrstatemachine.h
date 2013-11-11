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
// This file defines the class for the MT-LR protocol state machine.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CMTLRSTATEMACHINE_H__
#define __CMTLRSTATEMACHINE_H__

#include "cstatemachinebase.h"


/** MT-LR protocol state machine.
This class manages the MT-LR protocol procedure and is employed when the protocol
module is required to handle interactions for a mobile terminate locate request.

The class implements the pure virtual methods defined by CStateMachineBase.
@see CStateMachineBase

The class also implements a timer observer interface defined by MLbsCallbackTimerObserver
@see MLbsCallbackTimerObserver
*/
NONSHARABLE_CLASS(CMtLrStateMachine) : public CStateMachineBase, public MLbsCallbackTimerObserver
	{
public:

	/** MT-LR specific states
	These are specific to MT-LR and identify the current state within the
	MT-LR protocol procedure.
	*/
	enum TMtLrState
		{
		/** Not valid */
		EStateNull,
		/** Network request received */
		EStateNetReqRecvd,

		/** LBS Privacy session started */
		EStatePrivacySessStarted,
		/** LBS Privacy response received - no measurement control yet */
		EStatePrivacyRespRecvd,
		/** LBS Privacy response received - after measurement control */
		EStatePrivacyRespRecvdAfterMeasure,
		/** LBS Privacy rejected - no measurement control yet */
		EStatePrivacyReject,
		/** LBS Privacy response received - after measurement control */
		EStatePrivacyRejectAfterMeasure,

		/** Measurement control received from the network */
		EStateMeasureDataRecvd,
		/** Network reference location sent to LBS */
		EStateNetBasedLocSent,
		/** Network location request received (and sent to LBS) */
		EStateLocReqByNet,
		/** Location response received from LBS */
		EStateLocRespRecvd,
		/** Location response sent to network */
		EStateLocSentToNet,


		/** LBS Client session about to close - after measure control */
		EStateClientSessToCloseAfterMeasure,
		/** LBS Client session about close to then wait for measure control */
		EStateClientSessToCloseWaitMeasure,
		/** LBS Client session about close to then wait for location response */
		EStateClientSessToCloseWaitLocResp,

		/** LBS Client and network sessions closed */
		EStateSessionsClosed,

		/** Protocol procedure is cancelling */
		EStateCancelling

		};

	/** Request identifiers used when a request is queued.
	*/
	enum TMtLrQueue
		{
		/** Privacy response received from LBS */
		EQueuePrivacyResponse	 = 100,
		/** Location response received from LBS */
		EQueueLocResponse		 = 200,
		/** Measurement control received from network */
		EQueueMeasurementControl = 300
		};

public:

	static CMtLrStateMachine* NewL(MStateMachineObserver& aObserver);
	virtual ~CMtLrStateMachine();
	
	CMtLrStateMachine::TMtLrState CurrentState();

	void StartMtLrReqTimer();
	void StartMeasureControlTimer();
	void StartLocRespTimer();

	TBool IsEmergency() const;

	TBool IsPrivacyResponseHandled() const;
	const TLbsExternalRequestInfo& PrivacyReqInfo() const;
	const CLbsNetworkProtocolBase::TLbsPrivacyResponse& PrivacyResponseValue() const;
	const TLbsNetPosRequestPrivacy& PrivacyReq() const;
	TBool IsRejectedWaitingMeasureControl() const;
	
	void MtLrReq(const TLbsExternalRequestInfo& aReqInfo,
					const TLbsNetPosRequestPrivacy& aPrivacy);
	void QueueMtLrReq(const TLbsExternalRequestInfo& aReqInfo,
					const TLbsNetPosRequestPrivacy& aPrivacy);
	void MtLrCancelInd(TInt aReason);
	void PrivacyResp(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse);

	// CStateMachineBase derived methods

	void MeasurementControlInd(const TPositionInfoBase& aPosInfo,
							const TLbsNetPosRequestQuality& aQuality,
							const TLbsNetPosRequestMethod& aPosMethod);
	void MeasurementControlErrorInd(TInt aReason);
	void NetworkErrorInd();

	// CStateMachineBase pure virtual methods
	void StartQueuedMachine();
	void PreStateTransition();
	void StateTransition();
	void PostStateTransition();
	void CancelProcedure();
	void DoQueuedRequest(TInt aRequest);

	void LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo);
	void MeasurementControlTimeout();

	// MLbsCallbackTimerObserver methods
	
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);
	
protected:

	CMtLrStateMachine(MStateMachineObserver& aObserver);
	
private:
	
	void ConstructL();
	void InitialiseProcedure();
	void ProcedureDone();
	void CompleteProcedure();

	void StorePrivacyReq(const TLbsExternalRequestInfo& aReqInfo, 
				const TLbsNetPosRequestPrivacy& aPrivacy);
	void StorePrivacyResp(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& iPrivacyResp);

protected:
	
	/** Current MT-LR state
	*/
	TMtLrState iCurrentState;
	
	/** Timer for Privacy response required from LBS
	*/
	CLbsCallbackTimer* iMtLrReqIssuedTimer;
	
	/** Timer for Measurement Control expected by LBS
	*/
	CLbsCallbackTimer* iMeasureControlTimer;
	
	/** Timer for Location response expected by network
	*/
	CLbsCallbackTimer* iLocRespTimer;

	/** Privacy requestor information
	*/
	TLbsExternalRequestInfo iPrivacyReqInfo;

	/** Privacy response
	*/
	CLbsNetworkProtocolBase::TLbsPrivacyResponse iPrivacyResp;

	/** Flag indicates privacy response received
	*/
	TBool iPrivacyResponseRecvd;

	/** Flag indicates privacy response handled
	*/
	TBool iPrivacyResponseHandled;
	
	/** Indicates if privacy response arrived before measurement control
	*/
	TBool iIsPrivacyRespBeforeMeasureControl;

	/** Privacy request information
	*/
	TLbsNetPosRequestPrivacy iPrivacyRequest;

	};

#endif // __CMTLRSTATEMACHINE_H__

